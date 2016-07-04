//----------------------------------------------------------------------
// Copyright (c) 2012-2016, Codalogic Ltd (http://www.codalogic.com)
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
// - Neither the name Codalogic nor the names of its contributors may be used
//   to endorse or promote products derived from this software without
//   specific prior written permission.
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

#include "clunit.h"

#include "dsl-pa/dsl-pa.h"

using namespace cl;

static bool is_false_optional_test_helper()
{
    return false;
}

static bool is_true_optional_test_helper()
{
    return true;
}

static size_t size_0_optional_test_helper()
{
    return 0;
}

static size_t size_12_optional_test_helper()
{
    return 12;
}

static size_t int_minus_2_optional_test_helper()
{
    return -2;
}

TFUNCTION( dsl_pa_optional_test )
{
    TBEGIN( "dsl_pa::optional() operation" );

    TTEST( dsl_pa::optional( is_false_optional_test_helper() ) );
    TTEST( dsl_pa::optional( is_true_optional_test_helper() ) );
    TTEST( dsl_pa::optional( size_0_optional_test_helper() ) );
    TTEST( dsl_pa::optional( size_12_optional_test_helper() ) );
    TTEST( dsl_pa::optional( int_minus_2_optional_test_helper() ) );
}

TFUNCTION( dsl_pa_set_test )
{
    TBEGIN( "dsl_pa::set() operation" );

    TSETUP( int i = 0 );
    TSETUP( dsl_pa::set( i, 1 ) );
    TTEST( i == 1 );

    enum { HOT, COLD } e = HOT;
    TSETUP( dsl_pa::set( e, COLD ) );
    TTEST( e == COLD );
}

TFUNCTION( dsl_pa_record_test )
{
    TBEGIN( "dsl_pa::record() operation" );

    TSETUP( bool t1 = false );
    TSETUP( dsl_pa::record( t1, true ) );
    TTEST( t1 == true );

    TSETUP( bool t2 = true );
    TSETUP( dsl_pa::record( t2, false ) );
    TTEST( t2 == false );
}

TFUNCTION( dsl_pa_clear_test )
{
    TBEGIN( "dsl_pa::clear() operation" );

    std::string v( "abc" );
    TTEST( ! v.empty() );
    TTEST( dsl_pa::clear( v ) );    // Test to ensure it returns true
    TTEST( v.empty() );
}

TFUNCTION( dsl_pa_append_test )
{
    TBEGIN( "dsl_pa::append() operation" );

    std::string v( "abc" );
    TTEST( ! v.empty() );
    TTEST( dsl_pa::append( v, "def" ) );    // Test to ensure it returns true
    TTEST( v == "abcdef" );
    TTEST( dsl_pa::append( v, 'g' ) );    // Test to ensure it returns true
    TTEST( v == "abcdefg" );
}

TFUNCTION( dsl_pa_append_current_test )
{
    TBEGIN( "dsl_pa::append_current() operation" );

    reader_string my_reader( "abcdef" );
    dsl_pa my_dsl_pa( my_reader );

    TTEST( my_dsl_pa.get() == 'a' );

    std::string v( "abc" );
    TTEST( my_dsl_pa.append_current( v ) );    // Test to ensure it returns true
    TTEST( v == "abca" );
}

TFUNCTION( dsl_pa_push_back_test )
{
    TBEGIN( "dsl_pa::push_back() operation" );

    std::vector<char> v;

    TTEST( dsl_pa::push_back( v, 'a' ) );    // Test to ensure it returns true

    TTEST( v.size() == 1 && v[0] == 'a' );
}

TFUNCTION( dsl_pa_error_test )
{
    TBEGIN( "dsl_pa::error() operation" );

    {
    bool is_thrown = false;
    try
    {
        dsl_pa::error( "Expected other input" );
    }
    catch( const dsl_pa_recoverable_exception & )
    {
        is_thrown = true;
    }
    TTEST( is_thrown );
    }

    {
    bool is_thrown = false;
    try
    {
        dsl_pa::error_fatal( "Expected other input" );
    }
    catch( const dsl_pa_fatal_exception & )
    {
        is_thrown = true;
    }
    TTEST( is_thrown );
    }

    {
    class my_exception
    {
    private:
        int error_code;
    public:
        my_exception( int error_code_in ) : error_code( error_code_in ) {}
        int get_code() const { return error_code; }
    };

    bool is_thrown = false;
    try
    {
        dsl_pa::error( my_exception( 12 ) );
    }
    catch( const my_exception & r_exception )
    {
        is_thrown = true;
        TTEST( r_exception.get_code() == 12 );
    }
    TTEST( is_thrown );
    }
}

TFUNCTION( dsl_pa_location_top_return_codes )
{
    TBEGIN( "dsl pa location_top() return codes" );

    reader_string my_reader( "abcdef" );
    dsl_pa my_dsl_pa( my_reader );

    my_dsl_pa.location_push();
    TTEST( my_dsl_pa.location_top() );
    TTEST( ! my_dsl_pa.location_top( false ) );
    TTEST( my_dsl_pa.location_top( true ) );
    my_dsl_pa.location_pop();
}

TFUNCTION( dsl_pa_location_revise )
{
    TBEGIN( "dsl pa location_revise()" );

    reader_string my_reader( "abcdefg" );
    dsl_pa my_dsl_pa( my_reader );

    TTEST( my_dsl_pa.get() == 'a' );
    TTEST( my_dsl_pa.get() == 'b' );
    my_dsl_pa.location_push();
    TTEST( my_dsl_pa.get() == 'c' );
    TTEST( my_dsl_pa.get() == 'd' );
    TTEST( my_dsl_pa.location_revise() );
    TTEST( my_dsl_pa.get() == 'e' );
    TTEST( my_dsl_pa.get() == 'f' );
    TTEST( my_dsl_pa.location_top() );
    TTEST( my_dsl_pa.get() == 'e' );
    TTEST( my_dsl_pa.get() == 'f' );
    my_dsl_pa.location_pop();
}

TFUNCTION( dsl_pa_low_level_read_test )
{
    TBEGIN( "dsl pa low level read tests" );

    reader_string my_reader( "abcdef" );
    dsl_pa my_dsl_pa( my_reader );

    TTEST( my_dsl_pa.get() == 'a' );
    TTEST( my_dsl_pa.get() == 'b' );
    TTEST( my_dsl_pa.current() == 'b' );
    TTEST( my_dsl_pa.current() == 'b' );
    my_dsl_pa.location_push();
    TTEST( my_dsl_pa.unget() );         // Check true is returned
    TTEST( my_dsl_pa.get() == 'b' );
    TTEST( my_dsl_pa.unget( 'l' ) );    // Check true is returned
    TTEST( my_dsl_pa.get() == 'l' );
    TTEST( my_dsl_pa.peek() == 'c' );
    TTEST( my_dsl_pa.get() == 'c' );
    TTEST( my_dsl_pa.get() == 'd' );
    TTEST( my_dsl_pa.get() == 'e' );
    TTEST( my_dsl_pa.location_top() );
    TTEST( my_dsl_pa.get() == 'c' );
    my_dsl_pa.location_pop();

    {
    TSETUP( locator my_location_logger( my_dsl_pa.get_reader() ) );
    TTEST( ! my_dsl_pa.is_get_char( 'x' ) );
    TTEST( my_dsl_pa.is_get_char( 'd' ) );
    TTEST( my_dsl_pa.get() == 'e' );
    TTEST( my_dsl_pa.location_top() );
    TTEST( my_dsl_pa.get() == 'd' );
    }
}

