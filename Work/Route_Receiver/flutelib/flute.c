/** \file flute.c \brief FLUTE sender and receiver
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.83 $
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
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <io.h>
#include <direct.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "../alclib/alc_session.h"
#include "../alclib/alc_tx.h"
#include "../alclib/alc_rx.h"

#include "flute.h"
#include "fdt_gen.h"
#include "receiver.h"
#include "sender.h"
#include "uri.h"
#include "padding_encoding.h"
#include "mad_zlib.h"
#include "mad_md5.h"
#include "display.h"

#ifdef USE_FILE_REPAIR
#include "http_file_repair.h"
#include "flute_file_repair.h"
#include "apd.h"
#endif

/**
* This is a private function which names all uncompleted objects.
*
* @param receiver pointer to structure containing receiver information
*
* @return 0 in success, -1 in error cases
*
*/

int name_incomplete_objects(flute_receiver_t *receiver) {

	trans_obj_t *to = NULL;
	file_t *file = NULL;
	file_t *next_file = NULL;
	uri_t *uri = NULL;
	char *filepath = NULL;
	char *tmp = NULL;
	char* session_basedir = NULL;
	char *ptr;
	int point; 
	int ch = '/';
	int i;
	char fullpath[MAX_PATH_LENGTH];
	char filename[MAX_PATH_LENGTH];
	int retval;
	char *pad_tmp_filename = NULL;
	char *tmp_filename = NULL;

	session_basedir = get_session_basedir(receiver->s_id);
	to = get_session_obj_list(receiver->s_id);

	while(to != NULL) {
		next_file = receiver->fdt->file_list;
		while(next_file != NULL) {
			file = next_file;

			if(file->toi == to->toi) {
				break;
			}

			next_file = file->next;
		}

		if(file->encoding != NULL && strcmp(file->encoding, "gzip") == 0) {
			printf("Cannot rename content encoded incomplete object.\n");
			to = to->next;
			continue;
		}

		uri = parse_uri(file->location, strlen(file->location));
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

		memset(fullpath, 0, MAX_PATH_LENGTH);
		memcpy(fullpath, session_basedir, strlen(session_basedir));

		i = 0;

		if(ptr != NULL) {
			while(ptr != NULL) {
				i++;
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

						free(tmp);
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

		if(!(tmp_filename = (char*)calloc((strlen(to->tmp_filename) + 1), sizeof(char)))) {
			printf("Could not alloc memory for tmp_filename!\n");

			free(tmp);
			free(filepath);
			free_uri(uri);
			return -1;    
		}

		if(file->encoding == NULL) {
			memcpy(tmp_filename, to->tmp_filename, strlen(to->tmp_filename));
			free_object(to, get_alc_session(receiver->s_id), 1);
		}
		else if(strcmp(file->encoding, "pad") == 0) {

			if(!(pad_tmp_filename = (char*)calloc((strlen(to->tmp_filename) + 1), sizeof(char)))) {
				printf("Could not alloc memory for pad_tmp_filename!\n");

				free(tmp);
				free(filepath);
				free_uri(uri);
				return -1;    
			}

			memcpy(pad_tmp_filename, to->tmp_filename, strlen(to->tmp_filename));

			ptr = strstr(to->tmp_filename, PAD_SUFFIX);
			memcpy(tmp_filename, to->tmp_filename, (ptr - to->tmp_filename));

			free_object(to, get_alc_session(receiver->s_id), 1);

			retval = padding_decoder(pad_tmp_filename, (int)file->content_len);

			if(retval == -1) {
				free(tmp_filename);
				free(pad_tmp_filename);	
				free(tmp);
				free(filepath);
				free_uri(uri);
				return -1;             
			}

			free(pad_tmp_filename);
		}	

		if(rename(tmp_filename, fullpath) < 0) {

			if(errno == EEXIST) {
				retval = remove(fullpath);

				if(retval == -1) {    
					printf("errno: %i\n", errno);
					fflush(stdout);
				}

				if(rename(tmp_filename, fullpath) < 0) {
					printf("rename() error1: %s\n", tmp_filename);
					fflush(stdout);
				}
			}
			else {
				printf("rename() error2: %s\n", tmp_filename);
				fflush(stdout);
			}
		}

		free(tmp);
		free(filepath);
		free_uri(uri);
		free(tmp_filename);

		to = get_alc_session(receiver->s_id)->obj_list;
	}

	return 0;
}

/**
* This is a private function which adds missing bytes information with receiver reporting.
*
* @param report pointer to structure containing receiver information
* @param first first missing byte
* @param last last missing byte
*
*/

void add_missing_block(flute_receiver_report_t *report, unsigned long long first, 
                       unsigned long long last) {
    missing_block_t *block = malloc(sizeof(missing_block_t));
    block->first = first;
    block->last = last;
    block->next = report->mb_list;

    report->mb_list = block;
}

/**
* This is a private function which copies file name with receiver reporting.
*
* @param file_name name of the file
* @param r pointer to structure containing receiver information
* @param toi transport object identifier
* 
*/

void copy_file_name(char **file_name, flute_receiver_t *r, unsigned long long toi) {
    file_t *file;

    file = find_file_with_toi(r->fdt, toi);

    *file_name = malloc(strlen(file->location)+1);
    strcpy(*file_name, file->location);
}

/**
* This is a private function which copies MD5 with receiver reporting.
*
* @param md5 MD5 of the file 
* @param r pointer to structure containing receiver information
* @param toi transport object identifier
* 
*/

void copy_md5(char **md5, flute_receiver_t *r, unsigned long long toi) {
    file_t *file;

    file = find_file_with_toi(r->fdt, toi);

    *md5 = malloc(strlen(file->md5)+1);
    strcpy(*md5, file->md5);
}

/**
* This is a private function which builds the receiver report.
*
* @param a arguments structure where command line arguments are parsed
* @param r pointer to structure containing receiver information 
* @param report stores receiver report.
* 
*/

void build_report(arguments_t *a, flute_receiver_t *r, flute_receiver_report_t **report) {
    trans_obj_t *obj_list;
    flute_receiver_report_t *report_row;
	unsigned long long position ;
    unsigned long long to_data_left;
    trans_block_t *tb;
    unsigned int i;

    if(report == NULL) {
        return;
    }

    assert(a != NULL);
    assert(r != NULL);

    *report = NULL;
    obj_list = get_session_obj_list(r->s_id);
    while(obj_list != NULL) {
        if(!object_completed(obj_list)) {
            // Not completed file found
            position = 0;
            to_data_left = obj_list->len;
            tb = obj_list->block_list;

            report_row = malloc(sizeof(flute_receiver_report_t));
            copy_file_name(&(report_row->file_name), r, obj_list->toi);
            copy_md5(&(report_row->md5), r, obj_list->toi);
            report_row->mb_list = NULL;
            report_row->next = *report;
            *report = report_row;

            if(a->alc_a.verbosity >= 4) {
                printf("%s received incomplete\n", report_row->file_name);
            }

            for(i = 0; i < obj_list->bs->N; ++i) {

                // This is the len of a generic undecoded block
                unsigned long long block_len = obj_list->es_len*tb->k;

                // This is the len of the next undecoded block
                unsigned long long len = to_data_left < block_len ? to_data_left : block_len;

                if(!block_ready_to_decode(tb)) {
                    // missed bytes from position to position+len;
                    add_missing_block(report_row, position, position+len);
                    if(a->alc_a.verbosity >= 4) {
#ifdef _MSC_VER
                        printf("Missing bytes from %I64x to %I64x\n", position, position+len);
#else
                        printf("Missing bytes from %llu to %llu\n", position, position+len);
#endif
                    }
                }

                assert(0 <= position);
                assert(position < obj_list->len+1);
                assert(len <= (obj_list->len-position));

                position += len;
                to_data_left -= len;

                assert(to_data_left >= 0);

                tb = obj_list->block_list+(i+1);
            }
        }

        obj_list = obj_list->next;
    }
}


/**
* This function returns a random number between zero and max.
*
* @param max maximum number for random number
*
* @return random number between zero and max
*
*/

#ifdef USE_FILE_REPAIR
unsigned int random_number(int max) {
	return (unsigned int)(rand()%max);
}
#endif

int flute_sender(arguments_t *a, int *s_id, unsigned long long *session_size) {

	unsigned short i;
	int j, n;
	int retval = 0;
	int retcode = 0;

	struct sockaddr_in ipv4;
	struct sockaddr_in6 ipv6;

	char addrs[MAX_CHANNELS_IN_SESSION][INET6_ADDRSTRLEN];  /* Mcast addresses on which to send */
	char ports[MAX_CHANNELS_IN_SESSION][MAX_PORT_LENGTH];	   /* Local port numbers  */

	char tmp[5];

	time_t systime;

	unsigned long long curr_time;

#ifdef USE_FILE_REPAIR
	char flute_fdt_file[MAX_PATH_LENGTH];
	char fullpath[MAX_PATH_LENGTH];
	FILE *fp;
#endif

	char *sdp_buf = NULL;
	FILE *sdp_fp;
	struct stat sdp_file_stats;
	int nbytes;

#ifdef _MSC_VER
	HANDLE handle_sender_file_table_output_thread;
	unsigned int sender_file_table_output_thread_id;
	int addr_size;
#else
	pthread_t sender_file_table_output_thread_id;
	int join_retval;
#endif

	flute_sender_t sender;

	*session_size = 0;

	if(strcmp(a->sdp_file, "") != 0) {

		if(stat(a->sdp_file, &sdp_file_stats) == -1) {
			printf("Error: %s is not valid file name\n", a->sdp_file);
			fflush(stdout);
			memset(a->sdp_file, 0, MAX_PATH_LENGTH);
			return -1;
		}

		/* Allocate memory for buf */
		if(!(sdp_buf = (char*)calloc((sdp_file_stats.st_size + 1), sizeof(char)))) {
			printf("Could not alloc memory for sdp buffer!\n");
			fflush(stdout);
			return -1;
		}

		if((sdp_fp = fopen(a->sdp_file, "rb")) == NULL) {
			printf("Error: unable to open sdp_file %s\n", a->sdp_file);
			fflush(stdout);
			free(sdp_buf);
			return -1;
		}

		nbytes = fread(sdp_buf, 1, sdp_file_stats.st_size, sdp_fp); 

		if(parse_sdp_file(a, addrs, ports, sdp_buf) == -1) {
			free(sdp_buf);
			return -1;
		}

		free(sdp_buf);
	}
	else {
		if(a->alc_a.addr_family == PF_INET) {

			for(j = 0; j < a->alc_a.nb_channel; j++) {
				memset(addrs[j], 0, INET6_ADDRSTRLEN);
				ipv4.sin_addr.s_addr = htonl(ntohl(inet_addr(a->alc_a.addr)) + j);
				sprintf(addrs[j], "%s", inet_ntoa(ipv4.sin_addr));

				memset(ports[j], 0, MAX_PORT_LENGTH);
				sprintf(ports[j], "%i", (atoi(a->alc_a.port) + j));
			}
		}
		else if(a->alc_a.addr_family == PF_INET6) {

#ifdef _MSC_VER
			addr_size = sizeof(struct sockaddr_in6);
			WSAStringToAddress((char*)a->alc_a.addr, AF_INET6, NULL, (struct sockaddr*)&ipv6, &addr_size);
#else 
			inet_pton(AF_INET6, a->alc_a.addr, &ipv6.sin6_addr);
#endif

			for(j = 0; j < a->alc_a.nb_channel; j++) {

				memset(addrs[j], 0, INET6_ADDRSTRLEN);

#ifdef _MSC_VER
				addr_size = sizeof(addrs[j]);
				WSAAddressToString((struct sockaddr*)&ipv6, sizeof(struct sockaddr_in6),
					NULL, addrs[j], &addr_size);
#else
				inet_ntop(AF_INET6, &ipv6.sin6_addr, addrs[j], sizeof(addrs[j]));
#endif

				memset(ports[j], 0, MAX_PORT_LENGTH);
				sprintf(ports[j], "%i", (atoi(a->alc_a.port) + j));

				if(j < (a->alc_a.nb_channel - 1)) {
					if(increase_ipv6_address(&ipv6.sin6_addr) == -1) {
						printf("Increasing IPv6 address %s is not possible\n", addrs[j]);
						return -1;
					}
				}
			}
		}
	}

	if(a->alc_a.stop_time != 0) {
		time(&systime);
		curr_time = systime + 2208988800U;

		if(a->alc_a.stop_time <= curr_time) {
			printf("Session end time reached\n");
			fflush(stdout);
			return -3;
		}
	}

	*s_id = open_alc_session(&a->alc_a);

	if(*s_id < 0) {
		printf("Error opening ALC session\n");
		fflush(stdout);
		return -1;
	}

	for(i = 0; (int)i < a->alc_a.nb_channel; i++) {

		if(a->alc_a.addr_type == 1) {
			retval = add_alc_channel(*s_id, ports[i], addrs[0], a->alc_a.intface, a->alc_a.intface_name);
		}
		else {
			retval = add_alc_channel(*s_id, ports[i], addrs[i], a->alc_a.intface, a->alc_a.intface_name);
		}

		if(retval == -1) {    
			close_alc_session(*s_id);
			return -1;
		}	
	}

	/* Generate fdt file first */
	if(strcmp(a->fdt_file, "") == 0) {

		memset(a->fdt_file, 0, MAX_PATH_LENGTH);
		strcpy(a->fdt_file, "fdt_tsi");

		memset(tmp, 0, 5);

#ifdef _MSC_VER
		sprintf(tmp, "%I64u", a->alc_a.tsi);
#else
		sprintf(tmp, "%llu", a->alc_a.tsi);
#endif

		strcat(a->fdt_file, tmp);
		strcat(a->fdt_file, ".xml");

		retcode = generate_fdt(a->file_path, a->alc_a.base_dir, s_id, a->fdt_file, a->complete_fdt,
			a->alc_a.verbosity);

		if(retcode < 0) {
			close_alc_session(*s_id);
			return -1;
		}
	}

#ifdef USE_FILE_REPAIR
	if(strcmp(a->repair, "") != 0) {

		if((fp = fopen(a->repair, "wb")) == NULL) {
			close_alc_session(*s_id);
			return -1;
		}

		memset(fullpath, 0, MAX_PATH_LENGTH);
		memset(flute_fdt_file, 0, MAX_PATH_LENGTH);

		if(getcwd(fullpath, MAX_PATH_LENGTH) != NULL) {

		  if(strcmp(a->alc_a.base_dir, "") == 0) {
		    fprintf(fp, "BaseDir=%s\n", fullpath);
		  }
		  else {
		    fprintf(fp, "BaseDir=%s\n", a->alc_a.base_dir);
		  }
		  
		  if(((a->alc_a.fec_enc_id == SB_SYS_FEC_ENC_ID) && (a->alc_a.fec_inst_id == REED_SOL_FEC_INST_ID))) {
		    fprintf(fp, "FECRatio=%i\n", a->alc_a.fec_ratio);
		  }
		  

		  if(strcmp(a->file_path, "") != 0) {
		    
		    memcpy(flute_fdt_file, fullpath, strlen(fullpath));
#ifdef _MSC_VER
		    strcat(flute_fdt_file, "\\");
#else
		    strcat(flute_fdt_file, "/");
#endif
		    strcat(flute_fdt_file, a->fdt_file);
		  }
		  else {
		    strcat(flute_fdt_file, a->fdt_file);
		  }
		  fprintf(fp, "FDTFile=%s\n", flute_fdt_file);
		}
		
		fclose(fp);
	}
#endif

	sender.fdt = NULL;
	sender.s_id = *s_id;

	/* Create Display thread */

	if(a->file_table_output == TRUE) {

#ifdef _MSC_VER
		handle_sender_file_table_output_thread =
			(HANDLE)_beginthreadex(NULL, 0, (void*)sender_file_table_output_thread,
			(void*)&sender, 0, &sender_file_table_output_thread_id);

		if(handle_sender_file_table_output_thread == NULL) {
			perror("flute_sender(): _beginthread");
			close_alc_session(*s_id);
			return -1;
		}

#else
		if(pthread_create(&sender_file_table_output_thread_id, NULL, sender_file_table_output_thread,
			(void*)&sender) != 0) {
				perror("flute_sender(): pthread_create");
				close_alc_session(*s_id);
				return -1;
		}


#endif
	}

	/***** FDT based send *****/

	retval = sender_in_fdt_based_mode(a, &sender);

	/* If A flag packets must not be included into the session size, use this */
	*session_size = get_session_sent_bytes(*s_id);

	if(a->send_session_close_packets == 1) {

		if(retval != -1) {

			if(a->alc_a.verbosity > 0) {
				printf("Sending session close packets\n");
				fflush(stdout);
			}

			/* Let's send three session close packets for the base channel */

			for(n = 0; n < 3; n++) {
				retcode = send_session_close_packet(*s_id);
				if(retcode == -1) {
					break;
				}
			}
		}
	}

	set_session_state(*s_id, SExiting);

	if(((a->alc_a.cc_id == RLC) || ((a->alc_a.cc_id == Null) && (a->alc_a.nb_channel != 1)))) {
#ifdef _MSC_VER
		WaitForSingleObject(get_alc_session(*s_id)->handle_tx_thread, INFINITE);
		CloseHandle(get_alc_session(*s_id)->handle_tx_thread);
#else
		join_retval = pthread_join(get_alc_session(*s_id)->tx_thread_id, NULL);
		assert(join_retval == 0);
		pthread_detach(get_alc_session(*s_id)->tx_thread_id);
#endif
	}

	if(a->file_table_output == TRUE) {
#ifdef _MSC_VER
		WaitForSingleObject(handle_sender_file_table_output_thread, INFINITE);
		CloseHandle(handle_sender_file_table_output_thread);
#else
		join_retval = pthread_join(sender_file_table_output_thread_id, NULL);
		assert(join_retval == 0);
		pthread_detach(sender_file_table_output_thread_id);
#endif
	}

	/* If A flag packets have to be included into the session size, use this */
	/**session_size = get_session_sent_bytes(*s_id);*/

	if(sender.fdt != NULL) {
		FreeFDT(sender.fdt);
	}

	close_alc_session(*s_id);

	return retval;
}

int flute_receiver(arguments_t *a, int *s_id) {
    return flute_receiver_report(a, s_id, NULL);
}

int flute_receiver_report(arguments_t *a, int *s_id, flute_receiver_report_t **report) {

	unsigned short i;
	int j;
	int retval = 0;

	char wildcard_token[MAX_PATH_LENGTH];
	char *file_uri = NULL;
	int file_nb = 0;

	struct sockaddr_in ipv4;
	struct sockaddr_in6 ipv6;

	char addrs[MAX_CHANNELS_IN_SESSION][INET6_ADDRSTRLEN];  /* Mcast addresses */
	char ports[MAX_CHANNELS_IN_SESSION][MAX_PORT_LENGTH];         /* Local port numbers  */

	time_t systime;
	BOOL is_printed = FALSE;

	unsigned long long curr_time;

#ifdef _MSC_VER
	HANDLE handle_fdt_thread;
	unsigned int fdt_thread_id;
	HANDLE handle_receiver_file_table_output_thread;
	unsigned int receiver_file_table_output_thread_id;
	int addr_size;
#else
	pthread_t fdt_thread_id;
	pthread_t receiver_file_table_output_thread_id;
	int join_retval;
#endif

	char *sdp_buf = NULL;
	FILE *sdp_fp;
	struct stat sdp_file_stats;
	int nbytes;

	flute_receiver_t receiver;

#ifdef USE_FILE_REPAIR
	int n;
	char *file_repair_sdp_buf = NULL;
	serviceURI_t *tmp_serviceURI;
	int waitTime;
	apd_t *apd;
	unsigned int nb_of_http_servers;
	unsigned int http_server_index;
	int repairing_needed = 0;

	file_t *file = NULL;
	char *apd_buf = NULL;
	FILE *apd_fp;
	struct stat apd_file_stats;      
	int apd_nbytes;

	CURL *curl = NULL;
	CURLcode code;
	long responseCode;
	chunk_t chunk;
	char errorBuffer[CURL_ERROR_SIZE];
#endif

#ifdef _MSC_VER
	if(mkdir(a->alc_a.base_dir) < 0) {					
#else		
	if(mkdir(a->alc_a.base_dir, S_IRWXU) < 0) {
#endif
		if(errno != EEXIST) {
			printf("mkdir failed: cannot create directory %s (errno=%i)\n", a->alc_a.base_dir, errno);
			fflush(stdout);
			return -1;
		}
	}

	if(strcmp(a->sdp_file, "") != 0) {

		if(stat(a->sdp_file, &sdp_file_stats) == -1) {
			printf("Error: %s is not valid file name\n", a->sdp_file);
			fflush(stdout);
			return -1;
		}

		/* Allocate memory for buf */
		if(!(sdp_buf = (char*)calloc((sdp_file_stats.st_size + 1), sizeof(char)))) {
			printf("Could not alloc memory for sdp buffer!\n");
			fflush(stdout);
			return -1;
		}

		if((sdp_fp = fopen(a->sdp_file, "rb")) == NULL) {
			printf("Error: unable to open sdp_file %s\n", a->sdp_file);
			fflush(stdout);
			free(sdp_buf);
			return -1;
		}

		nbytes = fread(sdp_buf, 1, sdp_file_stats.st_size, sdp_fp); 

		if(parse_sdp_file(a, addrs, ports, sdp_buf) == -1) {
			free(sdp_buf);
			return -1;
		}

		free(sdp_buf);

		if(a->alc_a.nb_channel == 0) {
			printf("Error: No acceptable channels found in SDP.");
			fflush(stdout);
			return -1;
		}    
	}
	else {
		if(((a->alc_a.cc_id == Null) && (a->alc_a.nb_channel != 1))) {

			if(a->alc_a.addr_family == PF_INET) {
				for(j = 0; j < a->alc_a.nb_channel; j++) {
					memset(addrs[j], 0, INET6_ADDRSTRLEN);
					ipv4.sin_addr.s_addr = htonl(ntohl(inet_addr(a->alc_a.addr)) + j);
					sprintf(addrs[j], "%s", inet_ntoa(ipv4.sin_addr));

					memset(ports[j], 0, MAX_PORT_LENGTH);
					sprintf(ports[j], "%i", (atoi(a->alc_a.port) + j));
				}
			}
			else if(a->alc_a.addr_family == PF_INET6) {

#ifdef _MSC_VER
				addr_size = sizeof(struct sockaddr_in6);
				WSAStringToAddress((char*)a->alc_a.addr, AF_INET6, NULL, (struct sockaddr*)&ipv6, &addr_size);
#else 
				inet_pton(AF_INET6, a->alc_a.addr, &ipv6.sin6_addr);
#endif

				for(j = 0; j < a->alc_a.nb_channel; j++) {
					memset(addrs[j], 0, INET6_ADDRSTRLEN);

#ifdef _MSC_VER
					addr_size = sizeof(addrs[j]);
					WSAAddressToString((struct sockaddr*)&ipv6, sizeof(struct sockaddr_in6),
						NULL, addrs[j], &addr_size);
#else
					inet_ntop(AF_INET6, &ipv6.sin6_addr, addrs[j], sizeof(addrs[j]));
#endif

					memset(ports[j], 0, MAX_PORT_LENGTH);
					sprintf(ports[j], "%i", (atoi(a->alc_a.port) + j));

					if(j < (a->alc_a.nb_channel - 1)) {
						if(increase_ipv6_address(&ipv6.sin6_addr) == -1) {
							printf("Increasing IPv6 address %s is not possible\n", addrs[j]);
							return -1;
						}
					}
				}
			}
		}
		else {
			memset(addrs[0], 0, INET6_ADDRSTRLEN);
			memset(ports[0], 0, MAX_PORT_LENGTH);

			memcpy(addrs[0], a->alc_a.addr, strlen(a->alc_a.addr));
			memcpy(ports[0], a->alc_a.port, strlen(a->alc_a.port));
		}
	}

	if(a->alc_a.stop_time != 0) {
		time(&systime);
		curr_time = systime + 2208988800U;

		if(a->alc_a.stop_time <= curr_time) {
			printf("Session end time reached\n");
			fflush(stdout);
			return -1;
		}
	}

	*s_id = open_alc_session(&a->alc_a);

	if(*s_id < 0) {
		printf("Error opening ALC session\n");
		fflush(stdout);
		return -1;
	}

	if(a->alc_a.start_time != 0) {
		while(1) {

			time(&systime);
			curr_time = systime + 2208988800U;

			if((a->alc_a.start_time - 3) > curr_time) {

				if(!is_printed) {
					printf("Waiting for session start time...\n");
					fflush(stdout);
					is_printed = TRUE;
				}
#ifdef _MSC_VER
				Sleep(1000);
#else
				sleep(1);
#endif
			}
			else {
				break;
			}

			if(get_session_state(*s_id) == SExiting) {
				close_alc_session(*s_id);
				return -5;
			}
		}
	}

	if(a->alc_a.cc_id == Null) {

		for(i = 0; (int)i < a->alc_a.nb_channel; i++) {

			if(a->alc_a.addr_type == 1) {
				retval = add_alc_channel(*s_id, ports[i], addrs[0], a->alc_a.intface, a->alc_a.intface_name);
			}
			else {
				retval = add_alc_channel(*s_id, ports[i], addrs[i], a->alc_a.intface, a->alc_a.intface_name);
			}

			if(retval == -1) {
				close_alc_session(*s_id);
				return -1;
			}
		}
	}
	else if(a->alc_a.cc_id == RLC) {

		retval = add_alc_channel(*s_id, ports[0], addrs[0], a->alc_a.intface, a->alc_a.intface_name);

		if(retval == -1) {
			close_alc_session(*s_id);
			return -1;	
		}
	}

	if(a->rx_object) {
		retval = receiver_in_object_mode(s_id, a);
		receiver.fdt = NULL;
		receiver.efdt = NULL;
	}
	else {

		for(i = 0; i < 10; i++) {
			receiver.file_uri_table[i] = NULL;
		}

		receiver.wildcard_token = NULL;

		file_uri = strtok(a->file_path, ",");

		while(file_uri != NULL) {

			if(strchr(file_uri, '*') != NULL) {

				memset(wildcard_token, 0, MAX_PATH_LENGTH);

				if((file_uri[0] == '*') && (strlen(file_uri) == 1) ) {
					printf("Only *something*, something* and *something are valid values in wild card mode!\n");
					fflush(stdout);
					close_alc_session(*s_id);
					return -1;
				}
				else if((file_uri[0] == '*') && (file_uri[(strlen(file_uri) - 1)] == '*') ) {
					memcpy(wildcard_token, (file_uri + 1), (strlen(file_uri) - 2));
				}
				else if(file_uri[0] == '*') {
					memcpy(wildcard_token, (file_uri + 1), (strlen(file_uri) - 1));
				}
				else if(file_uri[(strlen(file_uri) - 1)] == '*') {
					memcpy(wildcard_token, file_uri, (strlen(file_uri) - 2));
				}
				else {
					printf("Only *something*, something* and *something are valid values in wild card mode!\n");
					fflush(stdout);
					close_alc_session(*s_id);
					return -1;
				}

				receiver.wildcard_token = wildcard_token;
			}
			else {
				receiver.file_uri_table[file_nb] = file_uri;
			}

			file_nb++;
			file_uri = strtok(NULL, ",");
		}

		receiver.fdt = NULL;
		receiver.efdt = NULL;
		receiver.s_id = *s_id;
		receiver.rx_automatic = a->rx_automatic;
		receiver.accept_expired_fdt_inst = a->alc_a.accept_expired_fdt_inst;
		receiver.verbosity = a->alc_a.verbosity;

		/* Create FDT receiving thread */

#ifdef _MSC_VER
		handle_fdt_thread =
			(HANDLE)_beginthreadex(NULL, 0,
			(void*)fdt_thread, (void*)&receiver, 0, &fdt_thread_id);
		if(handle_fdt_thread==NULL) {
			printf("Error: flute_receiver, _beginthread\n");
			fflush(stdout);
			close_alc_session(*s_id);
			return -1;
		}
#else
		//MALEK EL KHATIB 07.05.2014...JUST A COMMENT: IT COMES HERE TO START RECEIVING FDT INSTANCES
		if((pthread_create(&fdt_thread_id, NULL, fdt_thread, (void*)&receiver)) != 0) {
			printf("Error: flute_receiver, pthread_create\n");
			fflush(stdout);
			close_alc_session(*s_id);
			return -1;        
		}
#endif

		/* Create Display thread */

		if(((a->rx_automatic) && (a->file_table_output))) {

#ifdef _MSC_VER
			handle_receiver_file_table_output_thread =
				(HANDLE)_beginthreadex(NULL, 0, (void*)receiver_file_table_output_thread,
				(void*)&receiver, 0, &receiver_file_table_output_thread_id);

			if(handle_receiver_file_table_output_thread == NULL) {
				perror("flute_receiver(): _beginthread");
				close_alc_session(*s_id);
				return -1;
			}
#else
			if(pthread_create(&receiver_file_table_output_thread_id, NULL, receiver_file_table_output_thread,
				(void*)&receiver) != 0) {
					perror("flute_receiver(): pthread_create");
					close_alc_session(*s_id);
					return -1;
			}
#endif
		}
		//MALEK EL KHATIB 08.05.2014...JUST A COMMENT: IT COMES HERE TO START RECEIVING files
		if(((a->rx_automatic) || (strcmp(a->file_path, "") != 0))) {
			retval = receiver_in_fdt_based_mode(a, &receiver);
		}
		else {
			retval = receiver_in_ui_mode(a, &receiver);
		}
	}

	build_report(a, &receiver, report);

	if(retval == -3) {
		if(a->alc_a.verbosity > 0) {
			printf("Sender closed the session\n");
			fflush(stdout);
		}
	}
	//MALEK EL KHATIB 07.05.2014...JUST A COMMENT: IT COMES HERE ONLY IF THE RECEIVING PROCESS IS STOPPED
	set_session_state(*s_id, SExiting);

#ifdef _MSC_VER
	if(get_alc_session(*s_id)->handle_rx_thread != NULL) {
		WaitForSingleObject(get_alc_session(*s_id)->handle_rx_thread, INFINITE);
		CloseHandle(get_alc_session(*s_id)->handle_rx_thread);
		get_alc_session(*s_id)->handle_rx_thread = NULL;
	}

	if(!a->rx_object) {
		WaitForSingleObject(handle_fdt_thread, INFINITE);
		CloseHandle(handle_fdt_thread);
	}

	if(((a->rx_automatic) && (a->file_table_output))) {
		WaitForSingleObject(handle_receiver_file_table_output_thread, INFINITE);
		CloseHandle(handle_receiver_file_table_output_thread); 
	}
#else
	if(get_alc_session(*s_id)->rx_thread_id != 0) {
		join_retval = pthread_join(get_alc_session(*s_id)->rx_thread_id, NULL);
		assert(join_retval == 0);
		pthread_detach(get_alc_session(*s_id)->rx_thread_id);
		get_alc_session(*s_id)->rx_thread_id = 0;
	}

	if(!a->rx_object) {
		join_retval = pthread_join(fdt_thread_id, NULL);
		assert(join_retval == 0);
		pthread_detach(fdt_thread_id);
	}

	if(((a->rx_automatic) && (a->file_table_output))) {
		join_retval = pthread_join(receiver_file_table_output_thread_id, NULL);
		assert(join_retval == 0);
		pthread_detach(receiver_file_table_output_thread_id);
	}
#endif

	remove_alc_channels(*s_id);

#ifdef USE_FILE_REPAIR

	if(receiver.fdt != NULL) {
		file = receiver.fdt->file_list;
	}

	if(file != NULL) {      
		if(a->rx_automatic) {

			/* If we found one file entry from the fdt that is not downloaded completely we */
			/* should do file repairing */

			while(1) {

				if(file->status != 2) {
					repairing_needed = 1;
					break;
				}

				if(file->next == NULL) {
					break;
				}
				else {
					file = file->next;
				}
			}
		}
		else if(receiver.wildcard_token != NULL) {

			/* If we found file entries from the fdt that match to the wildcard token and */
			/* is not downloaded completely we should do file repairing. */

			while(1) {

				if(strstr(file->location, receiver.wildcard_token) != NULL) {
					if(file->status == 1) {
						repairing_needed = 1;
					}
				}

				if(file->next == NULL) {
					break;
				}
				else {
					file = file->next;
				}
			}
		}
		else {

			/* If we found wanted files defined in file_uri_table which are not downloaded */
			/* completely repairing is needed */

			for(i = 0; i < FILE_URI_TABLE_SIZE; i++) {  
				if(receiver.file_uri_table[i] == NULL) {
					continue;
				}
				else {
					repairing_needed = 1;
					break;
				}
			}
		}
	}

	if((strcmp(a->repair, "") != 0) && repairing_needed) {

		curl = curl_easy_init();

		if(curl == NULL) {
			printf("Failed to create CURL connection\n");
			close_alc_session(*s_id);

			if(receiver.fdt != NULL) {
				FreeFDT(receiver.fdt);
			}

			return -1; 
		}

		code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

		if(code != CURLE_OK) {
			printf("Failed to set error buffer [%d]\n", code);
    		close_alc_session(*s_id);
			
			if(receiver.fdt != NULL) {
				FreeFDT(receiver.fdt);
			}

			curl_easy_cleanup(curl);
			return -1; 
		}
		
		code = curl_easy_setopt(curl, CURLOPT_USERAGENT, "mbms-rel6-FLUTE-repair/0.1");

		if(code != CURLE_OK) {
			printf("Failed to set user agent [%s]\n", errorBuffer);
    		close_alc_session(*s_id);

			if(receiver.fdt != NULL) {
				FreeFDT(receiver.fdt);
			}

			curl_easy_cleanup(curl);
			return -1; 
		}

		/* Let's do file repairing */

		retval = 0;

		if(a->alc_a.verbosity > 0) {
			printf("Starting file repair procedure...\n");
		}

		/* Let's get the config from "apd.xml" */

		if(stat(a->repair, &apd_file_stats) == -1) {
			printf("Error: %s is not valid file name\n", a->repair);
			fflush(stdout);
			close_alc_session(*s_id);

			if(receiver.fdt != NULL) {
				FreeFDT(receiver.fdt);
			}

			curl_easy_cleanup(curl);
			return -1;
		}

		/* Allocate memory for buf */
		if(!(apd_buf = (char*)calloc((apd_file_stats.st_size + 1), sizeof(char)))) {
			printf("Could not alloc memory for buffer!\n");
			fflush(stdout);
			close_alc_session(*s_id);

			if(receiver.fdt != NULL) {
				FreeFDT(receiver.fdt);
			}

			curl_easy_cleanup(curl);
			return -1;
		}

		if((apd_fp = fopen(a->repair, "rb")) == NULL) {
			printf("Error: unable to open file %s\n", a->repair);
			fflush(stdout);
			free(apd_buf);
			close_alc_session(*s_id);

			if(receiver.fdt != NULL) {
				FreeFDT(receiver.fdt);
			}

			curl_easy_cleanup(curl);
			return -1;
		}

		apd_nbytes = fread(apd_buf, 1, apd_file_stats.st_size, apd_fp);

		if(apd_nbytes <= 0) {
			free(apd_buf);
			fclose(apd_fp);
			close_alc_session(*s_id);
	
			if(receiver.fdt != NULL) {
				FreeFDT(receiver.fdt);
			}

			curl_easy_cleanup(curl);
			return -1;
		}

		fclose(apd_fp);

		apd = decode_apd_config(apd_buf);
		free(apd_buf);

		if(apd->bmFileRepair->sessionDescriptionURI != NULL) {

			/* Let's get SDP description for the multicast repair session */

			code = curl_easy_setopt(curl, CURLOPT_URL, apd->bmFileRepair->sessionDescriptionURI);

			if(code != CURLE_OK) {
				printf("Failed to set URL [%s]\n", errorBuffer);
				close_alc_session(*s_id);

				if(receiver.fdt != NULL) {
					FreeFDT(receiver.fdt);
				}

				curl_easy_cleanup(curl);
				FreeAPD(apd);
				return -1;
			}

			code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buffer);

			if(code != CURLE_OK) {
				printf("Failed to set writer [%s]\n", errorBuffer);
       			close_alc_session(*s_id);

				if(receiver.fdt != NULL) {
					FreeFDT(receiver.fdt);
				}
	
				curl_easy_cleanup(curl);
				FreeAPD(apd);
				return -1;
			}

			chunk.data = NULL;
			chunk.size = 0;

			code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

			if(code != CURLE_OK) {
				printf("Failed to set write data [%s]\n", errorBuffer);
       			close_alc_session(*s_id);

				if(receiver.fdt != NULL) {
					FreeFDT(receiver.fdt);
				}

				curl_easy_cleanup(curl);
				FreeAPD(apd);
				return -1;
			}

			code = curl_easy_perform(curl);

			if(code != CURLE_OK) {
				printf("Failed to get '%s' [%s]\n", apd->bmFileRepair->sessionDescriptionURI, errorBuffer);
				close_alc_session(*s_id);

				if(receiver.fdt != NULL) {
					FreeFDT(receiver.fdt);
				}

				curl_easy_cleanup(curl);
				FreeAPD(apd);
				return -1;
			}
			
			code = curl_easy_getinfo(curl , CURLINFO_HTTP_CODE , &responseCode);

			if(code != CURLE_OK) {
				printf("Failed to get http response code [%s]\n", errorBuffer);
				close_alc_session(*s_id);

				if(receiver.fdt != NULL) {
					FreeFDT(receiver.fdt);
				}
			
				curl_easy_cleanup(curl);
				FreeAPD(apd);
				return -1;
			}

			if(responseCode == 200 && chunk.size != 0) {
				if(a->alc_a.verbosity > 0) {
					printf("Starting FLUTE file repair...\n");
				}

				retval = flute_file_repair(&receiver, a, chunk.data);

				if(retval == -3) {
					if(a->alc_a.verbosity > 0) {
						printf("Sender closed the session\n");
						fflush(stdout);
					}
				}

				free(chunk.data);
			}
		}
		else {

			/* if NULL then we should do ptp repair */

			while(1) {
				http_server_index = 0;
				nb_of_http_servers = 0;

				/* Let's get the amount of available serviceURIs */

				tmp_serviceURI = apd->postFileRepair->serviceURI_List;

				if(tmp_serviceURI == NULL) {
					if(a->alc_a.verbosity > 0) {
						printf("Number of HTTP servers available: %i\n", nb_of_http_servers);
						printf("HTTP file repair procedure failed.\n");
					}
					break;
				}
				else {
					while(1) {
						nb_of_http_servers++;
						if(tmp_serviceURI->next == NULL) {                              
							break;
						}   
						else {
							tmp_serviceURI = tmp_serviceURI->next;
						}
					}

					/* Let's choose randomly one of the servers */

					http_server_index = 1 + random_number(nb_of_http_servers);

					if(a->alc_a.verbosity > 0) {
						printf("Number of HTTP servers available: %i\n", nb_of_http_servers);
					}

					tmp_serviceURI = apd->postFileRepair->serviceURI_List;

					for(n = 1;; n++) {
						if(n == (int)http_server_index) {
							printf("Using: %s\n", tmp_serviceURI->URI);
							break;
						}
						else {
							tmp_serviceURI = tmp_serviceURI->next;
						}
					}

					/* Let's wait offsetTime + randomTime */

					waitTime = apd->postFileRepair->offsetTime;

					waitTime += random_number(apd->postFileRepair->randomTimePeriod + 1);

					if(a->alc_a.verbosity > 0) {
						printf("waitTime: %i\n", waitTime);               
					}

	#ifdef _MSC_VER
					Sleep(1000*waitTime);
	#else
					sleep(waitTime);
	#endif

					if(a->alc_a.verbosity > 0) {
						printf("Starting HTTP file repair...\n");
					}

					file_repair_sdp_buf = http_file_repair(&receiver, a->open_file, &retval, curl, tmp_serviceURI->URI);

					/* if file_repair_sdp_buf != NULL we should do multicast file repair */

					if(file_repair_sdp_buf != NULL) {
						break;
					}

					/* If retval == 0 HTTP repair has succeeded, if not we should try another server */

					if(retval == 0) {
						break;
					}
					else {
						/*Let's remove the used http server from the list so that it is not used anymore */

						if(tmp_serviceURI->prev != NULL) {
							tmp_serviceURI->prev->next = tmp_serviceURI->next;
						}
						else {
							apd->postFileRepair->serviceURI_List = tmp_serviceURI->next;								
						}
						if(tmp_serviceURI->next != NULL) {
							tmp_serviceURI->next->prev = tmp_serviceURI->prev;
						}
						free(tmp_serviceURI);
					}
				}
			}

			/* if file_repair_sdp_buf != NULL we should do multicast file repair */

			if(file_repair_sdp_buf != NULL) {

				if(a->alc_a.verbosity > 0) {
					printf("Starting FLUTE file repair after 302 Redirect response...\n");
				}

				retval = flute_file_repair(&receiver, a, file_repair_sdp_buf);

				if(retval == -3) {
					if(a->alc_a.verbosity > 0) {
						printf("Sender closed the session\n");
						fflush(stdout);
					}
				}
			}  
		}

		FreeAPD(apd);
		curl_easy_cleanup(curl);
	}
#endif /* USE_FILE_REPAIR */

	if(a->name_incomplete_objects) {
		/* name incomplete objects, so that all received data is used */
		retval = name_incomplete_objects(&receiver);
	}

	if(a->alc_a.cc_id == RLC) {
		if(a->alc_a.verbosity > 0) {
			printf("%i packets were lost in the session\n", get_alc_session(*s_id)->lost_packets);
			fflush(stdout);
		}
	}

	if(receiver.fdt != NULL) {
		FreeFDT(receiver.fdt);
	}

	close_alc_session(*s_id);

	return retval;
}

void free_receiver_report(flute_receiver_report_t *report) {
    flute_receiver_report_t *iterReport = report;

    while (iterReport != NULL) {
        missing_block_t *iterMB = iterReport->mb_list;
        while(iterMB != NULL) {
            missing_block_t *temp = iterMB->next;
            free (iterMB);
            iterMB = temp;
        }

        free (report->file_name);
        free (report->md5);
        iterReport = iterReport->next;
    }
}

unsigned long long flute_session_size(arguments_t *a, int *s_id) {

	BOOL old_flag = a->alc_a.calculate_session_size;
	int old_flag2 = a->alc_a.verbosity;
	int old_flag3 = a->alc_a.nb_tx;
	int old_flag4 = a->alc_a.cc_id;
	int myRet;
	unsigned long long session_size;

	a->alc_a.calculate_session_size = TRUE;
	a->alc_a.verbosity = 0;
	a->alc_a.cc_id = Null;

	if(a->cont) {
		printf("Session size can't be calculated due to continuous Tx mode.\n");
		a->alc_a.calculate_session_size = old_flag;
		a->alc_a.verbosity = old_flag2;
		a->alc_a.nb_tx = old_flag3;
		a->alc_a.cc_id = old_flag4;
		return 0;
	}

	myRet = flute_sender(a, s_id, &session_size);

	if(myRet < 0) {
		printf("Session size can't be calculated because myRet < 0.\n");
		printf("Error: myRet: %d\n", myRet);

		a->alc_a.calculate_session_size = old_flag;
		a->alc_a.verbosity = old_flag2;
		a->alc_a.nb_tx = old_flag3;
		a->alc_a.cc_id = old_flag4;
		return 0;
	}

	a->alc_a.calculate_session_size = old_flag;
	a->alc_a.verbosity = old_flag2;
	a->alc_a.nb_tx = old_flag3;
	a->alc_a.cc_id = old_flag4;

	return session_size;
}

void set_flute_session_state(int s_id, enum alc_session_states state) {

	set_session_state(s_id, state);
}

void set_all_flute_sessions_state(enum alc_session_states state) {

	set_all_sessions_state(state); 
}

void set_flute_session_base_rate(int s_id, int base_tx_rate) {
	update_session_tx_rate(s_id, base_tx_rate); 
}

int start_up_flute(void) {

#ifdef _MSC_VER
	WSADATA wsd;
	int wsreturn;
#endif

#ifdef _MSC_VER

	wsreturn = WSAStartup(MAKEWORD(2, 2), &wsd);

	if(wsreturn != 0) {
		printf("WSAStartup() failed\n");
		return wsreturn;
	}

#endif

	initialize_fdt_parser();
	initialize_session_handler();

	return 0;
}

void shut_down_flute(arguments_t *anArguments) {

#ifdef _MSC_VER
	WSACleanup();
#endif

	release_session_handler();
	release_fdt_parser();

	if(anArguments == NULL) {
		return;
	}

	if(anArguments->log_fd != -1) {
		close(anArguments->log_fd);
	}

	if(strcmp(anArguments->sdp_file, "") != 0) {
		sf_free(anArguments->src_filt);
		free(anArguments->src_filt);
		sdp_message_free(anArguments->sdp);
	}
}

void shut_down_flute2(void) {

#ifdef _MSC_VER
	WSACleanup();
#endif

	release_session_handler();
	release_fdt_parser();
}
