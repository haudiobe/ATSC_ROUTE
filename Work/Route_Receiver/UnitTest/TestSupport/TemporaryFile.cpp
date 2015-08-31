/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: TemporaryFile.cpp
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

#include "TemporaryFile.h"

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

#include <boost/filesystem/path.hpp>

using namespace std;

TemporaryFile::TemporaryFile(int aFileSize, const std::string& aCreationPath)
{
    theFilePath = tmpnam(NULL);
    if (theFilePath[0]=='\\')
    {
        theFilePath[0]='/';
    }
    theFilePath += "fut";

    if ( aCreationPath != "" ) {
        theFilePath = theFilePath.substr(theFilePath.rfind('/'));
    }

    theFilePath = aCreationPath + theFilePath;

    this->CreateAndFill(aFileSize);
}

TemporaryFile::TemporaryFile(const std::string& aFilePath)
{
    theFilePath = aFilePath;
}

TemporaryFile::~TemporaryFile()
{
    remove(theFilePath.c_str());
}

void TemporaryFile::CreateAndFill(int aFileSize)
{

    ofstream myFile(theFilePath.c_str());
   
    if ( !myFile ) {
        throw std::runtime_error("Unable to create the file");
    }

    const int myBufferSize = 1024*1024;
    std::vector<char> myBuffer(myBufferSize, 'x');

    while (aFileSize>=myBufferSize)
    {
        myFile.write(&myBuffer[0], myBufferSize);
        aFileSize -= myBufferSize;
    }

    std::vector<char> myBuffer2(aFileSize, 'x');
    myFile.write(&myBuffer2[0], aFileSize);
}

std::string TemporaryFile::getFileName() const
{
    //boost::filesystem::path myPath(theFilePath.c_str());

    //return myPath.leaf();

    return theFilePath.substr(theFilePath.rfind('/'));
}

bool TemporaryFile::operator==(const TemporaryFile& aRHS) const
{
    ifstream myLHS(this->getFilePath().c_str());
    ifstream myRHS(aRHS.getFilePath().c_str());

    while (!myLHS.eof() || !myRHS.eof())
    {
        if (myLHS.get() != myRHS.get())
        {
            return false;
        }
    }

    if (myLHS.eof() && myRHS.eof())
    {
        return true;
    }

    return false;
}

std::ostream& operator<<(std::ostream& aOStream, const TemporaryFile& aTempFile)
{
    aOStream << aTempFile.getFilePath();

    return aOStream;
}
