/** \file mad_rlc.c \brief Receiver-driven layered congestion control<br>
 *  <br>Portions of code derived from MCL library by Vincent Roca et al.
 *  (http://www.inrialpes.fr/planete/people/roca/mcl/)<br><br>
 *  Copyright (c) 1999-2004 INRIA - Universite Paris 6 - All rights reserved<br>
 *  main authors: Julien Laboure - julien.laboure@inrialpes.fr and
 *                Vincent Roca - vincent.roca@inrialpes.fr
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.24 $
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
#include <string.h>

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "mad_rlc.h"
#include "alc_channel.h"

/**
 * This is a private function which process RLC SP.
 *
 * @param s pointer to the session
 *
 * @return 0 when no new layer is added, 1 when new layeris added, -1 otherwise
 *
 */

int mad_rlc_process_rx_sp(alc_session_t *s) {

  static char addrs[MAX_CHANNELS_IN_SESSION][INET6_ADDRSTRLEN];	/* multicast addresses */
  static char ports[MAX_CHANNELS_IN_SESSION][MAX_PORT_LENGTH];	/* local port numbers  */

  struct sockaddr_in ipv4;
  struct sockaddr_in6 ipv6;

#ifdef _MSC_VER
  int addr_size;
#endif

  int retcode;
  
  int retval = 0;
  
  if(s->rlc->rx_nblost_since_sp <= s->rlc->loss_accepted
     && s->rlc->rx_nblate_since_sp <= s->rlc->late_accepted) {
    
    if(s->addr_family == PF_INET) {
      if(s->addr_type == 1) {
	ipv4.sin_addr.s_addr = htonl(ntohl(inet_addr(s->ch_list[s->nb_channel - 1]->addr)));
      }
      else {
	ipv4.sin_addr.s_addr = htonl(ntohl(inet_addr(s->ch_list[s->nb_channel - 1]->addr)) + 1);
      }
      
      memset(addrs[s->nb_channel], 0, INET6_ADDRSTRLEN);
      sprintf(addrs[s->nb_channel], "%s", inet_ntoa(ipv4.sin_addr));
      
      memset(ports[s->nb_channel], 0, MAX_PORT_LENGTH);
      sprintf(ports[s->nb_channel], "%i", (atoi(s->ch_list[s->nb_channel - 1]->port) + 1));
    }
    else if(s->addr_family == PF_INET6) {
      
#ifdef _MSC_VER
	addr_size = sizeof(struct sockaddr_in6);
	WSAStringToAddress((char*)s->ch_list[s->nb_channel - 1]->addr, AF_INET6, NULL, (struct sockaddr*)&ipv6, &addr_size);
#else 
	inet_pton(AF_INET6, s->ch_list[s->nb_channel - 1]->addr, &ipv6.sin6_addr);
#endif

      if(s->addr_type == 0) {
	if(increase_ipv6_address(&ipv6.sin6_addr) == -1) {
	  printf("Increasing IPv6 address %s is not possible\n", s->ch_list[s->nb_channel - 1]->addr);
	  return -1;
	}
      }
      
      memset(addrs[s->nb_channel], 0, INET6_ADDRSTRLEN);

#ifdef _MSC_VER
	  addr_size = sizeof(addrs[s->nb_channel]);
      WSAAddressToString((struct sockaddr*)&ipv6, sizeof(struct sockaddr_in6),
			NULL, addrs[s->nb_channel], &addr_size);
#else
	  inet_ntop(AF_INET6, &ipv6.sin6_addr, addrs[s->nb_channel], sizeof(addrs[s->nb_channel]));
#endif
      
      memset(ports[s->nb_channel], 0, MAX_PORT_LENGTH);
      sprintf(ports[s->nb_channel], "%i", (atoi(s->ch_list[s->nb_channel - 1]->port) + 1));
    }
    
    retcode = add_alc_channel(s->s_id, ports[s->nb_channel], addrs[s->nb_channel],
			      s->ch_list[s->nb_channel - 1]->intface,
			      s->ch_list[s->nb_channel - 1]->intface_name);
    
    if(!(retcode < 0)) {
      s->rlc->rx_nblost_since_sp = 0;
      s->rlc->rx_nblate_since_sp = 0;
      retval = 1;
    }
  }
  
  return retval;
}

/**
 * This is a private function which free lists.
 *
 * @param s pointer to the session
 *
 */

