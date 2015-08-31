/** \file fdt.c \brief FDT parsing
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.35 $
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <expat.h>

#include "../alclib/fec.h"

#include "fdt.h"
#include "mad_utf8.h"
	
fdt_t *fdt;				/**< FDT */
file_t *file;			/**< file */
file_t *prev;			/**< previous parsed file */
BOOL is_first_toi;		/**< is first TOI parsed or not? */

/**
 * Global variables semaphore
 */

#ifdef _MSC_VER
RTL_CRITICAL_SECTION global_variables_semaphore;
#else
pthread_mutex_t global_variables_semaphore = PTHREAD_MUTEX_INITIALIZER;
#endif

/**
 * This is a private function, which locks the FDT.
 *
 */

void lock_fdt(void) {
#ifdef _MSC_VER
	EnterCriticalSection(&global_variables_semaphore);
#else
	pthread_mutex_lock(&global_variables_semaphore);
#endif
}

/**
 * This is a private function, which unlocks the FDT.
 *
 */

void unlock_fdt(void) {
#ifdef _MSC_VER
	LeaveCriticalSection(&global_variables_semaphore);
#else
	pthread_mutex_unlock(&global_variables_semaphore);
#endif
}

/**
 * This is a private function which copies file description from source to destination. 
 *
 * @param src pointer to source file structure
 * @param dest pointer to destination file structure
 *
 * @return 1 if file description is updated, 0 if not, and -1 in error cases
 *
 */

int copy_file_info(file_t *src, file_t *dest) {

	int updated = 0;

	/* Copy only if particular field is not present in destination, so file description can be only
	complemented not modified */

	if(src->toi != 0) {
		if(dest->toi == 0) {
			dest->toi = src->toi;
			updated = 1;
		}
	}

	if(src->expires != 0) {
		if(dest->expires == 0) {
			dest->expires = src->expires;
			updated = 1;
		}
	}

	if(src->transfer_len != 0) {
		if(dest->transfer_len == 0) {
			dest->transfer_len = src->transfer_len;
			updated = 1;
		}
	}

	if(src->content_len != 0) {
		if(dest->content_len == 0) {
			dest->content_len = src->content_len;
			updated = 1;
		}
	}

	if(src->fec_enc_id != -1) {
		if(dest->fec_enc_id == -1) {
			dest->fec_enc_id = src->fec_enc_id;
			updated = 1;
		}
	}

	if(src->fec_inst_id != -1) {
		if(dest->fec_inst_id == -1) {
			dest->fec_inst_id = src->fec_inst_id;
			updated = 1;
		}
	}

	if(src->max_sb_len != 0) {
		if(dest->max_sb_len == 0) {
			dest->max_sb_len = src->max_sb_len;
			updated = 1;
		}
	}

	if(src->es_len != 0) {
		if(dest->es_len == 0) {
			dest->es_len = src->es_len;
			updated = 1;
		}
	}

	if(src->max_nb_of_es != 0) {
		if(dest->max_nb_of_es == 0) {
			dest->max_nb_of_es = src->max_nb_of_es;
			updated = 1;
		}
	}

	if(src->location != NULL) {

		if(dest->location == NULL) {

			if(!(dest->location  = (char*)calloc((strlen(src->location) + 1), sizeof(char)))) {
				printf("Could not alloc memory for file->location!\n");
				return -1;
			}

			memcpy(dest->location, src->location, strlen(src->location));
			updated = 1;
		}
	}

	if(src->type != NULL) {

		if(dest->type == NULL) {

			if(!(dest->type  = (char*)calloc((strlen(src->type) + 1), sizeof(char)))) {
				printf("Could not alloc memory for file->type!\n");
				return -1;
			}

			memcpy(dest->type, src->type, strlen(src->type));
			updated = 1;
		}
	}

	if(src->md5 != NULL) {

		if(dest->md5 == NULL) {

			if(!(dest->md5 = (char*)calloc((strlen(src->md5) + 1), sizeof(char)))) {
				printf("Could not alloc memory for file->md5!\n");
				return -1;
			}

			memcpy(dest->md5, src->md5, strlen(src->md5));
			updated = 1;
		}
	}

	if(src->encoding != NULL) {

		if(dest->encoding == NULL) {

			if(!(dest->encoding = (char*)calloc((strlen(src->encoding) + 1), sizeof(char)))) {
				printf("Could not alloc memory for file->encoding!\n");
				return -1;
			}

			memcpy(dest->encoding, src->encoding, strlen(src->encoding));
			updated = 1;
		}
	}

	return updated;
}

