/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: FluteSender.h
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

#ifndef __FLUTESENDER__
#define __FLUTESENDER__

#include "TemporaryFile.h"

#include <boost/thread/thread.hpp>

#include <string>
#include <list>
#include <vector>

class FluteSenderThread
{
public:
    FluteSenderThread(const std::string& aPath, int aBitRates);

    void addArgument(const std::string& anArgument);

    void operator() ();

    unsigned long long sessionSize();

private:
    typedef std::list<std::string> TArguments;
    const std::string thePath;
    int theBitRates;
    TArguments theArguments;

    std::vector<const char*> theArgv;

    /**
     * Parse theArguments building the couple argc and argv.
     */
    void getArgcArgv(int& anArgc, const char**& anArgv);

    /**
     * Initialize the arguments.
     */
    void initArgument();
};



/**
 * Send aTemporaryFile at the given aBitRates.
 * An exception is raised if something goes wrong.
 */
class FluteSender
{
public:
    FluteSender(const std::string& aPath, int aBitRates);

    ~FluteSender();

    void addArgument(const std::string& anArgument);

    unsigned long long sessionSize();

    void startSending();

private:
    boost::thread* theThread;
    FluteSenderThread theFluteSenderThread;
};

#endif //__FLUTESENDER__
