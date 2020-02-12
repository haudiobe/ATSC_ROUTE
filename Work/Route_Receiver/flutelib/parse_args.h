/** \file parse_args.h \brief Parse command line argumets
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.22 $
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

#ifndef _PARSE_ARGS_H_
#define _PARSE_ARGS_H_

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

#include "flute_defines.h"
#include "../alclib/defines.h"
#include "../alclib/mad.h"
#include "../sdplib/sdp_message.h"
#include "../sdplib/sdplib.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Structure for command line arguments.
 * @struct arguments
 */

typedef struct arguments {

  struct alc_arguments alc_a;          /**< structure for ALC level arguments*/
  
  /** buffer which contains file repair file name, defaults are 'apd.xml' for the receiver and 'flute.conf' for the sender
      repair sender uses also this variable, and default is 'repair_sender.conf' */
  char repair[MAX_PATH_LENGTH];                
  char file_path[MAX_PATH_LENGTH];        /**< files and/or directories to send or receive */

  int log_fd;                  /**< log file descriptor */
  BOOL rx_automatic;              /**< download files defined in the FDT automatically */
  BOOL open_file;                /**< open received file automatically */
  char sdp_file[MAX_PATH_LENGTH];        /**< SDP file name */
  BOOL rx_object;                /**< receive object with receiver_in_object_mode() */
  BOOL name_incomplete_objects;          /**< name incomplete files after the session is ended*/
  unsigned short complete_fdt;          /**< send complete FDT Instance in the beginning of a loop */
  BOOL file_table_output;            /**< prints sending/receiving information in a thread to the table */
  
  unsigned long long toi;                       /**< transport object identifier used with receiver_in_object_mode() */
  
  char fdt_file[MAX_PATH_LENGTH];               /**< FDT file used with -f */
  BOOL cont;                                    /**< continuous transmission */
  int send_session_close_packets;               /**< send session close packet: 1 == Yes, 0 == No */
    
  /* Repair Sender */
  char requested_blocks_file[MAX_PATH_LENGTH];  /**< PTM repair 'ptm_requested_blocks' file */
  char flute_conf_file[MAX_PATH_LENGTH];      /**< FLUTE configuration file */

  sdp_message_t *sdp;              /**< structure for referencing a SDP packet */
  sf_t *src_filt;                /**< source filter structure */
  
} arguments_t;

/**
 * This function validates and parses command line arguments.
 *
 * @param argc number of command line arguments
 * @param argv pointer to command line arguments
 * @param a arguments structure where command line arguments are parsed
 *
 * @return 0 in success, -1 otherwise
 *
 */

int parse_args(int argc, char **argv, arguments_t *a);

/**
 * This function parses session information from the SDP file.
 *
 * @param a arguments structure where command line arguments are parsed
 * @param addrs multiccast addresses
 * @param ports local port numbers
 * @param sdp_buf buffer containing session description to be parsed
 *
 * @return 0 in success, -1 otherwise
 *
 */

int parse_sdp_file(arguments_t *a, char addrs[MAX_CHANNELS_IN_SESSION][INET6_ADDRSTRLEN],
                   char ports[MAX_CHANNELS_IN_SESSION][MAX_PORT_LENGTH], char *sdp_buf);

/**
 * This function free arguments structure.
 *
 * @param a arguments structure where command line arguments are parsed
 *
 */

void free_args(arguments_t *a);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
