//----------------------------------------------------------------------------
// Copyright (c) 2012, Codalogic Ltd (http://www.codalogic.com)
// All rights reserved.
//
// The license for this file is based on the BSD-3-Clause license
// (http://www.opensource.org/licenses/BSD-3-Clause).
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// - Neither the name Codalogic nor the names of its contributors may be used
//   to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// cl::clunit, is a simple C++ unit testing framework.  It's USP is that it
// supports to-do statements so that you can easily record tests that you plan
// to do, but haven't implemented yet.  This allows you to plan ahead, but
// still only work on fixing one test at a time.
//
// A test function is created using the TFEATURE( "A descriptive string" )
// macro.  This represents the beginning of a test function that is
// automagically registered for execution when the tests are initiated.
//
/* For example:

example-test.cpp:
    #include "clunit.h"

    TFEATURE( "Example tests" )         // Register test with descriptive name
    {
        TDOC( "Test description" );     // Add any documentation (anywhere in function)
        TTOC( "Example description" );  // Output table of contents entry to toc file
        TSETUP( int t=1 );              // Do any lines needed to setup a test
        int b=1;                        // Use of TSETUP for test setup is optional
        TTODO( "Need todo this" );      // Log any tests that need to be done
        TTODON( 2, "Need todo this" );  // As above but with a depth indicator (i.e. 2) to help prioritise work
        TTODOX( t == b );               // Log a todo that is compilable but not trying to pass yet
        TTODOXN( 2, t == b );           // A version of TTODOX() with a depth indicator
        TDOC( "More description" );
        TTEST( 1 != 0 );                // Run a test
        TTESTN( 2, 1 != 0 );            // A version of TTEST() to mirror TTODOXN()
        TCRITICALTEST( 1 == 1 );        // Return from function immediately if test fails
        TCRITICALTESTN( 2, 1 == 1 );    // Version of TCRITICALTEST() with depth indicator
        TTESTFAIL( 1 != 0 );            // Fail a test that should pass - to check it's being run
        TTESTNFAIL( 1, 1 != 0 );        // TTESTN variant of TTFAIL()
        TCALL( func( 12, "y", "y" ) );  // Call a test function
    }
*/
// A note can be made of features that need to be tested using the TFEATTODO()
// macros.  These effectively create a test function containing a single
// TTODO() test description macro.
//
/* For example (not in a function):

    TFEATTODO( "Feature Todo" );        // A quick way of recording a feature that needs testing

    TFEATTODON( 2, "Feature N Todo" );  // As above but with a depth indicator
*/
//
// The tests are initiated by calling the TRUNALL(); macro in the test
// program's main() function.  Before doing #include "clunit.h" in the test
// file containing main(), be sure to #define CLUNIT_HOME first.
//
/* For example:

main-test.cpp:
    #define CLUNIT_HOME
    #include "clunit.h"

    int main()
    {
        TRUNALL();  // Run registered tests and print final pass/fail result
    }
*/
//
// By default the test output goes to "clunit.out". If you wish to direct
// the test output to a different file, do:
//       #define CLUNIT_OUT "tout.out"
// or similar.
//----------------------------------------------------------------------------

#ifndef CLUNIT
#define CLUNIT

#include <iostream>
#include <fstream>
#ifdef _MSC_VER
    #include <strstream>
#endif
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <cstring>

#ifdef _MSC_VER
    #include <crtdbg.h>
    extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA( const char * lpOutputString );
    #define TVS_DEBUG_CONSOLE_OUT( x ) OutputDebugStringA( (x).c_str() )
#else
    #define TVS_DEBUG_CONSOLE_OUT( x )
#endif

