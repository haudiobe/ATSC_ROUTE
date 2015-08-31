/** \file repair_sender.c \brief Point-to-multipoint repair sender
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.50 $
 *
 *  MAD-REPAIR-SENDER: Implementation of simple point-to-multipoint repair for FLUTE protocol.
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#ifdef _MSC_VER
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include "../flutelib/flute.h"
#include "../flutelib/uri.h"
#include "../flutelib/mad_zlib.h"

#include "../alclib/blocking_alg.h"
#include "../alclib/alc_session.h"
#include "../alclib/alc_tx.h"

#include "repair_sender.h"

miss_obj_t *miss_obj_list;  /**< Stores information about missing objects */

/**
 * This is a private function which creates missing block structure.
 *
 * @return missing block structure in success, NULL otherwise
 *
 */

miss_block_t* create_miss_block(void) {
  
  miss_block_t *miss_block = NULL;
  
  if (!(miss_block = (miss_block_t*)calloc(1, sizeof(miss_block_t)))) {
    printf("Could not alloc memory for missing block!\n");
    return NULL;
  }
  
  return miss_block;
}

/**
 * This is a private function which creates missing object structure.
 *
 * @return missing object structure in success, NULL otherwise
 *
 */

miss_obj_t* create_miss_object(void) {
  
  miss_obj_t *miss_obj = NULL;
  
  if (!(miss_obj = (miss_obj_t*)calloc(1, sizeof(miss_obj_t)))) {
    printf("Could not alloc memory for missing object!\n");
    return NULL;
  }
  
  return miss_obj;
}

/** 
 * This is a private function which insert missing object to the missing object list.
 *
 * @param miss_obj missing object structure to be inserted
 *
 */

void insert_miss_object(miss_obj_t *miss_obj) {
  
  miss_obj_t * tmp = miss_obj_list;
  
  if(tmp == NULL) {
    miss_obj_list = miss_obj;
  }
  else {
    for(;;) {
      if(miss_obj->toi < tmp->toi) {
	
	if(tmp->prev == NULL) {
	  miss_obj->next = tmp;
	  miss_obj->prev = tmp->prev;
	  
	  tmp->prev = miss_obj;
	  
	  miss_obj_list = miss_obj;
	}
	else {
	  miss_obj->next = tmp;
	  miss_obj->prev = tmp->prev;
	  
	  tmp->prev->next = miss_obj;
	  tmp->prev = miss_obj;
	}
	break;
      }
      
      if(tmp->next == NULL) {
	
	miss_obj->next = tmp->next;
	miss_obj->prev = tmp;
	
	tmp->next = miss_obj;
	break;
      }
      
      tmp = tmp->next;
    }
  }
}

/** 
 * This is a private function which insert missing block to the missing object.
 *
 * @param miss_block missing block structure to be inserted
 * @param miss_obj missing object structure where to insert
 *
 */

void insert_miss_block(miss_block_t *miss_block, miss_obj_t *miss_obj) {
  
  miss_block_t *tmp;
  
  tmp = miss_obj->block_list;
  
  if(tmp == NULL) {
    miss_obj->block_list = miss_block;
  }
  else {
    for(;;) {
      if(miss_block->sbn < tmp->sbn) {
	
	if(tmp->prev == NULL) {
	  miss_block->next = tmp;
	  miss_block->prev = tmp->prev;
	  
	  tmp->prev = miss_block;
	  
	  miss_obj->block_list = miss_block;
	}
	else {
	  miss_block->next = tmp;
	  miss_block->prev = tmp->prev;
	  
	  tmp->prev->next = miss_block;
	  tmp->prev = miss_block;
	  
	}
	break;
      }
      if(tmp->next == NULL) {
	miss_block->next = tmp->next;
	miss_block->prev = tmp;
	
	tmp->next = miss_block;
	break;
      }
      tmp = tmp->next;
    }
  }
}

