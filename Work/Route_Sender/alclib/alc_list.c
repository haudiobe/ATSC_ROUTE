/** \file alc_list.c \brief Receive packets to the list
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.15 $
 *
*   MAD-ALCLIB: Implementation of ALC/LCT protocols, Compact No-Code FEC,
*   Simple XOR FEC, Reed-Solomon FEC, and RLC Congestion Control protocol.
*   Copyright (c) 2003-2007 TUT - Tampere University of Technology
*   main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#ifdef _MSC_VER
#else
#include <sys/time.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "alc_list.h"

#ifdef _MSC_VER

/**
 * This is a private function, which returns time value.
 *
 * @param tv stores time value
 *
 * @return 0
 *
 */

int get_time_of_day(struct timeval *tv) {
  time_t tempo_t;
  time(&tempo_t);
  
  tv->tv_sec = tempo_t;
  tv->tv_usec = 0;
  
  return 0;
}
#else

/**
 * This is a private function, which returns time value.
 *
 * @param tv stores time value
 *
 * @return 0 in success, -1 otherwise
 *
 */

int get_time_of_day(struct timeval *tv){
  return gettimeofday(tv, NULL);
}

#endif

/**
 * This is a private function, which locks the list.
 *
 * @param a_list the list to be locked
 *
 */

void lock_list(alc_list_t *a_list) {
#ifdef _MSC_VER
  EnterCriticalSection(&(a_list->session_variables_semaphore));
#else
  pthread_mutex_lock(&(a_list->session_variables_semaphore));
#endif
}

/**
 * This is a private function, which unlocks the list.
 *
 * @param a_list the list to be unlocked
 *
 */

void unlock_list(alc_list_t *a_list) {
#ifdef _MSC_VER
  LeaveCriticalSection(&(a_list->session_variables_semaphore));
#else
  pthread_mutex_unlock(&(a_list->session_variables_semaphore));
#endif
}

/**
 * This is a private function, which checks if the list is empty.
 *
 * @param a_list the list to be checked
*
 * @return 1 if the list is empty, 0 if not
*
*/

int is_empty_private(const alc_list_t *a_list) {
	return a_list->first_elem == NULL;
}

/**
* This is a private function, which prepares the node to be inserted.
*
* @param a_data the container to be inserted in list
*
* @return the node to be inserted, NULL if the container is too old
*
*/

struct alc_list_node* prepare_insert(alc_rcv_container_t *a_data) {
  struct alc_list_node *my_node;
  struct timeval now;
  double container_age;

  assert(a_data != NULL);
  
  if(a_data->time_stamp.tv_sec == 0) {
    /* mark the timestamp first time the container is inserted */
    get_time_of_day(&(a_data->time_stamp));
    assert(a_data->time_stamp.tv_sec != 0);
  }
  else {
    /* check if the container is too old, in case I discard it */
    get_time_of_day(&now);
    
    container_age =  now.tv_sec  - a_data->time_stamp.tv_sec + 
      (now.tv_usec - a_data->time_stamp.tv_usec ) / 1E6;
    assert(container_age >= 0);

    if(container_age > 3) {
      /* containers older than 3 secs are discarded */
      return NULL;
    } 
  }
  
  my_node = malloc(1 * sizeof(struct alc_list_node));
  my_node->next = NULL;
  my_node->data = a_data;
  
  return my_node;
} 

void push_back(alc_list_t *a_list, alc_rcv_container_t *a_data) {
  struct alc_list_node *my_node;
  
  my_node = prepare_insert(a_data);
  
  if(my_node == NULL) {
    return;
  }
  
  lock_list(a_list);
  
  if(is_empty_private(a_list)) {
    a_list->first_elem = my_node;
    a_list->last_elem = my_node;
  }
  else {
    a_list->last_elem->next = my_node;
    a_list->last_elem = my_node;
  }
  
  unlock_list(a_list);
}

void push_front(alc_list_t *a_list, alc_rcv_container_t *a_data) {
  struct alc_list_node *my_node;
  
  my_node = prepare_insert(a_data);
  
  if(my_node == NULL) {
    return;
  }
  
  lock_list(a_list);
  
  if(is_empty_private(a_list)) {
    a_list->first_elem = my_node;
    a_list->last_elem = my_node;
  }
  else {
    my_node->next = a_list->first_elem;
    a_list->first_elem = my_node;
  }
  
  unlock_list(a_list);
}

alc_rcv_container_t* pop_front(alc_list_t *a_list) {
  struct alc_list_node *my_node = NULL;
  void *my_ret = NULL;
  
  lock_list(a_list);
  
  if(is_empty_private(a_list)) {
  }
  else {
    my_node = a_list->first_elem;
    
    if(a_list->first_elem == a_list->last_elem) {  /* last element in list */
      assert(my_node->next == NULL);
      
      a_list->first_elem = NULL;
      a_list->last_elem = NULL;
    }
    else {
      a_list->first_elem = my_node->next;
    }
    
    my_ret = my_node->data;
  }
  
  unlock_list(a_list);
  free(my_node);
  
  return my_ret;
}

int is_empty(const alc_list_t *a_list) {
  int my_ret;
  
  lock_list((alc_list_t*)a_list);
  
  my_ret = (a_list->first_elem == NULL);
  
	unlock_list((alc_list_t*)a_list);
	
	return my_ret;
}

alc_list_t* build_list(void) {
  alc_list_t *my_list;
  
  my_list = calloc(1, sizeof(alc_list_t));
  
#ifdef _MSC_VER
  InitializeCriticalSection(&(my_list->session_variables_semaphore));
#else
  my_list->session_variables_semaphore = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
#endif
  
  return my_list;
}

void destroy_list(alc_list_t *a_list) {
  struct alc_list_node *my_node = NULL;
  struct alc_list_node *my_currentnode = NULL;
  
  if(a_list == NULL) {
    return;
  }
  
  lock_list(a_list);
  
  for(my_node = a_list->first_elem; my_node != NULL;) {
    my_currentnode = my_node;
    my_node = my_node->next;
    
    free(my_currentnode->data);
    free(my_currentnode);
  }
  
  unlock_list(a_list);
  
#ifdef _MSC_VER
  DeleteCriticalSection(&(a_list->session_variables_semaphore));
#else
#endif
  
  free(a_list);
}
