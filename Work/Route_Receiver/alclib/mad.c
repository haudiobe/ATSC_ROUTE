/** \file mad.c \brief General ALC stuff
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.27 $
 *
 *  MAD-ALCLIB: Implementation of ALC/LCT protocols, Compact No-Code FEC,
 *  Simple XOR FEC, Reed-Solomon FEC, and RLC Congestion Control protocol.
 *  Copyright (c) 2003-2007 TUT - Tampere University of Technology
 *  main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  In addition, as a special exception, TUT - Tampere University of Technology
 *  gives permission to link the code of this program with the OpenSSL library (or
 *  with modified versions of OpenSSL that use the same license as OpenSSL), and
 *  distribute linked combinations including the two. You must obey the GNU
 *  General Public License in all respects for all of the code used other than
 *  OpenSSL. If you modify this file, you may extend this exception to your version
 *  of the file, but you are not obligated to do so. If you do not wish to do so,
 *  delete this exception statement from your version.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/time.h>
#endif

#include "mad.h"

/* Initialize global variable */

BOOL lib_init = FALSE;

#ifdef _MSC_VER
double time_factor;				/**< local global variable for timer */
unsigned long long start_time;	/**< local global variable for timer */
#endif

#ifdef _MSC_VER
/**
 * This function initializes timer.
 */

void sec_init(void) {	
	unsigned long long perf_cnt;
	QueryPerformanceFrequency((LARGE_INTEGER*)&perf_cnt);
	time_factor = 1.0/perf_cnt;
	QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
}
#endif

void alc_init(void) {

#ifdef _MSC_VER
	sec_init(); /* Initialize timer */
#endif
	lib_init = TRUE;
}

#ifdef _MSC_VER
double sec(void) {	
	long long cur_time;
	double time_span;         
	QueryPerformanceCounter((LARGE_INTEGER*)&cur_time);

	time_span = (cur_time - start_time) * time_factor;
	return time_span;
}	
#else
double sec(void) {

	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}
#endif

int increase_ipv6_address(struct in6_addr *ipv6) { 
 
  if(ipv6->s6_addr[15] != 0xFF) { 
    ipv6->s6_addr[15]++; 
  } 
  else if(ipv6->s6_addr[14] != 0xFF) { 
    ipv6->s6_addr[15] = 0; 
    ipv6->s6_addr[14]++; 
  } 
  else if(ipv6->s6_addr[13] != 0xFF) { 
    ipv6->s6_addr[13]++; 
  } 
  else if(ipv6->s6_addr[12] != 0xFF) { 
    ipv6->s6_addr[13] = 0; 
    ipv6->s6_addr[12]++; 
  } 
  else if(ipv6->s6_addr[11] != 0xFF) { 
    ipv6->s6_addr[11]++; 
  } 
  else if(ipv6->s6_addr[10] != 0xFF) { 
    ipv6->s6_addr[11] = 0; 
    ipv6->s6_addr[10]++; 
  } 
  else if(ipv6->s6_addr[9] != 0xFF) { 
    ipv6->s6_addr[9]++; 
  } 
  else if(ipv6->s6_addr[8] != 0xFF) { 
    ipv6->s6_addr[9] = 0; 
    ipv6->s6_addr[8]++; 
  } 
  else if(ipv6->s6_addr[7] != 0xFF) { 
    ipv6->s6_addr[7]++; 
  } 
  else if(ipv6->s6_addr[6] != 0xFF) { 
    ipv6->s6_addr[7] = 0; 
    ipv6->s6_addr[6]++; 
  } 
  else if(ipv6->s6_addr[5] != 0xFF) { 
    ipv6->s6_addr[5]++; 
  } 
  else if(ipv6->s6_addr[4] != 0xFF) { 
    ipv6->s6_addr[5] = 0; 
    ipv6->s6_addr[4]++; 
  } 
  else if(ipv6->s6_addr[3] != 0xFF) { 
    ipv6->s6_addr[3]++; 
  } 
  else if(ipv6->s6_addr[2] != 0xFF) { 
    ipv6->s6_addr[3] = 0; 
    ipv6->s6_addr[2]++; 
  } 
  else if(ipv6->s6_addr[1] != 0xFF) { 
    ipv6->s6_addr[1]++; 
  } 
  else if(ipv6->s6_addr[0] != 0xFF) { 
    ipv6->s6_addr[1] = 0; 
    ipv6->s6_addr[0]++; 
  } 
  else { 
    return -1; 
  } 
   
  return 0; 
} 

int randomloss(double lossprob) {
	
	int loss = 0;

	double msb;
	double lsb;

	double tmp;

	if(lossprob == 0.0) {
		return loss;
	}
	
	msb = (double)(rand()%100);
	lsb = (double)(rand()%10);
	
	tmp = msb + (double)(lsb/(double)10);

	if(tmp < lossprob) {
		loss = 1;
	}
	
	return loss;
}


#ifdef _MSC_VER

lldiv_t lldiv(long long num, long long denom) {
  lldiv_t r;

  if (num > 0 && denom < 0) {
    num = -num;
    denom = -denom;
  }

  r.quot = num / denom;
  r.rem = num % denom;

  if (num < 0 && denom > 0) {
    if (r.rem > 0) {
      r.quot++;
      r.rem -= denom;
    }
  }
  
  return r;
}

#endif
