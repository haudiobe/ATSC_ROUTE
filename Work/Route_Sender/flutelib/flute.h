/** \file flute.h \brief FLUTE sender and receiver
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.36 $
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

#ifndef _FLUTE_H_
#define _FLUTE_H_

#if defined(__cplusplus) && defined(_MSC_VER) 
#include <winsock2.h> 
#include <ws2tcpip.h> 
#endif

#include "../alclib/defines.h"
#include "../alclib/alc_session.h"
#include "parse_args.h"
#include "fdt.h"

/**
 * The following file is included so that
 * flute application needs to include only flute.h
 */

#include "flute_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** File URI table size */
#define FILE_URI_TABLE_SIZE 10

/**
 * Structure for FLUTE sender information.
 * @struct flute_sender
 */

typedef struct flute_sender {
  int s_id;			/**< session identifer */
  fdt_t *fdt;		/**< FDT structure */
} flute_sender_t;

/**
 * Structure for FLUTE receiver information.
 * @struct flute_receiver
 */

typedef struct flute_receiver {
  int s_id;										/**< session identifer */
  fdt_t *fdt;									/**< FDT structure */
  int rx_automatic;								/**< download files defined in the FDT automatically */
  char *file_uri_table[FILE_URI_TABLE_SIZE];    /**< file URIs for wanted files */
  char *wildcard_token;                         /**< wild card token */
  int accept_expired_fdt_inst;					/**< accept expired FDT Instances */
  int verbosity;								/**< verbosity level */
} flute_receiver_t;

/**
 * Structure for missing block information with FLUTE receiver report.
 * @struct missing_block
 */

typedef struct missing_block {
    struct missing_block *next;
    unsigned long long first;
    unsigned long long last;
} missing_block_t;

/**
 * Structure for FLUTE receiver report information.
 * @struct flute_receiver_report
 */

typedef struct flute_receiver_report {
    struct flute_receiver_report *next;
    char *file_name;
    char *md5;
    missing_block_t *mb_list;
} flute_receiver_report_t;

/**
 * This function is flute sender's "main function".
 *
 * @param a arguments structure where command line arguments are parsed
 * @param s_id stores session identifier
 * @param session_size stores session size
 *
 * @return 0 in success, -1 in error cases, -2 when state is SExiting, -3 if session end time passed
 *
 */

int flute_sender(arguments_t *a, int *s_id, unsigned long long *session_size);

/**
 * This function is flute receiver's "main function".
 *
 * @param a arguments structure where command line arguments are parsed
 * @param s_id stores session identifier
 *
 * @return 0 when state is SClosed, -1 in error cases, -2 when state is SExiting,
 * -3 when state is STxStopped, -4 when MD5 check error occurs, -5 when state is
 * SExiting in start time waiting period
 *
 */

int flute_receiver(arguments_t *a, int *s_id);

/**
 * This function is flute receiver's "main function" with receiver reporting.
 *
 * @param a arguments structure where command line arguments are parsed
 * @param s_id stores session identifier
 * @param report stores receiver report.
 *
 * @return 0 when state is SClosed, -1 in error cases, -2 when state is SExiting,
 * -3 when state is STxStopped, -4 when MD5 check error occurs, -5 when state is
 * SExiting in start time waiting period
 *
 */

int flute_receiver_report(arguments_t *a, int *s_id, flute_receiver_report_t **report);

/**
 * This function frees receiver report structure.
 * 
 * @param report pointer to receiver report structure.
 *
 */

void free_receiver_report(flute_receiver_report_t *report);

/**
 * This function calculates the amount of data that the FLUTE sender
 * will send (including UDP/IP).
 *
 * @param a pointer to the argument struct
 * @param s_id session identifier
 *
 * @return session size in bytes
 *
 */

unsigned long long flute_session_size(arguments_t *a, int *s_id);

/**
 * This function sets the session state for the session.
 *
 * @param s_id session identifier
 * @param state new state
 *
 */

void set_flute_session_state(int s_id, enum alc_session_states state);

/**
 * This function sets the session state for all opened sessions.
 *
 * @param state new state
 *
 */

void set_all_flute_sessions_state(enum alc_session_states state);

/**
 * This function sets new base rate for the session.
 *
 * @param s_id session identifier
 * @param base_tx_rate new transmission rate for the base channel
 * 
 */

void set_flute_session_base_rate(int s_id, int base_tx_rate);

/**  
 * This function performs all operations to start-up the library.
 * 
 * @return 0 in success, -1 otherwise                
 *
 */

int start_up_flute(void);

/**
 * This function performs all operations to shut-down the library.
 *
 * @param anArguments pointer to argument structure
 *
 */

void shut_down_flute(arguments_t* anArguments);

/**
 * This function performs all operations to shut-down the library.
 *
 */

void shut_down_flute2(void);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