TFUNCTION( dsl_pa_ws_test )
{
    TBEGIN( "dsl pa ws tests" );

    {
    reader_string my_reader( "ab  cdef" );
    dsl_pa pa( my_reader );

    TTEST( pa.get() == 'a' && pa.get() == 'b' && pa.space() );

    TTEST( pa.get() == 'c' );
    }

    {
    reader_string my_reader( "abcdef" );
    dsl_pa pa( my_reader );

    TTEST( pa.get() == 'a' && pa.get() == 'b' );
    TTEST( ! pa.space() );

    TTEST( pa.get() == 'c' );
    }

    {
    reader_string my_reader( "abcdef" );
    dsl_pa pa( my_reader );

    TTEST( pa.get() == 'a' && pa.get() == 'b' && pa.opt_space() );

    TTEST( pa.get() == 'c' );
    }
}

TFUNCTION( dsl_pa_wsp_test )
{
    TBEGIN( "dsl pa wsp tests" );

    {
    reader_string my_reader( "ab  cdef" );
    dsl_pa pa( my_reader );

    TTEST( pa.get() == 'a' && pa.get() == 'b' && pa.wsp() );

    TTEST( pa.get() == 'c' );
    }

    {
    reader_string my_reader( "abcdef" );
    dsl_pa pa( my_reader );

    TTEST( pa.get() == 'a' && pa.get() == 'b' );
    TTEST( ! pa.wsp() );

    TTEST( pa.get() == 'c' );
    }

    {
    reader_string my_reader( "abcdef" );
    dsl_pa pa( my_reader );

    TTEST( pa.get() == 'a' && pa.get() == 'b' && pa.opt_wsp() );

    TTEST( pa.get() == 'c' );
    }

    {
    reader_string my_reader( "ab  \ncdef" );
    dsl_pa pa( my_reader );

    TTEST( pa.get() == 'a' && pa.get() == 'b' && pa.wsp() );

    TTEST( pa.get() == '\n' && pa.get() == 'c' );
    }

    {
    reader_string my_reader( "ab  \ncdef" );
    dsl_pa pa( my_reader );

    TTEST( pa.get() == 'a' && pa.get() == 'b' && pa.opt_wsp() );

    TTEST( pa.get() == '\n' && pa.get() == 'c' );
    }

    {
    reader_string my_reader( "ab\ncdef" );
    dsl_pa pa( my_reader );

    TTEST( pa.get() == 'a' && pa.get() == 'b' && pa.opt_wsp() );

    TTEST( pa.get() == '\n' && pa.get() == 'c' );
    }
}

TFUNCTION( dsl_pa_get_test )
{
    TBEGIN( "dsl pa get() tests" );

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;
    std::string channel;
    std::string level;

    TTEST( my_pa.get( &command, alphabet_word_char() ) &&
                my_pa.space() &&
                my_pa.get( &channel, alphabet_alpha() ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '=' ) &&
                my_pa.opt_space() &&
                my_pa.get( &level, alphabet_digit() ) );

    TTEST( command == "Mode" );
    TTEST( channel == "p" );
    TTEST( level == "12" );
    }

    {
    reader_string my_reader( "  Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;

    TTEST( my_pa.opt_space() && my_pa.get( &command, alphabet_word_char() ) == 4 );

    TTEST( command == "Mode" );
    }

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command_1;
    std::string command_2;
    std::string channel;
    std::string level;

    TTEST( my_pa.get( &command_1, alphabet_word_char(), 2 ) &&
                my_pa.get( &command_2, alphabet_word_char() ) &&
                my_pa.space() &&
                my_pa.get( &channel, alphabet_alpha() ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '=' ) &&
                my_pa.opt_space() &&
                my_pa.get( &level, alphabet_digit() ) );

    TTEST( command_1 == "Mo" );
    TTEST( command_2 == "de" );
    TTEST( channel == "p" );
    TTEST( level == "12" );
    }

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command_1;
    std::string command_2;
    std::string channel;
    std::string level;

    TTEST( my_pa.get( &command_1, alphabet_word_char(), 2 ) &&
                my_pa.get( &command_2, alphabet_word_char() ) &&
                my_pa.space() &&
                my_pa.get( &channel, alphabet_alpha() ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char_in( alphabet_char_class( "<=" ) ) &&
                my_pa.opt_space() &&
                my_pa.get( &level, alphabet_digit() ) );

    TTEST( command_1 == "Mo" );
    TTEST( command_2 == "de" );
    TTEST( channel == "p" );
    TTEST( level == "12" );
    }
}

TFUNCTION( dsl_pa_get_until_test )
{
    TBEGIN( "dsl pa get_until() tests" );

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;
    std::string channel;
    std::string level;

    TTEST( my_pa.get_until( &command, alphabet_space() ) &&
                my_pa.space() &&
                my_pa.get_until( &channel, alphabet_char_class( "=" ) ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '=' ) &&
                my_pa.opt_space() &&
                my_pa.get_until( &level, alphabet_space() ) );

    TTEST( command == "Mode" );
    TTEST( channel == "p" );
    TTEST( level == "12" );
    }

    {
    reader_string my_reader( "  Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;

    TTEST( my_pa.opt_space() && my_pa.get_until( &command, alphabet_space() ) == 4 );

    TTEST( command == "Mode" );
    }

    {
    reader_string my_reader( "p='He\\'s got it\\n all' 12 " );
    dsl_pa my_pa( my_reader );

    std::string channel;
    std::string sentence;
    std::string level;

    TTEST( my_pa.get_until( &channel, alphabet_char_class( "=" ) ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '=' ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '\'' ) &&
                my_pa.get_escaped_until( &sentence, alphabet_char_class( "'" ), '\\' ) &&
                my_pa.is_get_char( '\'' ) &&
                my_pa.space() &&
                my_pa.get_until( &level, alphabet_space() ) );

    TTEST( channel == "p" );
    TTEST( sentence == "He's got it\\n all" );
    TTEST( level == "12" );
    }

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command_1;
    std::string command_2;
    std::string channel;
    std::string level;

    TTEST( my_pa.get_bounded_until( &command_1, alphabet_space(), 2 ) &&
                my_pa.get_until( &command_2, alphabet_space() ) &&
                my_pa.space() &&
                my_pa.get_until( &channel, alphabet_char_class( "=" ) ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '=' ) &&
                my_pa.opt_space() &&
                my_pa.get_until( &level, alphabet_space() ) );

    TTEST( command_1 == "Mo" );
    TTEST( command_2 == "de" );
    TTEST( channel == "p" );
    TTEST( level == "12" );
    }
}

