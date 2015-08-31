/** \file mad_utf8.h \brief ISO-8859-1 MultiByte Char / UTF-8 Conversion Routines
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.10 $
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

#ifndef _MAD_UTF8_H_
#define _MAD_UTF8_H_

#include <stddef.h>

#include "flute_defines.h"
#include "../alclib/defines.h"

/**
 * This function converts a UTF-8 string to a ISO-8859-1 multibyte string.
 *
 * @param mbstr stores ISO-8859-1 multibyte string
 * @param utf8str UTF-8 string to be converted
 * @param count number of bytes to be written to the output buffer
 *
 * @return the size of the converted string in bytes (excluding null terminator).
 *
 */

int x_utf8s_to_iso_8859_1s(char *mbstr, const char *utf8str, size_t count);

/**
 * This function converts a ISO-8859-1 multibyte string to a UTF-8 string.
 *
 * @param utf8str stores UTF-8 string
 * @param mbstr ISO-8859-1 multibyte string to be converted
 * @param count number of bytes to be written to the output buffer
 *
 * @return the size of the converted string in bytes (excluding null terminator).
 *
 */

int x_iso_8859_1s_to_utf8s(char *utf8str, const char *mbstr, size_t count);

#endif

