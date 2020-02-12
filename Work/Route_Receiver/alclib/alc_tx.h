/** \file alc_tx.h \brief ALC level sending
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.28 $
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

#ifndef _ALC_TX_H_
#define _ALC_TX_H_

#include "defines.h"

//Malek El Khatib 11.08.2014
#include "../flutelib/parse_args.h"
extern unsigned short numEncSymbPerPacket; /*<This is configurable in the program parameters>*/
//End

#ifdef __cplusplus
extern "C" {
#endif


/**
 * This function sends source block from object to the channel or transmission queue.
 *
 * @param s_id session identifier
 * @param tx_mode transmission mode (NO_TX_THREAD or TX_THREAD)
 * @param buf pointer to data to be sent
 * @param buf_len lenght of data to be sent
 * @param toi transport object identifier
 * @param transfer_len length of transport object
 * @param es_len encoding symbol length
 * @param max_sb_len maximum source block length
 * @param sbn source block number
 * @param fec_enc_id: FEC encoding id
 * @param fec_inst_id: FEC instance id
 *
 * Return:  int: number of sent bytes after succesfull sending, -1 -2 otherwise
 *
 */

int alc_send(int s_id, int tx_mode, char *buf, int buf_len,
       unsigned long long toi, unsigned long long transfer_len,
       unsigned short es_len, unsigned int max_sb_len, unsigned int sbn, 
       unsigned char fec_enc_id, unsigned short fec_inst_id);

/**
 * This function sends A flag packet to channel.
 *
 * @param s_id session identifier
 *
 * @return 0 in success, -1 otherwise
 *
 */

int send_session_close_packet(int s_id);

/** 
 * This function sends packets to the all channels in the session when TX_THREAD mode is used.
 * 
 * @param s pointer to the session  
 *
 */

void* tx_thread(void *s);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

