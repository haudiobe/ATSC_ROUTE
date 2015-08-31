/** \file parse_query_string.c \brief Functions for query string parsing 
 *
 * $Author: peltotal $ $Date: 2007/02/27 12:50:48 $ $Revision: 1.9 $
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parse_query_string.h"

/**
 * This is a private function which checks if missing block already exists in the list.
 *
 * @param sbn source block number
 * @param block_list list of missing blocks
 *
 * @return missing_block structure if exists, NULL if not
 *
 */

qs_missing_block_t* missing_block_exist(unsigned int sbn, qs_missing_block_t *block_list) {

  qs_missing_block_t *block;

  block = block_list;

  if(block != NULL) {
    for(;;) {
      if(block->sbn == sbn) {
        break;
      }           
      if(block->next == NULL) {
        block = NULL;
        break;
      }
      block = block->next;
    }
  }

  return block;
}

/**
 * This is a private function which adds missing block to the list.   
 *               
 * @param block missing block
 * @param qs query string structure, which contains also missing block list
 *
 */

void insert_missing_block(qs_missing_block_t *block, query_str_t *qs) {

  qs_missing_block_t *mb;

  mb = qs->block_list;

  while(1) {

    if(mb == NULL) {               
      qs->block_list = block;
      break;                
    }

    if(mb->next == NULL) {
        mb->next = block;  
        break;   
    }  
    mb = mb->next;
  }
}

/**
 * This is a private function which adds missing block to the list.
 *
 * @param symbol missing symbol             
 * @param block missing block
 *   
 */

void insert_missing_symbol(qs_missing_symbol_t *symbol, qs_missing_block_t *block) {

  qs_missing_symbol_t *ms;

  ms = block->es_list;

  while(1) {

    if(ms == NULL) {
      block->es_list = symbol;
      break;
    }

    if(ms->next == NULL) {
        ms->next = symbol;
        break;
    }
    ms = ms->next;
  }
}

