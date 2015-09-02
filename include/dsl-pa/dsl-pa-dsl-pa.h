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

class dsl_pa_exception : public std::exception
{
public:
    dsl_pa_exception( const char * const what ) : std::exception( what )
    {}
};

class dsl_pa_recoverable_exception : public dsl_pa_exception
{
public:
    dsl_pa_recoverable_exception( const char * const what ) : dsl_pa_exception( what )
    {}
};

class dsl_pa_fatal_exception : public dsl_pa_exception
{
public:
    dsl_pa_fatal_exception( const char * const what ) : dsl_pa_exception( what )
    {}
};

class dsl_pa
{
private:
    const static size_t unbounded = ~0;

    reader & r_reader;

    template< typename Twriter >
    size_t read_or_skip_handler( std::string * p_output, const alphabet & r_alphabet, size_t max_chars );
    template< typename Twriter >
    size_t read_or_skip_until_handler( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars );
    template< class Tcomparer >
    bool read_fixed_or_ifixed( std::string * p_output, const char * p_seeking );

public:
    dsl_pa( reader & r_reader_in ) : r_reader( r_reader_in ) {}
    virtual ~dsl_pa() {}

    // parse() provides a hook to allow use with factories that return
    // 'abstract' parsers.  If desired, implement parse() in a derived class to
    // initiate parsing.
    virtual bool parse() { return false; }

    // Methods useful for deciding branches to take in BNF description
    bool /*is_not_eof*/ get_char();     // Use current() method to access value
    bool /*is_not_eof*/ get_char_no_space();     // Use current() method to access value
    bool /*is_not_eof*/ get_char_no_wsp();       // Use current() method to access value
    bool /*is_not_eof*/ peek_char();    // Use current(), current_is() and current_is_in() methods to access value
    bool /*is_not_eof*/ peek_char_no_space();     // Use current(), current_is() and current_is_in() methods to access value
    bool /*is_not_eof*/ peek_char_no_wsp();       // Use current(), current_is() and current_is_in() methods to access value
    bool is_get_char( char c ) { return r_reader.is_get_char( c ); }    // Will unget() if got char is not c
    bool current_is( int c ) const;
    bool current_is_in( const alphabet & r_alphabet ) const;
    bool peek_is( int c );
    bool peek_is_in( const alphabet & r_alphabet );
    bool is_current_at_end() { return r_reader.is_current_at_end(); }
    bool is_peek_at_end() { return r_reader.is_peek_at_end(); }

    // Type specific parsing functions
    size_t space();
    bool opt_space() { return optional( space() ); }
    size_t wsp();     // From ABNF (RFC5234) whitespace: non-newline space chars
    bool opt_wsp() { return optional( wsp() ); }

    bool /*is_not_eof*/ get_char( std::string * p_input );
    bool /*is_not_eof*/ read_char( std::string * p_input );
    bool /*is_not_eof*/ get_char( int * p_char );
    bool /*is_parsed*/ get_bool( std::string * p_input );
    bool /*is_parsed*/ read_bool( std::string * p_input );
    bool /*is_parsed*/ get_bool( bool * p_bool );
    size_t /*num chars read*/ get_int( std::string * p_num );
    size_t /*num chars read*/ read_int( std::string * p_num );
    size_t /*num chars read*/ get_int( int * p_int );
    size_t /*num chars read*/ get_uint( std::string * p_num );
    size_t /*num chars read*/ read_uint( std::string * p_num );
    size_t /*num chars read*/ get_uint( unsigned int * p_int );
    bool /*is_parsed*/ get_float( std::string * p_num );
    bool /*is_parsed*/ read_float( std::string * p_num );
    bool /*is_parsed*/ get_float( double * p_float );
    bool /*is_parsed*/ get_float( float * p_float );
    bool /*is_parsed*/ get_sci_float( std::string * p_num );
    bool /*is_parsed*/ read_sci_float( std::string * p_num );
    bool /*is_parsed*/ get_sci_float( double * p_float );
    bool /*is_parsed*/ get_sci_float( float * p_float );

    // The primary workhorse functions
    // These get...() functions clear the output string before reading the input
    size_t /*num chars read*/ get( std::string * p_output, const alphabet & r_alphabet );
    size_t get( std::string * p_output, const alphabet & r_alphabet, size_t max_chars );
    size_t get_until( std::string * p_output, const alphabet & r_alphabet );
    size_t get_bounded_until( std::string * p_output, const alphabet & r_alphabet, size_t max_chars );
    size_t get_escaped_until( std::string * p_output, const alphabet & r_alphabet, char escape_char );
    size_t get_until( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars );

