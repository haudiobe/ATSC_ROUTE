/** \file mad_zlib.h \brief ZLIB compression
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

#ifndef _MAD_ZLIB_H_
#define _MAD_ZLIB_H_

#include "flute_defines.h"
#include "../alclib/defines.h"

#ifdef USE_ZLIB

/**
 * This function compresses the given file and creates a corresponding ~gz file.
 *
 * @param file file name
 * @param mode compression mode
 *
 * @return 0 in success, -1 otherwise
 *
 */

int file_gzip_compress(char *file, char *mode);

/**
 * This function uncompresses the given file and removes the original.
 *
 * @param file file name
 *
 * @return 0 in success, -1 otherwise
 *
 */

int file_gzip_uncompress(char* file);

/**
 * This function compresses the given buffer.
 *
 * @param buf pointer to the buffer to be compressed
 * @param buflen the length of the buffer to be compressed
 * @param comprlen stores the length of the compressed buffer  
 *
 * @return pointer to the buffer containing compressed data, NULL in error cases
 *
 */

char* buffer_zlib_compress(char *buf, unsigned long long buflen,
						   unsigned long long *comprlen);

/**
 * This function uncompresses the given buffer.
 *
 * @param buf pointer to the buffer to be uncompressed
 * @param buflen the length of the buffer to be uncompressed
 * @param uncomprlen stores the length of the uncompressed buffer
 *
 * @return pointer to the buffer containing uncompressed data, NULL in error cases
 *
 */

char* buffer_zlib_uncompress(char *buf, unsigned long long buflen,
							 unsigned long long *uncomprlen);

/**
 * This function uncompresses the given buffer.
 *
 * @param buf pointer to the buffer to be uncompressed
 * @param buflen the length of the buffer to be uncompressed
 * @param uncomprlen stores the length of the uncompressed buffer
 *
 * @return pointer to the buffer containing uncompressed data, NULL in error cases
 *
 * TODO: not work yet, returns invalid compressed data now.
 */

char* buffer_gzip_uncompress(char *buf, unsigned long long buflen,
							 unsigned long long uncomprlen);

#endif

#endif
