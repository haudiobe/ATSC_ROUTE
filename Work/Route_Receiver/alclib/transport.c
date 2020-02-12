/** \file transport.c \brief Transport structures and functions
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.43 $
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
#include <errno.h>
#include <assert.h>
#include <string.h>

#ifdef _MSC_VER
#include <io.h>
#endif

#include "transport.h"


/**
 * Global variables semaphore
 */

#ifdef _MSC_VER
RTL_CRITICAL_SECTION session_variables_semaphore;
#else
pthread_mutex_t transport_variables_semaphore = PTHREAD_MUTEX_INITIALIZER;
#endif

/**
 * This is a private function, which locks the transport object.
 *
 */

void lock_trans_obj() {
#ifdef _MSC_VER
  EnterCriticalSection(&transport_variables_semaphore);
#else
  pthread_mutex_lock(&transport_variables_semaphore);
#endif
}

/**
 * This is a private function, which unlocks the transport object.
 *
 */

void unlock_trans_obj() {
#ifdef _MSC_VER
  LeaveCriticalSection(&transport_variables_semaphore);
#else
  pthread_mutex_unlock(&transport_variables_semaphore);
#endif
}


trans_obj_t* create_object(void) {
  
  trans_obj_t *obj = NULL;

  if (!(obj = (trans_obj_t*)calloc(1, sizeof(trans_obj_t)))) {
    printf("Could not alloc memory for transport object!\n");
    return NULL;
  }

  return obj;
}

trans_block_t* create_block(void) {
 
  trans_block_t *block = NULL;

  if (!(block = (trans_block_t*)calloc(1, sizeof(trans_block_t)))) {
    printf("Could not alloc memory for transport block!\n");
    return NULL;
  }

  return block;
}

trans_unit_t* create_units(unsigned int number) {
  
  trans_unit_t *unit = NULL;

  if (!(unit = (trans_unit_t*)calloc(number, sizeof(trans_unit_t)))) {
    printf("Could not alloc memory for %i transport units!\n", number);
  }

  return unit;
}

void free_units(trans_block_t *tb) {

  trans_unit_t *tu = NULL;
  trans_unit_t *next_tu = NULL;

  next_tu = tb->unit_list;

  while(next_tu != NULL) {
    tu = next_tu;

    if(tu->data != NULL) {
      free(tu->data);
      tu->data = NULL;
    }
    
    next_tu = tu->next;
    free(tu);
  }
  
  tb->unit_list = NULL;
}

#ifdef USE_RETRIEVE_UNIT

int FRV_counter = 0;

trans_unit_t* retrieve_unit(alc_session_t *s, unsigned short es_len) {

  trans_unit_container_t *container = NULL;
  trans_unit_container_t *tmp;
  trans_unit_container_t *start_search;

  if(s->last_given != NULL) {
    start_search = s->last_given->next;
  } else {
    start_search = s->unit_pool;
  }

  tmp = start_search;

  while(tmp != NULL ) { 
    if(tmp->u.used == 0) {

      //Malek El Khatib 12.08.2014
      if (numEncSymbPerPacket != 0)
      {//END
        if(tmp->u.len < es_len) {
          printf("[FRV]: tmp->u.len (%d) < es_len (%d)\n", tmp->u.len, es_len);
          fflush(stdout);
          FRV_counter++;
          if (FRV_counter == 40)
          {
            int a = FRV_counter/0;

          }
          continue;

        }
      }

      tmp->u.used = 1;
      s->last_given = tmp;
      assert(tmp->u.data != NULL);
      return &(tmp->u);
    }
    tmp = tmp->next;
  }
  
  tmp = s->unit_pool;
  while(tmp != start_search) {    
    if(tmp->u.used == 0) {

      //Malek El Khatib 12.08.2014
      if (numEncSymbPerPacket != 0)
      {//END
        if(tmp->u.len < es_len) {
          continue;
        }
      }

      tmp->u.used = 1;
      s->last_given = tmp;
      assert(tmp->u.data != NULL);
      return &(tmp->u);
    }
    tmp = tmp->next;
  }

  if(!(container = (trans_unit_container_t*)calloc(1, sizeof(trans_unit_container_t)))) {
    printf("Could not alloc memory for a transport unit container!\n");
    return NULL;
  }

  container->next = s->unit_pool;
  s->unit_pool = container;
  container->u.used = 1;
  s->last_given = s->unit_pool;

  if(!(container->u.data = (char*)calloc(es_len, sizeof(char)))) {
    printf("Could not alloc memory for transport unit's data!\n");
  free(container);
    return NULL;
  }

  return &(container->u);
}

