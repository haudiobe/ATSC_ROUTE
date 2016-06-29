/** \file sender.c \brief FLUTE sender
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.63 $
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
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#endif

#include "../alclib/blocking_alg.h"
#include "../alclib/alc_session.h"
#include "../alclib/alc_tx.h"

#include "sender.h"
#include "uri.h"
#include "mad_zlib.h"
#include "flute.h"
#include "fdt.h"
#include "efdt.h"

//Malek El Khatib 14.08.2014
//The below variables need to retain value when going between sender.c and alc_tx.c (i.e. while sending the different segments
  FILE * FLUTEInputFile, *fabcd;
  char str[1000];
  char * tok;
  unsigned long long fileValue;
  BOOL getNextLine = TRUE;

//End

/**
 * This is a private function which sends one FDT instance.
 *
 * @param fdt_instance pointer to buffer containing FDT instance
 * @param fdt_inst_len length of the FDT instance
 * @param s_id session identifier
 * @param tx_mode transmission mode
 * @param eslen encoding symbol length
 * @param max_sblen maximum source block length
 * @param fec_enc_id FEC encoding id
 * @param fec_inst_id FEC instance id
 * @param verbosity verbosity level
 *
 * @return 0 in success, -1 in error cases, -2 when state is SExiting
 *
 */
                                                                                                                                              
int send_fdt_instance(char *fdt_instance, unsigned long long fdt_inst_len, int s_id,
					  int tx_mode, unsigned short eslen, unsigned int max_sblen,
					  unsigned char fec_enc_id, unsigned short fec_inst_id, int verbosity) {

  unsigned long long transfer_len;
	
  int sentbytes = 0;
  int bytes_left;
  int nbytes;
  int retval = 0;
  unsigned int sbn = 0;
  char *buf = NULL;
FILE *fabcd;
  blocking_struct_t *bs;                
                                                                                                                          
  transfer_len = fdt_inst_len;
  bytes_left = (int)transfer_len;
   
  if(tx_mode != TX_THREAD) {

    if(verbosity == 4) {
      printf("Sending FDT Instance (ID=%i)\n", get_fdt_instance_id(s_id));
      fflush(stdout);
    }
    
    set_object_sent_bytes(s_id, 0);
  }
  else {
    if(verbosity == 4) {
      printf("Adding FDT Instance to Tx queue (ID=%i)\n", get_fdt_instance_id(s_id));
      fflush(stdout);
    }
  }

  /* Let's compute the blocking structure */
  
  bs = compute_blocking_structure(transfer_len, max_sblen, eslen);

  while(sbn < bs->N) {

    if(get_session_state(s_id) == SExiting) {
      free(buf);
      return -2;
    }

    if(sbn < bs->I) {
      nbytes = eslen*(bs->A_large);
    }
    else {
      nbytes = eslen*(bs->A_small);
    }

    nbytes = bytes_left < nbytes ? bytes_left : nbytes;

    buf = fdt_instance + sentbytes;

    retval = alc_send(s_id, tx_mode, buf, nbytes, FDT_TOI, transfer_len, eslen, max_sblen, sbn,
		    fec_enc_id, fec_inst_id);

    if(retval == -1) {
      free(bs);
      return -1;
    }
    else if(retval == -2) {
      free(bs);
      return -2;
    }
    
    bytes_left -= nbytes;
    sentbytes += nbytes;

    sbn++;
  }


  if(tx_mode != TX_THREAD) {
     
    if(verbosity == 4) {
      
      printf("FDT Instance sent (ID=%i)\n", get_fdt_instance_id(s_id));
      fflush(stdout);
     
  free(bs);
    }
  }
  else {
    if(verbosity == 4) {
      printf("FDT Instance added to Tx queue (ID=%i)\n", get_fdt_instance_id(s_id));
      fflush(stdout);
    }
  }
    
  return 0;
}
/**
 * This is a private function which sends one file.
 *
 * @param tx_file pointer to buffer containing filepath
 * @param s_id session identifier
 * @param tx_mode transmission mode
 * @param es_len encoding symbol length
 * @param max_sb_len maximum source block length
 * @param fec_enc_id FEC encoding id
 * @param fec_inst_id FEC instance id
 * @param file FDT information for the file
 * @param fdt_inst_len length of the FDT instance
 * @param fdt_inst_buf pointer to buffer containing FDT instance
 * @param verbosity verbosity level
 *
 * @return 0 in success, -1 in error cases, -2 when state is SExiting
 *
 */

