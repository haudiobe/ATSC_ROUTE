/** \file flute_defines.h \brief FLUTE level definitions
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.43 $
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

#ifndef _FLUTE_DEFINES_H_
#define _FLUTE_DEFINES_H_

#define USE_FILE_REPAIR			/**< Use file repair based on 3GPP TS 26.346 */
#define USE_OPENSSL				/**< Use OpenSSL library for MD5 check */
/*#define HOSTNAME_TO_FDT*/		/**< Include host name to the file URI in the FDT */

#define FDT_INST_FEC_OTI_COMMON		/**< Place for the FEC OTI in the FDT Instance, i.e. common for all or own for each file */
/*#define FDT_INST_FEC_OTI_FILE*/	/**< Place for the FEC OTI in the FDT Instance, i.e. common for all or own for each file  */

/** Maximum encoding symbol length for IPv4 with FEC encoding IDs 0, 3, and 130 (calculated for FDT with TSI&TOI 4+4 bytes) */
#define MAX_SYMB_LENGTH_IPv4_FEC_ID_0_3_130			1428
/** Maximum encoding symbol length for IPv4 with FEC encoding IDs 2, 128, and 129 (calculated for FDT with TSI&TOI 4+4 bytes) */
#define MAX_SYMB_LENGTH_IPv4_FEC_ID_2_128_129	1424
/** Maximum encoding symbol length for IPv6 with FEC encoding IDs 0, 3, and 130 (calculated for FDT with TSI&TOI 4+4 bytes) */
#define MAX_SYMB_LENGTH_IPv6_FEC_ID_0_3_130			1408
/** Maximum encoding symbol length for IPv6 with FEC encoding IDs 2, 128, and 129 (calculated for FDT with TSI&TOI 4+4 bytes) */
#define MAX_SYMB_LENGTH_IPv6_FEC_ID_2_128_129	1404	

#define MAX_SB_LEN_NULL_FEC			255				/**< Maximum source block length for Compact No-Code FEC */
#define MAX_SB_LEN_SIMPLE_XOR_FEC	10				/**< Maximum source block length for Simple XOR FEC */
#define MAX_N_REED_SOLOMON			((1 << 8) - 1)	/**< Maximum number of encoding symbols per block for Reed-Solomon FEC */

#ifdef USE_FILE_REPAIR
#define MAX_HTTP_URL_LENGTH 256				/**< Maximum length for HTTP URL */
#define DEF_APD_CONF_FILE "apd.xml"			/**< Default Assosiated Prosedure Description file */
#define DEF_FLUTE_CONF_FILE "flute.conf"	/**< Default FLUTE configuration file */
#endif

#define DEF_REPAIR_SENDER_CONF_FILE "repair_sender.conf"	/**< Default repair sender configuration file */

/** Default Forward Error Correction (0 = Compact No-Code FEC, 1 = Simple XOR FEC, 2 = Reed-Solomon FEC (old-ID)
    3 = Reed-Solomon FEC (new-id) */
#define DEF_FEC					0
#define DEF_CC					Null				/**< Default Congestion Control */
#define DEF_MCAST_IPv4_ADDR		"226.10.40.1"		/**< Default IPv4 multicast address */
#define DEF_MCAST_IPv6_ADDR		"ff1a::1"			/**< Default IPv6 multicast address */
#define DEF_MCAST_PORT			"4001"				/**< Default port number */
#define DEF_NB_CHANNEL			1					/**< Default number of channels */
#define DEF_TSI					0					/**< Default transport session identifier */
#define DEF_FDT					"fdt.xml"			/**< Default FDT filename (used when only -f is set) */
#define DEF_SYMB_LENGTH			MAX_SYMB_LENGTH_IPv4_FEC_ID_0_3_130	/**< Default encoding symbol length */
#define DEF_MAX_SB_LEN			64					/**< Default maximum source block length */
#define DEF_TX_RATE				250					/**< Default transmission rate */
#define DEF_TTL					1					/**< Default Time to Live value */
#define DEF_TX_NB				1					/**< Default number of transmission loops */
#define DEF_BASE_DIR			"flute-downloads"	/**< Default base directory */
#define DEF_FEC_ENC_ID			COM_NO_C_FEC_ENC_ID	/**< Default FEC encoding id */
#define DEF_FEC_INST_ID			REED_SOL_FEC_INST_ID	/**< Default FEC instance id */
#define DEF_FEC_RATIO			50						/**< Default FEC ratio */
#define DEF_ADDR_FAMILY			PF_INET					/**< Default address family */
#define DEF_SESSION_DURATION	604800					/**< Default session "uptime", sender will send one week if -C is set */
/**< Reveiver will quit after this time if no packets is received from the session */
#define DEF_RECEIVER_TIMEOUT	3600					 

#define P_LOSS_WHEN_OK		5			/**< Packet loss probability when earlier packet was not lost */
#define P_LOSS_WHEN_LOSS	50			/**< Packet loss probabilities when earlier packet was lost */				
 
#define FDT_INTERVAL (1*1048576)		/**< Interval between FDT Instances (x*MB) */

#endif