/**
 * This is a private function which return missing object structure if it exists.
 *
 * @param toi transport object identifier
 *
 * @return missing object structure in success, NULL otherwise
 *
 */

miss_obj_t* miss_object_exist(unsigned long long toi) {
  
  miss_obj_t *miss_obj = miss_obj_list;
  
  if(miss_obj != NULL) {
    for(;;) {
      if(miss_obj->toi == toi) {
	break;
      }
      if(miss_obj->next == NULL) {
	miss_obj = NULL;
	break;
      }
      miss_obj = miss_obj->next;
    }
  }
  
  return miss_obj;
}

/**
 * This is a private function which return missing block structure if it exists.
 *
 * @param sbn source block number
 * @param miss_obj missing object structure where to look missing block
 *
 * @return missing block structure in success, NULL otherwise
 *
 */

miss_block_t* miss_block_exist(unsigned int sbn, miss_obj_t *miss_obj) {
  
  miss_block_t *miss_block;
  
  miss_block = miss_obj->block_list;
  
  if(miss_block != NULL) {
    for(;;) {
      if(miss_block->sbn == sbn) {
	break;
      }
      if(miss_block->next == NULL) {
	miss_block = NULL;
	break;
      }
      miss_block = miss_block->next;
    }
  }
  
  return miss_block;
}

/**
 * This is a private function which frees missing object list.
 *
 */

void free_miss_object_list(void) {
  
  miss_obj_t *miss_obj;
  miss_obj_t *next_miss_obj;
  miss_block_t *miss_block;
  miss_block_t *next_miss_block;
  
  next_miss_obj = miss_obj_list;
  
  while(next_miss_obj != NULL) {
    
    miss_obj = next_miss_obj;
    
    next_miss_block = miss_obj->block_list;
    
    while(next_miss_block != NULL) {
      miss_block = next_miss_block;
      next_miss_block = miss_block->next;
      free(miss_block);
    }
    
    next_miss_obj = miss_obj->next;
    free(miss_obj);
  }
}

/**
 * This is a private function sends needed repair data for one file.
 *
 * @param file_path file path for the repaired file
 * @param s_id session identifier
 * @param tx_mode transmission mode
 * @param toi transport object identifier
 * @param es_len encoding symbol length
 * @param max_sb_len maximum source block length
 * @param fec_enc_id FEC encoding id
 * @param fec_inst_id FEC instance id
 * @param miss_obj missing object structure
 *
 * @return 0 in success, -1 in error cases, -2 when state is SExiting
 *
 */

