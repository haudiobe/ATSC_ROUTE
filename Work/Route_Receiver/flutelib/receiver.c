/** \file receiver.c \brief FLUTE receiver
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.47 $
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
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#ifdef _MSC_VER
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#include <process.h>
#include <io.h>
#include <direct.h>
#include <shellapi.h>
#else
#include <ctype.h>
#include <unistd.h>
#endif

#include "../alclib/alc_session.h"
#include "../alclib/alc_rx.h"

#include "receiver.h"
#include "mad_zlib.h"
#include "mad_md5.h"
#include "padding_encoding.h"
#include "uri.h"
#include "fdt.h"
#include "efdt.h"

/**
 * This is a private function which checks if all wanted files are received.
 *
 * @param file_uri_table pointers to wanted file URIs
 *
 * @return 1 if all files are received, 0 otherwise
 *
 */

 int all_files_received(char *file_uri_table[FILE_URI_TABLE_SIZE]) {

   int i;
   int retval = 1;

   for(i = 0; i < FILE_URI_TABLE_SIZE; i++) {
     if(file_uri_table[i] != NULL) {
       retval = 0;
     }
   }

   return retval;
 }

/**
 * This is a private function which sets file received.
 *
 * @param file_uri_table pointers to wanted file URIs
 * @param file_uri pointer to file's path
 *
 */

void set_file_received(char *file_uri_table[FILE_URI_TABLE_SIZE], char *file_uri) {

  int i;
  
  for(i = 0; i < FILE_URI_TABLE_SIZE; i++) {
    
    if(file_uri_table[i] != NULL) {
      if(strcmp(file_uri, file_uri_table[i]) == 0) {
	file_uri_table[i] = NULL;
	break;
      }
    }
  }
}

/**
 * This is a private function which receives file identified with TOI.
 *
 * @param s_id session identifier
 * @param filepath pointer to file's path string, used when creating file and storing data to it
 * @param toi transport object identifier for the desired file
 * @param file_len the length of file
 * @param md5 MD5 checksum for the file
 * @param rx_memory_mode receiver memory consumption mode
 * @param encoding content encoding for file
 * @param verbosity verbosity level
 *
 * @return 1 in success, 0 when state is SClosed, -1 in error cases, -2 when state is SExiting,
 * -3 when state is STxStopped, -4 when MD5 check error occurs
 *
 */

