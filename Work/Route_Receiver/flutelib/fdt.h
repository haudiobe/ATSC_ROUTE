/** \file fdt.h \brief FDT parsing
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:01 $ $Revision: 1.26 $
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

#ifndef _FDT_H_
#define _FDT_H_

#include "flute_defines.h"
#include "../alclib/defines.h"
#include "../alclib/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure for file in the FDT structure.
 * @struct file
 */

typedef struct file {
	
	struct file *prev;				/**< previous item */
	struct file *next;				/**< next item */

	int status;						/**< status of the file (0 = not wanted, 1 = downloading, 2 = downloaded) */

	unsigned long long expires;		/**< expiration time for the file */
		
	unsigned long long toi;			/**< transport object identifier */
	unsigned long long content_len;	/**< length of the file */
	char *location;					/**< content location, file URI */
	char *md5;						/**< MD5 checksum for the file */
	
	/* These overwrite FDT level values */

	char *encoding;					/**< content encoding */					
	char *type;						/**< content type */

	/* FEC OTI, these overwrite FDT level values */

	short fec_enc_id;					/**< FEC encoding id */
	int fec_inst_id;					/**< FEC instance id */
	unsigned char finite_field;			/**< finite field parameter with new RS FEC */
	unsigned char nb_of_es_per_group;	/**< number of encoding symbols in packet with new RS FEC */
	unsigned int max_sb_len;			/**< maximum source block length */
	unsigned short es_len;				/**< encoding symbol length */
	unsigned short max_nb_of_es;		/**< maximum number of encoding symbols */

	/* FEC OTI, only at file level */

	unsigned long long transfer_len;	/**< transport length */

} file_t;

/**
 * Structure for FDT.
 * @struct fdt
 */

typedef struct fdt {
	unsigned long long expires;	/* fdt expiration time in NTP-time */
	BOOL complete;				/**< is complete FDT? */
	file_t *file_list;			/**< list of files in the FDT*/
	unsigned int nb_of_files;	/**< number of file in the FDT*/

	/* Other common parameters */

	char *type;					/**< default content type */
	char *encoding;				/**< default content encoding */

	/* FEC OTI */

	short fec_enc_id;					/**< default FEC encoding id */
	int fec_inst_id;					/**< default FEC instance id */
	unsigned char finite_field;			/**< default finite field parameter  with new RS FEC */
	unsigned char nb_of_es_per_group;	/**< default number of encoding symbols in packet with new RS FEC */
	unsigned int max_sb_len;			/**< default maximum source block length */
	unsigned short es_len;				/**< default encoding symbol length */
	unsigned short max_nb_of_es;		/**< default maximum number of encoding symbols */

} fdt_t;

/**
* This function initializes FDT parser.
*
*/

void initialize_fdt_parser(void);

/**
* This function releases FDT parser.
*
*/

void release_fdt_parser(void);

/**
 * This function decodes FDT XML document to the fdt structure using Expat XML library.
 *
 * @param fdt_payload pointer to buffer containing FDT XML document
 *
 * @return pointer to the fdt structure, NULL in error cases
 *
 */

fdt_t* decode_fdt_payload(char *fdt_payload);

/**
 * This function frees FDT structure.
 *
 * @param fdt pointer to the FDT structure
 *
 */

void FreeFDT(fdt_t *fdt);

/**
 * This function updates FDT database.
 *
 * @param fdt_db pointer to FDT database
 * @param instance pointer to received FDT Instance
 *
 * @return 1 if existing file description in the FDT database is complemented or 2
 * when new file description entity is added in the FDT database, 0 if FDT database is not
 * updated, -1 in error cases
 *
 */

int update_fdt(fdt_t *fdt_db, fdt_t *instance);

/**
 * This function returns FDT information structure for wanted toi.
 *
 * @param fdt pointer to FDT database
 * @param toi transport object identifier
 *
 * @return FDT information structure for wanted toi, NULL if FDT information does not exists
 *
 */

file_t* find_file_with_toi(fdt_t *fdt, unsigned long long toi);

/**
 * This function prints FDT information.
 *
 * @param fdt pointer to FDT structure
 * @param s_id session identifier
 *
 */

void PrintFDT(fdt_t *fdt, int s_id);

/**
 * This function frees file structure.
 *
 * @param file pointer to file structure
 *
 */

void free_file(file_t *file);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