int send_file(char *tx_file, int s_id, int tx_mode, unsigned short es_len, unsigned int max_sb_len,
			  unsigned char fec_enc_id, unsigned short fec_inst_id, file_t *file,
			  unsigned long long fdt_inst_len, char *fdt_inst_buf, int verbosity) {

	unsigned long long transfer_len;
	unsigned long long sent = 0;

	int nbytes;
	int read_bytes;
	double print_percent;

	FILE *fp;	
 
#ifdef _MSC_VER
	struct __stat64 file_stats;
#else
	struct stat64 file_stats;
#endif

	char *buf = NULL;

	unsigned int sbn = 0;

	unsigned int Mbytes_of_data = 0;
	int retval = 0;
	alc_session_t *s;

	blocking_struct_t *bs;
  
	s = get_alc_session(s_id);

	set_session_tx_toi(s_id, file->toi);

#ifdef _MSC_VER
	if(_stat64(tx_file, &file_stats) == -1) {
#else
	if(stat64(tx_file, &file_stats) == -1) {
#endif
		printf("Error: %s is not valid file name\n", tx_file);
		fflush(stdout);
		return -1;
	}

	if(file->encoding == NULL) {
		transfer_len = file->content_len;
	}
	else {
		transfer_len = file->transfer_len;
	}

	if(transfer_len == 0) {
		printf("Tx_file %s size = 0\n", tx_file);
		fflush(stdout);
		return -1;
	}

#ifdef _MSC_VER
    if(transfer_len > (unsigned long long)0xFFFFFFFFFFFF) {
#else
	if(transfer_len > 0xFFFFFFFFFFFFULL) {
#endif
		printf("Tx_file %s too big!!\n", tx_file);
		fflush(stdout);
		return -1;
	}
	
	/* Allocate memory for buf */
	if(!(buf = (char*)calloc((unsigned int)(es_len*max_sb_len), sizeof(char)))) {
		printf("Could not alloc memory for buf!\n");
		fflush(stdout);
		return -1;
	}

	/* File to send */
#ifdef _MSC_VER
	if((fp = fopen(tx_file, "rb")) == NULL) {
#else
	if((fp = fopen64(tx_file, "rb")) == NULL) {
#endif
	  printf("Error: unable to open tx_file %s\n", tx_file);
	  fflush(stdout);
	  free(buf);
	  return -1;
	}

	if(tx_mode != TX_THREAD) {
	  if(verbosity > 0) {
#ifdef _MSC_VER
            printf("Sending file: %s (TOI=%I64u)\n", tx_file, file->toi);
#else
            printf("Sending file: %s (TOI=%llu)\n", tx_file, file->toi);
#endif
	    fflush(stdout);
	  }
	  set_object_sent_bytes(s_id, 0);
	  set_object_last_print_tx_percent(s_id, 0);
	}
	else {
	  if(verbosity > 0) {
#ifdef _MSC_VER
            printf("Adding file to Tx queue: %s (TOI=%I64u)\n", tx_file, file->toi);
#else
            printf("Adding file to Tx queue: %s (TOI=%llu)\n", tx_file, file->toi);
#endif
            fflush(stdout);
          }
	}
	
	/* Let's compute the blocking structure */
	bs = compute_blocking_structure(transfer_len, max_sb_len, es_len);

	//Malek El Khatib 21.07.2014
	//Start
	BOOL seekBack=FALSE;	//Used to send blocks out of order
	unsigned int sbnBackup;
	//printf("WHAT ARE THE VALUES FOR TRANSFER LENGTH, MAX_SB_Len, and ES_Len: %llu %u %hu",transfer_len,max_sb_len,es_len);
	//END

	while(sbn < bs->N /*Malek El Khatib 22.07.2014*/ || seekBack /*END*/ ) {

		//Malek El Khatib 22.07.2014
		//Start
		//It is time to send first source block (i.e. After sending all other subsequent blocks)
		//Not Tested
		//if (bs->N > 1 && sendFDTAfterObj && seekBack)
		//{
		//	sbnBackup=sbn;
		//	sbn=0;
		//	fseek(fp,0,SEEK_SET);
		//}
		//End

        if(sbn < bs->I) {
			nbytes = es_len*(bs->A_large);
        }
        else {
			nbytes = es_len*(bs->A_small);
        }
		
		//Malek El Khatib 22.07.2014
		//Start
		//if(bs->N > 1 && sendFDTAfterObj && !seekBack && sbn == 0)
		//{
		//	fseek(fp,nbytes,SEEK_SET);
		//	sbn++;
		//	continue;
		//}
		//END

        memset(buf, 0, (es_len * max_sb_len));
        read_bytes = fread(buf, 1, (unsigned int)nbytes, fp);

		
		//Malek El Khatib 21.7.2014
		//Start
		printf("THIS IS THE NUMBER OF READ BYTES %d\n",read_bytes);
		//if (sbn == 0 && !feof(fp)) {
			//printf("comes here????????????????????????????????????????????????????????????\n");
			//fseek(ptr_myfile,sizeof(struct rec)*counter,SEEK_SET);
		//	sbn++;
		//	continue;
		//}
		//END

		if(ferror(fp)) {
			printf("fread error, file: %s\n", tx_file);
			fflush(stdout);
			free(buf);
			fclose(fp);
			free(bs);
			return -1;
		}

		if(file->encoding == NULL || strcmp(file->encoding, "gzip") == 0) {
			retval = alc_send(s_id, tx_mode, buf, read_bytes, file->toi, transfer_len, es_len, max_sb_len, sbn, fec_enc_id,
							fec_inst_id);
		}
		else if (strcmp(file->encoding, "pad") == 0) {
		  retval = alc_send(s_id, tx_mode, buf, nbytes, file->toi, transfer_len, es_len, max_sb_len, sbn, fec_enc_id,
				    fec_inst_id);
		}

		if(tx_mode != TX_THREAD) {
#ifdef _MSC_VER
		  if(verbosity > 2) {
		    printf("%u/%u Source Blocks sent (TOI=%I64u SBN=%u)\n", (sbn+1), bs->N, file->toi, sbn);
		    fflush(stdout);
		  }
#else
		  if(verbosity > 2) {
		    printf("%u/%u Source Blocks sent (TOI=%llu SBN=%u)\n", (sbn+1), bs->N, file->toi, sbn);
		    fflush(stdout);
		  }
#endif
		}
		
		if(retval == -1) {
		  fclose(fp);
		  free(buf);
		  free(bs);
		  return -1;
		}
		else if(retval == -2) {
		  fclose(fp);
		  free(buf);
		  free(bs);
		  return -2;
		}

		if(fdt_inst_buf != NULL) {
			sent = get_object_sent_bytes(s_id);
			print_percent = get_object_last_print_tx_percent(s_id);

			if((sent/FDT_INTERVAL) > Mbytes_of_data) {

				Mbytes_of_data = (unsigned int)(sent/FDT_INTERVAL);
				retval = send_fdt_instance(fdt_inst_buf, fdt_inst_len, s_id, tx_mode, s->def_eslen,
							   s->def_max_sblen, s->def_fec_enc_id, s->def_fec_inst_id,
							   verbosity);
				 
					
				if(retval == -1) {
					free(bs);
					fclose(fp);
					free(buf);
					return -1;
				}
				else if(retval == -2) {
					free(bs);
					fclose(fp);
					free(buf);
					return -2;
				}

				set_object_sent_bytes(s_id, sent);
				set_object_last_print_tx_percent(s_id, print_percent);
			}
		}

        sbn++;

		//Malek El Khatib 22.07.2014
		//Start
		//if (bs->N > 1 && sendFDTAfterObj)
		//{
		//	if (seekBack)
		//	{
		//		sbn=sbnBackup;
		//		seekBack = FALSE;
		//	}
		//	else if (sbn == bs->N)
		//	{
		//		seekBack = TRUE;
		//	}
		//}
		//End
	}

	free(bs);
	fclose(fp);
	free(buf);

	if(tx_mode != TX_THREAD) {
	  if(verbosity > 0) {
#ifdef _MSC_VER
	    printf("File sent: %s (TOI=%I64u)\n", tx_file, file->toi);
#else 
	    printf("File sent: %s (TOI=%llu)\n", tx_file, file->toi);
#endif
	    fflush(stdout);
	  }
	}
	else {
	  if(verbosity > 0) {
#ifdef _MSC_VER
            printf("File added to Tx queue: %s (TOI=%I64u)\n", tx_file, file->toi);
#else
            printf("File added to Tx queue: %s (TOI=%llu)\n", tx_file, file->toi);
#endif
	    fflush(stdout);
	  }
	}
		
	return 0;
	}
	
/**
 * This is a private function which creates FDT Instance string buffer from file structure(s).
 *
 * @param file pointer to first file structure to be defined in an FDT Instance
 * @param nb_of_files number of files to be defined in an FDT Instance
 * @param fdt pointer to Complete FDT to be splitted to FDT Instances
 * @param s_id session identifier
 * @param fdt_inst_len stores length of created FDT Instance
 *
 * @return pointer to created FDT Instance string buffer, NULL in error cases
 *
 */

char *create_fdt_instance(file_t *file, int nb_of_files, fdt_t *fdt, int s_id,
						  unsigned long long *fdt_inst_len) {

  file_t *tmp_file;
  char *fdt_inst_payload = NULL;
  char tmp_line[MAX_PATH_LENGTH];
  int i;
  int print_help = 0;
  
  unsigned long long size = 0;
  unsigned long long position = 0;
  
  alc_session_t *s;
  
  s = get_alc_session(s_id);
  
  tmp_file = file;
  
  memset(tmp_line, 0, MAX_PATH_LENGTH);
  sprintf(tmp_line, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n");
  size = strlen(tmp_line);
  
  /* Allocate memory for fdt_inst_payload */
  if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
    printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
    printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
    return NULL;
  }
  
  memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
  position += strlen(tmp_line);
  
  memset(tmp_line, 0, MAX_PATH_LENGTH);
  
#ifdef _MSC_VER
  sprintf(tmp_line, "<FDT-Instance Expires=\"%I64u\"", fdt->expires);
#else
  sprintf(tmp_line, "<FDT-Instance Expires=\"%llu\"", fdt->expires);
#endif
  size += strlen(tmp_line);
  
  /* Reallocate memory for fdt_inst_payload */
  if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
    printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
    printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
    return NULL;
  }
  
  memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
  position += strlen(tmp_line);
  
  if(fdt->complete) {

    if(nb_of_files == fdt->nb_of_files) {
      memset(tmp_line, 0, MAX_PATH_LENGTH);
      sprintf(tmp_line, "\n\tComplete=\"true\"");
      size += strlen(tmp_line);
      
      /* Reallocate memory for fdt_inst_payload */
      if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
	printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
	printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
	return NULL;
      }
      
      memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
      position += strlen(tmp_line);
    }
  }

  if(fdt->type != NULL) {
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\tContent-Type=\"%s\"", fdt->type);
    size += strlen(tmp_line);
    
    /* Reallocate memory for fdt_inst_payload */
    if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
      
    }
    
    memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
  }
  
  if(fdt->encoding != NULL) {
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\tContent-Encoding=\"%s\"", fdt->encoding);
    size += strlen(tmp_line);
    
    /* Reallocate memory for fdt_inst_payload */
    if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
      
    }
    
    memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
  }
  
#ifdef FDT_INST_FEC_OTI_COMMON
  if(s->use_fec_oti_ext_hdr == 0) {
	  /**** FEC-OTI parameters ****/
   
		  memset(tmp_line, 0, MAX_PATH_LENGTH);
		  sprintf(tmp_line, "\n\tFEC-OTI-FEC-Encoding-ID=\"%u\"", fdt->fec_enc_id);
		  size += strlen(tmp_line);
		  
		  /* Reallocate memory for fdt_inst_payload */
		  if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			  printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
			  printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
			  return NULL;
		  }
		  
		  memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		  position += strlen(tmp_line);
		  
		  if(fdt->fec_enc_id >= 128) {
			  memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\tFEC-OTI-FEC-Instance-ID=\"%u\"", fdt->fec_inst_id);
			size += strlen(tmp_line);
		  
			/* Reallocate memory for fdt_inst_payload */
			if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			  printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
			  printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
			  return NULL;
			}
		  
			memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
		 }
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\tFEC-OTI-Maximum-Source-Block-Length=\"%u\"",
	    fdt->max_sb_len);
    size += strlen(tmp_line);
    
    /* Reallocate memory for fdt_inst_payload */
    if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
    }
    
    memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\tFEC-OTI-Encoding-Symbol-Length=\"%u\"",
	    fdt->es_len);
    size += strlen(tmp_line);
    
    /* Reallocate memory for fdt_inst_payload */
    if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
    }
    
    memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
    
	if(fdt->fec_enc_id == RS_FEC_ENC_ID || fdt->fec_enc_id == SB_SYS_FEC_ENC_ID) {

		memset(tmp_line, 0, MAX_PATH_LENGTH);
		sprintf(tmp_line, "\n\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"%u\"",
			fdt->max_nb_of_es);
		size += strlen(tmp_line);

		/* Reallocate memory for fdt_inst_payload */
		if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

		memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);
	}

	if(fdt->fec_enc_id == RS_FEC_ENC_ID) {

		memset(tmp_line, 0, MAX_PATH_LENGTH);
		sprintf(tmp_line, "\n\tFEC-OTI-Number-of-Encoding-Symbols-per-Group=\"%u\"",
			fdt->nb_of_es_per_group);
		size += strlen(tmp_line);

		/* Reallocate memory for fdt_inst_payload */
		if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

		memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);

		memset(tmp_line, 0, MAX_PATH_LENGTH);
		sprintf(tmp_line, "\n\tFEC-OTI-Finite-Field-Parameter=\"%u\"", fdt->finite_field);
		size += strlen(tmp_line);

		/* Reallocate memory for fdt_inst_payload */
		if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

		memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);
	}
  }
#endif

  memset(tmp_line, 0, MAX_PATH_LENGTH);
  sprintf(tmp_line, ">\n");
  size += strlen(tmp_line);
  
  /* Reallocate memory for fdt_inst_payload */
  if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
    printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
    printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
    return NULL;
  }
  
  memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
  position += strlen(tmp_line);
  
  for(i = 0; i < nb_of_files; i++) {
    
    if(tmp_file == NULL) {
      break;
    }
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    
#ifdef _MSC_VER
    sprintf(tmp_line, "\t<File TOI=\"%I64u\"", tmp_file->toi);
#else
    sprintf(tmp_line, "\t<File TOI=\"%llu\"", tmp_file->toi);
#endif
    size += strlen(tmp_line);
    
    /* Reallocate memory for fdt_inst_payload */
    if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
    }
    
    memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
    
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\t\tContent-Location=\"%s\"", tmp_file->location);
    size += strlen(tmp_line);
    
    /* Reallocate memory for fdt_inst_payload */
    if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
    }
    
    memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
