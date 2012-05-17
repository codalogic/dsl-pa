//----------------------------------------------------------------------
// Copyright (c) 2012, Codalogic Ltd
// All rights reserved.
//
// The license for this file is based on the BSD-3-Clause license
// (http://www.opensource.org/licenses/BSD-3-Clause).
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
//
// - Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright 
//   notice, this list of conditions and the following disclaimer in 
//   the documentation and/or other materials provided with the 
//   distribution.
// - Neither the name Codalogic Ltd nor the names of its contributors 
//   may be used to endorse or promote products derived from this 
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------

#ifndef CLUNIT
#define CLUNIT

#include <iostream>
#include <fstream>
#include <strstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <crtdbg.h>

namespace cl {

#define OutputDebugString  OutputDebugStringA
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA( const char * lpOutputString );

#define TINIT( x ) cl::clunit::tout() << "\n\n    " << x << " (" << __FILE__ << ")\n    ==========================\n"; OutputDebugString( x "\n" )
#define TDOC( x ) cl::clunit::tout() << "      " << x << "\n"
#define TTODO( x ) cl::clunit::add_todo( x, __FILE__, __LINE__ )
#define TTODOX( x ) { bool t=(x); cl::clunit::add_todo( (std::string( #x ) + ((t)?" (passing)":" (failing)")).c_str(), __FILE__, __LINE__ ); }
#define TSETUP( x ) cl::clunit::tout() << "      : " << #x << '\n'; x
#define TTEST( x ) { bool t=(x); if( !(t) ) {cl::clunit::tout() << "not "; ++cl::clunit::errors;}else{cl::clunit::tout()<<"    ";} ++cl::clunit::tests; cl::clunit::tout() << "ok: " << #x; if( !(t) ) cl::clunit::tout() << " (" << __LINE__ << ")"; cl::clunit::tout() << "\n"; }
#define TRUNALL() { cl::clunit::run(); cl::clunit::report(); if( cl::clunit::errors > 255 ) return 255; return cl::clunit::errors; }

class clunit
{
public:
	typedef void(*job_func_ptr)() ;
	typedef std::vector< job_func_ptr > job_list;
	typedef std::vector< std::string > todo_list;

private:
	static bool first;
	static job_list & get_jobs();
	static todo_list & get_todos();

public:
	static int tests;
	static int errors;
	clunit( job_func_ptr job ) { get_jobs().push_back( job ); }
	static void add_todo( const char * todo, const char * file, int line );
	static void run();
	static std::ostream & tout();
	static void report();
	static void clear() { get_jobs().clear(); get_todos().clear(); }
};

#ifdef CLUNIT_HOME
	bool clunit::first = true;
	int clunit::tests = 0;
	int clunit::errors = 0;
	clunit::job_list & clunit::get_jobs()
	{
		static job_list jobs;
		return jobs;
	}
	clunit::todo_list & clunit::get_todos()
	{
		static todo_list todos;
		return todos;
	}
	std::ostream & clunit::tout()
	{
#ifdef CLUNIT_OUT
		static std::ofstream o_tout( CLUNIT_OUT );
#else
		static std::ofstream o_tout( "clunit.out" );
#endif
		if( first ) { time_t t=time(NULL); o_tout << ctime(&t) << '\n'; first = false; }
		return o_tout;
	}
	void clunit::add_todo( const char * todo, const char * file, int line )
	{
		std::ostringstream report;
		report << todo << " [" << file << ":" << line << "]";
		get_todos().push_back( report.str() );
	}
	void clunit::run()
	{
		{
		// The iostream (and possibly string) functions dynamically allocate memory
		// -the first time they are called.  They are not cleared until the program
		// -ends.  So that these allocations do not muck up the heap checking stats,
		// -dummy uses of the libraries are made so that they are initialised.  We
		// -can then checkpoint the heap after this point.
		std::ostrstream t1;
		t1 << "" << 12;
		std::ostringstream t2;
		t2 << "" << 12;
		tout() << "";
		}

		job_list::iterator task( get_jobs().begin() );
		while( task != get_jobs().end() )
		{ 
			_CrtMemState s1, s2, s3;
			// Store a memory checkpoint in the s1 memory-state structure
			_CrtMemCheckpoint( &s1 );

			try
			{
				(*task)(); 
			}
			catch(...)
			{
				TTEST( "Unhandled exception" == NULL );		// Force fail case
			}
			
			++task;

			// Store a 2nd memory checkpoint in s2
			_CrtMemCheckpoint( &s2 );
			TTEST( ! _CrtMemDifference( &s3, &s1, &s2 ) );
			if ( _CrtMemDifference( &s3, &s1, &s2 ) )
			{
				_CrtMemDumpStatistics( &s3 );
				_CrtMemDumpAllObjectsSince( &s1 );
			}
		}

		TTEST( _CrtCheckMemory() != 0 );
	}
	void clunit::report()
	{
		if( ! get_todos().empty() )
		{
			tout() << "TODOs (" << get_todos().size() << "):\n------------------------\n";
			OutputDebugString( "TODOs:\n------------------------\n" );
			for( todo_list::const_iterator i( get_todos().begin() ), i_end( get_todos().end() );
					i != i_end;
					++i )
			{
				tout() << "- " << (*i) << "\n";
				OutputDebugString( (std::string( "- ") + *i + "\n").c_str() );
			}
		}
		tout() << errors << " error(s) of " << tests << " test(s)\n";
		std::cout << errors << " error(s) of " << tests << " test(s)\n";
	}
#endif

// cl::clunit test
// To use cl::clunit, create a function in a test file that has a similar format to the 
// example below.  Then assign the pseudo return value of the function to a global 
// variable.  This will ensure that the test function is called before main is entered. 
// In main, call the TSUMMARY function.
//
// In the main test file, do #define CLUNIT_HOME to ensure the main function is include.
// If you wish to direct the test output to a different file, do:
//		 #define CLUNIT_OUT "tout.out"
// or similar
/* Example:

	void basic_test()
	{
		TINIT();						// Setup testing for function
		TDOC( Test tests );				// Do any documentation
		TSETUP( strlen( "This\n" ) );	// Do any lines needed to setup a test
		TTODO( "Need todo this" );		// Log any tests that need to be done
		TTODOX( a == b );				// Log a todo that is compilable but not trying to pass yet
		TTEST( 1 != 0 );				// Run tests
		TTEST( 2 == 2 );
		TTEST( (1 == 0) == false );
		return 0;						// Must return something!
	}

	static cl::clunit t1( basic_test );	// Ensure basic_test is registered for calling

	void main()
	{
		TRUNALL();						// Print final pass/fail result
	}
*/

} // End of namespace cl

#endif CLUNIT
