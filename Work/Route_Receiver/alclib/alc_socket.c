/** \file alc_socket.c \brief Socket functions
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.49 $
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#ifdef _MSC_VER
#include <io.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#endif

#include "alc_socket.h"

/**
* This is a private function which reports the error
*
* @return the code error
*
*/

int error_socket() {
#ifdef _MSC_VER
    return WSAGetLastError();
#else
    return errno;
#endif
}

/**
* This is a private function which closes the socket
*
* @param a_socket socket to be closed
*
* @return 0 in success, -1 otherwise
*
*/

#ifdef _MSC_VER
int close_socket(SOCKET a_socket) {
    return closesocket(a_socket);
#else
int close_socket(int a_socket) {
    return close(a_socket);
#endif
}

int init_alc_socket(alc_channel_t *ch) {
	
  int receiver_socket_buffer_size = RECEIVER_SOCKET_BUFFER_SIZE;
  int ttl;
  int loop;
  int reuse;
  int mode;
  int addr_type;
  struct addrinfo hints, *local;
  struct in_addr if_addr;	/* interface IP address for IPv4 multicast */
  int return_value;  
  
#if defined(LINUX) && defined(SSM)
  struct sockaddr_in6 source;
#endif
	
  local = NULL;
  mode = ch->s->mode;
  addr_type = ch->s->addr_type;
  
  if(mode == SENDER) {
    
    if((ch->tx_sock  = socket(ch->s->addr_family, SOCK_DGRAM, 0)) < 0) {
	  printf("socket failed with: %d\n", error_socket());
      fflush(stdout);
      return -1;
    }
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = ch->s->addr_family;
    hints.ai_socktype = SOCK_DGRAM;
    
    /* ch->addrinfo is used in sendto function */
    
    if((return_value = getaddrinfo(ch->addr, ch->port, &hints, &ch->addrinfo)) != 0) {
#ifdef _MSC_VER
      printf("getaddrinfo failed: %d\n", error_socket());
#else
      if(return_value == EAI_SYSTEM) {
		printf("getaddrinfo failed: %d\n", error_socket());
      }
	  else{
		printf("getaddrinfo failed: %d %s\n", return_value, gai_strerror(return_value));
      }
#endif
	  fflush(stdout);	
      close_socket(ch->tx_sock);
      return -1;
    }
    
    /* Bind the socket to right interface */
    
    if(ch->intface != NULL) {
      
      if((((ch->s->addr_family == PF_INET6)) || (addr_type == 1))) {
	
		if((return_value = getaddrinfo(ch->intface, 0, &hints, &local)) != 0) {
#ifdef _MSC_VER
			printf("getaddrinfo failed: %d\n", error_socket());
#else
			if(return_value == EAI_SYSTEM) {
				printf("getaddrinfo failed: %d\n", error_socket());
			}
			else{
				printf("getaddrinfo failed: %d %s\n", return_value, gai_strerror(return_value));
			}	
#endif
			fflush(stdout);
			close_socket(ch->tx_sock);
			return -1;
		}
	
		if(bind(ch->tx_sock, local->ai_addr, local->ai_addrlen) < 0) {
			printf("bind failed: %d\n", error_socket());
			fflush(stdout);
			close_socket(ch->tx_sock);
			return -1;
		}
      }
      else if(((ch->s->addr_family == PF_INET) && (addr_type == 0))) {
		/* does not work without this interface setting */
		if_addr.s_addr = inet_addr(ch->intface);
	
		/* specify multicast interface */
		if(setsockopt(ch->tx_sock, IPPROTO_IP, IP_MULTICAST_IF, 
		      (char*)&if_addr.s_addr, sizeof(if_addr.s_addr)) < 0 ) {
	  
			printf("setsockopt (IP_MULTICAST_IF) failed: %d\n", error_socket());
			fflush(stdout);
			close_socket(ch->tx_sock);
			return -1;
		}
      }
    }
    
    if(addr_type == 0) {
      
      /* TTL */
      ttl = min(ch->s->def_ttl, 255);
      
      if(ch->s->addr_family == PF_INET) {
	
		if(setsockopt(ch->tx_sock, IPPROTO_IP, IP_MULTICAST_TTL, 
		      (char *)&ttl, sizeof(ttl)) < 0 ) {
			printf("setsockopt (IP_MULTICAST_TTL) failed: %d\n", error_socket());
			fflush(stdout);
			close_socket(ch->tx_sock);
			return -1;
		}
      }
      else if(ch->s->addr_family == PF_INET6) {
	
		if(setsockopt(ch->tx_sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, 
		      (char *)&ttl, sizeof(ttl)) < 0 ) {
			printf("setsockopt (IPV6_MULTICAST_HOPS) failed: %d\n", error_socket());
			fflush(stdout);
			close_socket(ch->tx_sock);
			return -1;
		}
      }	
      
      /* Loopback (doesn't work in Windows, even if loop = 0, data is looped back) */
      /* set to 0, when you don't have to do testing any more */
      loop = 1;
      
      if(ch->s->addr_family == PF_INET) {
	
		if(setsockopt(ch->tx_sock, IPPROTO_IP, IP_MULTICAST_LOOP, 
		      (char *)&loop, sizeof(loop)) != 0) {
			printf("setsockopt (IP_MULTICAST_LOOP) failed: %d\n", error_socket());
			fflush(stdout);
			close_socket(ch->tx_sock);
			return -1;
		}
      }
      else if(ch->s->addr_family == PF_INET6) {
	
		if(setsockopt(ch->tx_sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, 
		      (char *)&loop, sizeof(loop)) != 0) {
			printf("setsockopt (IPV6_MULTICAST_LOOP) failed: %d\n", error_socket());
			fflush(stdout);
			close_socket(ch->tx_sock);
			return -1;
		}
      }
    }
    
    if(local != NULL) {
      freeaddrinfo(local);
    }
  }
  
  if(mode == RECEIVER) {
    
    if((ch->rx_sock = socket(ch->s->addr_family, SOCK_DGRAM, 0)) < 0) {
      printf("socket failed with: %d\n", error_socket());
      fflush(stdout);
      return -1;
    }
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = ch->s->addr_family;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    
#ifdef _MSC_VER
	if((return_value = getaddrinfo(ch->intface, ch->port, &hints, &ch->addrinfo)) != 0) {
#else
	if((return_value = getaddrinfo(ch->addr, ch->port, &hints, &ch->addrinfo)) != 0) {
#endif

#ifdef _MSC_VER
      printf("getaddrinfo failed: %d\n", error_socket());
#else
      if(return_value == EAI_SYSTEM) {
		printf("getaddrinfo failed: %d\n", error_socket());
      }
	  else{
		printf("getaddrinfo failed: %d %s\n", return_value, gai_strerror(return_value));
      }
      
#endif
	  fflush(stdout);
	  close_socket(ch->rx_sock);
      return -1;
	}

	reuse = 1;

    if(setsockopt(ch->rx_sock, SOL_SOCKET, SO_REUSEADDR,
                  (char *)&reuse, sizeof(reuse)) < 0 ) {
      printf("setsockopt (SO_REUSEADDR) failed: %d\n", error_socket());
	  fflush(stdout);
      close_socket(ch->rx_sock);
      return -1;
    }

    if(setsockopt(ch->rx_sock, SOL_SOCKET, SO_RCVBUF, (char *)&receiver_socket_buffer_size, sizeof(int))) {
      printf("setsockopt (SO_RCVBUF) failed: %d\n", error_socket());
	  fflush(stdout);
      close_socket(ch->rx_sock);
      return -1;
    }
    
    if(bind(ch->rx_sock, ch->addrinfo->ai_addr, ch->addrinfo->ai_addrlen) < 0) {

      printf("bind failed: %d\n", error_socket());
	  fflush(stdout);
      close_socket(ch->rx_sock);
      return -1;
    }
    
    if(addr_type == 0) {
      if(ch->s->addr_family == PF_INET) {
	
		memset((void *)&ch->remote, 0, sizeof(ch->remote));
		
		ch->remote.sin_addr.s_addr = inet_addr(ch->addr);
		ch->remote.sin_port = (unsigned short)atoi(ch->port);
		ch->remote.sin_family = ch->s->addr_family;
      }
      else if(ch->s->addr_family == PF_INET6) {
	
		memset((void *)&ch->remote6, 0, sizeof(ch->remote6));
#ifdef _MSC_VER
		WSAStringToAddress((char*)ch->addr, PF_INET6, NULL, (struct sockaddr*)&ch->remote6,
			   &ch->addrinfo->ai_addrlen);
#else
		inet_pton(PF_INET6, ch->addr, &ch->remote6.sin6_addr);
#endif
		ch->remote6.sin6_port = (unsigned short)atoi(ch->port);
		ch->remote6.sin6_family = ch->s->addr_family;
      }
    }
    
    if(addr_type == 0) {
      
      /* Join the multicast group */
      
      if(ch->s->addr_family == PF_INET) {
	
#ifdef SSM
	if(ch->s->ssm) {
	  ch->source_imr.imr_multiaddr.s_addr = ch->remote.sin_addr.s_addr;
	  
	  if(ch->intface == NULL) {
	    ch->source_imr.imr_interface.s_addr = htonl(INADDR_ANY);
	  }
	  else {
	    ch->source_imr.imr_interface.s_addr = inet_addr(ch->intface);
	  }
	  
	  ch->source_imr.imr_sourceaddr.s_addr = inet_addr(ch->s->src_addr);
	  
	  if(setsockopt(ch->rx_sock, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP,
			(char *)&ch->source_imr, sizeof(ch->source_imr)) < 0) {
	    printf("setsockopt (IP_ADD_SOURCE_MEMBERSHIP) failed: %d\n", error_socket());
	    fflush(stdout);
		close_socket(ch->rx_sock);
	    return -1;
	  }
	}
	else {
	  ch->imr.imr_multiaddr.s_addr = ch->remote.sin_addr.s_addr;
	  
	  if(ch->intface == NULL) {
	    ch->imr.imr_interface.s_addr = htonl(INADDR_ANY);
	  }
	  else {
	    ch->imr.imr_interface.s_addr = inet_addr(ch->intface);
	  }
	  
	  if(setsockopt(ch->rx_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			(char *)&ch->imr, sizeof(ch->imr)) < 0) {
	    printf("setsockopt (IP_ADD_MEMBERSHIP) failed: %d\n", error_socket());
	    fflush(stdout);
		close_socket(ch->rx_sock);
	    return -1;
	  }
	}
#else
	ch->imr.imr_multiaddr.s_addr = ch->remote.sin_addr.s_addr;
	
	if(ch->intface == NULL) {
	  ch->imr.imr_interface.s_addr = htonl(INADDR_ANY);
	}
	else {
	  ch->imr.imr_interface.s_addr = inet_addr(ch->intface);
	}
	
	if(setsockopt(ch->rx_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		      (char *)&ch->imr, sizeof(ch->imr)) < 0) {
	  printf("setsockopt (IP_ADD_MEMBERSHIP) failed: %d\n", error_socket());
	  fflush(stdout);
	  close_socket(ch->rx_sock);
	  return -1;
	}
#endif
      }
      else if(ch->s->addr_family == PF_INET6) {
	
#if defined(LINUX) && defined(SSM)
	if(ch->s->ssm) {
	  
	  if(ch->intface_name == NULL) {
	    ch->greqs.gsr_interface = 0;
	  }
	  else {
	    ch->greqs.gsr_interface = if_nametoindex(ch->intface_name);
	  }
	  
	  memcpy(&ch->greqs.gsr_group, &ch->remote6, sizeof(ch->remote6));
	  source.sin6_family = AF_INET6;        
	  
	  inet_pton(AF_INET6, ch->s->src_addr, &source.sin6_addr);
	  memcpy(&ch->greqs.gsr_source, &source, sizeof(source));
	  
	  if(setsockopt(ch->rx_sock, IPPROTO_IPV6, MCAST_JOIN_SOURCE_GROUP,
			(char *)&ch->greqs, sizeof(ch->greqs)) < 0) {
	    
	    printf("setsockopt (MCAST_JOIN_SOURCE_GROUP) failed: %d\n", error_socket());
	    fflush(stdout);
	    close_socket(ch->rx_sock);
	    return -1;
	  }
	}
	else {
	  ch->imr6.ipv6mr_multiaddr = ch->remote6.sin6_addr;
	  
	  if(ch->intface_name == NULL) {
	    ch->imr6.ipv6mr_interface = 0;
	  }
	  else {
	    ch->imr6.ipv6mr_interface = if_nametoindex(ch->intface_name);
	  }
	  
	  if(setsockopt(ch->rx_sock, IPPROTO_IPV6, IPV6_JOIN_GROUP,
			(char *)&ch->imr6, sizeof(ch->imr6)) < 0) {
	    
	    printf("setsockopt (IPV6_JOIN_GROUP) failed: %d\n", error_socket());
	    fflush(stdout);
	    close_socket(ch->rx_sock);
	    return -1;
	  }
	}
#else
	ch->imr6.ipv6mr_multiaddr = ch->remote6.sin6_addr;
	
#ifdef LINUX
	if(ch->intface_name == NULL) {
	  ch->imr6.ipv6mr_interface = 0;
	}
	else {
	  ch->imr6.ipv6mr_interface = if_nametoindex(ch->intface_name);
	}
#else
	if(ch->intface_name == NULL) {
	  ch->imr6.ipv6mr_interface = 0;
	}
	else {
	  ch->imr6.ipv6mr_interface = atoi(ch->intface_name);
	}
	
#endif	
	if(setsockopt(ch->rx_sock, IPPROTO_IPV6, IPV6_JOIN_GROUP,
		      (char *)&ch->imr6, sizeof(ch->imr6)) < 0) {
	  printf("setsockopt (IPV6_JOIN_GROUP) failed: %d\n", error_socket());
	  fflush(stdout);
	  close_socket(ch->rx_sock);
	  return -1;
	}
#endif
      }
    }
  }
  
  return 0;
}

int close_alc_socket(alc_channel_t *ch) {
  
  int mode;
  int addr_type;
  mode = ch->s->mode;
  addr_type = ch->s->addr_type;
  
  if(mode == SENDER) {
    close_socket(ch->tx_sock);    
  }
  
  if(mode == RECEIVER) {
    
    if(addr_type == 0) {
      
      if(ch->s->addr_family == PF_INET) {
	
#ifdef SSM
	if(ch->s->ssm) {
	  
	  if(setsockopt(ch->rx_sock, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP,
			(char *)&ch->source_imr, sizeof(ch->source_imr)) < 0) {
	    printf("setsockopt (IP_DROP_SOURCE_MEMBERSHIP) failed: %d\n", error_socket());
	    fflush(stdout);
		close_socket(ch->rx_sock);
	    return -1;
	  }
	}
	else {
	  if(setsockopt(ch->rx_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
			(char *)&ch->imr, sizeof(ch->imr)) < 0) {
	    printf("setsockopt (IP_DROP_MEMBERSHIP) failed: %d\n", error_socket());
	    fflush(stdout);
		close_socket(ch->rx_sock);
	    return -1;
	  }
	}
#else
	if(setsockopt(ch->rx_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		      (char *)&ch->imr, sizeof(ch->imr)) < 0) {
	  printf("setsockopt (IP_DROP_MEMBERSHIP) failed: %d\n", error_socket());
	  fflush(stdout);
	  close_socket(ch->rx_sock);
	  return -1;
	}
#endif
      }
      else if(ch->s->addr_family == PF_INET6) {
	
#if defined(LINUX) && defined(SSM)
	if(ch->s->ssm) {
	  if(setsockopt(ch->rx_sock, IPPROTO_IPV6, MCAST_LEAVE_SOURCE_GROUP,
			(char *)&ch->greqs, sizeof(ch->greqs)) < 0) {
	    
	    printf("setsockopt (MCAST_LEAVE_SOURCE_GROUP) failed: %d\n", error_socket());
	    fflush(stdout);
	    close(ch->rx_sock);
	    return -1;
	  }
	}
	else {
	  if(setsockopt(ch->rx_sock, IPPROTO_IPV6, IPV6_LEAVE_GROUP,
			(char *)&ch->imr6, sizeof(ch->imr6)) < 0) {
	    
	    printf("setsockopt (IPV6_LEAVE_GROUP) failed: %d\n", error_socket());
	    fflush(stdout);
	    close(ch->rx_sock);
	    return -1;
	  }
	}
#else
	if(setsockopt(ch->rx_sock, IPPROTO_IPV6, IPV6_LEAVE_GROUP,
		      (char *)&ch->imr6, sizeof(ch->imr6)) < 0) {
	  printf("setsockopt (IPV6_LEAVE_GROUP) failed: %d\n", error_socket());
	  fflush(stdout);
	  close_socket(ch->rx_sock); 
	  return -1;
	}
#endif
      }
    }

    close_socket(ch->rx_sock);
  }
  
  return 0;
}

