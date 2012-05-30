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
// - Neither the name Codalogic Ltd nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
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
// dsl-pa is a Domain Specific Language Parsing Assistant library designed to
// take advantage of the C++ logic shortcut operators such as && and ||.
// See the brief overview in dsl-pa.h or, for more information, README.html
// at https://github.com/codalogic/dsl-pa
//----------------------------------------------------------------------------

#ifndef CL_DSL_PA_DSL_PA
#define CL_DSL_PA_DSL_PA

#include <string>

#include "dsl-pa-reader.h"
#include "dsl-pa-alphabet.h"

namespace cl {

class dsl_pa_recoverable_exception : public std::exception
{
public:
    dsl_pa_recoverable_exception( const char * const what ) : std::exception( what )
    {}
};

class dsl_pa_fatal_exception : public std::exception
{
public:
    dsl_pa_fatal_exception( const char * const what ) : std::exception( what )
    {}
};

class dsl_pa
{
private:
    const static size_t unbounded = ~0;

    reader & r_reader;

public:
    dsl_pa( reader & r_reader_in ) : r_reader( r_reader_in ) {}
    virtual ~dsl_pa() {}

    // parse() provides a hook to allow use with factories that return
    // 'abstract' parsers.  If desired, implement parse() in a derived class to
    // initiate parsing.
    virtual bool parse() { return false; }

    // optional() essentially ignore the result of the function that generated
    // the input argument. e.g. allows optional( ws() ); etc.
    static bool optional( bool ) { return true; }
    static bool optional( size_t ) { return true; } // Overloads to avoid performance warnings due to convertin size_t to bool
    static bool optional( int ) { return true; }

    // optional_rewind() will call location_top() if the shortcut arguments
    // it is called with yield false.
    bool optional_rewind( bool is_ok ) { if( ! is_ok ) location_top(); return true; }

    // on_fail() allows inclusion of clean-up code in a short cut sequence
    // that is embedded in the parameters of an optional_rewind() call.
    // Do ...optioinal_rewind( XYZ || on_fail( ABC ) )...
    static bool on_fail( bool ) { return false; }
    static bool on_fail( size_t ) { return false; }
    static bool on_fail( int ) { return false; }

    // set() allows setting of state information within a set of
    // shortcut operators
    template< typename T >
    static bool set( T & r_variable, const T & r_value )
    {
        r_variable = r_value;
        return true;
    }
    // record() allows recording the return values of parsing functions in the
    // middle of a sequence of shortcut operators
    template< typename T, typename U >
    static const U & record( T & r_variable, const U & r_value )
    {
        r_variable = r_value;
        return r_value;
    }
    template< typename T >
    static bool clear( T & r_value )
    {
        r_value.clear();
        return true;
    }
    template< typename T >
    static bool append( T & r_variable, const T & r_value )
    {
        r_variable += r_value;
        return true;
    }
    template< typename T >
    static bool error( const T & r_exception )  // To throw custom exceptions
    {
        throw T( r_exception );
        return true;    // Won't be called!
    }
    static bool error( const char * p_what )
    {
        throw dsl_pa_recoverable_exception( p_what );
        return true;    // Won't be called!
    }
    static bool error_fatal( const char * p_what )
    {
        throw dsl_pa_fatal_exception( p_what );
        return true;    // Won't be called!
    }

    // The primary workhorse functions
    size_t /*num chars read*/ get( std::string * p_output, const alphabet & r_alphabet );
    size_t get( std::string * p_output, const alphabet & r_alphabet, size_t max_chars );
    size_t get_until( std::string * p_output, const alphabet & r_alphabet );
    size_t get_bounded_until( std::string * p_output, const alphabet & r_alphabet, size_t max_chars );
    size_t get_escaped_until( std::string * p_output, const alphabet & r_alphabet, char escape_char );
    size_t get_until( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars );

    // fixed() returns ensure that the specified text is read from the input, or leave input location unchanged.
    // ifixed() ignores ASCII case.
    bool fixed( const char * p_seeking );
    bool ifixed( const char * p_seeking );
    bool get_fixed( std::string * p_output, const char * p_seeking );
    bool get_ifixed( std::string * p_output, const char * p_seeking );

    // Type specific parsing functions
    bool space();
    bool opt_space() { return optional( space() ); }
    bool wsp();     // From ABNF (RFC5234) whitespace: non-newline space chars
    bool opt_wsp() { return optional( wsp() ); }

    bool /*is_parsed*/ get_bool( std::string * p_input );
    bool /*is_parsed*/ get_bool( bool * p_bool );
    size_t /*num chars read*/ get_int( std::string * p_num );
    size_t /*num chars read*/ get_int( int * p_int );
    size_t /*num chars read*/ get_uint( std::string * p_num );
    size_t /*num chars read*/ get_uint( unsigned int * p_int );
    bool /*is_parsed*/ get_float( std::string * p_num );
    bool /*is_parsed*/ get_float( double * p_float );
    bool /*is_parsed*/ get_float( float * p_float );
    bool /*is_parsed*/ get_sci_float( std::string * p_num );
    bool /*is_parsed*/ get_sci_float( double * p_float );
    bool /*is_parsed*/ get_sci_float( float * p_float );

    // Low-level reader access
    reader & get_reader() { return r_reader; }  // Primarily for use with location_logger class
    char get() { return r_reader.get(); }
    char current() const { return r_reader.current(); }
    bool unget() { r_reader.unget(); return true; }
    bool unget( char c ) { r_reader.unget( c ); return true; }
    char peek() { return r_reader.peek(); }
    bool is_char( char c ) { return r_reader.is_char( c ); }
    bool location_push() { return r_reader.location_push(); }   // See reader class for documentation
    bool location_top() { return r_reader.location_top(); }
    bool location_pop() { return r_reader.location_pop(); }
};

// The following strategy for handling an optional sequence does not work
// because C++ doesn't ensure that if you do optional_sequence().exec( xyz )
// the temporary object is created before the arguments to exec() are
// executed.
//
//class optional_sequence
//{
//private:
//    dsl_pa & r_dsl_pa;
//    bool is_location_popped;
//
//public:
//    optional_sequence( dsl_pa & r_dsl_pa_in )
//        :
//        r_dsl_pa( r_dsl_pa_in ),
//        is_location_popped( false )
//    {
//        r_dsl_pa.location_push();
//    }
//    ~optional_sequence()
//    {
//        // Just in case someone created the object (and pushed a location)
//        // but then forgot to call exec!
//        assert( is_location_popped );
//    }
//    bool exec( bool is_sequence_passed )
//    {
//        // N.B. The actual parsing sequence is run when creating the boolean
//        // value with which exec() is called!  i.e. after the object is created,
//        // but before exec() is called.
//        if( ! is_sequence_passed )
//            r_dsl_pa.location_top();
//        r_dsl_pa.location_pop();
//        is_location_popped = true;
//        return true;
//    }
//};

} // End of namespace cl

#endif // CL_DSL_PA_DSL_PA