TFUNCTION( dsl_pa_skip_test )
{
    TBEGIN( "dsl pa skip() tests" );

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string channel;
    std::string level;

    TTEST( my_pa.skip( alphabet_word_char() ) &&
                my_pa.space() &&
                my_pa.get( &channel, alphabet_alpha() ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '=' ) &&
                my_pa.opt_space() &&
                my_pa.get( &level, alphabet_digit() ) );

    TTEST( channel == "p" );
    TTEST( level == "12" );
    }

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string channel;
    std::string level;

    TTEST( my_pa.skip_until( alphabet_space() ) &&
                my_pa.space() &&
                my_pa.get_until( &channel, alphabet_char_class( "=" ) ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '=' ) &&
                my_pa.opt_space() &&
                my_pa.get_until( &level, alphabet_space() ) );

    TTEST( channel == "p" );
    TTEST( level == "12" );
    }
}

TFUNCTION( dsl_pa_fixed_test )
{
    TBEGIN( "dsl pa fixed and ifixed tests" );

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;
    std::string channel;
    std::string level;

    TTEST( (my_pa.fixed( "Range" ) || my_pa.fixed( "Mode" )) &&
                my_pa.space() &&
                my_pa.get( &channel, alphabet_alpha() ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '=' ) &&
                my_pa.opt_space() &&
                my_pa.get( &level, alphabet_digit() ) );

    TTEST( channel == "p" );
    TTEST( level == "12" );
    }

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;
    std::string channel;
    std::string level;

    TTEST( (my_pa.ifixed( "rANGE" ) || my_pa.ifixed( "mODE" )) &&
                my_pa.space() &&
                my_pa.get( &channel, alphabet_alpha() ) &&
                my_pa.opt_space() &&
                my_pa.is_get_char( '=' ) &&
                my_pa.opt_space() &&
                my_pa.get( &level, alphabet_digit() ) );

    TTEST( channel == "p" );
    TTEST( level == "12" );
    }

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;

    TTEST( (my_pa.get_fixed( &command, "Range" ) || my_pa.get_fixed( &command, "Mode" )) );

    TTEST( command == "Mode" );
    }

    {
    reader_string my_reader( "MyMode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;

    TTEST( (my_pa.get_fixed( &command, "MyRange" ) || my_pa.get_fixed( &command, "MyMode" )) );

    TTEST( command == "MyMode" );
    }

    {
    reader_string my_reader( "Mode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;

    TTEST( (my_pa.get_ifixed( &command, "rANGE" ) || my_pa.get_ifixed( &command, "mODE" )) );

    TTEST( command == "Mode" );
    }

    {
    reader_string my_reader( "MyMode p=12" );
    dsl_pa my_pa( my_reader );

    std::string command;

    TTEST( (my_pa.get_ifixed( &command, "MYrANGE" ) || my_pa.get_ifixed( &command, "MYmODE" )) );

    TTEST( command == "MyMode" );
    }
}

TFUNCTION( dsl_pa_get_char_test )
{
    TBEGIN( "dsl_pa::get_char() Tests" );

    {
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.get_char() );
    TTEST( my_pa.current() == 'A' );
    }
}

TFUNCTION( dsl_pa_get_char_no_space_test )
{
    TBEGIN( "dsl_pa::get_char_no_space() Tests" );

    {
    reader_string my_reader( "   A   BC" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.get_char_no_space() );
    TTEST( my_pa.current() == 'A' );
    TTEST( my_pa.get_char_no_space() );
    TTEST( my_pa.current() == 'B' );
    TTEST( my_pa.get_char_no_space() );
    TTEST( my_pa.current() == 'C' );
    }

    {
    reader_string my_reader( "   A   BC" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.get_char_no_wsp() );
    TTEST( my_pa.current() == 'A' );
    TTEST( my_pa.get_char_no_wsp() );
    TTEST( my_pa.current() == 'B' );
    TTEST( my_pa.get_char_no_wsp() );
    TTEST( my_pa.current() == 'C' );
    }
}

TFUNCTION( dsl_pa_peek_char_no_space_test )
{
    TBEGIN( "dsl_pa::peek_char_no_space() Tests" );

    {
    reader_string my_reader( "   A   BC" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.peek_char_no_space() );
    TTEST( my_pa.current() == 'A' );
    TTEST( my_pa.get_char() );
    TTEST( my_pa.current() == 'A' );
    TTEST( my_pa.peek_char_no_space() );
    TTEST( my_pa.current() == 'B' );

    TTEST( my_pa.peek_char_no_space() );    // Peeking again should return same char
    TTEST( my_pa.current() == 'B' );

    TTEST( my_pa.get_char() );
    TTEST( my_pa.peek_char_no_space() );
    TTEST( my_pa.current() == 'C' );
    }

    {
    reader_string my_reader( "   A   BC" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.peek_char_no_wsp() );
    TTEST( my_pa.current() == 'A' );
    TTEST( my_pa.get_char() );
    TTEST( my_pa.current() == 'A' );
    TTEST( my_pa.peek_char_no_wsp() );
    TTEST( my_pa.current() == 'B' );

    TTEST( my_pa.peek_char_no_wsp() );  // Peeking again should return same char
    TTEST( my_pa.current() == 'B' );

    TTEST( my_pa.get_char() );
    TTEST( my_pa.peek_char_no_wsp() );
    TTEST( my_pa.current() == 'C' );
    }
}

TFUNCTION( dsl_pa_get_char_type_test )
{
    TBEGIN( "dsl_pa::get_char(...) Tests" );

    {
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );
    int c;
    TTEST( my_pa.get_char( &c ) );
    TTEST( c == 'A' );
    }

    {
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );
    std::string out( "XYZ" );
    TTEST( my_pa.get_char( &out ) );
    TTEST( out == "A" );
    }

    {
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );
    std::string out( "XYZ" );
    TTEST( my_pa.read_char( &out ) );
    TTEST( out == "XYZA" );
    }
}

enum test_valid { TEST_VALID, TEST_INVALID };

void dsl_pa_bool_test( const char * p_input, test_valid valid, bool expected_value, char next_char )
{
    TDOC( (std::string( "Input: " ) + p_input).c_str() );

    {
    reader_string my_reader( p_input );
    dsl_pa my_pa( my_reader );
    std::string result;
    TTEST( my_pa.get_bool( &result ) == (valid == TEST_VALID) );
    TTEST( my_pa.get() == next_char );  // Check input location rewound
    }

    {
    reader_string my_reader( p_input );
    dsl_pa my_pa( my_reader );
    bool result;
    TTEST( my_pa.get_bool( &result ) == (valid == TEST_VALID) );
    if( valid == TEST_VALID )
        { TTEST( result == expected_value ); }
    TTEST( my_pa.get() == next_char );  // Check input location rewound
    }
}

TFUNCTION( dsl_pa_get_bool_test )
{
    TBEGIN( "dsl_pa::get_bool() Tests" );

    dsl_pa_bool_test( "true", TEST_VALID, true, '\0' );
    dsl_pa_bool_test( "true ", TEST_VALID, true, ' ' );
    dsl_pa_bool_test( "false", TEST_VALID, false, '\0' );
    dsl_pa_bool_test( "false ", TEST_VALID, false, ' ' );
    dsl_pa_bool_test( "trunk ", TEST_INVALID, false, 't' );
    dsl_pa_bool_test( "junk ", TEST_INVALID, false, 'j' );
}

