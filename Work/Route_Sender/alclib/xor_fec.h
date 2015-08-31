/** \file xor_fec.h \brief Simple XOR FEC
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.12 $
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

#ifndef _XOR_FEC_H_
#define _XOR_FEC_H_

#include "defines.h"
#include "transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function encodes source block data to transport block using Simple XOR-FEC.
 *
 * @param data pointer to data string to be segmented
 * @param len length of data string
 * @param sbn source block number
 * @param es_len encoding symbol length
 *
 * @return pointer to transport block, NULL in error cases
 *
 */

trans_block_t* xor_fec_encode_src_block(char *data, unsigned long long len, unsigned int sbn,
										unsigned short es_len);

/**
 * This function decodes source block data to buffer using Simple XOR-FEC.
 *
 * @param tr_block pointer to source block
 * @param block_len stores length of block
 * @param es_len encoding symbol length for this block
 *
 * @return pointer to buffer which contains block's data, NULL when memory could
 * not be allocated
 *
 */

char *xor_fec_decode_src_block(trans_block_t *tr_block, unsigned long long *block_len,
							   unsigned short es_len);

/**
 * This function decodes object to buffer using Simple XOR-FEC.
 *
 * @param to pointer to object
 * @param data_len stores the length of object
 * @param s pointer to the session
 *
 * @return pointer to the buffer which contains object's data, NULL when memory
 * could not be allocated
 *
 */

char *xor_fec_decode_object(trans_obj_t *to, unsigned long long *data_len,
							alc_session_t *s);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

