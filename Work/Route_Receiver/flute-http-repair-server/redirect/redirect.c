/** \file redirect.c \brief Small program that handles redirection with 302 message
 *
 *  $Author: peltotal $ $Date: 2007/02/27 11:32:01 $ $Revision: 1.10 $
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

#include <stdlib.h>
#include<stdio.h>
#include <string.h>

#include "../mbms-rel6-flute-repair/helpers.h"

#include "../../flutelib/getdnsname.h"

/**
 * This function is programs main function.
 *
 * @return 0 in success, -1 otherwise
 *
 */

int main(void) {

  char *hostname = NULL;
  
  char *repair_conf_file = NULL;
  repair_arguments_t ra;
  int retval;
  
  repair_conf_file = getenv("RepairConfFile");

  if(repair_conf_file == NULL) {
    return -1;
  }

  if(repair_conf_file[strlen(repair_conf_file)-1] == '\r') {
    repair_conf_file[strlen(repair_conf_file)-1] = '\0';
  }
  
  retval = parse_repair_conf_file(repair_conf_file, &ra);
    
  if(retval == -1) {
    return -1;
  }

  hostname = getdnsname();

  printf("%s%s%s%s%c%c", "Location: http://", hostname, "/", ra.ptm_repair_sdp_file, 13, 10);
  printf("%s%c%c", "Connection: close", 13, 10);
  printf("%c%c", 13, 10);

  free(hostname);
  return 0;
}
