/** \file padding_encoding.h \brief Content padding
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

#ifndef _PADDING_ENCODING_H_
#define _PADDING_ENCODING_H_

#include "flute_defines.h"
#include "../alclib/defines.h"

/**
 * This function calculates the amount of needed padding.
 *
 * @param f_size the size of the file to be padded
 * @param block_len number of encoding symbols per block 
 * @param es_len encoding symbol length in bytes
 *
 * @return the amount of needed padding in bytes, 0 when padding is not needed
 *
 */

int compute_padding_length(unsigned long long f_size, int block_len, int es_len);

/**
 * This function removes padding from the given file and renames file (removes ~pad extension).
 *
 * @param file_name name of the file to be "unpadded"
 * @param content_length length of the file to be "unpadded"
 *
 * @return 0 in success, -1 otherwise
 *
 */

int padding_decoder(char *file_name, unsigned long long content_length);

#endif

