/** \file /repair_sender/main.c \brief Repair sender command line application
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.37 $
 *
 *  MAD-REPAIR-SENDER: Implementation of simple point-to-multipoint repair for FLUTE protocol.
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

#include <signal.h>
#include <stdio.h>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "../flutelib/flute.h"
#include "repair_sender.h"

#define MAD_REPAIR_SENDER_VERSION "0.1" /**< Release version */

int s_id; /**< session identifier */

/**
 * This is a private function which receives and handles signals.
 *
 * @param sig must be, not used for anything
 *
 */

void signal_handler(int sig){
  printf("\nExiting...\n\n");
  fflush(stdout);
  set_flute_session_state(s_id, SExiting);
}

/**
 * This function prints programs usage information.
 *
 */

void usage(void) {
  printf("\nRepair Sender Version %s, %s\n\n", MAD_REPAIR_SENDER_VERSION, MAD_FCL_RELEASE_DATE);
  printf("  Copyright (c) 2003-2007 TUT - Tampere University of Technology\n");
  printf("  main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi\n");
  printf("  web site: http://www.atm.tut.fi/mad\n\n");
  printf("  This is free software, and you are welcome to redistribute it\n");
  printf("  under certain conditions; See the GNU General Public License\n");
  printf("  as published by the Free Software Foundation, version 2 or later,\n");
  printf("  for more details.\n");  
  printf("  * mad_rlc.c & mad_rlc.h -- Portions of code derived from MCL library by\n");
  printf("  * Vincent Roca et al. (http://www.inrialpes.fr/planete/people/roca/mcl/)\n");
  printf("  *\n");
  printf("  * Copyright (c) 1999-2004 INRIA - Universite Paris 6 - All rights reserved\n");
  printf("  * (main author: Julien Laboure - julien.laboure@inrialpes.fr\n");
  printf("  *               Vincent Roca - vincent.roca@inrialpes.fr)\n");
  printf("  * fec.c & fec.h -- forward error correction based on Vandermonde matrices\n");
  printf("  * 980624\n");
  printf("  * (C) 1997-98 Luigi Rizzo (luigi@iet.unipi.it)\n");
  printf("  *\n");
  printf("  * Portions derived from code by Phil Karn (karn@ka9q.ampr.org),\n");
  printf("  * Robert Morelos-Zaragoza (robert@spectra.eng.hawaii.edu) and\n");
  printf("  * Hari Thirumoorthy (harit@spectra.eng.hawaii.edu), Aug 1995\n");
  printf("  *\n");
  printf("  * Redistribution and use in source and binary forms, with or without\n");
  printf("  * modification, are permitted provided that the following conditions\n");
  printf("  * are met:\n");
  printf("  *\n");
  printf("  * 1. Redistributions of source code must retain the above copyright\n");
  printf("  *    notice, this list of conditions and the following disclaimer.\n");
  printf("  * 2. Redistributions in binary form must reproduce the above\n");
  printf("  *    copyright notice, this list of conditions and the following\n");
  printf("  *    disclaimer in the documentation and/or other materials\n");
  printf("  *    provided with the distribution.\n");
  printf("  *\n");
  printf("  * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND\n");
  printf("  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,\n");
  printf("  * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A\n");
  printf("  * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS\n");
  printf("  * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,\n");
  printf("  * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n");
  printf("  * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,\n");
  printf("  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n");
  printf("  * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR\n");
  printf("  * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT\n");
  printf("  * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY\n");
  printf("  * OF SUCH DAMAGE.\n");
  printf("\nUsage: repair_sender [-m:str] [-p:int] [-i:str] [-c:int] [-t:ull] [-e:int]\n"); 
  printf("                     [-K:str] [-a:str] [-h] [[-n:int] | [-C]] [-f:str] [-r:int]\n");
  printf("                     [-v:int] [-D:ull] [-V:str] [-T:int] [-P:float,float]\n");
  printf("                     [-H] [-B:str]");
  printf(" [-d:str]");
  printf(" [-w:int]");
  printf("\n\n");
  printf("Options:\n\n");
  printf("   -m:str           IP4 or IP6 address for base channel,\n");
  printf("   -e:int           FEC-OTI for file objects\n");
  printf("                    [0 = FEC-OTI in FDT, 1 = FEC-OTI in EXT_FTI], default: 0\n");
  printf("                    default: %s or %s\n", DEF_MCAST_IPv4_ADDR, DEF_MCAST_IPv6_ADDR);
  printf("   -p:int           Port number for base channel, default: %s\n", DEF_MCAST_PORT);
  printf("   -i:str           Local interface to bind to, default: INADDR_ANY\n");
  printf("   -t:ull           TSI for this session, default: %i\n", DEF_TSI);
  printf("   -w:int           Congestion control scheme [0 = Null, 1 = RLC],\n");
  printf("                    default: %i; the number of channels, defined by -c option,\n", DEF_CC);
  printf("                    are used with both schemes and bitrate of each channel is\n");
  printf("                    set according to RLC rules\n");
  printf("   -a:str           Address family (IP4 or IP6), default: IP4\n");
  printf("   -v:int           Verbosity level, [0 = low, 1 = high] default: 1\n");
  printf("   -V:str           Print logs to 'str' file, default: print to stdout\n");
  printf("   -d:str           SDP file (start FLUTE session based on SDP file),\n");
  printf("                    default: no\n");
  printf("   -h               Print this help\n");
  printf("   -c:int           Number of used channels, default: %i\n", DEF_NB_CHANNEL);
  printf("   -D:ull           Duration of the session in seconds, default: %i\n", DEF_SESSION_DURATION);
  printf("   -f:str           FDT file of the FLUTE session to be repaired, default: no\n");
  printf("   -B:str           Base directory for files of the FLUTE session to be\n");
  printf("                    repaired, default: no\n");
  printf("   -K:str           Repair configuration file, default: %s\n", DEF_REPAIR_SENDER_CONF_FILE);
  printf("   -r:int           Transmission rate at base channel in kbits/s, default: %i\n", DEF_TX_RATE);
  printf("   -T:int           Time To Live or Hop Limit for this session, default: %i\n", DEF_TTL);
  printf("   -e:int           Use EXT_FTI extensions header for file objects\n");
  printf("                    [0 = no, 1 = yes], default: 1\n");
  printf("   -n:int           Number of transmissions, default: %i\n", DEF_TX_NB);
  printf("   -C               Continuous transmission, default: not used\n");
  printf("   -P[:float,float] Simulate packet losses, default: %.1f,%.1f\n", (float)P_LOSS_WHEN_OK,
	 (float)P_LOSS_WHEN_LOSS);
  printf("   -H               Use Half-word (if used TSI field could be 16, 32 or 48\n");
  printf("                    bits long and TOI field could be 16, 32, 48 or 64 bits\n");
  printf("                    long), default: not used\n");
  exit(1);
}

/**
 * This function is programs main function.
 *
 * @param argc number of command line arguments
 * @param argv pointer to command line arguments
 *
 * @return different values (0, -1, -2, -3) depending on how program ends
 *
 */

int main(int argc, char **argv) {

  int retval = 0;
  arguments_t a;
    
  /* Set signal handler */
  
  signal(SIGINT, signal_handler);
#ifdef _MSC_VER
  signal(SIGBREAK, signal_handler);
#endif
  
  if(start_up_flute() != 0) {
	return -1;
  }

  retval = parse_args(argc, argv, &a);
  
  if(retval == -1) {
    usage();
  }
  
  s_id = -1; 
		
  retval = repair_sender(&a, &s_id);
  
  if(s_id == -1) {    
	shut_down_flute(&a);
    	return retval;
  }
  
  if(retval == -4) { /* bind failed */
	shut_down_flute(&a);
	return retval;	
  }

  shut_down_flute(&a);
  return retval;
}