int send_repair_data(char *file_path, int *s_id, int tx_mode, unsigned long long toi, unsigned short es_len,
					 unsigned int max_sb_len, unsigned char fec_enc_id, unsigned short fec_inst_id,
					 miss_obj_t *miss_obj) {
  
  unsigned long long transfer_len;

  int sent = 0;
  int nbytes;
  int fp;
#ifdef _MSC_VER
  struct __stat64 file_stats;
#else
  struct stat64 file_stats;
#endif

  char *buf = NULL;
  
  unsigned int sbn = 0;
  blocking_struct_t *bs;
  
  miss_block_t *miss_block = NULL;

  unsigned long long pos;

#ifdef _MSC_VER
  if(_stat64(file_path, &file_stats) == -1) {
#else
  if(stat64(file_path, &file_stats) == -1) {
#endif
    printf("Error: %s is not valid file name\n", file_path);
    fflush(stdout);
    return -1;
  }
  
 transfer_len = (unsigned long long)file_stats.st_size;
  
  if(transfer_len == 0) {
    printf("Tx_file %s size = 0\n", file_path);
    fflush(stdout);
    return -1;
  }
  
  if(transfer_len > 0xFFFFFFFFFFFFULL) {
    printf("Tx_file %s too big!!\n", file_path);
    fflush(stdout);
    return -1;
  }
  
  /* Allocate memory for buf */
  if(!(buf = (char*)calloc((unsigned int)(es_len*max_sb_len), sizeof(char)))) {
    printf("Could not alloc memory for buf!\n");
    fflush(stdout);
    return -1;
  }

#ifdef _MSC_VER
  if((fp = open(file_path, _O_BINARY, 0)) < 0) {
#else
  if((fp = open64(file_path, 0, 0)) < 0) {
#endif
      printf("Error: unable to open file %s\n", file_path);
      fflush(stdout);
      free(buf);
      return -1;
  }

  printf("\nSending file: %s\n\n", file_path);
  fflush(stdout);
  
  set_object_sent_bytes(*s_id, 0);
  set_object_last_print_tx_percent(*s_id, 0);
  
  /* Let's compute the blocking structure */
  bs = compute_blocking_structure(transfer_len, max_sb_len, es_len);
  
  while(sbn < bs->N) {
    
    miss_block = miss_block_exist(sbn, miss_obj);

    if(((miss_obj->block_list != NULL) && (miss_block == NULL))) {
      sbn++;
      continue;
    }

    if(sbn < bs->I) {
      nbytes = es_len * bs->A_large;
      pos = (unsigned long long)sbn * (unsigned long long)bs->A_large * (unsigned long long)es_len;
    }
    else {
      nbytes = es_len * bs->A_small;
      pos = ( ( ( (unsigned long long)bs->I * (unsigned long long)bs->A_large) +
		((unsigned long long)sbn - (unsigned long long)bs->I ) * (unsigned long long)bs->A_small )* (unsigned long long)es_len );
    }

#ifdef _MSC_VER
    if((_lseeki64(fp, pos, SEEK_SET)) < 0) {
#else
    if((lseek64(fp, pos, SEEK_SET)) < 0) {
#endif
      printf("lseek error\n");
      fflush(stdout);
      free(buf);
      close(fp);
      return -1;
    }
    
    memset(buf, 0, (es_len * max_sb_len));
    nbytes = read(fp, buf, (unsigned int)nbytes);
    
    if(nbytes < 0) {
      printf("read error, file: %s\n", file_path);
      fflush(stdout);
      free(buf);
      close(fp);
      return -1;
    }

    sent = alc_send(*s_id, tx_mode, buf, nbytes, toi, transfer_len, es_len, max_sb_len, sbn, fec_enc_id, fec_inst_id);
    
    if(sent == -1) {
      /*free(sent_sbn);*/
      close(fp);
      free(buf);
      free(bs);
      return -1;
    }
    else if(sent == -2) {
      close(fp);
      free(buf);
      free(bs);
      return -2;
    }
    
    sbn++;
  }
  
  free(bs);
  close(fp);
  free(buf);
  
  printf("\nFile: %s sent.\n", file_path);
  fflush(stdout);
  
  return 0;
}
  
/**
 * This function sends repair data for the files defined in the FDT file.
 *
 * @param s_id session identifier
 * @param a arguments structure where command line arguments are parsed
 *
 * @return 0 in success, -1 in error cases, -2 when state is SExiting
 *
 */

int repair_sender_in_fdt_based_mode(int *s_id, arguments_t *a) {
  
  char *buf = NULL;
  
#ifdef _MSC_VER
  int j;
#endif
  
  FILE *fp;
  struct stat file_stats;
  
  fdt_t *fdt;
  file_t *file;
  uri_t *uri;
  
  char path[MAX_PATH_LENGTH];
  
  static time_t fdt_mtime = 0;					/* Time of last FDT modification */
  
  unsigned short tmp_eslen = a->alc_a.es_len;
  unsigned int tmp_max_sblen = a->alc_a.max_sb_len;
  unsigned char tmp_fec_enc_id = a->alc_a.fec_enc_id;
  unsigned short tmp_fec_inst_id = a->alc_a.fec_inst_id;
  
  time_t systime;
  
  unsigned long long curr_time;
  unsigned long long fdt_length = 0;
  unsigned long long sent = 0;
  long long nbytes = 0;
  
  BOOL incomplete_fdt = FALSE;
  
  unsigned long long toi;

  unsigned int sbn;
  
  struct stat	repair_file_stats;
  char *repair_file_buf = NULL;
  unsigned repair_file_nbytes;
  FILE *repair_file_fp;
  
  miss_obj_t *miss_obj = NULL;
  miss_block_t *miss_block = NULL;
  miss_block_t *next_miss_block = NULL;

  char *tmp = NULL;
  
  if(stat(a->requested_blocks_file, &repair_file_stats) == -1) {
    printf("Error: %s is not valid file name\n", a->requested_blocks_file);
    fflush(stdout);
    return -1;
  }
  
  /* Allocate memory for buf */
  if(!(repair_file_buf = (char*)calloc((repair_file_stats.st_size + 1), sizeof(char)))) {
    printf("Could not alloc memory for buffer!\n");
    fflush(stdout);
    return -1;
  }
  
  if((repair_file_fp = fopen(a->requested_blocks_file, "rb")) == NULL) {
    free(repair_file_buf);
    return -1;
  }
  
  repair_file_nbytes = (unsigned int)fread(repair_file_buf, 1, repair_file_stats.st_size, repair_file_fp);
  
  if(repair_file_nbytes <= 0) {
    fclose(repair_file_fp);
    free(repair_file_buf);
    return -1;
  }
  
  fclose(repair_file_fp);
  
  /* TODO: TOI is 64bits, now using only 32bits */
  
  tmp = strtok(repair_file_buf, ":");
  
  while(tmp != NULL) {
    	  
    toi = (unsigned long long)atoi(tmp);
    tmp = strtok(NULL, "\n");

    if(tmp[0] == '*') {

      /* Check if missing object already exist */
      miss_obj = miss_object_exist(toi);

      if(miss_obj == NULL) {

        miss_obj = create_miss_object();

        if(miss_obj == NULL) {
          return MEM_ERROR;
        }

        miss_obj->toi = toi;

        insert_miss_object(miss_obj);
      }
      else {
	next_miss_block = miss_obj->block_list;

	while(next_miss_block != NULL) {
	  miss_block = next_miss_block;
	  next_miss_block = miss_block->next;
	  free(miss_block);
	}
	
	miss_obj->block_list = NULL;
      }

    }
    else {
      
      sbn = atoi(tmp);

      /* Check if missing object already exist */
      miss_obj = miss_object_exist(toi);
    
      if(miss_obj == NULL) {
	
	miss_obj = create_miss_object();
	
	if(miss_obj == NULL) {
	  return MEM_ERROR;
	}
	
	miss_obj->toi = toi;
	
	insert_miss_object(miss_obj);
      }
      
      /* Check if missing block already exist */
      miss_block = miss_block_exist(sbn, miss_obj);
      
      if(miss_block == NULL) {
	
	miss_block = create_miss_block();
	
	if(miss_block == NULL) {
	  return MEM_ERROR;
	}
	
	miss_block->sbn = sbn;
	
	insert_miss_block(miss_block, miss_obj);
      }
    }

    tmp = strtok(NULL, ":");
  }
  
  free(repair_file_buf);
	
  if(stat(a->fdt_file, &file_stats) == -1) {
    printf("Error: %s is not valid file name\n", a->fdt_file);
    fflush(stdout);
    free_miss_object_list();
    return -1;
  }
  
  fdt_length = (unsigned long long)file_stats.st_size;
  
  /* If someone has modified the fdt file */
  
  if(fdt_mtime != file_stats.st_mtime) {
    fdt_mtime = file_stats.st_mtime;
  }
  
  /* Allocate memory for buf, to read fdt file to it */
  if(!(buf = (char*)calloc((unsigned int)(fdt_length + 1), sizeof(char)))) {
    printf("Could not alloc memory for fdt buffer!\n");
    free_miss_object_list();
    return -1;
  }
  
  if((fp = fopen(a->fdt_file, "rb")) == NULL) {
    printf("Error: unable to open tx_file %s\n", a->fdt_file);
    fflush(stdout);
    free(buf);
    free_miss_object_list();
    return -1;
  }
  
  nbytes = fread(buf, 1, (unsigned int)fdt_length, fp);
  
  if(nbytes <= 0) {
    printf("fread error\n");
    fflush(stdout);
    fclose(fp);
    free(buf);
    free_miss_object_list();
    return -1;
  }
  
  fdt = decode_fdt_payload(buf);
  free(buf);
  fclose(fp);
  
  if(fdt == NULL) {
    free_miss_object_list();
    return -1;
  }
  
  time(&systime);
  curr_time = systime + 2208988800U;
  
  if(fdt->expires < curr_time) {
    set_session_state(*s_id, SExiting);
    FreeFDT(fdt);
    free_miss_object_list();
    return -2;
  }

  while(a->alc_a.nb_tx) {

    file = fdt->file_list;
  
    while(file != NULL) { /* loop for sending all files in fdt */
    
      miss_obj = miss_object_exist(file->toi);
    
      if(miss_obj == NULL) {
	file = file->next;
	continue;
      }
      
      if(a->alc_a.use_fec_oti_ext_hdr == 0) {
	
	if(file->fec_enc_id == -1) {
	  incomplete_fdt = TRUE;
	}
	else if(((file->fec_enc_id > 127) && (file->fec_inst_id == -1))) {
	  incomplete_fdt = TRUE;
	}
	else if(file->max_sb_len == 0) {
	  incomplete_fdt = TRUE;
	}
	else if(file->es_len == 0) {
	  incomplete_fdt = TRUE;
	}
	else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID) &&
		 (file->max_nb_of_es == 0))) {
	  incomplete_fdt = TRUE;
	}
	
	if(incomplete_fdt) {
#ifdef _MSC_VER
	  printf("FDT does not contain FEC-OTI information, TOI: %I64u\n", file->toi);
#else
	  printf("FDT does not contain FEC-OTI information, TOI: %llu\n", file->toi);
#endif
	  fflush(stdout);
	  FreeFDT(fdt);
	  free_miss_object_list();
	  return -1;
	}
	
	if(file->toi == 0) {
	  incomplete_fdt = TRUE;
	}
	else if(file->location == NULL) {
	  incomplete_fdt = TRUE;
	}
	
#ifdef USE_ZLIB
	if(((a->alc_a.encode_content == 2) && (file->transfer_len == 0))) {
	  incomplete_fdt = TRUE;
	}
	else if(((a->alc_a.encode_content != 2) && (file->transfer_len == 0))) {
	  incomplete_fdt = TRUE;
	}
#else	
	if(file->content_len == 0) {
	  incomplete_fdt = TRUE;
	}	
#endif
	
	if(incomplete_fdt) {
#ifdef _MSC_VER
	  printf("FDT does not contain enough File information, TOI: %I64u\n", file->toi);
#else
	  printf("FDT does not contain enough File information, TOI: %llu\n", file->toi);
#endif
	  fflush(stdout);
	  FreeFDT(fdt);
	  free_miss_object_list();
	  return -1;
	}
      }
      
#ifdef _MSC_VER
      Sleep(500);
#else
      usleep(500000);
#endif
      uri = parse_uri(file->location, (int)strlen(file->location));
      
      memset(path, 0, MAX_PATH_LENGTH);
      
      if(!(strcmp(a->alc_a.base_dir, "") == 0)) {
	strcpy(path, a->alc_a.base_dir);
	strcat(path, "/");
      }
      strcat(path, uri->path);
      
#ifdef USE_ZLIB
      if(a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
	strcat(path, GZ_SUFFIX);	
      }
#endif
      if(file->next == NULL) {
	set_session_a_flag_usage(*s_id);
      }
      
      if(((file->es_len != 0) && (file->max_sb_len != 0))) {
	
	tmp_eslen = file->es_len;
	tmp_max_sblen = file->max_sb_len;
      }
      
      if(file->fec_enc_id != -1) {
	if(file->fec_enc_id != tmp_fec_enc_id) {
	  tmp_fec_enc_id = (unsigned char)file->fec_enc_id;
	  tmp_fec_inst_id = (unsigned short)file->fec_inst_id;
	}
      }
      
#ifdef _MSC_VER
      for(j = 0; j < (int)strlen(path); j++) {
	if(*(path + j) == '/') {
	  *(path + j) = '\\';
	}
      }
#endif
      
      sent = send_repair_data(path, s_id, NO_TX_THREAD, file->toi, tmp_eslen, tmp_max_sblen, tmp_fec_enc_id,
			      tmp_fec_inst_id, miss_obj);
      
      free_uri(uri);
      
      if(sent == -1) {
	FreeFDT(fdt);
	free_miss_object_list();
	return -1;
      }
      else if(sent == -2) {
	FreeFDT(fdt);
	return -2;
      }
      
      file = file->next;
    
      /* Sleep between files so that receiver can decode them */
      
#ifdef _MSC_VER
      Sleep(500);
#else
      usleep(500000);
#endif
    }

    if(!a->cont) {
      a->alc_a.nb_tx--;
    }
  }
  
  FreeFDT(fdt);
  free_miss_object_list();
  
  return 0;
}

