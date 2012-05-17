//----------------------------------------------------------------------
//                        Copyright (c) 2012 Codalogic.
//
// This file is currently not licensed for public use.
//----------------------------------------------------------------------

#ifndef CLUNIT
#define CLUNIT

#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include <string>
#include <ctime>
#include <crtdbg.h>

#define OutputDebugString  OutputDebugStringA
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA( const char * lpOutputString );

#define TINIT( x ) c_clunit::tout() << "\n\n    " << x << " (" << __FILE__ << ")\n    ==========================\n"; OutputDebugString( x "\n" )
#define TDOC( x ) c_clunit::tout() << "      " << x << "\n"
#define TTODO( x ) c_clunit::add_todo( x )
#define TTODOX( x ) { bool t=(x); c_clunit::add_todo( (std::string( #x ) + ((t)?" (passing)":" (failing)")).c_str() ); }
#define TSETUP( x ) c_clunit::tout() << "      : " << #x << '\n'; x
#define TTEST( x ) { bool t=(x); if( !(t) ) {c_clunit::tout() << "not "; ++c_clunit::errors;}else{c_clunit::tout()<<"    ";} ++c_clunit::tests; c_clunit::tout() << "ok: " << #x; if( !(t) ) c_clunit::tout() << " (" << __LINE__ << ")"; c_clunit::tout() << "\n"; }
#define TRUNALL() { c_clunit::run(); c_clunit::report(); if( c_clunit::errors > 255 ) return 255; return c_clunit::errors; }

class c_clunit
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
	c_clunit( job_func_ptr job ) { get_jobs().push_back( job ); }
	static void add_todo( const char * todo ) { get_todos().push_back( todo ); }
	static void run();
	static std::ostream & tout();
	static void report();
};

#ifdef CLUNIT_HOME
	bool c_clunit::first = true;
	int c_clunit::tests = 0;
	int c_clunit::errors = 0;
	c_clunit::job_list & c_clunit::get_jobs()
	{
		static job_list jobs;
		return jobs;
	}
	c_clunit::todo_list & c_clunit::get_todos()
	{
		static todo_list todos;
		return todos;
	}
	std::ostream & c_clunit::tout()
	{
#ifdef CLUNIT_OUT
		static std::ofstream o_tout( CLUNIT_OUT );
#else
		static std::ofstream o_tout( "clunit.out" );
#endif
		if( first ) { time_t t=time(NULL); o_tout << ctime(&t) << '\n'; first = false; }
		return o_tout;
	}
	void c_clunit::run()
	{
		{
		// The iostream (and possibly string) functions dynamically allocate memory
		// -the first time they are called.  They are not cleared until the program
		// -ends.  So that these allocations do not muck up the heap checking stats,
		// -dummy uses of the libraries are made so that they are initialised.  We
		// -can then checkpoint the heap after this point.
		std::ostrstream l_t1;
		l_t1 << "" << 12;
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
	void c_clunit::report()
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

// clunit test
// To use clunit, create a function in a test file that has a similar format to the 
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

	static c_clunit t1( basic_test );	// Ensure basic_test is registered for calling

	void main()
	{
		TRUNALL();						// Print final pass/fail result
	}
*/

#endif CLUNIT
