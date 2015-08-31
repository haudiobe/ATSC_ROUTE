/** \file alc_hdr.c  \brief ALC header
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.22 $
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

#include <stdio.h>

#ifdef _MSC_VER
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "alc_hdr.h"

int add_alc_fpi_2_128(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned int sbn, unsigned int es_id) {

	int len = 0;

	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(sbn);
	len += 4;
	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl(es_id);	
	len += 4;
	
	return len;
}

int add_alc_fpi_129(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned int sbn, 
					unsigned short sbl, unsigned short es_id) {

	int len = 0;
	unsigned int word = 0;

	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(sbn);

	len += 4;
	word = ((sbl << 16) | (es_id & 0xFFFF));
	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen + len) = htonl(word);	
	len += 4;

	return len;
}

int add_alc_fpi_0_130(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned short sbn, unsigned int es_id) {

	int len = 0;
	unsigned int word = 0;
	//Malek El Khatib
	//word = ((sbn << 16) | (es_id & 0xFFFF));
	word = (es_id & 0xFFFFFFFF);
	//printf("While sending, we get the following encoding symbols: %u\n",word); //Malek El Khatib 19.08.2014 Commented this
	//End
	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);	
	len += 4;

	return len;
}

int add_alc_fpi_3(def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned int sbn,
		  unsigned int es_id, unsigned char m) {

	int len = 0;
	unsigned int word = 0;

	word = ((sbn << m) | (es_id & ((1 << m) - 1)));

	*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);	
	len += 4;

	return len;
}


