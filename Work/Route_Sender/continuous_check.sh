#!/bin/bash

#  MAD-FLUTE-UNITTEST: FLUTE unit test.
#  File: CppUnitTestSuite.cpp
#  Copyright (c) 2006-2007 M.B.I. www.mbigroup.it
#  main authors/contacts: gmendola@mbigroup.it and lpoderico@mbigroup.it
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

LOG_FILE='flute_unit_test.log'

sleep 1

# Cleans the work space
rm -fr *

# Gets the last revision
svn update . >> ${LOG_FILE} 2>&1

# Make all
make >>${LOG_FILE} 2>&1

if [ $? -ne 0 ]
then
    cat ${LOG_FILE} | mail -s 'mad_fcl unit test failed' test_flute@mbigroup.it
fi

# Run the test
./bin/flute_unittest >>${LOG_FILE} 2>&1

if [ $? -ne 0 ]
then
    cat ${LOG_FILE} | mail -s 'mad_fcl unit test failed' test_flute@mbigroup.it
fi

#Run itself
./continuous_check.sh &

