/** \file utils.h \brief Macros and definitions if not defined in the operation system
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.10 $
 *
 *  MAD-ALCLIB: Implementation of ALC/LCT protocols, Compact No-Code FEC,
 *  Simple XOR FEC, Reed-Solomon FEC, and RLC Congestion Control protocol.
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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef min
	/** A macro that returns the minimum of a and b if not defined in Operation System. */
        #define min(a,b)        ((a) <= (b) ? (a) : (b))
#endif

#ifndef max
	/** A macro that returns the maximum of a and b if not defined in Operation System. */
        #define max(a,b)        ((a) >= (b) ? (a) : (b))
#endif

#ifndef BOOL
	/** A type definition for BOOL if not defined in Operation System. */
	typedef int BOOL;
#endif

#ifndef TRUE
	/** Define TRUE if not defined in Operation System. */
	#define TRUE 1
#endif

#ifndef FALSE
	/** Define FALSE if not defined in Operation System. */
	#define FALSE 0
#endif

#ifdef __cplusplus
}; //extern "C"
#endif


#endif