#ifdef _MSC_VER
        sprintf(tmp_line, "\n\t\tContent-Length=\"%I64u\"", tmp_file->content_len);
#else
		sprintf(tmp_line, "\n\t\tContent-Length=\"%llu\"", tmp_file->content_len);
#endif
		size += strlen(tmp_line);

		/* Reallocate memory for fdt_inst_payload */
		if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

        memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);

		print_help = 0;

		if(tmp_file->type != NULL) {
			if(fdt->type != NULL) {
				if(strcmp(tmp_file->type, fdt->type) != 0) {
					print_help = 1;
				}
			}
			else {
				print_help = 1;
			}
		}	
		if(print_help) {
			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tContent-Type=\"%s\"", tmp_file->type);
			size += strlen(tmp_line);
	
			/* Reallocate memory for fdt_inst_payload */
			if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;

			}

        	memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
		}

        if(tmp_file->md5 != NULL) {
            memset(tmp_line, 0, MAX_PATH_LENGTH);
            sprintf(tmp_line, "\n\t\tContent-MD5=\"%s\"", tmp_file->md5);
			size += strlen(tmp_line);
	
			/* Reallocate memory for fdt_inst_payload */
			if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
        }
			
		print_help = 0;

		if(tmp_file->encoding != NULL) {
			if(fdt->encoding != NULL) {
				if(strcmp(tmp_file->encoding, fdt->encoding) != 0) {
					print_help = 1;
				}
			}
			else {
				print_help = 1;
			}
		}	
		if(print_help) {
			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tContent-Encoding=\"%s\"", tmp_file->encoding);
			size += strlen(tmp_line);

			/* Reallocate memory for fdt_inst_payload */
			if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
		}
		if(tmp_file->encoding != NULL || fdt->encoding != NULL) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
	#ifdef _MSC_VER
				sprintf(tmp_line, "\n\t\tFEC-OTI-Transfer-Length=\"%I64u\"", tmp_file->transfer_len);
	#else
				sprintf(tmp_line, "\n\t\tTransfer-Length=\"%llu\"", tmp_file->transfer_len);
	#endif
				size += strlen(tmp_line);

				/* Reallocate memory for fdt_inst_payload */
				if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
	#ifdef _MSC_VER
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
	#else	
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
	#endif
					return NULL;
				}

				memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
		}

		if(s->use_fec_oti_ext_hdr == 0) {
			/**** FEC-OTI parameters ****/

#ifdef FDT_INST_FEC_OTI_FILE

			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tFEC-OTI-FEC-Encoding-ID=\"%u\"", tmp_file->fec_enc_id);
			size += strlen(tmp_line);
	
			/* Reallocate memory for fdt_inst_payload */
			if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);

			if(tmp_file->fec_encoding_id >= 128) {
		
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-FEC-Instance-ID=\"%u\"", tmp_file->fec_inst_id);
				size += strlen(tmp_line);
	
				/* Reallocate memory for fdt_inst_payload */
				if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}
		
			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tFEC-OTI-Maximum-Source-Block-Length=\"%u\"",
					tmp_file->max_sblen);
			size += strlen(tmp_line);
	
			/* Reallocate memory for fdt_inst_payload */
			if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
		
			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tFEC-OTI-Encoding-Symbol-Length=\"%u\"",
					tmp_file->es_len);
			size += strlen(tmp_line);
	
			/* Reallocate memory for fdt_inst_payload */
			if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
	
			
			if(tmp_file->fec_enc_id == RS_FEC_ENC_ID || tmp_file->fec_enc_id == SB_SYS_FEC_ENC_ID) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"%u\"",
						tmp_file->max_nb_of_es);
				size += strlen(tmp_line);
	
				/* Reallocate memory for fdt_inst_payload */
				if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}

			if(tmp_file->fec_enc_id == RS_FEC_ENC_ID) {

				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\tFEC-OTI-Number-of-Encoding-Symbols-per-Group=\"%u\"",
						tmp_file->nb_es_per_group);
				size += strlen(tmp_line);

				/* Reallocate memory for fdt_inst_payload */
				if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);

				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\tFEC-OTI-Finite-Field-Parameter=\"%u\"", tmp_file->finite_field);
				size += strlen(tmp_line);

				/* Reallocate memory for fdt_inst_payload */
				if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}

#elif defined(FDT_INST_FEC_OTI_COMMON)
			
			if(tmp_file->fec_enc_id != fdt->fec_enc_id) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-FEC-Encoding-ID=\"%u\"", tmp_file->fec_enc_id);
				size += strlen(tmp_line);
		
				/* Reallocate memory for fdt_inst_payload */
				if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}
			
			if(tmp_file->fec_enc_id >= 128) {

				if(tmp_file->fec_inst_id != fdt->fec_inst_id) {
					memset(tmp_line, 0, MAX_PATH_LENGTH);
					sprintf(tmp_line, "\n\t\tFEC-OTI-FEC-Instance-ID=\"%u\"", tmp_file->fec_inst_id);
					size += strlen(tmp_line);
		
					/* Reallocate memory for fdt_inst_payload */
					if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
						printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
						printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
						return NULL;
					}

					memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
					position += strlen(tmp_line);
				}
			}

			if(tmp_file->max_sb_len != fdt->max_sb_len) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-Maximum-Source-Block-Length=\"%u\"",
						tmp_file->max_sb_len);
				size += strlen(tmp_line);
		
				/* Reallocate memory for fdt_inst_payload */
				if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}
			if(tmp_file->es_len != fdt->es_len) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-Encoding-Symbol-Length=\"%u\"",
						tmp_file->es_len);
				size += strlen(tmp_line);
		
				/* Reallocate memory for fdt_inst_payload */
				if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}
        
			if(tmp_file->fec_enc_id == RS_FEC_ENC_ID || tmp_file->fec_enc_id == SB_SYS_FEC_ENC_ID) {
				if(tmp_file->max_nb_of_es != fdt->max_nb_of_es) {
					memset(tmp_line, 0, MAX_PATH_LENGTH);
					sprintf(tmp_line, "\n\t\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"%u\"",
							tmp_file->max_nb_of_es);
					size += strlen(tmp_line);
			
					/* Reallocate memory for fdt_inst_payload */
					if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
						printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
						printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
						return NULL;
					}

					memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
					position += strlen(tmp_line);
				}
			}

			if(tmp_file->fec_enc_id == RS_FEC_ENC_ID) {
				if(tmp_file->nb_of_es_per_group != fdt->nb_of_es_per_group) {
					memset(tmp_line, 0, MAX_PATH_LENGTH);
					sprintf(tmp_line, "\n\tFEC-OTI-Number-of-Encoding-Symbols-per-Group=\"%u\"", tmp_file->nb_of_es_per_group);
					size += strlen(tmp_line);

					/* Reallocate memory for fdt_inst_payload */
					if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
						printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
						printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
						return NULL;
					}

					memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
					position += strlen(tmp_line);
				}
	
				if(tmp_file->finite_field != fdt->finite_field) {
					
					memset(tmp_line, 0, MAX_PATH_LENGTH);
					sprintf(tmp_line, "\n\tFEC-OTI-Finite-Field-Parameter=\"%u\"", tmp_file->finite_field);
					size += strlen(tmp_line);

					/* Reallocate memory for fdt_inst_payload */
					if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
						printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
						printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
						return NULL;
					}

					memcpy((fdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
					position += strlen(tmp_line);
				}
			}
#endif
		}
		
		memset(tmp_line, 0, MAX_PATH_LENGTH);
        sprintf(tmp_line, "/>\n");
		size += strlen(tmp_line);
	
		/* Reallocate memory for fdt_inst_payload */
		if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

		memcpy((fdt_inst_payload + position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);

		if(tmp_file->next == NULL) {
			break;
		}

		tmp_file = tmp_file->next;
	}
 
	memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "</FDT-Instance>\n");
	size += strlen(tmp_line);

	size++;
	
	/* Reallocate memory for fdt_inst_payload */
	if(!(fdt_inst_payload = (char*)realloc(fdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
		printf("Could not (re)alloc memory for fdt_inst_payload, size: %I64u!\n", size);
#else
		printf("Could not (re)alloc memory for fdt_inst_payload, size: %llu!\n", size);
#endif
		return NULL;
	}

    memcpy((fdt_inst_payload + position), tmp_line, strlen(tmp_line));
	position += strlen(tmp_line);
	
	*(fdt_inst_payload + position) = '\0';

	*fdt_inst_len = size - 1;

	return fdt_inst_payload;
}

/**
 * This is a private function which creates EFDT Instance string buffer from file structure(s).
 *
 * @param file pointer to first file structure to be defined in an EFDT Instance
 * @param nb_of_files number of files to be defined in an EFDT Instance
 * @param efdt pointer to Complete EFDT to be splitted to EFDT Instances
 * @param s_id session identifier
 * @param efdt_inst_len stores length of created EFDT Instance
 *
 * @return pointer to created EFDT Instance string buffer, NULL in error cases
 *
 */

char *create_efdt_instance(file_t *file, int nb_of_files, efdt_t *efdt, int s_id,
						  unsigned long long *efdt_inst_len) {

  file_t *tmp_file;
  char *efdt_inst_payload = NULL;
  char tmp_line[MAX_PATH_LENGTH];
  int i;
  int print_help = 0;
  
  unsigned long long size = 0;
  unsigned long long position = 0;
  
  alc_session_t *s;
  
  s = get_alc_session(s_id);
  
  tmp_file = file;
  
  memset(tmp_line, 0, MAX_PATH_LENGTH);
  sprintf(tmp_line, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n");
  size = strlen(tmp_line);
  
  /* Allocate memory for efdt_inst_payload */
  if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
    return NULL;
  }
  
  memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
  position += strlen(tmp_line);
  
  memset(tmp_line, 0, MAX_PATH_LENGTH);
  
  /* Adding efdt elements.*/
  #ifdef _MSC_VER
  sprintf(tmp_line, "<EFDT-Instance TSI=\"%d\"", efdt->tsi);
#else
  sprintf(tmp_line, "<EFDT-Instance TSI=\"%d\"", efdt->tsi);
#endif
  size += strlen(tmp_line);
  
    /* Reallocate memory for efdt_inst_payload */
  if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
    return NULL;
  }
  
  memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
  position += strlen(tmp_line);
  
  
    if(efdt->idRef != NULL) {
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\tIDRef=\"%s\"", efdt->idRef);
    size += strlen(tmp_line);
    
    /* Reallocate memory for efdt_inst_payload */
    if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
      
    }
    
    memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
  }
  
  memset(tmp_line, 0, MAX_PATH_LENGTH);
  sprintf(tmp_line, ">\n");
  size += strlen(tmp_line);
  
    /* Reallocate memory for efdt_inst_payload */
  if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
    return NULL;
  }
  
  memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
  position += strlen(tmp_line);
  
  memset(tmp_line, 0, MAX_PATH_LENGTH);
  /*  EFDT elements added. */
  
