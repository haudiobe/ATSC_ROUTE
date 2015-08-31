/** \file uri.h \brief Funtions for file URI handling
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.11 $
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

#ifndef _URI_H_
#define _URI_H_

#include "flute_defines.h"
#include "../alclib/defines.h"

/**
 * Structure for file URI.
 * @struct uri
 */

typedef struct uri {

	char* scheme;	/**< URI scheme (file, http, ftp...) */
	char* host;		/**< hostname part (www.foo.com) */
	char* port;		/**< port part if any (www.foo.com:8080 => 8080) */
	char* path;		/**< path portion without params and query */
	char* params;	/**< params part (/foo;dir/bar => foo) */
	char* query;	/**< query part (/foo?bar=val => bar=val) */
	char* frag;		/**< frag part (/foo#part => part) */
	char* user;		/**< user part (http://user:pass@www.foo.com => user) */
	char* passwd;	/**< user part (http://user:pass@www.foo.com => pass) */

} uri_t;

/**
 * This function allocates memory for uri_t structure.
 *
 * @return pointer to allocated uri_t structure, NULL in error cases.
 *
 */

uri_t* alloc_uri_struct(void);

/**
 * This function parses URI string to the uri_t structure.
 *
 * @param uri_string URI string
 * @param len length of thr URI string
 *
 * @return pointer to constructed uri_t structure, NULL in error cases.
 *
 */

uri_t* parse_uri(char* uri_string, int len);

/**
 * This function constructs URI string.
 *
 * @param uri uri_t structure
 *
 * @return pointer to constructed URI string, NULL in error cases.
 *
 */

char* uri_string(uri_t *uri);

/**
 * This function frees uri_t structure.
 *
 * @param uri uri_t structure
 *
 */

void free_uri(uri_t* uri);

/**
 * This function returns host and path string (e.g. www.foo.com/foo).
 *
 * @param uri uri_t structure
 *
 * @return pointer to constructed string, NULL in error cases.
 *
 */

char* get_uri_host_and_path(uri_t* uri);

/**
 * This function sets scheme.
 *
 * @param uri uri_t structure
 * @param scheme URI scheme (file, http, ftp...)
 *
 */

void set_uri_scheme(uri_t* uri, char* scheme);

/**
 * This function sets username.
 *
 * @param uri uri_t structure
 * @param user username
 *
 */

void set_uri_user(uri_t* uri, char* user);

/**
 * This function sets hostname.
 *
 * @param uri uri_t structure
 * @param host (sender's) hostname
 *
 */

void set_uri_host(uri_t* uri, char* host);

/**
 * This function sets path.
 *
 * @param uri uri_t structure
 * @param path file path
 *
 */

void set_uri_path(uri_t* uri, char* path);

#endif

