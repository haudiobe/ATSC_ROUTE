/** \file xor_fec.c \brief Simple XOR FEC
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.23 $
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
#include <math.h>
#include <memory.h>

#include "xor_fec.h"

trans_block_t* xor_fec_encode_src_block(char *data, unsigned long long len,
										unsigned int sbn, unsigned short es_len) {
		
	trans_block_t *tr_block;		/* transport block struct */
	trans_unit_t *tr_unit;			/* transport unit struct */
	unsigned int nb_of_units;		/* number of units */

	unsigned int i;					/* loop variables */
	unsigned long long data_left;

	char *ptr;					/* pointer to left data */
	char *parity_symb;
    char *padded_symb;
	int j;

	data_left = len;

    parity_symb = (char*)calloc(es_len, sizeof(char));
	padded_symb = (char*)calloc(es_len, sizeof(char));

	nb_of_units = (unsigned int)ceil((double)(unsigned int)len / (double)es_len);

	tr_block = create_block();

	if(tr_block == NULL) {
		return tr_block;
	}

	tr_unit = create_units(nb_of_units + 1); /* One parity symbol */

	if(tr_unit == NULL) {
		free(tr_block);
		return NULL;
	}

	ptr = data;

	tr_block->unit_list = tr_unit;
	tr_block->sbn = sbn;
    tr_block->k = nb_of_units;
	tr_block->n = nb_of_units + 1;
		
	for(i = 0; i < nb_of_units; i++) {

		tr_unit->esi = i;
		tr_unit->len = data_left < es_len ? (unsigned short)data_left : es_len; /*min(es_len, data_left);*/

		/* Alloc memory for TU data */
		if(!(tr_unit->data = (char*)calloc(tr_unit->len, sizeof(char)))) {
			printf("Could not alloc memory for transport unit's data!\n");
			
			tr_unit = tr_block->unit_list;	

			while(tr_unit != NULL) {
				free(tr_unit->data);
				tr_unit++;
			}
	
			free(tr_block->unit_list);
			free(tr_block);
			free(parity_symb);
			free(padded_symb);
			return NULL;
		}

		memcpy(tr_unit->data, ptr, tr_unit->len);

		memset(padded_symb, 0, es_len);
		memcpy(padded_symb, tr_unit->data, tr_unit->len);
		
		if(i == 0) {
			memcpy(parity_symb, tr_unit->data, tr_unit->len);
		}
		else {
			/* We need to create the parity symbol by XORing the symbols, first symbol is not XORed. */

			for(j = 0; j < es_len; j++) {
				*(parity_symb + j) = *(parity_symb + j) ^ *(padded_symb + j);
			}
		}

		ptr += tr_unit->len;
		data_left -= tr_unit->len;
		tr_unit++;

		if (i == (nb_of_units-1)) {
			
	        /* Now we need to add the parity symbol to the block (XOR of all other symbols). */

	                tr_unit->esi = i+1;
        	        tr_unit->len = es_len;

                	/* Alloc memory for TU data */
                	if(!(tr_unit->data = (char*)calloc(es_len, sizeof(char)))) {
                        	printf("Could not alloc memory for transport unit's data!\n");

                        	tr_unit = tr_block->unit_list;

                        	while(tr_unit != NULL) {
                                	free(tr_unit->data);
                                	tr_unit++;
                        	}

                        	free(tr_block->unit_list);
                        	free(tr_block);
	                        free(parity_symb);
        	                free(padded_symb);
                        	return NULL;
                	}

                	memcpy(tr_unit->data, parity_symb, es_len);
		}
	}

        free(parity_symb);
        free(padded_symb);

	return tr_block;
}

