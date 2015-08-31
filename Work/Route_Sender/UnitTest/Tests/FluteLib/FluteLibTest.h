/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: FluteLibTest.h
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

#ifndef __FLUTELIBTEST__
#define __FLUTELIBTEST__

#include <cppunit/extensions/HelperMacros.h>


class FluteLibTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( FluteLibTest );
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

    CPPUNIT_TEST( testSpeedSlowTransmission );
    CPPUNIT_TEST( testSpeedFastTransmission );
    CPPUNIT_TEST( testSpeedVeryFastTransmission );
    CPPUNIT_TEST( testSpeedVeryFastTransmission2 );
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

    void testSpeedSlowTransmission();
    void testSpeedFastTransmission();
    void testSpeedVeryFastTransmission();
    void testSpeedVeryFastTransmission2();

private:
    void performsFilesTest(int aFileSize, int aKBitRates, int aRepetition);
    void performsDirsTest(std::vector<int> aFileSizes, int aKBitRates, int aRepetition);
    unsigned long long performsFileSend(
        const class TemporaryFile& aTemporaryFile, int aKBitRates);
    void performsSpeedFilesTest(int aFileSize, int aKBitRates);
};


#endif //__FLUTELIBTEST__