namespace cl {

#define TCAT( x, y ) TCAT2( x, y )
#define TCAT2( x, y ) x ## y

#define TFEATURE( d ) TFEATURE_IMPL( d, TCAT( test_func_, __LINE__ ) )
#define TFEATURE_IMPL( d, f ) static void f(); TREGISTERD( f, d ); void f()
#define TREGISTERD( f, d ) static cl::clunit f ## _registered_clunit_test( f, d, __FILE__, __LINE__ );
#define TFUNCTION( f ) static void f(); TREGISTER( f ); void f()
#define TREGISTER( f ) static cl::clunit f ## _registered_clunit_test( f );
#define TBEGIN( d ) cl::clunit::tbegin( d, __FILE__, __LINE__ )
#define TTOC( d ) cl::clunit::ttoc( d, __FILE__, __LINE__ )
#define TDOC( d ) cl::clunit::tdoc( d )
#define TSETUP( x ) cl::clunit::tsetup_log( #x ); x
#define TTODO( d ) cl::clunit::ttodo( d, __FILE__, __LINE__ )
#define TTODON( n, d ) cl::clunit::ttodo( "[" #n "] " d, __FILE__, __LINE__ )
#define TTODOX( x ) { cl::clunit::ttodox( #x, (x), __FILE__, __LINE__ ); }
#define TTODOXN( n, x ) { cl::clunit::ttodox( "[" #n "] " #x, (x), __FILE__, __LINE__ ); }
#define TTEST( x ) { cl::clunit::ttest( #x, (x), __FILE__, __LINE__ ); }
#define TTESTN( n, x ) TTEST( x )
#define TTESTFAIL( x ) { cl::clunit::ttestfail( #x, (x), __FILE__, __LINE__ ); }
#define TTESTNFAIL( n, x ) TTESTFAIL( x )
#define TCRITICALTEST( x ) { if( ! cl::clunit::ttest( #x, (x), __FILE__, __LINE__ ) ) return; }
#define TCRITICALTESTN( n, x ) TCRITICALTEST( x )
#define TCALL( x ) { cl::clunit::tcall( #x, __FILE__, __LINE__ ); (x); }
#define TFEATURETODO( d ) TFUNCTION( TCAT( todo_function_, __LINE__ ) ) { TTODO( d ); }
#define TFEATURETODON( n, d ) TFUNCTION( TCAT( todo_function_, __LINE__ ) ) { TTODON( n, d ); }
#define TRUNALL() { cl::clunit::run(); size_t n_errors = cl::clunit::report(); if( n_errors > 255 ) return 255; return n_errors; }

typedef void(*job_func_ptr)();
struct job_description
{
    job_func_ptr job;
    const char * p_description;
    const char * p_file;
    int line;
    job_description( job_func_ptr job_in )
        : job( job_in ), p_description( 0 ), p_file( 0 ), line( 0 )
    {}
    job_description( job_func_ptr job_in, const char * p_description_in,
            const char * p_file_in, int line_in )
        : job( job_in ), p_description( p_description_in ),
            p_file( p_file_in ), line( line_in )
    {}
};
typedef std::vector< job_description > job_list;

class fixed_size_log
{
    // We want to log TODO operations while carrying out tests, but also
    // check for memory leaks.  Therefore memory for logging can't be
    // allocated during the testing process.  Therefore this class allocates
    // a chunk of memory up front and we log into that until it is full.
private:
    std::string log;
    size_t n_items_logged;
    bool is_log_full;

public:
    fixed_size_log( size_t size )
        :
        n_items_logged( 0 ),
        is_log_full( false )
    {
        log.reserve( size );
    }
    void insert( const std::string & new_entry )
    {
        ++n_items_logged;
        if( ! is_log_full )
        {
            if( log.size() + new_entry.size() < log.capacity() )
                log += new_entry;
            else
                is_log_full = true;
        }
    }
    const std::string & get() const { return log; }
    size_t size() const { return n_items_logged; }
    bool empty() const { return  n_items_logged == 0; }
};

class clunit
{
private:
    class singleton
    {
    private:
        bool is_first_tout;
        bool is_new_tout_section;
        bool is_new_print_all_section;
        int n_tests;
        int n_errors;
        int n_forced_fails_invoked;
        int n_forced_fails_occurred;
        bool is_first_ttoc;
        const char * p_current_test_file;
        fixed_size_log todo_log;

        job_list & get_jobs();
        std::ostream & tout();
        std::ostream & ttocout();

    public:
        singleton()
            :
            is_first_tout( true ),
            is_new_tout_section( false ),
            is_new_print_all_section( false ),
            n_tests( 0 ),
            n_errors( 0 ),
            n_forced_fails_invoked( 0 ),
            n_forced_fails_occurred( 0 ),
            is_first_ttoc( true ),
            p_current_test_file( 0 ),
            todo_log( 10000 )
        {}