char *xor_fec_decode_src_block(trans_block_t *tr_block, unsigned long long *block_len,
							   unsigned short es_len) {

  char *buf = NULL; /* buffer where to construct the source block from data units */

  trans_unit_t *next_tu = NULL;
  trans_unit_t *tu = NULL;

  unsigned long long len = 0;
  unsigned long long tmp = 0;

    int last_esi = -1;
    int missing_esi = -1;
    
    char *missing_symb = NULL;
    char *padded_symb = NULL;
    trans_unit_t *missing_unit = NULL;
    unsigned int j = 0;

    missing_symb = (char*)calloc(es_len, sizeof(char));
    padded_symb = (char*)calloc(es_len, sizeof(char));

    memset(missing_symb, 0, es_len);
    memset(padded_symb, 0, es_len);

    len = es_len*tr_block->k;

    /* Allocate memory for buf */
    if(!(buf = (char*)calloc((unsigned int)(len + 1), sizeof(char)))) {
        printf("Could not alloc memory for buf!\n");
        return NULL;
    }

    tmp = 0;

	/* We must first find out is there a parity symbol */

	next_tu = tr_block->unit_list;
 
	/* We must scroll to the last symbol */

        while(next_tu != NULL) {

                tu = next_tu;
                next_tu = tu->next;
        }

        if(tu->esi == tr_block->k) { /* There is a parity symbol */
		
		/* We need to find out which symbol is missing */
		
		next_tu = tr_block->unit_list;	

		while(next_tu != NULL) {

        	        tu = next_tu;

			if((int)tu->esi != (last_esi + 1)) {
				missing_esi = (tu->esi - 1);
				break;
			}

                	next_tu = tu->next;
			last_esi = tu->esi;
	        }

		/* Now we need to construct the missing symbol */

                next_tu = tr_block->unit_list;

                while(next_tu != NULL) {

                        tu = next_tu;

	                memset(padded_symb, 0, es_len);
			memcpy(padded_symb, tu->data, tu->len);

                        for(j = 0; j < es_len; j++) {
                                *(missing_symb + j) = *(missing_symb + j) ^ *(padded_symb + j);
                        }

                        next_tu = tu->next;
                }

		/* Now we need to create the missing unit */

		missing_unit = create_units(1);

		missing_unit->data = (char*)calloc(es_len, sizeof(char));

		missing_unit->esi = missing_esi;
		memcpy(missing_unit->data, missing_symb, es_len);		
		missing_unit->len = es_len;

		/* Now we need to insert the missing symbol to the block */

                next_tu = tr_block->unit_list;

                while(next_tu != NULL) {

                        tu = next_tu;

                        if(missing_esi == 0) { /* The first symbol was missing */

				tr_block->unit_list = missing_unit;
				missing_unit->next = tu;
				tu->prev = missing_unit;	
                                break;
                        }

			if((int)tu->esi > missing_esi) { /* It was the previous symbol which was missing */

				tu->prev->next = missing_unit;
			        missing_unit->prev = tu->prev;
				missing_unit->next = tu;
				tu->prev = missing_unit;
				break;
			}

                        next_tu = tu->next;
                }

		/* Now we need to remove the parity symbol from the block */

		tu = tr_block->unit_list;
		
        	for(j = 0; j < tr_block->k; j++) {

			if(tu->esi == (tr_block->k - 1)) { /* Second last symbol */

#ifndef USE_RETRIEVE_UNIT
				free(tu->next->data);
				free(tu->next);
#else
                tu->next->used = 0;
#endif

				tu->next = NULL;
				break;
			}

                	tu = tu->next;
        	}

		        next_tu = tr_block->unit_list;

	}

	next_tu = tr_block->unit_list;

	while(next_tu != NULL) {

        	tu = next_tu;
        	memcpy((buf+(unsigned int)tmp), tu->data, tu->len);

#ifndef USE_RETRIEVE_UNIT
        free(tu->data);
        tu->data = NULL;
#endif
        	tmp += tu->len;

        	next_tu = tu->next;
	}

	*block_len = len;

        free(missing_symb);
        free(padded_symb);

	return buf;
}

char *xor_fec_decode_object(trans_obj_t *to, unsigned long long *data_len, alc_session_t *s) {
	
	char *object = NULL;
	char *block = NULL;

	trans_block_t *tb;

	unsigned long long to_data_left;
	unsigned long long len;
	unsigned long long block_len;
	unsigned long long position;

	unsigned int i;
	
	/* Allocate memory for buf */
	if(!(object = (char*)calloc((unsigned int)(to->len+1), sizeof(char)))) {
		printf("Could not alloc memory for buf!\n");
		return NULL;
	}
	
	to_data_left = to->len;

	tb = to->block_list;
	position = 0;
	
	for(i = 0; i < to->bs->N; i++) {

		block = xor_fec_decode_src_block(tb, &block_len, (unsigned short)to->es_len);

		/* the last packet of the last source block might be padded with zeros */
		len = to_data_left < block_len ? to_data_left : block_len;

		memcpy(object+(unsigned int)position, block, (unsigned int)len);
		position += len;
		to_data_left -= len;

		free(block);
		tb = to->block_list+(i+1);
	}
	
	*data_len = to->len;
	return object;
}

