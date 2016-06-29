/** \file efdt.c \brief EFDT parsing
 *
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
#include "efdt.h"
#include "mad_utf8.h"
	
fdt_t *fdt;				/**< FDT */
file_t *file;			/**< file */
file_t *prev;			/**< previous parsed file */
BOOL is_first_toi;		/**< is first TOI parsed or not? */
efdt_t *efdt;

/**
 * Global variables semaphore
 */

#ifdef _MSC_VER
RTL_CRITICAL_SECTION global_variables_semaphore1;
#else
pthread_mutex_t global_variables_semaphore1 = PTHREAD_MUTEX_INITIALIZER;
#endif

/**
 * This is a private function, which locks the EFDT.
 *
 */

void lock_efdt(void) {
#ifdef _MSC_VER
	EnterCriticalSection(&global_variables_semaphore1);
#else
	pthread_mutex_lock(&global_variables_semaphore1);
#endif
}

/**
 * This is a private function, which unlocks the EFDT.
 *
 */

void unlock_efdt(void) {
#ifdef _MSC_VER
	LeaveCriticalSection(&global_variables_semaphore1);
#else
	pthread_mutex_unlock(&global_variables_semaphore1);
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

/*int copy_file_info(file_t *src, file_t *dest) {

	int updated = 0;

	/* Copy only if particular field is not present in destination, so file description can be only
	complemented not modified */

/*	if(src->toi != 0) {
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
*/

/**
 * This is a private function which is used in FDT parsing.
 *
 * @param userData not used, must be
 * @param name pointer to buffer containing element's name
 * @param atts pointer to buffer containing element's attributes
 *
 */

static void startElement_EFDT(void *userData, const char *name, const char **atts) {

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

				file->expires = efdt->expires;

				file->fec_enc_id = efdt->fec_enc_id;
				file->fec_inst_id = efdt->fec_inst_id;
				file->finite_field = efdt->finite_field;
				file->nb_of_es_per_group = efdt->nb_of_es_per_group;
				file->max_sb_len = efdt->max_sb_len;
				file->es_len = efdt->es_len;
				file->max_nb_of_es = efdt->max_nb_of_es;

				efdt->nb_of_files++;
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
					efdt->file_list = file;
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

				if(file->type == NULL && efdt->type != NULL) {
					if(!(file->type = (char*)calloc((strlen(efdt->type) + 1), sizeof(char)))) {
						printf("Could not alloc memory for file->type!\n");
						return;
					}
					memcpy(file->type, efdt->type, strlen(efdt->type));
				}
				if(file->encoding == NULL && efdt->encoding != NULL) {
					if(!(file->encoding = (char*)calloc((strlen(efdt->encoding) + 1), sizeof(char)))) {
						printf("Could not alloc memory for file->encoding!\n");
						return;
					}
					memcpy(file->encoding, efdt->encoding, strlen(efdt->encoding));
				}
			}

		}
		else if(!strcmp(name, "FDT-Parameters")) {

			if(!strcmp(*atts, "Expires")) {  
#ifdef _MSC_VER
				efdt->expires = _atoi64(*(++atts));

				if(efdt->expires > (unsigned long long)0xFFFFFFFFFFFFFFFF) {
					printf("Expires too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}
#else
				/*efdt->expires = atoll(*(++atts));*/

				efdt->expires = strtoull(*(++atts), &ep, 10);

				if(*(atts) == '\0' || *ep != '\0') {
					printf("Expires not a number\n");
					fflush(stdout);
					return;
				}

				if(errno == ERANGE && efdt->expires == 0xFFFFFFFFFFFFFFFFULL) {
					printf("Expires too big for unsigned long long (64 bits)\n");
					fflush(stdout);
					return;
				}
#endif
			}
			else if(!strcmp(*atts, "Complete")) {
				if(!strcmp("true", *(++atts))) {
					efdt->complete = TRUE;
				}
				else {
					efdt->complete = FALSE;
				}
			}
			else if(!strcmp(*atts, "FEC-OTI-FEC-Encoding-ID")) {
				efdt->fec_enc_id = (unsigned char)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-FEC-Instance-ID")) {
				efdt->fec_inst_id = (unsigned short)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Maximum-Source-Block-Length")) {
				efdt->max_sb_len = (unsigned int)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Encoding-Symbol-Length")) {
				efdt->es_len = (unsigned short)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Max-Number-of-Encoding-Symbols")) {
				efdt->max_nb_of_es = (unsigned short)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Number-of-Encoding-Symbols-per-Group")) {
				efdt->nb_of_es_per_group = (unsigned char)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "FEC-OTI-Finite-Field-Parameter")) {
				efdt->finite_field = (unsigned char)atoi(*(++atts));
			}
			else if(!strcmp(*atts, "Content-Type")) {
				atts++;

				if(!(efdt->type = (char*)calloc((strlen(*atts) + 1), sizeof(char)))) {
					printf("Could not alloc memory for efdt->type!\n");
					return;
				}

				memcpy(efdt->type, *atts, strlen(*atts));
			}
			else if(!strcmp(*atts, "Content-Encoding")) {
				atts++;

				if(!(efdt->encoding = (char*)calloc((strlen(*atts) + 1), sizeof(char)))) {
					printf("Could not alloc memory for efdt->encoding!\n");
					return;
				}

				memcpy(efdt->encoding, *atts, strlen(*atts));
			}
			else {
				atts++;
			}
			atts++;
		}
		else if(!strcmp(name, "EFDT-Instance")) {
		  if(!strcmp(*atts, "tsi")) {
		    	efdt->tsi = (unsigned int)atoi(*(++atts));
		  }
		  else if(!strcmp(*atts, "idRef")) {
		    
		    atts++;

				if(!(efdt->idRef = (char*)calloc((strlen(*atts) + 1), sizeof(char)))) {
					printf("Could not alloc memory for efdt->idRef!\n");
					return;
				}

				memcpy(efdt->idRef, *atts, strlen(*atts));
		  }
		  else{
		     atts++;
		  }
		  
		}
		else {
			atts += 2;
		}
	}

	file = NULL;
}