void mad_rlc_free_lists(alc_session_t *s) {
	int i;
	late_list_t *current_late;
	lost_list_t *current_lost;

	for(i = 0; i < MAX_CHANNELS_IN_SESSION; i++) {	
		current_late = s->rlc->rx_missing[i].next;
		while(current_late != NULL) {
			s->rlc->rx_missing[i].next = current_late->next;
			free(current_late);
			current_late = s->rlc->rx_missing[i].next;
		}
		s->rlc->rx_missing[i].next = NULL; /* redundant, isn't it? */
	}

	current_lost = s->rlc->rx_lost.next;
	while(current_lost != NULL) {
		s->rlc->rx_lost.next = current_lost->next;
		free(current_lost);
		current_lost = s->rlc->rx_lost.next;
	}
	s->rlc->rx_lost.next = NULL; /* redundant, isn't it? */
	s->rlc->rx_nblost = 0;
	s->rlc->rx_nblate = 0;
	s->rlc->rx_nblost_since_sp = 0;
	s->rlc->rx_nblate_since_sp = 0;
}

/**
 * This is a private function which adds late packet to the session's late list.
 *
 * @param s pointer to the session,
 * @param layer layer number
 * @param nseq packet's sequence number
 *
 * @return 0 in success, -1 otherwise
 *
 */

int mad_rlc_add_late(alc_session_t *s, int layer, int nseq) {
	late_list_t	*new_missing;
	double currenttime;

	if(layer > s->nb_channel) {
		return -1;
	}

	if(!( new_missing = (late_list_t *)malloc(sizeof(late_list_t))) ) {
		printf("Could not alloc memory for late_list_t structure!\n");
		return -1;
	}

	currenttime = sec();

	new_missing->seq_num = nseq;
	new_missing->losttime = currenttime + (double)((double)s->rlc->pkt_timeout / (double)1000);
	new_missing->next = s->rlc->rx_missing[layer].next;

	s->rlc->rx_missing[layer].next = new_missing;
	s->rlc->rx_nblate++;
	s->rlc->rx_nblate_since_sp++;
	
	/* update stats: assume a late pkt is lost (corrected later if req.) */
	s->lost_packets++;
		
	return 0;
}

/**
 * This is a private function which removes late packet from session's late list.
 *
 * @param s pointer to the session
 * @param layer layer number
 * @param nseq packet's sequence number
 *
 * @return 0 in success, -1 otherwise
 *
 */

int mad_rlc_remove_late(alc_session_t *s, int layer, int nseq) {
	late_list_t *prev;
	late_list_t *current;

	if(layer > s->nb_channel) {
		return -1;
	}

	prev = &(s->rlc->rx_missing[layer]);
	current = prev->next;
	
	while(current != NULL && current->seq_num != nseq ) {
		prev = current;
		current = current->next;
	}

	if(current == NULL) {
		return -1;
	}
	else {
		prev->next = current->next;
		free(current);
		s->rlc->rx_nblate--;
		s->rlc->rx_nblate_since_sp--;
	}

	/* correct stats... */
	s->lost_packets++;

	return 0;
}


/**
 * This is a private function which adds lost packet to session's lost list.
 *
 * @param s pointer to the session
 *
 * @return -1000 when layer should be dropped, 0 when layer should not be dropped, -1 otherwise
 *
 */

int mad_rlc_add_lost(alc_session_t *s) {
	
	lost_list_t	*new_lost;
	double currenttime;

	currenttime = sec();

	s->rlc->rx_nblost++;
	s->rlc->rx_nblost_since_sp++;
	
	if(s->rlc->rx_nblost >= s->rlc->loss_limit) {
		/* too many losses... drop a layer! */
		
		s->rlc->drop_highest_layer = TRUE;

		/* initial state for future potential use of this layer*/
		s->rlc->rx_first_pkt[s->ch_list[s->nb_channel - 1]->ch_id] = 1;

		/* Clean up late and lost lists... */
		mad_rlc_free_lists(s);

		/* entering the deaf period... */
		s->rlc->rx_deaf_wait = currenttime + (double)s->rlc->deaf_period / (double)1000;

		return -1000;
	}

	if(!(new_lost = (lost_list_t *)malloc(sizeof(lost_list_t)))) {
		printf("Could not alloc memory for lost_list_t structure!\n");
		return -1;
	}

	new_lost->pkt_remaining = s->rlc->loss_timeout;
	new_lost->next = s->rlc->rx_lost.next;
	s->rlc->rx_lost.next = new_lost;
	
	return 0;
}

/**
 * This is a private function which updates session's late list.
 *
 * @param s pointer to the session
 *
 * @return 0 in success, -1 otherwise
 *
 */

