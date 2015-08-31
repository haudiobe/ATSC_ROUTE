/** \file padding_encoding.c \brief Content padding
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.12 $
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
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

#include "padding_encoding.h"

int compute_padding_length(unsigned long long f_size, int block_len, int es_len) {
  int p_length;

  if(f_size <= (block_len * es_len)) {
	  p_length = 0;
  }
  else if(!(f_size % (block_len * es_len))) {
	  p_length = 0;
  }
  else {
	  p_length = (int)((block_len * es_len) - f_size % (block_len * es_len));
  }

  return p_length;
}

int padding_decoder(char *file_name, unsigned long long content_length) {

  int fp_in;
  unsigned long long f_size;

#ifdef _MSC_VER
  struct __stat64 file_stats;
#else  
  struct stat64 file_stats;
#endif

  char file_name_in[256] = "";
  char file_name_out[256] = "";
  char *ptr;
  int retval;

  strcpy(file_name_in, file_name);  
  ptr = strstr(file_name_in, PAD_SUFFIX);

  memcpy(file_name_out, file_name_in, (ptr - file_name_in));

#ifdef _MSC_VER
  fp_in = open((const char*)file_name_in, _O_RDWR | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE);
#else
  fp_in = open64(file_name_in, O_RDWR | O_CREAT, S_IRWXU);
#endif

  if(fp_in == -1) {
    printf("open error: %s\n", file_name_in);
    return -1;
  }

#ifdef _MSC_VER
  _fstat64(fp_in, &file_stats);
#else
  fstat64(fp_in, &file_stats);
#endif

  f_size = file_stats.st_size;

  if(f_size > content_length) {
#ifdef _MSC_VER
    retval = _chsize(fp_in, (long)content_length); /* TODO: 64 bits, how ??? */
#else
	  retval = ftruncate64(fp_in, content_length);
#endif

	  if(retval != 0) {
		  printf("Problem in padding decoding.\n" );
		  close(fp_in);
		  return -1;
	  }
  }

  close(fp_in);
	 
	if(rename(file_name_in, file_name_out) < 0) {

		if(errno == EEXIST) {
			retval = remove(file_name_out);
		
			if(retval == -1) {    
				printf("errno: %i\n", errno);
				fflush(stdout);
				close(fp_in);
				return -1;
			}
		
			if(rename(file_name_in, file_name_out) < 0) {
				printf("rename() error1: %s\n", file_name_in);
				fflush(stdout);
				close(fp_in);
				return -1;
			}
		}
		else {
			printf("rename() error2: %s\n", file_name_in);
			fflush(stdout);
			close(fp_in);
			return -1;
		}
	}

  return 0;
}

