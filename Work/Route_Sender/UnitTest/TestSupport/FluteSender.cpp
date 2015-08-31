/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: FluteSender.cpp
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

#include <string>
#include <sstream>

#include "FluteSender.h"

#include "flute.h"

using namespace std;

FluteSender::FluteSender(const std::string& aPath, int aBitRates)
:theThread(0)
,theFluteSenderThread(aPath, aBitRates)
{
}

FluteSender::~FluteSender()
{
    theThread->join();
    delete theThread;
}

void FluteSender::addArgument(const std::string& anArgument)
{
    theFluteSenderThread.addArgument(anArgument);
}

void FluteSender::startSending()
{
    theThread = new boost::thread(theFluteSenderThread);
}

unsigned long long FluteSender::sessionSize()
{
    return theFluteSenderThread.sessionSize();
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

FluteSenderThread::FluteSenderThread(const std::string& aPath, int aBitRates)
:thePath(aPath)
,theBitRates(aBitRates)
,theArgv(0)
{
}

void FluteSenderThread::addArgument(const std::string& anArgument)
{
    theArguments.push_back(anArgument);
}

void FluteSenderThread::operator() ()
{
    arguments_t myArguments;
    int mySessionId;

    this->initArgument();

    string myFileOption = "-F:" + thePath;
    theArguments.push_back(myFileOption);

    ostringstream myBitRateOptionStream;
    myBitRateOptionStream<< "-r:" << theBitRates << ends;
    string myBitRateOption = myBitRateOptionStream.str();
    theArguments.push_back(myBitRateOption);

    int myArgc;
    const char** myArgv;
    this->getArgcArgv(myArgc, myArgv);

    parse_args(myArgc, const_cast<char**>(myArgv), &myArguments);

    unsigned long long mySessionSize;
    flute_sender(&myArguments, &mySessionId, &mySessionSize);
}

unsigned long long FluteSenderThread::sessionSize()
{
    arguments_t myArguments;
    int mySessionId;

    this->initArgument();

    string myFileOption = "-F:" + thePath;
    theArguments.push_back(myFileOption);

    theArguments.push_back("-G"); // Calculate session size only.

    ostringstream myBitRateOptionStream;
    myBitRateOptionStream<< "-r:" << theBitRates << ends;
    string myBitRateOption = myBitRateOptionStream.str();
    theArguments.push_back(myBitRateOption);

    int myArgc;
    const char** myArgv;
    this->getArgcArgv(myArgc, myArgv);

    parse_args(myArgc, const_cast<char**>(myArgv), &myArguments);

    unsigned long long mySessionSize;
    flute_sender(&myArguments, &mySessionId, &mySessionSize);

    return mySessionSize;
}

void FluteSenderThread::getArgcArgv(int& anArgc, const char**& anArgv)
{
    anArgc = int(theArguments.size());
    theArgv.resize(anArgc);

    TArguments::const_iterator myIterArgs = theArguments.begin();
    for (int i=0; i<anArgc; ++i)
    {
        assert (myIterArgs != theArguments.end());
        theArgv[i] = myIterArgs->c_str();
        ++myIterArgs;
    }
    anArgv = &theArgv[0];
}

void FluteSenderThread::initArgument()
{
    theArguments.clear();
    theArguments.push_back("FluteSender"); // Name
    theArguments.push_back("-S"); // Send
    theArguments.push_back("-v:0"); // Verbose option
    theArguments.push_back("-T:0"); // Nothing goes out this machine
}

