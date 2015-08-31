/** \file alc_session.c \brief ALC session
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.103 $
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
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#ifdef _MSC_VER
#include <process.h>
#else
#include <pthread.h>
#endif

#include <sys/timeb.h>

#include "alc_session.h"
#include "mad_rlc.h"
#include "alc_rx.h"
#include "alc_tx.h"
#include "transport.h"
#include "alc_channel.h"

/**
 * Use absolute path with base directory.
 */

#define ABSOLUTE_PATH 1

struct alc_session *alc_session_list[MAX_ALC_SESSIONS]; /**< List which contains all ALC sessions */
int nb_alc_session = 0; /**< Number of ALC sessions */

/**
 * Global variables semaphore
 */

#ifdef _MSC_VER
RTL_CRITICAL_SECTION session_variables_semaphore;
#else
pthread_mutex_t session_variables_semaphore = PTHREAD_MUTEX_INITIALIZER;
#endif

/**
 * This is a private function, which locks the session.
 *
 */

void lock_session() {
#ifdef _MSC_VER
  EnterCriticalSection(&session_variables_semaphore);
#else
  pthread_mutex_lock(&session_variables_semaphore);
#endif
}

/**
 * This is a private function, which unlocks the session.
 *
 */

void unlock_session() {
#ifdef _MSC_VER
  LeaveCriticalSection(&session_variables_semaphore);
#else
  pthread_mutex_unlock(&session_variables_semaphore);
#endif
}

