/** \file fdt.h \brief EFDT parsing

 */

#ifndef _EFDT_H_
#define _EFDT_H_

#include "flute_defines.h"
#include "../alclib/defines.h"
#include "../alclib/utils.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Structure for EFDT.
 * @struct efdt
 */

typedef struct efdt {
	unsigned int tsi;
	char *idRef;
	unsigned int version;
	unsigned int maxExpiresDelta;
	unsigned int maxTransportSize;
	unsigned long long expires;	/* efdt expiration time in NTP-time */
	BOOL complete;				/**< is complete EFDT? */
	file_t *file_list;			/**< list of files in the EFDT*/
	unsigned int nb_of_files;	/**< number of file in the EFDT*/

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

} efdt_t;

/**
* This function initializes EFDT parser.
*
*/

void initialize_efdt_parser(void);

/**
* This function releases EFDT parser.
*
*/

void release_efdt_parser(void);

/**
 * This function decodes EFDT XML document to the efdt structure using Expat XML library.
 *
 * @param efdt_payload pointer to buffer containing EFDT XML document
 *
 * @return pointer to the efdt structure, NULL in error cases
 *
 */

efdt_t* decode_efdt_payload(char *efdt_payload);

/**
 * This function frees EFDT structure.
 *
 * @param efdt pointer to the EFDT structure
 *
 */

void FreeEFDT(efdt_t *efdt);

/**
 * This function updates EFDT database.
 *
 * @param efdt_db pointer to EFDT database
 * @param instance pointer to received EFDT Instance
 *
 * @return 1 if existing file description in the EFDT database is complemented or 2
 * when new file description entity is added in the EFDT database, 0 if EFDT database is not
 * updated, -1 in error cases
 *
 */

int update_efdt(efdt_t *efdt_db, efdt_t *instance);

/**
 * This function returns EFDT information structure for wanted toi.
 *
 * @param efdt pointer to EFDT database
 * @param toi transport object identifier
 *
 * @return EFDT information structure for wanted toi, NULL if EFDT information does not exists
 *
 */


void PrintEFDT(efdt_t *efdt, int s_id);

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