void dsl_pa_int_test( const char * p_input, int expected_value, size_t n_chars )
{
    TDOC( (std::string( "Input: " ) + p_input).c_str() );
    reader_string my_reader( p_input );
    dsl_pa my_pa( my_reader );
    int result=0;
    TTEST( my_pa.get_int( &result ) == n_chars );
    if( n_chars != 0 )
        { TTEST( result == expected_value ); }
    else
        { TTEST( my_pa.get() == *p_input ); }   // Check input location rewound
}

TFUNCTION( dsl_pa_get_int_test )
{
    TBEGIN( "dsl_pa::get_int() Tests" );

    dsl_pa_int_test( "100", 100, 3 );
    dsl_pa_int_test( "-100", -100, 4 );
    dsl_pa_int_test( "0", 0, 1 );
    dsl_pa_int_test( "-0", 0, 2 );
    dsl_pa_int_test( "000", 0, 3 );
    // The following should fail
    dsl_pa_int_test( "-", 0, 0 );
    dsl_pa_int_test( "+", 0, 0 );
    dsl_pa_int_test( "--0", 0, 0 );
    dsl_pa_int_test( "", 0, 0 );
    dsl_pa_int_test( "w", 0, 0 );
}

void dsl_pa_uint_test( const char * p_input, unsigned int expected_value, size_t n_chars )
{
    TDOC( (std::string( "Input: " ) + p_input).c_str() );
    reader_string my_reader( p_input );
    dsl_pa my_pa( my_reader );
    unsigned int result=0;
    TTEST( my_pa.get_uint( &result ) == n_chars );
    if( n_chars != 0 )
        { TTEST( result == expected_value ); }
    else
        { TTEST( my_pa.get() == *p_input ); }   // Check input location rewound
}

TFUNCTION( dsl_pa_get_uint_test )
{
    TBEGIN( "dsl_pa::get_uint() Tests" );

    dsl_pa_uint_test( "100", 100, 3 );
    dsl_pa_uint_test( "-100", -100, 4 );
    dsl_pa_uint_test( "0", 0, 1 );
    dsl_pa_uint_test( "-0", 0, 2 );
    dsl_pa_uint_test( "000", 0, 3 );
    // The following should fail
    dsl_pa_uint_test( "-", 0, 0 );
    dsl_pa_int_test( "+", 0, 0 );
    dsl_pa_int_test( "--0", 0, 0 );
    dsl_pa_uint_test( "", 0, 0 );
    dsl_pa_uint_test( "w", 0, 0 );
}

void dsl_pa_float_test( const char * p_input, float expected_value, bool is_valid )
{
    TDOC( (std::string( "Input: " ) + p_input).c_str() );
    reader_string my_reader( p_input );
    dsl_pa my_pa( my_reader );
    float result=0;
    TTEST( my_pa.get_float( &result ) == is_valid );
    if( is_valid )
        { TTEST( result == expected_value ); }
    else
        { TTEST( my_pa.get() == *p_input ); }   // Check input location rewound
}

TFUNCTION( dsl_pa_get_float_test )
{
    TBEGIN( "dsl_pa::get_float() Tests" );

    dsl_pa_float_test( "1", 1.0f, true );
    dsl_pa_float_test( "1.0", 1.0f, true );
    dsl_pa_float_test( "0.1", 0.1f, true );
    dsl_pa_float_test( ".1", 0.1f, true );
    dsl_pa_float_test( "-1", -1.0f, true );
    dsl_pa_float_test( "-1.0", -1.0f, true );
    dsl_pa_float_test( "-0.1", -0.1f, true );
    dsl_pa_float_test( "-.1", -0.1f, true );
    // The following should fail
    dsl_pa_float_test( "-", 0.0f, false );
    dsl_pa_int_test( "+", 0, 0 );
    dsl_pa_int_test( "--0", 0, 0 );
    dsl_pa_float_test( ".", 0.0f, false );
    dsl_pa_float_test( "", 0.0f, false );
    dsl_pa_float_test( "w", 0.0f, false );
}

void dsl_pa_sci_float_test( const char * p_input, float expected_value, bool is_valid, char next_char )
{
    TDOC( (std::string( "Input: " ) + p_input).c_str() );
    reader_string my_reader( p_input );
    dsl_pa my_pa( my_reader );
    float result=0;
    TTEST( my_pa.get_sci_float( &result ) == is_valid );
    if( is_valid )
        { TTEST( result == expected_value ); }
    TTEST( my_pa.get() == next_char );
}

TFUNCTION( dsl_pa_get_sci_float_test )
{
    TBEGIN( "dsl_pa::get_sci_float() Tests" );

    dsl_pa_sci_float_test( "1 ", 1.0f, true, ' ' );
    dsl_pa_sci_float_test( "1.0 ", 1.0f, true, ' ' );
    dsl_pa_sci_float_test( "0.1 ", 0.1f, true, ' ' );
    dsl_pa_sci_float_test( "1", 1.0f, true, '\0' );
    dsl_pa_sci_float_test( "1.0", 1.0f, true, '\0' );
    dsl_pa_sci_float_test( "0.1", 0.1f, true, '\0' );
    dsl_pa_sci_float_test( ".1", 0.1f, true, '\0' );
    dsl_pa_sci_float_test( "-1", -1.0f, true, '\0' );
    dsl_pa_sci_float_test( "-1.0", -1.0f, true, '\0' );
    dsl_pa_sci_float_test( "-0.1", -0.1f, true, '\0' );
    dsl_pa_sci_float_test( "-.1", -0.1f, true, '\0' );
    dsl_pa_sci_float_test( "1e1", 10.0f, true, '\0' );
    dsl_pa_sci_float_test( "1e-1", 0.1f, true, '\0' );
    dsl_pa_sci_float_test( "1E1", 10.0f, true, '\0' );
    dsl_pa_sci_float_test( "1E-1", 0.1f, true, '\0' );
    dsl_pa_sci_float_test( "1e", 1.0f, true, 'e' );
    dsl_pa_sci_float_test( "1e-", 1.0f, true, 'e' );
    dsl_pa_sci_float_test( "1e+", 1.0f, true, 'e' );
    // The following should fail
    dsl_pa_sci_float_test( "-", 0.0f, false, '-' );
    dsl_pa_sci_float_test( "--1", 0.0f, false, '-' );
    dsl_pa_sci_float_test( ".", 0.0f, false, '.' );
    dsl_pa_sci_float_test( "-e", 0.0f, false, '-' );
    dsl_pa_sci_float_test( "", 0.0f, false, '\0' );
    dsl_pa_sci_float_test( "w", 0.0f, false, 'w' );
}

void dsl_pa_qstring_test( std::string input, const std::string & r_expected_value, bool is_valid )
{
    input += '"';
    reader_string my_reader( input );
    dsl_pa my_pa( my_reader );
    std::string result;
    TTEST( my_pa.get_qstring_contents( &result ) == is_valid );
    if( is_valid )
    {
        TTEST( result == r_expected_value );
        TTEST( my_pa.is_get_char( '"' ) );
    }
    else
    {
        TTEST( my_pa.get() == input[0] );   // Check input location rewound
    }
}

