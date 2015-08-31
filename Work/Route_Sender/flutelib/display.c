/** \file display.c \brief Display information in a thread
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.13 $
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

#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#endif

#include "fdt.h"
#include "flute.h"
#include "../alclib/alc_session.h"
#include "../alclib/transport.h"
#include "../alclib/alc_rx.h"

/**
 * This is a private function which prints sending information.
 *
 * @param sender pointer to sender info structure
 *
 */

void print_sender_info(flute_sender_t *sender) {

  alc_session_t *s;
  file_t *next_file;
  file_t *file;
  
  s = get_alc_session(sender->s_id);

#ifdef _MSC_VER
  system("cls");
#else
  printf("\e[H\e[J");
#endif

  if(s->cc_id == 0 && s->nb_channel == 1) { 
    printf("%-7s%-7s%-13s%-52s\n\n", "TOI", "Tx[%]", "Size[B]", "File");
  }
  else {
    printf("%-7s%-13s%-59s\n\n", "TOI", "Size[B]", "File");
  }
  
  next_file = sender->fdt->file_list;

  while(next_file != NULL) {
    file = next_file;
    
#ifdef _MSC_VER
    printf("%-7I64u", file->toi);
#else 
    printf("%-7llu", file->toi);
#endif

    if(s->cc_id == 0 && s->nb_channel == 1) {
    
      if(get_session_tx_toi(sender->s_id) == file->toi) {
      
	printf("%-7.2f", (double)((double)100 *
				  ((double)(long long)get_object_sent_bytes(sender->s_id)/(double)(long long)file->content_len)));
      }
      else {
	printf("%-7.s", "");
      }
    }
      
#ifdef _MSC_VER
    printf("%-13I64u",  file->content_len);
#else
    printf("%-13llu",  file->content_len);
#endif
    
    if(s->cc_id == 0 && s->nb_channel == 1) {
      printf("%-52s\n",  file->location);
    }
    else {
      printf("%-59s\n",  file->location);
    }
    
    fflush(stdout);
    
    next_file = file->next;
  }
}

/**
 * This is a private function which prints receiving information.
 *
 * @param receiver pointer to receiver info structure
 *
 */

void print_receiver_info(flute_receiver_t *receiver) {

  trans_obj_t *trans_obj;
  alc_session_t *s;
  
  file_t *next_file;
  file_t *file;

  char tmp[17];
  
  s = get_alc_session(receiver->s_id);

#ifdef _MSC_VER
  system("cls");
#else
  printf("\e[H\e[J");
#endif

  printf("%-7s%-7s%-16s%-13s%-36s\n\n", "TOI", "Rx[%]", "Blocks",  "Size[B]", "File");

  next_file = receiver->fdt->file_list;

  while(next_file != NULL) {
    file = next_file;

#ifdef _MSC_VER
    printf("%-7I64u", file->toi);
#else
    printf("%-7llu", file->toi);
#endif

    trans_obj = object_exist(file->toi, s, 1);
    
    if(trans_obj != NULL) {

      printf("%-7.2f", (double)((double)100 *
                                ((double)(long long)trans_obj->rx_bytes/(double)(long long)trans_obj->len)));
      
      memset(tmp, 0, 17);
      sprintf(tmp, "%u/%u", trans_obj->nb_of_ready_blocks, trans_obj->bs->N);
      printf("%-16s", tmp);
    }
    else if(file->status == 2) {
       printf("%-7s%-16s", "100", "");
    }
    else {
      printf("%-7s%-16s", "", "");
    }
       
#ifdef _MSC_VER
    printf("%-13I64u",  file->content_len);
#else
    printf("%-13llu",  file->content_len);
#endif

    printf("%-36s\n",  file->location);
    fflush(stdout);
    
    next_file = file->next;
  }
}

void* sender_file_table_output_thread(void *a) {

  flute_sender_t *sender;

  sender = (flute_sender_t*)a;

  while(get_session_state(sender->s_id) == SActive || get_session_state(sender->s_id) == SPaused) {

    if(sender->fdt != NULL) {
      print_sender_info(sender);
    }

#ifdef _MSC_VER
    Sleep(1000);
#else
    sleep(1);
#endif
  }

#ifdef _MSC_VER
  _endthread();
#else
  pthread_exit(0);
#endif

  return NULL;
}

void* receiver_file_table_output_thread(void *a) {

  flute_receiver_t *receiver;

  receiver = (flute_receiver_t*)a;
  
  while(get_session_state(receiver->s_id) == SActive ||
		get_session_state(receiver->s_id) == SAFlagReceived) {

    if(receiver->fdt != NULL) { 
      print_receiver_info(receiver);
    }

#ifdef _MSC_VER
    Sleep(1000);
#else
    sleep(1);
#endif
  }

#ifdef _MSC_VER
  _endthread();
#else
  pthread_exit(0);
#endif

  return NULL;
}
