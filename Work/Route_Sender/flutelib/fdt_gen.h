/** \file fdt_gen.h \brief FDT generation
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

#ifndef _FDT_GEN_H_
#define _FDT_GEN_H_

#include "flute_defines.h"
#include "../alclib/defines.h"
#include "../alclib/utils.h"

/**
 * This function generates an FDT file.
 *
 * @param file_token files or directories to be parsed to the FDT
 * @param base_dir base directory for files or directories to be parsed to the FDT
 * @param s_id session identifier
 * @param fdt_file_name file name for the generated FDT
 * @param complete_fdt generate complete FDT (0 = no, 1 and 2 = yes)
 * @param verbosity verbosity level
 *
 * @return 0 in success, -1 otherwise
 *
 */

int generate_fdt(char *file_token, char *base_dir, int *s_id, char* fdt_file_name,
				 int complete_fdt, int verbosity);

#endif