TFUNCTION( dsl_pa_get_qstring_contents_test )
{
    TBEGIN( "dsl_pa::get_qstring_contents() Tests" );

    TCALL( dsl_pa_qstring_test( "", "", true ) );
    TCALL( dsl_pa_qstring_test( "a", "a", true ) );
    TCALL( dsl_pa_qstring_test( "ba", "ba", true ) );
    TCALL( dsl_pa_qstring_test( "b\\/a", "b/a", true ) );
    TCALL( dsl_pa_qstring_test( "b\\\"a", "b\"a", true ) );
    TCALL( dsl_pa_qstring_test( "b\\ta", "b\ta", true ) );
    TCALL( dsl_pa_qstring_test( "b\\t\\r\\na", "b\t\r\na", true ) );
    TCALL( dsl_pa_qstring_test( "b\\f\\ba", "b\f\ba", true ) );
    TCALL( dsl_pa_qstring_test( "b\\\\a", "b\\a", true ) );
    TCALL( dsl_pa_qstring_test( "b\\u0020a", "b a", true ) );


    // The following converted using http://rishida.net/tools/conversion/
    TCALL( dsl_pa_qstring_test( "\\uD800\\uDC02", "\xF0\x90\x80\x82", true ) );  // \uD800\uDC02 -> u+10002
    TCALL( dsl_pa_qstring_test( "\\ud800\\udc02", "\xF0\x90\x80\x82", true ) );  // \ud800\udc02 -> u+10002
    TCALL( dsl_pa_qstring_test( "\\u0802", "\xE0\xA0\x82", true ) );
    TCALL( dsl_pa_qstring_test( "\\uFFFC", "\xEF\xBF\xBC", true ) );
    TCALL( dsl_pa_qstring_test( "\\ufffc", "\xEF\xBF\xBC", true ) );
    TCALL( dsl_pa_qstring_test( "\\u0082", "\xC2\x82", true ) );
    TCALL( dsl_pa_qstring_test( "\\u07FC", "\xDF\xBC", true ) );

    // Check conversions also work within a string
    TCALL( dsl_pa_qstring_test( "X\\uD800\\uDC02A", "X\xF0\x90\x80\x82""A", true ) );    // \uD800\uDC02 -> u+10002
    TCALL( dsl_pa_qstring_test( "X\\u0802A", "X\xE0\xA0\x82""A", true ) );
    TCALL( dsl_pa_qstring_test( "X\\u0082A", "X\xC2\x82""A", true ) );

    TDOC( "get_qstring_contents() - truncated BMP unicode escape fails" );
    TCALL( dsl_pa_qstring_test( "Say \\u002 Fred", "", false ) );
    TCALL( dsl_pa_qstring_test( "Say \\u002QFred", "", false ) );

    // From rfc3629
    TCALL( dsl_pa_qstring_test( "\\u65E5\\u672C\\u8A9E", "\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E", true ) );

    // From rfc2781
    TDOC( "get_qstring_contents() - Surrogates unicode escape" );
    TCALL( dsl_pa_qstring_test( "\\uD808\\uDF45=Ra", "\xF0\x92\x8D\x85=Ra", true ) );

    TDOC( "get_qstring_contents() - High surrogate without following low surrogate fails" );
    TCALL( dsl_pa_qstring_test( "\\uD808Fred", "", false ) );
    TCALL( dsl_pa_qstring_test( "\\uD808\\u0022", "", false ) );

    TDOC( "get_qstring_contents() - Low surrogate without preceeding high surrogate fails" );
    TCALL( dsl_pa_qstring_test( "\\uDF45Fred", "", false ) );
    TCALL( dsl_pa_qstring_test( "\\uDF45\\u0022", "", false ) );

    TDOC( "get_qstring_contents() - UTF-8 sequences are valid" );
    TCALL( dsl_pa_qstring_test( "\xE0\xAC\x8Bz", "\xE0\xAC\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "abc\xE0\xAC\x8Bz", "abc\xE0\xAC\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "abc\xE0\xAC\x8Bz\xE0\xAC\x8Bz", "abc\xE0\xAC\x8Bz\xE0\xAC\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "abc\xE0\xAC\x8B\xE0\xAC\x8Bz", "abc\xE0\xAC\x8B\xE0\xAC\x8Bz", true ) );

    // Test the following from RFC 3629 is satisfied:
    // UTF8-2      = %xC2-DF UTF8-tail
    // UTF8-3      = %xE0 %xA0-BF UTF8-tail / %xE1-EC 2( UTF8-tail ) /
    //               %xED %x80-9F UTF8-tail / %xEE-EF 2( UTF8-tail )
    // UTF8-4      = %xF0 %x90-BF 2( UTF8-tail ) / %xF1-F3 3( UTF8-tail ) /
    //               %xF4 %x80-8F 2( UTF8-tail )
    // UTF8-tail   = %x80-BF
    TCALL( dsl_pa_qstring_test( "a\xc2\x8Bz", "a\xc2\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xdf\x8Bz", "a\xdf\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xdf\x8B\x8Bz", "", false ) );    // To long
    TCALL( dsl_pa_qstring_test( "a\xdfz", "", false ) );            // To short
    TCALL( dsl_pa_qstring_test( "a\xe0\xa0\x8Bz", "a\xe0\xa0\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xe0\xbf\x8Bz", "a\xe0\xbf\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xe0\x8B\x8Bz", "", false ) );
    TCALL( dsl_pa_qstring_test( "a\xe0\xbf\x8B\x8Bz", "", false ) );    // To long
    TCALL( dsl_pa_qstring_test( "a\xe0\xbfz", "", false ) );            // To short
    TCALL( dsl_pa_qstring_test( "a\xe1\xbf\x8Bz", "a\xe1\xbf\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xec\xbf\x8Bz", "a\xec\xbf\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xec\xbf\x8B\x8Bz", "", false ) );    // To long
    TCALL( dsl_pa_qstring_test( "a\xec\xbfz", "", false ) );            // To short
    TCALL( dsl_pa_qstring_test( "a\xed\x80\x8Bz", "a\xed\x80\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xed\x9f\x8Bz", "a\xed\x9f\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xed\xa0\x8Bz", "a\xed\xa0\x8Bz", false ) );
    TCALL( dsl_pa_qstring_test( "a\xed\xa0\x8B\x8Bz", "", false ) );    // To long
    TCALL( dsl_pa_qstring_test( "a\xed\xa0z", "", false ) );            // To short
    TCALL( dsl_pa_qstring_test( "a\xee\xb0\x8Bz", "a\xee\xb0\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xef\xb0\x8Bz", "a\xef\xb0\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xef\xb0\x8B\x8Bz", "", false ) );    // To long
    TCALL( dsl_pa_qstring_test( "a\xef\xb0z", "", false ) );            // To short
    TCALL( dsl_pa_qstring_test( "a\xf0\x90\x8B\x8Bz", "a\xf0\x90\x8B\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xf0\xbf\x8B\x8Bz", "a\xf0\xbf\x8B\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xf0\x80\x8B\x8Bz", "a\xf0\x80\x8B\x8Bz", false ) );
    TCALL( dsl_pa_qstring_test( "a\xf0\xbf\x8B\x8B\x8Bz", "", false ) );    // To long
    TCALL( dsl_pa_qstring_test( "a\xf0\xbf\x8Bz", "", false ) );            // To short
    TCALL( dsl_pa_qstring_test( "a\xf1\xbf\x8B\x8Bz", "a\xf1\xbf\x8B\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xf3\xbf\x8B\x8Bz", "a\xf3\xbf\x8B\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xf3\xbf\x8B\x8B\x8Bz", "", false ) );    // To long
    TCALL( dsl_pa_qstring_test( "a\xf3\xbf\x8Bz", "", false ) );            // To short
    TCALL( dsl_pa_qstring_test( "a\xf4\x80\x8B\x8Bz", "a\xf4\x80\x8B\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xf4\x8f\x8B\x8Bz", "a\xf4\x8f\x8B\x8Bz", true ) );
    TCALL( dsl_pa_qstring_test( "a\xf4\x90\x8B\x8Bz", "a\xf4\x90\x8B\x8Bz", false ) );
    TCALL( dsl_pa_qstring_test( "a\xf4\x90\x8B\x8B\x8Bz", "", false ) );    // To long
    TCALL( dsl_pa_qstring_test( "a\xf4\x90\x8Bz", "", false ) );            // To short

    TCALL( dsl_pa_qstring_test( "abc\xE0\xAC\x8B\xE0\xE0\xAC\x8Bz", "", false ) );  // Duplicate UTF-8 start character \xE0
    TCALL( dsl_pa_qstring_test( "abc\xE0\xAC\x8B\xE0\xAC\x8B\x8Bz", "", false ) );  // Additional UTF-8 continuation character \x8B
}

TFUNCTION( dsl_pa_current_is_test )
{
    TBEGIN( "dsl_pa::current_is() Tests" );

    {
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.get_char() );
    TTEST( my_pa.current_is( 'A' ) );
    }

    {
    const alphabet_char_class alphabet_char_class_ABX( "ABX" );
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );

    TTEST( my_pa.get_char() );
    TTEST( my_pa.current_is( 'A' ) );
    TTEST( my_pa.current_is_in( alphabet_char_class_ABX ) );

    TTEST( my_pa.get_char() );
    TTEST( my_pa.current_is( 'B' ) );
    TTEST( my_pa.current_is_in( alphabet_char_class_ABX ) );

    TTEST( my_pa.get_char() );
    TTEST( my_pa.current_is( 'C' ) );
    TTEST( ! my_pa.current_is_in( alphabet_char_class_ABX ) );
    }
}