int open_alc_session(alc_arguments_t *a) {

  alc_session_t *s;
  int i;
  int retval;
  struct timeb timeb_current_time;
  
#if ABSOLUTE_PATH
  char fullpath[MAX_PATH_LENGTH];
#endif

  lock_session();
  
  if(!lib_init) {
    alc_init();
    /* alc session list initialization */
    memset(alc_session_list, 0, MAX_ALC_SESSIONS * sizeof(alc_session_t*));
  }
  
  if(nb_alc_session >= MAX_ALC_SESSIONS) {
    /* Could not create new alc session */
    printf("Could not create new alc session: too many sessions!\n");
    unlock_session();
    return -1;
  }
  
  if (!(s = (alc_session_t*)calloc(1, sizeof(alc_session_t)))) {
    printf("Could not alloc memory for alc session!\n");
    unlock_session();
    return -1;
  }

  memset(s, 0, sizeof(alc_session_t));

  s->mode = a->mode;
  s->tsi = a->tsi;
  s->state = SActive;
  s->addr_family = a->addr_family;
  s->addr_type = a->addr_type;
  s->cc_id = a->cc_id;
  
  if(s->cc_id == RLC) {
    retval = init_mad_rlc(s);
    
    if(retval < 0) {
      unlock_session();
      return retval;
    }
  }
  
  s->rx_memory_mode = a->rx_memory_mode;
  s->verbosity = a->verbosity;
  s->starttime = a->start_time;
  s->stoptime = a->stop_time;
  s->simul_losses = a->simul_losses;
  s->loss_ratio1 = a->loss_ratio1;
  s->loss_ratio2 = a->loss_ratio2;
  
  if(s->mode == SENDER) {

    ftime(&timeb_current_time);
	s->ftimestarttime = timeb_current_time.time+timeb_current_time.millitm/1000.0;

    memcpy(s->base_dir, a->base_dir, strlen(a->base_dir));
    
    s->nb_channel = 0;
    s->max_channel = a->nb_channel;
    s->def_fec_ratio = a->fec_ratio; 
    s->sent_bytes = 0;
    s->obj_sent_bytes = 0;
    s->last_print_tx_percent = 0;
    s->a_flag = 0;
    s->fdt_instance_id = 0; /*uplimit 16777215;*/
    s->def_ttl = a->ttl;
    s->def_fec_enc_id = a->fec_enc_id;
    s->def_fec_inst_id = a->fec_inst_id;
    s->def_max_sblen = a->max_sb_len;
    s->def_eslen = a->es_len;
    s->use_fec_oti_ext_hdr = a->use_fec_oti_ext_hdr;
    s->def_tx_rate = a->tx_rate;
    s->tx_queue_begin = NULL;
    s->tx_queue_end = NULL;
    s->tx_queue_size = 0;
    s->first_unit_in_loop = TRUE;
    s->nb_ready_channel = 0;
    s->nb_sending_channel = 0;
    s->half_word = a->half_word;
    s->encode_content = a->encode_content;
    s->optimize_tx_rate =  a->optimize_tx_rate;
    s->calculate_session_size = a->calculate_session_size;

    if(((s->cc_id == RLC) || ((s->cc_id == Null) && (s->max_channel != 1)))) {

      /**** Start tx_thread ****/
      
#ifdef _MSC_VER
      s->handle_tx_thread = (HANDLE)_beginthreadex(NULL, 0, (void*)tx_thread, (void*)s, 0, &s->tx_thread_id);

      if(s->handle_tx_thread == NULL) {
	perror("open_alc_session: _beginthread");
	unlock_session();
	return -1;
      }
#else
      if(pthread_create(&s->tx_thread_id, NULL, tx_thread, (void*)s) != 0) {
	perror("open_alc_session: pthread_create");
	unlock_session();
	return -1;
      }
#endif
    }
  }
  
  if(s->mode == RECEIVER) {
    
#ifdef SSM
    s->ssm = a->use_ssm;
#endif

#if ABSOLUTE_PATH    
#ifdef _MSC_VER
    memset(fullpath, 0, MAX_PATH_LENGTH);
    
    if(_fullpath(fullpath, a->base_dir, MAX_PATH_LENGTH) != NULL) {
      memcpy(s->base_dir, fullpath, strlen(fullpath));
    }
    else {
      memcpy(s->base_dir, a->base_dir, strlen(a->base_dir));
    }
#else
    memset(fullpath, 0, MAX_PATH_LENGTH);
    
    if(a->base_dir[0] != '/') {
      
      if(getcwd(fullpath, MAX_PATH_LENGTH) != NULL) {
	memcpy(s->base_dir, fullpath, strlen(fullpath));
	strcat(s->base_dir, "/");
	strcat(s->base_dir, a->base_dir);
      }
      else {
	memcpy(s->base_dir, a->base_dir, strlen(a->base_dir));
      }
    }
    else {
      memcpy(s->base_dir, a->base_dir, strlen(a->base_dir));
    }
#endif
#else
	 memcpy(s->base_dir, a->base_dir, strlen(a->base_dir));
#endif

    s->nb_channel = 0;
    s->max_channel = a->nb_channel;
    s->obj_list = NULL;
    s->fdt_list = NULL;
    s->wanted_obj_list = NULL;
    s->rx_fdt_instance_list = NULL;
    s->accept_expired_fdt_inst = a->accept_expired_fdt_inst;
    
    memset(s->src_addr, 0, 40);
    
    if(a->src_addr != NULL) {
      memcpy(s->src_addr, a->src_addr, strlen(a->src_addr));
    }
    
    /* Create receiving thread */
    
#ifdef _MSC_VER
    s->handle_rx_thread =
      (HANDLE)_beginthreadex(NULL, 0, (void*)rx_thread, (void*)s, 0, &s->rx_thread_id);
    
    if(s->handle_rx_thread == NULL) {
      perror("open_alc_session: _beginthread");
      unlock_session();
      return -1;
    }
#else
    if(pthread_create(&s->rx_thread_id, NULL, rx_thread, (void*)s) != 0) {
      perror("open_alc_session: pthread_create");
      unlock_session();
      return -1;
    }
#endif
    
  }
  
  for(i = 0; i < MAX_ALC_SESSIONS; i++) {
    if(alc_session_list[i] == NULL) {
      s->s_id = i;
      alc_session_list[s->s_id] = s;
      break;
    }
  }
  
  nb_alc_session++;
  
  unlock_session();
  return s->s_id;
}

