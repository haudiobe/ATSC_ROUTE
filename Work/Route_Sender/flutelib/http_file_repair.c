/** \file http_file_repair.c \brief Point-to-point file repair
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.39 $
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

#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#ifdef _MSC_VER
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#include <shellapi.h>
#include <io.h>
#include <direct.h>
#endif

#include "http_file_repair.h"

#include "../alclib/alc_session.h"
#include "../alclib/transport.h"
#include "../alclib/alc_rx.h"
#include "../alclib/xor_fec.h"
#include "../alclib/rs_fec.h"
#include "../alclib/null_fec.h"

#include "uri.h"
#include "mad_md5.h"
#include "mad_zlib.h"

#ifdef USE_FILE_REPAIR

/**
 * This is a private function which parses repair data.
 *
 * @param chunk contains repair data
 * @param file pointer to the file's FDT info
 * @param obj pointer to the transport object
 * @param s pointer to the session
 * @param openfile Open repaired file automatically [in Windows only].
 *
 * @return 0 in success, -1 in error cases, -4 when MD5 check error occurs
 *
 */

int parse_data(chunk_t chunk, file_t *file, trans_obj_t *obj, alc_session_t *s, int openfile) {
	
	unsigned int sbn = 0;
	unsigned int esi = 0;
	unsigned short sb_len = 0;
	int li;
	int i;
	int j;
	int loop;
	int bytes_left;
	char *buf = NULL;

#ifdef USE_OPENSSL
	char *md5 = NULL;
#endif

	unsigned long long block_len;
	unsigned long long pos;

	uri_t *uri;
	char *ptr;
	int point;
	int ch = '/';
	char *tmp = NULL;
	char fullpath[MAX_PATH_LENGTH];
	char filename[MAX_PATH_LENGTH];
	char *filepath = NULL;
	char *session_basedir = NULL;
	int retcode;

#ifdef USE_ZLIB
#ifdef _MSC_VER
	struct __stat64 file_stats;
#else
	struct stat64 file_stats;
#endif
#endif

	trans_block_t *block = NULL;
	trans_unit_t *unit = NULL;
	trans_unit_t *tu = NULL;
	trans_unit_t *next_tu = NULL;

	char *string;

	sbn = 0;
	esi = 0;
	li = 0;
	j = 0;
	loop = 1;
	bytes_left = chunk.size;
	string = chunk.data;

	while(1) {

		if(bytes_left == 0) {
			break;
		}

		/* parse length indicator field */
		li = (unsigned char)string[j+1] + (unsigned short)((string[j] << 8) & 0xFF00);
		j += 2;
		bytes_left -= 2;

		if(file->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
			sbn = (unsigned char)string[j+1] + (unsigned short)((string[j] << 8) & 0xFF00);
			esi = (unsigned char)string[j+3] + (unsigned short)((string[j+2] << 8) & 0xFF00);
			j += 4;
			bytes_left -= 4;
		}
		else if(file->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
			sbn = (unsigned char)string[j+3] + (unsigned short)((string[j+2] << 8) & 0xFF00) +
				(unsigned int)((string[j+1] << 16) & 0xFF0000) + (unsigned int)((string[j] << 24) & 0xFF000000);
			esi = (unsigned char)string[j+7] + (unsigned short)((string[j+6] << 8) & 0xFF00) +
				(unsigned int)((string[j+5] << 16) & 0xFF0000) + (unsigned int)((string[j+4] << 24) & 0xFF000000);
			j += 8;
			bytes_left -= 8;
		}
		else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID) &&
			(file->fec_inst_id == REED_SOL_FEC_INST_ID))) {
				sbn = (unsigned char)string[j+3] + (unsigned short)((string[j+2] << 8) & 0xFF00) +
					(unsigned int)((string[j+1] << 16) & 0xFF0000) + (unsigned int)((string[j] << 24) & 0xFF000000);
				sb_len = (unsigned char)string[j+5] + (unsigned short)((string[j+4] << 8) & 0xFF00);
				esi = (unsigned char)string[j+7] + (unsigned short)((string[j+6] << 8) & 0xFF00);
				j += 8;
				bytes_left -= 8;
		}
		else {
			printf("FEC Encoding %i/%i is not supported!\n", file->fec_enc_id, file->fec_inst_id);
			return -1;
		}

		for(i = 0; i < li; i++) {

#ifdef USE_RETRIEVE_UNIT
			/* Retrieve a transport unit from session pool  */
			unit = retrieve_unit(s, file->es_len);
#else
			/* Create transport unit */
			unit = create_units(1);
#endif

			if(unit == NULL) {
				return -1;
			}

			unit->esi = (esi + i);

			if(bytes_left >= file->es_len) {
				unit->len = file->es_len;
			}
			else {
				unit->len = bytes_left;
			}

#ifndef USE_RETRIEVE_UNIT
			if(!(unit->data = (char*)calloc(unit->len, sizeof(char)))) {
				printf("Could not alloc memory for transport unit's data!\n");
				return -1;
			}
#endif

			memcpy(unit->data, (string + j), unit->len);

			j += unit->len;
			bytes_left -= unit->len;

			block = obj->block_list+sbn;

			if(block->unit_list == NULL) {

				block->sbn = sbn;	
				block->nb_of_rx_units = 0;

				if(file->fec_enc_id == COM_NO_C_FEC_ENC_ID) { 

					if(sbn < obj->bs->I) {
						block->k = obj->bs->A_large;
					}
					else {
						block->k = obj->bs->A_small;
					}
				}
				else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID)
					&& (file->fec_inst_id == REED_SOL_FEC_INST_ID))) {

						block->k = sb_len;
						block->max_k = file->max_sb_len;
						block->max_n = file->max_nb_of_es;
				}
				else if(file->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {

					if(sbn < obj->bs->I) {
						block->k = obj->bs->A_large;
					}
					else {
						block->k = obj->bs->A_small;
					}

					block->max_k = file->max_sb_len;
				}
			}

			insert_unit(unit, block, obj);

			/* if large file mode data symbol is stored in the tmp file */
			if(s->rx_memory_mode == 2) {

#ifdef _MSC_VER
				unit->offset = _lseeki64(obj->fd_st, 0, SEEK_END);
#else
				unit->offset = lseek64(obj->fd_st, 0, SEEK_END);
#endif
				if(unit->offset == -1) {
#ifdef _MSC_VER
					printf("lseek error, toi: %I64u\n", obj->toi);
#else
					printf("lseek error, toi: %llu\n", obj->toi);
#endif
					fflush(stdout);
					return -1;
				}

				if(write(obj->fd_st, unit->data, (unsigned int)unit->len) == -1) {
#ifdef _MSC_VER
					printf("write error, toi: %I64u, sbn: %i\n", obj->toi, sbn);
#else
					printf("write error, toi: %llu, sbn: %i\n", obj->toi, sbn);
#endif
					fflush(stdout);
					return -1;
				}

				free(unit->data);
				unit->data = NULL;
			}
		}

		if(s->rx_memory_mode == 1 || s->rx_memory_mode == 2) {

			if(block_ready_to_decode(block)) {

				if(s->rx_memory_mode == 2){

					/* We have to restore the data symbols to trans_units from the symbol store tmp file */

					next_tu = block->unit_list;

					while(next_tu != NULL) {

						tu = next_tu;	  

						if(tu->data != NULL) {
							next_tu = tu->next; 
							continue;
						}
#ifdef _MSC_VER
						if(_lseeki64(obj->fd_st, tu->offset, SEEK_SET) == -1) {
#else
						if(lseek64(obj->fd_st, tu->offset, SEEK_SET) == -1) {
#endif

#ifdef _MSC_VER
							printf("lseek error, toi: %I64u\n", obj->toi);
#else
							printf("lseek error, toi: %llu\n", obj->toi);
#endif
							fflush(stdout);
							return -1;
						}
						/* let's copy the data symbols from the tmp file to the memory */

						/* Alloc memory for restoring data symbol */

						if(!(tu->data = (char*)calloc(tu->len, sizeof(char)))) { /* rs_fec&xor_fec: 
																				 trans_unit->len --> eslen */
							printf("Could not alloc memory for transport unit's data!\n");
							return -1;
						}

						if(read(obj->fd_st, tu->data, tu->len) == -1) {
#ifdef _MSC_VER
							printf("read error, toi: %I64u, sbn: %i\n", obj->toi, sbn);
#else
							printf("read error, toi: %llu, sbn: %i\n", obj->toi, sbn);
#endif
							fflush(stdout);
							return -1;
						}

						next_tu = tu->next;
					}
				}

				/* decode and save data */
				if(file->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
					buf = null_fec_decode_src_block(block, &block_len, file->es_len);
				}
				else if(file->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
					buf = xor_fec_decode_src_block(block, &block_len, file->es_len);
				}
				else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID)
					&& (file->fec_inst_id == REED_SOL_FEC_INST_ID))) {
						buf = rs_fec_decode_src_block(block, &block_len, file->es_len);
				}

				if(buf == NULL) {
					return -1;
				}

				if(block->sbn < obj->bs->I) {
					pos = ( (unsigned long long)block->sbn * (unsigned long long)obj->bs->A_large *
						(unsigned long long)file->es_len );
				}
				else {
					pos = ( ( ( (unsigned long long)obj->bs->I * (unsigned long long)obj->bs->A_large ) +
						( (unsigned long long)block->sbn - (unsigned long long)obj->bs->I )  *
						(unsigned long long)obj->bs->A_small ) * (unsigned long long)file->es_len );
				}

				/* We have to check if there is padding in the last source symbol of the last source block */

				if(block->sbn == ((obj->bs->N) - 1)) {
					block_len = (file->transfer_len - (file->es_len * (obj->bs->I * obj->bs->A_large +
						(obj->bs->N - obj->bs->I -1) * obj->bs->A_small)));
				}

				/* set correct position */