int repair_sender(arguments_t *a, int *s_id) {
  
  unsigned short i;
  int j;
  int retval = 0;

  struct sockaddr_in ipv4;
  struct sockaddr_in6 ipv6;
 
  char addrs[MAX_CHANNELS_IN_SESSION][INET6_ADDRSTRLEN];	/* Mcast addresses on which to send */
  char ports[MAX_CHANNELS_IN_SESSION][MAX_PORT_LENGTH];	/* Local port numbers  */
  
  time_t systime;
  unsigned long long curr_time;

  char *sdp_buf = NULL;
  FILE *sdp_fp;
  struct stat sdp_file_stats;
  int nbytes;

#ifdef _MSC_VER
  int addr_size;
#endif
  
  if(strcmp(a->repair, "") != 0) {
    
    retval = parse_repair_sender_conf_file(a);
    
    if(retval == -1) {
      return -1;
    }
    
    retval = parse_flute_conf_file(a);
    
    if(retval == -1) {
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
    
    nbytes = (int)fread(sdp_buf, 1, sdp_file_stats.st_size, sdp_fp); 
    
    if(parse_sdp_file(a, addrs, ports, sdp_buf) == -1) {
      return -1;
    }
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
      return -1;
    }
  }
  
  *s_id = open_alc_session(&a->alc_a);

  for(i = 0; (int)i < a->alc_a.nb_channel; i++) {
    
    retval = add_alc_channel(*s_id, ports[i], addrs[i], a->alc_a.intface, a->alc_a.intface_name);
    
    if(retval == -1) {
      
      close_alc_session(*s_id);
      return -1;
    }	
  }
    
  retval = repair_sender_in_fdt_based_mode(s_id, a);
  
  return retval;
}
 
