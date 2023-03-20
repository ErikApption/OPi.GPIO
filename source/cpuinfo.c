/*
Copyright (c) 2012-2019 Ben Croston

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* See the following for up to date information:
 * https://www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "cpuinfo.h"

int get_opi_info(rpi_info *info)
{
   FILE *fp;
   char buffer[1024];
   char hardware[1024];
   int found = 0;
   int len;

   if ((fp = fopen("/proc/device-tree/system/linux,revision", "r"))) {
      uint32_t n;
      if (fread(&n, sizeof(n), 1, fp) != 1) {
         fclose(fp);
         return -1;
      }
      sprintf(revision, "%x", ntohl(n));
      found = 1;
   }
   else if ((fp = fopen("/proc/cpuinfo", "r"))) {
      while(!feof(fp) && fgets(buffer, sizeof(buffer), fp)) {
         sscanf(buffer, "CPU part        : %s", hardware);
         if (strcmp(hardware, "0xd0b") == 0 || //orange pi 5
             strcmp(hardware, "BCM2709") == 0 ||
             strcmp(hardware, "BCM2835") == 0 ||
             strcmp(hardware, "BCM2836") == 0 ||
             strcmp(hardware, "BCM2837") == 0 ) {
            found = 1;
         }
      }
   }
   else
      return -1;
   fclose(fp);

   if (!found)
      return -1;

   info->type = "Orange Pi 5";
   info->processor = "RK3588";
   info->manufacturer = "Rockchip";   
   info->type = "N/A";
   info->p1_revision = 1;
   info->ram = "4G";
   info->manufacturer = "Orange Pi";
   info->revision = "Unknown";
   return 0;
}

/*

32 bits
NEW                   23: will be 1 for the new scheme, 0 for the old scheme
MEMSIZE             20: 0=256M 1=512M 2=1G
MANUFACTURER  16: 0=SONY 1=EGOMAN
PROCESSOR         12: 0=2835 1=2836
TYPE                   04: 0=MODELA 1=MODELB 2=MODELA+ 3=MODELB+ 4=Pi2 MODEL B 5=ALPHA 6=CM
REV                     00: 0=REV0 1=REV1 2=REV2

pi2 = 1<<23 | 2<<20 | 1<<12 | 4<<4 = 0xa01040

--------------------

SRRR MMMM PPPP TTTT TTTT VVVV

S scheme (0=old, 1=new)
R RAM (0=256, 1=512, 2=1024)
M manufacturer (0='SONY',1='EGOMAN',2='EMBEST',3='UNKNOWN',4='EMBEST')
P processor (0=2835, 1=2836 2=2837)
T type (0='A', 1='B', 2='A+', 3='B+', 4='Pi 2 B', 5='Alpha', 6='Compute Module')
V revision (0-15)

*/