#ifdef _MSC_VER
				if(_lseeki64(obj->fd, pos, SEEK_SET) == -1) {
#else
				if(lseek64(obj->fd, pos, SEEK_SET) == -1) {
#endif
					printf("lseek error\n");
					free(buf);
					return -1;
				}

				if(write(obj->fd, buf, (unsigned int)block_len) == -1) {
					printf("write error\n");
					return -1;
				}

				obj->nb_of_ready_blocks++;

				if(s->verbosity > 1) {
#ifdef _MSC_VER
					printf("%u/%u Source Blocks decoded (TOI=%I64u SBN=%u)\n", obj->nb_of_ready_blocks, obj->bs->N, obj->toi, sbn);
#else
					printf("%u/%u Source Blocks decoded (TOI=%llu SBN=%u)\n", obj->nb_of_ready_blocks, obj->bs->N, obj->toi, sbn);
#endif
					fflush(stdout);
				}

				free(buf);

#ifdef USE_RETRIEVE_UNIT
				free_units2(block);
#else
				free_units(block);
#endif

				if(object_completed(obj)) {
#ifdef USE_OPENSSL
					if(file->md5 != NULL) {

						md5 = file_md5(obj->tmp_filename);

						if(md5 == NULL) {
#ifdef _MSC_VER
							printf("MD5 check failed (TOI=%I64u)!\n\n", file->toi);
#else
							printf("MD5 check failed (TOI=%llu)!\n\n", file->toi);
#endif
							fflush(stdout);
							remove(obj->tmp_filename);
							return -4;
						}
						else {
							if(strcmp(md5, file->md5) != 0) {
#ifdef _MSC_VER
								printf("MD5 check failed (TOI=%I64u)!\n\n", file->toi);
#else
								printf("MD5 check failed (TOI=%llu)!\n\n", file->toi);
#endif
								fflush(stdout);
								remove(obj->tmp_filename);
								free(md5);
								return -4;
							}
							free(md5);
						}
					}
#endif

#ifdef USE_ZLIB
					if(file->encoding != NULL) {
						if(strcmp(file->encoding, "gzip") == 0) {

							retcode = file_gzip_uncompress(obj->tmp_filename);

							if(retcode == -1) {
								return -1;
							}

							*(obj->tmp_filename + (strlen(obj->tmp_filename) - GZ_SUFFIX_LEN)) = '\0';

#ifdef _MSC_VER
							if(_stat64(obj->tmp_filename, &file_stats) == -1) {
#else
							if(stat64(obj->tmp_filename, &file_stats) == -1) {
#endif
								printf("Error: %s is not valid file name\n", obj->tmp_filename);
								fflush(stdout);
								return -1;
							}

							if(file_stats.st_size != file->content_len) {
								printf("Error: uncompression failed, file-size not ok.\n");
								fflush(stdout);
								return -1;
							}
						}
					}
#endif
					uri = parse_uri(file->location, (int)strlen(file->location));

					filepath = get_uri_host_and_path(uri);

					if(!(tmp = (char*)calloc((strlen(filepath) + 1), sizeof(char)))) {
						printf("Could not alloc memory for tmp-filepath!\n");
						fflush(stdout);
						free(filepath);
						free_uri(uri);
						return -1;
					}

					memcpy(tmp, filepath, strlen(filepath));
					ptr = strchr(tmp, ch);

					session_basedir = get_session_basedir(s->s_id);

					memset(fullpath, 0, MAX_PATH_LENGTH);
					memcpy(fullpath, session_basedir, strlen(session_basedir));

					if(ptr != NULL) {

						while(ptr != NULL) {

							point = (int)(ptr - tmp);

							memset(filename, 0, MAX_PATH_LENGTH);				    
#ifdef _MSC_VER
							memcpy((fullpath + strlen(fullpath)), "\\", 1);
#else
							memcpy((fullpath + strlen(fullpath)), "/", 1);
#endif
							memcpy((fullpath + strlen(fullpath)), tmp, point);

							memcpy(filename, (tmp + point + 1), (strlen(tmp) - (point + 1)));

#ifdef _MSC_VER
							if(mkdir(fullpath) < 0) {
#else
							if(mkdir(fullpath, S_IRWXU) < 0) {
#endif
								if(errno != EEXIST) {
									printf("mkdir failed: cannot create directory %s (errno=%i)\n", fullpath, errno);
									fflush(stdout);
									free(filepath);
									free_uri(uri);
									return -1;
								}
							}

							strcpy(tmp, filename);
							ptr = strchr(tmp, ch);
						}
#ifdef _MSC_VER
						memcpy((fullpath + strlen(fullpath)), "\\", 1);
#else
						memcpy((fullpath + strlen(fullpath)), "/", 1);
#endif
						memcpy((fullpath + strlen(fullpath)), filename, strlen(filename));
					}
					else{
#ifdef _MSC_VER
						memcpy((fullpath + strlen(fullpath)), "\\", 1);
#else
						memcpy((fullpath + strlen(fullpath)), "/", 1);
#endif
						memcpy((fullpath + strlen(fullpath)), filepath, strlen(filepath));
					}

					if(close(obj->fd) == -1) {
						printf("close failed, errno: %i\n", errno);
					}

					if(rename(obj->tmp_filename, fullpath) < 0) {

						if(errno == EEXIST) {

							retcode = remove(fullpath);

							if(retcode == -1) {
								printf("remove() error: errno: %i\n", errno);
								fflush(stdout);
							}

							if(rename(obj->tmp_filename, fullpath) < 0) {
								printf("rename() error1: %s\n", obj->tmp_filename);
								fflush(stdout);
							}
						}
						else {
							printf("rename() error2: %s\n%i\n", obj->tmp_filename, errno);
							fflush(stdout);
						}
					}

					free(obj->tmp_filename);
					obj->tmp_filename = NULL;

					free(tmp);

					if(s->verbosity > 0) {
#ifdef _MSC_VER
						printf("File received: %s (TOI=%I64u)\n", file->location, file->toi);
#else
						printf("File received: %s (TOI=%llu)\n", file->location, file->toi);
#endif
						fflush(stdout);
					}

					free_uri(uri);

#ifdef _MSC_VER
					if(openfile) {
						ShellExecute(NULL, "Open", fullpath, NULL, NULL, SW_SHOWNORMAL);
					}
#endif
					free(filepath);
				}
			}
		}

		loop++; 
	}

	return 0;
}

