/** \file mad_utf8.c \brief ISO-8859-1 MultiByte Char / UTF-8 Conversion Routines
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.11 $
 *
 *  MAD-FLUTELIB: Implementation of FLUTE protocol.
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

#include "mad_utf8.h"

/* Map from the most-significant 6 bits of the first byte to the total
number of bytes in a UTF-8 character. */

static char UTF8_2_ISO_8859_1_len[] =
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* erroneous */
    2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 6
  };

static char UTF8_2_ISO_8859_1_mask[] = {0x3F, 0x7F, 0x1F, 0x0F, 0x07, 0x03, 0x01};

int x_utf8s_to_iso_8859_1s(char *mbstr, const char *utf8str, size_t count) {
	int res = 0;

	while (*utf8str != '\0') {
		int len = UTF8_2_ISO_8859_1_len[(*utf8str >> 2) & 0x3F];
		unsigned long u = *utf8str & UTF8_2_ISO_8859_1_mask[len];

		/* erroneous */
		if(len == 0) {
			len = 5;
		}

		for(++utf8str; --len > 0 && (*utf8str != '\0'); ++utf8str) {
			/* be sure this is not an unexpected start of a new character */
			if((*utf8str & 0xC0) != 0x80) {
				break;
			}

			u = (u << 6) | (*utf8str & 0x3F);
		}

		if(mbstr != 0 && count != 0) {
			/* be sure there is enough space left in the destination buffer */
			if(res >= (int)count) {
				return res;
			}

			/* add the mapped character to the destination string or '?' (0x1A, SUB) if character
			   can't be represented in ISO-8859-1 */
			*mbstr++ = (u <= 0xFF ? (char)u : '?');
		}
		++res;
	}

	/* add the terminating null character */
	if(mbstr != 0 && count != 0) {
		// be sure there is enough space left in the destination buffer
		if(res >= (int)count) {
			return res;
		}

		*mbstr = 0;
	}

	return res;
}

int x_iso_8859_1s_to_utf8s(char *utf8str, const char *mbstr, size_t count) {
  
	int res = 0;

	/* loop until we reach the end of the mb string */
	for(; *mbstr != '\0'; ++mbstr) {

		/* the character needs no mapping if the highest bit is not set */
		if((*mbstr & 0x80) == 0) {
			if(utf8str != 0 && count != 0) {
				/* be sure there is enough space left in the destination buffer */
				if(res >= (int)count) {
					return res;
				}

				*utf8str++ = *mbstr;
			}
			++res;
		}

		/* otherwise mapping is necessary */
		else {
			if(utf8str != 0 && count != 0) {
				/* be sure there is enough space left in the destination buffer */
				if(res+1 >= (int)count) {
					return res;
				}

				*utf8str++ = (0xC0 | (0x03 & (*mbstr >> 6)));
				*utf8str++ = (0x80 | (0x3F & *mbstr));
			}
			res += 2;
		}
	}

	/* add the terminating null character */
	if(utf8str != 0 && count != 0) {
		/* be sure there is enough space left in the destination buffer */
		if(res >= (int)count) {
			return res;
		}

		*utf8str = 0;
	}

	return res;
}
