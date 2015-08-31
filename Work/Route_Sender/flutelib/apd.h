/** \file apd.h \brief Associated prosedure description
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.13 $
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

#ifndef _APD_H_
#define _APD_H_

#include "flute_defines.h"
#include "../alclib/defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_FILE_REPAIR

/**
 * Structure for service URI.
 * @struct service_URI
 */

typedef struct serviceURI {

  char URI[MAX_HTTP_URL_LENGTH];	/**< service URI */
  struct serviceURI *prev;			/**< previous service URI */
  struct serviceURI *next;			/**< next service URI */

} serviceURI_t;

/**
 * Structure for post file repair information.
 * @struct postFileRepair
 */

typedef struct postFileRepair {

  unsigned int offsetTime;			/**< offsetTime + randomTimePeriod is the time to wait before starting file repair */ 
  unsigned int randomTimePeriod;	/**< offsetTime + randomTimePeriod is the time to wait before starting file repair */ 
  serviceURI_t *serviceURI_List;	/**< service URI list */

} postFileRepair_t;

/**
 * Structure for broadcast multicast file repair information.
 * @struct bmFileRepair
 */

typedef struct bmFileRepair {

  char sessionDescriptionURI[MAX_HTTP_URL_LENGTH];	/**< URI where to fecth file repair session description */

} bmFileRepair_t;

/**
 * Structure for file repair information.
 * @struct apd
 */

typedef struct apd {

  postFileRepair_t *postFileRepair;
  bmFileRepair_t *bmFileRepair;

} apd_t;

/**
 * This function decodes APD XML document to apd structure by using Expat XML library.
 *
 * @param apd_config pointer to buffer containing APD XML document
 *
 * @return pointer to apd structure, NULL in error cases
 *       
 */

apd_t* decode_apd_config(char *apd_config);

/**
 * This function frees apd structure.
 *
 * @param apd pointer to apd structure
 *       
 */

void FreeAPD(apd_t *apd);

#endif

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