size_t write_to_buffer(void *ptr, size_t size, size_t nmemb, void *a) {
  
  size_t neededsize;
  chunk_t *chunk;
  neededsize = size * nmemb;
  chunk = (chunk_t*)a;

  chunk->data = (char*)realloc(chunk->data, (chunk->size + neededsize + 1));
  
  if(chunk->data) {
    memcpy(&(chunk->data[chunk->size]), ptr, neededsize);
    chunk->size += neededsize;
    chunk->data[chunk->size] = 0;
  }
  else {
	  return 0;
  }
  
  return neededsize;
}

char* http_file_repair(flute_receiver_t *receiver, int openfile, int *retval, CURL *curl,
					   char *serviceURI) {

	alc_session_t *s = NULL;
	trans_obj_t *obj = NULL;
	trans_obj_t *next_obj = NULL;
	trans_block_t *block = NULL;
	trans_unit_t *unit = NULL;
	char esi_table[255]; /* TODO: Max SBL is 255! */
	unsigned int i, j;

	char tmp_string[10];
	file_t *file = NULL;

	int first_esi;
	int tmp_uri_len;

	int counter;
	int nb_of_missing_symbols;

	char *sdp_buf = NULL;

	char filename[MAX_PATH_LENGTH];

	CURLcode code;
	long response_code;
	chunk_t content;
	char *content_type;
	char query_string[MAX_HTTP_URL_LENGTH];
	char errorBuffer[CURL_ERROR_SIZE];
	int x;

	code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

	if(code != CURLE_OK) {
		printf("Failed to set error buffer [%d]\n", code);
    	*retval = -1;
		return NULL; 
	}

	code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buffer);

	if(code != CURLE_OK) {
		printf("Failed to set writer [%s]\n", errorBuffer);
       	*retval = -1;
		return NULL; 
	}

	code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&content);	

	if(code != CURLE_OK) {
		printf("Failed to set write data [%s]\n", errorBuffer);
       	*retval = -1;
		return NULL; 
	}

	code = curl_easy_setopt(curl, CURLOPT_URL, serviceURI);

	if(code != CURLE_OK) {
		printf("Failed to set URL [%s]\n", errorBuffer);
		*retval = -1;
		return NULL; 
	}

	code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

	if(code != CURLE_OK) {
		printf("Failed to set redirect option [%s]\n", errorBuffer);
		*retval = -1;
		return NULL; 
	}

	s = get_alc_session(receiver->s_id);
	
	/* We should go through the fdt and repair objects which status != 2 with */
	/* automatic mode and objects which status is 1 with wild card and file */
	/* uri list mode */

	file = receiver->fdt->file_list;

	if(file == NULL) {
		/* This should not happen, because this is already checked in main.c */
		*retval = -1;
		return NULL; 
	}

	while(file != NULL) {

		if((receiver->rx_automatic && file->status != 2) || file->status == 1) {

			if(receiver->verbosity > 0) {
#ifdef _MSC_VER
				printf("Repairing TOI: %I64u\n", file->toi);
#else
				printf("Repairing TOI: %llu\n", file->toi);
#endif
			}

			memset(query_string, 0, MAX_HTTP_URL_LENGTH);
			strcpy(query_string, "fileURI=");
			strcat(query_string, file->location);

			tmp_uri_len = strlen(query_string);

			/* Search the object from the object structure */

			obj = NULL;

			next_obj = get_session_obj_list(receiver->s_id);

			while(next_obj != NULL) {

				if(next_obj->toi == file->toi) {
					obj = next_obj;
					break;
				}
				next_obj = next_obj->next;
			}

			if(obj == NULL) {

				/* We have to create the object structure */

				if(receiver->verbosity > 0) {
#ifdef _MSC_VER
					printf("TOI: %I64u is completely missing.\n", file->toi);
#else           
					printf("TOI: %llu is completely missing.\n", file->toi);
#endif
				}

				obj = create_object();
				obj->toi = file->toi;
				obj->len = file->transfer_len;
				obj->fec_enc_id = (unsigned char)file->fec_enc_id;
				obj->fec_inst_id = file->fec_inst_id;
				obj->es_len = file->es_len;
				obj->max_sb_len = file->max_sb_len;
				obj->bs = compute_blocking_structure(file->transfer_len, file->max_sb_len, file->es_len);

				if (!(obj->block_list = (trans_block_t*)calloc(obj->bs->N, sizeof(trans_block_t)))) {
					printf("Could not alloc memory for transport block list!\n");
					*retval = -1;
					return NULL;
				}

				memset(filename, 0, MAX_PATH_LENGTH);

				if(file->encoding == NULL) {
#ifdef _MSC_VER
					sprintf(filename, "%s\\%s", get_session_basedir(receiver->s_id), "object_XXXXXX");
#else                 
					sprintf(filename, "%s/%s", get_session_basedir(receiver->s_id), "object_XXXXXX");
#endif
					mktemp(filename);
				}
#ifdef USE_ZLIB
				else if(strcmp(file->encoding, "gzip") == 0) {
#ifdef _MSC_VER
					sprintf(filename, "%s\\%s", get_session_basedir(receiver->s_id), "object_XXXXXX");
#else
					sprintf(filename, "%s/%s", get_session_basedir(receiver->s_id), "object_XXXXXX");
#endif
					mktemp(filename);
					strcat(filename, GZ_SUFFIX);
				}
#endif
				else {
					printf("Not supported encoding: %s\n", file->encoding);
					*retval = -1;
					return NULL;
				}

				/* Alloc memory for tmp_filename */
				if(!(obj->tmp_filename = (char*)calloc(strlen(filename)+1, sizeof(char)))) {
					printf("Could not alloc memory for tmp_filename!\n");
					*retval = -1;
					return NULL;
				}

				memcpy(obj->tmp_filename, filename, strlen(filename));

#ifdef _MSC_VER
				if((obj->fd = open((const char*)obj->tmp_filename,
					_O_WRONLY | _O_CREAT | _O_BINARY | _O_TRUNC , _S_IWRITE)) < 0) {
#else
				if((obj->fd = open(obj->tmp_filename,
					O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU)) < 0) {
#endif
						printf("Error: unable to open file %s\n", obj->tmp_filename);
						fflush(stdout);
						*retval = -1;
						return NULL;
				}

				insert_object(obj, s, 1);
			}
			else {
				/* Object has been received partly and it is found from the object structure */

				block = obj->block_list;

				for(j = 0; j < obj->bs->N; j++) {

					if(block->nb_of_rx_units != 0 && block_ready_to_decode(block)) {
						/* We have a complete block */
						block = obj->block_list+(j+1);
						continue;
					}
					else if(block->nb_of_rx_units == 0) {
						/* We have completely missing block */

						strcat(query_string, "&SBN=");
						memset(tmp_string, 0, 10);
						sprintf(tmp_string, "%i", j);
						strcat(query_string, tmp_string);

						if(strlen(query_string) > 200) {

							code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query_string);

							if(code != CURLE_OK) {
								printf("Failed to set postfields [%s]\n", errorBuffer);
       							*retval = -1;
								return NULL;
							}

							printf("%s\n", query_string);
							
							content.data = NULL;
							content.size = 0;

							code = curl_easy_perform(curl);

							if(code != CURLE_OK) {
								printf("Failed to get repair data for '%s' [%s]\n", query_string, errorBuffer);
								if(content.data != NULL) {
									free(content.data);
								}
								*retval = -1;
								return NULL;
							}

							code = curl_easy_getinfo(curl , CURLINFO_HTTP_CODE , &response_code);

							if(code != CURLE_OK) {
								printf("Failed to get http response code [%s]\n", errorBuffer);
								if(content.data != NULL) {
									free(content.data);
								}
								*retval = -1;
								return NULL;
							}

							code = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);

							if(code != CURLE_OK) {
								printf("Failed to get content type [%s]\n", errorBuffer);
								*retval = -1;
								if(content.data != NULL) {
									free(content.data);
								}
								return NULL;
							}

							for(x = 0; x < (int)strlen(content_type); x++) {
							  content_type[x] = tolower(content_type[x]);
							}
							
							if(response_code == 200) {
								if(strcmp(content_type, "application/simplesymbolcontainer") == 0) {
									if(parse_data(content, file, obj, s, openfile) < 0) {
									}
									free(content.data);
								}
								else  {

									if(sdp_buf == NULL) {
										sdp_buf = content.data;
									}
									else {
										free(content.data);
									}

									code = curl_easy_setopt(curl, CURLOPT_URL, serviceURI);
									
									if(code != CURLE_OK) {
										printf("Failed to set URL [%s]\n", errorBuffer);
										if(content.data != NULL) {
											free(content.data);
										}
										*retval = -1;
										return NULL; 
									}
								}
							}
							else {
								printf("Response code [%lu]\n", response_code);
								free(content.data);
								*retval = -1;
								return NULL; 
							}

							memset(query_string, 0, MAX_HTTP_URL_LENGTH);
							strcpy(query_string, "fileURI=");
							strcat(query_string, file->location);
						}
					}
					else {
						/* We have partly missing block */
						counter = 0;

						for(i = 0;i < 255; i++) {
							esi_table[i] = 0;
						}

						if(!block_ready_to_decode(block)) {
							unit = block->unit_list;

							while(unit != NULL) {
								esi_table[unit->esi] = 1;
								unit = unit->next;
							}

							first_esi = 1;

							/* Now we are fetching only source symbols */
							nb_of_missing_symbols = (block->k - block->nb_of_rx_units);

							for(i = 0; i < (int)block->k; i++) {
								if(counter == nb_of_missing_symbols) {
									break;
								}

								if(esi_table[i] != 1) {
									counter++;

									if(first_esi) {	    
										memset(tmp_string, 0, 10);
										strcat(query_string, "&SBN=");
										sprintf(tmp_string, "%i", block->sbn);
										strcat(query_string, tmp_string);

										memset(tmp_string, 0, 10);
										strcat(query_string, ";ESI=");
										sprintf(tmp_string, "%i", i);
										strcat(query_string, tmp_string);
										first_esi = 0;
									}
									else {
										memset(tmp_string, 0, 10);
										strcat(query_string, ",");
										sprintf(tmp_string, "%i", i);
										strcat(query_string, tmp_string);
									}

									if(strlen(query_string) > 200) {

										code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query_string);

										if(code != CURLE_OK) {
											printf("Failed to set postfields [%s]\n", errorBuffer);
       										*retval = -1;
											return NULL;
										}

										printf("%s\n", query_string);
										
										content.data = NULL;
										content.size = 0;

										code = curl_easy_perform(curl);

										if(code != CURLE_OK) {
											printf("Failed to get repair data for '%s' [%s]\n", query_string, errorBuffer);
											if(content.data != NULL) {
												free(content.data);
											}
											*retval = -1;
											return NULL;
										}

										code = curl_easy_getinfo(curl , CURLINFO_HTTP_CODE , &response_code);

										if(code != CURLE_OK) {
											printf("Failed to get http response code [%s]\n", errorBuffer);
											if(content.data != NULL) {
												free(content.data);
											}
											*retval = -1;
											return NULL;
										}

										code = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);

										if(code != CURLE_OK) {
											printf("Failed to get content type [%s]\n", errorBuffer);
											if(content.data != NULL) {
												free(content.data);
											}
											*retval = -1;
											return NULL;
										}

										for(x = 0; x < (int)strlen(content_type); x++) {
										  content_type[x] = tolower(content_type[x]);
										}

										if(response_code == 200) {
											if(strcmp(content_type, "application/simplesymbolcontainer") == 0) {
												if(parse_data(content, file, obj, s, openfile) < 0) {
												}
												free(content.data);
											}
											else  {
												if(sdp_buf == NULL) {
													sdp_buf = content.data;
												}
												else {
													free(content.data);
												}

												code = curl_easy_setopt(curl, CURLOPT_URL, serviceURI);

												if(code != CURLE_OK) {
													printf("Failed to set URL [%s]\n", errorBuffer);
													if(content.data != NULL) {
														free(content.data);
													}
													*retval = -1;
													return NULL; 
												}
											}
										}
										else {
											printf("Response code [%lu]\n", response_code);
											free(content.data);
											*retval = -1;
											return NULL; 
										}

										memset(query_string, 0, MAX_HTTP_URL_LENGTH);
										strcpy(query_string, "fileURI=");
										strcat(query_string, file->location);
										first_esi = 1;
									}
								}
							}
						}
					}
					block = obj->block_list+(j+1);
				}
			}
		}

		code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query_string);
		
		if(code != CURLE_OK) {
			printf("Failed to set postfields [%s]\n", errorBuffer);
			*retval = -1;
			return NULL;
		}
		
		printf("%s\n", query_string);

		content.data = NULL;
		content.size = 0;

		code = curl_easy_perform(curl);

		if(code != CURLE_OK) {
			printf("Failed to get repair data for '%s' [%s]\n", query_string, errorBuffer);
			if(content.data != NULL) {
				free(content.data);
			}
			*retval = -1;
			return NULL;
		}

		code = curl_easy_getinfo(curl , CURLINFO_HTTP_CODE , &response_code);

		if(code != CURLE_OK) {
			printf("Failed to get http response code [%s]\n", errorBuffer);
			if(content.data != NULL) {
				free(content.data);
			}
			*retval = -1;
			return NULL;
		}

		code = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);

		if(code != CURLE_OK) {
			printf("Failed to get content type [%s]\n", errorBuffer);
			if(content.data != NULL) {
				free(content.data);
			}
			*retval = -1;
			return NULL;
		}

		for(x = 0; x < (int)strlen(content_type); x++) {
		  content_type[x] = tolower(content_type[x]);
		}

		if(response_code == 200) {
			if(strcmp(content_type, "application/simplesymbolcontainer") == 0) {
				if(parse_data(content, file, obj, s, openfile) < 0) {
				}
				free(content.data);
			}
			else  {

				if(sdp_buf == NULL) {
					sdp_buf = content.data;
				}
				else {
					free(content.data);
				}

				code = curl_easy_setopt(curl, CURLOPT_URL, serviceURI);

				if(code != CURLE_OK) {
					printf("Failed to set URL [%s]\n", errorBuffer);
					if(content.data != NULL) {
						free(content.data);
					}
					*retval = -1;
					return NULL; 
				}
			}
		}
		else {
			printf("Response code [%lu]\n", response_code);
			free(content.data);
			*retval = -1;
			return NULL; 
		}

		file = file->next;
	}

	*retval = 0;
	return sdp_buf;
}

#endif
