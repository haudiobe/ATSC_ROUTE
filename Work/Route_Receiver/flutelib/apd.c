/** \file apd.c \brief Associated prosedure description
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.14 $
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

#include <expat.h>

#include "apd.h"    

#ifdef USE_FILE_REPAIR

/**
 * Global variable used in parsing
 */

apd_t *apd;

/**
 * This is a private function which is used in parsing.
 *     
 * @param userData used to carry element name to characterDataHandler function
 * @param name pointer to buffer containing element's name
 * @param atts pointer to buffer containing element's attributes
 *            
 */  

static void startElementHandler_APD(void *userData, const char *name, const char **atts) {

  strcpy((char*)userData, name);

  while(*atts != NULL) {

    if(!strcmp(name, "postFileRepair")) {

      if(apd->postFileRepair == NULL) {

        if(!(apd->postFileRepair = (postFileRepair_t*)calloc(1, sizeof(postFileRepair_t)))) {
          printf("Could not alloc memory for postFileRepair!\n");
          return;          
        }

        /* initialise */

        apd->postFileRepair->offsetTime = 0;
        apd->postFileRepair->randomTimePeriod = 0;
        apd->postFileRepair->serviceURI_List = NULL;
      }

      if(!strcmp(*atts, "offsetTime")) {

        apd->postFileRepair->offsetTime = atoi(*(++atts));
      }
      else if(!strcmp(*atts, "randomTimePeriod")) {

        apd->postFileRepair->randomTimePeriod = atoi(*(++atts));
      }
    }

    else if(!strcmp(name, "bmFileRepair")) {

      if(apd->bmFileRepair == NULL) {

        if(!(apd->bmFileRepair = (bmFileRepair_t*)calloc(1, sizeof(bmFileRepair_t)))) {
          printf("Could not alloc memory for bmFileRepair!\n");
          return;
        }

        /* initialise */

        strcpy(apd->bmFileRepair->sessionDescriptionURI, "");

      }

      if(!strcmp(*atts, "sessionDescriptionURI")) {

        strcpy(apd->bmFileRepair->sessionDescriptionURI, *(++atts));
      }
    }

    /* attribute in unknown element */

    else {
      atts ++;
    }

    atts++;

  }
}

/**
 * This is a private function which is used in parsing.
 *
 * @param userData must be, not used
 * @param name pointer to buffer containing element's name
 *
 */

static void endElementHandler_APD(void *userData, const char *name) {

/* Let's remove the last element name from the userData */
  strcpy((char*)userData, "");
}

/**
 * This is a private function which is used in parsing.
 *
 * @param userData used to check in what element we are
 * @param s pointer to buffer containing character data (s is not 0 terminated)
 * @param len character data length
 *
 */

static void characterDataHandler_APD(void *userData, const XML_Char *s, int len) {

  serviceURI_t *tmp_serviceURI;
  serviceURI_t *serviceURI;
  char *value;
  
  value = (char*)calloc(len+1, sizeof(char));
  memcpy(value, s, len);

  /* userData includes the element (tag) name if we are inside some element */

  if(strcmp((const char*)userData, "") == 0) {

  }
  else if(strcmp((const char*)userData, "serviceURI") == 0) {

    if(!(serviceURI = (serviceURI_t*)calloc(1, sizeof(serviceURI_t)))) {
      printf("Could not alloc memory for serviceURI!\n");
      return;
    }

    strcpy(serviceURI->URI, value);
    serviceURI->prev = NULL;
    serviceURI->next = NULL;

    tmp_serviceURI = apd->postFileRepair->serviceURI_List;

    while(1) {

      if(tmp_serviceURI == NULL) {
        apd->postFileRepair->serviceURI_List = serviceURI;
        break;
      }

      if(tmp_serviceURI->next == NULL) {
        serviceURI->prev = tmp_serviceURI;
        tmp_serviceURI->next = serviceURI;
        break;
      }

      tmp_serviceURI = tmp_serviceURI->next;
    }
  }

  free(value);
}

apd_t* decode_apd_config(char *apd_config) {

        XML_Parser parser = XML_ParserCreate(NULL);
        size_t len;
        char tag[100];

        len = strlen(apd_config);

        if(!(apd = (apd_t*)calloc(1, sizeof(apd_t)))) {
                printf("Could not alloc memory for apd!\n");
                XML_ParserFree(parser);
                return NULL;
        }

        XML_SetUserData(parser, tag);
        XML_SetStartElementHandler(parser, startElementHandler_APD);
        XML_SetEndElementHandler(parser, endElementHandler_APD);
        XML_SetCharacterDataHandler(parser, characterDataHandler_APD);

        if(XML_Parse(parser, apd_config, len, 1) == XML_STATUS_ERROR) {
                fprintf(stderr, "%s at line %lu\n",
                                XML_ErrorString(XML_GetErrorCode(parser)),
                                XML_GetCurrentLineNumber(parser));
                XML_ParserFree(parser);
                return NULL;
        }

        XML_ParserFree(parser);

        return apd;
}

void FreeAPD(apd_t *apd) {		

	serviceURI_t *tmp_serviceURI;

	if(apd->postFileRepair != NULL) {

		while(1) {

			tmp_serviceURI = apd->postFileRepair->serviceURI_List;

			if(tmp_serviceURI != NULL) {
				apd->postFileRepair->serviceURI_List = tmp_serviceURI->next;
				free(tmp_serviceURI);
			}
			else {
				break;
			}
		}

		free(apd->postFileRepair);
	}

	if(apd->bmFileRepair != NULL) {
		free(apd->bmFileRepair);
	}

	free(apd);
}

#endif

