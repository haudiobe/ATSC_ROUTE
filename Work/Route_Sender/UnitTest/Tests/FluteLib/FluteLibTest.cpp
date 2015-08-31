/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: FluteLibTest.cpp
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

#include "FluteLibSuite.h"
#include "FluteLibTest.h"

#include "TestSupport/TemporaryFile.h"
#include "TestSupport/TemporaryDirectory.h"
#include "TestSupport/FluteReceiver.h"
#include "TestSupport/FluteSender.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FluteLibTest,
                                       fluteLibSuite() );

using namespace std;
using namespace boost::posix_time;

void FluteLibTest::testSimpleTransmission()
{
    const int myFileSize = 100*1024;
    const int myKBitRates = 1024;

    this->performsFilesTest(myFileSize, myKBitRates, 10);
}

void FluteLibTest::testShortestTransmission()
{
    const int myFileSize = 1;
    const int myKBitRates = 1024;

    this->performsFilesTest(myFileSize, myKBitRates, 50);
}

void FluteLibTest::testHugeTransmission()
{
    const int myFileSize = 10*1024*1024; // 10MByte
    const int myKBitRates = 5*1024;

    this->performsFilesTest(myFileSize, myKBitRates, 10);
}

void FluteLibTest::testSlowTransmission()
{
    const int myFileSize = 10;
    const int myKBitRates = 1;

    this->performsFilesTest(myFileSize, myKBitRates, 10);
}

void FluteLibTest::testFastTransmission()
{
    const int myFileSize = 1*1024*1024;
    const int myKBitRates = 40*1024; // 40 Mb/s

    this->performsFilesTest(myFileSize, myKBitRates, 30);
}

void FluteLibTest::testVeryFastTransmission()
{
    const int myFileSize = 10*1024*1024;
    const int myKBitRates = 60*1024; // 60 Mb/s

    this->performsFilesTest(myFileSize, myKBitRates, 20);
}


///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void FluteLibTest::testSimpleDirTrasmission()
{
    std::vector<int> mySizes;
    mySizes.push_back(100*1024);
    mySizes.push_back(50*1024);
    const int myKBitRates = 1024;

    performsDirsTest(mySizes, myKBitRates, 20);
}

void FluteLibTest::testShortestDirTransmission()
{
    std::vector<int> mySizes;
    mySizes.push_back(1);
    mySizes.push_back(2);
    mySizes.push_back(3);
    const int myKBitRates = 1024;

    performsDirsTest(mySizes, myKBitRates, 20);
}

void FluteLibTest::testHugeDirTransmission()
{
    std::vector<int> mySizes;
    for ( int i = 0; i < 10; ++i ) {
        mySizes.push_back(10*1024*1024);
    }

    const int myKBitRates = 10*1024;

    performsDirsTest(mySizes, myKBitRates, 5);
}

void FluteLibTest::testSlowDirTransmission()
{
    const int myFileSize = 10;
    
    std::vector<int> mySizes;
    for ( int i = 1; i < 6; ++i ) {
        mySizes.push_back(i*myFileSize);
    }

    const int myKBitRates = 1;

    performsDirsTest(mySizes, myKBitRates, 10);
}

void FluteLibTest::testFastDirTransmission()
{
    const int myFileSize = 1*1024*1024;
    const int myKBitRates = 40*1024; // 40 Mb/s
    
    std::vector<int> mySizes;
    for ( int i = 1; i < 6; ++i ) {
        mySizes.push_back(i*myFileSize);
    }

    performsDirsTest(mySizes, myKBitRates, 10);

}

void FluteLibTest::testVeryFastDirTransmission()
{ 
    const int myFileSize = 1*1024*1024;
    const int myKBitRates = 60*1024; // 60 Mb/s
    
    std::vector<int> mySizes;
    for ( int i = 1; i < 6; ++i ) {
           mySizes.push_back(i*myFileSize);
    }

    performsDirsTest(mySizes, myKBitRates, 10);
}