#ifdef _MSC_VER
  sprintf(tmp_line, "<FDT-Parameters Expires=\"%I64u\"", efdt->expires);
#else
  sprintf(tmp_line, "<FDT-Parameters Expires=\"%llu\"", efdt->expires);
#endif
  size += strlen(tmp_line);
  
  /* Reallocate memory for efdt_inst_payload */
  if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
    return NULL;
  }
  
  memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
  position += strlen(tmp_line);
  
  if(efdt->complete) {

    if(nb_of_files == efdt->nb_of_files) {
      memset(tmp_line, 0, MAX_PATH_LENGTH);
      sprintf(tmp_line, "\n\tComplete=\"true\"");
      size += strlen(tmp_line);
      
      /* Reallocate memory for efdt_inst_payload */
      if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
	printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
	printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
	return NULL;
      }
      
      memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
      position += strlen(tmp_line);
    }
  }

  if(efdt->type != NULL) {
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\tContent-Type=\"%s\"", efdt->type);
    size += strlen(tmp_line);
    
    /* Reallocate memory for efdt_inst_payload */
    if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
      
    }
    
    memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
  }
  
  if(efdt->encoding != NULL) {
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\tContent-Encoding=\"%s\"", efdt->encoding);
    size += strlen(tmp_line);
    
    /* Reallocate memory for efdt_inst_payload */
    if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
      
    }
    
    memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
  }
  
#ifdef FDT_INST_FEC_OTI_COMMON
  if(s->use_fec_oti_ext_hdr == 0) {
	  /**** FEC-OTI parameters ****/
   
		  memset(tmp_line, 0, MAX_PATH_LENGTH);
		  sprintf(tmp_line, "\n\tFEC-OTI-FEC-Encoding-ID=\"%u\"", efdt->fec_enc_id);
		  size += strlen(tmp_line);
		  
		  /* Reallocate memory for efdt_inst_payload */
		  if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			  printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
			  printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
			  return NULL;
		  }
		  
		  memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		  position += strlen(tmp_line);
		  
		  if(efdt->fec_enc_id >= 128) {
			  memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\tFEC-OTI-FEC-Instance-ID=\"%u\"", efdt->fec_inst_id);
			size += strlen(tmp_line);
		  
			/* Reallocate memory for efdt_inst_payload */
			if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			  printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
			  printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
			  return NULL;
			}
		  
			memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
		 }
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\tFEC-OTI-Maximum-Source-Block-Length=\"%u\"",
	    efdt->max_sb_len);
    size += strlen(tmp_line);
    
    /* Reallocate memory for efdt_inst_payload */
    if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
    }
    
    memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\tFEC-OTI-Encoding-Symbol-Length=\"%u\"",
	    efdt->es_len);
    size += strlen(tmp_line);
    
    /* Reallocate memory for efdt_inst_payload */
    if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
    }
    
    memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
    
	if(efdt->fec_enc_id == RS_FEC_ENC_ID || efdt->fec_enc_id == SB_SYS_FEC_ENC_ID) {

		memset(tmp_line, 0, MAX_PATH_LENGTH);
		sprintf(tmp_line, "\n\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"%u\"",
			efdt->max_nb_of_es);
		size += strlen(tmp_line);

		/* Reallocate memory for efdt_inst_payload */
		if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

		memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);
	}

	if(efdt->fec_enc_id == RS_FEC_ENC_ID) {

		memset(tmp_line, 0, MAX_PATH_LENGTH);
		sprintf(tmp_line, "\n\tFEC-OTI-Number-of-Encoding-Symbols-per-Group=\"%u\"",
			efdt->nb_of_es_per_group);
		size += strlen(tmp_line);

		/* Reallocate memory for efdt_inst_payload */
		if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

		memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);

		memset(tmp_line, 0, MAX_PATH_LENGTH);
		sprintf(tmp_line, "\n\tFEC-OTI-Finite-Field-Parameter=\"%u\"", efdt->finite_field);
		size += strlen(tmp_line);

		/* Reallocate memory for efdt_inst_payload */
		if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

		memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);
	}
  }
#endif

  memset(tmp_line, 0, MAX_PATH_LENGTH);
  sprintf(tmp_line, "/>\n");
  size += strlen(tmp_line);
  
  /* Reallocate memory for efdt_inst_payload */
  if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
    printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
    return NULL;
  }
  
  memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
  position += strlen(tmp_line);
  
  for(i = 0; i < nb_of_files; i++) {
    
    if(tmp_file == NULL) {
      break;
    }
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    
#ifdef _MSC_VER
    sprintf(tmp_line, "\t<File TOI=\"%I64u\"", tmp_file->toi);
#else
    sprintf(tmp_line, "\t<File TOI=\"%llu\"", tmp_file->toi);
#endif
    size += strlen(tmp_line);
    
    /* Reallocate memory for efdt_inst_payload */
    if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
    }
    
    memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
    
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "\n\t\tContent-Location=\"%s\"", tmp_file->location);
    size += strlen(tmp_line);
    
    /* Reallocate memory for efdt_inst_payload */
    if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
      printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
      return NULL;
    }
    
    memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
    position += strlen(tmp_line);
    
    memset(tmp_line, 0, MAX_PATH_LENGTH);
#ifdef _MSC_VER
        sprintf(tmp_line, "\n\t\tContent-Length=\"%I64u\"", tmp_file->content_len);
#else
		sprintf(tmp_line, "\n\t\tContent-Length=\"%llu\"", tmp_file->content_len);
#endif
		size += strlen(tmp_line);

		/* Reallocate memory for efdt_inst_payload */
		if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

        memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);

		print_help = 0;

		if(tmp_file->type != NULL) {
			if(efdt->type != NULL) {
				if(strcmp(tmp_file->type, efdt->type) != 0) {
					print_help = 1;
				}
			}
			else {
				print_help = 1;
			}
		}	
		if(print_help) {
			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tContent-Type=\"%s\"", tmp_file->type);
			size += strlen(tmp_line);
	
			/* Reallocate memory for efdt_inst_payload */
			if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;

			}

        	memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
		}

        if(tmp_file->md5 != NULL) {
            memset(tmp_line, 0, MAX_PATH_LENGTH);
            sprintf(tmp_line, "\n\t\tContent-MD5=\"%s\"", tmp_file->md5);
			size += strlen(tmp_line);
	
			/* Reallocate memory for efdt_inst_payload */
			if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
        }
			
		print_help = 0;

		if(tmp_file->encoding != NULL) {
			if(efdt->encoding != NULL) {
				if(strcmp(tmp_file->encoding, efdt->encoding) != 0) {
					print_help = 1;
				}
			}
			else {
				print_help = 1;
			}
		}	
		if(print_help) {
			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tContent-Encoding=\"%s\"", tmp_file->encoding);
			size += strlen(tmp_line);

			/* Reallocate memory for efdt_inst_payload */
			if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
		}
		if(tmp_file->encoding != NULL || efdt->encoding != NULL) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
	#ifdef _MSC_VER
				sprintf(tmp_line, "\n\t\tFEC-OTI-Transfer-Length=\"%I64u\"", tmp_file->transfer_len);
	#else
				sprintf(tmp_line, "\n\t\tTransfer-Length=\"%llu\"", tmp_file->transfer_len);
	#endif
				size += strlen(tmp_line);

				/* Reallocate memory for efdt_inst_payload */
				if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
	#ifdef _MSC_VER
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
	#else	
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
	#endif
					return NULL;
				}

				memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
		}

		if(s->use_fec_oti_ext_hdr == 0) {
			/**** FEC-OTI parameters ****/

#ifdef FDT_INST_FEC_OTI_FILE

			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tFEC-OTI-FEC-Encoding-ID=\"%u\"", tmp_file->fec_enc_id);
			size += strlen(tmp_line);
	
			/* Reallocate memory for efdt_inst_payload */
			if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);

			if(tmp_file->fec_encoding_id >= 128) {
		
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-FEC-Instance-ID=\"%u\"", tmp_file->fec_inst_id);
				size += strlen(tmp_line);
	
				/* Reallocate memory for efdt_inst_payload */
				if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}
		
			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tFEC-OTI-Maximum-Source-Block-Length=\"%u\"",
					tmp_file->max_sblen);
			size += strlen(tmp_line);
	
			/* Reallocate memory for efdt_inst_payload */
			if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
		
			memset(tmp_line, 0, MAX_PATH_LENGTH);
			sprintf(tmp_line, "\n\t\tFEC-OTI-Encoding-Symbol-Length=\"%u\"",
					tmp_file->es_len);
			size += strlen(tmp_line);
	
			/* Reallocate memory for efdt_inst_payload */
			if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
				printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
				return NULL;
			}

			memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
			position += strlen(tmp_line);
	
			
			if(tmp_file->fec_enc_id == RS_FEC_ENC_ID || tmp_file->fec_enc_id == SB_SYS_FEC_ENC_ID) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"%u\"",
						tmp_file->max_nb_of_es);
				size += strlen(tmp_line);
	
				/* Reallocate memory for efdt_inst_payload */
				if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}

			if(tmp_file->fec_enc_id == RS_FEC_ENC_ID) {

				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\tFEC-OTI-Number-of-Encoding-Symbols-per-Group=\"%u\"",
						tmp_file->nb_es_per_group);
				size += strlen(tmp_line);

				/* Reallocate memory for efdt_inst_payload */
				if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);

				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\tFEC-OTI-Finite-Field-Parameter=\"%u\"", tmp_file->finite_field);
				size += strlen(tmp_line);

				/* Reallocate memory for efdt_inst_payload */
				if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}

