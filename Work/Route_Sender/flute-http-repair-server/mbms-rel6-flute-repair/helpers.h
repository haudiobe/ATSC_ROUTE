/** \file helpers.h \brief Functions file parsing information the configuration files
 *
 * $Author: peltotal $ $Date: 2007/02/27 11:32:01 $ $Revision: 1.10 $
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

#ifndef _HELPERS_H_
#define _HELPERS_H_

#include "../../alclib/defines.h"

/** Structure for the needed repair arguments.
 * @struct repair_arguments
 */

typedef struct repair_arguments {
  char requested_blocks_file[MAX_PATH_LENGTH]; /**< File where to store requested blocks whe ptm repair is used */
  char repair_method[MAX_PATH_LENGTH];  /**< Used repair method (PTP or PTM) */
  char flute_conf_file[MAX_PATH_LENGTH]; /**< File which contains information about FLUTE session to be repaired */
  char base_dir[MAX_PATH_LENGTH]; /**< Base directory for the files */
  char ptm_repair_sdp_file[MAX_PATH_LENGTH]; /**< SDP file for the PTM repair session */
  char fdt_file[MAX_PATH_LENGTH]; /**< FDT file of the FLUTE session to be repaired */
  unsigned short fec_ratio; /**< FEC ratio  of the FLUTE session to be repaired */
} repair_arguments_t;

/**
 * This function parses repair server configuration file.
 *
 * @param repair_conf_file file which contains repair server configuration arguments
 * @param ra structure where to store repair server configuration arguments
 *
 * @return 0 in success, -1 otherwise
 */

int parse_repair_conf_file(char *repair_conf_file, repair_arguments_t *ra);

/**
 * This function parses FLUTE session configuration file.         
 *                                  
 * @param ra structure where to store repair server configuration arguments
 *
 * @return 0 in success, -1 otherwise      
 */

int parse_flute_conf_file(repair_arguments_t *ra);

#endif
