/** \file alc_list.h \brief Receive packets to the list
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.10 $
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

#ifndef _ALCLIST_H_
#define _ALCLIST_H_

#ifdef _MSC_VER
#include <winsock2.h>
#else
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#endif

#include "defines.h"

/**
 * Container which stores received packet and its information.
 * @struct alc_rcv_container
 */

typedef struct alc_rcv_container {
    char recvbuf[MAX_PACKET_LENGTH]; /**< buffer for the received data */ 
    struct sockaddr_storage from;	/**< information about sender of the packet */ 

#ifdef _MSC_VER
    int fromlen;					/**< the actual length of from */ 
#else
    socklen_t fromlen;				/**< the actual length of from */ 
#endif

    int recvlen;					/**< the length of received data */ 

    struct timeval time_stamp;
} alc_rcv_container_t;

/**
 * List item for received packet.
 * @struct alc_list_node
 */

typedef struct alc_list_node {
    struct alc_list_node  *next; /**< next item in the list*/ 
    void  *data;				/**< pointer to the stored data (alc_rcv_container) */ 
} alc_list_node_t;

/**
 * List for received packets.
 * @struct alc_list
 */

typedef struct alc_list {
    struct alc_list_node  *first_elem; /**< first item in the list */ 
    struct alc_list_node  *last_elem;  /**< last item in the list */ 

#ifdef _MSC_VER
    RTL_CRITICAL_SECTION session_variables_semaphore; /**< used when the list is locked/unlocked */ 
#else
    pthread_mutex_t session_variables_semaphore;      /**< used when the list is locked/unlocked */ 
#endif

} alc_list_t;

/**
 * This function inserts the data to the end of the list.
 *
 * @param a_list the list
 * @param a_data the data
 *
 */

void push_back(alc_list_t *a_list, alc_rcv_container_t *a_data);

/**
 * This function inserts the data to the beginning of the list.
 *
 * @param a_list the list
 * @param a_data the data
 *
 */

void push_front(alc_list_t *a_list, alc_rcv_container_t *a_data);

/**
 * This function returns the data from the beginning of the list.
 *
 * @param a_list the list
 *
 * @return pointer to the data, NULL if the list is empty
 *
 */

alc_rcv_container_t *pop_front(alc_list_t *a_list);

/**
 * This function checks if the list is empty.
 *
 * @param a_list the list
 *  
 * @return 1 if the list is empty, 0 if not
 *
 */

int is_empty(const alc_list_t *a_list);

/**
 * This function creates new list.
 *
 * @return pointer to the created list, NULL in error cases.
 *
 */

alc_list_t* build_list(void);

/**
 * This function destroy the list.
 *
 * @param a_list the list
 *
 */

void destroy_list(alc_list_t *a_list);

#endif
