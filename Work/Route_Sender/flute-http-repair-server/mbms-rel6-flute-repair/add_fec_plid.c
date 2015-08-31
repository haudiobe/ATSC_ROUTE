/** \file add_fec_plid.c \brief Functions which add FEC payload id
 *
 *  $Author: peltotal $ $Date: 2007/02/27 11:32:01 $ $Revision: 1.9 $
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

#include <stdio.h>

void add_fec_plid_128(unsigned int sbn, unsigned int esi) {
  printf("%c", (unsigned char)((sbn >> 24) & 0xFF));
  printf("%c", (unsigned char)((sbn >> 16) & 0xFF));
  printf("%c", (unsigned char)((sbn >> 8) & 0xFF));
  printf("%c", (unsigned char)(sbn & 0xFF));
  printf("%c", (unsigned char)((esi >> 24) & 0xFF));
  printf("%c", (unsigned char)((esi >> 16) & 0xFF));
  printf("%c", (unsigned char)((esi >> 8) & 0xFF));
  printf("%c", (unsigned char)(esi & 0xFF));
}

void add_fec_plid_129(unsigned int sbn, unsigned short sbl, unsigned short esi) {
  printf("%c", (unsigned char)((sbn >> 24) & 0xFF));
  printf("%c", (unsigned char)((sbn >> 16) & 0xFF));
  printf("%c", (unsigned char)((sbn >> 8) & 0xFF));
  printf("%c", (unsigned char)(sbn & 0xFF));
  printf("%c", (unsigned char)((sbl >> 8) & 0xFF));
  printf("%c", (unsigned char)(sbl & 0xFF));
  printf("%c", (unsigned char)((esi >> 8) & 0xFF));
  printf("%c", (unsigned char)(esi & 0xFF));
}

void add_fec_plid_0_130(unsigned short sbn, unsigned short esi) {
  printf("%c", (unsigned char)((sbn >> 8) & 0xFF));
  printf("%c", (unsigned char)(sbn & 0xFF));
  printf("%c", (unsigned char)((esi >> 8) & 0xFF));
  printf("%c", (unsigned char)(esi & 0xFF));
}

void add_length_indicator(unsigned short li) {
  printf("%c", (unsigned char)((li >> 8) & 0xFF));
  printf("%c", (unsigned char)(li & 0xFF));
}
