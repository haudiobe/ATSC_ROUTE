/** \file repair_sender.h \brief Point-to-multipoint repair sender
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.15 $
 *
 *  MAD-REPAIR-SENDER: Implementation of simple point-to-multipoint repair for FLUTE protocol.
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

#ifndef _REPAIR_SENDER_H_
#define _REPAIR_SENDER_H_

#include "../flutelib/flute.h"

/**
 * Structure for missing block.
 * @struct miss_block
 */

typedef struct miss_block {
  struct miss_block *next;	/**< next item */
  struct miss_block *prev;	/**< previous item */
  unsigned int sbn;			/**< source block number */
} miss_block_t;

/**
 * Structure for missing object.
 * @struct miss_obj
 */

typedef struct miss_obj {
  struct miss_obj *next;			/**< next item */
  struct miss_obj *prev;			/**< previous item */
  struct miss_block *block_list;	/**< first missing block for this object */
  unsigned long long toi;			/**< transport object identifier */
} miss_obj_t;

/**
 * This function is repair sender's main function.
 *
 * @param a arguments structure where command line arguments are parsed
 * @param s_id session identifier
 *
 * @return 0 in success, -1 in error cases, -2 when state is SExiting
 *
 */

int repair_sender(arguments_t *a, int *s_id);

/*
 * This function parses repair sender's configuration file.
 *
 * @param a arguments structure where command line arguments are parsed
 *
 * Return: int: 0 in success, -1 otherwise
 */

int parse_repair_sender_conf_file(arguments_t *a);

/**
 * This function parses FLUTE configuration file for repair sender.
 *
 * @param a arguments structure where command line arguments are parsed
 *
 * Return: int: 0 in success, -1 otherwise
 */

int parse_flute_conf_file(arguments_t *a);

#endif
