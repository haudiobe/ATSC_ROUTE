/** \file parse_query_string.h \brief Functions for query string parsing
 *
 *  $Author: peltotal $ $Date: 2007/02/27 12:50:48 $ $Revision: 1.9 $
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _PARSE_QUERY_STRING_
#define _PARSE_QUERY_STRING_

#include "../../flutelib/uri.h"

/** Structure for the missing symbols
 * @struct qs_missing_symbol  
 */

typedef struct qs_missing_symbol {
  struct qs_missing_symbol *next; /**< next missing symbol */ 
  unsigned int esi; /**< encoding symbol identifier */
} qs_missing_symbol_t;

/** Structure for the missing block
 * @struct qs_missing_block
 */

typedef struct qs_missing_block {
  struct qs_missing_block *next; /**< next missing block */
  unsigned int sbn; /**< source block number */
  struct qs_missing_symbol *es_list; /**< list of missing symbol for this block */
} qs_missing_block_t;

/** Structure for the query string
 * @struct query_str
 */

typedef struct query_str {
  uri_t *fileURI;  /**< fileURI */
  qs_missing_block_t *block_list; /**< list of missing blocks */
} query_str_t;


/**
 * This function parses query string.
 *
 * @param query_str query string to be parsed
 *
 * @return query_string structure, NULL in error cases
 *
 */

query_str_t* parse_query_string(char *query_str);

/**
 * This function frees query_string structure.
 *
 * @param qs query_string structure to be freed      
 *
 */

void free_query_str(query_str_t *qs);

#endif
