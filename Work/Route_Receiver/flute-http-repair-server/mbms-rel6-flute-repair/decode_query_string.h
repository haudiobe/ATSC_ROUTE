/** \file decode_query_string.h \brief Functions for query string decoding
 *
 * $Author: peltotal $ $Date: 2007/02/27 11:32:01 $ $Revision: 1.7 $
 *
 *  MAD-FLUTE-REPAIR-SERVER: Implementation of repair server for FLUTE protocol.
 *  Copyright (c) 2005-2007 TUT - Tampere University of Technology
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
 */

#ifndef _DECODE_QUERY_STRING_
#define _DECODE_QUERY_STRING_

/**
 * This function decodes query string.
 *
 * @param query_str query string to be decoded
 *
 * @return decoded query string, NULL in error cases
 *
 */


char* decode_query_string(char *query_str);

#endif