int mad_rlc_update_late_list(alc_session_t *s) {
	int layer;
	late_list_t *prev;
	late_list_t *current;
	double currenttime;

	for(layer = 0; layer < s->nb_channel; layer++) {
		currenttime = sec();

		prev = &(s->rlc->rx_missing[layer]);
		current = prev->next;
		
		while(current != NULL) {
			if(current->losttime <= currenttime) {
				/* this one is lost! */
				prev->next = current->next;
				free(current);
				s->rlc->rx_nblate--;
				s->rlc->rx_nblate_since_sp--;
				
				if(mad_rlc_add_lost(s) == -1000) {
					return -1; /* roca 0 */
				}
			}
			else {
				prev = current;
				current = prev->next;
			}
		}
	}

	return 0;
}

/**
 * This is a private function which updates session's lost list.
 *
 * @param s pointer to the session
 *
 * @return 0
 *
 */

int mad_rlc_update_loss_list(alc_session_t *s) {
	lost_list_t *prev;
	lost_list_t *current;

	prev = &(s->rlc->rx_lost);
	current = prev->next;

	while(current != NULL) { 	
		if(--(current->pkt_remaining) == 0) { /* this loss is too old : removing */		
			prev->next = current->next;
			free(current);
			s->rlc->rx_nblost--;
		}
		else {
			prev = current;
		}

		current = prev->next;
	}

	return 0;
}

/**
 * This is a private function which checks packet's RLC sequence number.
 *
 * @param s pointer to the session
 * @param layer layer number
 * @param seqid packet's sequence number
 *
 * @return 0
 *
 */

int mad_rlc_check_sequence(alc_session_t *s, int layer, int seqid) {
	unsigned short Delta1;
	unsigned short Delta2;

	int i;
	int late_limit;
	
	if(s->rlc->rx_wait_for[layer] == seqid) {
		/* This is the packet we're waiting for, let's go on! */
		s->rlc->rx_wait_for[layer]++;
		return 0;
	}

	/* This is not the one we're waiting for... */
	if(s->rlc->rx_wait_for[layer] < seqid) {

		Delta1 = seqid - s->rlc->rx_wait_for[layer];
		Delta2 = 65535 - Delta1;
		
		if(Delta1 < Delta2) {
			
			late_limit = RLC_MAX_LATES;
			
			for(i = s->rlc->rx_wait_for[layer]; i < seqid; i++) {
				mad_rlc_add_late(s, layer, i);
				late_limit--;
				if(late_limit <= 0 ) {
					printf("RLC Warning*** Max number of LATE packets reached\n");
					fflush(stdout);
					break;
				}
			}
			/* EOFIX */
			s->rlc->rx_wait_for[layer] = seqid + 1;
		}
		else {
			/* Late arrival packet (uint16 overflow) */
			/* eg. we're waiting seq 4 and we get seq 65532 */
			mad_rlc_remove_late(s, layer, seqid);
		}
	}
	else { /* rx_wait_for > rlc_seqid */
		
		Delta1 = s->rlc->rx_wait_for[layer] - seqid;
		Delta2 = 65535 - Delta1;
		if (Delta1 < Delta2) { /* Late arrival packet */
		/* ie: we're waiting seq 501 and we get seq 498 */
			mad_rlc_remove_late(s, layer, seqid);
		}
		else {
		/* Some packet(s) are missing (uint16 overflow) */
		/* ie: waiting seq 65531 and get seq 3 */
			/* FIX 14/12/01 */
			late_limit = RLC_MAX_LATES;
			for(i = s->rlc->rx_wait_for[layer]; i != (seqid-1); i++) {
				mad_rlc_add_late(s, layer, i);
				late_limit--;
				if(late_limit <= 0 ) {
					printf("RLC Warning*** Max number of LATE packets reached\n");
					fflush(stdout);
					break;
				}
			}
			/* EOFIX */
			s->rlc->rx_wait_for[layer] = seqid + 1;
		}
	}

	return 0;
}

int init_mad_rlc(alc_session_t *s) {
	int i;

	if(!(s->rlc = (mad_rlc_t*)calloc(1, sizeof(mad_rlc_t)))) {
		printf("Could not alloc memory for mad rlc congestion control!\n");
		return -1;
	}

	if(s->mode == SENDER) {
		s->rlc->sp_cycle  =	RLC_SP_CYCLE;
	}
	else if(s->mode == RECEIVER) {
		s->rlc->drop_highest_layer = FALSE;
		s->rlc->pkt_timeout = RLC_PKT_TIMEOUT;
		s->rlc->deaf_period = RLC_DEAF_PERIOD;
		s->rlc->loss_accepted =	RLC_LOSS_ACCEPTED;
		s->rlc->late_accepted =	RLC_LATE_ACCEPTED;
		s->rlc->loss_limit  = RLC_LOSS_LIMIT;
		s->rlc->loss_timeout = RLC_LOSS_TIMEOUT;
		s->rlc->rx_deaf_wait = 0;

		for(i = 0 ; i < MAX_CHANNELS_IN_SESSION; i++) {
			s->rlc->rx_first_pkt[i] = 1;
			s->rlc->rx_first_sp[i] = 0;
		}
	}
	
	return 0;	
}