TFUNCTION( dsl_pa_peek_is_test )
{
    TBEGIN( "dsl_pa::peek_is() Tests" );

    {
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.peek_is( 'A' ) );
    TTEST( my_pa.peek_is( 'A' ) );      // Should be able to peek_is() multiple times
    }

    {
    const alphabet_char_class alphabet_char_class_ABX( "ABX" );
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );

    TTEST( my_pa.peek_is( 'A' ) );
    TTEST( my_pa.peek_is_in( alphabet_char_class_ABX ) );
    TTEST( my_pa.get_char() );

    TTEST( my_pa.peek_is( 'B' ) );
    TTEST( my_pa.peek_is_in( alphabet_char_class_ABX ) );
    TTEST( my_pa.get_char() );

    TTEST( my_pa.peek_is( 'C' ) );
    TTEST( ! my_pa.peek_is_in( alphabet_char_class_ABX ) );
    }
}

TFUNCTION( dsl_pa_peek_char_test )
{
    TBEGIN( "dsl_pa::peek_char() Tests" );

    {
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.peek_char() );
    TTEST( my_pa.current_is( 'A' ) );      // Should be able to current_is() multiple times
    TTEST( my_pa.current_is( 'A' ) );      // Should be able to current_is() multiple times
    TTEST( ! my_pa.current_is( 'B' ) );
    }

    {
    const alphabet_char_class alphabet_char_class_ABX( "ABX" );
    reader_string my_reader( "ABC" );
    dsl_pa my_pa( my_reader );

    TTEST( my_pa.peek_char() );
    TTEST( my_pa.current_is( 'A' ) );
    TTEST( my_pa.current_is_in( alphabet_char_class_ABX ) );
    TTEST( my_pa.get_char() );

    TTEST( my_pa.peek_char() );
    TTEST( my_pa.peek_is( 'B' ) );
    TTEST( my_pa.current_is_in( alphabet_char_class_ABX ) );
    TTEST( my_pa.current_is_in( alphabet_char_class_ABX ) );      // Should be able to current_is() multiple times
    TTEST( my_pa.get_char() );

    TTEST( my_pa.peek_char() );
    TTEST( my_pa.peek_is( 'C' ) );
    TTEST( ! my_pa.current_is_in( alphabet_char_class_ABX ) );
    }
}

void optional_rewind_test(
            const char * p_msg,
            std::string & command,
            std::string & type,
            bool & is_empty_mode )
{
    reader_string my_reader( p_msg );
    dsl_pa my_pa( my_reader );

    is_empty_mode = false;

    locator loc( my_pa.get_reader() );
    TTEST( my_pa.optional_rewind(
                    my_pa.get_fixed( &command, "Mode" ) &&
                    my_pa.space() &&
                    my_pa.get_fixed( &type, "empty" ) &&
                    my_pa.set( is_empty_mode, true )
                    || my_pa.on_fail(
                            my_pa.clear( command ) && my_pa.clear( type ) )
                ) ||
                my_pa.get_fixed( &command, "Mode" ) &&
                my_pa.space() &&
                my_pa.get_fixed( &type, "full" ) );
}

TFUNCTION( dsl_pa_optional_rewind_test )
{
    TBEGIN( "optional_rewind class Tests" );

    {
    std::string command;
    std::string type;
    bool is_empty_mode = false;

    optional_rewind_test( "Mode full", command, type, is_empty_mode );

    TTEST( command == "Mode" );
    TTEST( is_empty_mode == false );
    TTEST( type == "full" );
    }

    {
    std::string command;
    std::string type;
    bool is_empty_mode = false;

    optional_rewind_test( "Mode empty", command, type, is_empty_mode );

    TTEST( command == "Mode" );
    TTEST( is_empty_mode == true );
    TTEST( type == "empty" );
    }
}

TFUNCTION( dsl_pa_is_peek_at_end_test )
{
    TBEGIN( "DSL PA is_peek_at_end Tests" );

    {
    reader_string my_reader( "" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.is_peek_at_end() );
    }

    {
    reader_string my_reader( "M" );
    dsl_pa my_pa( my_reader );
    TTEST( ! my_pa.is_peek_at_end() );
    TTEST( my_pa.is_get_char( 'M' ) );
    TTEST( my_pa.is_peek_at_end() );
    }
}

