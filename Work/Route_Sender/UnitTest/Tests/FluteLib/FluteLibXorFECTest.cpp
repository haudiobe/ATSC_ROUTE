/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: FluteLibXorFECTest.cpp
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
#include "FluteLibXorFECTest.h"

#include "TestSupport/TemporaryFile.h"
#include "TestSupport/TemporaryDirectory.h"
#include "TestSupport/FluteReceiver.h"
#include "TestSupport/FluteSender.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FluteLibXorFECTest,
                                       fluteLibSuite() );

using namespace std;

void FluteLibXorFECTest::testSimpleTransmission()
{
    const int myFileSize = 100*1024;
    const int myBitRates = 1024;

    this->performsFilesTest(myFileSize, myBitRates, 10);
}

void FluteLibXorFECTest::testShortestTransmission()
{
    const int myFileSize = 1;
    const int myBitRates = 1024;

    this->performsFilesTest(myFileSize, myBitRates, 50);
}

void FluteLibXorFECTest::testHugeTransmission()
{
    const int myFileSize = 10*1024*1024; // 10MByte
    const int myBitRates = 5*1024;

    this->performsFilesTest(myFileSize, myBitRates, 10);
}

void FluteLibXorFECTest::testSlowTransmission()
{
    const int myFileSize = 10;
    const int myBitRates = 1;

    this->performsFilesTest(myFileSize, myBitRates, 10);
}

void FluteLibXorFECTest::testFastTransmission()
{
    const int myFileSize = 1*1024*1024;
    const int myBitRates = 40*1024; // 40 Mb/s

    this->performsFilesTest(myFileSize, myBitRates, 30);
}

void FluteLibXorFECTest::testVeryFastTransmission()
{
    const int myFileSize = 10*1024*1024;
    const int myBitRates = 60*1024; // 60 Mb/s

    this->performsFilesTest(myFileSize, myBitRates, 20);
}


//========================================================

void FluteLibXorFECTest::testSimpleDirTrasmission()
{
    std::vector<int> mySizes;
    mySizes.push_back(100*1024);
    mySizes.push_back(50*1024);
    const int myBitRates = 1024;

    performsDirsTest(mySizes, myBitRates, 20);
}

void FluteLibXorFECTest::testShortestDirTransmission()
{
    std::vector<int> mySizes;
    mySizes.push_back(1);
    mySizes.push_back(2);
    mySizes.push_back(3);
    const int myBitRates = 1024;

    performsDirsTest(mySizes, myBitRates, 20);
}

void FluteLibXorFECTest::testHugeDirTransmission()
{
    std::vector<int> mySizes;
    for ( int i = 0; i < 10; ++i ) {
        mySizes.push_back(10*1024*1024);
    }

    const int myBitRates = 10*1024;

    performsDirsTest(mySizes, myBitRates, 5);
}

void FluteLibXorFECTest::testSlowDirTransmission()
{
    const int myFileSize = 10;
    
    std::vector<int> mySizes;
    for ( int i = 1; i < 6; ++i ) {
        mySizes.push_back(i*myFileSize);
    }

    const int myBitRates = 1;

    performsDirsTest(mySizes, myBitRates, 10);
}

void FluteLibXorFECTest::testFastDirTransmission()
{
    const int myFileSize = 1*1024*1024;
    const int myBitRates = 40*1024; // 40 Mb/s
    
    std::vector<int> mySizes;
    for ( int i = 1; i < 6; ++i ) {
        mySizes.push_back(i*myFileSize);
    }

    performsDirsTest(mySizes, myBitRates, 10);

}

void FluteLibXorFECTest::testVeryFastDirTransmission()
{ 
    const int myFileSize = 1*1024*1024;
    const int myBitRates = 60*1024; // 60 Mb/s
    
    std::vector<int> mySizes;
    for ( int i = 1; i < 6; ++i ) {
           mySizes.push_back(i*myFileSize);
    }

    performsDirsTest(mySizes, myBitRates, 10);
}

void FluteLibXorFECTest::testVeryFastTransmissionHugeDir()
{ 
    const int myFileSize = 10*1024*1024;
    const int myBitRates = 60*1024; // 60 Mb/s
    
    std::vector<int> mySizes;
    for ( int i = 1; i < 6; ++i ) {
           mySizes.push_back(i*myFileSize);
    }

    performsDirsTest(mySizes, myBitRates, 10);
}


void FluteLibXorFECTest::performsFilesTest(int aFileSize, int aBitRates, int aRepetition)
{

    TemporaryFile myTemporaryFile(aFileSize);

    while (aRepetition--)
    {
        string myReceivedFilePath;
        {
            FluteReceiver myReceiver;
            myReceiver.startReceiving();
            
            {
                FluteSender mySender(myTemporaryFile.getFilePath(), aBitRates);
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


void FluteLibXorFECTest::performsDirsTest(std::vector<int> aFileSizes, int aBitRates, int aRepetition)
{
    TemporaryDirectory myTemporaryDir(aFileSizes);

    while (aRepetition--)
    {
        string myReceivedPath;
        {
            FluteReceiver myReceiver;
            myReceiver.startReceiving();

            {
                FluteSender mySender(myTemporaryDir.getDirPath(), aBitRates);
                mySender.addArgument("-x:1");
                mySender.addArgument("-X:10");
                mySender.addArgument("-P");
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

