/** \file lct_hdr.h \brief LCT header
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.23 $
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

#ifndef _LCT_HDR_H_
#define _LCT_HDR_H_

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure for the default part of LCT header.
 * @struct def_lct_hdr
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   V   | C | r |S| O |H|T|R|A|B|   HDR_LEN     |       CP      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    Congestion Control Information (CCI, length = 32 bits)     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

typedef struct def_lct_hdr {
#ifdef _BIT_FIELDS_LTOH
  unsigned short flag_b:1;		/**< close session flag */
  unsigned short flag_a:1;		/**< close object flag */
  unsigned short flag_r:1;		/**< expected residual time present flag */ 
  unsigned short flag_t:1;		/**< sender current time present flag */
  unsigned short flag_h:1;		/**< half word flag */
  unsigned short flag_o:2;		/**< transport object identifier flag */
  unsigned short flag_s:1;		/**< transport session identifier flag */
  unsigned short reserved:2;	/**< reserved; must be zero */
  unsigned short flag_c:2;		/**< congestion control flag */
  unsigned short version:4;		/**< LCT version number */
  unsigned char	hdr_len;		/**< total length of LCT header */
  unsigned char	codepoint;		/**< identifier used by payload decoder */
#else
  unsigned short version:4;		/**< LCT version number */
  unsigned short flag_c:2;		/**< congestion control flag */
  unsigned short reserved:2;	/**< reserved; must be zero */
  unsigned short flag_s:1;		/**< transport session identifier flag */
  unsigned short flag_o:2;		/**< transport object identifier flag */
  unsigned short flag_h:1;		/**< half word flag */
  unsigned short flag_t:1;		/**< sender current time present flag */
  unsigned short flag_r:1;		/**< expected residual time present flag */
  unsigned short flag_a:1;		/**< close object flag */
  unsigned short flag_b:1;		/**< close session flag */
  unsigned char	hdr_len;		/**< total length of LCT header */
  unsigned char	codepoint;		/**< identifier used by payload decoder */
#endif
  unsigned int cci;				/**< congestion control header */
} def_lct_hdr_t;

/**
 * This function adds FDT LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param fdt_instance_id FDT instance id
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3 
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 192   |   V   |           FDT Instance ID             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

int add_fdt_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned int fdt_instance_id);

/**
 * This function adds CENC LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param content_enc_algo content encoding algorith used with the FDT instance payload
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 193   |     CENC      |          Reserved             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

int add_cenc_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned char content_enc_algo);

/**
 * This function adds FTI (FEC Encoding IDs 0, 2, 128, 130) LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param transferlen length of the transport object
 * @param fec_inst_id FEC instance id (or reserved/zeroed with ID 0)
 * @param eslen encoding symbol length
 * @param max_sblen maximum source block length
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 64    |     HEL       |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * |                       Transfer Length                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Reserved/FEC Instance ID    |     Encoding Symbol Length    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                  Maximum Source Block Length                  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 */

int add_fti_0_2_128_130_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned long long transferlen,
								unsigned short fec_inst_id, unsigned short eslen, unsigned int max_sblen);
 
/**
 * This function adds FTI (FEC Encoding ID 3) LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param transferlen length of the transport object
 * @param m finite field parameter
 * @param G number of encoding symbols per FLUTE packet
 * @param eslen encoding symbol length
 * @param max_sblen maximum source block length
 * @param mxnbofes maximum number of encoding symbols per block
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 64    |     HEL       |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * |                       Transfer Length                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |       m       |       G       |     Encoding Symbol Length    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  Maximum Source Block Length  | Max. Num. of Encoding Symbols |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 */

int add_fti_3_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned long long transferlen,
					unsigned char m, unsigned char G, unsigned short eslen, unsigned short max_sblen,
					unsigned short mxnbofes);

/*
 * This function adds FTI (FEC Encoding ID 129) LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param transferlen length of the transport object
 * @param fec_inst_id FEC instance id (or reserved/zeroed with ID 0)
 * @param eslen encoding symbol length
 * @param max_sblen maximum source block length
 * @param mxnbofes maximum number of encoding symbols per block
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 64    |     HEL       |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * |                       Transfer Length                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    FEC Instance ID            |     Encoding Symbol Length    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  Maximum Source Block Length  | Max. Num. of Encoding Symbols |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 */

int add_fti_129_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned long long transferlen,
						unsigned short fec_inst_id, unsigned short eslen, unsigned short max_sblen,
						unsigned short mxnbofes);

/**
 * This function adds NOP LCT extension header to FLUTE's header.
 * Not yet implemented.
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |      HET      |      HEL      |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * .                                                               . 
 * .           Header Extension Content (HEC)                      . 
 * .                                                               .
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

int add_nop_lct_he(void);

/*
 * This function adds AUTH LCT extension header to FLUTE's header.
 * Not yet implemented.
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    HET = 0    |      HEL      |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * .                                                               . 
 * .           Header Extension Content (HEC)                      . 
 * .                                                               .
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

int add_auth_lct_he(void);

/**
 * This function adds TIME LCT extension header to FLUTE's header.
 * Not yet implemented.
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    HET = 2    |      HEL      |         Use (bit field)       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       first time value                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * ...            (other time values (optional)                  ...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *                 Use (bit field)
 *
 *                 2                                       3
 *   6   7   8   9   0   1   2   3   4   5   6   7   8   9   0   1
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |SCT|SCT|ERT|SLC|   reserved    |          PI-specific          |
 * |Hi |Low|   |   |    by LCT     |              use              |
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *
 */

int add_time_lct_he(void);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