#elif defined(FDT_INST_FEC_OTI_COMMON)
			
			if(tmp_file->fec_enc_id != efdt->fec_enc_id) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-FEC-Encoding-ID=\"%u\"", tmp_file->fec_enc_id);
				size += strlen(tmp_line);
		
				/* Reallocate memory for efdt_inst_payload */
				if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}
			
			if(tmp_file->fec_enc_id >= 128) {

				if(tmp_file->fec_inst_id != efdt->fec_inst_id) {
					memset(tmp_line, 0, MAX_PATH_LENGTH);
					sprintf(tmp_line, "\n\t\tFEC-OTI-FEC-Instance-ID=\"%u\"", tmp_file->fec_inst_id);
					size += strlen(tmp_line);
		
					/* Reallocate memory for efdt_inst_payload */
					if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
						printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
						printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
						return NULL;
					}

					memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
					position += strlen(tmp_line);
				}
			}

			if(tmp_file->max_sb_len != efdt->max_sb_len) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-Maximum-Source-Block-Length=\"%u\"",
						tmp_file->max_sb_len);
				size += strlen(tmp_line);
		
				/* Reallocate memory for efdt_inst_payload */
				if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}
			if(tmp_file->es_len != efdt->es_len) {
				memset(tmp_line, 0, MAX_PATH_LENGTH);
				sprintf(tmp_line, "\n\t\tFEC-OTI-Encoding-Symbol-Length=\"%u\"",
						tmp_file->es_len);
				size += strlen(tmp_line);
		
				/* Reallocate memory for efdt_inst_payload */
				if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
					printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
					return NULL;
				}

				memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
				position += strlen(tmp_line);
			}
        
			if(tmp_file->fec_enc_id == RS_FEC_ENC_ID || tmp_file->fec_enc_id == SB_SYS_FEC_ENC_ID) {
				if(tmp_file->max_nb_of_es != efdt->max_nb_of_es) {
					memset(tmp_line, 0, MAX_PATH_LENGTH);
					sprintf(tmp_line, "\n\t\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"%u\"",
							tmp_file->max_nb_of_es);
					size += strlen(tmp_line);
			
					/* Reallocate memory for efdt_inst_payload */
					if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
						printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
						printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
						return NULL;
					}

					memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
					position += strlen(tmp_line);
				}
			}

			if(tmp_file->fec_enc_id == RS_FEC_ENC_ID) {
				if(tmp_file->nb_of_es_per_group != efdt->nb_of_es_per_group) {
					memset(tmp_line, 0, MAX_PATH_LENGTH);
					sprintf(tmp_line, "\n\tFEC-OTI-Number-of-Encoding-Symbols-per-Group=\"%u\"", tmp_file->nb_of_es_per_group);
					size += strlen(tmp_line);

					/* Reallocate memory for efdt_inst_payload */
					if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
						printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
						printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
						return NULL;
					}

					memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
					position += strlen(tmp_line);
				}
	
				if(tmp_file->finite_field != efdt->finite_field) {
					
					memset(tmp_line, 0, MAX_PATH_LENGTH);
					sprintf(tmp_line, "\n\tFEC-OTI-Finite-Field-Parameter=\"%u\"", tmp_file->finite_field);
					size += strlen(tmp_line);

					/* Reallocate memory for efdt_inst_payload */
					if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
						printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
						printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
						return NULL;
					}

					memcpy((efdt_inst_payload + (unsigned int)position), tmp_line, strlen(tmp_line));
					position += strlen(tmp_line);
				}
			}
#endif
		}
		
		memset(tmp_line, 0, MAX_PATH_LENGTH);
        sprintf(tmp_line, "/>\n");
		size += strlen(tmp_line);
	
		/* Reallocate memory for efdt_inst_payload */
		if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
			printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
			return NULL;
		}

		memcpy((efdt_inst_payload + position), tmp_line, strlen(tmp_line));
		position += strlen(tmp_line);

		if(tmp_file->next == NULL) {
			break;
		}

		tmp_file = tmp_file->next;
	}
 
	memset(tmp_line, 0, MAX_PATH_LENGTH);
    sprintf(tmp_line, "</EFDT-Instance>\n");
	size += strlen(tmp_line);

	size++;
	
	/* Reallocate memory for efdt_inst_payload */
	if(!(efdt_inst_payload = (char*)realloc(efdt_inst_payload, ((unsigned int)size * sizeof(char))))) {
#ifdef _MSC_VER
		printf("Could not (re)alloc memory for efdt_inst_payload, size: %I64u!\n", size);
#else
		printf("Could not (re)alloc memory for efdt_inst_payload, size: %llu!\n", size);
#endif
		return NULL;
	}

    memcpy((efdt_inst_payload + position), tmp_line, strlen(tmp_line));
	position += strlen(tmp_line);
	
	*(efdt_inst_payload + position) = '\0';

	*efdt_inst_len = size - 1;

	return efdt_inst_payload;
}
/*End of EFDT instance.*/

#ifdef USE_ZLIB

/**
 * This is a private function which removes tempororary ~gz files in sender side.
 *
 * @param fdt_file pointer to buffer containing FDT file name
 * @param base_dir pointer to buffer containing base directory
 * @param file_path pointer to buffer containing file or directory
 *
 * @return 0 in success, -1 otherwise
 *
 */

int remove_gz_files(char *fdt_file, char *base_dir, char *file_path) {
	
	uri_t *uri;
	char path[MAX_PATH_LENGTH];
	file_t *file;
	fdt_t *fdt;
	char *buf = NULL;
	struct stat	file_stats;
	unsigned int fdt_length = 0; 
    FILE *fp;
	int nbytes;

#ifdef _MSC_VER
	int j;
#endif
	
	if(stat(fdt_file, &file_stats) == -1) {
		printf("Error: %s is not valid file name\n", fdt_file);
		fflush(stdout);
		return -1;
	}

	fdt_length = file_stats.st_size;

	/* Allocate memory for buf, to read fdt file to it */
	if(!(buf = (char*)calloc((fdt_length+1), sizeof(char)))) {
		printf("Could not alloc memory for fdt buffer!\n");
		fflush(stdout);
		return  -1;
	}

	if((fp = fopen(fdt_file, "rb")) == NULL) {
		printf("Error: unable to open fdt_file %s\n", fdt_file);
		fflush(stdout);
		free(buf);
		return -1;
	}

	nbytes = fread(buf, 1, fdt_length, fp);

	if(nbytes <= 0) {
		printf("fread error\n");
		fflush(stdout);
		fclose(fp);
		free(buf);
		return -1;
	}

	fdt = decode_fdt_payload(buf);
	free(buf);
	fclose(fp);

	if(fdt == NULL) {
		return -1;
	}
	
	file = fdt->file_list;

	while(file != NULL) {

		uri = parse_uri(file->location, strlen(file->location));

		memset(path, 0, MAX_PATH_LENGTH);

		if(!(strcmp(base_dir, "") == 0)) {
			strcpy(path, base_dir);
			strcat(path, "/");
		}
		strcat(path, uri->path);
		strcat(path, GZ_SUFFIX);

#ifdef _MSC_VER
		for(j = 0; j < (int)strlen(path); j++) {
			if(path[j] == '/') {
				path[j] = '\\';
			}
		}
#endif

		if(remove(path) == -1) {
			printf("%s: errno %i\n", path, errno);
			fflush(stdout);
		}

		file = file->next;
	}

	return 0;
}

#endif


/**
 * This is a private function which sends files defined in the FDT.
 *
 * @param sender structure containing sender information
 * @param tx_mode transmission mode (NO_TX_THREAD or TX_THREAD)
 * @param a pointer to arguments structure where command line arguments are parsed
 *
 * @return 0 in success, -1 in error cases, -2 when state is SExiting
 *
 */

