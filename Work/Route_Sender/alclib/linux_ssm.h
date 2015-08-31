/** \file linux_ssm.h \brief Defines and structures needed for SSM<br>
 *  <br>All in this file should be in linux/in.h on new enough systems, so there's an
 *  attempt to protect them with an ifdef
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.17 $
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

#ifndef _LINUX_SSM_H_
#define _LINUX_SSM_H_

#ifdef LINUX

#include <netinet/in.h>

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IP_ADD_SOURCE_MEMBERSHIP

/**
 * @struct ip_mreq_source
 */

struct ip_mreq_source {
  struct in_addr imr_multiaddr;  /**< IP address of group */
  struct in_addr imr_interface;  /**< IP address of interface */
  struct in_addr imr_sourceaddr; /**< IP address of source */
};

#define IP_ADD_SOURCE_MEMBERSHIP        39
#define IP_DROP_SOURCE_MEMBERSHIP       40

#endif

#ifndef MCAST_JOIN_SOURCE_GROUP

/**
 * @struct group_req
 */

struct group_req {
  uint32_t gr_interface;			/**< interface index */
  struct sockaddr_storage gr_group;	/**< group address */
};

/**
 * @struct group_req
 */

struct group_source_req {
  uint32_t gsr_interface;				/**< interface index */
  struct sockaddr_storage gsr_group;	/**< group address */
  struct sockaddr_storage gsr_source;	/**< source address */
};

#define MCAST_JOIN_SOURCE_GROUP		46
#define MCAST_LEAVE_SOURCE_GROUP	47

#endif

#endif

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
