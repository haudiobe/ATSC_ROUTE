/** \file add_fec_plid.h \brief Functions which add FEC payload id   
 *
 *  $Author: peltotal $ $Date: 2007/02/27 11:32:01 $ $Revision: 1.7 $
 *
 *  MAD-FLUTE-REPAIR-SERVER: Implementation of repair server for FLUTE protocol.
 *  Copyright (c) 2005-2007 TUT - Tampere University of Technology
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
 */

#ifndef _ADD_FEC_PLID_H_
#define _ADD_FEC_PLID_H_

/**
 * This function adds FEC Payload ID header for FEC Encoding ID 128.
 *
 * @param sbn source block number
 * @param esi encoding symbol identifier
 *
 */

/*
 * FEC Payload ID for ID 128
 *
 * 0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                   Source Block Number                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                   Encoding Symbol ID                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

void add_fec_plid_128(unsigned int sbn, unsigned int esi);

/**
 * This function adds FEC Payload ID header for FEC Encoding ID 129.
 *
 * @param sbn source block number
 * @param sbl source block length
 * @param esi encoding symbol identifier
 *
 */

/*
 * FEC Payload ID for ID 129
 *
 * 0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                   Source Block Number                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Source Block Length       |      Encoding Symbol ID       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

void add_fec_plid_129(unsigned int sbn, unsigned short sbl, unsigned short esi);

/**
 * This function adds FEC Payload ID header for FEC Encoding IDs 0 and 130.
 *
 * @param sbn source block number
 * @param esi encoding symbol identifier
 *
 *
 */

/*
 * FEC Payload ID for IDs 0 & 130
 *
 * 0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Source Block Number       |    Encoding Symbol ID         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

void add_fec_plid_0_130(unsigned short sbn, unsigned short esi);

/**
 * This function adds length indicator.
 *
 * @param li length indicator
 *
 */

/*
 * Length Indicator
 *
 * 0                   1
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |        Length Indicator       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

void add_length_indicator(unsigned short li);

#endif
