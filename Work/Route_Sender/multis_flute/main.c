/** \file /multis_flute/main.c \brief FLUTE multisession demo with two simultaneous session
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.14 $
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
#include <stdio.h>
#include <time.h>

#ifdef _MSC_VER
#include <windows.h>
#include <io.h>
#include <crtdbg.h>
#include <process.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#endif

#include "../flutelib/flute.h"

/**
 * Run time memory debug (0 = no, 1 = yes)
 */

#define _DEBUG_MEMORY 0

/**
 * Structure for the thread parameters.
 * @struct thread_params
 */

typedef struct thread_params {
  arguments_t a;
  int s_id;
  unsigned long long session_size;
} thread_params_t;


/**
 * This is a private function which receives and handles signals.
 *
 * @param sig must be, not used for anything
 *
 */

void signal_handler(int sig){

  printf("Exiting...\n");
  fflush(stdout);

  set_all_flute_sessions_state(SExiting);
}

/**
 * This function prints programs usage information.
 *
 */

void usage(void) {
  printf("\nFLUTE Version %s, %s\n\n", MAD_FCL_RELEASE_VERSION, MAD_FCL_RELEASE_DATE);
  printf("  Copyright (c) 2003-2007 TUT - Tampere University of Technology\n");
  printf("  main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi\n");
  printf("  web site: http://www.atm.tut.fi/mad/\n\n");
  printf("  This is free software, and you are welcome to redistribute it\n");
  printf("  under certain conditions; See the GNU General Public License\n");
  printf("  as published by the Free Software Foundation, version 2 or later,\n");
  printf("  for more details.\n\n");
  printf("  * mad_rlc.c & mad_rlc.h -- Portions of code derived from MCL library by\n");
  printf("  * Vincent Roca et al. (http://www.inrialpes.fr/planete/people/roca/mcl/)\n");
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
  printf("                    [-f:str] [-F:str,str,...] [-G] [-h] [-H] [-i:str] [-k:int] [-l:int]\n");
  printf("                    [-L:int] [-m:str] [-n:int] [-p:int] [-P:float,float]\n");
  printf("                    [-q:int] [-r:int]");
#ifdef USE_FILE_REPAIR
  printf(" [-R:str] [-S] [-t:ull] [-T:int] [-U]\n");
  printf("                    [-v:int] [-V:str] [-w:int] [-W] [-x:int] [-X:int] [-z:int]\n");
  printf("                    [-Z]\n\n");
#else
  printf(" [-S] [-t:ull] [-T:int] [-U] [-v:int]\n");
  printf("                    [-V:str] [-w:int] [-W] [-x:int] [-X:int] [-z:int] [-Z]\n\n");
#endif
  printf("Receiver usage: flute [-a:str] [-A] [-b:int] [-B:str] [-c:int] [-d:str] [-E]\n"); 
  printf("                      [-F:str,str,...] [-h] [-i:str]"); 
#ifdef _MSC_VER
  printf(" [-I:int]");
#else
  printf(" [-I:str]");
#endif
  printf(" [-m:str]"); 
#ifdef SSM
  printf(" [-M]\n");
#endif
  printf("                      [-N] [-o:ull]");
#ifdef _MSC_VER
  printf(" [-O]");
#endif
  printf(" [-p:int] [-P:float,float]");
#ifdef USE_FILE_REPAIR
  printf(" [-R:str]\n");
  printf("                      [-s:str] [-t:ull] [-U] [-v:int] [-V:str] [-w:int] [-W]\n\n");
#else
  printf(" [-s:str]\n");
  printf("                      [-t:ull] [-U] [-v:int] [-V:str] [-w:int] [-W]\n\n");
#endif
  printf("Common options:\n\n");
  printf("   -a:str           Address family [IPv4 or IPv6], default: IPv4\n");
  printf("   -d:str           SDP file (start/join FLUTE session based on SDP file),\n");
  printf("                    default: no\n");
  printf("   -h               Print this help\n");
  printf("   -i:str           Local interface to bind to, default: INADDR_ANY\n");
  printf("   -m:str           IPv4 or IPv6 address for base channel,\n");
  printf("                    default: %s or %s\n", DEF_MCAST_IPv4_ADDR, DEF_MCAST_IPv6_ADDR);
  printf("   -p:int           Port number for base channel, default: %s\n", DEF_MCAST_PORT);
  printf("   -P[:float,float] Simulate packet losses, default packet loss\n");
  printf("                    percentages: %.1f,%.1f\n", (float)P_LOSS_WHEN_OK, (float)P_LOSS_WHEN_LOSS);
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
  printf("                    -A option\n\n");
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
  printf("   -Z               Optimize transmission rate (use more CPU)\n\n");
  printf("Receiver options:\n\n");
  printf("   -A               Receive files automatically\n");
  printf("   -b:int           Receiver's memory consumption [0 = high, 1 = medium,\n");
  printf("                    2 = low], default: 1\n");
  printf("   -B:str           Base directory for downloaded files,\n");
  printf("                    default: %s\n", DEF_BASE_DIR);
  printf("   -c:int           Maximum number of channels, default: %i\n", DEF_NB_CHANNEL);
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
 * This function start FLUTE receiver or sender in thread.
 *
 * @param t_par thread parameter structure
 *
 */

void* start_flute_process(void *t_par) {

  thread_params_t *t_p;
  int retval = 0;

  t_p = (thread_params_t*)t_par;

  if(t_p->a.alc_a.mode == SENDER) {
	retval = flute_sender(&t_p->a, &t_p->s_id, &t_p->session_size);

	if(t_p->a.alc_a.verbosity > 0) {
#ifdef _MSC_VER
	  printf("Total bytes sent: %I64u\n", t_p->session_size);
#else	
	  printf("Total bytes sent: %llu\n", t_p->session_size);
#endif	
	}
  }
  else if(t_p->a.alc_a.mode == RECEIVER) {
	retval = flute_receiver(&t_p->a, &t_p->s_id);
  }

#ifdef _MSC_VER
	_endthread();
#else	
	pthread_exit(0);
#endif

  return NULL;
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

  thread_params_t par1;
  thread_params_t par2;

  /* Handles for two session */
#ifdef _MSC_VER
  HANDLE handle_thread1;
  unsigned int thread_id1;
  HANDLE handle_thread2;
  unsigned int thread_id2;
  int addr_size;
#else
  pthread_t thread_id1;
  int join_retval1;
  pthread_t thread_id2;
  int join_retval2;
#endif

  char tmp_port[MAX_PORT_LENGTH];
  char tmp_addr[INET6_ADDRSTRLEN];
  struct sockaddr_in ipv4;
  struct sockaddr_in6 ipv6;
  int j;

#if defined(_MSC_VER) && _DEBUG_MEMORY > 0
  _CrtSetDbgFlag(
        _CRTDBG_ALLOC_MEM_DF |
        _CRTDBG_CHECK_ALWAYS_DF |
        _CRTDBG_CHECK_CRT_DF |
        _CRTDBG_DELAY_FREE_MEM_DF |
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


  /* Command line argumants are parsed twise, own argument
     structure for both session */

  retval = parse_args(argc, argv, &par1.a);

  if(retval == -1) {
    usage();
  }

  retval = parse_args(argc, argv, &par2.a);

  if(retval == -1) {
    usage();
  }

  /* Essential parameters are changed */

  par2.a.alc_a.tsi = par1.a.alc_a.tsi + 1;
  
  memset(tmp_port, 0, MAX_PORT_LENGTH);
  sprintf(tmp_port, "%i", (atoi(par1.a.alc_a.port) + 1000));
  par2.a.alc_a.port = tmp_port;

  if(par1.a.alc_a.addr_family == PF_INET) {
	memset(tmp_addr, 0, INET6_ADDRSTRLEN);
	ipv4.sin_addr.s_addr = htonl(ntohl(inet_addr(par1.a.alc_a.addr)) + 10);
	sprintf(tmp_addr, "%s", inet_ntoa(ipv4.sin_addr));
  }
  else if(par1.a.alc_a.addr_family == PF_INET6) {
#ifdef _MSC_VER
    addr_size = sizeof(struct sockaddr_in6);
	WSAStringToAddress((char*)par1.a.alc_a.addr, AF_INET6, NULL, (struct sockaddr*)&ipv6, &addr_size);
#else 
	inet_pton(AF_INET6, par1.a.alc_a.addr, &ipv6.sin6_addr);
#endif
	
	for(j = 0; j < 10; j++) {
		if(increase_ipv6_address(&ipv6.sin6_addr) == -1) {
			printf("Increasing IPv6 address is not possible\n");
			return -1;
		}
	}

	memset(tmp_addr, 0, INET6_ADDRSTRLEN);

#ifdef _MSC_VER
	addr_size = sizeof(tmp_addr);
	WSAAddressToString((struct sockaddr*)&ipv6, sizeof(struct sockaddr_in6),
					NULL, tmp_addr, &addr_size);
#else
	inet_ntop(AF_INET6, &ipv6.sin6_addr, tmp_addr, sizeof(tmp_addr));
#endif
  }

  par2.a.alc_a.addr = tmp_addr;
  
  par1.s_id = -1;
  par2.s_id = -1;

#ifdef _MSC_VER
    handle_thread1 =
      (HANDLE)_beginthreadex(NULL, 0, (void*)start_flute_process,
			 (void*)&par1, 0, &thread_id1);
    
    if(handle_thread1 == NULL) {
      perror("main.c: _beginthread 1");
      return -1;
    }
#else
    if(pthread_create(&thread_id1, NULL, start_flute_process, (void*)&par1) != 0) {
      perror("main.c: pthread_create 1");
      return -1;
    }
#endif

#ifdef _MSC_VER
    handle_thread2 =
      (HANDLE)_beginthreadex(NULL, 0, (void*)start_flute_process,
			     (void*)&par2, 0, &thread_id2);
    
    if(handle_thread2 == NULL) {
      perror("main.c: _beginthread 2");
      return -1;
    }
#else
    if(pthread_create(&thread_id2, NULL, start_flute_process, (void*)&par2) != 0) {
      perror("main.c: pthread_create 2");
      return -1;
    }
#endif

#ifdef _MSC_VER
    WaitForSingleObject(handle_thread1, INFINITE);
    CloseHandle(handle_thread1);
#else
    join_retval1 = pthread_join(thread_id1, NULL);
    assert(join_retval1 == 0);
    pthread_detach(thread_id1);
#endif

#ifdef _MSC_VER
    WaitForSingleObject(handle_thread2, INFINITE);
    CloseHandle(handle_thread2);
#else
    join_retval2 = pthread_join(thread_id2, NULL);
    assert(join_retval2 == 0);
    pthread_detach(thread_id2);
#endif

  free_args(&par1.a);
  free_args(&par2.a);
  shut_down_flute2();
  
  return retval;
}