void close_alc_session(int s_id) {

  int i;
  wanted_obj_t *next_want;
  wanted_obj_t *want;
  rx_fdt_instance_t *next_instance;
  rx_fdt_instance_t *instance;
  
  tx_queue_t *next_pkt;
  tx_queue_t *pkt;
  
  trans_obj_t *to;
  
  alc_session_t *s;

#ifdef USE_RETRIEVE_UNIT
  trans_unit_container_t *tmp;
  trans_unit_container_t *to_delete;
#endif

#ifdef LINUX
  int join_retval;
#endif

  /* Wait for open thread. */
#ifdef _MSC_VER
  if(alc_session_list[s_id]->handle_rx_thread != NULL) {
	WaitForSingleObject(alc_session_list[s_id]->handle_rx_thread, INFINITE);
	CloseHandle(alc_session_list[s_id]->handle_rx_thread);
	alc_session_list[s_id]->handle_rx_thread = NULL;
  }
#else
  if(alc_session_list[s_id]->rx_thread_id != 0) {   
     join_retval = pthread_join(alc_session_list[s_id]->rx_thread_id, NULL);
     assert(join_retval == 0);
     pthread_detach(alc_session_list[s_id]->rx_thread_id);
     alc_session_list[s_id]->rx_thread_id = 0;
  }
#endif

  lock_session();
  s = alc_session_list[s_id];
  s->state = SClosed;  

  for(i = 0; i < s->max_channel; i++) {
    
    if(s->ch_list[i] != NULL) {
      close_alc_channel(s->ch_list[i], s);
    }
  }

#ifdef USE_RETRIEVE_UNIT
	tmp = s->unit_pool;

	while(tmp != NULL ) {
		to_delete = tmp;
		tmp = tmp->next;
		free(to_delete->u.data);
		free(to_delete);
	}

	s->unit_pool = NULL;
#endif

  /* Closing, free all uncompleted objects, uncompleted fdt instances and wanted obj list */
  
  to = s->obj_list;
  
  while(to != NULL) {
    free_object(to, s, 1);
    to = s->obj_list;
  }
  
  to = s->fdt_list;
  
  while(to != NULL) {
    free_object(to, s, 0);
    to = s->fdt_list;
  }
  
  want = s->wanted_obj_list;
  
  while(want != NULL) {
    next_want = want->next;
    free(want);
    want = next_want;
  } 	
  
  instance = s->rx_fdt_instance_list;
  
  while(instance != NULL) {
    next_instance = instance->next;
    free(instance);
    instance = next_instance;
  }
    
  if(s->cc_id == RLC) {
    close_mad_rlc(s);
  }
  
  pkt = s->tx_queue_begin;
  
  while(pkt != NULL) {
    next_pkt = pkt->next;
    free(pkt->data);
    free(pkt);
    pkt = next_pkt;
  }
  
  free(s);
  alc_session_list[s_id] = NULL;
  nb_alc_session--;
  unlock_session();
}

alc_session_t* get_alc_session(int s_id) {

  alc_session_t* alc_session;

  lock_session();
  assert (alc_session_list != NULL);
  assert (s_id >= 0);
  assert (s_id < MAX_ALC_SESSIONS);
  alc_session = alc_session_list[s_id];

  unlock_session();

  return alc_session;
}


int add_alc_channel(int s_id, const char *port, const char *addr, const char *intface, const char *intface_name) {
	
  alc_channel_t *ch;
  alc_session_t *s;
  int ret;

  lock_session();
  ch = NULL;
  s = alc_session_list[s_id];

  if(s->nb_channel >= s->max_channel) {
    /* Could not add new alc channel to alc session */
    printf("Could not create new alc channel: Max number of channels already used!\n");
    unlock_session();
    return -1;
  }

  ret = open_alc_channel(ch, s, port, addr, intface, intface_name, s->def_tx_rate);
  unlock_session();

  return ret;
}

