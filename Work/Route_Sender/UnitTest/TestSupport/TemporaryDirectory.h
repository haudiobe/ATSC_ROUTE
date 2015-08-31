/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: TemporaryDirectory.h
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

//#ifndef __TEMPORARYDIRECTORY__
//#define __TEMPORARYDIRECTORY__

#pragma once

#include "TemporaryFile.h"

#include <boost/utility.hpp>

#include <string>
#include <iostream>
#include <map>
#include <vector>

/**
 * Create a temporary file of aFileSize bytes. It is deleted automatically when the
 * object is deleted.
 */
class TemporaryDirectory : public boost::noncopyable
{
private:

    typedef std::map<std::string, TemporaryFile*> Files;

public:
    /**
     * Create a temporary file of aFileSize bytes.
     */
    TemporaryDirectory(std::vector<int>  aFileSizeVector);

    /**
     * Attach this at an existing file.
     */
    TemporaryDirectory(const std::string& aDirPath);

    ~TemporaryDirectory();

    /**
     * Return the complete file path.
     */
    std::string getDirPath() const
    {
        return theDirPath;
    }

    /**
     * Return the name of the file.
     */
    std::string getDirName() const;

    bool operator == (const TemporaryDirectory& aDir) const;

private:


    std::string theDirPath;

    Files theFiles;

    void createAndAddFile(int aFileSize);
};

std::ostream& operator<<(std::ostream&, const TemporaryDirectory&);

/*#endif */// defined __TEMPORARYDIRECTORY__