        void tregister( job_func_ptr job )
        {
            get_jobs().push_back( job_description( job ) );
        }
        void tregister( job_func_ptr job, const char * p_description,
                const char * p_file, int line )
        {
            get_jobs().push_back(
                    job_description( job, p_description, p_file, line ) );
        }
        void tbegin( const char * what, const char * file, int line )
        {
            std::string indent( "    " );
            std::ostringstream documentation;
            documentation << what << " [" << file_base( file ) << ":" << line << "]";
            std::string heading( documentation.str() );
            std::string underline( heading.size(), '=' );
            print_to_all_outputs( indent + heading + "\n" + indent + underline + "\n" );
            ttoc( what, file, line);
        }
        void ttoc( const char * what, const char * file, int line )
        {
            if( ! p_current_test_file || strcmp( p_current_test_file, file ) != 0 )
            {
                ttocout() << "\n";
                ttocout() << "# " << file_base( file ) << "\n";
                ttocout() << "| Description | Line |\n";
                ttocout() << "|-------------|------|\n";
                p_current_test_file = file;
            }
            ttocout() << "| " << what << " | " << line << " |\n";
        }
        void tdoc( const char * what )
        {
            tout() << "      " << what << "\n";
        }
        void tsetup_log( const char * what )
        {
            tout() << "      : " << what << '\n';
        }
        void ttodo( const char * what, const char * file, int line )
        {
            std::ostringstream report;
            report << "- " << what << " [" << file_base( file ) << ":" << line << "]\n";
            todo_log.insert( report.str() );
        }
        void ttodox( const char * what, bool is_passed, const char * file, int line )
        {
            std::ostringstream report;
            report << "- " <<
                    what << ((is_passed)?" (passing)":" (failing)") <<
                    " [" << file_base( file ) << ":" << line << "]\n";
            todo_log.insert( report.str() );
        }
        bool ttest( const char * what, bool is_passed, const char * file, int line )
        {
            std::ostringstream report;
            if( ! is_passed )
            {
                report << "not ";
                ++n_errors;
            }
            else
            {
                report << "    ";
            }
            ++n_tests;
            report << "ok: " << what;
            if( ! is_passed )
                report << " (" << line << ")";
            report << "\n";
            if( is_passed )
                tout() << report.str();
            else
                print_to_all_outputs( report.str() );
            return is_passed;
        }
        bool ttestfail( const char * what, bool is_passed, const char * file, int line )
        {
            ++n_forced_fails_invoked;
            int n_errors_at_beginning = n_errors;
            bool result = ttest( what, ! is_passed, file, line );
            n_forced_fails_occurred += n_errors - n_errors_at_beginning;
            return result;
        }
        void tcall( const char * what, const char * file, int line )
        {
            tout() << "      Calling: " << what << " (" << line << ")" << "\n";
        }
        void run()
        {
            {
            // The iostream (and possibly string) functions dynamically allocate memory
            // -the first time they are called.  They are not cleared until the program
            // -ends.  So that these allocations do not muck up the heap checking stats,
            // -dummy uses of the libraries are made so that they are initialised.  We
            // -can then checkpoint the heap after this point.
            #ifdef _MSC_VER
                std::ostrstream t1;
                t1 << "" << 12;
            #endif
            std::ostringstream t2;
            t2 << "" << 12;
            tout() << "";
            }

            for( job_list::const_iterator job( get_jobs().begin() ), job_end( get_jobs().end() );
                    job != job_end;
                    ++job )
            {
#if defined( _MSC_VER ) && defined( _DEBUG )
                _CrtMemState s1, s2, s3;
                // Store a memory checkpoint in the s1 memory-state structure
                _CrtMemCheckpoint( &s1 );
#endif

                try
                {
                    is_new_tout_section = is_new_print_all_section = true;
                    if( job->p_description )
                        tbegin( job->p_description, job->p_file, job->line );
                    job->job();
                }
                catch(...)
                {
                    TTEST( "Unhandled exception" == NULL );     // Force fail case
                }

#if defined( _MSC_VER ) && defined( _DEBUG )
                // Store a 2nd memory checkpoint in s2
                _CrtMemCheckpoint( &s2 );
                TTEST( ! _CrtMemDifference( &s3, &s1, &s2 ) );
                if ( _CrtMemDifference( &s3, &s1, &s2 ) )
                {
                    _CrtMemDumpStatistics( &s3 );
                    _CrtMemDumpAllObjectsSince( &s1 );
                }
#endif
            }

#if defined( _MSC_VER ) && defined( _DEBUG )
            TTEST( _CrtCheckMemory() != 0 );
#endif
        }
        size_t report()
        {
            if( ! todo_log.empty() )
            {
                std::ostringstream todo_report;
                todo_report <<
                        "\nTODOs (" <<
                        todo_log.size() <<
                        "):\n------------------------\n" <<
                        todo_log.get() <<
                        "\n";
                print_to_all_outputs( todo_report.str() );
            }
            std::ostringstream summary;
            summary <<
                    n_errors << " error(s), " <<
                    todo_log.size() << " todo(s), " <<
                    n_tests << " test(s)\n";
            if( n_forced_fails_invoked > 0 )
                summary <<
                        "FORCED FAILURES: " <<
                            n_forced_fails_invoked << " invoked, " <<
                            n_forced_fails_occurred << " occurred\n";
            print_to_all_outputs( summary.str() );
            return n_errors;
        }
        void print_to_all_outputs( const std::string & message )
        {
            if( is_new_print_all_section )
            {
                is_new_print_all_section = false;
                std::cout << "\n";
                TVS_DEBUG_CONSOLE_OUT( std::string( "\n" ) );
            }
            tout() << message;
            std::cout << message;
            TVS_DEBUG_CONSOLE_OUT( message );
        }
        void clear() { get_jobs().clear(); }
    private:
        static const char * file_base( const char * p_file )
        {
            const char * p_base = p_file;
            for( const char * p_seek = "/\\:"; *p_seek; ++p_seek )
            {
                const char * p_found = strrchr( p_base, *p_seek );
                if( p_found && *(p_found + 1) )
                    p_base = p_found + 1;
            }
            return p_base;
        }
    };

