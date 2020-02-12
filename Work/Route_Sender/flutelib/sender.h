/** \file sender.h \brief FLUTE sender
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.19 $
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

#ifndef _SENDER_H_
#define _SENDER_H_

#include "../alclib/defines.h"
#include "flute_defines.h"
#include "parse_args.h"
#include "flute.h"

//Malek El Khatib 14.05.2014
//START 
//These are defined in parse_args.c
//extern unsigned long video_segment; /**< Video Segment Duration. Populated with value entered as argument -y:value >*/
//extern unsigned long audio_segment; /**< Audio Segment Duration. Populated with value entered as argument -Y:value >*/

extern char* sendingTimesFile;     /**<If no file is passed using -y:value, no delays would be introduced while sending segments>*/
extern unsigned short numEncSymbPerPacket; /**< The number of symbols per packet. Populated with value entered as argument -Y:value. If zero, use input file to determine chunks to send >*/

//This is defined in parse_args.c;
extern  FILE* logFilePtr;
//extern char* logFile;
extern BOOL sendFDTAfterObj;
//END

/**
 * This function is flute sender's fdt based sending function.
 *
 * @param a pointer to arguments structure where command line arguments are parsed
 * @param sender structure containing sender information
 *
 * @return 0 in success, -1 in error cases, -2 when state is SExiting
 *
 */

int sender_in_fdt_based_mode(arguments_t *a, flute_sender_t *sender);

#endif