TFUNCTION( dsl_pa_is_current_at_end_test )
{
    TBEGIN( "DSL PA is_current_at_end Tests" );

    {
    reader_string my_reader( "" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.get_char() == false );     // get_char() returns false at EOF
    TTEST( my_pa.is_current_at_end() );
    }

    {
    reader_string my_reader( "M" );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.get_char() );
    TTEST( my_pa.current_is( 'M' ) );
    TTEST( ! my_pa.is_current_at_end() );
    TTEST( my_pa.get_char() == false );     // get_char() returns false at EOF
    TTEST( my_pa.is_current_at_end() );
    }
}

TFUNCTION( get_read_check )
{
    TBEGIN( "DSL PA get vs. read check" );

    {
    std::string in( "AAAAAABBBBB" );
    std::string parsed( "origin" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read( &parsed, alphabet_char_class( "A" ) ) > 0 );
    TTEST( parsed == "originAAAAAA" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get( &parsed, alphabet_char_class( "A" ) ) > 0 );
    TTEST( parsed == "AAAAAA" );
    }

    {
    std::string in( "AAAAAABBBBB" );
    std::string parsed( "origin" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read( &parsed, alphabet_char_class( "A" ), 20 ) > 0 );
    TTEST( parsed == "originAAAAAA" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get( &parsed, alphabet_char_class( "A" ), 20 ) > 0 );
    TTEST( parsed == "AAAAAA" );
    }

    {
    std::string in( "AAAAAABBBBB" );
    std::string parsed( "origin" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_until( &parsed, alphabet_char_class( "B" ) ) > 0 );
    TTEST( parsed == "originAAAAAA" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_until( &parsed, alphabet_char_class( "B" ) ) > 0 );
    TTEST( parsed == "AAAAAA" );
    }

    {
    std::string in( "AAAAAABBBBB" );
    std::string parsed( "origin" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_bounded_until( &parsed, alphabet_char_class( "B" ), 20 ) > 0 );
    TTEST( parsed == "originAAAAAA" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_bounded_until( &parsed, alphabet_char_class( "B" ), 20 ) > 0 );
    TTEST( parsed == "AAAAAA" );
    }

    {
    std::string in( "AAAAAABBBBB" );
    std::string parsed( "origin" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_escaped_until( &parsed, alphabet_char_class( "B" ), 'C' ) > 0 );
    TTEST( parsed == "originAAAAAA" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_escaped_until( &parsed, alphabet_char_class( "B" ), 'C' ) > 0 );
    TTEST( parsed == "AAAAAA" );
    }

    {
    std::string in( "AAAAAABBBBB" );
    std::string parsed( "origin" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_until( &parsed, alphabet_char_class( "B" ), 'C', 20 ) > 0 );
    TTEST( parsed == "originAAAAAA" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_until( &parsed, alphabet_char_class( "B" ), 'C', 20 ) > 0 );
    TTEST( parsed == "AAAAAA" );
    }

    {
    std::string in( "left right" );
    std::string parsed;

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_fixed( &parsed, "left" ) && my_pa.space() && my_pa.read_fixed( &parsed, "right" ) );
    TTEST( parsed == "leftright" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_fixed( &parsed, "left" ) && my_pa2.space() && my_pa2.get_fixed( &parsed, "right" ) );
    TTEST( parsed == "right" );
    }

    {
    std::string in( "left right" );
    std::string parsed;

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_ifixed( &parsed, "LEFT" ) && my_pa.space() && my_pa.read_ifixed( &parsed, "RIGHT" ) );
    TTEST( parsed == "leftright" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_ifixed( &parsed, "LEFT" ) && my_pa2.space() && my_pa2.get_ifixed( &parsed, "RIGHT" ) );
    TTEST( parsed == "right" );
    }

    {
    std::string in( "true false" );
    std::string parsed;

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_bool( &parsed ) && my_pa.space() && my_pa.read_bool( &parsed ) );
    TTEST( parsed == "truefalse" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_bool( &parsed ) && my_pa2.space() && my_pa2.get_bool( &parsed ) );
    TTEST( parsed == "false" );
    }

    {
    std::string in( "123 456" );
    std::string parsed;

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_int( &parsed ) && my_pa.space() && my_pa.read_int( &parsed ) );
    TTEST( parsed == "123456" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_int( &parsed ) && my_pa2.space() && my_pa2.get_int( &parsed ) );
    TTEST( parsed == "456" );
    }

    {
    std::string in( "123 456" );
    std::string parsed;

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_uint( &parsed ) && my_pa.space() && my_pa.read_uint( &parsed ) );
    TTEST( parsed == "123456" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_uint( &parsed ) && my_pa2.space() && my_pa2.get_uint( &parsed ) );
    TTEST( parsed == "456" );
    }

    {
    std::string in( "123 456" );
    std::string parsed;

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_float( &parsed ) && my_pa.space() && my_pa.read_float( &parsed ) );
    TTEST( parsed == "123456" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_float( &parsed ) && my_pa2.space() && my_pa2.get_float( &parsed ) );
    TTEST( parsed == "456" );
    }

    {
    std::string in( "123 456" );
    std::string parsed;

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.read_sci_float( &parsed ) && my_pa.space() && my_pa.read_sci_float( &parsed ) );
    TTEST( parsed == "123456" );

    reader_string my_reader2( in );
    dsl_pa my_pa2( my_reader2 );
    TTEST( my_pa2.get_sci_float( &parsed ) && my_pa2.space() && my_pa2.get_sci_float( &parsed ) );
    TTEST( parsed == "456" );
    }
}

