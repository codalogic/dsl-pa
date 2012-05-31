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
// See dsl-pa.h and README.html at https://github.com/codalogic/dsl-pa for
// more information.
//----------------------------------------------------------------------------

#include "dsl-pa/dsl-pa-alphabet.h"

#include "dsl-pa/dsl-pa-reader.h"

namespace cl {

using namespace cl::alphabet_helpers;

char_map::char_map()
{
    clear();
}

void char_map::clear()
{
    for( size_t i=0; i<sizeof( char_map_array ); ++i )
        index[i] = 0;
}

char_map & char_map::clear( char c )
{
    index[ char_to_size_t( c ) ] = 0;
    return *this;
}

char_map & char_map::set( char c )
{
    index[ char_to_size_t( c ) ] = 1;
    return *this;
}

char_map & char_map::set_range( char start, char end )
{
    size_t start_index = char_to_size_t( start );
    size_t end_index = char_to_size_t( end );
    for( size_t i = start_index; i <= end_index; ++i )
        index[i] = 1;
    return *this;
}

char_map & char_map::set_inverted_range( char start, char end )
{
    merge( char_map().set_range( start, end ).invert() );
    return *this;
}

char_map & char_map::invert()
{
    for( size_t i=0; i<sizeof( char_map_array ); ++i )
        index[i] ^= 1;
    return *this;
}

void char_map::merge( const char_map & r_rhs )
{
    for( size_t i=0; i<sizeof( char_map_array ); ++i )
        index[i] |= r_rhs.index[i];
}

bool char_map::is_set( char c ) const
{
    return index[ char_to_size_t( c ) ] != 0;
}

char_map_w::char_map_w()
{
    set_range( 'a', 'z' );
    set_range( 'A', 'Z' );
    set_range( '0', '9' );
    set( '_' );
}

char_map_d::char_map_d()
{
    set_range( '0', '9' );
}

char_map_s::char_map_s()
{
    set( ' ' ).set( '\t' ).set( '\r' ).set( '\n' );
}

char_map_l::char_map_l()
{
    set_range( 'a', 'z' ).set_range( 'A', 'Z' );
}

alphabet_char_class::alphabet_char_class( const char * p_spec )
{
    bool is_inverted = false;

    if( *p_spec == '^' )
    {
        is_inverted = true;
        ++p_spec;
    }

    char last_char = (char)0xff;

    for( ; *p_spec != '\0'; ++p_spec )
    {
        if( *p_spec == '-' )            // Allow for ranges
        {
            if( ! add_range( last_char, *++p_spec ) )
                return;
        }

        else if( *p_spec == '\\' || *p_spec == '~' )        // Allow for specials
        {
            if( ! add_special_char_class( *++p_spec ) )
                return;
        }

        else
        {
            wanted_chars.set( last_char = *p_spec );
        }
    }

    if( is_inverted )
        wanted_chars.invert();

    wanted_chars.clear( reader::R_EOI );    // No alphabets want EOI ('\0')
}

bool alphabet_char_class::add_range( char start, char end )
{
    if( end == '\0' )
    {
        wanted_chars.set( '-' );
        return false;
    }
    else
        wanted_chars.set_range( start, end );   // Don't ++ last_char here incase we have a spec of a-a!

    return true;
}

bool alphabet_char_class::add_special_char_class( char key )
{
    switch( key )
    {
    case 'w':
        wanted_chars.merge( char_map_w() );
    break;

    case 'W':
        wanted_chars.merge( char_map_w().invert() );
    break;

    case 'd':
        wanted_chars.merge( char_map_d() );;
    break;

    case 'D':
        wanted_chars.merge( char_map_d().invert() );
    break;

    case 's':
        wanted_chars.merge( char_map_s() );
    break;

    case 'S':
        wanted_chars.merge( char_map_s().invert() );
    break;

    case 'l':
        wanted_chars.merge( char_map_l() );
    break;

    case 'L':
        wanted_chars.merge( char_map_l().invert() );
    break;

    case 'u':
        wanted_chars.set_range( '\x21', '\xff' );
    break;

    case '\\':
        wanted_chars.set( '\\' );
    break;

    case '~':
        wanted_chars.set( '~' );
    break;

    case '-':
        wanted_chars.set( '-' );
    break;

    // The following included for completeness.  Instead of ~t you could do \t etc.
    case 't':
        wanted_chars.set( '\t' );
    break;

    case 'r':
        wanted_chars.set( '\r' );
    break;

    case 'n':
        wanted_chars.set( '\n' );
    break;

    case '\0':
        wanted_chars.set( '~' );
        return false;
    break;

    default:
        wanted_chars.set( key );
    }

    return true;
}

} // End of namespace cl