void initialize_efdt_parser(void) {
#ifdef _MSC_VER
  InitializeCriticalSection(&global_variables_semaphore1);
#else
#endif
}

void release_efdt_parser(void) {
#ifdef _MSC_VER
  DeleteCriticalSection(&global_variables_semaphore1);
#else
#endif
}

efdt_t* decode_efdt_payload(char *efdt_payload) {

	XML_Parser parser;
	size_t len;

	lock_efdt();

	parser = XML_ParserCreate(NULL);
	/* parser = XML_ParserCreate("iso-8859-1"); */

	len = strlen(efdt_payload);
	efdt = NULL;

	if(!(efdt = (efdt_t*)calloc(1, sizeof(efdt_t)))) {
		printf("Could not alloc memory for efdt!\n");
		XML_ParserFree(parser);
		unlock_efdt();
		return NULL;
	}

	/* initialise fdt parameters */

	efdt->tsi=0;
	efdt->idRef=NULL;
	efdt->version=0;
	efdt->maxExpiresDelta=0;
	efdt->maxTransportSize=0;
	efdt->expires = 0;
	efdt->complete = FALSE;
	efdt->fec_enc_id = -1;
	efdt->fec_inst_id = -1;
	efdt->finite_field = GF_BITS;
	efdt->nb_of_es_per_group = 1;
	efdt->max_sb_len = 0;
	efdt->es_len = 0;
	efdt->max_nb_of_es = 0;
	efdt->nb_of_files = 0;
	efdt->type = NULL;
	efdt->encoding = NULL;
	efdt->file_list = NULL;  

	file = NULL;
	prev = NULL;
	is_first_toi = TRUE;

	XML_SetStartElementHandler(parser, startElement_EFDT);

	if(XML_Parse(parser, efdt_payload, len, 1) == XML_STATUS_ERROR) {
		fprintf(stderr, "%s at line %d\n",
			XML_ErrorString(XML_GetErrorCode(parser)),
			XML_GetCurrentLineNumber(parser));
		XML_ParserFree(parser);
		unlock_efdt();
		return NULL;
	}

	XML_ParserFree(parser);
	unlock_efdt();
	return efdt;
}

void FreeEFDT(efdt_t *efdt) {

	file_t *next_file;
	file_t *file;

	lock_efdt();

	/**** Free EFDT struct ****/

	next_file = efdt->file_list;

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

	if(efdt->encoding != NULL) {
		free(efdt->encoding);
	}
	if(efdt->type != NULL) {
		free(efdt->type);
	}

	free(efdt);
	unlock_efdt();
}

int update_efdt(efdt_t *efdt_db, efdt_t *instance) {

	file_t *tmp_file;
	file_t *efdt_file;
	file_t *new_file;
	int retval = 0;
	int updated = 0;

	assert (efdt_db != NULL);
	assert (instance != NULL);

	lock_efdt();

	tmp_file = instance->file_list;

	while(tmp_file != NULL) {

		efdt_file = efdt_db->file_list;

		for(;; efdt_file = efdt_file->next) {

			if(tmp_file->toi == efdt_file->toi) {

				retval = copy_file_info(tmp_file, efdt_file);

				if(retval < 0) {
					unlock_efdt();
					return -1;
				}
				else if(((retval == 1)&&(updated != 2))) {
					updated = 1;
				}

				break;
			}
			else if(efdt_file->next != NULL) {
				continue;
			}
			else {

				if(!(new_file = (file_t*)calloc(1, sizeof(file_t)))) {
					printf("Could not alloc memory for mad_fdt file!\n");
					unlock_efdt();
					return -1;
				}

				new_file->fec_enc_id = -1;
				new_file->fec_inst_id = -1;

				retval = copy_file_info(tmp_file, new_file);

				if(retval < 0) {
					unlock_efdt();
					return -1;
				}
				else if(retval == 1) {
					updated = 2;
				}

				new_file->next = efdt_file->next;
				new_file->prev = efdt_file;
				efdt_file->next = new_file;

				break;
			}
		}

		tmp_file = tmp_file->next;
	}
	unlock_efdt();
	return updated;
}



void PrintEFDT(efdt_t *efdt, int s_id) {

	file_t *next_file;
	file_t *file;
	char encoding[5] = "null"; 
	char *enc = encoding;

	lock_efdt();

	next_file = efdt->file_list;

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
	unlock_efdt();
}

void free_file(file_t *file) {

	lock_efdt();

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
	unlock_efdt();
}
