/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: FluteReceiver.h
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

#ifndef __FLUTERECEIVER__
#define __FLUTERECEIVER__

#include "flute.h"

#include <string>

#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

// Private class
class FluteReceiverThread
{
public:

    FluteReceiverThread();

    void operator() ();

    struct Status
    {
        int theSessionId;
        arguments_t theArguments;
    };

    typedef boost::shared_ptr<Status> StatusPtr;

    StatusPtr theStatusPtr;
};

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

/**
 * Receive the test transmission. Raise an exception is sometihin goes wrong.
 */
class FluteReceiver
{
public:
    FluteReceiver();

    ~FluteReceiver();

    void startReceiving();

    void operator() ();

    std::string getReceivedPath(const std::string& aName) const;

private:
    boost::thread* theThread;

    FluteReceiverThread theFluteReceiverThread;
};

#endif //__FLUTERECEIVER__