/**
 * This is a private function which is used in FDT parsing.
 *
 * @param userData not used, must be
 * @param name pointer to buffer containing element's name
 * @param atts pointer to buffer containing element's attributes
 *
 */

static void startElement_FDT(void *userData, const char *name, const char **atts) {

#ifndef _MSC_VER
	char *ep;
#endif

	char *mbstr;

	while(*atts != NULL) {
		if(!strcmp(name, "File")) {

			if(file == NULL) {
				if(!(file = (file_t*)calloc(1, sizeof(file_t)))) {
					printf("Could not alloc memory for mad_fdt file!\n");
					return;
				}

				/* initialise file parameters */
				file->prev = NULL;
				file->next = NULL;
				file->toi = 0;
				file->status = 0;
				file->transfer_len = 0;
				file->content_len = 0;
				file->location = NULL;
				file->md5 = NULL;
				file->type = NULL;
				file->encoding = NULL;

				file->expires = fdt->expires;

				file->fec_enc_id = fdt->fec_enc_id;
				file->fec_inst_id = fdt->fec_inst_id;
				file->finite_field = fdt->finite_field;
				file->nb_of_es_per_group = fdt->nb_of_es_per_group;
				file->max_sb_len = fdt->max_sb_len;
				file->es_len = fdt->es_len;
				file->max_nb_of_es = fdt->max_nb_of_es;

				fdt->nb_of_files++;
			}

			if(!strcmp(*atts, "TOI")) {

#ifdef _MSC_VER    
				file->toi = _atoi64(*(++atts));

				if(file->toi > (unsigned long long)0xFFFFFFFFFFFFFFFF) {
					printf("TOI too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}
#else               
				file->toi = strtoull(*(++atts), &ep, 10);

				if(*(atts) == '\0' || *ep != '\0') {
					printf("TOI not a number\n");
					fflush(stdout);
					return;
				}

				if(errno == ERANGE && file->toi == 0xFFFFFFFFFFFFFFFFULL) {
					printf("TOI too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}
#endif

				if(is_first_toi) {
					fdt->file_list = file;
					is_first_toi = FALSE;
				}
				else {
					prev->next = file;
					file->prev = prev;
				}

				prev = file;
			}
			else if(!strcmp(*atts, "Content-Location")) {

				atts++;

				if(!(mbstr = (char*)calloc((strlen(*atts)+ 1), sizeof(char)))) {
					printf("Could not alloc memory for mbstr!\n");
					return;
				}

				x_utf8s_to_iso_8859_1s(mbstr, *atts, strlen(*atts));

				if(!(file->location = (char*)calloc((size_t)(strlen(mbstr) + 1), sizeof(char)))) {
					printf("Could not alloc memory for file->location!\n");
					return;
				}

				memcpy(file->location, mbstr, strlen(mbstr));
				free(mbstr);

			}
			else if(!strcmp(*atts, "Content-Length")) {

#ifdef _MSC_VER     
				file->content_len = _atoi64(*(++atts));

				if(file->content_len > (unsigned long long)0xFFFFFFFFFFFFFFFF) {
					printf("Content-Length too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}
#else               
				file->content_len = strtoull(*(++atts), &ep, 10);

				if(*(atts) == '\0' || *ep != '\0') {
					printf("Content-Length not a number\n");
					fflush(stdout);
					return;
				}

				if(errno == ERANGE && file->content_len == 0xFFFFFFFFFFFFFFFFULL) {
					printf("Content-Length too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}	
#endif
				if(file->transfer_len == 0) {
					file->transfer_len = file->content_len;
				}
			}
			else if(!strcmp(*atts, "Transfer-Length")) {

#ifdef _MSC_VER			  
				file->transfer_len = _atoi64(*(++atts));

				if(file->transfer_len > (unsigned long long)0xFFFFFFFFFFFFFFFF) {
					printf("Transfer-Length too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}
#else
				file->transfer_len = strtoull(*(++atts), &ep, 10);

				if(*(atts) == '\0' || *ep != '\0') {
					printf("Transfer-Length not a number\n");
					fflush(stdout);
					return;
				}

				if(errno == ERANGE && file->transfer_len == 0xFFFFFFFFFFFFFFFFULL) {
					printf("Transfer-Length too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}
#endif 

#ifdef _MSC_VER
				if(file->transfer_len > (unsigned long long)0xFFFFFFFFFFFF) {
					printf("Transfer-Length too big (max=%I64u)\n", (unsigned long long)0xFFFFFFFFFFFF);
#else
				if(file->transfer_len > 0xFFFFFFFFFFFFULL) {
					printf("Transfer-Length too big (max=%llu)\n", 0xFFFFFFFFFFFFULL);
#endif
					fflush(stdout);
					return;
				}
			}
			else if(!strcmp(*atts, "Content-Type")) {

				atts++;

				if(!(file->type = (char*)calloc((strlen(*atts) + 1), sizeof(char)))) {
					printf("Could not alloc memory for file->type!\n");
					return;
				}

				memcpy(file->type, *atts, strlen(*atts));
			}
			else if(!strcmp(*atts, "Content-Encoding")) {

				atts++;

				if(!(file->encoding = (char*)calloc((strlen(*atts) + 1), sizeof(char)))) {
					printf("Could not alloc memory for file->encoding!\n");
					return;
				}

				memcpy(file->encoding, *atts, strlen(*atts));
			}
			else if(!strcmp(*atts, "Content-MD5")) {

				atts++;

				if(!(file->md5 = (char*)calloc((strlen(*atts) + 1), sizeof(char)))) {
					printf("Could not alloc memory for file->md5!\n");
					return;
				}

				memcpy(file->md5, *atts, strlen(*atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-FEC-Encoding-ID")) {
				file->fec_enc_id = (unsigned char)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-FEC-Instance-ID")) {
				file->fec_inst_id = (unsigned short)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Maximum-Source-Block-Length")) {
				file->max_sb_len = (unsigned int)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Encoding-Symbol-Length")) {
				file->es_len = (unsigned short)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Max-Number-of-Encoding-Symbols")) {
				file->max_nb_of_es = (unsigned short)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Number-of-Encoding-Symbols-per-Group")) {
				file->nb_of_es_per_group = (unsigned char)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Finite-Field-Parameter")) {
				file->finite_field = (unsigned char)atoi(*(++atts));
			}
			else {			  
				atts++;
			}

			atts++;

			/* copy common parameters from FDT to File when we are leaving File element */

			if(*atts == NULL) {

				if(file->type == NULL && fdt->type != NULL) {
					if(!(file->type = (char*)calloc((strlen(fdt->type) + 1), sizeof(char)))) {
						printf("Could not alloc memory for file->type!\n");
						return;
					}
					memcpy(file->type, fdt->type, strlen(fdt->type));
				}
				if(file->encoding == NULL && fdt->encoding != NULL) {
					if(!(file->encoding = (char*)calloc((strlen(fdt->encoding) + 1), sizeof(char)))) {
						printf("Could not alloc memory for file->encoding!\n");
						return;
					}
					memcpy(file->encoding, fdt->encoding, strlen(fdt->encoding));
				}
			}

		}
		else if(!strcmp(name, "FDT-Instance")) {

			if(!strcmp(*atts, "Expires")) {  
#ifdef _MSC_VER
				fdt->expires = _atoi64(*(++atts));

				if(fdt->expires > (unsigned long long)0xFFFFFFFFFFFFFFFF) {
					printf("Expires too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}
#else
				/*fdt->expires = atoll(*(++atts));*/

				fdt->expires = strtoull(*(++atts), &ep, 10);

				if(*(atts) == '\0' || *ep != '\0') {
					printf("Expires not a number\n");
					fflush(stdout);
					return;
				}

				if(errno == ERANGE && fdt->expires == 0xFFFFFFFFFFFFFFFFULL) {
					printf("Expires too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}
#endif
			}
			else if(!strcmp(*atts, "Complete")) {
				if(!strcmp("true", *(++atts))) {
					fdt->complete = TRUE;
				}
				else {
					fdt->complete = FALSE;
				}
			}
			else if(!strcmp(*atts, "FEC-OTI-FEC-Encoding-ID")) {
				fdt->fec_enc_id = (unsigned char)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-FEC-Instance-ID")) {
				fdt->fec_inst_id = (unsigned short)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Maximum-Source-Block-Length")) {
				fdt->max_sb_len = (unsigned int)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Encoding-Symbol-Length")) {
				fdt->es_len = (unsigned short)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Max-Number-of-Encoding-Symbols")) {
				fdt->max_nb_of_es = (unsigned short)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Number-of-Encoding-Symbols-per-Group")) {
				fdt->nb_of_es_per_group = (unsigned char)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Finite-Field-Parameter")) {
				fdt->finite_field = (unsigned char)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "Content-Type")) {
				atts++;

				if(!(fdt->type = (char*)calloc((strlen(*atts) + 1), sizeof(char)))) {
					printf("Could not alloc memory for fdt->type!\n");
					return;
				}

				memcpy(fdt->type, *atts, strlen(*atts));
			}
			else if(!strcmp(*atts, "Content-Encoding")) {
				atts++;

				if(!(fdt->encoding = (char*)calloc((strlen(*atts) + 1), sizeof(char)))) {
					printf("Could not alloc memory for fdt->encoding!\n");
					return;
				}

				memcpy(fdt->encoding, *atts, strlen(*atts));
			}
			else {
				atts++;
			}
			atts++;
		}
		else {
			atts += 2;
		}
	}

	file = NULL;
}

void initialize_fdt_parser(void) {
#ifdef _MSC_VER
  InitializeCriticalSection(&global_variables_semaphore);
#else
#endif
}

void release_fdt_parser(void) {
#ifdef _MSC_VER
  DeleteCriticalSection(&global_variables_semaphore);
#else
#endif
}

fdt_t* decode_fdt_payload(char *fdt_payload) {

	XML_Parser parser;
	size_t len;

	lock_fdt();

	parser = XML_ParserCreate(NULL);
	/* parser = XML_ParserCreate("iso-8859-1"); */

	len = strlen(fdt_payload);
	fdt = NULL;

	if(!(fdt = (fdt_t*)calloc(1, sizeof(fdt_t)))) {
		printf("Could not alloc memory for fdt!\n");
		XML_ParserFree(parser);
		unlock_fdt();
		return NULL;
	}

	/* initialise fdt parameters */

	fdt->expires = 0;
	fdt->complete = FALSE;
	fdt->fec_enc_id = -1;
	fdt->fec_inst_id = -1;
	fdt->finite_field = GF_BITS;
	fdt->nb_of_es_per_group = 1;
	fdt->max_sb_len = 0;
	fdt->es_len = 0;
	fdt->max_nb_of_es = 0;
	fdt->nb_of_files = 0;
	fdt->type = NULL;
	fdt->encoding = NULL;
	fdt->file_list = NULL;  

	file = NULL;
	prev = NULL;
	is_first_toi = TRUE;

	XML_SetStartElementHandler(parser, startElement_FDT);

	if(XML_Parse(parser, fdt_payload, len, 1) == XML_STATUS_ERROR) {
		fprintf(stderr, "%s at line %d\n",
			XML_ErrorString(XML_GetErrorCode(parser)),
			XML_GetCurrentLineNumber(parser));
		XML_ParserFree(parser);
		unlock_fdt();
		return NULL;
	}

	XML_ParserFree(parser);
	unlock_fdt();
	return fdt;
}

void FreeFDT(fdt_t *fdt) {

	file_t *next_file;
	file_t *file;

	lock_fdt();

	/**** Free FDT struct ****/

	next_file = fdt->file_list;

	while(next_file != NULL) {
		file = next_file;

		if(file->encoding != NULL) {
			free(file->encoding);
		}

		if(file->location != NULL) {
			free(file->location);
		}

		if(file->md5 != NULL) {
			free(file->md5);
		}

		if(file->type != NULL) {
			free(file->type);
		}

		next_file = file->next;
		free(file);
	}

	if(fdt->encoding != NULL) {
		free(fdt->encoding);
	}
	if(fdt->type != NULL) {
		free(fdt->type);
	}

	free(fdt);
	unlock_fdt();
}

int update_fdt(fdt_t *fdt_db, fdt_t *instance) {

	file_t *tmp_file;
	file_t *fdt_file;
	file_t *new_file;
	int retval = 0;
	int updated = 0;

	assert (fdt_db != NULL);
	assert (instance != NULL);

	lock_fdt();

	tmp_file = instance->file_list;

	while(tmp_file != NULL) {

		fdt_file = fdt_db->file_list;

		for(;; fdt_file = fdt_file->next) {

			if(tmp_file->toi == fdt_file->toi) {

				retval = copy_file_info(tmp_file, fdt_file);

				if(retval < 0) {
					unlock_fdt();
					return -1;
				}
				else if(((retval == 1)&&(updated != 2))) {
					updated = 1;
				}

				break;
			}
			else if(fdt_file->next != NULL) {
				continue;
			}
			else {

				if(!(new_file = (file_t*)calloc(1, sizeof(file_t)))) {
					printf("Could not alloc memory for mad_fdt file!\n");
					unlock_fdt();
					return -1;
				}

				new_file->fec_enc_id = -1;
				new_file->fec_inst_id = -1;

				retval = copy_file_info(tmp_file, new_file);

				if(retval < 0) {
					unlock_fdt();
					return -1;
				}
				else if(retval == 1) {
					updated = 2;
				}

				new_file->next = fdt_file->next;
				new_file->prev = fdt_file;
				fdt_file->next = new_file;

				break;
			}
		}

		tmp_file = tmp_file->next;
	}
	unlock_fdt();
	return updated;
}

file_t* find_file_with_toi(fdt_t *fdt, unsigned long long toi) {
    file_t* file = fdt->file_list;

    while(file != NULL) {
		if(file->toi == toi) {
			return file;
        }

        file = file->next;
    }

    return NULL;
}

void PrintFDT(fdt_t *fdt, int s_id) {

	file_t *next_file;
	file_t *file;
	char encoding[5] = "null"; 
	char *enc = encoding;

	lock_fdt();

	next_file = fdt->file_list;

	while(next_file != NULL) {	
		file = next_file;

		if(file->encoding != NULL) {
			enc = file->encoding;
		}

#ifdef _MSC_VER
		printf("URI: %s (TOI=%I64u)\n",  file->location, file->toi);
#else
		printf("URI: %s (TOI=%llu)\n",  file->location, file->toi);
#endif
		fflush(stdout);

		next_file = file->next;
	}
	unlock_fdt();
}

void free_file(file_t *file) {

	lock_fdt();

	if(file->encoding != NULL) {
		free(file->encoding);
	}

	if(file->location != NULL) {
		free(file->location);
	}

	if(file->md5 != NULL) {
		free(file->md5);
	}

	if(file->type != NULL) {
		free(file->type);
	}

	free(file);
	unlock_fdt();
}