    // These read...() functions DO NOT clear the output string before reading the input
    size_t /*num chars read*/ read( std::string * p_output, const alphabet & r_alphabet );
    size_t read( std::string * p_output, const alphabet & r_alphabet, size_t max_chars );
    size_t read_until( std::string * p_output, const alphabet & r_alphabet );
    size_t read_bounded_until( std::string * p_output, const alphabet & r_alphabet, size_t max_chars );
    size_t read_escaped_until( std::string * p_output, const alphabet & r_alphabet, char escape_char );
    size_t read_until( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars );

    size_t /*num chars skipped*/ skip( const alphabet & r_alphabet );
    size_t skip( const alphabet & r_alphabet, size_t max_chars );
    size_t skip_until( const alphabet & r_alphabet );
    size_t skip_bounded_until( const alphabet & r_alphabet, size_t max_chars );
    size_t skip_escaped_until( const alphabet & r_alphabet, char escape_char );
    size_t skip_until( const alphabet & r_alphabet, char escape_char, size_t max_chars );

    // fixed() ensures that the specified text is read from the input, or leave input location unchanged.
    // ifixed() ignores ASCII case.
    bool fixed( const char * p_seeking );
    bool ifixed( const char * p_seeking );
    bool get_fixed( std::string * p_output, const char * p_seeking );
    bool get_ifixed( std::string * p_output, const char * p_seeking );
    bool read_fixed( std::string * p_output, const char * p_seeking );
    bool read_ifixed( std::string * p_output, const char * p_seeking );

    // Low-level reader access
    reader & get_reader() { return r_reader; }  // Primarily for use with location_logger class
    char get() { return r_reader.get(); }
    char current() const { return r_reader.current(); }
    bool unget() { r_reader.unget(); return true; }
    bool unget( char c ) { r_reader.unget( c ); return true; }
    char peek() { return r_reader.peek(); }

    // See class reader for documentation.  A typical code sequence might be:
    // location_push(); path_a() || location_top() && path_b(); location_pop();
    void location_push() { r_reader.location_push(); }
    bool location_top() { r_reader.location_top(); return true; }
    bool location_top( bool ret ) { r_reader.location_top(); return ret; }
    void location_pop() { r_reader.location_pop(); }    // pop should always be called, so discourage it's use in a shortcut sequence

    // optional_rewind() will call location_top() if the shortcut arguments
    // it is called with yield false.  Used when the specified path is determined
    // not to be the one encountered.
    // Do location_push(); optional_rewind( XYZ() && ABC() ) && DEF(); location_pop()
    bool optional_rewind( bool is_ok ) { if( ! is_ok ) location_top(); return is_ok; }

    // optional() essentially ignore the result of the (single) function that
    // generated the input argument. e.g. allows optional( space() ); etc.
    static bool optional( bool ) { return true; }
    static bool optional( size_t ) { return true; } // Overloads to avoid performance warnings due to converting size_t to bool
    static bool optional( int ) { return true; }

    // on_fail() allows inclusion of clean-up code in a short cut sequence
    // that is embedded in the parameters of an optional_rewind() call.
    // Do ...optional_rewind( XYZ() && ABC() || on_fail( DEF() && GHI() ) )...
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
    template< typename Tvar, typename Tval >
    static bool append( Tvar & r_variable, const Tval & r_value )
    {
        r_variable += r_value;
        return true;
    }
    template< typename Tvar >
    bool append_current( Tvar & r_variable )
    {
        r_variable += current();
        return true;
    }

    template< typename T >
    static bool error( const T & r_exception )  // To throw custom exceptions
    {
        throw T( r_exception );
        return false;    // Won't be called!
    }
    static bool error( const char * p_what )
    {
        throw dsl_pa_recoverable_exception( p_what );
        return false;    // Won't be called!
    }
    static bool error_fatal( const char * p_what )
    {
        throw dsl_pa_fatal_exception( p_what );
        return false;    // Won't be called!
    }
    template< typename T >
    static bool retreat()   // Throw the specified exception in order to resume parsing at a different location in the parser
    {
        throw T();
        return false;    // Won't be called!
    }
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