int fdtbasedsend(flute_sender_t *sender, int tx_mode, arguments_t *a) {
  
  char *fdt_inst_buf = NULL;
  char *efdt_inst_buf = NULL;
  unsigned long long fdt_inst_len = 0; 
  unsigned long long efdt_inst_len = 0; 

#ifdef USE_ZLIB
  char *compr_fdt_inst_buf = NULL;
  char *compr_efdt_inst_buf = NULL;
  unsigned long long compr_fdt_inst_buf_len = 0;
  unsigned long long compr_efdt_inst_buf_len = 0;
  #endif
  
#ifdef _MSC_VER
  int j;
#endif
    
  file_t *file;
  uri_t *uri;
  
  char path[MAX_PATH_LENGTH];
  
  static int nb_of_instances_in_last_round = 0;
  
  unsigned short file_es_len = a->alc_a.es_len;
  unsigned int file_max_sb_len = a->alc_a.max_sb_len;
  unsigned char file_fec_enc_id = a->alc_a.fec_enc_id;
  unsigned short file_fec_inst_id = a->alc_a.fec_inst_id;
  
  time_t systime;
  
  unsigned long long curr_time;
  
  
  int sent = 0 ,z;
  BOOL incomplete_fdt = FALSE;
  
  time(&systime);
  curr_time = systime + 2208988800U;

  /*
  set_session_tx_toi(s_id, 0);
  */

  set_fdt_instance_id(sender->s_id, (get_fdt_instance_id(sender->s_id) - nb_of_instances_in_last_round));
  nb_of_instances_in_last_round = 0;

  if(sender->fdt->expires < curr_time) {
    set_session_state(sender->s_id, SExiting);
    return -2;
  }

  if(sender->fdt->complete || a->complete_fdt > 0) {
     
    if(!sender->fdt->complete) {
      sender->fdt->complete = TRUE;
      sender->efdt->complete = TRUE;
    }
    
    /* Send "Complete FDT" with all File definitions at the beginning of the carousel */
    
    file = sender->fdt->file_list;
   //for(z=0;z<25;z++){
    efdt_inst_buf = create_efdt_instance(file, sender->efdt->nb_of_files, sender->efdt, sender->s_id, &efdt_inst_len);
    fdt_inst_buf = create_fdt_instance(file, sender->fdt->nb_of_files, sender->fdt, sender->s_id, &fdt_inst_len);
  
			  
	

    if(fdt_inst_buf == NULL) {
      return -1;
    }
    
#ifdef USE_ZLIB
    if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
      
      compr_efdt_inst_buf = buffer_zlib_compress(efdt_inst_buf, efdt_inst_len, &compr_efdt_inst_buf_len);
      compr_fdt_inst_buf = buffer_zlib_compress(fdt_inst_buf, fdt_inst_len, &compr_fdt_inst_buf_len);
      
      if(compr_fdt_inst_buf == NULL) {
		free(fdt_inst_buf);
		return -1;
      }	
	}
#endif
    
			  
#ifdef USE_ZLIB
    if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
      sent = send_fdt_instance(compr_efdt_inst_buf, compr_efdt_inst_buf_len, sender->s_id, tx_mode, a->alc_a.es_len,
			       a->alc_a.max_sb_len, a->alc_a.fec_enc_id, a->alc_a.fec_inst_id, a->alc_a.verbosity);
      
    }
    else {
      sent = send_fdt_instance(efdt_inst_buf, efdt_inst_len, sender->s_id, tx_mode, a->alc_a.es_len, a->alc_a.max_sb_len,
			       a->alc_a.fec_enc_id, a->alc_a.fec_inst_id, a->alc_a.verbosity);
      
			
		
      
    }
#else
    sent = send_fdt_instance(efdt_inst_buf, efdt_inst_len, sender->s_id, tx_mode, a->alc_a.es_len, a->alc_a.max_sb_len,
			     a->alc_a.fec_enc_id, a->alc_a.fec_inst_id, a->alc_a.verbosity);

    
#endif

    if(a->complete_fdt != 2) {
        set_fdt_instance_id(sender->s_id, (get_fdt_instance_id(sender->s_id) + 1));
        nb_of_instances_in_last_round++;
	}
	
#ifdef USE_ZLIB
	if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
	  free(compr_fdt_inst_buf);
	  compr_fdt_inst_buf = NULL;
	  compr_fdt_inst_buf_len = 0;
	}
