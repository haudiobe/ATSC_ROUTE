/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: FluteReceiver.cpp
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

#include "FluteReceiver.h"

#include <boost/thread/xtime.hpp>

using namespace std;

FluteReceiver::FluteReceiver()
:theThread(0)
{    
}


FluteReceiver::~FluteReceiver()
{
    theThread->join();

    delete theThread;
}

void FluteReceiver::startReceiving()
{
    theFluteReceiverThread.theStatusPtr->theSessionId = -1;

    theThread = new boost::thread(theFluteReceiverThread);
}

string FluteReceiver::getReceivedPath(const std::string& aFileName) const
{
    const arguments_t& myArguments = theFluteReceiverThread.theStatusPtr->theArguments;
    string myReceivedPath = myArguments.alc_a.base_dir;
    if ( aFileName[0] != '/' ) {
        myReceivedPath += "/";
    }
    myReceivedPath += aFileName;

    return myReceivedPath;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

FluteReceiverThread::FluteReceiverThread()
:theStatusPtr(new Status)
{
}

void FluteReceiverThread::operator() ()
{
    arguments_t& myArguments = theStatusPtr->theArguments;

    const int argc = 3;
    char* argv[argc] = {"test", "-A", "-v:1"};

    parse_args(argc, argv, &myArguments);
    flute_receiver(&myArguments, &theStatusPtr->theSessionId);
}
