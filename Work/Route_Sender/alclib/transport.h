/** \file transport.h \brief Transport structures and functions
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.46 $
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

#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <sys/types.h>

#ifdef LINUX
#include <unistd.h>
#endif

#include "defines.h"
#include "alc_session.h"
#include "blocking_alg.h"

//Malek El Khatib 12.08.2014
//Start
extern unsigned short numEncSymbPerPacket;
//END

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure for transport unit.
 * @struct trans_unit
 */

typedef struct trans_unit {

  struct trans_unit  *prev;  /**< pointer to previous unit */
  struct trans_unit  *next;  /**< pointer to next unit */
  unsigned int esi;      /**< encoding symbol id*/
  unsigned short len;    /**< length of this transport unit (in bytes) */
  char *data;        /**< pointer to data buffer */
  
#ifdef _MSC_VER
  __int64 offset;      /**< data symbol offset in the temporary file */
#else
  off64_t offset;      /**< data symbol offset in the temporary file */
#endif

#ifdef USE_RETRIEVE_UNIT
  unsigned char  used;       /**< is the current transport unit available inside the pool? */
#endif

} trans_unit_t;

#ifdef USE_RETRIEVE_UNIT

/**
 * Structure for transport unit container.
 * @struct trans_unit_container
 */

typedef struct trans_unit_container {
  struct trans_unit_container  *next;  /**< pointer to next container */
  struct trans_unit u;        /**< transport unit */
} trans_unit_container_t;
#endif

/**
 * Structure for transport block.
 * @struct trans_block
 */

typedef struct trans_block {

  unsigned int sbn;        /**< source block number */
  struct trans_unit *unit_list;  /**< pointer to first unit for this block */
  struct trans_unit  *last_unit;  /**< pointer to last unit for this block */
  unsigned int nb_of_rx_units;  /**< number of received units for this block */
  unsigned int n;        /**< number of encoding symbols for this block */
  unsigned int k;        /**< number of source symbols for this block*/
  unsigned int max_k;      /**< maximum source block length */
  unsigned int max_n;      /**< maximum number of encoding symbols */
  unsigned char finite_field;  /**< finite field parameter*/

  //Malek EL Khatib 11.08.2014
  unsigned int nb_of_rx_symbols;/**<Add field to determine number of rx symbols in case multiple ones are sent per packet and processed simultaneously>*/
  //End

} trans_block_t;

/**
 * Structure for transport object.
 * @struct trans_obj
 */

typedef struct trans_obj {

  struct trans_obj *prev;      /**< pointer to previous object */
  struct trans_obj *next;      /**< pointer to next object */
  struct trans_block *block_list;  /**< pointer to the transport block list */
  unsigned int nb_of_ready_blocks;  /**< number of ready blocks for this object */
  unsigned char fec_enc_id;      /**< FEC encoding id */
  unsigned short fec_inst_id;    /**< FEC instance id */
  unsigned char content_enc_algo;  /**< content encoding algorithm */
  unsigned long long len;      /**< transfer length*/
  unsigned long long rx_bytes;    /**< received bytes */
  unsigned long long toi;      /**< transport object identifier */
  unsigned int es_len;        /**< encoding symbol length */
  unsigned int max_sb_len;      /**< maximum source block length */
  struct blocking_struct  *bs;    /**< blocking structure */
  char *tmp_filename;        /**< temporary filename for this object */
  int fd;              /**< file descriptor to be used for file saving */
  char *tmp_st_filename;      /**< temporary filename for the symbol store */
  int fd_st;            /**< file descriptor for the symbol store */
  double last_print_rx_percent;    /**< last printed received percent*/
  
} trans_obj_t;


/**
 * This function creates new transport object structure.
 *
 * @return pointer to created transport object in success, NULL otherwise
 *
 */

trans_obj_t* create_object(void);

/**
 * This function creates new transport block structure.
 *
 * @return pointer to created transport block in success, NULL otherwise
 *
 */

trans_block_t* create_block(void);

/**
 * This function creates new transport unit structures.
 *
 * @param number number of units to be created
 *
 * @return pointer to created transport unit/tranport unit list in success, 
 * NULL otherwise
 *
 */

trans_unit_t* create_units(unsigned int number);

/**
 * This function frees transport units from transport block.
 *
 * @param tb pointer to transport block
 *
 */

void free_units(trans_block_t *tb);

#ifdef USE_RETRIEVE_UNIT

/**
 * This function looks for a transport unit structure in the transport block pool.
 *
 * @param s pointer to the current session from where extract to transport units
 * @param es_len wanted encoding symbol length
 *
 * @return pointer to available (or created) unit in success, NULL otherwise.
 *
 */

trans_unit_t* retrieve_unit(alc_session_t *s, unsigned short es_len);

/**
 * This function frees transport units from transport block. This version just marks the
 * transport units available.
 *
 * @param tb pointer to the transport block
 *
 */

void free_units2(trans_block_t *tb);
#endif

/**
 * This function inserts transport object to the session.
 *
 * @param to pointer to transport object to be inserted
 * @param s pointer to the session
 * @param type type of object to be inserted (0 = FDT Instance, 1 = normal object)  
 *
 */

void insert_object(trans_obj_t *to, alc_session_t *s, int type);

/**
 * This function inserts transport unit to transport block.
 *
 * @param tu pointer to transport unit to be inserted
 * @param tb pointer to transport block
 * @param to pointer to transport object
 *
 * @return 0 when transport unit is inserted, 1 when duplicated transport unit
 *
 */

int insert_unit(trans_unit_t *tu, trans_block_t *tb, trans_obj_t *to);

/**
 * This function frees transport object structure.
 *
 * @param to pointer to transport object to be freed
 * @param s pointer to the session
 * @param type type of the object to be freed (0 = FDT Instance, 1 = normal object)
 *
 */

void free_object(trans_obj_t *to, alc_session_t *s, int type);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
