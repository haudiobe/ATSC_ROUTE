/** \file helpers.c \brief Functions for parsing information from the configuration files
 *
 * $Author: peltotal $ $Date: 2007/02/27 11:32:01 $ $Revision: 1.10 $
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "helpers.h"

int parse_repair_conf_file(char *repair_conf_file, repair_arguments_t *ra) {
  
  char *buf = NULL;
  FILE *fp;
  struct stat file_stats;
  int nbytes;
  
  char *tmp = NULL;
  
  if(stat(repair_conf_file, &file_stats) == -1) {
    printf("Error: %s is not valid file name\n", repair_conf_file);
    fflush(stdout);
    return -1;
  }
  
  /* Allocate memory for buf */
  if(!(buf = (char*)calloc((file_stats.st_size + 1), sizeof(char)))) {
    printf("Could not alloc memory for buffer!\n");
    fflush(stdout);
    return -1;
  }
  
  if((fp = fopen(repair_conf_file, "rb")) == NULL) {
    printf("Error: unable to open file %s\n", repair_conf_file);
    fflush(stdout);
    free(buf);
    return -1;
  }
  
  nbytes = fread(buf, 1, file_stats.st_size, fp);
  
  if(nbytes <= 0) {
    free(buf);
    fclose(fp);
    return -1;
  }
  
  fclose(fp);
  
  tmp = strtok (buf, "=");
  
  while(tmp != NULL) {
    if(strcmp(tmp, "RepairMethod") == 0) {
      tmp = strtok(NULL, "\n");
      memset(ra->repair_method, 0, MAX_PATH_LENGTH);
      strcpy(ra->repair_method, tmp);
      
      if(ra->repair_method[strlen(ra->repair_method)-1] == '\r') {
	ra->repair_method[strlen(ra->repair_method)-1] = '\0';
      }
    }
    else if(strcmp(tmp, "RequestedBlocksFile") == 0) {
      tmp = strtok(NULL, "\n");
      memset(ra->requested_blocks_file, 0, MAX_PATH_LENGTH);
      
      strcpy(ra->requested_blocks_file, tmp);
      
      if(ra->requested_blocks_file[strlen(ra->requested_blocks_file)-1] == '\r') {
	ra->requested_blocks_file[strlen(ra->requested_blocks_file)-1] = '\0';
      }
    }
    else if(strcmp(tmp, "PTMRepairSDPFile") == 0) {
      tmp = strtok(NULL, "\n");
      memset(ra->ptm_repair_sdp_file, 0, MAX_PATH_LENGTH);
      strcpy(ra->ptm_repair_sdp_file, tmp);
      
      if(ra->ptm_repair_sdp_file[strlen(ra->ptm_repair_sdp_file)-1] == '\r') {
	ra->ptm_repair_sdp_file[strlen(ra->ptm_repair_sdp_file)-1] = '\0';
      }
    }
    else if(strcmp(tmp, "FluteConfFile") == 0) {
      tmp = strtok(NULL, "\n");
      memset(ra->flute_conf_file, 0, MAX_PATH_LENGTH);
      strcpy(ra->flute_conf_file, tmp);
      
      if(ra->flute_conf_file[strlen(ra->flute_conf_file)-1] == '\r') {
	ra->flute_conf_file[strlen(ra->flute_conf_file)-1] = '\0';
      }
    }
    
    tmp = strtok (NULL, "=");
  }
  
  free(buf);
  
  return 0;
}

int parse_flute_conf_file(repair_arguments_t *ra) {
  
  char *buf = NULL;
  FILE *fp;
  struct stat file_stats;
  int nbytes;
  char *tmp = NULL;
  
  if(stat(ra->flute_conf_file, &file_stats) == -1) {
    printf("Error: %s is not valid file name\n", ra->flute_conf_file);
    fflush(stdout);
    return -1;
  }
  
  if(!(buf = (char*)calloc((file_stats.st_size + 1), sizeof(char)))) {
    printf("Could not alloc memory for buffer!\n");
    fflush(stdout);
    return -1;
  }
  
  if((fp = fopen(ra->flute_conf_file, "rb")) == NULL) {
    printf("Error: unable to open file %s\n", ra->flute_conf_file);
    fflush(stdout);
    free(buf);
    return -1;
  }
  
  nbytes = fread(buf, 1, file_stats.st_size, fp); 
  
  if(nbytes <= 0) {
    free(buf);
    fclose(fp);
    return -1;
  }
  fclose(fp);
  
  tmp = strtok (buf, "=");
  
  while(tmp != NULL) {
    if(strcmp(tmp, "BaseDir") == 0) {
      tmp = strtok(NULL, "\n");
      
      memset(ra->base_dir, 0, MAX_PATH_LENGTH);
      strcpy(ra->base_dir, tmp);
      
      if(ra->base_dir[strlen(ra->base_dir)-1] == '\r') {
	ra->base_dir[strlen(ra->base_dir)-1] = '\0';
      }
    }
    else if(strcmp(tmp, "FECRatio") == 0) {
      tmp = strtok(NULL, "\n");
      ra->fec_ratio = (unsigned short)atoi(tmp);
    }
    else if(strcmp(tmp, "FDTFile") == 0) {
      tmp = strtok(NULL, "\n");

      memset(ra->fdt_file, 0, MAX_PATH_LENGTH);
      strcpy(ra->fdt_file, tmp);
      
      if(ra->fdt_file[strlen(ra->fdt_file)-1] == '\r') {
	ra->fdt_file[strlen(ra->fdt_file)-1] = '\0';
      }
    }
    
    tmp = strtok (NULL, "=");
  }
  
  free(buf);
  
  return 0;
}
