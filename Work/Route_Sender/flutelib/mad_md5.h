/** \file mad_md5.h \brief MD5 check
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

#ifndef _MAD_MD5_H_
#define _MAD_MD5_H_

#include "flute_defines.h"
#include "../alclib/defines.h"

#ifdef USE_OPENSSL

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function calculates MD5 for file.
 *
 * @param filename pointer to buffer containg file name
 *
 * @return pointer to buffer containing file's MD5, NULL in error cases
 *
 */

char* file_md5(const char *filename);

/**
 * This function calculates MD5 for buffer.
 *
 * @param buffer pointer to buffer containg data
 * @param length buffer length
 *
 * @return pointer to buffer containing data's MD5, NULL in error cases
 *
 */

char* buffer_md5(char *buffer, unsigned long long length);

#endif

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

