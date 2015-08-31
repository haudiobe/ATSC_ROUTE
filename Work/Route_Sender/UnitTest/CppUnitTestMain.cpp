/*
 *   MAD-FLUTE-UNITTEST: FLUTE unit test.
 *   File: CppUnitTestMain.cpp
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

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include <stdexcept>
#include <fstream>
#include <string>

#include "flute.h"

/* Set to 0 to avoid memory run time debug */
#define _DEBUG_MEMORY 0

int 
main( int argc, char* argv[] )
{
    
#if defined(_MSC_VER) && _DEBUG_MEMORY > 0
  _CrtSetDbgFlag(
        _CRTDBG_ALLOC_MEM_DF |
        _CRTDBG_CHECK_ALWAYS_DF |
        _CRTDBG_CHECK_CRT_DF |
       // _CRTDBG_DELAY_FREE_MEM_DF |
        _CRTDBG_LEAK_CHECK_DF);
#endif
  
  // Retreive test path from command line first argument. Default to "" which resolve
  // to the top level suite.
  std::string testPath = (argc > 1) ? std::string(argv[1]) : std::string("");

  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
#ifdef WIN32
  CPPUNIT_NS::BriefTestProgressListener progress;
//  CPPUNIT_NS::TextTestProgressListener progress;
#else
  CPPUNIT_NS::BriefTestProgressListener progress;
#endif
  controller.addListener( &progress );

  start_up_flute();


  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );   
  try
  {
    std::cout << "Running "  <<  testPath;
    runner.run( controller, testPath );

    std::cerr << std::endl;

    // Print test in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
    outputter.write(); 

// Uncomment this for XML output
//    std::ofstream file( "tests.xml" );
//    CPPUNIT_NS::XmlOutputter xml( &result, file );
//    xml.setStyleSheet( "report.xsl" );
//    xml.write();
//    file.close();
  }
  catch ( std::invalid_argument &e )  // Test path not resolved
  {
    std::cerr  <<  std::endl  
               <<  "ERROR: "  <<  e.what()
               << std::endl;
    shut_down_flute(NULL);
    return 1;
  }

  shut_down_flute(NULL);
  return result.wasSuccessful() ? 0 : 1;
}
