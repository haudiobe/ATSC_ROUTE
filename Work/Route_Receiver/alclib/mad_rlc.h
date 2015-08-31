/** \file mad_rlc.h \brief Receiver-driven layered congestion control<br>
 *  <br>Portions of code derived from MCL library by Vincent Roca et al.
 *  (http://www.inrialpes.fr/planete/people/roca/mcl/)<br><br>
 *  Copyright (c) 1999-2004 INRIA - Universite Paris 6 - All rights reserved<br>
 *  main authors: Julien Laboure - julien.laboure@inrialpes.fr and
 *                Vincent Roca - vincent.roca@inrialpes.fr
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.17 $
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

#ifndef _MAD_RLC_H_
#define _MAD_RLC_H_

#include "defines.h"
#include "utils.h"
#include "alc_session.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure for RLC header.
 * @struct rlc_hdr
 */

typedef struct rlc_hdr {

#ifdef  _BIT_FIELDS_LTOH
	unsigned char	reserved:7;		/**< Unused, must be 0x55 (1010101) */
	unsigned char	sp:1;			/**< Is this packet a Synchronisation Point (SP) ? */
#else
	unsigned char	sp:1;			/**< Is this packet a Synchronisation Point (SP) ? */
	unsigned char	reserved:7;		/**< Unused, must be 0x55 (1010101) */
#endif

	unsigned char	layer;			/**< packet's layer */
	unsigned short	seqid;			/**< packet's sequence number (per layer sequence) */

} rlc_hdr_t;

/**
 * Structure for information about late packet.
 * @struct late_list
 */

typedef struct late_list {
	struct late_list *next;	/**< next late packet */
	unsigned short seq_num;	/**< RLC sequence number */
	double losttime;		/**< time when packet is considering lost */
} late_list_t;

/**
 * Structure for information about lost packet.
 * @struct lost_list
 */

typedef struct lost_list {
	struct lost_list *next;		/**< next missing packet */
	int pkt_remaining;			/**< number of packets to receive before we forget this one */
} lost_list_t;

/**
 * RLC control structure.
 * @struct mad_rlc
 */

typedef struct mad_rlc {
	int sp_cycle;		/**< interval between two syncronisation points at layer 0 */
	int pkt_timeout; 	/**< default time to wait for a late packet before assuming it's lost */
	int deaf_period;	/**< time for deaf period after a dropped layer */
	int late_accepted;	/**< if the amount of late packets between two syncronisation points at
						the toplayer is <= RLC_LATE_ACCEPTED then a layer can be added */
	int loss_accepted;	/**< if the amount of lost packets between two syncronisation points at
						the toplayer is	<= RLC_LOSS_ACCEPTED then a layer can be added */

	int loss_limit;		/**< RLC_LOSS_LIMIT / RLC_LOSS_TIMEOUT is the max loss rate for packet.
						If this rate is reached then we should drop the highest layer. */
	int loss_timeout;	/**< RLC_LOSS_LIMIT / RLC_LOSS_TIMEOUT is the max loss rate for packet.
						If this rate is reached then we should drop the highest layer. */
	
	/** value of current sequence number for each layer */
	unsigned short tx_layers_seq[MAX_CHANNELS_IN_SESSION];

	/** time for the next syncronisation point for each layer */
	double tx_next_sp[MAX_CHANNELS_IN_SESSION];

	/** =1 if waiting for the first packet (for each layer) */
	char rx_first_pkt[MAX_CHANNELS_IN_SESSION];

	/** =1 if waiting for the first syncronisation
	point after deaf period (for each layer) */
	char rx_first_sp[MAX_CHANNELS_IN_SESSION];

	/** seq number of the next packet to receive for each layer */
	unsigned short rx_wait_for[MAX_CHANNELS_IN_SESSION];

	/** list of missing sequence numbers for each layer */
	late_list_t	rx_missing[MAX_CHANNELS_IN_SESSION];
	
	unsigned short rx_nblate_since_sp;	/**< amount of late packets since the last syncronisation point */
	unsigned short	rx_nblate;			/**< amount of recent late packets */
	unsigned short	rx_nblost_since_sp;	/**< amount of lost packets since the last syncronisation point */
	unsigned short	rx_nblost;			/**< amount of recent lost packets */
	lost_list_t	rx_lost;				/**< Current list of lost packets */
	double rx_deaf_wait;				/**< when in deaf period, specify deaf period end time */	
	BOOL drop_highest_layer;			/**< if TRUE receiver will drop highest layer */

} mad_rlc_t;

/**
 * This function initializes MAD RLC Congestion Control.
 *
 * @param s pointer to the session
 *
 * @return 0 in success, -1 otherwise
 *
 */

int init_mad_rlc(alc_session_t *s);

/**
 * This function closes MAD RLC Congestion Control.
 *
 * @param s pointer to the session
 *
 */

void close_mad_rlc(alc_session_t *s);

/**
 * This function calculates time for next syncronisation point.
 *
 * @param s pointer to the session
 * @param layer layer number
 *
 * @return time for next syncronisation point
 *
 */

double mad_rlc_next_sp(alc_session_t *s, int layer);

/**
 * This function resets syncronisation points in the sender.
 *
 * @param s pointer to the session
 *
 */

void mad_rlc_reset_tx_sp(alc_session_t *s);

/**
 * This function fills ALC packet's RLC header.
 *
 * @param s pointer to the session
 * @param rlc_hdr pointer to the RLC header
 * @param layer layer number
 *
 * @return 0 in success, -1 otherwise
 *
 */

int mad_rlc_fill_header(alc_session_t *s, rlc_hdr_t *rlc_hdr, int layer);

/**
 * This function analyzes ALC packet's CCI field.
 *
 * @param s pointer to the session
 * @param rlc_hdr pointer to the RLC header		
 *
 * @return 0 in success, -1 otherwise
 *
 */

int mad_rlc_analyze_cci(alc_session_t *s, rlc_hdr_t *rlc_hdr);

#ifdef __cplusplus
}; //extern "C"
#endif


#endif

