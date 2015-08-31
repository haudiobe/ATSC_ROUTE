/** \file alc_channel.h \brief ALC channel
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.34 $
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

#ifndef _ALC_CHANNEL_H_
#define _ALC_CHANNEL_H_

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include "defines.h"
#include "utils.h"
#include "alc_session.h"

#ifdef SSM
#include "linux_ssm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Structure for the ALC channel
 * @struct alc_channel
 */

typedef struct alc_channel {
	
	int ch_id;							/**< session level identifier for the channel */
	struct alc_session *s;				/**< pointer to the parent session */

	const char *port;					/**< channel's port */ 
	const char *addr;					/**< channel's address */
	const char *intface;				/**< channel's interface */
	const char *intface_name;			/**< the name of channel's interface */
	
	int tx_rate;						/**< transmission rate in kbit/s on this channel */
	int nb_tx_units;					/**< number of sent units per one loop */
	BOOL start_sending;					/**< start sending when TRUE in RLC CC */
	BOOL ready;							/**< channel ready when TRUE */
	int wait_after_sp;					/**< wait this number of loops before start sending in RLC CC */

	BOOL previous_lost;					/**< is previous packet lost (with -P option) */

#ifdef _MSC_VER
	SOCKET	rx_sock;					/**< receiving socket */
	SOCKET	tx_sock;					/**< transmitting socket */	
#else
	int	rx_sock;						/**< receiving socket */
	int	tx_sock;						/**< transmitting socket */	
#endif

#ifdef SSM
	struct ip_mreq_source source_imr;	/**< for SSM join/leave */

#ifdef LINUX
	struct group_source_req greqs;		/**< for MLDv2 SSM join/leave */
#endif

#endif

	struct ip_mreq imr;					/**< for join/leave */
	struct ipv6_mreq imr6;				/**< for IPv6 join/leave*/

	struct sockaddr_in remote;			/**< remote multicast address */
	struct sockaddr_in6 remote6;		/**< remote IPv6 multicast address */
	struct addrinfo *addrinfo;			/**< structure which provides hints concerning the type of socket */
	struct tx_queue_struct *queue_ptr;  /**< list which stores packets to be sent */

	struct alc_list *receiving_list;    /**< list which stores received packets */

#ifdef _MSC_VER
    HANDLE handle_rx_socket_thread;     /**< handle to thread which receives packets from the socket */
    unsigned int rx_socket_thread_id;   /**< identifier for thread which receives packets from the socket */
#else
    pthread_t rx_socket_thread_id;       /**< identifier for thread which receives packets from the socket */
#endif
} alc_channel_t;

/**
 * This function creates and initializes a new channel.
 *
 * @param ch pointer to the channel structure
 * @param s pointer to the parent session structure
 * @param port pointer to the port string
 * @param addr pointer to the address string
 * @param intface pointer to the interface string
 * @param intface_name pointer to the interface name string
 * @param tx_rate transmission rate
 *
 * @return identifier for created channel in success, -1 in error cases
 *
 */

int open_alc_channel(alc_channel_t *ch, alc_session_t *s, const char *port,
					 const char *addr, const char *intface, const char *intface_name, int tx_rate); 

/**
 * This function closes existing channel.
 *
 * @param ch pointer to the channel structure to be closed
 * @param s pointer to the parent session structure
 *
 * @return 0 when closing was successful, -1 in error cases
 *
 */

int close_alc_channel(alc_channel_t *ch, alc_session_t *s);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

