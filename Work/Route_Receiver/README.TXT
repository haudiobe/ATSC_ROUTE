--------------------------------------------------------------------------------

                               MAD-FCL archive

--------------------------------------------------------------------------------
	
    MAD-ALCLIB: Implementation of ALC/LCT protocols, Compact No-Code FEC,
    Simple XOR FEC, Reed-Solomon FEC, and RLC Congestion Control protocol.
    Copyright (c) 2003-2007 TUT - Tampere University of Technology
    main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi

    MAD-SDPLIB: Implementation of SDP protocol with FLUTE descriptors.
    Copyright (c) 2003-2007 TUT - Tampere University of Technology        
    main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi

    MAD-FLUTELIB: Implementation of FLUTE protocol.
    Copyright (c) 2003-2007 TUT - Tampere University of Technology
    main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi

    MAD-FLUTE: FLUTE application.              
    Copyright (c) 2003-2007 TUT - Tampere University of Technology  
    main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi

    MAD-FLUTE-REPAIR-SERVER: Implementation of repair server for FLUTE protocol.
    Copyright (c) 2005-2007 TUT - Tampere University of Technology
    main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi
	
    MAD-REPAIR-SENDER: Implementation of simple point-to-multipoint repair for
                       FLUTE protocol.
    Copyright (c) 2003-2007 TUT - Tampere University of Technology
    main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi
	
    MAD-FLUTE-UNITTEST: FLUTE unit test.
    Copyright (c) 2006-2007 M.B.I. www.mbigroup.it
    main authors/contacts: gmendola@mbigroup.it and lpoderico@mbigroup.it
	
    This program is free software; you can redistribute it and/or modify	
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	
    In addition, as a special exception, TUT - Tampere University of Technology
    gives permission to link the code of this program with the OpenSSL library
    (or with modified versions of OpenSSL that use the same license as OpenSSL),
    and distribute linked combinations including the two. You must obey the GNU
    General Public License in all respects for all of the code used other than
    OpenSSL. If you modify this file, you may extend this exception to your
    version of the file, but you are not obligated to do so. If you do not wish
    to do so, delete this exception statement from your version.

--------------------------------------------------------------------------------

    MAD-FCL archive consist of:

    - alclib:                   Source files for MAD-ALCLIB library.
    - flute:                    Source files for MAD-FLUTE application.
    - flutelib:                 Source files for MAD-FLUTELIB library.
    - multis_flute:             Source files for MAD-FLUTE multisession demo
                                application with two simultaneous session.
    - sdplib:                   Source files for MAD-SDPLIB library.
    - doc:                      Example File Delivery Table (FDT), Associated
                                Procedure Description (APD) and Session
                                Description Protocol (SDP) files.
    - flute-http-repair-server: Source files for MAD-FLUTE-REPAIR-SERVER
                                application.
    - repair_sender:            Source files for MAD-REPAIR-SENDER application.
    - UnitTest:                 Source files for MAD-FLUTE-UNITTEST application.
    - Makefile:                 Makefile for Linux OS.
    - mad_fcl.sln:              Microsoft Visual Studio 2005 solution file.
    - README.TXT:               This readme file.
    - LICENCE.TXT:              Archive's licence file.
	
--------------------------------------------------------------------------------

    Supported Operation Systems are:

    - Debian GNU/Linux (Tested with version 2.6.18) 
    - Windows 2000/XP

--------------------------------------------------------------------------------

    How to compile MAD-FCL in Linux:

    The MAD-FCL archive contains a Makefile to compile the MAD-ALCLIB, the
    MAD-FLUTELIB and the MAD-SDPLIB libraries, and the MAD-FLUTE and
    MAD-FLUTE-UNITTEST applications. Please note that the Makefile is defined
    for the GNU make. 

    Only write (g)make in the top directory, and all directories are scanned
    through and the MAD-ALCLIB, the MAD-FLUTELIB and the MAD-SDPLIB libraries,
    and the MAD-FLUTE and MAD-FLUTE-UNITTEST applications are compiled.

    The MAD-ALCLIB, the MAD-FLUTELIB and the MAD-SDPLIB libraries are created to
    the lib directory, and the MAD-FLUTE and MAD-FLUTE-UNITTEST applications are
    created to the bin directory.
	
    There are own makefiles for MAD-FLUTE-REPAIR-SERVER and MAD-REPAIR-SENDER
    applications in corresponding directories. 
	
--------------------------------------------------------------------------------

    How to compile MAD-FCL in Windows:

    The MAD-FCL archive contains a Microsoft Visual Studio 2005 solution file
    (mad_fcl.sln) to compile the MAD-ALCLIB, the MAD-FLUTELIB and the
    MAD-SDPLIB libraries, and the MAD-FLUTE, MAD-FLUTE-UNITTEST and
    MAD-REPAIR-SENDER applications. 

    First start the Microsoft Visual Studio 2005 and open the MAD-FCL solution,
    and then use Build menu to create the MAD-ALCLIB, the MAD-FLUTELIB and the
    MAD-SDPLIB libraries, and the MAD-FLUTE, MAD-FLUTE-UNITTEST and
    MAD-REPAIR-SENDER applications. Note that you have to define paths to the
    external libraries by yourself.

    The MAD-ALCLIB, the MAD-FLUTELIB and the MAD-SDPLIB libraries are created
    to the lib\win32\debug or lib\win32\release directory, and the MAD-FLUTE,
    MAD-FLUTE-UNITTEST and MAD-REPAIR-SENDER applications are created to the
    bin\win32\debug or bin\win32\release directory depending on the used
    configuration.

--------------------------------------------------------------------------------