void remove_alc_channels(int s_id) {

  alc_session_t *s;
  alc_channel_t *ch;
  int i;

  lock_session();
  s = alc_session_list[s_id];

  for(i = 0; i < s->max_channel; i++) {

    if(s->ch_list[i] != NULL) {
      ch = s->ch_list[i];
      close_alc_channel(ch, s);
    }
  }

  unlock_session();
}

trans_obj_t* get_session_obj_list(int s_id) {

  trans_obj_t* obj_list;

  lock_session();
  obj_list = alc_session_list[s_id]->obj_list;
  unlock_session();

  return obj_list;
}

trans_obj_t* get_session_fdt_list(int s_id) {

  trans_obj_t* fdt_list;

  lock_session();
  fdt_list = alc_session_list[s_id]->fdt_list;
  unlock_session();

  return fdt_list;
}

wanted_obj_t* get_session_wanted_obj_list(int s_id) {

  wanted_obj_t* wanted_obj_list;

  lock_session();
  wanted_obj_list = alc_session_list[s_id]->wanted_obj_list;
  unlock_session();

  return wanted_obj_list;
}

int get_session_state(int s_id) {
  int state;

  lock_session();

  if(alc_session_list[s_id] == NULL) {
    unlock_session();
    return -1;
  }

  state = alc_session_list[s_id]->state;
  unlock_session();
  return state;
}

void set_session_state(int s_id, enum alc_session_states state) {
  lock_session();
  alc_session_list[s_id]->state = state;
  unlock_session();
}

void set_all_sessions_state(enum alc_session_states state) {

  int i;
  lock_session();

  for(i = 0; i < MAX_ALC_SESSIONS; i++)
    {
      if(alc_session_list[i] != NULL)
        {
	  alc_session_list[i]->state = state;
        }
    }
  unlock_session();
}

int get_session_a_flag_usage(int s_id) {

  int flag;
  lock_session();
  flag = alc_session_list[s_id]->a_flag;
  unlock_session();

  return flag;
}

void set_session_a_flag_usage(int s_id) {

  lock_session();
  alc_session_list[s_id]->a_flag = 1;
  unlock_session();
}

unsigned int get_fdt_instance_id(int s_id) {

  int instance_id;

  lock_session();
  instance_id = alc_session_list[s_id]->fdt_instance_id;
  unlock_session();

  return instance_id;
}

void set_fdt_instance_id(int s_id, unsigned int instance_id) {
  lock_session();
  alc_session_list[s_id]->fdt_instance_id =  (instance_id & 0x00FFFFFF);
  unlock_session();
}

void set_fdt_instance_parsed(int s_id) {
  lock_session();
  alc_session_list[s_id]->waiting_fdt_instance = FALSE;
  unlock_session();
}

unsigned long long get_session_sent_bytes(int s_id) {
  unsigned long long byte_sent;

  lock_session();
  byte_sent = alc_session_list[s_id]->sent_bytes;
  unlock_session();

  return byte_sent;
}

void set_session_sent_bytes(int s_id, unsigned long long sent_bytes) {

  lock_session();
  alc_session_list[s_id]->sent_bytes = sent_bytes;
  unlock_session();
}

void add_session_sent_bytes(int s_id, unsigned int sent_bytes) {

  lock_session();
  alc_session_list[s_id]->sent_bytes += sent_bytes;
  unlock_session();
}

unsigned long long get_object_sent_bytes(int s_id) {
  unsigned long long byte_sent;

  lock_session();
  byte_sent = alc_session_list[s_id]->obj_sent_bytes;
  unlock_session();

  return byte_sent;
}

 void set_object_sent_bytes(int s_id, unsigned long long sent_bytes) {

   lock_session();
   alc_session_list[s_id]->obj_sent_bytes = sent_bytes;
   unlock_session();
 }

void add_object_sent_bytes(int s_id, unsigned int sent_bytes) {

  lock_session();
  alc_session_list[s_id]->obj_sent_bytes += sent_bytes;
  unlock_session();
}

double get_object_last_print_tx_percent(int s_id) {                                                                                                                   
  double tx_percent;

  lock_session();
  tx_percent = alc_session_list[s_id]->last_print_tx_percent;
  unlock_session();

  return tx_percent;
}
                                                                                                                                          