void close_mad_rlc(alc_session_t *s) {
	if(s->mode == RECEIVER) {
		mad_rlc_free_lists(s);
	}

	free(s->rlc);
}

double mad_rlc_next_sp(alc_session_t *s, int layer) {
	
	double spacing;
	double currenttime;

	currenttime = sec();

	spacing = (double)((double)s->rlc->sp_cycle / (double)1000) * (double)(min((1 << layer), 128 + layer));

	return (currenttime + (double)spacing);
}

void mad_rlc_reset_tx_sp(alc_session_t *s) {
	
	int i;

	for(i = 0 ; i < MAX_CHANNELS_IN_SESSION; i++) {
		s->rlc->tx_next_sp[i] = mad_rlc_next_sp(s, i);
	}
}

int mad_rlc_fill_header(alc_session_t *s, rlc_hdr_t *rlc_hdr, int layer) {
	
	unsigned short hdr_seqid;
	double currenttime;

	currenttime = sec();

	if(layer > MAX_CHANNELS_IN_SESSION || layer > s->nb_channel) {
		printf("BAD layer!\n");
		fflush(stdout);
		return -1;
	}	

	hdr_seqid = s->rlc->tx_layers_seq[layer];
	s->rlc->tx_layers_seq[layer]++;
		
	rlc_hdr->reserved = 0x55;	/* unused: rlc_reserved = 1010101 */
	rlc_hdr->layer = layer;
	rlc_hdr->seqid = htons(hdr_seqid);

	if(s->rlc->tx_next_sp[layer] <= currenttime) {

		if((layer + 1) == s->nb_channel) {
			rlc_hdr->sp = 0;	
		}
		else {
			/* OK this is a new SP for this layer */
			rlc_hdr->sp = 1;

			/* calculate when next SP will occur */
			s->rlc->tx_next_sp[layer] = mad_rlc_next_sp(s, layer);

			if(layer + 1 < MAX_CHANNELS_IN_SESSION &&
				s->ch_list[layer + 1]->start_sending == FALSE) {

				s->ch_list[layer + 1]->start_sending = TRUE;
				s->nb_sending_channel++;
			}
		}
	}
	else {	/* not a SP... */
		rlc_hdr->sp = 0;
	}

	return 0;
}

int mad_rlc_analyze_cci(alc_session_t *s, rlc_hdr_t *rlc_hdr) {
	
	unsigned char hdr_layer;	
	unsigned short hdr_seqid;
	unsigned char hdr_sp;
	double currenttime;

	hdr_seqid = ntohs(rlc_hdr->seqid);
	hdr_layer = rlc_hdr->layer;
	hdr_sp = rlc_hdr->sp;

	if(rlc_hdr->reserved != 0x55) {
		return -1;
	}
	if(rlc_hdr->layer >= s->nb_channel) {
		return -1;
	}

	mad_rlc_update_loss_list(s);
	mad_rlc_update_late_list(s);

	currenttime = sec();

	if(s->rlc->rx_deaf_wait != 0) {
		
		if(s->rlc->rx_deaf_wait > currenttime) {

			s->rlc->rx_wait_for[rlc_hdr->layer] = hdr_seqid + 1;
			return 0;
		}
	}

	if(s->rlc->rx_first_pkt[hdr_layer]) {
		/* First packet on this layer... */
		s->rlc->rx_wait_for[hdr_layer] = hdr_seqid + 1;
		s->rlc->rx_first_pkt[hdr_layer] = 0;
		/* continue as this pkt may contain an SP */
	} else {
		/* check if some pkts have been lost or not */
		mad_rlc_check_sequence(s, hdr_layer, hdr_seqid);
	}

	if(hdr_sp) {
		if(hdr_layer == s->nb_channel - 1) {
			/* This is a Synchronisation Point */
			if(s->rlc->rx_first_sp[hdr_layer] == 1) {
				/* skip 1st SP, especially after deaf-period */
				s->rlc->rx_first_sp[hdr_layer] = 0;
			} else {
				mad_rlc_process_rx_sp(s);
			}
		}
	}

	return 0;
}
