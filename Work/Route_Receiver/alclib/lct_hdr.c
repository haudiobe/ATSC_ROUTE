/** \file lct_hdr.c \brief LCT header
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.23 $
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

#ifdef _MSC_VER
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "lct_hdr.h"

int add_fdt_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned int fdt_instance_id) {

	unsigned int word;
	int len = 0;

	word = ((EXT_FDT << 24) | ((FLUTE_VERSION & 0xF) << 20) | (fdt_instance_id & 0x000FFFFF));
	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
	len += 4;

	return len;

}

int add_cenc_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned char content_enc_algo) {

        unsigned int word;
        int len = 0;

        word = ((EXT_CENC << 24) | (content_enc_algo << 16) | (0 & 0x0000FFFF));
        *(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
        len += 4;

        return len;
}

int add_fti_0_2_128_130_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen,
							unsigned long long transferlen,
							unsigned short fec_inst_id, unsigned short eslen, unsigned int max_sblen) {

	unsigned int word;
	unsigned short tmp;
	int len = 0;
	
	tmp = ((unsigned int)(transferlen >> 32) & 0x0000FFFF);
	word = ((EXT_FTI << 24) | (4 << 16) | tmp);
	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
	len += 4;

	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl((unsigned int)transferlen);
	len += 4;

	word = ((fec_inst_id << 16) | eslen);
	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl(word);
	len += 4;

	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl(max_sblen);
	len += 4;
	
	return len;
}

int add_fti_3_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen,
					   unsigned long long transferlen,
					   unsigned char m, unsigned char G, unsigned short eslen,
					   unsigned short max_sblen, unsigned short mxnbofes) {

  unsigned int word;
  unsigned short tmp;
  int len = 0;
  
  tmp = ((unsigned int)(transferlen >> 32) & 0x0000FFFF);
  word = ((EXT_FTI << 24) | (4 << 16) | tmp);
  *(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
  len += 4;
  
  *(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl((unsigned int)transferlen);
  len += 4;
  
  word = ((m << 24) | (G << 16) | eslen);
  *(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl(word);
  len += 4;
  
  word = ((max_sblen << 16) | mxnbofes);
  *(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl(word);
  len += 4;
  
  return len;
}

int add_fti_129_lct_he(def_lct_hdr_t *def_lct_hdr, int hdrlen,
					 unsigned long long transferlen,
					 unsigned short fec_inst_id, unsigned short eslen,
					 unsigned short max_sblen, unsigned short mxnbofes) {

	unsigned int word;
	unsigned short tmp;
	int len = 0;
	
	tmp = ((unsigned int)(transferlen >> 32) & 0x0000FFFF);
	word = ((EXT_FTI << 24) | (4 << 16) | tmp);
	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
	len += 4;

	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl((unsigned int)transferlen);
	len += 4;

	word = ((fec_inst_id << 16) | eslen);
	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl(word);
	len += 4;

	word = ((max_sblen << 16) | mxnbofes);
	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl(word);
	len += 4;
	
	return len;
}

int add_nop_lct_he(void) {
	int len = 0;
	/* TODO */
	return len;
}

int add_auth_lct_he(void) {
	int len = 0;
	/* TODO */
	return len;
}

int add_time_lct_he(void) {
	int len = 0;
	/* TODO */
	return len;
}