int parse_repair_sender_conf_file(arguments_t *a) {
  
   char *buf = NULL;
   FILE *fp;
   struct stat file_stats;
   int nbytes;
  
   char *tmp = NULL;
  
   if(stat(a->repair, &file_stats) == -1) {
     printf("Error: %s is not valid file name\n", a->repair);
     fflush(stdout);
     return -1;
   }
  
   /* Allocate memory for buf */
   if(!(buf = (char*)calloc((file_stats.st_size + 1), sizeof(char)))) {
     printf("Could not alloc memory for buffer!\n");
     fflush(stdout);
     return -1;
   }
  
   if((fp = fopen(a->repair, "rb")) == NULL) {
     printf("Error: unable to open file %s\n", a->repair);
     fflush(stdout);
     free(buf);
     return -1;
   }
  
   nbytes = (int)fread(buf, 1, file_stats.st_size, fp);
  
   if(nbytes <= 0) {
     free(buf);
     fclose(fp);
     return -1;
   }
  
   fclose(fp);
  
   tmp = strtok (buf, "=");
  
   while(tmp != NULL) {

     if(strcmp(tmp, "RequestedBlocksFile") == 0) {
       tmp = strtok(NULL, "\n");
       memset(a->requested_blocks_file, 0, MAX_PATH_LENGTH);
       strcpy(a->requested_blocks_file, tmp);
      
       if(a->requested_blocks_file[strlen(a->requested_blocks_file)-1] == '\r') {
	 a->requested_blocks_file[strlen(a->requested_blocks_file)-1] = '\0';
       }
     }
     else if(strcmp(tmp, "PTMRepairSDPFile") == 0) {
       tmp = strtok(NULL, "\n");
       memset(a->sdp_file, 0, MAX_PATH_LENGTH);
       strcpy(a->sdp_file, tmp);
      
       if(a->sdp_file[strlen(a->sdp_file)-1] == '\r') {
	 a->sdp_file[strlen(a->sdp_file)-1] = '\0';
       }
     }
     else if(strcmp(tmp, "FluteConfFile") == 0) {
       tmp = strtok(NULL, "\n");
       memset(a->flute_conf_file, 0, MAX_PATH_LENGTH);
       strcpy(a->flute_conf_file, tmp);
      
       if(a->flute_conf_file[strlen(a->flute_conf_file)-1] == '\r') {
	 a->flute_conf_file[strlen(a->flute_conf_file)-1] = '\0';
       }
     }
    
     tmp = strtok (NULL, "=");
   }
 
   free(buf);
  
   return 0;
 }

