/** \file flute_file_repair.h \brief Point-to-multipoint file repair
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

#ifndef _FLUTE_FILE_REPAIR_H_
#define _FLUTE_FILE_REPAIR_H_

#include "../alclib/defines.h"
#include "flute_defines.h"
#include "fdt.h"
#include "flute.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_FILE_REPAIR

/**
 * This function handles PTMP file repair.
 *
 * @param receiver pointer to structure containing receiver information
 * @param a arguments structure where command line arguments are parsed
 * @param sdp_buf buffer containing session description for the repair session
 *
 * @return 0 when state is SClosed, -1 in error cases, -2 when state is SExiting,
 * -3 when state is STxStopped, -4 when MD5 check error occurs, -5 when state is
 * SExiting in start time waiting period
 *
 */

int flute_file_repair(flute_receiver_t *receiver, arguments_t *a, char *sdp_buf);

#endif

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