    static singleton my_singleton;

public:
    clunit( job_func_ptr job )
        { tregister( job ); }
    clunit( job_func_ptr job, const char * p_description,
                const char * p_file, int line )
        { tregister( job, p_description, p_file, line ); }
    static void tregister( job_func_ptr job )
        { my_singleton.tregister( job ); }
    static void tregister( job_func_ptr job, const char * p_description,
                const char * p_file, int line )
        { my_singleton.tregister( job, p_description, p_file, line ); }
    static void tbegin( const char * what, const char * file, int line )
        { my_singleton.tbegin( what, file, line ); }
    static void ttoc( const char * what, const char * file, int line )
        { my_singleton.ttoc( what, file, line ); }
    static void tdoc( const char * what )
        { my_singleton.tdoc( what ); }
    static void tsetup_log( const char * what )
        { my_singleton.tsetup_log( what ); }
    static void ttodo( const char * what, const char * file, int line )
        { my_singleton.ttodo( what, file, line ); }
    static void ttodox( const char * what, bool is_passed, const char * file, int line )
        { my_singleton.ttodox( what, is_passed, file, line ); }
    static bool ttest( const char * what, bool is_passed, const char * file, int line )
        { return my_singleton.ttest( what, is_passed, file, line ); }
    static bool ttestfail( const char * what, bool is_passed, const char * file, int line )
        { return my_singleton.ttestfail( what, is_passed, file, line ); }
    static void tcall( const char * what, const char * file, int line )
        { my_singleton.tcall( what, file, line ); }
    static void run()
        { my_singleton.run(); }
    static size_t report()
        { return my_singleton.report(); }
    static void clear()
        { my_singleton.clear(); }
};

#ifdef CLUNIT_MAIN
#define CLUNIT_HOME
    } // End of namespace cl

    int main( int argc, char * argv[] )
    {
        TRUNALL();
    }

    namespace cl {
#endif

#ifdef CLUNIT_HOME
    clunit::singleton clunit::my_singleton;
    job_list & clunit::singleton::get_jobs()
    {
        static job_list jobs;
        return jobs;
    }
    std::ostream & clunit::singleton::tout()
    {
#ifdef CLUNIT_OUT
        static std::ofstream os( CLUNIT_OUT );
#else
        static std::ofstream os( "clunit.out" );
#endif
        if( is_first_tout )
        {
            time_t t=time(NULL);
            os << "Tests run on " << ctime(&t);
            is_first_tout = false;
        }
        if( is_new_tout_section )
        {
            is_new_tout_section = false;
            os << "\n";
        }
        return os;
    }
    std::ostream & clunit::singleton::ttocout()
    {
        static std::ofstream os( "clunit-toc.md" );
        if( is_first_ttoc )
        {
            time_t t=time(NULL);
            os << "Tests table of contents generated on " << ctime(&t);
            is_first_ttoc = false;
        }
        return os;
    }
#endif

} // End of namespace cl

#endif // CLUNIT
