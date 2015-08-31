/** \file /flute-http-repair-server/mbms-rel6-flute-repair/main.c \brief Implementation of repair server for FLUTE protocol
 *
 *  $Author: peltotal $ $Date: 2007/02/27 12:50:48 $ $Revision: 1.15 $
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "../../alclib/blocking_alg.h"
#include "../../alclib/transport.h"
#include "../../alclib/null_fec.h"
#include "../../alclib/xor_fec.h"
#include "../../alclib/rs_fec.h"

#include "../../flutelib/fdt.h"
#include "../../flutelib/uri.h"

#include "decode_query_string.h"
#include "parse_query_string.h"
#include "add_fec_plid.h"
#include "helpers.h"

/**
 * This function is programs main function.
 *
 * @return 0 in success, -1 otherwise
 *
 */

int main(void) {

  char *request_method = NULL;
  char *size_of_query_string = NULL;
  char *query_string = NULL;
  char *decoded_query_string = NULL;
  blocking_struct_t *bs = NULL;
  query_str_t *qs = NULL;
  char filepath[MAX_PATH_LENGTH];
  struct stat64 file_stats;
  struct stat fdtfile_stats;
  char *fdt_buf = NULL;
  unsigned int fdt_length;
  unsigned fdt_nbytes;
  FILE *fdt_fp;
  fdt_t *fdt = NULL;
  file_t *file = NULL;

  int fec_ratio = 0;
  
  char *buf = NULL;
  unsigned long long nbytes;
  unsigned long long pos;
  int fp;

  trans_block_t *tr_block = NULL;
  trans_unit_t *tr_unit = NULL;

  qs_missing_block_t *ms_block = NULL;
  qs_missing_symbol_t *ms_symbol = NULL;

  int i;
  char *uri_path = NULL;

  char *repair_conf_file = NULL;
  repair_arguments_t ra;
  int retval;
  FILE *ptm_repair_file;

  unsigned int sbn = 0;

  repair_conf_file = getenv("RepairConfFile");

  if(repair_conf_file == NULL) {
    return -1;
  }

  if(repair_conf_file[strlen(repair_conf_file)-1] == '\r') {
    repair_conf_file[strlen(repair_conf_file)-1] = '\0';
  }
  
  retval = parse_repair_conf_file(repair_conf_file, &ra);
    
  if(retval == -1) {
    return -1;
  }

  retval = parse_flute_conf_file(&ra);
    
  if(retval == -1) {
    return -1;
  }

  if(stat(ra.fdt_file, &fdtfile_stats) == -1) {
    return -1;
  }

  fdt_length = fdtfile_stats.st_size;

  /* Allocate memory for buf, to read fdt file to it */
  if(!(fdt_buf = (char*)calloc((fdt_length + 1), sizeof(char)))) {
    return -1;
  }

  if((fdt_fp = fopen(ra.fdt_file, "rb")) == NULL) {
    free(fdt_buf);
    return -1;
  }

  fdt_nbytes = fread(fdt_buf, 1, fdt_length, fdt_fp);

  if(fdt_nbytes <= 0) {
    fclose(fdt_fp);
    free(fdt_buf);
    return -1;
  }

  fdt = decode_fdt_payload(fdt_buf);
  free(fdt_buf);
  fclose(fdt_fp);

  request_method = getenv("REQUEST_METHOD");

  if(strcmp(request_method, "GET") == 0) {
    query_string = getenv("QUERY_STRING");

    if(query_string == NULL) {
      FreeFDT(fdt);
      return -1;
    }
  }
  else {
      size_of_query_string = getenv("CONTENT_LENGTH");

      if(!(query_string = (char*)calloc((atoi(size_of_query_string) + 1), sizeof(char)))) {
	FreeFDT(fdt);
	return -1;
      }

      read(0, query_string, atoi(size_of_query_string)); /* 0 is stdin */
  }

  decoded_query_string = decode_query_string(query_string);
  
  if(decoded_query_string == NULL) {

    if(strcmp(request_method, "POST") == 0) {
      free(query_string);
    }
    FreeFDT(fdt);
    return -1;
  }

  /* parse query string */
  qs = parse_query_string(decoded_query_string);
  
  if(qs == NULL) {
    free(decoded_query_string);

    if(strcmp(request_method, "POST") == 0) {
      free(query_string);
    }

    FreeFDT(fdt);
    return -1;
  }

  /* Find file struct */

  file = fdt->file_list;

  while(file != NULL) {
    uri_path = get_uri_host_and_path(qs->fileURI);

    if(strstr(file->location, uri_path) != NULL) {
      break;
    }

    file = file->next;
  }

  if(file == NULL) {

    free(decoded_query_string);

    if(strcmp(request_method, "POST") == 0) {
      free(query_string);
    }

    FreeFDT(fdt);
    free_query_str(qs);
    return -1;
  }

  if(strcmp(ra.repair_method, "PTM") == 0){

    if((ptm_repair_file = fopen(ra.requested_blocks_file, "a")) == NULL) {
      free(decoded_query_string);

      if(strcmp(request_method, "POST") == 0) {
	free(query_string);
      }

      free_query_str(qs);
      FreeFDT(fdt);
      return -1;
    }

    ms_block = qs->block_list;

    if(ms_block == NULL) {
      fprintf(ptm_repair_file, "%llu:*\n", file->toi);
    }
    else {
      while(ms_block != NULL) {
	fprintf(ptm_repair_file, "%llu:%i\n", file->toi, ms_block->sbn);
	ms_block = ms_block->next;
      }
    }
    
    free(decoded_query_string);

    if(strcmp(request_method, "POST") == 0) {
      free(query_string);
    }

    free_query_str(qs);
    FreeFDT(fdt);
    fclose(ptm_repair_file);
    
    printf("%s%c%c", "Location:", 13, 10);
    printf("%c%c", 13, 10);
  }
  else {
    memset(filepath, 0, MAX_PATH_LENGTH);
    
    if(ra.base_dir != NULL) {
      if(!(strcmp(ra.base_dir, "") == 0)) {
	strcpy(filepath, ra.base_dir);
	strcat(filepath, "/");
      }
    } 
    strcat(filepath, get_uri_host_and_path(qs->fileURI));

    if(file->encoding != NULL) {
#ifdef USE_ZLIB
      if(strcmp(file->encoding, "gzip") == 0) {
	strcat(filepath, GZ_SUFFIX);
      }
#endif
    }

    /* use stat to get file size */
    
    if(stat64(filepath, &file_stats) == -1) {
      free(decoded_query_string);

      if(strcmp(request_method, "POST") == 0) {
	free(query_string);
      }

      free_query_str(qs);
      FreeFDT(fdt);
      return -1;

    }
    
    if(file_stats.st_size == 0) {
      free(decoded_query_string);

      if(strcmp(request_method, "POST") == 0) {
	free(query_string);
      }

      free_query_str(qs);
      FreeFDT(fdt);
      return -1;
    }
    
    /* calculate blocking structure */

    bs = compute_blocking_structure((unsigned long long)file_stats.st_size, file->max_sb_len, file->es_len);
       
    /* Allocate memory for buf */
    if(!(buf = (char*)calloc((unsigned int)(file->es_len * file->max_sb_len), sizeof(char)))) {
      free(decoded_query_string);

      if(strcmp(request_method, "POST") == 0) {
	free(query_string);
      }

      free_query_str(qs);
      free(bs);
      FreeFDT(fdt);
      return -1;
    }
    
    /* File to repair */
    if((fp = open64(filepath, 0, 0)) < 0) {
      free(decoded_query_string);

      if(strcmp(request_method, "POST") == 0) {
	free(query_string);
      }

      free_query_str(qs);
      free(bs);
      free(buf);
      FreeFDT(fdt);
      return -1;
    }
        
    /* http response message */    
    /* http response message headers*/

    printf("%s%c%c", "Content-Type: application/simpleSymbolContainer", 13, 10);
    printf("%s%c%c", "Content-Transfer-Encoding: binary", 13, 10);
    printf("%c%c", 13, 10);
    
    ms_block = qs->block_list;

    if(ms_block == NULL) {
     
      while(sbn < bs->N) {
	
	if(sbn < bs->I) {
          nbytes = file->es_len * (bs->A_large);
        }
        else {
          nbytes = file->es_len * (bs->A_small);
        }
	
	memset(buf, 0, (file->es_len * file->max_sb_len));
        nbytes = read(fp, buf, (unsigned int)nbytes);
	
	if(nbytes < 0) {
	  free(decoded_query_string);

	  if(strcmp(request_method, "POST") == 0) {
	    free(query_string);
	  }

          free_query_str(qs);
          free(buf);
          free(bs);
          close(fp);
          FreeFDT(fdt);
	  return -1;
	}
	
	/* all could use null_fec_encode_src_block() functions, because FEC symbols are not transmitted */
	
        if(file->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
          tr_block = null_fec_encode_src_block(buf, nbytes, sbn, file->es_len);
        }
        else if(file->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
          tr_block = xor_fec_encode_src_block(buf, nbytes, sbn, file->es_len);
        }
        else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID) && (file->fec_inst_id == REED_SOL_FEC_INST_ID))) {
          tr_block = rs_fec_encode_src_block(buf, nbytes, sbn, file->es_len, fec_ratio, file->max_sb_len);
        }
        else {
          free(decoded_query_string);

	  if(strcmp(request_method, "POST") == 0) {
	    free(query_string);
	  }

          free_query_str(qs);
          free(buf);
          free(bs);
          close(fp);
          FreeFDT(fdt);
          return -1;
        }
	
	if(tr_block == NULL) {
          free(decoded_query_string);

	  if(strcmp(request_method, "POST") == 0) {
	    free(query_string);
	  }

          free_query_str(qs);
          free(buf);
          free(bs);
          close(fp);
          FreeFDT(fdt);
          return -1;
        }
	
	tr_unit = tr_block->unit_list;
	
	add_length_indicator(tr_block->k);

	if(file->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
	  add_fec_plid_0_130((unsigned short)tr_block->sbn, (unsigned short)tr_unit->esi);
	}
	else if(file->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
	  add_fec_plid_128(tr_block->sbn, tr_unit->esi);
	}
	else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID) && (file->fec_inst_id == REED_SOL_FEC_INST_ID))) {
	  add_fec_plid_129(tr_block->sbn, (unsigned int)tr_block->k, (unsigned int)tr_unit->esi);
	}
	
	while(1) {
	  
	  for(i = 0; i < tr_unit->len; i++) {
	    printf("%c", tr_unit->data[i]);
	  }
	  
	  if(tr_unit->esi == (tr_block->k - 1)) { /* no FEC symbols */
	    break;
	  }
	  
	  tr_unit++;
	}

	tr_unit = tr_block->unit_list;

        while(1) {
          free(tr_unit->data);

          if(tr_unit->esi == (tr_block->n - 1)) {
            break;
          }

          tr_unit++;
        }

        free(tr_block->unit_list);
        free(tr_block);

        sbn++;
      }
    }
    else {
    
      while(ms_block != NULL) {
	
	/* Set place where to read */
	
	if(ms_block->sbn < bs->I) {
	  pos = (unsigned long long)ms_block->sbn * (unsigned long long)bs->A_large * (unsigned long long)file->es_len;
	}
	else {
	  pos = ( ( ( (unsigned long long)bs->I * (unsigned long long)bs->A_large ) + 
		    ( (unsigned long long)ms_block->sbn - (unsigned long long)bs->I ) * (unsigned long long)bs->A_small ) * (unsigned long long)file->es_len );
	}

	/* set correct position */
	
	if(lseek64(fp, pos, SEEK_SET) == -1) {
	  free(decoded_query_string);

	  if(strcmp(request_method, "POST") == 0) {
	    free(query_string);
	  }

	  free_query_str(qs);
	  free(buf);
	  free(bs);
	  close(fp);
	  FreeFDT(fdt);
	  return -1;
	}
	
	if(ms_block->sbn < bs->I) {
	  nbytes = file->es_len * (bs->A_large);
	}
	else {
	  nbytes = file->es_len * (bs->A_small);
	}
	
	memset(buf, 0, (file->es_len * file->max_sb_len));
	nbytes = read(fp, buf, (unsigned int)nbytes);
	
	/* all could use null_fec_encode_src_block() functions, because FEC symbols are not transmitted */
	
	if(file->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
	  tr_block = null_fec_encode_src_block(buf, nbytes, ms_block->sbn, file->es_len);
	}
	else if(file->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
	  tr_block = xor_fec_encode_src_block(buf, nbytes, ms_block->sbn, file->es_len);
	}
	else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID) && (file->fec_inst_id == REED_SOL_FEC_INST_ID))) {
	  tr_block = rs_fec_encode_src_block(buf, nbytes, ms_block->sbn, file->es_len, fec_ratio, file->max_sb_len);
	}
	else {
	  free(decoded_query_string);

	  if(strcmp(request_method, "POST") == 0) {
	    free(query_string);
	  }

	  free_query_str(qs);
	  free(buf);
	  free(bs);
	  close(fp);
	  FreeFDT(fdt);
	  return -1; 
	}
	
	if(tr_block == NULL) {
	  free(decoded_query_string);

	  if(strcmp(request_method, "POST") == 0) {
	    free(query_string);
	  }

	  free_query_str(qs);
	  free(buf);
	  free(bs);
	  close(fp);
	  FreeFDT(fdt);
	  return -1;
	}
	
	ms_symbol = ms_block->es_list;
	
	if(ms_symbol == NULL) {
	  tr_unit = tr_block->unit_list;
	  
	  add_length_indicator(tr_block->k);
	  
	  if(file->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
	    add_fec_plid_0_130((unsigned short)ms_block->sbn, (unsigned short)tr_unit->esi);
	  }
	  else if(file->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
	    add_fec_plid_128(ms_block->sbn, tr_unit->esi);
	  }
	  else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID) && (file->fec_inst_id == REED_SOL_FEC_INST_ID))) {
	    add_fec_plid_129(ms_block->sbn, (unsigned int)tr_block->k, (unsigned int)tr_unit->esi);
	  }
	  
	  while(1) {
	    
	    for(i = 0; i < tr_unit->len; i++) {
	      printf("%c", tr_unit->data[i]);
	    }
	    
	    if(tr_unit->esi == (tr_block->k - 1)) { /* no FEC symbols */
	      break;
	    }
	    
	    tr_unit++;
	  }
	}
	else {
	  
	  while(ms_symbol != NULL) {
	    
	    tr_unit = tr_block->unit_list;
	    
	    while(1) {
	      if(tr_unit->esi == ms_symbol->esi) {
		add_length_indicator(1);
		
		if(file->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
		  add_fec_plid_0_130((unsigned short)ms_block->sbn, (unsigned short)ms_symbol->esi);
		}
		else if(file->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
		  add_fec_plid_128(ms_block->sbn, ms_symbol->esi);
		}
		else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID) && (file->fec_inst_id == REED_SOL_FEC_INST_ID))) {
		  add_fec_plid_129(ms_block->sbn, (unsigned short)tr_block->k,
				   (unsigned short)ms_symbol->esi);
		}
		
		for(i = 0; i < tr_unit->len; i++) {
		  printf("%c", tr_unit->data[i]);
		}
		
		break;
	      }
	      
	      if(tr_unit->esi == (tr_block->k - 1)) { /* no FEC symbols */
		break;
	      }
	      
	      tr_unit++;
	    }
	    ms_symbol = ms_symbol->next;
	  }
	}
	
	tr_unit = tr_block->unit_list;
	
	while(1) {
	  free(tr_unit->data);
	  
	  if(tr_unit->esi == (tr_block->n - 1)) {
	    break;
	  }
	  
	  tr_unit++;
	} 
	
	free(tr_block->unit_list);
	free(tr_block);
	
	ms_block = ms_block->next;
      }
    }

    free(decoded_query_string);

    if(strcmp(request_method, "POST") == 0) {
      free(query_string);
    }

    free_query_str(qs);
    free(bs);
    free(buf);
    close(fp);        
    FreeFDT(fdt);
  }

  return 0;
}
