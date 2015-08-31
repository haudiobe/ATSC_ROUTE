/** \file mad.h \brief General ALC stuff
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.40 $
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

#ifndef _MAD_H_
#define _MAD_H_

#ifdef LINUX
#include <netinet/in.h>
#endif

#include "utils.h"
#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Is library initialized? 
 */

extern BOOL lib_init;

/**
 * Structure for ALC level arguments.
 * @struct alc_arguments
 */

typedef struct alc_arguments {
  unsigned long long tsi;				/**< transport session identifier */
  unsigned long long start_time;			/**< start time of the session */
  unsigned long long stop_time;			/**< stop time of the session */
  
  const char *port;                     /**< base channel port number  */
  const char *addr;                     /**< base channel multicastast address */
  const char *intface;                  /**< local interface to bind */
  const char *intface_name;             /**< name/index of local interface for IPv6 multicast join */
  unsigned int addr_family;             /**< address family */
  unsigned char addr_type;              /**< address type, multicast (0) or unicast (1) */
  unsigned char mode;                   /**< mode for the session (sender or receiver) */
  unsigned char nb_channel;             /**< number of channels in the session */
  unsigned char cc_id;                  /**< used congestion control, 0 = NULL, 1 = RLC */
  unsigned char rx_memory_mode;         /**< used memory mode in the receiver */
  int verbosity;                        /**< verbosity level */
  const char *src_addr;                 /**< source address for the session in the receiver */
  char base_dir[MAX_PATH_LENGTH];       /**< Base directory for downloaded/sent files */
  BOOL accept_expired_fdt_inst;			/**< accept expired FDT instances */
  
#ifdef SSM
  BOOL use_ssm;                         /**< use source specific multicast */
#endif
    
  unsigned int tx_rate;                 /**< transmission rate in kbit/s */
  unsigned char ttl;                    /**< time to live */
  unsigned short nb_tx;                 /**< how many times to send the file/directory? */
  BOOL simul_losses;                    /**< Simulate packet losses */
  double loss_ratio1;                   /**< packet loss ratio one */
  double loss_ratio2;                   /**< packet loss ratio two */
  unsigned short fec_ratio;             /**< FEC ratio percent */
  unsigned short es_len;                /**< encoding symbol length */
  unsigned int max_sb_len;              /**< maximum Source block length */
  unsigned char fec_enc_id;             /**< FEC encoding id */
  unsigned short fec_inst_id;           /**< FEC instance id */
  BOOL use_fec_oti_ext_hdr;             /**< use FEC OTI extension header */
  unsigned char encode_content;         /**< encode content using zlib library (0 = no, 1 = FDT, 2 = FDT and files) */
  BOOL half_word;						/**< use half word flag */
  BOOL optimize_tx_rate;				/**< optimize transmission rate (use more CPU) */
  BOOL calculate_session_size;

} alc_arguments_t;

#ifdef _MSC_VER
/**
 * lldiv_t structure.
 */

typedef struct {
  long long quot;
  long long rem;
}lldiv_t;
#endif

/**
 * This function initializes the library.
 *
 */

void alc_init(void);

/**
 * This function return number of seconds since sec_init() function was called.
 *
 * @return time since sec_init() was called
 *
 */

double sec(void);

/**
 * This function increses IPv6 address by one.
 * 
 * @param ipv6 pointer to IPv6 address to be increased 
 * 
 * @return 0 in success, -1 otherwise
 * 
 */ 

int increase_ipv6_address(struct in6_addr *ipv6);

/**
 * This function simulates packets losses randomly.
 *
 * @param lossprob loss probability percent
 *
 * @return 1 if packet should be dropped, 0 otherwize
 *
 */

int randomloss(double lossprob);

#ifdef _MSC_VER
/*
 * This function computes the quotient and remainder of an integer division.
 * 
 * @param num numerator
 * @param denom denumerator
 *
 * @return lldiv_t structure
 *
 */

lldiv_t lldiv(long long num, long long denom);
#endif

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