int recvfile(int s_id, char *filepath, unsigned long long toi,
	     unsigned long long file_len,
	     char *md5, int rx_memory_mode, char *encoding, int verbosity) {
  
#ifdef USE_ZLIB
  char *uncompr_buf = NULL;
  unsigned long long uncompr_buflen = 0;
#endif

  unsigned char fdt_cont_enc_algo = 0;
  int fdt_instance_id = 0;

  struct stat file_stats;
  
  unsigned long long recvbytes = 0;
  
  char *buf = NULL;
  int fd;
  int j;

  char filename[MAX_PATH_LENGTH];
  char tmp_filename[MAX_PATH_LENGTH];
  
  char *tmp_file_name;
  int retcode = 0;

  int retval;
  char fullpath[MAX_PATH_LENGTH];
  char *tmp = NULL;
  char *ptr;
  int point;
  int ch = '/';
  int i = 0;
  FILE *fabcd;
#ifdef USE_OPENSSL
  char *md5_digest = NULL;
#endif
  
  char* session_basedir = get_session_basedir(s_id);

  if(filepath != NULL) {

    if(verbosity > 0) {  
#ifdef _MSC_VER
      printf("Receiving file: %s (TOI=%I64u)\n", filepath, toi);
#else
      printf("Receiving file: %s (TOI=%llu)\n", filepath, toi);
#endif
      fflush(stdout);
    }

    for(j = 0; j < (int)strlen(filepath); j++) {
      if(*(filepath + j) == '\\') {
	*(filepath + j) = '/';
      }
    }
  }
  else {

    if(verbosity > 0) {
#ifdef _MSC_VER
      printf("Receiving object: TOI=%I64u\n", toi);
#else
      printf("Receiving object: TOI=%llu\n", toi);
#endif
      fflush(stdout);
    }
  }

  if(toi == FDT_TOI) {

			
    buf = fdt_recv(s_id, &recvbytes, &retcode, &fdt_cont_enc_algo, &fdt_instance_id);

    if(buf == NULL) {
      return retcode;
    }

    /* open tmp file */

    memset(tmp_filename, 0, MAX_PATH_LENGTH);
    sprintf(tmp_filename, "%s/%s", session_basedir, "object_XXXXXX");
    mktemp(tmp_filename);

#ifdef _MSC_VER
    if((fd = open((const char*)tmp_filename,
		  _O_WRONLY | _O_CREAT | _O_BINARY | _O_TRUNC , _S_IWRITE)) < 0) {
#else
    if((fd = open(tmp_filename, O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU)) < 0) {
#endif
      printf("Error: unable to open file %s\n", tmp_filename);
      fflush(stdout);
      return MEM_ERROR;
    }
    
    if(fdt_cont_enc_algo == 0) {

      /* write buf to tmp file */

      if(write(fd, buf, (unsigned int)recvbytes) == -1) {
#ifdef _MSC_VER
        printf("write error, toi: %I64u\n", toi);
#else
        printf("write error, toi: %llu\n", toi);
#endif
        fflush(stdout);
        free(buf);
        close(fd);
        return MEM_ERROR;
      }
      free(buf);
    }
#ifdef USE_ZLIB
    else if(fdt_cont_enc_algo == ZLIB) {
      uncompr_buf = buffer_zlib_uncompress(buf, recvbytes, &uncompr_buflen);
      
      if(uncompr_buf == NULL) {
	free(buf);
	return -1;
      }
      free(buf);
      
      /* write uncompr_buf to tmp file */
      
      if(write(fd, uncompr_buf, (unsigned int)uncompr_buflen) == -1) {
#ifdef _MSC_VER
        printf("write error, toi: %I64u\n", toi);
#else
        printf("write error, toi: %llu\n", toi);
#endif
        fflush(stdout);
        free(uncompr_buf);
        close(fd);
        return -1;
      }
      free(uncompr_buf);
    }
#endif
    close(fd);
  }
  else {	
    if(rx_memory_mode == 1) {
      
      tmp_file_name = alc_recv3(s_id, &toi, &retcode);
      
      if(tmp_file_name == NULL) {
	return retcode;
      }
      
      memset(tmp_filename, 0, MAX_PATH_LENGTH);
      memcpy(tmp_filename, tmp_file_name, strlen(tmp_file_name));
      free(tmp_file_name);    
    }
    else if(rx_memory_mode == 2) {
      printf("-b:2 option is not supported with object mode\n");
      fflush(stdout);
      return -1;
    }
    else {
      
      buf = alc_recv(s_id, toi, &recvbytes, &retcode);
      
      if(buf == NULL) {
	return retcode;
      }

      /* open tmp file */

      memset(tmp_filename, 0, MAX_PATH_LENGTH);

      if(encoding == NULL) {
	   sprintf(tmp_filename, "%s/%s", session_basedir, "object_XXXXXX");
	   mktemp(tmp_filename);
      }
#ifdef USE_ZLIB
      else if(strcmp(encoding, "gzip") == 0) {
	   sprintf(tmp_filename, "%s/%s", session_basedir, "object_XXXXXX");
	   strcat(tmp_filename, GZ_SUFFIX);
	   mktemp(tmp_filename);
      }
#endif
      else if(strcmp(encoding, "pad") == 0) {
	   sprintf(tmp_filename, "%s/%s", session_basedir, "object_XXXXXX");
	   strcat(filename, PAD_SUFFIX);
	   mktemp(tmp_filename);
      }

#ifdef _MSC_VER
      if((fd = open((const char*)tmp_filename,
		    _O_WRONLY | _O_CREAT | _O_BINARY | _O_TRUNC , _S_IWRITE)) < 0) {
#else
      if((fd = open(tmp_filename, O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU)) < 0) {
#endif
	printf("Error: unable to open file %s\n", tmp_filename);
	fflush(stdout);
	return -1;
      }
      
      /* write buffer to tmp file */
      
      if(write(fd, buf, (unsigned int)recvbytes) == -1) {
#ifdef _MSC_VER
	printf("write error, toi: %I64u\n", toi);
#else
	printf("write error, toi: %llu\n", toi);
#endif
	fflush(stdout);
	free(buf);
	close(fd);
	return -1;
      }
      
      free(buf);
      close(fd);
    }
      
#ifdef USE_OPENSSL
    if(md5 != NULL) {
	    
      md5_digest = file_md5(tmp_filename);
      
      if(md5_digest == NULL) {
	printf("MD5 check failed!\n");
	fflush(stdout);
	remove(tmp_filename);
	return -4;
      }
      else{
	if(strcmp(md5, md5_digest) != 0) {
	  printf("MD5 check failed!\n");
	  fflush(stdout);
	  remove(tmp_filename);
	  free(md5_digest);
	  return -4;
	}
	
	free(md5_digest);
      }
    }
#endif
    if(encoding != NULL) {
      
      if(strcmp(encoding, "pad") == 0) {
	
	retcode = padding_decoder(tmp_filename, (int)file_len);
	
	if(retcode == -1) {
	  return -1;             
	}
	
	*(tmp_filename + (strlen(tmp_filename) - PAD_SUFFIX_LEN)) = '\0';
	
	if(file_len != 0) {
	  
	  if(stat(tmp_filename, &file_stats) == -1) {
	    printf("Error: %s is not valid file name\n", tmp_filename);
	    fflush(stdout);
	    return -1;
	  }
	  if(file_stats.st_size != file_len) {
	    printf("Error: padding decoding failed\n");
	    remove(tmp_filename);
	    fflush(stdout);
	    return -1;
	  }
	}
      }
#ifdef USE_ZLIB
      else if(strcmp(encoding, "gzip") == 0) {
	
	retcode = file_gzip_uncompress(tmp_filename);
	
	if(retcode == -1) {
	  return -1;
	}
	
	*(tmp_filename + (strlen(tmp_filename) - GZ_SUFFIX_LEN)) = '\0';
	
	if(file_len != 0) {
	  
	  if(stat(tmp_filename, &file_stats) == -1) {
	    printf("Error: %s is not valid file name\n", tmp_filename);
	    fflush(stdout);
	    return -1;
	  }
	  
	  if(file_stats.st_size != file_len) {
	    printf("Error: uncompression failed\n");
	    remove(tmp_filename);
	    fflush(stdout);
	    return -1;
	  }
	}
      }
#endif
    }
  }
  
  if(filepath == NULL) {

    if(verbosity > 0) {      
#ifdef _MSC_VER
      printf("Object received: TOI=%I64u\n", toi);
#else
      printf("Object received: TOI=%llu\n", toi);
#endif
      fflush(stdout);
    }

    return 1;
  }

  if(!(tmp = (char*)calloc((strlen(filepath) + 1), sizeof(char)))) {
    printf("Could not alloc memory for tmp (filepath)!\n");
    fflush(stdout);    
    return -1;
  }
  
  memcpy(tmp, filepath, strlen(filepath));
  
  ptr = strchr(tmp, ch);
  
  memset(fullpath, 0, MAX_PATH_LENGTH);
  memcpy(fullpath, session_basedir, strlen(session_basedir));
  
  if(ptr != NULL) {
    
    while(ptr != NULL) {
      
      i++;
      
      point = (int)(ptr - tmp);
      
      memset(filename, 0, MAX_PATH_LENGTH);
      memcpy((fullpath + strlen(fullpath)), "/", 1);
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
	  
	  if(toi == FDT_TOI) {
#ifdef USE_ZLIB
	    if(fdt_cont_enc_algo == ZLIB) {
	      free(uncompr_buf);
	    }
	    else {
	      free(buf);
	    }
#else
	    free(buf);
#endif
	  }
	  free(tmp);
	  return -1;
	}
      }
      
      strcpy(tmp, filename);
      ptr = strchr(tmp, ch);
    }
    memcpy((fullpath + strlen(fullpath)), "/", 1);
	memcpy((fullpath + strlen(fullpath)), filename, strlen(filename));
  }
  else{
    memcpy((fullpath + strlen(fullpath)), "/", 1);
    memcpy((fullpath + strlen(fullpath)), filepath, strlen(filepath));
  }
    
  if(rename(tmp_filename, fullpath) < 0) {
      
    if(errno == EEXIST) {
      retval = remove(fullpath);
      
      if(retval == -1) {
	printf("errno: %i\n", errno);
      }
      
      if(rename(tmp_filename, fullpath) < 0) {
	printf("rename() error1\n");
      }
    }
    else {
      printf("rename() error2\n");
    }
  }
    
  free(tmp);
    
  if(verbosity > 0) {
#ifdef _MSC_VER
    printf("File received: %s (TOI=%I64u)\n", filepath, toi);
#else
    printf("File received: %s (TOI=%llu)\n", filepath, toi);
#endif
    fflush(stdout);
  }
  
  return 1;
}

