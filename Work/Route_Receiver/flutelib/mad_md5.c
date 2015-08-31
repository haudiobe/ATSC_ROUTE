/** \file mad_md5.c \brief MD5 check
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.16 $
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

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "mad_md5.h"

#ifdef USE_OPENSSL

#include <openssl/md5.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

char* file_md5(const char *filename) {

#ifdef _MSC_VER
	struct __stat64 file_stats;
#else
    struct stat64 file_stats;
#endif

	int nbytes;
	FILE *fp;
	char *md5 = NULL;

	char b64_digest[MD5_DIGEST_LENGTH*2];
	BIO *bio, *b64, *mem;

	unsigned char md5_digest[MD5_DIGEST_LENGTH];
	MD5_CTX ctx;				
	int i;
	char zBuf[10240]; /*10240*/

	memset(md5_digest, 0, MD5_DIGEST_LENGTH);
	memset(b64_digest, 0, MD5_DIGEST_LENGTH*2);

	MD5_Init(&ctx);

#ifdef _MSC_VER
	fp = fopen(filename, "rb");
#else
	fp = fopen64(filename, "rb");
#endif

	if(fp == NULL) {
	  printf("mad_md5.c: fopen error\n");
	  fflush(stdout);
	  return NULL;
	}

#ifdef _MSC_VER
	if(_stat64(filename, &file_stats) == -1) {
#else
    if(stat64(filename, &file_stats) == -1) {
#endif
		printf("Error: %s is not valid file name\n", filename);
		fflush(stdout);
        return NULL;
	}

	for(;;) {
		nbytes = fread(zBuf, 1, sizeof(zBuf), fp);
	
		if(nbytes <= 0) {
			break;
		}
		
		MD5_Update(&ctx, (unsigned char*)zBuf, (unsigned)nbytes);
    }

	MD5_Final(md5_digest, &ctx); 

	b64 = BIO_new(BIO_f_base64());
	mem = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, mem);
	BIO_write(bio, md5_digest, MD5_DIGEST_LENGTH);
	BIO_flush(bio);
	BIO_gets(mem, b64_digest, MD5_DIGEST_LENGTH*2);
	BIO_free_all(bio);

	for(i = 0; i < MD5_DIGEST_LENGTH*2; i++) {
		if(b64_digest[i] <= ' ') {
			b64_digest[i] = '\0';
        }
	}

	if(!(md5 = (char*)calloc((strlen(b64_digest) + 1), sizeof(char)))) {
		printf("Could not alloc memory for md5 buffer!\n");
		fflush(stdout);
        return NULL;
	} 

	memcpy(md5, b64_digest, strlen(b64_digest));
	
	if(fclose(fp) != 0) {
		printf("fclose failed, errno: %i\n", errno);
	}

	return md5;
}

char* buffer_md5(char *buffer, unsigned long long length) {
	
	char b64_digest[MD5_DIGEST_LENGTH*2];
	BIO *bio, *b64, *mem;

	unsigned char md5_digest[MD5_DIGEST_LENGTH];
	MD5_CTX ctx;				
	int i;
	char *md5 = NULL;

	memset(md5_digest, 0, MD5_DIGEST_LENGTH);
	memset(b64_digest, 0, MD5_DIGEST_LENGTH*2);

	MD5_Init(&ctx);

	MD5_Update(&ctx, buffer, (unsigned int)length);
	MD5_Final(md5_digest, &ctx); 

	b64 = BIO_new(BIO_f_base64());
	mem = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, mem);
	BIO_write(bio, md5_digest, MD5_DIGEST_LENGTH);
	BIO_flush(bio);
	BIO_gets(mem, b64_digest, MD5_DIGEST_LENGTH*2);
	BIO_free_all(bio);

	for(i = 0; i < MD5_DIGEST_LENGTH*2; i++) {
		if(b64_digest[i] <= ' ') {
			b64_digest[i] = '\0';
        }
	}

	if(!(md5 = (char*)calloc((strlen(b64_digest) + 1), sizeof(char)))) {
		printf("Could not alloc memory for md5 buffer!\n");
		fflush(stdout);
        return NULL;
	} 

	memcpy(md5, b64_digest, strlen(b64_digest));

	return md5;
}

#endif