void free_units2(trans_block_t *tb) {
    trans_unit_t *tu = NULL;
    trans_unit_t *current_tu = NULL;

    char *data = NULL;
    unsigned short  data_buffer_len;

    current_tu = tb->unit_list;
  
    while(current_tu != NULL) {
      tu = current_tu;

      /* backup the data pointer */

      data = tu->data;
      data_buffer_len = tu->len;

      current_tu = tu->next;

      memset(tu, 0, sizeof(trans_unit_t));

      /* reinstall the backup */
      tu->data = data;
      tu->len = data_buffer_len;
    }

    tb->unit_list = NULL;
}

#endif

/*
void insert_object(trans_obj_t *to, alc_session_t *s, int type) {
  
  trans_obj_t *tmp;
  
  if(type == 0) {
    tmp = s->fdt_list;
  }
  else {
    tmp = s->obj_list;
  }

  if(tmp == NULL) {

      if(type == 0) {
                  s->fdt_list = to;
          }  
          else {
                  s->obj_list = to;
            }
  }
  else {
    for(;;) {
      if(to->toi < tmp->toi) {

        if(tmp->prev == NULL) {

          to->next = tmp;
          to->prev = tmp->prev;
        
          tmp->prev = to;

                if(type == 0) {
                        s->fdt_list = to;
                }
                else {
                        s->obj_list = to;
                }

        }
        else {

          to->next = tmp;
          to->prev = tmp->prev;
        
          tmp->prev->next = to;
          tmp->prev = to;
        }
        break;
      }

      if(tmp->next == NULL) {

        to->next = tmp->next;
        to->prev = tmp;
        
        tmp->next = to;
        break;
      }

      tmp = tmp->next;
    }
  }
}*/

void insert_object(trans_obj_t *to, alc_session_t *s, int type) {

    trans_obj_t *tmp;

    if(type == 0) {
        tmp = s->fdt_list;
    }
    else {
        tmp = s->obj_list;
    }

    if(tmp == NULL) {

        if(type == 0) {
          s->fdt_list = to;
        }  
        else {
          s->obj_list = to;
        }
        return;
    }

    for(;;) {
        if(to->toi < tmp->toi) {

            if(tmp->prev == NULL) {

                to->next = tmp;
                to->prev = tmp->prev;

                tmp->prev = to;

                if(type == 0) {
                  s->fdt_list = to;
                }
                else {
                    s->obj_list = to;
                }

            }
            else {

                to->next = tmp;
                to->prev = tmp->prev;

                tmp->prev->next = to;
                tmp->prev = to;
            }
            break;
        }

        if(tmp->next == NULL) {

            to->next = tmp->next;
            to->prev = tmp;

            tmp->next = to;
            break;
        }

        tmp = tmp->next;
    }
}

/*
 * This function inserts transport unit to transport block.
 *
 * Params:  trans_unit_t *tu: Pointer to transport unit to be inserted,
 *      trans_block_t *tb: Pointer to transport block,
 *      trans_obj_t *tu: Pointer to transport object.
 *
 * Return:  int: 0 when transport unit is inserted, 1 when duplicated transport unit.
 *
 */