void set_object_last_print_tx_percent(int s_id, double last_print_tx_percent) {                                                                                  
  lock_session();
  alc_session_list[s_id]->last_print_tx_percent = last_print_tx_percent;
  unlock_session();
}

void set_session_tx_toi(int s_id, unsigned long long toi) {

  lock_session();
  alc_session_list[s_id]->tx_toi = toi;
  unlock_session();
}

unsigned long long get_session_tx_toi(int s_id) {

  unsigned long long tx_toi;

  lock_session();
  tx_toi = alc_session_list[s_id]->tx_toi;
  unlock_session();

  return tx_toi;
}

void update_session_tx_rate(int s_id, int base_tx_rate) {
	alc_session_t *s;
	alc_channel_t *ch;
	int i;

	lock_session();
	s = alc_session_list[s_id];
	
	for(i = 0; i < s->max_channel; i++) {
		if(s->ch_list[i] != NULL) {
			ch = s->ch_list[i];

			if(ch->ch_id == 0) {
        			ch->tx_rate = base_tx_rate;
        			ch->nb_tx_units = 1;
			}
			else {
				ch->tx_rate = base_tx_rate * (int)pow(2.0, (double)(ch->ch_id - 1));
				ch->nb_tx_units = (int)pow(2.0, (double)(ch->ch_id - 1));
			}
			printf("new rate [channel: %i]: %i\n", ch->ch_id, ch->tx_rate);
		}
	}
	unlock_session();
}

wanted_obj_t* get_wanted_object(alc_session_t *s, unsigned long long toi) {

	wanted_obj_t *tmp;

	lock_session();

	tmp = s->wanted_obj_list;

	while(tmp != NULL) {
		if(tmp->toi == toi) {
		  unlock_session();
		  return tmp;
		}
		tmp = tmp->next;
	}

	unlock_session();
	return NULL;
}

int set_wanted_object(int s_id, unsigned long long toi,
		      unsigned long long transfer_len,
		      unsigned short es_len, unsigned int max_sb_len, int fec_inst_id,
		      short fec_enc_id, unsigned short max_nb_of_es,
		      unsigned char content_enc_algo, unsigned char finite_field,
			  unsigned char nb_of_es_per_group) {
	
  alc_session_t *s;
  wanted_obj_t *wanted_obj;
  wanted_obj_t *tmp;
  
  lock_session();
   
  s = alc_session_list[s_id];
  tmp = s->wanted_obj_list;
  
  if(tmp == NULL) {
    
    if (!(wanted_obj = (wanted_obj_t*)calloc(1, sizeof(wanted_obj_t)))) {
      printf("Could not alloc memory for wanted object!\n");
      unlock_session();
      return -1;
    }
    
    wanted_obj->toi = toi;
    wanted_obj->transfer_len = transfer_len;
    wanted_obj->es_len = es_len;
    wanted_obj->max_sb_len = max_sb_len;
    wanted_obj->fec_inst_id = fec_inst_id;
    wanted_obj->fec_enc_id = fec_enc_id;
    wanted_obj->max_nb_of_es = max_nb_of_es;
    wanted_obj->content_enc_algo = content_enc_algo;
	wanted_obj->finite_field = finite_field;
	wanted_obj->nb_of_es_per_group = nb_of_es_per_group;

    wanted_obj->prev = NULL;
    wanted_obj->next = NULL;

    s->wanted_obj_list = wanted_obj;
  }
  else {
    for(;; tmp = tmp->next) {
      if(tmp->toi == toi) {
	break;
      }
      else if(tmp->next == NULL) {
	
	if (!(wanted_obj = (wanted_obj_t*)calloc(1, sizeof(wanted_obj_t)))) {
	  printf("Could not alloc memory for wanted object!\n");
	  unlock_session();
	  return -1;
	}
	
	wanted_obj->toi = toi;
	wanted_obj->transfer_len = transfer_len;
	wanted_obj->es_len = es_len;
	wanted_obj->max_sb_len = max_sb_len;
	wanted_obj->fec_inst_id = fec_inst_id;
	wanted_obj->fec_enc_id = fec_enc_id;
	wanted_obj->max_nb_of_es = max_nb_of_es;
	wanted_obj->content_enc_algo = content_enc_algo;
	wanted_obj->finite_field = finite_field;
	wanted_obj->nb_of_es_per_group = nb_of_es_per_group;
	
	tmp->next = wanted_obj;
	wanted_obj->prev = tmp;
	wanted_obj->next = NULL;

	break;
      }
    }
  }
  
  unlock_session();
  return 0;
}

