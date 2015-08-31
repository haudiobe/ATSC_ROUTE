/** \file blocking_alg.h \brief \brief Algorithm for computing source block structure
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.21 $
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

#ifndef _BLOCKING_ALG_H_
#define _BLOCKING_ALG_H_

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure for the blocking information.
 * @struct blocking_struct
 */

typedef struct blocking_struct {
        unsigned int N;  	/**< The total number of source blocks */
        unsigned int I;         /**< The first number of source blocks */
        unsigned int A_large;	/**< The larger source block length, this is for I number of source block */
        unsigned int A_small;	/**< The smaller source block length, this is for N - I number of source block */

} blocking_struct_t;

/**
 * This function calculates blocking scheme parameters.
 *
 * @param L transport object length
 * @param B source block length
 * @param E encoding symbol length
 *
 * @return structure containing blocking scheme parameters, NULL in error situations
 *
 */

blocking_struct_t *compute_blocking_structure(unsigned long long L, unsigned int B, unsigned int E);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
