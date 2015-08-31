/** \file mad_zlib.c \brief ZLIB compression
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.13 $
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
#include <math.h>

#include <zlib.h>

#include "mad_zlib.h"

#ifdef USE_ZLIB

/**
 * This is a private function which compresses the input file to the output file and then closes both files.
 *
 * @param in input file
 * @param out output file
 *
 * @return 0 in success, -1 otherwise
 *
 */

int gz_compress(FILE *in, gzFile out) {

    char buf[ZLIB_BUFLEN];
    int len;
    int err;

    for(;;) {
		memset(buf, 0, ZLIB_BUFLEN);
        len = (int)fread(buf, 1, sizeof(buf), in);
        
		if(ferror(in)) {
           	printf("Error: fread failed in gz_compress\n");
			fflush(stdout);
            return -1;
        }

        if(len == 0) {
			break;
		}

        if(gzwrite(out, buf, (unsigned int)len) != len) {
			printf("Error: %s\n", gzerror(out, &err));
			fflush(stdout);
			return -1;
		}
    }

    if(gzclose(out) != Z_OK) {
		printf("Error: gzclose failed in gz_compress\n");
		fflush(stdout);
		return -1;
	} 
	
	if(fclose(in)) {
		printf("Error: fclose failed in gz_compress\n");
		fflush(stdout);
		return -1;
	}

	return 0;
}

/**
 * This is a private function which uncompresses the input file to the output file and then closes both files.
 *
 * @param in input file
 * @param out output file
 *
 * @return 0 in success, -1 otherwise
 *
 */

int gz_uncompress(gzFile in, FILE *out) {

    char buf[ZLIB_BUFLEN];
    int len;
    int err;

    for(;;) {
        len = gzread(in, buf, sizeof(buf));

        if(len < 0) {
			printf("Error: %s\n", gzerror(out, &err));
			fflush(stdout);
			return -1;
		}
        
		if(len == 0) {
			break;
		}

        if((int)fwrite(buf, 1, (unsigned int)len, out) != len) {
            printf("Error: fwrite failed in gz_compress\n");
			fflush(stdout);
            return -1;
        }
    }

    if(fclose(out)) {
		printf("Error: fclose failed in gz_compress\n");
		fflush(stdout);
		return -1;
	}

    if(gzclose(in) != Z_OK) {
		printf("Error: gzclose failed in gz_compress\n");
		fflush(stdout);
		return -1;
	}

	return 0;
}

int file_gzip_compress(char *file, char *mode) {
    
	char outfile[MAX_PATH_LENGTH];
    FILE  *in;
    gzFile out;
	int retval = 0;

	memset(outfile, 0, MAX_PATH_LENGTH);

    strcpy(outfile, file);
    strcat(outfile, GZ_SUFFIX);

    if((in = fopen(file, "rb")) == NULL) {
       	printf("Error: unable to fopen inputfile: %s\n", file);
		fflush(stdout);
		return -1;
    }

    if((out = gzopen(outfile, mode)) == NULL) {
		printf("Error: unable to gzopen outputfile: %s\n", outfile);
		fflush(stdout);
		return -1;
    }

    retval = gz_compress(in, out);

	return retval;
}

int file_gzip_uncompress(char* file) {

    char buf[MAX_PATH_LENGTH];
    char *infile;
    char *outfile;

    FILE  *out;
    gzFile in;
    uInt len;

    int retval = 0;

    len = (uInt)strlen(file);

    memset(buf, 0, MAX_PATH_LENGTH);
    strcpy(buf, file);

    infile = file;
    outfile = buf;
    outfile[len - (GZ_SUFFIX_LEN)] = '\0';

    if((in = gzopen(infile, "rb")) == NULL) {
        printf("Error: unable to gzopen inputfile: %s\n", infile);
		fflush(stdout);
		return -1;
    }
   
    if((out = fopen(outfile, "wb")) == NULL) {
       	printf("Error: unable to fopen outputfile: %s\n", outfile);
		fflush(stdout);
		return -1;
    }

    retval = gz_uncompress(in, out);

	if(retval == -1) {
		printf("infile: %s, outfile: %s\n", infile, outfile);
	}

    remove(infile);

	return retval;
}

