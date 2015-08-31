/** \file alc_channel.c \brief ALC channel
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.34 $
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
#include <math.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>

#ifdef _MSC_VER
#include <process.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#endif

#include "alc_channel.h"
#include "alc_socket.h"
#include "alc_list.h"
#include "alc_rx.h"

int open_alc_channel(alc_channel_t *ch, alc_session_t *s, const char *port,
		     const char *addr, const char *intface, const char *intface_name, int tx_rate) {
  
  int ret_val;
  
  if (!(ch = (alc_channel_t*)calloc(1, sizeof(alc_channel_t)))) {
    printf("Could not alloc memory for  alc channel!\n");
    return -1;
  }
  
  ch->ch_id = s->nb_channel; /* session level identifier for channel */
  ch->s = s;
  
  if(s->mode == SENDER) {
    if(ch->ch_id == 0) {
      
      if(ch->s->cc_id == Null) {
	ch->tx_rate = tx_rate;
	ch->nb_tx_units = 1;
	ch->ready = FALSE;
	
	s->nb_sending_channel++;
      }
      else if(ch->s->cc_id == RLC) {
	ch->tx_rate = tx_rate;
	ch->nb_tx_units = 1;
	ch->start_sending = TRUE;
	ch->ready = FALSE;
	ch->wait_after_sp = 0;
	
	s->nb_sending_channel++;
      }
    }
    else {
      
      if(ch->s->cc_id == Null) {
	ch->tx_rate = tx_rate * (int)pow(2.0, (double)(ch->ch_id - 1));
	ch->nb_tx_units = (int)pow(2.0, (double)(ch->ch_id - 1));
	ch->ready = FALSE;
	
	s->nb_sending_channel++;
      }			
      else if(ch->s->cc_id == RLC) {
	ch->tx_rate = tx_rate * (int)pow(2.0, (double)(ch->ch_id - 1));
	ch->nb_tx_units = (int)pow(2.0, (double)(ch->ch_id - 1));
	ch->start_sending = FALSE;
	ch->ready = FALSE;
	ch->wait_after_sp = RLC_WAIT_AFTER_SP;
      }
    }
    
    ch->addr = addr;
    ch->port = port;
    ch->intface = intface;
    
    ch->previous_lost = FALSE;
  }
  else if(s->mode == RECEIVER) {
    ch->addr = addr;
    ch->port = port;
    ch->intface = intface;
    ch->intface_name = intface_name;
  }
  
  ret_val = init_alc_socket(ch);
  
  if(ret_val < 0) {
    return ret_val;
  }
  
  assert(s->ch_list[s->nb_channel] == NULL);

  if(s->mode == RECEIVER) {

	assert(ch->receiving_list == 0);
	ch->receiving_list = build_list();

	/* Create receiving socket thread */
#ifdef _MSC_VER
	ch->handle_rx_socket_thread = (HANDLE)_beginthreadex(NULL, 0, (void*)rx_socket_thread,
								  (void*)ch, 0, &ch->rx_socket_thread_id);

	if(ch->handle_rx_socket_thread == NULL) {
		perror("open_alc_channel: _beginthread");
		return -1;
	}
#else
	if(pthread_create(&ch->rx_socket_thread_id, NULL, rx_socket_thread, (void*)ch) != 0) {
		perror("open_alc_channel: pthread_create");
		return -1;
	}
#endif
  }

  s->ch_list[s->nb_channel] = ch;
  s->nb_channel++;
  
  return ch->ch_id;
}

int close_alc_channel(alc_channel_t *ch, alc_session_t *s) {
	
  int ret_val;
  int ch_id = ch->ch_id;
  
  ret_val = close_alc_socket(ch);
  
  /* trying again if ret_val == -1 */
  if(ret_val == -1) {
    ret_val = close_alc_socket(ch);
  }	
  
  assert(s->ch_list[ch_id] == ch);
  freeaddrinfo(ch->addrinfo);

  if(s->mode == RECEIVER) {
	join_rx_socket_thread(ch);
	destroy_list(ch->receiving_list);
	ch->receiving_list = NULL;
  }

  assert(ch->receiving_list == NULL);

  free(ch);
  s->ch_list[ch_id] = NULL;
  
  s->nb_channel--;
  
  return ret_val;
}