#endif
	free(fdt_inst_buf);
	fdt_inst_buf = NULL;
	fdt_inst_len = 0; 
    
    if(sent == -1) {
      return -1;
    }
    else if(sent == -2) {		
	  return -2;
    }
  }
  //}
  file = sender->fdt->file_list;
  
  //Malek El Khatib 16.04.2014
  //START
  unsigned long long timeInUsec = 0L;		//Used later for timing purposes
  /*int loopCount = 0;						//This is used to emulate live production of DASH segments
											//After sending Audio/Video segments pair, wait for 2 sec (i.e. segment duration) before sending the next pair
  unsigned long long audioDelay= audio_segment;		//This is audio segment duration in microseconds
  unsigned long long videoDelay= video_segment;		//This is video segment duration in microseconds
  unsigned long long difference= 0;
  unsigned long long firstWait = 0;
  unsigned long long secondWait = 0;
  int toggle = 0;							//Used when we reach a point where two segments of same type needs to be sent before continuing with other type
											//This occurs at some point in time for type with shorter segment duration
  
  if ( audioDelay < videoDelay)
  {
	  //These values are used to determine sleep duration before every send. Note, 1 video (audio) segment is sent per videoDelay (audioDelay)
	  difference = videoDelay - audioDelay;
	  firstWait = audioDelay;
  }
  else
  {
	  difference = audioDelay - videoDelay;
	  firstWait = videoDelay;
  }

  secondWait = difference;
  */

  struct timeval start_timer,end_timer,currentTime;
  unsigned long long elapsedTime= 0L;
  int USDSent=0;

  if (sendingTimesFile != "")
  {
	  FLUTEInputFile = fopen(sendingTimesFile , "r");
	  if(FLUTEInputFile == NULL) {
		  perror("Error opening input file. Ignore it");
		  sendingTimesFile="";
	  }
  }
  //END Malek El Khatib

  while(file != NULL) { /* loop for sending all files in fdt */
	  
	  
	  //Malek El Khatib 14.08.2014
	  //Start
	  struct timeval fdt_time,send_time;		//send_time is used later to check time needed to send file
	  //if (sendFDTAfterObj)						//Malek El Khatib 20.08.2014 Commented this
	  //	  gettimeofday(&send_time, NULL);
	 // else
	//	  gettimeofday(&fdt_time, NULL);

	  //Delaying
	  if (numEncSymbPerPacket != 0 && sendingTimesFile !="")
	  {
		  if (getNextLine)
		  {
			  if( fgets (str, 1000,  FLUTEInputFile)!=NULL ) {

				  tok = strtok(str, " ");
				  fileValue = strtoull (tok, NULL, 0);

				  if (fileValue == file->toi)
				  {
					  tok = strtok(NULL, " \n");
					  fileValue = strtoull (tok, NULL, 0);
					  //printf("Current field is %llu\n",fileValue); //Malek El Khatib 19.08.2014 Commented this
					  getNextLine = TRUE;
					  gettimeofday(&currentTime, NULL);
					  if (fileValue > ((unsigned long long)currentTime.tv_sec*1000000 + (unsigned long long)currentTime.tv_usec))
					  {
						  elapsedTime = fileValue - (unsigned long long)currentTime.tv_sec*1000000 - (unsigned long long)currentTime.tv_usec;
						  usleep (elapsedTime);
					  }
				  }
				  else
					  getNextLine = FALSE;
			  }
		  }
		  else
		  {
			  if (fileValue == file->toi)
			  {
				  tok = strtok(NULL, " \n");
				  fileValue = strtoull (tok, NULL, 0);
				  //printf("Current field is %llu\n",fileValue); //Malek El Khatib 19.08.2014 Commented this
				  getNextLine = TRUE;
				  gettimeofday(&currentTime, NULL);
				  if(fileValue > ((unsigned long long)currentTime.tv_sec*1000000 + (unsigned long long)currentTime.tv_usec))
				  {
				  		elapsedTime = fileValue - (unsigned long long)currentTime.tv_sec*1000000 - (unsigned long long)currentTime.tv_usec;
					  	usleep (elapsedTime);
				  }
			  }
			  else
				  getNextLine = FALSE;
		  }
	  }
	  //This was added here to avoid sending any FDT data with the file itself
	  fdt_inst_buf = NULL;

	  //Note that below configuration assumes that objects in FDT are listed in the order they are available
	  //If sender code is modified to regularly check new objects in sending folder, then below two usleep operations and corresponding conditional statements would be removed
	  //since objects would be sent as soon as they available
	  gettimeofday(&start_timer, NULL);
	  //END

        
    if(file->toi == 0) {
		incomplete_fdt = TRUE;
    }
    else if(file->location == NULL) {
		incomplete_fdt = TRUE;
    }
    
    if(file->encoding != NULL) {
#ifdef USE_ZLIB
		if(!strcmp(file->encoding, "gzip") || !strcmp(file->encoding, "pad")) {
#else
		if(!strcmp(file->encoding, "pad")) {
#endif
			if(file->content_len == 0) {
				incomplete_fdt = TRUE;
			}
			else if(file->transfer_len == 0) {
				incomplete_fdt = TRUE;
			}
		}
	}
	
	if(incomplete_fdt) {
#ifdef _MSC_VER
		printf("FDT does not contain enough File information, TOI: %I64u\n", file->toi);
#else
		printf("FDT does not contain enough File information, TOI: %llu\n", file->toi);
#endif
		fflush(stdout);
		return -1;
	}
	
	if((file->encoding != NULL) && (strcmp(a->file_path, "") == 0)) {
		printf("Content encoding is not supported with -f option.\n");
		fflush(stdout);
		return -1;
	}
      
	uri = parse_uri(file->location, strlen(file->location));
      
    memset(path, 0, MAX_PATH_LENGTH);
      
      if(!(strcmp(a->alc_a.base_dir, "") == 0)) {
	strcpy(path, a->alc_a.base_dir);
	strcat(path, "/");
      }
      strcat(path, uri->path);

#ifdef USE_ZLIB
      if(file->encoding != NULL) {
	if(!strcmp(file->encoding, "gzip")) {
	  strcat(path, GZ_SUFFIX);                     
	}
      }
#endif
      		
      if(tx_mode != TX_THREAD) {
	if(((file->es_len != 0) && (file->max_sb_len != 0))) {
	  file_es_len = file->es_len;
	  file_max_sb_len = file->max_sb_len;
	}
      }
      
      if(file->fec_enc_id != -1) {
	if(file->fec_enc_id != file_fec_enc_id) {
	  file_fec_enc_id = (unsigned char)file->fec_enc_id;
	  file_fec_inst_id = (unsigned short)file->fec_inst_id;
	}
      }
      
#ifdef _MSC_VER
      for(j = 0; j < (int)strlen(path); j++) {
	if(*(path + j) == '/') {
	  *(path + j) = '\\';
	}
      }
#endif

	  
	  //struct timeval fdt_time,send_time;		//send_time is used later to check time needed to send file //Malek El Khatib 18.08.2014..Commented this 

	  //Malek El Khatib 16.07.2014
	  //Start
	  fprintf(logFilePtr,"%s ", uri->path);		//Malek El Khatib 20.08.2014	Added this

	  //If sendFDTAfterObj is false then we send the FDT before sending object; hence, same code is replicated below after file sending
	  if (!sendFDTAfterObj) {
      //END  
  
		  //Malek El Khatib 06.05.2014
		  //Start
		  //Check time needed to generate FDT Instance. NOTE THAT THIS IS APPLICABLE ONLY IF COMPLETE_FDT_SENT = FALSE	  
		  //Start of FDT instance generation	  
		  //struct timeval fdt_time,send_time;		//send_time is used later to check time needed to send file	 //Malek El Khatib 18.08.2014..Commented this  
		  gettimeofday(&fdt_time, NULL);			// Malek El Khatib 20.08.2014..Added This
		  timeInUsec = (unsigned long long)fdt_time.tv_sec*1000000 + (unsigned long long)fdt_time.tv_usec;
		  //printf("Writing to Log File\n"); //Malek El Khatib 19.08.2014 Commented this
		  fprintf(logFilePtr,"%llu ", timeInUsec);	  
		  //END Malek El Khatib
 
		  if(a->complete_fdt != 2) {  
			  /* Send FDT Instance with next File definition before the file is sent */
			  //fdt_inst_buf = create_fdt_instance(file, 1, sender->fdt, sender->s_id, &fdt_inst_len);
			  efdt_inst_buf = create_efdt_instance(file, 1, sender->efdt, sender->s_id, &efdt_inst_len);
			 //  efdt_inst_buf = create_efdt_instance(file, sender->efdt->nb_of_files, sender->efdt, sender->s_id, &efdt_inst_len);
			      FILE *fabcd;
   fabcd=fopen("ErrorDebugging.txt", "w");
			 
		
			//  fprintf(fabcd, "%llu\n", tmp->toi);
			  fprintf(fabcd, efdt_inst_buf);
			  fclose(fabcd);
			  if(efdt_inst_buf == NULL) {
				  free_uri(uri);
				  return -1;
			  }
      
#ifdef USE_ZLIB
			  if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
				  compr_efdt_inst_buf = buffer_zlib_compress(efdt_inst_buf, efdt_inst_len, &compr_efdt_inst_buf_len);
	
				  if(compr_efdt_inst_buf == NULL) {
					  free_uri(uri);
					  free(efdt_inst_buf);
					  return -1;
				  }	
			  }
#endif
	    
			  //Malek El Khatib 06.05.2014	 
			  //Start	  
			  //Start of FDT Instance Sending	  
			  gettimeofday(&fdt_time, NULL);
			  timeInUsec = (unsigned long long)fdt_time.tv_sec*1000000 + (unsigned long long)fdt_time.tv_usec;
			  fprintf(logFilePtr,"%llu ", timeInUsec);
			  //END Malek El Khatib

#ifdef USE_ZLIB
			  if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
				  sent = send_fdt_instance(compr_efdt_inst_buf, compr_efdt_inst_buf_len, sender->s_id, tx_mode, a->alc_a.es_len,
					  a->alc_a.max_sb_len, a->alc_a.fec_enc_id, a->alc_a.fec_inst_id, a->alc_a.verbosity);
				  
			  }
			  else {
				  sent = send_fdt_instance(efdt_inst_buf, efdt_inst_len, sender->s_id, tx_mode, a->alc_a.es_len, a->alc_a.max_sb_len,
					  a->alc_a.fec_enc_id, a->alc_a.fec_inst_id, a->alc_a.verbosity);
				  
				
			  }
#else
			  sent = send_fdt_instance(efdt_inst_buf, efdt_inst_len, sender->s_id, tx_mode, a->alc_a.es_len, a->alc_a.max_sb_len,
				  a->alc_a.fec_enc_id, a->alc_a.fec_inst_id, a->alc_a.verbosity);
			  
#endif
	
			  //Malek El Khatib 06.05.2014
			  //Start
			  //End of FDT instance sending
			  gettimeofday(&fdt_time, NULL);
			  timeInUsec = (unsigned long long)fdt_time.tv_sec*1000000 + (unsigned long long)fdt_time.tv_usec;
			  fprintf(logFilePtr,"%llu ", timeInUsec);
			  //END Malek El Khatib

			  if(sent == -1) {	  
#ifdef USE_ZLIB
				  if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
					  free(compr_fdt_inst_buf);
				  }
#endif
				  free(fdt_inst_buf);
				  free_uri(uri);
				  return -1;
			  }

			  else if(sent == -2) {

#ifdef USE_ZLIB
				  if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
					  free(compr_fdt_inst_buf);
				  }
#endif
				  free(fdt_inst_buf);
				  free_uri(uri);
				  return -2;
			  }
		  }
	}


	  //Malek El Khatib 06.05.2014
	  //Start
	  //Start of file sending
	  //gettimeofday(&send_time, NULL); //Malek El Khatib 18.08.2014..Commented this
	  //timeInUsec = (unsigned long long)send_time.tv_sec*1000000 + (unsigned long long)send_time.tv_usec; //Malek El Khatib 18.08.2014..Commented this
	  /*if (!sendFDTAfterObj) //This conditional statement is needed to generate correct loggings	//Malek El Khatib 20.08.2014...Commented this
	  {
		  gettimeofday(&send_time, NULL); // Malek El Khatib 18.08.2014..Added this
		  timeInUsec = (unsigned long long)send_time.tv_sec*1000000 + (unsigned long long)send_time.tv_usec;// Malek El Khatib 18.08.2014..Added this
		  fprintf(logFilePtr,"%llu ", timeInUsec);
	  }
	  else
	  {
		  timeInUsec = (unsigned long long)send_time.tv_sec*1000000 + (unsigned long long)send_time.tv_usec; // Malek El Khatib 18.08.2014..Added this
		  //printf("Writing to Log File\n"); //Malek El Khatib 19.08.2014 Commented this
		  fprintf(logFilePtr,"%s %llu ",uri->path, timeInUsec);
	  }//*/
	  //END Malek El Khatib

#ifdef USE_ZLIB
      if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
			sent = send_file(path, sender->s_id, tx_mode, file_es_len, file_max_sb_len, file_fec_enc_id,
			 file_fec_inst_id, file, compr_efdt_inst_buf_len, compr_efdt_inst_buf,
			 a->alc_a.verbosity);
      }
      else {
			sent = send_file(path, sender->s_id, tx_mode, file_es_len, file_max_sb_len, file_fec_enc_id,
			 file_fec_inst_id, file, efdt_inst_len, efdt_inst_buf, a->alc_a.verbosity);
			
			
      }
#else
      sent = send_file(path, sender->s_id, tx_mode, file_es_len, file_max_sb_len, file_fec_enc_id,
		       file_fec_inst_id, file, efdt_inst_len, efdt_inst_buf, a->alc_a.verbosity);	
