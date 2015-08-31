/** \file rs_fec.c \brief Reed-Solomon FEC
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.37 $
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
#include <memory.h>
#include <assert.h>

#include "rs_fec.h"
#include "fec.h"

trans_block_t* rs_fec_encode_src_block(char *data, unsigned long long len, unsigned int sbn,
									   unsigned short es_len, int rs, unsigned int max_sb_len) {

    trans_block_t *tr_block;                /* transport block struct */
    trans_unit_t *tr_unit;                  /* transport unit struct */
    unsigned int i;                         /* loop variables */
    char *ptr;                              /* pointer to left data */
    void *code;
    char *src[GF_SIZE];
    unsigned int k;                         /* number of source symbols */
    unsigned int n;                         /* number of encoding symbols */
    unsigned int max_k;
    unsigned int max_n;
    unsigned long long data_left;
    div_t div_k;
    div_t div_max_n;
    div_t div_n;

	data_left = len;
    max_k = max_sb_len;

    div_k = div((unsigned int)len, es_len);

    if(div_k.rem == 0) {
            k = (unsigned int)div_k.quot;
    }
    else {
            k = (unsigned int)div_k.quot + 1;
    }

    div_max_n = div((max_k * (100 + rs)), 100);
    max_n = (unsigned int)div_max_n.quot;

    div_n = div((k * max_n), (max_k));
    n = (unsigned int)div_n.quot;

    code =  fec_new(k, n);
	tr_block = create_block();

    if(tr_block == NULL) {
            return tr_block;
    }

    tr_unit = create_units(n);

    if(tr_unit == NULL) {
            free(tr_block);
            return NULL;
    }

    ptr = data;

    tr_block->unit_list = tr_unit;
    tr_block->sbn = sbn;
    tr_block->n = n;

    tr_block->k = k;
    tr_block->max_k = max_k;
    tr_block->max_n = max_n;

    for(i = 0; i < k; i++) {
           tr_unit->esi = i;
            tr_unit->len = data_left < es_len ? (unsigned short)data_left : es_len;

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
                    return NULL;
            }

            memcpy(tr_unit->data, ptr, tr_unit->len);

            src[i] = tr_unit->data;

            ptr += tr_unit->len;
            data_left -= tr_unit->len;
            tr_unit++;
    }
  /* let's create FEC units */

    for (i = 0; i < (n - k); i++) {

            tr_unit->esi = k+i;
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
                    return NULL;
            }

            fec_encode(code, (void**)src, tr_unit->data, k+i, es_len);

            tr_unit++;
    }
    fec_free(code);

    return tr_block;
}

char *rs_fec_decode_src_block(trans_block_t *tr_block, unsigned long long *block_len,
							  unsigned short es_len) {

    char *buf = NULL; /* buffer where to construct the source block from data units */
    trans_unit_t *next_tu;
    trans_unit_t *tu;

    unsigned long long len;
    void    *code;
    char    *dst[GF_SIZE];
    int             index[GF_SIZE];
    unsigned int i = 0;
    unsigned int k;
    unsigned int n;
    div_t div_n;

    k = tr_block->k;

    div_n = div((k * tr_block->max_n), tr_block->max_k);
    n = (unsigned int)div_n.quot;

    len = es_len*tr_block->k;

    code =  fec_new(k, n);

    /* Allocate memory for buf */
    if(!(buf = (char*)calloc((unsigned int)(len + 1), sizeof(char)))) {
            printf("Could not alloc memory for buf!\n");
            *block_len = 0;
            return NULL;
    }

    next_tu = tr_block->unit_list;

    while(next_tu != NULL) {

            tu = next_tu;
            dst[i] = tu->data;
            index[i] = tu->esi;

            next_tu = tu->next;
            i++;
    }

    /* Let's decode source block using Reed-Solomon FEC */

    fec_decode(code, (void**)dst, index, es_len);

    fec_free(code);

    /* Copy decoded encoding symbols to buffer */

    for(i = 0; i < k; i++) {
            memcpy(buf + i*es_len, dst[i], es_len);
    }

    next_tu = tr_block->unit_list;

#ifndef USE_RETRIEVE_UNIT
    while(next_tu != NULL) {
		tu = next_tu;
		free(tu->data);
		tu->data = NULL;
        next_tu = tu->next;
    }
#endif

   *block_len = len;

    return buf;
}



char *rs_fec_decode_object(trans_obj_t *to, unsigned long long *data_len, alc_session_t *s) {

    char *object = NULL; /* buffer where to construct the object */
    char *block = NULL; /* buffer where to contruct the object */

    trans_block_t *tb;

    unsigned long long len;
    unsigned long long position;
    unsigned long long to_data_left;
    unsigned long long block_len;
    unsigned int i;

    /* Allocate memory for object */
    if(!(object = (char*)calloc((unsigned int)(to->len + 1), sizeof(char)))) {
            printf("Could not alloc memory for object!\n");
            *data_len = 0;
            return NULL;
    }

    position = 0;
    to_data_left = to->len;

    tb = to->block_list;

    /*while(tb != NULL) {*/
    for(i = 0; i < to->bs->N; i++) {
            block = rs_fec_decode_src_block(tb, &block_len, (unsigned short)to->es_len);

            /* the last packet of the last source block might be padded with zeros */
            len = to_data_left < block_len ? to_data_left : block_len;

			assert(0 <= position);
			assert(position < to->len+1);
			assert(len <= (to->len-position));

            memcpy(object+(unsigned int)position, block, (unsigned int)len);
            position += len;
            to_data_left -= len;

            free(block);
            block = NULL;

            /*tb = tb->next;*/
            tb = to->block_list+(i+1);
    }

    *data_len = to->len;
    return object;
}