char* buffer_zlib_compress(char *buf, 
							unsigned long long buflen, 
							unsigned long long *comprlen
						   ) {
    
	int err;

	Byte *compr;
	uLong comprLen;

    comprLen =  (uLong)ceil((double)1.001*(double)(unsigned int)buflen) + 12;

	if((compr = (Byte*)calloc((uInt)comprLen, 1)) == Z_NULL) {
        printf("out of memory\n");
		fflush(stdout);
        return NULL;
    }

	if((err = compress(compr, &comprLen, (const Bytef*)buf, (unsigned int)buflen)) != Z_OK) {
        printf("Error: %d in buffer_compress\n", err);
		return NULL;
    }

	*comprlen = comprLen;

	return (char*)compr;
}

char* buffer_zlib_uncompress(char *buf,
								unsigned long long buflen, 
								unsigned long long *uncomprlen
							 ) {
    
	int err;

	Byte *uncompr = NULL;
	uLong uncomprLen = 5 * (unsigned int)buflen; /* TODO: size from where, now first allocate
												 5*compressed buffer and then reallocated double
												 amount if first size is not enough. */

	if((uncompr = (Byte*)calloc((uInt)uncomprLen, 1)) == Z_NULL) {
        printf("out of memory\n");
		fflush(stdout);
        return NULL;
    }

	err = uncompress(uncompr, &uncomprLen, (const Bytef*)buf, (unsigned long)buflen);

	if(err != Z_OK) {
		
		if(err == Z_BUF_ERROR) {
			free(uncompr);
			
			uncomprLen = 2 * uncomprLen;

			if((uncompr = (Byte*)calloc((uInt)uncomprLen, 1)) == Z_NULL) {
				printf("out of memory\n");
				fflush(stdout);
				return NULL;
			}
			
			err = uncompress(uncompr, &uncomprLen, (const Bytef*)buf, (unsigned long)buflen);
			
			if(err != Z_OK) {
				printf("Error: %d in buffer_uncompress\n", err);
				return NULL;
			}
		}
		else {
			printf("Error: %d in buffer_uncompress\n", err);
			return NULL;	
		}
	}

	*uncomprlen = uncomprLen;

	return (char*)uncompr;
}

char* buffer_gzip_uncompress(char *buf,
								unsigned long long buflen, 
								unsigned long long uncomprlen) {

	Byte *uncompr;
	int err;
    z_stream d_stream; /* decompression stream */

    d_stream.zalloc = Z_NULL;
    d_stream.zfree = Z_NULL;
    d_stream.opaque = Z_NULL;

    d_stream.next_in  = (Bytef*)buf;
    d_stream.avail_in = (uInt)buflen;

	if((uncompr = (Byte*)calloc(((uInt)buflen + (uInt)1), 1)) == Z_NULL) {
        printf("out of memory\n");
		fflush(stdout);
        return NULL;
    }
    
	if((err = inflateInit2(&d_stream, -MAX_WBITS)) != Z_OK) {
	  printf("Error: %d in inflateInit2\n", err);
      return NULL;
	}

    /*for (;;) {
        d_stream.next_out = uncompr;
        d_stream.avail_out = (uInt)buflen;
        err = inflate(&d_stream, Z_NO_FLUSH);

        if(err == Z_STREAM_END) {
			break;
		}

        if(err == Z_STREAM_ERROR) {
			printf("internal stream error!\n");
			return NULL;
		}
        if(err == Z_MEM_ERROR) {
			printf("out of memory\n");
			return NULL;
		}
        if(err == Z_DATA_ERROR) {
            printf("invalid compressed data\n");
			return NULL;
		}
    }

    if((err = inflateEnd(&d_stream)) != Z_OK) {
	  printf("Error: %d in inflateEnd\n", err);
      return NULL;
	}

    if(d_stream.total_out != (uLong)(2*uncomprlen + buflen/2)) {
		printf("uncompress error!\n");
		fflush(stdout);
        return NULL;;
    }*/

    while(d_stream.total_out < (uLong)uncomprlen && d_stream.total_in < (uLong)buflen) {
        d_stream.avail_in = d_stream.avail_out = 1; 
        err = inflate(&d_stream, Z_BLOCK);/*Z_NO_FLUSH);*/
        
		if(err == Z_STREAM_END) {
			break;
		}

        if(err == Z_STREAM_ERROR) {
			printf("internal stream error!\n");
			return NULL;
		}
        if(err == Z_MEM_ERROR) {
			printf("out of memory\n");
			return NULL;
		}
        if(err == Z_DATA_ERROR) {
            printf("invalid compressed data\n");
			return NULL;
		}
    }

    if((err = inflateEnd(&d_stream)) != Z_OK) {
	  printf("Error: %d in inflateEnd\n", err);
      return NULL;
	}

	return (char*)uncompr;
}

#endif