#endif

	  //Malek El Khatib 06.05.2014
	  //Start
	  //End of file sending
	  gettimeofday(&send_time, NULL);
	  timeInUsec = (unsigned long long)send_time.tv_sec*1000000 + (unsigned long long)send_time.tv_usec;

	  if (!sendFDTAfterObj) //This conditional statement is needed to generate correct loggings
		  fprintf(logFilePtr,"%llu\n", timeInUsec);
	  else
		  fprintf(logFilePtr,"%llu ", timeInUsec);
	  

	  //loopCount++;	//This is used to determine delay period before sending next file 
	  //END Malek El Khatib


	  //Malek El Khatib 16.07.2014
	  //Start
	  //If sendFDTAfterObj is true then we send the FDT now after having sent the complete object
	  if (sendFDTAfterObj)
	  {
		  //Check time needed to generate FDT Instance. NOTE THAT THIS IS APPLICABLE ONLY IF COMPLETE_FDT_SENT = FALSE
		  //Start of FDT instance generation
		  //struct timeval fdt_time,send_time;		//send_time is used later to check time needed to send file
		  gettimeofday(&fdt_time, NULL);
		  timeInUsec = (unsigned long long)fdt_time.tv_sec*1000000 + (unsigned long long)fdt_time.tv_usec;
		  fprintf(logFilePtr,"%llu ", timeInUsec);
		  
		  if(a->complete_fdt != 2) {
			  /* Send FDT Instance with next File definition before the file is sent */
			 /*fdt_inst_buf = create_fdt_instance(file, 1, sender->fdt, sender->s_id, &fdt_inst_len);
			  if(fdt_inst_buf == NULL) {
				  free_uri(uri);
				  return -1;
			  }
      
#ifdef USE_ZLIB
			  if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
				  
				  compr_fdt_inst_buf = buffer_zlib_compress(fdt_inst_buf, fdt_inst_len, &compr_fdt_inst_buf_len);

				  if(compr_fdt_inst_buf == NULL) {
					  free_uri(uri);
					  free(fdt_inst_buf);
					  return -1;
				  }	
			  }
#endif
	  
			  //Start of FDT Instance Sending
			  gettimeofday(&fdt_time, NULL);
			  timeInUsec = (unsigned long long)fdt_time.tv_sec*1000000 + (unsigned long long)fdt_time.tv_usec;
			  fprintf(logFilePtr,"%llu ", timeInUsec);
      
#ifdef USE_ZLIB
			  if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
				  sent = send_fdt_instance(compr_fdt_inst_buf, compr_fdt_inst_buf_len, sender->s_id, tx_mode, a->alc_a.es_len,
					  a->alc_a.max_sb_len, a->alc_a.fec_enc_id, a->alc_a.fec_inst_id, a->alc_a.verbosity);
			  }
			  
			  else {
				  sent = send_fdt_instance(fdt_inst_buf, fdt_inst_len, sender->s_id, tx_mode, a->alc_a.es_len, a->alc_a.max_sb_len,
					  a->alc_a.fec_enc_id, a->alc_a.fec_inst_id, a->alc_a.verbosity);
				    
			  }
#else
			  sent = send_fdt_instance(fdt_inst_buf, fdt_inst_len, sender->s_id, tx_mode, a->alc_a.es_len, a->alc_a.max_sb_len,
				  a->alc_a.fec_enc_id, a->alc_a.fec_inst_id, a->alc_a.verbosity);
#endif

			  //End of FDT instance sending
			  gettimeofday(&fdt_time, NULL);
			  timeInUsec = (unsigned long long)fdt_time.tv_sec*1000000 + (unsigned long long)fdt_time.tv_usec;
			  fprintf(logFilePtr,"%llu\n", timeInUsec);

			  if(sent == -1) {
#ifdef USE_ZLIB
			  if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
				free(compr_fdt_inst_buf);
			  }
#endif
			  free(fdt_inst_buf);
			  free_uri(uri);
			  return -1;
		}
			  else if(sent == -2) {
	  
#ifdef USE_ZLIB
				  if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
					  free(compr_fdt_inst_buf);
				  }
#endif
				  free(fdt_inst_buf);
				  free_uri(uri);
				  return -2;
			  }*/
		  }
	  }
	  //END

      free_uri(uri);

      if((a->complete_fdt != 2)) {
	
		set_fdt_instance_id(sender->s_id, (get_fdt_instance_id(sender->s_id) + 1));
		nb_of_instances_in_last_round++;
      
#ifdef USE_ZLIB
		if(a->alc_a.encode_content == ZLIB_FDT || a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
			free(compr_fdt_inst_buf);
		}
#endif
		free(fdt_inst_buf);
      }
      
      if(sent == -1) {
		return -1;
      }
      else if(sent == -2) {		  
		 return -2;
      }
      
      file = file->next;
	  
	  //Malek El Khatib 08.07.2014
	  //Start
	  gettimeofday(&end_timer, NULL);
	  elapsedTime = (unsigned long long)end_timer.tv_sec*1000000 + (unsigned long long)end_timer.tv_usec - (unsigned long long)start_timer.tv_sec*1000000 - (unsigned long long)start_timer.tv_usec;
	  printf("elapsedTime %llu\n", elapsedTime);
	  //if (USDSent >=1)
		//  usleep(video_segment - elapsedTime);
	  //else
		//  USDSent++;
	  //End

    }

    return 0;
  }

/**
 * This is a private function which checks if there is FEC OTI in the FDT.
 *
 * @param fdt pointer to the Complete FDT
 *
 * @return TRUE if there is FEC OTI in the FDT, FALSE otherwise
 *
 */

BOOL IsFECOTIInFDT(fdt_t *fdt) {
	file_t *file = NULL;
	BOOL fec_oti_in_fdt = TRUE;

	file =	fdt->file_list;
 
	while(file != NULL) { /* loop for checking all files in fdt */
	
		if(file->fec_enc_id == -1) {
			fec_oti_in_fdt = FALSE;
			break;
		}
		else if(((file->fec_enc_id >= 128) && (file->fec_inst_id == -1))) {
			fec_oti_in_fdt = FALSE;
			break;
		}
		else if(file->max_sb_len == 0) {
			fec_oti_in_fdt = FALSE;
			break;
		}
		else if(file->es_len == 0) {
			fec_oti_in_fdt = FALSE;
			break;
		}
		else if(((file->fec_enc_id == SB_SYS_FEC_ENC_ID) &&
				 (file->max_nb_of_es == 0))) {
			fec_oti_in_fdt = FALSE;
			break;
		}
			
		file = file->next;
	}

	return fec_oti_in_fdt;
}

int sender_in_fdt_based_mode(arguments_t *a, flute_sender_t *sender) {

  int retval = 0;
  int retcode = 0;
  
  alc_session_t *s;
  
  time_t systime;
  BOOL is_printed = FALSE;
  
  unsigned long long curr_time;

  struct stat fdt_file_stats;
  FILE *fp, *fabcd;

  char *buf = NULL;

  unsigned long long fdt_length = 0, **tempo;
  long long nbytes = 0;
  fdt_t *fdt;
  efdt_t *efdt;
  BOOL is_fec_oti_in_fdt = TRUE;

  if(stat(a->fdt_file, &fdt_file_stats) == -1) {
    printf("Error: %s is not valid file name\n", a->fdt_file);
    fflush(stdout);
    return -1;
  }

  fdt_length = fdt_file_stats.st_size;

  /* Allocate memory for buf, to read fdt file to it */
  if(!(buf = (char*)calloc((unsigned int)(fdt_length + 1), sizeof(char)))) {
    printf("Could not alloc memory for fdt buffer!\n");
    return -1;
  }

  if((fp = fopen(a->fdt_file, "rb")) == NULL) {
    printf("Error: unable to open FDT file %s\n", a->fdt_file);
    fflush(stdout);
    free(buf);
    return -1;
  }

  nbytes = fread(buf, 1, (unsigned int)fdt_length, fp);

  if(nbytes <= 0) {
    printf("fread error\n");
    fflush(stdout);
    fclose(fp);
    free(buf);
    return -1;
  }
  	
  sender->efdt = decode_efdt_payload(buf);

 
  fdt= calloc(1, sizeof(fdt_t));
  fdt->expires=sender->efdt->expires;
  fdt->file_list= sender->efdt->file_list;
  fdt->nb_of_files= sender->efdt->nb_of_files;
  fdt->type= sender->efdt->type;
  fdt->encoding= sender->efdt->encoding;
  fdt->fec_enc_id= sender->efdt->fec_enc_id;
  fdt->fec_inst_id= sender->efdt->fec_inst_id;
  fdt->finite_field= sender->efdt->finite_field;
  fdt->nb_of_es_per_group= sender->efdt->nb_of_es_per_group;
  fdt->max_sb_len= sender->efdt->max_sb_len;
  fdt->es_len= sender->efdt->es_len;
  fdt->max_nb_of_es= sender->efdt->max_nb_of_es;
  //fdt->complete = TRUE;
  //sender->efdt->complete= TRUE;
  sender->fdt=fdt;
  
  free(buf);
  fclose(fp);

  if(sender->fdt == NULL) {
    return -1;
  }

  time(&systime);
  curr_time = systime + 2208988800U;

  if(sender->fdt->expires < curr_time) {
    set_session_state(sender->s_id, SExiting);
    return -2;
  }
  
  if(a->alc_a.use_fec_oti_ext_hdr == 0) {
	/* check that we have FEC OTI in FDT */	
	
	is_fec_oti_in_fdt = IsFECOTIInFDT(sender->fdt);

	if(is_fec_oti_in_fdt == FALSE) {			
#ifdef _MSC_VER
		printf("FDT does not contain enough FEC-OTI information\n");
#else
		printf("FDT does not contain enough FEC-OTI information\n");
#endif
		fflush(stdout);
		return -1;
	}
  }

  s = get_alc_session(sender->s_id);
  
  if(a->alc_a.start_time != 0) {
    while(1) {
      
      time(&systime);
      curr_time = systime + 2208988800U;
      
      if(a->alc_a.start_time > curr_time) {
	
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
      
      if(s->state == SExiting) {
	return -2;
      }
    }
  }

  while(a->alc_a.nb_tx) {
    
    if(a->alc_a.cc_id == Null) {
      
      if(a->alc_a.nb_channel == 1) {
		retcode = fdtbasedsend(sender, NO_TX_THREAD, a);
      }
      else {
		retcode = fdtbasedsend(sender, TX_THREAD, a);
      }
    }
    else if(a->alc_a.cc_id == RLC) {
      retcode = fdtbasedsend(sender, TX_THREAD, a);
    }

    if(retcode == -1) {
      printf("\nError: fdtbasedsend() failed\n");
      retval = -1;
      break;
    }
    else if(retcode == -2) {
      retval = -2;
      break;
    }

    if(!a->cont) {
      a->alc_a.nb_tx--;
    }
    
    if(((a->alc_a.cc_id == RLC) || ((a->alc_a.cc_id == Null) && (a->alc_a.nb_channel != 1)))) {
      
      while(s->tx_queue_begin != NULL) {
	
	if(s->state == SExiting) {
#ifdef USE_ZLIB
	  if(a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
	    remove_gz_files(a->fdt_file, a->alc_a.base_dir, a->file_path);
	  }
#endif
	  return -2;
	}
#ifdef _MSC_VER
	Sleep(1);
#else
	usleep(1000);
#endif			
      }
    }
  }
 
#ifdef USE_ZLIB
  if(a->alc_a.encode_content == ZLIB_FDT_AND_GZIP_FILES) {
    remove_gz_files(a->fdt_file, a->alc_a.base_dir, a->file_path);
  }
#endif

  return retval;
}
