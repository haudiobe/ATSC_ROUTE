/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: FluteLibXorFECTest.h
 *   Copyright (c) 2006-2007 M.B.I. www.mbigroup.it
 *   main authors/contacts: gmendola@mbigroup.it and lpoderico@mbigroup.it
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __FLUTELIBXORFECTEST__
#define __FLUTELIBXORFECTEST__

#include <cppunit/extensions/HelperMacros.h>


class FluteLibXorFECTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( FluteLibXorFECTest );
    CPPUNIT_TEST( testSimpleDirTrasmission );
    CPPUNIT_TEST( testShortestDirTransmission );
    CPPUNIT_TEST( testHugeDirTransmission );
    CPPUNIT_TEST( testSlowDirTransmission );
    CPPUNIT_TEST( testFastDirTransmission );
    CPPUNIT_TEST( testVeryFastDirTransmission );
    CPPUNIT_TEST( testVeryFastTransmissionHugeDir );

    CPPUNIT_TEST( testSimpleTransmission );
    CPPUNIT_TEST( testShortestTransmission );
    CPPUNIT_TEST( testHugeTransmission );
    CPPUNIT_TEST( testSlowTransmission );
    CPPUNIT_TEST( testFastTransmission );
    CPPUNIT_TEST( testVeryFastTransmission );
    CPPUNIT_TEST_SUITE_END();

public:
    void testSimpleTransmission();
    void testShortestTransmission();
    void testHugeTransmission();
    void testSlowTransmission();
    void testFastTransmission();
    void testVeryFastTransmission();

    void testSimpleDirTrasmission();
    void testShortestDirTransmission();
    void testHugeDirTransmission();
    void testSlowDirTransmission();
    void testFastDirTransmission();
    void testVeryFastDirTransmission();
    void testVeryFastTransmissionHugeDir();

private:
    void performsFilesTest(int aFileSize, int aBitRates, int aRepetition);
    void performsDirsTest(std::vector<int> aFileSizes, int aBitRates, int aRepetition);
};


#endif //__FLUTELIBTEST__
