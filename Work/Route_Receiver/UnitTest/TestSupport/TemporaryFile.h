/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: TemporaryFile.h
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

#ifndef __TEMPORARYFILE__
#define __TEMPORARYFILE__



#include <boost/utility.hpp>

#include <string>
#include <iostream>

/**
 * Create a temporary file of aFileSize bytes. It is deleted automatically when the
 * object is deleted.
 */
class TemporaryFile :public boost::noncopyable
{
public:
    /**
     * Create a temporary file of aFileSize bytes.
     */
    TemporaryFile(int aFileSize, const std::string& aCreationPath = "");

    /**
     * Attach this at an existing file.
     */
    TemporaryFile(const std::string& aFilePath);

    ~TemporaryFile();

    /**
     * Return the complete file path.
     */
    std::string getFilePath() const
    {
        return theFilePath;
    }

    /**
     * Return the name of the file.
     */
    std::string getFileName() const;

    /**
     * Check if two files contains the same information.
     */
    bool operator==(const TemporaryFile& aRHS) const;

private:
    std::string theFilePath;

    void CreateAndFill(int aFileSize);

};

std::ostream& operator<<(std::ostream&, const TemporaryFile&);

#endif