query_str_t* parse_query_string(char *query_str) { 
  query_str_t *qs = NULL;

  char *chr = NULL;
  char *part = NULL;
  char *fileURI = NULL;
  char *sbn_list[100] = {NULL};
  int nb_of_sbn_part = 0;

  int search_pos = 0;
  int i;
  int j;

  int first_block;
  int last_block;

  int first_symbol;
  int last_symbol;

  qs_missing_block_t *ms_blck = NULL;
  qs_missing_symbol_t *ms_symb = NULL;

  unsigned int sbn;

  if (!(qs = (query_str_t*)calloc(1, sizeof(query_str_t)))) {
    return NULL;
  }

  qs->fileURI = NULL;	
  qs->block_list = NULL;

  part = strtok(query_str, "&");
  fileURI = part;

  part = strtok(NULL, "&");
  
  while(part != NULL) {
    sbn_list[nb_of_sbn_part] = part;
    nb_of_sbn_part++;
    part = strtok(NULL, "&");
  }

  /* fileURI */

  part = strtok(fileURI, "=");

  if(strcmp(part, "fileURI") == 0) {
    part = strtok(NULL, "&");
    qs->fileURI = parse_uri(part, strlen(part));
  }

  if(qs->fileURI == NULL) {
    free_query_str(qs);
    return NULL;
  }

  /* missing SBN and ESI part */

  for(i = 0; i < nb_of_sbn_part; i++) {

    search_pos = 0;
    part = strtok(sbn_list[i], "=");
  
    if(strcmp(part, "SBN") == 0) {

      search_pos += strlen(part);
      search_pos++;

      chr = strchr((sbn_list[i] + search_pos), ';');

      if(chr != NULL) {

	part = strtok(NULL, ";");

        search_pos += strlen(part);
        search_pos++;

       	sbn = atoi(part);

	ms_blck = missing_block_exist(sbn, qs->block_list);
	
	if(ms_blck == NULL) {

	  if(!(ms_blck = (qs_missing_block_t*)calloc(1, sizeof(qs_missing_block_t)))) {
	    free_query_str(qs);
	    return NULL;
	  }
	  
	  ms_blck->sbn = sbn;
	  ms_blck->next = NULL;
	  ms_blck->es_list = NULL;

	  insert_missing_block(ms_blck, qs);
	}

	part = strtok(NULL, "=");

	search_pos += strlen(part);
	search_pos++;

	if(strcmp(part, "ESI") == 0) {

	  chr = strchr((sbn_list[i] + search_pos), '-');

	  if(chr != NULL) {
	    
	    part = strtok(NULL, "-");
	    
	    first_symbol = atoi(part);
	    
	    part = strtok(NULL, " ");
	    
	    last_symbol = atoi(part);
	    
	    for(j = first_symbol; j < (last_symbol + 1); j++) {
	      
	      if (!(ms_symb = (qs_missing_symbol_t*)calloc(1, sizeof(qs_missing_symbol_t)))) {
		free_query_str(qs);
		return NULL;
	      }
	      
	      ms_symb->esi = j;
	      ms_symb->next = NULL;
	
	      insert_missing_symbol(ms_symb, ms_blck);
            }
	  }
	  else {
	    
	    part = strtok(NULL, ",");

	    while(part != NULL) {
	      
	      if (!(ms_symb = (qs_missing_symbol_t*)calloc(1, sizeof(qs_missing_symbol_t)))) {
		free_query_str(qs);
		return NULL;
	      }
	      
	      ms_symb->esi = atoi(part);
	      ms_symb->next = NULL;
	      
	      insert_missing_symbol(ms_symb, ms_blck);   

	      part = strtok(NULL, ",");
	    }
	  }
	}
      }
      else {

	chr = strchr(sbn_list[i] + search_pos, '-');
	
	if(chr != NULL) {

	  part = strtok(NULL, "-");
	
	  first_block = atoi(part);

	  part = strtok(NULL, " ");

	  last_block = atoi(part);

	  for(j = first_block; j < (last_block + 1); j++) {
	    ms_blck = missing_block_exist(j, qs->block_list);

	    if(ms_blck == NULL) {   

	      if(!(ms_blck = (qs_missing_block_t*)calloc(1, sizeof(qs_missing_block_t)))) {
		free_query_str(qs);
		return NULL;
	      }

	      ms_blck->sbn = j;
	      ms_blck->next = NULL;
	      ms_blck->es_list = NULL;
	  
	      insert_missing_block(ms_blck, qs);
	    }
	  }
	}
	else {
	  
	  part = strtok(NULL, " ");
	  sbn = atoi(part);
	  
	  ms_blck = missing_block_exist(sbn, qs->block_list);

	  if(ms_blck == NULL) {

	    if(!(ms_blck = (qs_missing_block_t*)calloc(1, sizeof(qs_missing_block_t)))) {
	      free_query_str(qs);
	      return NULL;
	    }
	    	    
	    ms_blck->sbn = sbn;
	    ms_blck->next = NULL;
	    ms_blck->es_list = NULL;

            insert_missing_block(ms_blck, qs); 
	  }
	}
      }
    }
  }
 
  return qs;
}

void free_query_str(query_str_t *qs) {

  qs_missing_block_t *mb;
  qs_missing_block_t *next_mb;
  qs_missing_symbol_t *ms;
  qs_missing_symbol_t *next_ms;

  if(qs->fileURI != NULL) {
    free_uri(qs->fileURI);
  }

  if(qs->block_list != NULL) {

    next_mb = qs->block_list;
    
    while(next_mb != NULL) {
      mb = next_mb;
      
      next_ms = mb->es_list;
      
      while(next_ms != NULL) {
	ms = next_ms;
	next_ms = ms->next;
	free(ms);
      }
      
      next_mb = mb->next;
      free(mb);
    }
  }
  
  if(qs != NULL) {
    free(qs);
  }
}