void remove_wanted_object(int s_id, unsigned long long toi) {

	alc_session_t *s; 	
	wanted_obj_t *next_want;
	wanted_obj_t *want;
	
	lock_session();

	s = alc_session_list[s_id];
	next_want = s->wanted_obj_list;

	while(next_want != NULL) {
		
		want = next_want;
		  
	    	if(want->toi == toi) {
			
	      		if(want->next != NULL) {
					want->next->prev = want->prev;
	      		}
	      		if(want->prev != NULL) {
					want->prev->next = want->next;
	      		}
	      		if(want == s->wanted_obj_list) {
					s->wanted_obj_list = want->next;
	      		}
	      
	      		free(want);
	      		break;
	    	}
	    	next_want = want->next;
	}

	unlock_session();
}
                                                                                                                                              
BOOL is_received_instance(alc_session_t *s, unsigned int fdt_instance_id) {
                                                                                                                                              
        BOOL retval = FALSE;
        rx_fdt_instance_t *list = s->rx_fdt_instance_list;
                                                                                                                                              
        while(list != NULL) {
                if(list->fdt_instance_id == fdt_instance_id) {
                        retval = TRUE;
                        break;
                }
                list = list->next;
        }
                                                                                                                                              
        return retval;
}

int set_received_instance(alc_session_t *s, unsigned int fdt_instance_id) {
 
        rx_fdt_instance_t *rx_fdt_instance;
        rx_fdt_instance_t *list;
        
	lock_session();

        list = s->rx_fdt_instance_list;
                                                                                                                                              
        if(list == NULL) {
                                                                                                                                              
                if (!(rx_fdt_instance = (rx_fdt_instance_t*)calloc(1, sizeof(rx_fdt_instance_t)))) {
                        printf("Could not alloc memory for rx_fdt_instance!\n");
			unlock_session();
                        return -1;
                }

                rx_fdt_instance->fdt_instance_id = fdt_instance_id;
                rx_fdt_instance->prev = NULL;
                rx_fdt_instance->next = NULL;
        
			s->rx_fdt_instance_list = rx_fdt_instance;
        }
        else {
                for(;; list = list->next) {
                        if(list->fdt_instance_id == fdt_instance_id) {
                                break;
                        }
                        else if(list->next == NULL) {
              
							  if (!(rx_fdt_instance = (rx_fdt_instance_t*)calloc(1, sizeof(rx_fdt_instance_t)))) {
								printf("Could not alloc memory for rx_fdt_instance!\n");
								unlock_session();
								return -1;
							  }
			  
							  rx_fdt_instance->fdt_instance_id = fdt_instance_id;
							  
							  list->next = rx_fdt_instance;
							  rx_fdt_instance->prev = list;
							  rx_fdt_instance->next = NULL;

							  break;
                        }
                }
        }
	
	unlock_session();
        return 0;
}

char* get_session_basedir(int s_id) {

  alc_session_t *s;
  char* base_dir;

  lock_session();

  s = alc_session_list[s_id];
  base_dir = s->base_dir;

  unlock_session();

  return base_dir;
}

void initialize_session_handler() {
#ifdef _MSC_VER
  InitializeCriticalSection(&session_variables_semaphore);
#else
#endif
}

void release_session_handler() {
#ifdef _MSC_VER
  DeleteCriticalSection(&session_variables_semaphore);
#else
#endif
}
