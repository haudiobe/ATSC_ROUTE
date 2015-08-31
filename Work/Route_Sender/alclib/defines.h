/** \file defines.h \brief Definitions
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.171 $
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

#ifndef _DEFINES_H_
#define _DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Receiver socket buffer size
 */

#define RECEIVER_SOCKET_BUFFER_SIZE 2000000

#define MAD_FCL_RELEASE_VERSION "1.7"			/**< Release version */
#define MAD_FCL_RELEASE_DATE "March 2, 2007"	/**< Release date */

#ifdef _MSC_VER
#define timeb _timeb
#define ftime _ftime
#endif

/**
 * If you want to compile the code in a system where SSM is not supported
 * undefine SSM. For example in Windows 2000 SSM is not supported. */

#define SSM

#define USE_ZLIB /**< Use ZLIB compression library */
#define USE_RETRIEVE_UNIT /**< Use retrieve_unit() function */

#if defined(LINUX) || defined(_MSC_VER)
#define _BIT_FIELDS_LTOH	/**< Bit field order is compiler/OS dependant */
#undef _BIT_FIELDS_HTOL
#elif defined(SOLARIS)
#undef _BIT_FIELDS_LTOH
#define _BIT_FIELDS_HTOL	/**< Bit field order is compiler/OS dependant */
#endif

#define FDT_TOI		0				/**< TOI for FDT */	
#define EXT_FDT		192				/**< Extension header for FDT defined by FLUTE */
#define EXT_CENC	193				/**< Extension header for FDT content encoding defined by FLUTE */

#define ALC_VERSION		1		/**< ALC version number */
#define FLUTE_VERSION	1		/**< FLUTE version number */

#define COM_NO_C_FEC_ENC_ID		0	/**< Compact No-Code FEC scheme */ 
#define SIMPLE_XOR_FEC_ENC_ID   2	/**< Simple XOR FEC scheme */
#define RS_FEC_ENC_ID			3	/**< Reed-Solomon FEC scheme, identifier not yet decided, 3 used temporarily */

#define SB_LB_E_FEC_ENC_ID		128		/**< Small Block, Large Block and Expandable FEC scheme */
#define SB_SYS_FEC_ENC_ID		129		/**< Small Block Systematic FEC scheme */
#define COM_FEC_ENC_ID			130		/**< Compact FEC scheme */

#define REED_SOL_FEC_INST_ID	0		/**< Reed-Solomon instance id, when Small Block Systematic FEC scheme is used */

#define Null 0			/**< Null congestion control mechanism */
#define RLC	1			/**< RLC congestion control mechanism */

/* for RLC */
#define RLC_SP_CYCLE		250     /**< 250 ms, for fast layer addition */
#define RLC_WAIT_AFTER_SP	2		/**< packets to wait before start sending after syncronisation point */
#define RLC_DEAF_PERIOD		10000	/* time for deaf period after a dropped layer (10000 ms), due to IGMP leave latency */
#define RLC_LATE_ACCEPTED	0		/**< maximum number of late packets between two syncronisation points at the toplayer
									when a new layer can be added */
#define RLC_LOSS_ACCEPTED	0		/**< maximum number of lost packets between two syncronisation points at the toplayer
									when a new layer can be added */
#define RLC_PKT_TIMEOUT		500     /**< time to wait for a late packet before assuming it's lost (500 ms) */
#define RLC_LOSS_LIMIT		1		/**< RLC_LOSS_LIMIT / RLC_LOSS_TIMEOUT is the max loss rate for packet. If this rate
									is reached then we should drop the highest layer. */
#define RLC_LOSS_TIMEOUT	20		/**< RLC_LOSS_LIMIT / RLC_LOSS_TIMEOUT is the max loss rate for packet. If this rate
									is reached then we should drop the highest layer. */
#define RLC_MAX_LATES		100		/**< Maximum number of late packets */

#define EXT_NOP		0		/**< No-operation extension header defined by LCT */
#define	EXT_AUTH	1		/**< Authentication extension header defined by LCT */
#define EXT_TIME	2		/**< Time extension header defined by LCT */
#define	EXT_FTI		64		/**< FEC object transmission information extension header defined by ALC */

#define SENDER		0		/**< Operation mode is sender */	
#define RECEIVER	1		/**< Operation mode is receiver */

#define MAX_ALC_SESSIONS	        100		/**< Maximum number of ALC sessions */
#define MAX_CHANNELS_IN_SESSION		10		/**< Maximum number of channels per session */
#define MAX_PACKET_LENGTH			1500	/**< Maximum packet length */
#define MAX_PATH_LENGTH				1024	/**< Maximum path length */
#define MAX_PORT_LENGTH				10		/**< Maximum length for string that contains port number */
#define MAX_TX_QUEUE_SIZE			1000	/**< Maximum transmission queue size in packets */

#define WAITING_FDT		5	/**< FDT instance is in parsing state */
#define OK				4	/**< Correct packet received */
#define EMPTY_PACKET	3	/**< Empty packet received */
#define HDR_ERROR		2	/**< Error in FLUTE header */
#define MEM_ERROR		1	/**< Error when reserving memory */
#define DUP_PACKET		0	/**< Duplicate packet received*/

#define NO_TX_THREAD 0		/**< Mode when transmission thread is not used in the sender (with Null
							congestion control and only one channel) */
#define TX_THREAD 1			/**< Mode when ransmission thread is used in the sender */

#define ZLIB 1		/**< ZLIB content encoding */
#define DEFLATE 2	/**< DEFLATE content encoding */
#define GZIP 3		/**< GZIP content encoding */
#define PAD 4		/**< Content padding */

#ifdef USE_ZLIB
#define ZLIB_BUFLEN 16384					/**< Buffer length used with zlib library */
#define GZ_SUFFIX "~gz"						/**< Used GZIP suffix */
#define GZ_SUFFIX_LEN (sizeof(GZ_SUFFIX)-1) /**< Length of used GZIP suffix */
#define ZLIB_FDT 1							/**< Use ZLIB content encoding with FDT */
#define ZLIB_FDT_AND_GZIP_FILES 2			/**< Use ZLIB content encoding with FDT and GZIP content encoding with files */
#endif

#define PAD_FILES 3								/**< Use content padding with files */
#define PAD_SUFFIX "~pad"						/**< Used suffix for content padding */
#define PAD_SUFFIX_LEN (sizeof(PAD_SUFFIX)-1)	/**< Length of used suffix for content padding */

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