void FluteLibTest::testVeryFastTransmissionHugeDir()
{ 
    const int myFileSize = 10*1024*1024;
    const int myKBitRates = 60*1024; // 60 Mb/s
    
    std::vector<int> mySizes;
    for ( int i = 1; i < 6; ++i ) {
           mySizes.push_back(i*myFileSize);
    }

    performsDirsTest(mySizes, myKBitRates, 10);
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void FluteLibTest::testSpeedSlowTransmission()
{
    const int myFileSize = 100 * 1024;
    const int myKBitRates = 64;

    this->performsSpeedFilesTest(myFileSize, myKBitRates);
}

void FluteLibTest::testSpeedFastTransmission()
{
    const int myFileSize = 1000 * 1024;
    const int myKBitRates = 2048;

    this->performsSpeedFilesTest(myFileSize, myKBitRates);
}

void FluteLibTest::testSpeedVeryFastTransmission()
{
    const int myFileSize = 10000 * 1024;
    const int myKBitRates = 10 * 1024;

    this->performsSpeedFilesTest(myFileSize, myKBitRates);
}

void FluteLibTest::testSpeedVeryFastTransmission2()
{
    const int myFileSize = 50000 * 1024;
    const int myKBitRates = 11 * 1024;

    this->performsSpeedFilesTest(myFileSize, myKBitRates);
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void FluteLibTest::performsFilesTest(int aFileSize, int aKBitRates, int aRepetition)
{

    TemporaryFile myTemporaryFile(aFileSize);

    while (aRepetition--)
    {
        string myReceivedFilePath;
        {
            FluteReceiver myReceiver;
            myReceiver.startReceiving();
            
            {
                FluteSender mySender(myTemporaryFile.getFilePath(), aKBitRates);
                mySender.startSending();
            }

            myReceivedFilePath =
                myReceiver.getReceivedPath(myTemporaryFile.getFilePath());
        }
        TemporaryFile myReceivedFile(myReceivedFilePath);
        CPPUNIT_ASSERT_EQUAL(myTemporaryFile, myReceivedFile);
        cout << '*';
    }
}


void FluteLibTest::performsDirsTest(
                                    std::vector<int> aFileSizes,
                                    int aKBitRates,
                                    int aRepetition)
{
    TemporaryDirectory myTemporaryDir(aFileSizes);

    while (aRepetition--)
    {
        string myReceivedPath;
        {
            FluteReceiver myReceiver;
            myReceiver.startReceiving();

            {
                FluteSender mySender(myTemporaryDir.getDirPath(), aKBitRates);
                mySender.startSending();
            }

            myReceivedPath =
                myReceiver.getReceivedPath(myTemporaryDir.getDirPath());
        }
        TemporaryDirectory myReceivedDir(myReceivedPath);
        CPPUNIT_ASSERT_EQUAL(myTemporaryDir, myReceivedDir);
        cout << '*';
    }
}

unsigned long long FluteLibTest::performsFileSend(
    const TemporaryFile& aTemporaryFile, int aKBitRates)
{
    FluteSender mySender(aTemporaryFile.getFilePath(), aKBitRates);

    unsigned long long mySessionSize = mySender.sessionSize();
    mySender.startSending();

    return mySessionSize;
}

void FluteLibTest::performsSpeedFilesTest(int aFileSize, int aKBitRates)
{
    TemporaryFile myTemporaryFile(aFileSize);

    ptime myStart(microsec_clock::local_time());
    unsigned long long mySessionSize =
        this->performsFileSend(myTemporaryFile, aKBitRates);
    ptime myEnd(microsec_clock::local_time());

    const time_duration myRealDuration = myEnd - myStart;
    const double myRealSecDuration = myRealDuration.total_milliseconds()/1000.0;

    const double mySecDuration = double(mySessionSize*8)/double(aKBitRates*1024);

    const double myError = fabs(mySecDuration-myRealSecDuration)/mySecDuration;

    CPPUNIT_ASSERT(myError<0.05);
}

