/** \file alc_hdr.h \brief ALC header 
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.20 $
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

#ifndef _ALC_HDR_H_
#define _ALC_HDR_H_

#include "defines.h"
#include "lct_hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function adds FEC Payload ID header for FEC Encoding IDs 2 and 128 to the FLUTE header.
 *
 * @param def_lct_hdr pointer to the default LCT header structure
 * @param hdrlen current length of the FLUTE header
 * @param sbn source block number
 * @param es_id encoding symbol identifier
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                   Source Block Number                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                   Encoding Symbol ID                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

int add_alc_fpi_2_128(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned int sbn, unsigned int es_id);

/**
 * This function adds FEC Payload ID header for FEC Encoding ID 129 to the FLUTE header.
 *
 * @param def_lct_hdr pointer to the default LCT header structure
 * @param hdrlen current length of the FLUTE header
 * @param sbn source block number
 * @param sbl source block length
 * @param es_id encoding symbol identifier
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                   Source Block Number                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Source Block Length       |      Encoding Symbol ID       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

int add_alc_fpi_129(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned int sbn,
					unsigned short sbl, unsigned short es_id);

/**
 * This function adds FEC Payload ID header for FEC Encoding IDs 0 and 130 to the FLUTE header.
 *
 * @param def_lct_hdr pointer to the default LCT header structure
 * @param hdrlen current length of the FLUTE header
 * @param sbn source block number
 * @param es_id encoding symbol identifier
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Source Block Number       |    Encoding Symbol ID         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

int add_alc_fpi_0_130(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned short sbn, unsigned int es_id);

/**
 * This function adds FEC Payload ID header for FEC Encoding ID 3 to the FLUTE header.
 *
 * @param def_lct_hdr pointer to the default LCT header structure
 * @param hdrlen current length of the FLUTE header
 * @param sbn source block number
 * @param es_id encoding symbol identifier
 * @param m finite field parameter (default for m is 8)
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Source Block Number (32-m bits)| Encoding Symbol ID (m bits)   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

int add_alc_fpi_3(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned int sbn, unsigned int es_id,
		  unsigned char m);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

