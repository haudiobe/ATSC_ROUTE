/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: TemporaryDirectory.cpp
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

#include "TemporaryDirectory.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <stdexcept>

#include <algorithm>
#include <functional>
#ifdef _MSC_VER
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif


TemporaryDirectory::TemporaryDirectory(std::vector<int> aFileSizeVector)
    : theDirPath()
    , theFiles()
{
    theDirPath = tmpnam(NULL);
    theDirPath += "fut";

    for(size_t i = 0; i < theDirPath.size(); ++i)
    {
        if (theDirPath[i]=='\\')
        {
            theDirPath[i]='/';
        }
    }

#ifdef _MSC_VER
    mkdir(theDirPath.c_str());
#else
    mkdir(theDirPath.c_str(), 0xffffff);
#endif
    
    int myVectorSize = (int)aFileSizeVector.size();
    for(int myIndex = 0; myIndex < myVectorSize; ++myIndex)
    {
        createAndAddFile(aFileSizeVector[myIndex]);
    }
}

TemporaryDirectory::TemporaryDirectory(const std::string& aDirPath)
{
    theDirPath = aDirPath;
    
    boost::filesystem::path myDirectory(theDirPath);

    if(!boost::filesystem::exists(myDirectory))
    {
        std::string myError("Directory ");
        myError += theDirPath;
        myError += " does not exist";
        throw std::runtime_error(myError);
    }

    if ( ! boost::filesystem::is_directory(myDirectory) ) {
        
        std::string myError = theDirPath;
        myError += " is not a directory";
        throw std::runtime_error(myError);
    }
    
    boost::filesystem::directory_iterator myEndIter;
    boost::filesystem::directory_iterator myDirIter(myDirectory);
    for(/*void*/;
        myDirIter != myEndIter;
        ++myDirIter)
    {
        TemporaryFile * myFile = new TemporaryFile(theDirPath + "/" + myDirIter->leaf() );

        theFiles.insert(std::make_pair(myFile->getFileName(), myFile));
    }
}


TemporaryDirectory::~TemporaryDirectory()
{
    Files::iterator myIter = theFiles.begin();
    Files::const_iterator myEnd = theFiles.end();
    for(/*void*/; myIter != myEnd; ++myIter)
    {
        delete myIter->second;
    }

    boost::filesystem::path myDirectory(theDirPath);

    boost::filesystem::remove(myDirectory);
}

void TemporaryDirectory::createAndAddFile(int aFilesize)
{
    TemporaryFile *myFile = new TemporaryFile(aFilesize, theDirPath);
    theFiles.insert(std::make_pair(myFile->getFileName(), myFile));
}


bool TemporaryDirectory::operator ==(const TemporaryDirectory& aDir) const
{
    if(theFiles.size() != aDir.theFiles.size())
    {
        return false;
    }

    Files::const_iterator myIter = theFiles.begin();
    Files::const_iterator myEnd = theFiles.end();
    for(/*void*/; myIter != myEnd; ++myIter)
    {
        Files::const_iterator myRightFile = aDir.theFiles.find(myIter->first);
        if(myRightFile == aDir.theFiles.end())
        {
            return false;
        }
        if(!(*(myIter->second) == *(myRightFile->second)))
        {
            return false;
        }
    }
    return true;
}


std::ostream& operator<<(std::ostream& aStream, const TemporaryDirectory& aDir)
{
    return aStream << aDir.getDirPath();
}

