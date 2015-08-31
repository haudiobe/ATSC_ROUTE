/** \file sdplib.h \brief Needed SDP functions which is not provided by oSIP library
 *
 *  $Author: peltotal $ $Date: 2007/02/27 11:32:01 $ $Revision: 1.18 $
 *
 *  MAD-SDPLIB: Implementation of SDP protocol with FLUTE descriptors.
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

#ifndef _SDPLIB_H_
#define _SDPLIB_H_

#include "sdp_message.h"

/**
 * Structure for source filter.
 * @struct sf
 */

typedef struct sf {
    char *filter_mode;	/**< filter mode ("incl") */
    char *net_type;		/**< network type */
    char *addr_type;	/**< address type */
    char *dest_addr;	/**< destination address */
    char *src_addr;		/**< source address */
} sf_t;

/**
 * Structure for FEC declaration.
 * @struct fec_dec
 */

typedef struct fec_dec {
	unsigned int index;		/**< internal identifier for FEC declaration */	
	short fec_enc_id;		/**< FEC encoding id */
	int fec_inst_id;		/**< FEC instance id */
	struct fec_dec* next;	/**< next declaration */
} fec_dec_t;

/**
 * This function returns wanted attribute from sdp_t structure.
 *
 * @param sdp pointer to sdp_t structure
 * @param attr_name name of wanted attribute
 *	
 * @return pointer to buffer containing value of wanted attribute, NULL in error cases
 *
 */

char* sdp_attr_get(sdp_message_t *sdp, char *attr_name);

/**
 * This function converts string source filter to source filter structure.
 *
 * @param src_filt pointer to buffer containing source filter string
 *
 * @return pointer to source filter structure, NULL in error cases
 *
 */

sf_t* sf_char2struct(char *src_filt);

/*
 * This function frees source filter structure.
 *
 * @param sf pointer to source filter structure
 *
 */

void sf_free(sf_t *sf) ;

/**
 * This function returns FEC declaration structure list from sdp_t structure.
 *
 * @param sdp pointer to sdp_t structure
 *	
 * @return pointer to FEC declaration structure list, NULL in error cases
 *
 */

fec_dec_t* sdp_fec_dec_get(sdp_message_t *sdp);

/**
 * This function converts string FEC declaration to FEC declaration structure.
 *
 * @param fec_dec pointer to buffer containing FEC declaration string
 *
 * @return pointer to FEC declaration structure, NULL in error cases
 *
 */

fec_dec_t* fec_dec_char2struct(char *fec_dec);

/**
 * This function frees FEC declaration structure.
 *
 * @param fec_dec pointer to FEC declaration structure
 *
 */

void fec_dec_free(fec_dec_t *fec_dec);

#endif
