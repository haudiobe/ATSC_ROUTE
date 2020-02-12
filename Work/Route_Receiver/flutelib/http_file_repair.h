/** \file http_file_repair.h \brief Point-to-point file repair
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.15 $
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

#ifndef _HTTP_FILE_REPAIR_H_
#define _HTTP_FILE_REPAIR_H_

#include "../alclib/defines.h"
#include "flute_defines.h"
#include "flute.h"

#ifdef USE_FILE_REPAIR

#include <curl/curl.h>
// Malek El Khatib 28/03/2014
// This was commented to compile source correctly
// #include <curl/types.h>
#include <curl/easy.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure for received data.
 * @struct chunk
 */

typedef struct chunk {
  char *data;
  size_t size;
} chunk_t;

/**
 * This function writes received data to the buffer.
 *
 * @param ptr pointer to the received data (it will not be zero terminated)
 * @param size the size of the data is size multiplied with nmemb
 * @param nmemb the size of the data is size multiplied with nmemb
 * @param a stores recveived data
 *
 * @return number of bytes written
 *
 */

size_t write_to_buffer(void *ptr, size_t size, size_t nmemb, void *a);

/**
 * This function handles PTP file repair.
 *
 * @param receiver pointer to structure containing receiver information
 * @param openfile open received file automatically
 * @param retval stores return value
 * @param curl libcurl handle
 * @param serviceURI post file repair serviceURI
 *
 * @return session description for the PTMP repair session if 302 response received, NULL otherwise
 *
 */

char* http_file_repair(flute_receiver_t *receiver, int openfile, int *retval, CURL *curl,
             char *serviceURI);

#endif

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