/**
 * This is a private function which receives files defined in an FDT file using alc_recv2() or alc_recv3() function.
 *
 * @param rx_memory_mode receiver memory consumtion mode
 * @param openfile open received file automatically (TRUE = yes, FALSE = no) [only in Windows]
 * @param receiver pointer to structure containing receiver information
 *
 * @return 1 in success, 0 when state is SClosed, -1 in error cases, -2 when state is SExiting,
 * -3 when state is STxStopped, -4 when MD5 check error occurs
 *
 */

int fdtbasedrecv(int rx_memory_mode, BOOL openfile, flute_receiver_t *receiver) {
  file_t *file = NULL;
  file_t *next_file = NULL;
  time_t systime;
  
  unsigned long long curr_time;

  unsigned long long toi;
  unsigned long long transfer_len;
  int i;
  char filename[MAX_PATH_LENGTH];
  
  BOOL is_all_files_received;
  BOOL is_printed = FALSE;
  BOOL any_files_received = FALSE;
  
  int retcode;
  
  unsigned char content_enc_algo = 0;
  struct stat file_stats;
  int retval;
  char *tmp = NULL;
  char fullpath[MAX_PATH_LENGTH];
  char *filepath = NULL;
  char *ptr = NULL;
  int point;
  int ch = '/';
  uri_t *uri = NULL;
FILE *fabcd;
  //Malek El Khatib 07.05.2014
  //Start
  unsigned long long timeInUsec = 0L;		//Used later for timing purposes
  struct timeval processing_time;
  //End
 
#ifdef USE_OPENSSL
  char *md5= NULL;
#endif
  
  char* session_basedir = NULL;
  
  /* rx_memory_mode == 1 || 2 */
  char *tmp_file_name;

  /* rx_memory_mode == 0 */
  char *buf = NULL;
  int fd;

  char tmp_filename[MAX_PATH_LENGTH];

  session_basedir = get_session_basedir(receiver->s_id);
  
  while(1) {
    
    if(get_session_state(receiver->s_id) == SExiting) {
      return -2;
    }
    else if(get_session_state(receiver->s_id) == STxStopped) {
      return -3;
    }
    
    is_all_files_received = TRUE;
    
    time(&systime);
    curr_time = systime + 2208988800U;
    
    next_file = receiver->fdt->file_list;

    while(next_file != NULL) {

      file = next_file;
      i = 0;
      
      if(((file->expires < curr_time) && (!receiver->accept_expired_fdt_inst))) {
	
	if(file->next != NULL) {
	  file->next->prev = file->prev;
	}
	
	if(file->prev != NULL) {
	  file->prev->next = file->next;
	}
	
	if(file == receiver->fdt->file_list) {
	  receiver->fdt->file_list = file->next;
	}
	
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

	continue;
      }
      
      if(receiver->rx_automatic) {
	if(file->status != 2) {
          is_all_files_received = FALSE;
          is_printed = FALSE;
        }
        if(file->status == 2) {
          any_files_received = TRUE;
        }
      }
      else if(receiver->wildcard_token != NULL && strstr(file->location, receiver->wildcard_token) != NULL) {
	if(file->status != 2) {
          is_all_files_received = FALSE;
          is_printed = FALSE;
        }
        if(file->status == 2) {
          any_files_received = TRUE;
        }
      }
      else {
	if(all_files_received(receiver->file_uri_table)){
	  return -2;
	}
      }
      
      next_file = file->next;
    }
    
    i = 0;
    retcode = 0;
    toi = 0;
    
    if(rx_memory_mode == 0) {
      transfer_len = 0;
    }

    if(receiver->rx_automatic || receiver->wildcard_token != NULL) { 
    
      if(is_all_files_received) {
	
	if(receiver->fdt->complete) {
	  
	  if(any_files_received) {
	    if(receiver->verbosity > 0) {
	      printf("All files received\n");
	      fflush(stdout);
	    }
	  }
	  else {
	    if(receiver->verbosity > 0) {
	      printf("No wanted files in the session\n");
	      fflush(stdout);
	    }
	  }
	
	  return 1;
	}
	else {
	
	  if(((!is_printed) && (any_files_received))) {
	    
	    if(receiver->verbosity > 0) {
	      printf("All files received, waiting for new files\n");
	      fflush(stdout);
	    }
	    
	    is_printed = TRUE;
	  }
	  
#ifdef _MSC_VER
	  Sleep(1);
#else
	  usleep(1000);
#endif
	  continue;
	}
      }
    }

    if(rx_memory_mode == 1 || rx_memory_mode == 2) {

      tmp_file_name = alc_recv3(receiver->s_id, &toi, &retcode);
      
      if(tmp_file_name == NULL) {
		return retcode;
      }
      memset(tmp_filename, 0, MAX_PATH_LENGTH);
      memcpy(tmp_filename, tmp_file_name, strlen(tmp_file_name));
      free(tmp_file_name);

      next_file = receiver->fdt->file_list;
      
      /* Find correct file structure, to get the file->location for file creation purpose */
      
      while(next_file != NULL) {
		file = next_file;
	
		if(file->toi == toi) {
		file->status = 2;
		break;
		}
	
		next_file = file->next;
      }
    }
    else {
      
      buf = alc_recv2(receiver->s_id, &toi, &transfer_len, &retcode);
      
      if(buf == NULL) {
	return retcode;
      }
      
      next_file = receiver->fdt->file_list;

      /* Find correct file structure, to get the file->location for file creation purpose */

      while(next_file != NULL) {
	file = next_file;

	if(file->toi == toi) {
	  file->status = 2;
	  break;
	}

	next_file = file->next;
      }

      /* open tmp file and write buffer to it */
      
      memset(tmp_filename, 0, MAX_PATH_LENGTH);
      
      if(file->encoding == NULL) {
        sprintf(tmp_filename, "%s/%s", session_basedir, "object_XXXXXX");
        mktemp(tmp_filename);
      }
#ifdef USE_ZLIB
      else if(strcmp(file->encoding, "gzip") == 0) {
        sprintf(tmp_filename, "%s/%s", session_basedir, "object_XXXXXX");
        strcat(filename, GZ_SUFFIX);
        mktemp(tmp_filename);
      }
#endif
      else if(strcmp(file->encoding, "pad") == 0) {
        sprintf(tmp_filename, "%s/%s", session_basedir, "object_XXXXXX");
        strcat(filename, PAD_SUFFIX);
        mktemp(tmp_filename);
      }

#ifdef _MSC_VER
      if((fd = open((const char*)tmp_filename,
                    _O_WRONLY | _O_CREAT | _O_BINARY | _O_TRUNC , _S_IWRITE)) < 0) {
#else
      if((fd = open(tmp_filename, O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU)) < 0) {
#endif
        printf("Error: unable to open file %s\n", tmp_filename);
        fflush(stdout);
        free(buf);
        return MEM_ERROR;
      }

      if(write(fd, buf, (unsigned int)transfer_len) == -1) {
#ifdef _MSC_VER
        printf("write error, toi: %I64u\n", toi);
#else
        printf("write error, toi: %llu\n", toi);
#endif
        fflush(stdout);
        free(buf);
        close(fd);
        return MEM_ERROR;
      }

      free(buf);
      close(fd);
    }

	//Malek El Khatib 06.05.2014
	//Start
	//Processing time starts when all bytes are received but not yet used to generate segment and store it in the directory
	//Start of File Processing
	gettimeofday(&processing_time, NULL);
	timeInUsec = (unsigned long long)processing_time.tv_sec*1000000 + (unsigned long long)processing_time.tv_usec;
	fprintf(logFilePtr,"%s %llu ", file->location, timeInUsec);
	//END Malek El Khatib
    
    if(file->encoding == NULL) {
      content_enc_algo = 0;
    }
    else {
      if(strcmp(file->encoding, "pad") == 0) {
		content_enc_algo = PAD;
      }
#ifdef USE_ZLIB
      else if(strcmp(file->encoding, "gzip") == 0) {
		content_enc_algo = GZIP;
      }
#endif
    }

#ifdef USE_OPENSSL
    if(file->md5 != NULL) {
	
      md5 = file_md5(tmp_filename);
	
      if(md5 == NULL) {
	  
#ifdef _MSC_VER
	printf("MD5 check failed (TOI=%I64u)!\n", file->toi);
#else
	printf("MD5 check failed (TOI=%llu)!\n", file->toi);
#endif
	fflush(stdout);
	remove(tmp_filename);
	file->status = 1;
	  
	set_wanted_object(receiver->s_id, file->toi, file->transfer_len, file->es_len,
			  file->max_sb_len,
			  file->fec_inst_id, file->fec_enc_id,
			  file->max_nb_of_es, content_enc_algo, file->finite_field,
			  file->nb_of_es_per_group
			  );
	continue;
      }
      else{
	if(strcmp(md5, file->md5) != 0) {
#ifdef _MSC_VER
	  printf("MD5 check failed (TOI=%I64u)!\n", file->toi);
#else
	  printf("MD5 check failed (TOI=%llu)!\n", file->toi);
#endif
	  fflush(stdout);
	  remove(tmp_filename);
	  free(md5);
	  file->status = 1;
	  
	  set_wanted_object(receiver->s_id, file->toi, file->transfer_len, file->es_len,
			    file->max_sb_len,
			    file->fec_inst_id, file->fec_enc_id,
			    file->max_nb_of_es,
				content_enc_algo, file->finite_field, file->nb_of_es_per_group
			    );
	  continue;
	}
	
	free(md5);
      }
    }
#endif

    if(file->encoding != NULL) {
	
      if(strcmp(file->encoding, "pad") == 0) {
	  
	retcode = padding_decoder(tmp_filename, (int)file->content_len);
	  
	if(retcode == -1) {
	  free(tmp_filename);
	  return -1;
	}
	
	*(tmp_filename + (strlen(tmp_filename) - PAD_SUFFIX_LEN)) = '\0';
	  
	if(stat(tmp_filename, &file_stats) == -1) {
	  printf("Error: %s is not valid file name\n", tmp_filename);
	  fflush(stdout);
	  return -1;
	}
	
	if(file_stats.st_size != file->content_len) {
	  printf("stats: %i file: %i\n", (int)file_stats.st_size, (int)file->content_len);
	  printf("Error: padding decoding failed, file size not ok.\n");   
	  fflush(stdout);
	  remove(tmp_filename);
	  return -1;
	}
      }
#ifdef USE_ZLIB
      else if(strcmp(file->encoding, "gzip") == 0) {
	  
	retcode = file_gzip_uncompress(tmp_filename);
	  
	if(retcode == -1) {
	  return -1;
	}
	
	*(tmp_filename + (strlen(tmp_filename) - GZ_SUFFIX_LEN)) = '\0';
	
	if(stat(tmp_filename, &file_stats) == -1) {
	  printf("Error: %s is not valid file name\n", tmp_filename);
	  fflush(stdout);
	  return -1;
	}
	
	if(file_stats.st_size != file->content_len) {
	  printf("Error: uncompression failed, file-size not ok.\n");
	  fflush(stdout);
	  remove(tmp_filename);
	  return -1;
	}
      }
#endif
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
    
    if(ptr != NULL) {
      
      while(ptr != NULL) {
	
	i++;
	
	point = (int)(ptr - tmp);
	
	memset(filename, 0, MAX_PATH_LENGTH);
	memcpy((fullpath + strlen(fullpath)), "/", 1);
	memcpy((fullpath + strlen(fullpath)), tmp, point);
	
	memcpy(filename, (tmp + point + 1), (strlen(tmp) - (point + 1)));
	
#ifdef _MSC_VER
	if(_mkdir(fullpath) != 0) {					
#else		
	if(mkdir(fullpath, S_IRWXU) != 0) {
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
      memcpy((fullpath + strlen(fullpath)), "/", 1);
      memcpy((fullpath + strlen(fullpath)), filename, strlen(filename));
    }
    else{
      memcpy((fullpath + strlen(fullpath)), "/", 1);
      memcpy((fullpath + strlen(fullpath)), filepath, strlen(filepath));
    }

    if(rename(tmp_filename, fullpath) != 0) {

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
		printf("fullpath: %s\n", fullpath);
		printf("errno: %i\n", errno);
		fflush(stdout);
      }
    }
    
    if(receiver->verbosity > 0) {
#ifdef _MSC_VER
      printf("File received: %s (TOI=%I64u)\n", file->location, toi);
#else
      printf("File received: %s (TOI=%llu)\n", file->location, toi);
#endif
      fflush(stdout);
    }

    if(!(receiver->rx_automatic || receiver->wildcard_token != NULL)) {
      set_file_received(receiver->file_uri_table, file->location);
    }

    free_uri(uri);		
    free(tmp);
    free(filepath);
    
#ifdef _MSC_VER
    if(openfile) {
		ShellExecute(NULL, "Open", fullpath, NULL, NULL, SW_SHOWNORMAL);
    }
#endif      
    
	//Malek El Khatib 06.05.2014
	//START

	//S_IRWXU |S_IRWXG|S_IRWXO is used to set read, write, execute permissions for all users
	if (chmod(fullpath,S_IRWXU |S_IRWXG|S_IRWXO ) == 0 )

	//Processing time starts when all bytes are received but not yet used to generate segment and store it in the directory
	//End of File processing. Segment is now available for DASH Reference Client
	gettimeofday(&processing_time, NULL);
	timeInUsec = (unsigned long long)processing_time.tv_sec*1000000 + (unsigned long long)processing_time.tv_usec;
	fprintf(logFilePtr,"%llu\n", timeInUsec);  
	//END Malek El Khatib

#ifdef _MSC_VER
    Sleep(1);
#else
    usleep(1000);
#endif
  }
  return 1;
}

int receiver_in_fdt_based_mode(arguments_t *a, flute_receiver_t *receiver) {

  int retval = 0;
  int retcode = 0;
  char *cont_desc = NULL;
 
  if(strcmp(a->sdp_file, "") != 0) {
    cont_desc = sdp_attr_get(a->sdp, "content-desc");
  }

  if(a->rx_automatic) {
    if(a->alc_a.verbosity > 0) {
      printf("FLUTE Receiver in automatic mode\n");
    }
  }
  else if(strchr(a->file_path, '*') != NULL) {
    if(a->alc_a.verbosity > 0) {
      printf("FLUTE Receiver in wild card mode\n");
    }
  }
  else {
    if(a->alc_a.verbosity > 0) {
      printf("FLUTE Receiver in fileURI list mode\n");
    }
  }
  
  if(cont_desc != NULL) {
    if(a->alc_a.verbosity > 0) {
      printf("Session content information available at:\n");
      printf("%s\n", cont_desc);
    }
  }
  
  fflush(stdout);
  //Malek El Khatib.....JUST A COMMENT: THIS NEEDS TO BE MODIFIED TO ALLOW RECEPTION BEFORE FDT
  while(receiver->fdt == NULL) {

    if(get_session_state(receiver->s_id) == SExiting) {
      return -2;
    }
    else if(get_session_state(receiver->s_id) == STxStopped) {
      return -3;
    }

#ifdef _MSC_VER
    Sleep(1);
#else
    usleep(1000);
#endif
    continue;
  }
  
  retcode = fdtbasedrecv(a->alc_a.rx_memory_mode,
#ifdef _MSC_VER
			 a->open_file,
#else 
			 0,
#endif
			 receiver);
  
  if(retcode == -1) {
    printf("Error: fdtbasedrecv() failed\n");
    fflush(stdout);
    retval = -1;
  }
  else if(retcode == -2) {
    retval = -2;
  }
  else if(retcode == -3) {
    retval = -3;
  }

  return retval;
}

int receiver_in_ui_mode(arguments_t *a, flute_receiver_t *receiver) {
	
	file_t *file;
	char input[100];

	unsigned long long rec_toi;

	char command;
	BOOL valid_toi = FALSE;
	BOOL expired_toi = FALSE;
	char *filepath = NULL;
	uri_t *uri = NULL;

	int retval = 0;
	int retcode = 0;

	time_t systime;
	unsigned long long curr_time;

	int content_enc_algo = -1;

	char *cont_desc = NULL;

	if(strcmp(a->sdp_file, "") != 0) {
	  cont_desc = sdp_attr_get(a->sdp, "content-desc");
	}

	printf("FLUTE Receiver in UI-mode\n");

	if(cont_desc != NULL) {
	  printf("Session content information available at:\n");
	  printf("%s\n", cont_desc);
	}
	
	fflush(stdout);

	while(receiver->fdt == NULL) {

		if(get_session_state(receiver->s_id) == SExiting) {
			return -2;
		}
		else if(get_session_state(receiver->s_id) == STxStopped) {
			return -3;
		}

#ifdef _MSC_VER
		Sleep(1);
#else
		usleep(1000);
#endif
		continue;
	}

	while(1) {

		if(get_session_state(receiver->s_id) == SExiting) {			
			return -2;
		}
		else if(get_session_state(receiver->s_id) == STxStopped) {
			return -3;
		}
	
		printf(">> Command: (D)ownload file (Q)uit\n");
		printf("-> ");

		if(fgets(input, 100, stdin) != NULL) {

			if(sscanf(input, "%c", &command) != EOF) {

				command = tolower(command);
							
				if(command == 'd') {
					printf("Files to download:\n");
					fflush(stdout);
					
					file = receiver->fdt->file_list;

					while(file != NULL) {
#ifdef _MSC_VER
						printf("\t%I64u) %s\n", file->toi, file->location);
#else
						printf("\t%llu) %s\n", file->toi, file->location);
#endif
						fflush(stdout);
						file = file->next;
					}

					printf("\nWhich one to download: ");
					fflush(stdout);

					if(fgets(input, 100, stdin) != NULL) {
						
#ifdef _MSC_VER
						retcode = sscanf(input, "%I64u", &rec_toi);
#else
						retcode = sscanf(input, "%llu", &rec_toi);
#endif
						if(!((retcode == 0) || (retcode == EOF))) {
				
							file = receiver->fdt->file_list;

							valid_toi = FALSE;
							expired_toi = FALSE;
			
							while(file != NULL) {

								if(file->toi == rec_toi) {

									time(&systime);
									curr_time = systime + 2208988800U;

									if(file->expires < curr_time) {
													
										if(!a->alc_a.accept_expired_fdt_inst) {

											expired_toi = TRUE;
#ifdef _MSC_VER
											printf("\nToi: %I64u Expired.\n", rec_toi);        
#else
											printf("\nToi: %llu Expired.\n", rec_toi);  
#endif
											if(file->next != NULL) {
												file->next->prev = file->prev;
											}
											if(file->prev != NULL) {
												file->prev->next = file->next;
											}
											if(file == receiver->fdt->file_list) {
												receiver->fdt->file_list = file->next;
											}

											free_file(file);
											break;
										}
										else {
#ifdef _MSC_VER
											printf("\nToi: %I64u Expired, receiving anyway.\n", rec_toi);        
#else
											printf("\nToi: %llu Expired, receiving anyway.\n", rec_toi);  
#endif											
										}
									}

									valid_toi = TRUE;

									uri = parse_uri(file->location, strlen(file->location));
	
									filepath = get_uri_host_and_path(uri);
																						
									if(file->encoding == NULL) {
										content_enc_algo = 0;	
									}
									else {
                                                                                if(strcmp(file->encoding, "pad") == 0) {
                                                                                        content_enc_algo = PAD;
                                                                                }
#ifdef USE_ZLIB
										else if(strcmp(file->encoding, "gzip") == 0) {
											content_enc_algo = GZIP;	
										}
#endif
										else {
											content_enc_algo = -1;
											break;
										}
									}

									retval = set_wanted_object(receiver->s_id, file->toi,
												   file->transfer_len,
												   file->es_len,
												   file->max_sb_len,
												   file->fec_inst_id,
												   file->fec_enc_id,
												   file->max_nb_of_es,
												   content_enc_algo,
												   file->finite_field,
												   file->nb_of_es_per_group
												   );
									break;
								}
								file = file->next;
							}

							if(expired_toi) {
								free_uri(uri);
								free(filepath);
								continue;
                            }

							if(!valid_toi) {
#ifdef _MSC_VER
								printf("\nToi: %I64u invalid.\n", rec_toi);        
#else
								printf("\nToi: %llu invalid.\n", rec_toi);  
#endif       
								fflush(stdout);
								continue;
                            }

							if(content_enc_algo == -1) {
								printf("Content-Encoding: %s not supported\n", file->encoding);
								fflush(stdout);
								continue;
							}
							retcode = recvfile(receiver->s_id, filepath, file->toi, file->content_len, file->md5,
									   a->alc_a.rx_memory_mode, file->encoding, a->alc_a.verbosity);
													
							if(retcode == -1) {
								free(filepath);
								free_uri(uri);
								printf("\nError: recvfile() failed\n");
								fflush(stdout);
								return -1;
							}
							else if(retcode == -2) {
								free(filepath);
								free_uri(uri);
								return -2;
							}
							else if(retcode == -3) {
								free(filepath);
								free_uri(uri);
								return -3;
							}

#ifdef _MSC_VER
							if(((retcode != -4) && (a->open_file))) {
								ShellExecute(NULL, "Open", filepath, NULL, NULL, SW_SHOWNORMAL); 
							}
#endif
							free(filepath);
						}
					}
				}
				else if(command == 'q') {
					break;
				}
				else {
					continue;
				}
			}
		}
	}

	return 0;
}

int receiver_in_object_mode(int *s_id, arguments_t *a) {

	int retval = 0;
	int retcode;
	div_t div_max_n;
	unsigned short max_n;

	char *cont_desc = NULL;
	
	div_max_n = div((a->alc_a.max_sb_len * (100 + a->alc_a.fec_ratio)), 100);
	max_n = (unsigned short)div_max_n.quot;

	if(strcmp(a->sdp_file, "") != 0) {
		cont_desc = sdp_attr_get(a->sdp, "content-desc");
	}

	if(a->alc_a.verbosity > 0) {
	  printf("FLUTE Receiver in object mode\n");
	}

	if(cont_desc != NULL) {
	  if(a->alc_a.verbosity > 0) {
	    printf("Session content information available at:\n");
	    printf("%s\n", cont_desc);
	  }
	}

	fflush(stdout);

	set_wanted_object(*s_id, a->toi, 0, a->alc_a.es_len, a->alc_a.max_sb_len,
			  a->alc_a.fec_inst_id, a->alc_a.fec_enc_id, max_n, 0, 0, 0
			  );

	retcode = recvfile(*s_id, NULL, a->toi, 0, NULL, a->alc_a.rx_memory_mode, NULL, a->alc_a.verbosity);	

	if(retcode == -1) {
		printf("\nError: recvfile() failed\n");
		fflush(stdout);
		retval = -1;
	}
	else if(retcode == -2) {
		retval =  -2;
	}
	else if(retcode == -3) {
		retval =  -3;
	}
	else {
#ifdef _MSC_VER
		if(a->open_file) {
			ShellExecute(NULL, "Open", a->file_path, NULL, NULL, SW_SHOWNORMAL); 
		}
#endif
	}

	return retval;
}

void* fdt_thread(void *s) {
  
  int i;
  flute_receiver_t *receiver;
  char *buf = NULL;
  
  unsigned long long buflen = 0;
  
  int updated;
  
  fdt_t *fdt_instance;
  efdt_t *efdt_instance;
  time_t systime;
  file_t *file;
  file_t *next_file;
  int retval;
  FILE *fabcd;
  
  unsigned long long curr_time;
  
#ifdef USE_ZLIB
  char *uncompr_buf = NULL;
  
  unsigned long long uncompr_buflen = 0;
  
#endif
  unsigned char content_enc_algo = 0;

  unsigned char fdt_content_enc_algo = 0;
  int fdt_instance_id = 0;
  
  receiver = (flute_receiver_t*)s;
  
  while(get_session_state(receiver->s_id) == SActive) {
    
    time(&systime);
    curr_time = systime + 2208988800U;

    /* Get initial fdt */
    if(receiver->efdt == NULL) {
 
      buf = fdt_recv(receiver->s_id, &buflen, &retval, &fdt_content_enc_algo, &fdt_instance_id);
       
 
		
      if(buf == NULL) {

	
		
	    if(retval == -1) {
	      continue;
	    }

#ifdef _MSC_VER
	    _endthread();
#else
	    pthread_exit(0);
#endif
      }

#ifdef USE_ZLIB
      if(fdt_content_enc_algo == ZLIB) {
	    uncompr_buf = buffer_zlib_uncompress(buf, buflen, &uncompr_buflen);
	
	    if(uncompr_buf == NULL) {
	      free(buf);
	      continue;
	    }
	    efdt_instance = decode_efdt_payload(uncompr_buf);
	      free(uncompr_buf);
        }
        else {
	     efdt_instance = decode_efdt_payload(buf);
	      
        }
#else 
      efdt_instance = decode_efdt_payload(buf);
       
#endif
  
      
      if(efdt_instance == NULL) {
		
	free(buf);
	continue;
      }
      
      if(efdt_instance->expires < curr_time) {
	
	if(!receiver->accept_expired_fdt_inst) {
	  
	  if(receiver->verbosity == 4) {
	    printf("Expired EFDT Instance received, discarding\n");
	    fflush(stdout);
	  }
	  
	  free(buf);
	  FreeFDT(efdt_instance);
	  continue;
	}
	else {
	  if(receiver->verbosity == 4) {
	    printf("Expired EFDT Instance received, using it anyway\n");
	    fflush(stdout);
	  }
	}
      }
      
 /*Copy efdt to fdt accordingly */
  fdt_instance= calloc(1, sizeof(fdt_t));
  fdt_instance->expires=              efdt_instance->expires;
  fdt_instance->file_list=            efdt_instance->file_list;
  fdt_instance->nb_of_files=          efdt_instance->nb_of_files;
  fdt_instance->type=                 efdt_instance->type;
  fdt_instance->encoding=             efdt_instance->encoding;
  fdt_instance->fec_enc_id=           efdt_instance->fec_enc_id;
  fdt_instance->fec_inst_id=          efdt_instance->fec_inst_id;
  fdt_instance->finite_field=         efdt_instance->finite_field;
  fdt_instance->nb_of_es_per_group=   efdt_instance->nb_of_es_per_group;
  fdt_instance->max_sb_len=           efdt_instance->max_sb_len;
  fdt_instance->es_len=               efdt_instance->es_len;
  fdt_instance->max_nb_of_es=         efdt_instance->max_nb_of_es;
  //fdt_instance->complete =            efdt_instance->complete;;
  
      receiver->fdt = fdt_instance;
      receiver->efdt = efdt_instance;
      
      if(receiver->verbosity == 4) {
	printf("FDT Instance received (ID=%i)\n", fdt_instance_id);
	printf("FDT updated, new file description(s) added\n");
	fflush(stdout);
	PrintFDT(fdt_instance, receiver->s_id);
      }
      
      free(buf);
      
      next_file = receiver->fdt->file_list;
      
      while(next_file != NULL) {
	i = 0;
	file = next_file;
	
	if(file->status == 0) {
	  
	  if(file->encoding == NULL) {
	    content_enc_algo = 0;
	  }
	  else {
	    if(strcmp(file->encoding, "pad") == 0) {
	      content_enc_algo = PAD;	
	    }
#ifdef USE_ZLIB
	    else if(strcmp(file->encoding, "gzip") == 0) {
	      content_enc_algo = GZIP;
	    }
#endif
	    else {
	      printf("Content-Encoding: %s not supported\n", file->encoding);
	      fflush(stdout);
	      file->status = 2;
	      next_file = file->next;
	      continue;
	    }
	  }
	  
	  if(receiver->rx_automatic) {
	    
	    retval = set_wanted_object(receiver->s_id, file->toi, file->transfer_len,
				       file->es_len,
				       file->max_sb_len,
				       file->fec_inst_id,
				       file->fec_enc_id,
				       file->max_nb_of_es, content_enc_algo,
					   file->finite_field, file->nb_of_es_per_group
				       );
	    
	    if(retval < 0) {
	      /* Memory error */
	    }
	    else {
	      file->status = 1;
	    }
	  }
	  else if(receiver->wildcard_token != NULL) { 
	    if(strstr(file->location, receiver->wildcard_token) != NULL) {
	      retval = set_wanted_object(receiver->s_id, file->toi, file->transfer_len,
					 file->es_len,
					 file->max_sb_len,
					 file->fec_inst_id,
					 file->fec_enc_id,
					 file->max_nb_of_es, content_enc_algo,
					 file->finite_field, file->nb_of_es_per_group
					 );
	      
	      if(retval < 0) {
		/* Memory error */
	      }
	      else {
		file->status = 1;
	      }
		}
	  }
	  else {

	    for(i = 0; i < FILE_URI_TABLE_SIZE; i++) {
	      
	      if(receiver->file_uri_table[i] == NULL) {
		continue;
	      }
	      
	      if(strcmp(file->location, receiver->file_uri_table[i]) == 0) {
		
		retval = set_wanted_object(receiver->s_id, file->toi, file->transfer_len,
					   file->es_len,
					   file->max_sb_len,
					   file->fec_inst_id,
					   file->fec_enc_id,
					   file->max_nb_of_es, content_enc_algo,
					   file->finite_field, file->nb_of_es_per_group
					   );
		
		if(retval < 0) {
		  /* Memory error */
		}
		else {
		  file->status = 1;
		  break;
		}
	      }
	    }
	  }
	}
	
	next_file = file->next;
	  }

	set_fdt_instance_parsed(receiver->s_id);
	}
    else { /* Receive new FDT Instance when it comes */
      updated = 0;

      buf = fdt_recv(receiver->s_id, &buflen, &retval, &fdt_content_enc_algo, &fdt_instance_id);
   	
      if(buf == NULL) {
	
	if(retval == -1) {
	  continue;
	}
	
#ifdef _MSC_VER
	_endthread();
#else
	pthread_exit(0);
#endif
      }

#ifdef USE_ZLIB
      if(fdt_content_enc_algo == ZLIB) {
	uncompr_buf = buffer_zlib_uncompress(buf, buflen, &uncompr_buflen);
	
	if(uncompr_buf == NULL) {
	  free(buf);
	  continue;
	}
	efdt_instance = decode_efdt_payload(uncompr_buf);
	free(uncompr_buf);
      }
      else {
	efdt_instance = decode_efdt_payload(buf);
	 
      }
#else 
      efdt_instance = decode_efdt_payload(buf);
#endif
       
      if(efdt_instance == NULL) {
	free(buf);
	continue;
      }
      
      if(efdt_instance->expires < curr_time) {
	if(!receiver->accept_expired_fdt_inst) {
	  if(receiver->verbosity == 4) {
	    printf("Expired FDT Instance received, discarding\n");
	    fflush(stdout);
	  }
	  FreeFDT(efdt_instance);
	  free(buf);
	  continue;
	}
	else {
	  if(receiver->verbosity == 4) {
	    printf("Expired FDT Instance received, using it anyway\n");
	    fflush(stdout);
	  }
	}
      }
      
      if(receiver->verbosity == 4) {
	printf("FDT Instance received (ID=%i)\n", fdt_instance_id);
	fflush(stdout);
      }

      free(buf);
      
      if((efdt_instance->complete == TRUE)&&(receiver->efdt->complete == FALSE)&&(fdt_instance_id == 0)) {
	receiver->fdt->complete = TRUE;
      }
      
      //updated = update_efdt(receiver->efdt, efdt_instance);
      /*Copy efdt to fdt accordingly*/
  fdt_instance= calloc(1, sizeof(fdt_t));
  fdt_instance->expires=              efdt_instance->expires;
  fdt_instance->file_list=            efdt_instance->file_list;
  fdt_instance->nb_of_files=          efdt_instance->nb_of_files;
  fdt_instance->type=                 efdt_instance->type;
  fdt_instance->encoding=             efdt_instance->encoding;
  fdt_instance->fec_enc_id=           efdt_instance->fec_enc_id;
  fdt_instance->fec_inst_id=          efdt_instance->fec_inst_id;
  fdt_instance->finite_field=         efdt_instance->finite_field;
  fdt_instance->nb_of_es_per_group=   efdt_instance->nb_of_es_per_group;
  fdt_instance->max_sb_len=           efdt_instance->max_sb_len;
  fdt_instance->es_len=               efdt_instance->es_len;
  fdt_instance->max_nb_of_es=         efdt_instance->max_nb_of_es;
  //fdt_instance->complete =            efdt_instance->complete;
  updated = update_fdt(receiver->fdt, fdt_instance);
      //receiver->fdt = fdt_instance;
      //receiver->efdt= efdt_instance;
      FILE *fabcd;
   fabcd=fopen("ErrorDebugging.txt", "w");
			 
		
			//  fprintf(fabcd, "%llu\n", tmp->toi);
			  fprintf(fabcd, fdt_instance->file_list);
			  fclose(fabcd);
      if(updated < 0) {
	continue;
      }
      else if(updated == 1) {
	if(receiver->verbosity == 4) {
	  printf("FDT updated, file description(s) complemented\n");
	  fflush(stdout);
	}
      }
      else if(updated == 2) {
	if(receiver->verbosity == 4) {
	  printf("FDT updated, new file description(s) added\n");
	  fflush(stdout);
	  PrintFDT(fdt_instance, receiver->s_id);
	}

	next_file = receiver->fdt->file_list;
	
	while(next_file != NULL) {
	  file = next_file;
	  
	  if(file->status == 0) {
	    
	    if(file->encoding == NULL) {
	      content_enc_algo = 0;	
	    }
	    else {
	      if(strcmp(file->encoding, "pad") == 0) {
		content_enc_algo = PAD;	
	      }
#ifdef USE_ZLIB
	      else if(strcmp(file->encoding, "gzip") == 0) { 
		content_enc_algo = GZIP; 
	      }
#endif
	      else {
		printf("Content-Encoding: %s not supported\n", file->encoding);
		fflush(stdout);
		file->status = 1;
	      }
	    }

	    if(receiver->rx_automatic) {
	      
	      retval = set_wanted_object(receiver->s_id, file->toi, file->transfer_len,
					 file->es_len,
					 file->max_sb_len,
					 file->fec_inst_id,
					 file->fec_enc_id,
					 file->max_nb_of_es, content_enc_algo,
					 file->finite_field, file->nb_of_es_per_group
					 );
	      
	      if(retval < 0) {
		/* Memory error */
	      }
	      else {
		file->status = 1;
	      }
	    }
	    else if(receiver->wildcard_token != NULL) {
	      if(strstr(file->location, receiver->wildcard_token) != NULL) {
		retval = set_wanted_object(receiver->s_id, file->toi, file->transfer_len,
					   file->es_len,
					   file->max_sb_len,
					   file->fec_inst_id,
					   file->fec_enc_id,
					   file->max_nb_of_es, content_enc_algo,
					   file->finite_field, file->nb_of_es_per_group
					   );

		if(retval < 0) {
		  /* Memory error */
		}
		else {
		  file->status = 1;
		}
	      }
	    }
	    else {
	      
	      for(i = 0; i < FILE_URI_TABLE_SIZE; i++) {
		
		if(receiver->file_uri_table[i] == NULL) {
		  continue;
		}

		if(strcmp(file->location, receiver->file_uri_table[i]) == 0) {

		  retval = set_wanted_object(receiver->s_id, file->toi, file->transfer_len,
					     file->es_len,
					     file->max_sb_len,
					     file->fec_inst_id,
					     file->fec_enc_id,
					     file->max_nb_of_es, content_enc_algo,
						 file->finite_field, file->nb_of_es_per_group
					     );
		  
		  if(retval < 0) {
		    /* Memory error */
		  }
		  else {
		    file->status = 1;
		    break;
		  }
		}
	      } 
	    }
	  }	
	  next_file = file->next;
	}
	  }

      FreeFDT(fdt_instance);

	  set_fdt_instance_parsed(receiver->s_id);
	}
  }

#ifdef _MSC_VER
  _endthread();
#else
  pthread_exit(0);
#endif

	return NULL;
}
