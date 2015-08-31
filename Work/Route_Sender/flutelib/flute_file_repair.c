/** \file flute_file_repair.c \brief Point-to-multipoint file repair
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.26 $
 *
 *  MAD-FLUTELIB: Implementation of FLUTE protocol.
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
#include <assert.h>
#include <time.h>

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif

#include "flute_file_repair.h"

#include "../alclib/alc_session.h"
#include "receiver.h"
#include "parse_args.h"
#include "fdt.h"

#ifdef USE_FILE_REPAIR

int flute_file_repair(flute_receiver_t *receiver, arguments_t *a, char *sdp_buf) {
  
  unsigned short i;
  int retval = 0;
  
  char addrs[MAX_CHANNELS_IN_SESSION][INET6_ADDRSTRLEN];	/* Mcast addresses */
  char ports[MAX_CHANNELS_IN_SESSION][MAX_PORT_LENGTH];	/* Local port numbers  */
  
  alc_session_t *old_session;
  alc_session_t *new_session;
  int new_s_id; /* new session identifier */

#ifdef _MSC_VER
  HANDLE handle_fdt_thread;
  unsigned int fdt_thread_id;
#else
  pthread_t fdt_thread_id;
  int join_retval;
#endif

  time_t systime;
  BOOL is_printed = FALSE;
  
  unsigned long long curr_time;
  
  if(parse_sdp_file(a, addrs, ports, sdp_buf) == -1) {
	return -1;
  }
  
  if(a->alc_a.nb_channel == 0) {
	printf("Error: No acceptable channels found in SDP.");
	fflush(stdout);
	return -1;
  }
  
  if(a->alc_a.stop_time != 0) {
	time(&systime);
	curr_time = systime + 2208988800U;
	
	if(a->alc_a.stop_time <= curr_time) {
	  printf("Session end time reached\n");
	  fflush(stdout);
	  return -1;
	}
  }
  
  new_s_id = open_alc_session(&a->alc_a);
  
  /* let's get the old flute session */
  old_session = get_alc_session(receiver->s_id);
  
  /* let's copy needed information from the old session to the new one */
  
  new_session = get_alc_session(new_s_id);

  new_session->obj_list = get_session_obj_list(receiver->s_id);
  new_session->fdt_list = get_session_fdt_list(receiver->s_id);
  new_session->wanted_obj_list = get_session_wanted_obj_list(receiver->s_id);

  /* change s_id to new_s_id */
  receiver->s_id = new_s_id;
  
  old_session->obj_list = NULL;
  old_session->fdt_list = NULL;
  old_session->wanted_obj_list = NULL;
  
  if(a->alc_a.start_time != 0) {
	while(1) {
	  
	  time(&systime);
	  curr_time = systime + 2208988800U;
	  
	  if((a->alc_a.start_time - 3) > curr_time) {
	
	if(!is_printed) {
	  printf("Waiting for session start time...\n");
	  fflush(stdout);
	  is_printed = TRUE;
	}
#ifdef _MSC_VER
	Sleep(1000);
#else
	sleep(1);
#endif
	  }
	  else {
	break;
	  }
	  
	  if(get_session_state(new_s_id) == SExiting) {
		return -5;
	  }
	}
  }
  
  if(a->alc_a.cc_id == Null) {
	
	for(i = 0; (int)i < a->alc_a.nb_channel; i++) {
	  
	  if(a->alc_a.addr_type == 1) {
	retval = add_alc_channel(new_s_id, ports[i], addrs[0], a->alc_a.intface, a->alc_a.intface_name);
	  }
	  else {
	retval = add_alc_channel(new_s_id, ports[i], addrs[i], a->alc_a.intface, a->alc_a.intface_name);
	  }
	  
	  if(retval == -1) {
	close_alc_session(new_s_id);
	return -1;
	  }
	}
  }
  else if(a->alc_a.cc_id == RLC) {
	
	retval = add_alc_channel(new_s_id, ports[0], addrs[0], a->alc_a.intface, a->alc_a.intface_name);
	
	if(retval == -1) {
	  close_alc_session(new_s_id);
	  return -1;	
	}
  }

  /* Create FDT receiving thread */
  
#ifdef _MSC_VER
  handle_fdt_thread =
	(HANDLE)_beginthreadex(NULL, 0,
			   (void*)fdt_thread, (void*)receiver, 0, &fdt_thread_id);
  if(handle_fdt_thread==NULL) {
	printf("Error: flute_file_repair, _beginthread\n");
	fflush(stdout);
	close_alc_session(new_s_id);
	return -1;
  }
#else
  if((pthread_create(&fdt_thread_id, NULL, fdt_thread, (void*)receiver)) != 0) {
	printf("Error: flute_file_repair, pthread_create\n");
	fflush(stdout);
	close_alc_session(new_s_id);
	return -1;
  }
#endif

  retval = receiver_in_fdt_based_mode(a, receiver);
  
  set_session_state(new_s_id, SExiting);
  
#ifdef _MSC_VER
  WaitForSingleObject(get_alc_session(new_s_id)->handle_rx_thread, INFINITE);
  CloseHandle(get_alc_session(new_s_id)->handle_rx_thread);

  WaitForSingleObject(handle_fdt_thread, INFINITE);
  CloseHandle(handle_fdt_thread);
#else
  join_retval = pthread_join(get_alc_session(new_s_id)->rx_thread_id, NULL);
  assert(join_retval == 0);
  pthread_detach(get_alc_session(new_s_id)->rx_thread_id);
  
  join_retval = pthread_join(fdt_thread_id, NULL);
  assert(join_retval == 0);
  pthread_detach(fdt_thread_id);
#endif

  /* Let's point new lists to NULL so that the data is not removed twise with close_alc_session */

  old_session->obj_list = get_session_obj_list(new_s_id);
  old_session->fdt_list = get_session_fdt_list(new_s_id);
  old_session->wanted_obj_list = get_session_wanted_obj_list(new_s_id);

  new_session->obj_list = NULL;
  new_session->fdt_list = NULL;
  new_session->wanted_obj_list = NULL;

  close_alc_session(new_s_id);
  
  sf_free(a->src_filt);
  free(a->src_filt);
  sdp_message_free(a->sdp);
  
  return retval;
}

#endif

