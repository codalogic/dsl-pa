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

#include "dsl-pa/dsl-pa.h"

#include <iostream>
#include <fstream>

using namespace cl;

void example1( std::ostream & fout )
{
    fout << "Example 1\n==============\n";

    // What we want to parse
    const char * const to_parse = " width=10, height = 5";

    // Create a reader with the desired input
    reader_string my_reader( to_parse );

    // Create a parser instance specifying the created reader
    dsl_pa pa( my_reader );

    // Create variables to capture parsed data
    size_t width, height;

    // Parse the 'to_parse' string
    if( pa.opt_space() &&
            pa.fixed( "width" ) &&
            pa.opt_space() && pa.is_char( '=' ) && pa.opt_space() &&
            pa.get_uint( &width ) &&
            pa.opt_space() && pa.is_char( ',' ) && pa.opt_space() &&
            pa.fixed( "height" ) &&
            pa.opt_space() && pa.is_char( '=' ) && pa.opt_space() &&
            pa.get_uint( &height ) )
    {
        // Report the results
        fout << "Example 1 OK: w=" << width << " & h=" << height << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}

class example_parser : public dsl_pa
{
public:
    struct date
    {
        int year;
        int month;
        int dom;
    };

public:
    example_parser( reader & r_reader_in ) : dsl_pa( r_reader_in ) {}

    void example2( std::ostream & fout );
    void example3( std::ostream & fout );
    void example4( std::ostream & fout );
    void example5( std::ostream & fout );
    void example6( std::ostream & fout );
    void example7( std::ostream & fout );

    bool eq() { return opt_space() && is_char( '=' ) && opt_space(); }
    bool comma() { return opt_space() && is_char( ',' ) && opt_space(); }

    bool get_date( date * p_date );
};

enum example_key {
        EXAMPLE_1 = 1, EXAMPLE_2, EXAMPLE_3, EXAMPLE_4, EXAMPLE_5, EXAMPLE_6,
        EXAMPLE_7 };

void class_example(
            example_key key,
            std::ostream & fout,
            const char * p_input = 0 )
{
    fout << "\nExample " << key << "\n==============\n";
    if( p_input )
        fout << "(Input = " << p_input << ")\n";

    reader_string my_reader( p_input ? p_input : " width=10, height = 5" );
    example_parser my_parser( my_reader );
    switch( key )
    {
    case EXAMPLE_2:
        my_parser.example2( fout );
    break;

    case EXAMPLE_3:
        my_parser.example3( fout );
    break;

    case EXAMPLE_4:
        my_parser.example4( fout );
    break;

    case EXAMPLE_5:
        my_parser.example5( fout );
    break;

    case EXAMPLE_6:
        my_parser.example6( fout );
    break;

    case EXAMPLE_7:
        my_parser.example7( fout );
    break;
    }
}

void example_parser::example2( std::ostream & fout )
{
    size_t width, height;

    if( opt_space() &&
            fixed( "width" ) &&
            opt_space() && is_char( '=' ) && opt_space() &&
            get_uint( &width ) &&
            opt_space() && is_char( ',' ) && opt_space() &&
            fixed( "height" ) &&
            opt_space() && is_char( '=' ) && opt_space() &&
            get_uint( &height ) )
    {
        fout << "Example OK: w=" << width << " & h=" << height << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}

void example_parser::example3( std::ostream & fout )
{
    // dsl-pa is explicit about where space characters can occur.
    // However, by defining your own methods for key tokens that
    // can have optional space around them you can simplify your
    // parsing code.  For example, eq() and comma() as used below
    // have been defined as inline class methods to allow optional
    // space surrounding the tokens:
    size_t width, height;

    if( opt_space() &&
            fixed( "width" ) && eq() && get_uint( &width ) &&
            comma() &&
            fixed( "height" ) && eq() && get_uint( &height ) )
    {
        fout << "Example OK: w=" << width << " & h=" << height << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}

void example_parser::example4( std::ostream & fout )
{
    // For machine-to-machine languages you can usually survive with fairly
    // coarse error reporting.  For finer-grain error reporting you can use
    // the dsl_pa::error() methods:
    // (P.S. The excessive number of brackets in the shortcuts are to avoid
    // gcc warnings.)
    size_t width, height;

    try
    {
        if( opt_space() &&
                ((fixed( "width" ) && eq() && get_uint( &width ))
                    || error( "Expected width parameter" )) &&
                ((comma())
                    || error( "Expected comma separator" )) &&
                ((fixed( "height" ) && eq() && get_uint( &height ))
                    || error( "Expected height parameter" )) )
        {
            fout << "Example OK: w=" << width << " & h=" << height << "\n";
        }
        else
        {
            fout << "Unable to parse input\n";
        }
    }
    catch( const dsl_pa_recoverable_exception & e )
    {
        fout << e.what() << "\n";
    }
}

void example_parser::example5( std::ostream & fout )
{
    // dsl-pa has additional helper functions to record and update state
    // information in the middle of shortcut sequences.  These are
    // dsl_pa::set(), record(), clear() and append():
    size_t width, height;
    bool is_width_found = false;
    bool is_height_found = false;

    if( opt_space() &&
            fixed( "width" ) && eq() && get_uint( &width ) &&
            set( is_width_found, true ) &&
            comma() &&
            fixed( "height" ) && eq() && get_uint( &height ) &&
            set( is_height_found, true ) )
    {
        fout << "Example OK: w=" << width << " & h=" << height << "\n";
        fout << "is_width_found=" << is_width_found << " & is_height_found=" << is_height_found << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}

bool example_parser::get_date( date * p_date )
{
    // location_logger is a class that uses RAII to ensure that a
    // location_push() has a corresponding location_pop() when the
    // function exits.
    location_logger my_location( get_reader() );

    if( get_int( &p_date->year ) == 4 &&
            is_char( '-' ) &&
            get_int( &p_date->month ) == 2 &&
            is_char( '-' ) &&
            get_int( &p_date->dom ) == 2 )
        return true;

    location_top();

    return false;
}

void example_parser::example6( std::ostream & fout )
{
    // If your language syntax is defined in some form of BNF (such as ABNF)
    // and it indicates that a non-terminal is present, then it is recommended
    // that you create your own function to parse that non-terminal rather
    // than expand the non-terminal inline (e.g. get_date() below):
    size_t width;
    date my_date;

    if( opt_space() &&
            fixed( "when" ) && eq() && get_date( &my_date ) &&
            comma() &&
            fixed( "width" ) && eq() && get_uint( &width ) )
    {
        fout << "Example OK: w=" << width << " & year=" << my_date.year << "\n";
    }
    else
    {
        fout << "Unable to parse input (Possibly the year is the wrong format)\n";
    }
}

const alphabet_char_class roman_numerals( "IVXLCM" );

void example_parser::example7( std::ostream & fout )
{
    // To parse an input corresponding to a specific set of characters you can
    // use the dsl_pa::get() method and specify the alphabet you want, such as
    // the roman_numerals alphabet described above. Alternatively you can use
    // dsl_pa::get_until() to get characters by specifying an alphabet that
    // specifies the set of characters you don't want.  A number of alphabets
    // are built-in, such as alphabet_space as used below.  The built-in
    // alphabets are efficient to construct and hence can be constructed in
    // place, but alphabet_char_class alphabets require some computation to
    // setup and are therefore better setup as const global objects:
    std::string roman_number;
    std::string non_space;

    if( opt_space() &&
            fixed( "year" ) && eq() && get( &roman_number, roman_numerals ) &&
            comma() &&
            get_until( &non_space, alphabet_space() ) )
    {
        fout << "Example OK: w=" << roman_number << " & non_space=" << non_space << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}

void example_lite( std::ostream & fout )
{
    // dsl_pa_lite is intended for light-weight parsing of short strings.
    // A typical scenario might be parsing a line-oriented protocol.

    fout << "\nExample Lite\n==============\n";

    std::string input( "year : 2012" );
    int year = 0;

    if( dsl_pa_lite( input ).fixed( "year" ).opt_space().is_char( ':' ).
            opt_space().get_int( &year ) )
    {
        fout << "Example OK: year=" << year << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}

int main()
{
    std::ofstream fout( "examples-out.txt" );

    example1( fout );
    class_example( EXAMPLE_2, fout );
    class_example( EXAMPLE_3, fout );
    class_example( EXAMPLE_4, fout );
    class_example( EXAMPLE_4, fout, " width=10 : height = 5" );
    class_example( EXAMPLE_5, fout );
    class_example( EXAMPLE_6, fout, " when = 2012-05-31, width=10" );
    class_example( EXAMPLE_6, fout, " when = 12-05-31, width=10" );
    class_example( EXAMPLE_7, fout, " year = MMXII, width::10 more" );

    example_lite( fout );
}
