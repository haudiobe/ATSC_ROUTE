/** \file getdnsname.c \brief Get host's DNS name
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.12 $
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#include <winsock2.h>
#else
#include <netdb.h>
#include <unistd.h>
#endif

#include "getdnsname.h"

char* getdnsname() {

  char *hostname = NULL;
  char host[MAX_PATH_LENGTH];
  struct hostent *hptr;
    
  memset(host, 0, MAX_PATH_LENGTH);
  gethostname(host, MAX_PATH_LENGTH);
  
  if((hptr = gethostbyname(host)) == NULL) {
    
    if(!(hostname = (char*)calloc((strlen(host) + 1), sizeof(char)))) {
      printf("Could not alloc memory for hostname!\n");
      fflush(stdout);
      return NULL;
    }
    
    memcpy(hostname, host, strlen(host));
    return hostname;
    
  }
  else {
    memset(host, 0, MAX_PATH_LENGTH);
    memcpy(host, hptr->h_name, strlen(hptr->h_name));
  }  
  
  if(!(hostname = (char*)calloc((strlen(host) + 1), sizeof(char)))) {
    printf("Could not alloc memory for hostname!\n");
    fflush(stdout);
    return NULL;
  }
  
  memcpy(hostname, host, strlen(host));
  
  return hostname;
}