TFUNCTION( accumulator_check )
{
    TBEGIN( "accumulator test" );

    {
    std::string in( "ABCDE" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator accumulator( &my_pa );
    TTEST( my_pa.accumulate( alphabet_char_class( "A" ) ) );
    TTEST( my_pa.accumulate( alphabet_char_class( "B" ) ) );
    TTEST( ! my_pa.accumulate( alphabet_char_class( "Z" ) ) );
    TTEST( my_pa.accumulate( alphabet_char_class( "C" ) ) );
    TTEST( accumulator.get() == "ABC" );
    }

    {
    TDOC( "Can use accumulate() on specific chars" );
    std::string in( "ABCDE" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator accumulator( &my_pa );
    TTEST( my_pa.accumulate( 'A' ) );
    TTEST( my_pa.accumulate( 'B' ) );
    TTEST( ! my_pa.accumulate( 'Z' ) );
    TTEST( my_pa.accumulate( 'C' ) );
    TTEST( accumulator.get() == "ABC" );
    }

    {
    TDOC( "Can use accumulate() without having somewhere to accumulate to" );
    std::string in( "ABCDE" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.accumulate( alphabet_char_class( "A" ) ) );
    TTEST( my_pa.accumulate( alphabet_char_class( "B" ) ) );
    TTEST( ! my_pa.accumulate( alphabet_char_class( "Z" ) ) );
    TTEST( my_pa.accumulate( alphabet_char_class( "C" ) ) );
    }

    {
    TDOC( "Specifying an accumulator operates in a stack-like nesting order" );
    std::string in( "+-ABCD123EFG+-" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    while( my_pa.accumulate( alphabet_not( alphabet_alpha() ) ) )
    {}
        {
        accumulator alpha_accumulator( &my_pa );
        while( my_pa.accumulate( alphabet_alpha() ) )
        {}
            {
            accumulator digit_accumulator( &my_pa );
            while( my_pa.accumulate( alphabet_digit() ) )
            {}
            TTEST( digit_accumulator.get() == "123" );
            }
        while( my_pa.accumulate( alphabet_alpha() ) )
        {}
        TTEST( alpha_accumulator.get() == "ABCDEFG" );
        }
    while( my_pa.accumulate( alphabet_not( alphabet_alpha() ) ) )   // Check reading ends
    {}
    }

    {
    TDOC( "Accumulators can be selected and unselected" );
    std::string in( "+-ABCD**123EFG+-" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator_deferred alphas_accumulator( &my_pa );
    accumulator_deferred digits_accumulator( &my_pa );
    while( my_pa.accumulate( alphabet_not( alphabet_alpha() ) ) )
    {}
    alphas_accumulator.select();
    while( my_pa.accumulate( alphabet_alpha() ) )
    {}
    alphas_accumulator.none();
    while( my_pa.accumulate( alphabet_not( alphabet_digit() ) ) )
    {}
    std::string alphas;
    int int_digits = 0;
    float float_digits = 0.0;
    double double_digits = 0.0;
    digits_accumulator.select() &&
        my_pa.accumulate_all( alphabet_digit() ) &&
        alphas_accumulator.select() &&
        my_pa.accumulate_all( alphabet_alpha() ) &&
        alphas_accumulator.put_in( alphas ) &&
        digits_accumulator.put_in( int_digits ) &&
        digits_accumulator.put_in( float_digits ) &&
        digits_accumulator.put_in( double_digits );
    TTEST( digits_accumulator.get() == "123" );
    TTEST( int_digits == 123 );
    TTEST( float_digits == 123 );
    TTEST( double_digits == 123 );
    TTEST( alphas == "ABCDEFG" );
    while( my_pa.accumulate( alphabet_not( alphabet_alpha() ) ) )   // Check reading ends
    {}
    }

    {
    TDOC( "Accumulators can be cleared" );
    std::string in( "ABCDE" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator accumulator( &my_pa );
    TTEST( my_pa.accumulate( alphabet_char_class( "A" ) ) );
    TTEST( my_pa.accumulate( alphabet_char_class( "B" ) ) );
    TTEST( accumulator.get() == "AB" );
    TTEST( accumulator.clear() );   // clear() method always returns 'true'
    TTEST( my_pa.accumulate( alphabet_char_class( "C" ) ) );
    TTEST( my_pa.accumulate( alphabet_char_class( "D" ) ) );
    TTEST( accumulator.get() == "CD" );
    }

    {
    TDOC( "Accumulators can be selected and cleared in one method" );
    std::string in( "ABCD123EFG" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator_deferred alphas_accumulator( &my_pa );
    accumulator_deferred digits_accumulator( &my_pa );
    alphas_accumulator.select() &&
        my_pa.accumulate_all( alphabet_alpha() );
    TTEST( alphas_accumulator.get() == "ABCD" );
    digits_accumulator.select() &&
        my_pa.accumulate_all( alphabet_digit() ) &&
        alphas_accumulator.select_and_clear() &&
        my_pa.accumulate_all( alphabet_alpha() );
    TTEST( digits_accumulator.get() == "123" );
    TTEST( alphas_accumulator.get() == "EFG" );
    while( my_pa.accumulate( alphabet_not( alphabet_alpha() ) ) )   // Check reading ends
    {}
    }

    {
    TDOC( "Accumulators can be unconditionally appended to using accumulator_append()" );
    std::string in( "" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    TTEST( my_pa.accumulator_append( 'b' ) );    // No active accumulator, but should be a safe operation
    accumulator my_accumulator( &my_pa );
    TTEST( my_pa.accumulator_append( 'X' ) );
    TTEST( my_pa.accumulator_append( "yz" ) );
    std::string extra( "AB" );
    TTEST( my_pa.accumulator_append( extra ) );
    TTEST( my_accumulator.get() == "XyzAB" );
    }

    {
    TDOC( "Accumulators can be appended to another accumulator using accumulator_append()" );
    std::string in( "" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator my_accumulator( &my_pa );
    TTEST( my_pa.accumulator_append( "abc" ) );
    accumulator other_accumulator( &my_pa );
    TTEST( my_pa.accumulator_append( "def" ) );
    TTEST( my_accumulator.get() == "abc" );
    TTEST( other_accumulator.get() == "def" );
    other_accumulator.previous();
    TTEST( my_pa.accumulator_append( other_accumulator ) );
    TTEST( my_accumulator.get() == "abcdef" );
    }

    {
    TDOC( "Accumulators can be appended to the previous accumulator using accumulator::append_to_previous()" );
    std::string in( "" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator my_accumulator( &my_pa );
    TTEST( my_pa.accumulator_append( "abc" ) );
    accumulator other_accumulator( &my_pa );
    TTEST( my_pa.accumulator_append( "def" ) );
    TTEST( my_accumulator.get() == "abc" );
    TTEST( other_accumulator.get() == "def" );
    TTEST( other_accumulator.append_to_previous() );
    TTEST( my_accumulator.get() == "abcdef" );
    }

    {
    TDOC( "Accumulators can be directly appended to using accumulator::append()" );
    std::string in( "" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator my_accumulator( &my_pa );
    TTEST( my_accumulator.append( 'X' ) );
    TTEST( my_accumulator.append( "yz" ) );
    std::string extra( "AB" );
    TTEST( my_accumulator.append( extra ) );
    TTEST( my_accumulator.get() == "XyzAB" );
    }

    {
    TDOC( "Accumulators can be put_in a string" );
    std::string in( "abcd0123" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator my_accumulator( &my_pa );
    my_pa.accumulate_all( alphabet_alpha() );
    std::string collected_alphas;
    TTEST( my_accumulator.put_in( collected_alphas ) );
    TTEST( collected_alphas == "abcd" );
    }

    {
    TDOC( "Accumulators can return a value converted to int" );
    std::string in( "101" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator my_accumulator( &my_pa );
    my_pa.accumulate_all( alphabet_digit() );
    TTEST( my_accumulator.to_int() == 101 );
    int collected_int = 0;
    TTEST( my_accumulator.put_in( collected_int ) );
    TTEST( collected_int == 101 );
    }

    {
    TDOC( "Accumulators can return a value converted to float/double" );
    std::string in( "101.5" );

    reader_string my_reader( in );
    dsl_pa my_pa( my_reader );
    accumulator my_accumulator( &my_pa );
    my_pa.accumulate_all( alphabet_or( alphabet_digit(), alphabet_char( '.' ) ) );
    TTEST( my_accumulator.to_float() == 101.5 );
    float collected_float = 0.0;
    double collected_double = 0.0;
    TTEST( my_accumulator.put_in( collected_float ) );
    TTEST( collected_float == 101.5 );
    TTEST( my_accumulator.put_in( collected_double ) );
    TTEST( collected_double == 101.5 );
    }
}
