/** \file /flute/main.c \brief FLUTE command line application
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.192 $
 *
 *  MAD-FLUTE: FLUTE application.
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
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>

#ifdef _MSC_VER
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#include <io.h>
#include <crtdbg.h>
#else
#include <unistd.h>
#endif

#include "../flutelib/flute.h"

/**
 * Run time memory debug (0 = no, 1 = yes)
 */

#define _DEBUG_MEMORY 0

int s_id; /**< session identifier */

//Malek El Khatib 14.05.2014
//Start
//This is defined in main;
extern char* logFile;
extern	FILE* logFilePtr;
//END

/**
 * This is a private function which receives and handles signals.
 *
 * @param sig must be, not used for anything
 *
 */

void signal_handler(int sig){

  printf("Exiting...\n");
  fflush(stdout);

#ifdef USE_FILE_REPAIR
  set_all_flute_sessions_state(SExiting);
#else
  set_flute_session_state(s_id, SExiting);
#endif
}

/**
 * This function prints programs usage information.
 *
 */
//Malek El Khatib 16.04.2014
//START
//I have added information regarding the use of -y: and -Y to add video and audio segment durations
//END
void usage(void) {
  printf("\nFLUTE Version %s, %s\n\n", MAD_FCL_RELEASE_VERSION, MAD_FCL_RELEASE_DATE);
  printf("  Copyright (c) 2003-2007 TUT - Tampere University of Technology\n");
  printf("  main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi\n");
  printf("  web site: http://www.atm.tut.fi/mad/\n\n");
  printf("  This is free software, and you are welcome to redistribute it\n");
  printf("  under certain conditions; See the GNU General Public License\n");
  printf("  as published by the Free Software Foundation, version 2 or later,\n");
  printf("  for more details.\n\n");
  printf("  * mad_rlc.c & mad_rlc.h -- Portions of code derived from MCLv3 library by\n");
  printf("  * Vincent Roca et al. (http://planete-bcast.inrialpes.fr/)\n");
  printf("  *\n");
  printf("  * Copyright (c) 1999-2004 INRIA - Universite Paris 6 - All rights reserved\n");
  printf("  * (main author: Julien Laboure - julien.laboure@inrialpes.fr\n");
  printf("  *               Vincent Roca - vincent.roca@inrialpes.fr)\n\n");
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
  printf("  * OF SUCH DAMAGE.\n\n");
  printf("Sender usage: flute [-a:str] [-B:str] [-c:int] [-C] [-d:str] [-D:ull] [-e:int]\n"); 
  printf("                    [-f:str] [-F:str,str,...] [-G] [-h] [-H] [-i:str] [-J:str] [-k:int]\n");
  printf("                    [-l:int] [-L:int] [-m:str] [-n:int] [-p:int] [-P:float,float]\n");
  printf("                    [-q:int] [-Q] [-r:int]");
#ifdef USE_FILE_REPAIR
  printf(" [-R:str] [-S] [-t:ull] [-T:int] [-U]\n");
  printf("                    [-v:int] [-V:str] [-w:int] [-W] [-x:int] [-X:int] [-y:str] [-Y:ul] [-z:int]\n");
  printf("                    [-Z]\n\n");
#else
  printf(" [-S] [-t:ull] [-T:int] [-U] [-v:int]\n");
  printf("                    [-V:str] [-w:int] [-W] [-x:int] [-X:int] [-y:str] [-Y:us] [-z:int] [-Z]\n\n");
#endif
  printf("Receiver usage: flute [-a:str] [-A] [-b:int] [-B:str] [-c:int] [-d:str] [-D:ull]\n"); 
  printf("                      [-E] [-F:str,str,...] [-h] [-i:str]"); 
#ifdef _MSC_VER
  printf(" [-I:int]");
#else
  printf(" [-I:str]");
#endif
  printf(" [-J:str]"); 
  printf(" [-m:str]"); 
#ifdef SSM
  printf(" [-M]\n");
#endif
  printf("                      [-N] [-o:ull]");
#ifdef _MSC_VER
  printf(" [-O]");
#endif
  printf(" [-p:int] [-P:float,float] [-Q]");
#ifdef USE_FILE_REPAIR
  printf(" [-R:str]\n");
  printf("                      [-s:str] [-t:ull] [-U] [-v:int] [-V:str] [-w:int] [-W] [-Y:ul]\n\n");
#else
  printf(" [-s:str]\n");
  printf("                      [-t:ull] [-U] [-v:int] [-V:str] [-w:int] [-W] [-Y:us]\n\n");
#endif
  printf("Common options:\n\n");
  printf("   -a:str           Address family [IPv4 or IPv6], default: IPv4\n");
  printf("   -d:str           SDP file (start/join FLUTE session based on SDP file),\n");
  printf("                    default: no\n");
  printf("   -h               Print this help\n");
  printf("   -i:str           Local interface to bind to, default: INADDR_ANY\n");
  printf("   -J:str           Log file containing data to be used for delay calculation\n");
  printf("   -m:str           IPv4 or IPv6 address for base channel,\n");
  printf("                    default: %s or %s\n", DEF_MCAST_IPv4_ADDR, DEF_MCAST_IPv6_ADDR);
  printf("   -p:int           Port number for base channel, default: %s\n", DEF_MCAST_PORT);
  printf("   -P[:float,float] Simulate packet losses, default packet loss\n");
  printf("                    percentages: %.1f,%.1f\n", (float)P_LOSS_WHEN_OK, (float)P_LOSS_WHEN_LOSS);
  printf("   -Q               Send/Receive FDT after object itself. SAME VALUE SHOULD BE SELECTED FOR FLUTE SENDER AND RECEIVER\n");
  printf("   -S               Act as sender, send data; otherwise receive data\n");
  printf("   -t:ull           TSI for the session, default: %i\n", DEF_TSI);
  printf("   -U               Address type is unicast, default: multicast\n");
  printf("   -v:int           Log verbosity level [0 = No output, 1 = File level,\n");
  printf("                    2 = 1 + percent, 3 = 2 + blocks, 4 = 3 + FDT], default: 1\n");
  printf("   -V:str           Print logs to 'str' file, default: print to stdout\n");  
  printf("   -w:int           Congestion control scheme [0 = Null, 1 = RLC],\n");
  printf("                    default: %i; the number of channels, defined by -c option,\n", DEF_CC);
  printf("                    are used with both schemes and bitrate of each channel is\n");
  printf("                    set according to RLC rules\n");
  printf("   -W               File table output mode. In receiver this work only with\n");
  printf("                    -A option\n");
  printf("   -Y:us            For Sender: The number of encoding symbols to send per packet.\n");
  printf("                    If zero, sender uses input file [-y:] which indicates chunks of bytes of a segment and corresponding absolute time in which to send them.\n");
  printf("                    For Receiver: If zero,receiver expects multiple es per packet. Otherwise, it is ignored.\n\n");
  printf("Sender options:\n\n");
  printf("   -B:str           Base directory for files or directories to be sent,\n");
  printf("                    default: working directory\n");
  printf("   -c:int           Number of used channels, default: %i\n", DEF_NB_CHANNEL);
  printf("   -C               Continuous transmission, default: not used\n");
  printf("   -D:ull           Duration of the session in seconds, default: %i\n", DEF_SESSION_DURATION);
  printf("   -e:int           FEC-OTI for file objects\n");
  printf("                    [0 = FEC-OTI in FDT, 1 = FEC-OTI in EXT_FTI], default: 0\n");
  printf("   -f:str           FDT file (send based on FDT), default: %s\n", DEF_FDT);
  printf("   -F:str,str,...   Files or directories to be sent. Given base directory is\n");
  printf("                    same for all entries.\n");
  printf("   -G               Calculate session size, but do not send anything\n");
  printf("   -H               Use Half-word (when used, TSI field can be 16, 32 or 48\n");
  printf("                    bits long and TOI field can be 16, 32, 48 or 64 bits\n");
  printf("                    long), default: not used\n");
  printf("   -k:int           Send a Complete FDT in the beginning of the session\n");
  printf("                    [0 = No, 1 = Yes, 2 = Yes, only Complete FDT], default: 0\n");
  printf("   -l:int           Encoding symbol length in bytes, default: %i\n", DEF_SYMB_LENGTH);
  printf("   -L:int           Maximum source block length in multiple of encoding\n");
  printf("                    symbols, default: %i\n", DEF_MAX_SB_LEN);
  printf("   -n:int           Number of transmissions, default: %i\n", DEF_TX_NB);
  printf("   -q:int           Send session close packets [0 = No, 1 = Yes], default: 1\n");
  printf("   -r:int           Transmission rate at base channel in kbits/s, default: %i\n", DEF_TX_RATE);
#ifdef USE_FILE_REPAIR
  printf("   -R[:str]         Save configuration to a file, default: %s\n", DEF_FLUTE_CONF_FILE);
#endif
  printf("   -T:int           Time To Live or Hop Limit for the session, default: %i\n", DEF_TTL);
  printf("   -x:int           FEC Encoding [0 = Null, 1 = Simple XOR, 2 = Reed-Solomon\n");
  printf("                    (old I-D) 3 = Reed-Solomon (new I-D), default: %i\n", DEF_FEC);
  printf("   -X:int           FEC ratio percent, default: %i\n", DEF_FEC_RATIO);
#ifdef USE_ZLIB
  printf("   -z:int           Encode content [0 = no, 1 = ZLIB FDT,\n");
  printf("                    2 = ZLIB FDT and GZIP files,\n");
  printf("                    3 = PAD files], default: 0\n");
#else
  printf("   -z:int           Encode content [0 = no, 3 = PAD files],\n");
  printf("                    default: 0\n");
#endif
  printf("   -y:str           Input file indicating when to send each segment (i.e. Object) or chunk of segment. Times should be absolute unix times\n\n");
  printf("   -Z               Optimize transmission rate (use more CPU)\n\n");
  printf("Receiver options:\n\n");
  printf("   -A               Receive files automatically\n");
  printf("   -b:int           Receiver's memory consumption [0 = high, 1 = medium,\n");
  printf("                    2 = low], default: 1\n");
  printf("   -B:str           Base directory for downloaded files,\n");
  printf("                    default: %s\n", DEF_BASE_DIR);
  printf("   -c:int           Maximum number of channels, default: %i\n", DEF_NB_CHANNEL);
  printf("   -D:ull           Reveiver will quit after this time (in seconds) if no\n");
  printf("                    packets is received from the session, default: %i\n", DEF_RECEIVER_TIMEOUT);
  printf("   -E               Accept expired FDT Instances\n");   
  printf("   -F:str,str,...   Files to be received\n");
#ifdef _MSC_VER
  printf("   -I:int           Local interface index for IPv6 multicast join, use for\n");
  printf("                    example 'ipv6 if' command to see interface indexes;\n");
  printf("                    otherwise OS default\n");
#else
  printf("   -I:str           Local interface name for IPv6 multicast join, use for\n");
  printf("                    example 'ifconfig' command to see interface names;\n");
  printf("                    otherwise OS default\n");
#endif
#ifdef SSM
  printf("   -M               Use Source-Specific Multicast, default: no\n");
#endif
  printf("   -N               Name incomplete objects after the session has ended\n");
  printf("   -o:ull           TOI for the object to be received\n");
#ifdef _MSC_VER
  printf("   -O               Open received file(s) automatically, default: no\n");
#endif
#ifdef USE_FILE_REPAIR
  printf("   -R[:str]         Use HTTP file repair with given config file, default:\n");
  printf("                    %s\n", DEF_APD_CONF_FILE);
#endif
  printf("   -s:str           Source IPv4 or IPv6 address of this session. If not set,\n");
  printf("                    receiver locks to the first IP address found from the\n");
  printf("                    FLUTE packets.\n"); 
  printf("\nExample use cases:\n\n");
  printf("1. Send a file or directory n times\n\n\tflute -S -m:224.1.1.1 -p:4000 -t:2 -r:100 -F:files/flute-draft.txt\n");
  printf("\t      -n:2\n\n");
  printf("2. Send a file or directory in a loop\n\n\tflute -S -m:224.1.1.1 -p:4000 -t:2 -r:100 -F:files/flute-draft.txt\n");
  printf("\t      -C\n\n");
  printf("3. Send files defined in an FDT file\n\n\tflute -S -m:224.1.1.1 -p:4000 -t:2 -r:100 -f:fdt2.xml\n\n");
  printf("4. Send files defined in an FDT file in a loop\n\n\tflute -S -m:224.1.1.1 -p:4000 -t:2 -r:100 -f:fdt2.xml -C\n\n");
  printf("5. Send using unicast\n\n\tflute -S -U -m:1.2.3.4 -p:4000 -t:2 -r:100 -f:fdt2.xml -C\n\n");
  printf("6. Receive one object\n\n\tflute -m:224.1.1.1 -p:4000 -t:2 -s:2.2.2.2 -o:1\n\n");
  printf("7. Receive file(s) defined by file name(s)\n\n\tflute -m:224.1.1.1 -p:4000 -t:2 -s:2.2.2.2\n");
  printf("\t      -F:files/flute-man.txt,flute-draft.txt\n\n");
  printf("8. Receive file(s) defined by wild card option\n\n\tflute -m:224.1.1.1 -p:4000 -t:2 -s:2.2.2.2 -F:*.jpg\n\n");
  printf("9. Receive file(s) with User Interface\n\n\tflute -m:224.1.1.1 -p:4000 -t:2 -s:2.2.2.2\n\n");
  printf("10. Receive file(s) automatically from session\n\n\tflute -A -m:224.1.1.1 -p:4000 -t:2 -s:2.2.2.2\n\n");
  printf("11. Receive using unicast\n\n\tflute -A -U -p:4000 -t:2 -s:2.2.2.2\n\n");
  
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
  unsigned long long session_size;

  /*flute_receiver_report_t *report;*/

#if defined(_MSC_VER) && _DEBUG_MEMORY > 0
  _CrtSetDbgFlag(
        _CRTDBG_ALLOC_MEM_DF |
        _CRTDBG_CHECK_ALWAYS_DF |
        _CRTDBG_CHECK_CRT_DF |
        /*_CRTDBG_DELAY_FREE_MEM_DF |*/
        _CRTDBG_LEAK_CHECK_DF);
#endif
  
  if(start_up_flute() != 0) {
    return -1;
  }

  /* Set signal handler */
  
  signal(SIGINT, signal_handler);
#ifdef _MSC_VER
  signal(SIGBREAK, signal_handler);
#endif

  srand((unsigned)time(NULL));

  retval = parse_args(argc, argv, &a);
  
  if(retval == -1) {
    usage();
  }

  //Malek El Khatib 15.05.2014
  //Start
  //Opening log file for writing
  printf("Opening Log File\n");
  logFilePtr=fopen(logFile, "w");
  fprintf(logFilePtr,"************************************************************************************************************************************************************\n");
  fprintf(logFilePtr,"Logger assumes that half word is not used, TOI field is 32 bits, FDT is NOT sent completely at beginning of session and that only one sender channel is used\n");
  fprintf(logFilePtr,"************************************************************************************************************************************************************\n");
  //End

  s_id = -1;
  
  if(a.alc_a.mode == SENDER) {
    
    if(a.alc_a.calculate_session_size == TRUE) {
      
      session_size = flute_session_size(&a, &s_id);

      if(a.alc_a.verbosity > 0) {
#ifdef _MSC_VER
  printf("Session size in bytes (including UDP/IP headers): %I64u\n", session_size);
#else
  printf("Session size in bytes (including UDP/IP headers): %llu\n", session_size);
#endif
      }
    }
    else {
      retval = flute_sender(&a, &s_id, &session_size);

      if(a.alc_a.verbosity > 0) {
#ifdef _MSC_VER
  printf("Total bytes sent: %I64u\n", session_size);
#else
  printf("Total bytes sent: %llu\n", session_size);
#endif
      }
    }
  }
  else if(a.alc_a.mode == RECEIVER) {
    
    retval = flute_receiver(&a, &s_id);
    
    /*
    retval = flute_receiver_report(&a, &s_id, &report);
    free_receiver_report(report);
    */
  }
  
  //Malek El Khatib 15.05.2014
  //Start
  //Closing log file
  printf("Closing Log File\n");
  fclose(logFilePtr);
  //End

  shut_down_flute(&a);

  return retval;
}