/*
int insert_unit(trans_unit_t *tu, trans_block_t *tb, trans_obj_t *to) {

  trans_unit_t *tmp;
  int retval = 0;

  tmp = tb->unit_list;

  if(tmp == NULL) {

    to->rx_bytes += tu->len;
    
    // for percentage counter
    if(to->rx_bytes > to->len) {
      to->rx_bytes = to->len;    
    }

    tb->nb_of_rx_units++;

    tb->unit_list = tu;
  }
  else {
    for(;;) {
      if(tu->esi < tmp->esi) {
        //Delayed unit

        to->rx_bytes += tu->len;

        if(to->rx_bytes > to->len) {
          to->rx_bytes = to->len;    
        }

        tb->nb_of_rx_units++;
        
        if(tmp->prev == NULL) {
          tu->next = tmp;
          tu->prev = tmp->prev;
        
          tmp->prev = tu;

          tb->unit_list = tu;
        }
        else {
          tu->next = tmp;
          tu->prev = tmp->prev;
        
          tmp->prev->next = tu;
          tmp->prev = tu;
          
        }
        break;
      }
      else if(tu->esi == tmp->esi) {
        //Duplicated unit
        retval = 1;
        break;
      }
      if(tmp->next == NULL) {
        // Last unit (normal order)

        to->rx_bytes += tu->len;
      

        if(to->rx_bytes > to->len) {
          to->rx_bytes = to->len;    
        }

        tb->nb_of_rx_units++;

        tu->next = tmp->next;
        tu->prev = tmp;
        
        tmp->next = tu;
        break;
      }
      tmp = tmp->next;
    }
  }

  return retval;
}*/

int insert_unit(trans_unit_t *tu, trans_block_t *tb, trans_obj_t *to) {

    trans_unit_t *tmp;
    int retval = 0;

    tmp = tb->unit_list;

    if(tmp == NULL) {

        to->rx_bytes += tu->len;

        // for percentage counter
        if(to->rx_bytes > to->len) {
            to->rx_bytes = to->len;    
        }

        tb->nb_of_rx_units++;

        tb->unit_list = tu;
        return retval;
    }

    for(;;) {
        if(tu->esi < tmp->esi) {
            //Delayed unit

            to->rx_bytes += tu->len;

            if(to->rx_bytes > to->len) {
                to->rx_bytes = to->len;    
            }

            tb->nb_of_rx_units++;

            if(tmp->prev == NULL) {
                tu->next = tmp;
                tu->prev = tmp->prev;

                tmp->prev = tu;

                tb->unit_list = tu;
            }
            else {
                tu->next = tmp;
                tu->prev = tmp->prev;

                tmp->prev->next = tu;
                tmp->prev = tu;

            }
            break;
        }
        else if(tu->esi == tmp->esi) {
            //Duplicated unit
            retval = 1;
            break;
        }
        if(tmp->next == NULL) {
            // Last unit (normal order)

            to->rx_bytes += tu->len;


            if(to->rx_bytes > to->len) {
                to->rx_bytes = to->len;    
            }

            tb->nb_of_rx_units++;

            tu->next = tmp->next;
            tu->prev = tmp;

            tmp->next = tu;
            break;
        }
        tmp = tmp->next;
    }

    return retval;
}

void free_object(trans_obj_t *to, alc_session_t *s, int type) {

#ifndef USE_RETRIEVE_UNIT
  trans_block_t *tb;
  trans_unit_t *tu;
  trans_unit_t *next_tu;
  unsigned int i;
  
  for(i=0; i < to->bs->N; i++) {
    
    tb = to->block_list+i;
    
    next_tu = tb->unit_list;
    
    while(next_tu != NULL) {
      tu = next_tu;
      
      if(tu->data != NULL) {
    free(tu->data);
    tu->data = NULL;
      }
      
      next_tu = tu->next;
      free(tu);
    }
  }
#endif
  
  free(to->bs);
  
  if(to->next != NULL) {
    to->next->prev = to->prev;
  }
  if(to->prev != NULL) {
    to->prev->next = to->next;
  }
  if(((type == 0)&&(to == s->fdt_list))) {
    s->fdt_list = to->next;
  }
  else if(((type == 1)&&(to == s->obj_list))) {
    s->obj_list = to->next;
  }

  if(to->tmp_filename != NULL) {
    free(to->tmp_filename);
    to->tmp_filename = NULL;
    if(type == 1) {
      if(close(to->fd) == -1) {
    printf("close failed, errno: %i\n", errno);
      }
    }
  }
  
  if(to->tmp_st_filename != NULL) {
    if(type == 1) {
      if(close(to->fd_st) == -1) {
    printf("close failed, errno: %i\n", errno);
      }
    }
    remove(to->tmp_st_filename);
    free(to->tmp_st_filename);
    to->tmp_st_filename = NULL;
  }        
  
  if(to->block_list != NULL) {
    free(to->block_list);
  }
  
  free(to);
}