int parse_flute_conf_file(arguments_t *a) {
  
   char *buf = NULL;
   FILE *fp;
   struct stat file_stats;
   int nbytes;
   char *tmp = NULL;
  
   if(stat(a->flute_conf_file, &file_stats) == -1) {
     printf("Error: %s is not valid file name\n", a->flute_conf_file);
     fflush(stdout);
     return -1;
   }
  
   if(!(buf = (char*)calloc((file_stats.st_size + 1), sizeof(char)))) {
     printf("Could not alloc memory for buffer!\n");
     fflush(stdout);
     return -1;
   }
  
   if((fp = fopen(a->flute_conf_file, "rb")) == NULL) {
     printf("Error: unable to open file %s\n", a->flute_conf_file);
     fflush(stdout);
     free(buf);
     return -1;
   }
  
   nbytes = (int)fread(buf, 1, file_stats.st_size, fp); 
  
   if(nbytes <= 0) {
     free(buf);
     fclose(fp);
     return -1;
   }
   fclose(fp);
  
   tmp = strtok (buf, "=");
  
   while(tmp != NULL) {

     if(strcmp(tmp, "BaseDir") == 0) {
       tmp = strtok(NULL, "\n");
      
       memset(a->alc_a.base_dir, 0, MAX_PATH_LENGTH);
       strcpy(a->alc_a.base_dir, tmp);
      
       if(a->alc_a.base_dir[strlen(a->alc_a.base_dir)-1] == '\r') {
	 a->alc_a.base_dir[strlen(a->alc_a.base_dir)-1] = '\0';
       }
     }
     else if(strcmp(tmp, "FECRatio") == 0) {
       tmp = strtok(NULL, "\n");
       a->alc_a.fec_ratio = (unsigned short)atoi(tmp);
     }
     else if(strcmp(tmp, "FDTFile") == 0) {
       tmp = strtok(NULL, "\n");

       memset(a->fdt_file, 0, MAX_PATH_LENGTH);
       strcpy(a->fdt_file, tmp);
      
       if(a->fdt_file[strlen(a->fdt_file)-1] == '\r') {
	 a->fdt_file[strlen(a->fdt_file)-1] = '\0';
       }
     }
    
     tmp = strtok (NULL, "=");
   }
  
   free(buf);
  
   return 0;
 }
