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

#include "dsl-pa/dsl-pa-dsl-pa.h"

namespace cl {

using namespace alphabet_helpers;

bool /*is_not_eof*/ dsl_pa::get_char()      // Use dsl_pa::current() method is access value
{
    int c = get();
    return c != r_reader.R_EOI;
}

bool /*is_not_eof*/ dsl_pa::get_char_no_space()     // Use current() method to access value
{
    return opt_space() && get_char();
}

bool /*is_not_eof*/ dsl_pa::get_char_no_wsp()        // Use current() method to access value
{
    return opt_wsp() && get_char();
}

bool /*is_not_eof*/ dsl_pa::peek_char()     // Use current(), current_is() and current_is_in() methods to access value
{
    int c = peek();
    return c != r_reader.R_EOI;
}

bool /*is_not_eof*/ dsl_pa::peek_char_no_space()     // Use current(), current_is() and current_is_in() methods to access value
{
    return opt_space() && peek_char();
}

bool /*is_not_eof*/ dsl_pa::peek_char_no_wsp()       // Use current(), current_is() and current_is_in() methods to access value
{
    return opt_wsp() && peek_char();
}

bool dsl_pa::current_is( int c ) const
{
    return current() == c;
}

bool dsl_pa::current_is_in( const alphabet & r_alphabet ) const
{
    return r_alphabet.is_sought( current() );
}

bool dsl_pa::peek_is( int c )
{
    return peek() == c;
}

bool dsl_pa::peek_is_in( const alphabet & r_alphabet )
{
    return r_alphabet.is_sought( peek() );
}

size_t dsl_pa::space()
{
    size_t n_spaces = 0;

    while( is_space( get() ) )
        ++n_spaces;

    unget();

    return n_spaces;
}

size_t dsl_pa::wsp()
{
    size_t n_spaces = 0;

    while( is_line_space( get() ) )
        ++n_spaces;

    unget();

    return n_spaces;
}

bool /*is_not_eof*/ dsl_pa::get_char( std::string * p_input )
{
    p_input->clear();
    return read_char( p_input );
}

bool /*is_not_eof*/ dsl_pa::read_char( std::string * p_input )
{
    int c = get();
    if( c == r_reader.R_EOI )
        return false;
    *p_input += c;
    return true;
}

bool /*is_not_eof*/ dsl_pa::get_char( int * p_char )
{
    *p_char = get();
    return *p_char != r_reader.R_EOI;
}

bool /*is_parsed*/ dsl_pa::get_bool( std::string * p_input )
{
    return get_ifixed( p_input, "true" ) || get_ifixed( p_input, "false" );
}

bool /*is_parsed*/ dsl_pa::read_bool( std::string * p_input )
{
    return read_ifixed( p_input, "true" ) || read_ifixed( p_input, "false" );
}

bool /*is_parsed*/ dsl_pa::get_bool( bool * p_bool )
{
    return (ifixed( "true" ) && set( *p_bool, true )) ||
            (ifixed( "false" ) && set( *p_bool, false ));
}

size_t /*num chars read*/ dsl_pa::get_int( std::string * p_num )
{
    p_num->clear();
    return read_int( p_num );
}

size_t /*num chars read*/ dsl_pa::read_int( std::string * p_num )
{
    location_logger location( r_reader );

    size_t n_sign_chars = read( p_num, alphabet_sign(), 1 );
    size_t n_digits = read( p_num, alphabet_digit() );

    if( n_digits == 0 )
    {
        location.top();
        return 0;
    }

    return n_digits + n_sign_chars;
}

size_t /*num chars read*/ dsl_pa::get_int( int * p_int )
{
    std::string input;
    size_t n_read = read_int( &input );
    if( n_read > 0 )
        *p_int = atoi( input.c_str() );
    return n_read;
}

size_t /*num chars read*/ dsl_pa::get_uint( std::string * p_num )
{
    p_num->clear();
    return read_uint( p_num );
}

size_t /*num chars read*/ dsl_pa::read_uint( std::string * p_num )
{
    return read( p_num, alphabet_digit() );
}

size_t /*num chars read*/ dsl_pa::get_uint( unsigned int * p_int )
{
    std::string input;
    size_t n_read = get_int( &input );
    if( n_read > 0 )
        sscanf( input.c_str(), "%ud", p_int );
    return n_read;
}

bool dsl_pa::get_float( std::string * p_num )
{
    p_num->clear();
    return read_float( p_num );
}

bool dsl_pa::read_float( std::string * p_num )
{
    location_logger location( r_reader );

    size_t n_digits_before_point = 0, n_digits_after_point = 0;

    optional( read( p_num, alphabet_sign(), 1 ) ) &&
            set( n_digits_before_point, read( p_num, alphabet_digit() ) ) &&
            optional( read( p_num, alphabet_point() ) &&
            set( n_digits_after_point, read( p_num, alphabet_digit() ) ) );

    if( n_digits_before_point + n_digits_after_point == 0 )
    {
        location.top();
        return false;
    }

    return true;
}

bool dsl_pa::get_float( double * p_float )
{
    std::string input;
    if( ! get_float( &input ) )
        return false;
    *p_float = atof( input.c_str() );
    return true;
}

bool dsl_pa::get_float( float * p_float )
{
    double double_value;
    if( get_float( &double_value ) )
    {
        *p_float = static_cast< float >( double_value );
        return true;
    }
    return false;
}

bool dsl_pa::get_sci_float( std::string * p_num )
{
    p_num->clear();
    return read_sci_float( p_num );
}

bool dsl_pa::read_sci_float( std::string * p_num )
{
    location_logger location( r_reader );

    if( read_float( p_num ) )
    {
        location_logger exponent_location( r_reader );

        std::string exponent;

        if( read( &exponent, alphabet_E(), 1 ) && read_int( &exponent ) )
            p_num->append( exponent );
        else
            location.top();

        return true;
    }

    location.top();
    return false;
}

bool dsl_pa::get_sci_float( float * p_float )
{
    double double_value;
    if( get_sci_float( &double_value ) )
    {
        *p_float = static_cast< float >( double_value );
        return true;
    }
    return false;
}

bool dsl_pa::get_sci_float( double * p_float )
{
    std::string input;
    if( ! get_sci_float( &input ) )
        return false;
    *p_float = atof( input.c_str() );
    return true;
}

size_t dsl_pa::get( std::string * p_output, const alphabet & r_alphabet )
{
    p_output->clear();
    return read( p_output, r_alphabet );
}

size_t dsl_pa::get( std::string * p_output, const alphabet & r_alphabet, size_t max_chars )
{
    p_output->clear();
    return read( p_output, r_alphabet, max_chars );
}

size_t dsl_pa::get_until( std::string * p_output, const alphabet & r_alphabet )
{
    p_output->clear();
    return read_until( p_output, r_alphabet );
}

size_t dsl_pa::get_bounded_until( std::string * p_output, const alphabet & r_alphabet, size_t max_chars )
{
    p_output->clear();
    return read_bounded_until( p_output, r_alphabet, max_chars );
}

size_t dsl_pa::get_escaped_until( std::string * p_output, const alphabet & r_alphabet, char escape_char )
{
    p_output->clear();
    return read_escaped_until( p_output, r_alphabet, escape_char );
}

size_t dsl_pa::get_until( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars )
{
    p_output->clear();
    return read_until( p_output, r_alphabet, escape_char, max_chars );
}

size_t dsl_pa::get( std::string * p_output, mutator & r_mutator )
{
    p_output->clear();
    return read( p_output, r_mutator );
}

struct writer_read_mode
{
    static void handle_char( std::string * p_output, char c ) { p_output->push_back( c ); }
    static void handle_string( std::string * p_output, const char * p_new ) { p_output->append( p_new ); }
};

struct writer_skip_mode
{
    static void handle_char( std::string * p_output, char c ) {}
    static void handle_string( std::string * p_output, const char * p_new ) {}
};

template< typename Twriter >
size_t dsl_pa::read_or_skip_handler( std::string * p_output, const alphabet & r_alphabet, size_t max_chars )
{
    size_t n_chars;

    for( n_chars = 0; n_chars < max_chars; ++n_chars )
    {
        if( ! r_alphabet.is_sought( get() ) )
            break;

        Twriter::handle_char( p_output, current() );
    }

    if( n_chars < max_chars )
        unget();

    return n_chars;

};

template< typename Twriter >
size_t dsl_pa::read_or_skip_until_handler( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars )
{
    size_t n_chars;
    bool is_escaped = false;

    for( n_chars = 0; n_chars < max_chars; ++n_chars )
    {
        if( get() == reader::R_EOI )
            return n_chars;

        if( ! is_escaped )
        {
            if( r_alphabet.is_sought( current() ) ) // For get_until(), 'sought' chars are unwanted!
                break;

            if( current() == escape_char )
                is_escaped = true;  // Escape chars are not collected in output
            else
                Twriter::handle_char( p_output, current() );
        }
        else
        {
            if( ! r_alphabet.is_sought( current() ) )   // If we didn't get [escape][sought] then add [escape] to string
                Twriter::handle_char( p_output, escape_char );
            Twriter::handle_char( p_output, current() );
            is_escaped = false;
        }
    }

    if( n_chars < max_chars )
        unget();

    return n_chars;
}

template< typename Twriter >
size_t dsl_pa::read_or_skip_handler( std::string * p_output, mutator & r_mutator )
{
    size_t n_chars_read = 0;
    for( ;; )
    {
        if( get() == reader::R_EOI )
            return r_mutator.got_eof() ? n_chars_read : 0;

        const char * p_new = r_mutator( current() );
        if( ! p_new )
        {
            unget();
            break;
        }

        Twriter::handle_string( p_output, p_new );
        ++n_chars_read;
    }

    return n_chars_read;
};

size_t dsl_pa::read( std::string * p_output, const alphabet & r_alphabet )
{
    return read( p_output, r_alphabet, unbounded );
}

size_t dsl_pa::read( std::string * p_output, const alphabet & r_alphabet, size_t max_chars )
{
    return read_or_skip_handler< writer_read_mode >( p_output, r_alphabet, max_chars );
}

size_t dsl_pa::read_until( std::string * p_output, const alphabet & r_alphabet )
{
    return read_until( p_output, r_alphabet, '\0', unbounded );
}

size_t dsl_pa::read_bounded_until( std::string * p_output, const alphabet & r_alphabet, size_t max_chars )
{
    return read_until( p_output, r_alphabet, '\0', max_chars );
}

size_t dsl_pa::read_escaped_until( std::string * p_output, const alphabet & r_alphabet, char escape_char )
{
    return read_until( p_output, r_alphabet, escape_char, unbounded );
}

size_t dsl_pa::read_until( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars )
{
    return read_or_skip_until_handler< writer_read_mode >( p_output, r_alphabet, escape_char, max_chars );
}

size_t dsl_pa::read( std::string * p_output, mutator & r_mutator )
{
    return read_or_skip_handler< writer_read_mode >( p_output, r_mutator );
}

size_t dsl_pa::skip( const alphabet & r_alphabet )
{
    return skip( r_alphabet, unbounded );
}

size_t dsl_pa::skip( const alphabet & r_alphabet, size_t max_chars )
{
    return read_or_skip_handler< writer_skip_mode >( 0, r_alphabet, max_chars );
}

size_t dsl_pa::skip_until( const alphabet & r_alphabet )
{
    return skip_until( r_alphabet, '\0', unbounded );
}

size_t dsl_pa::skip_bounded_until( const alphabet & r_alphabet, size_t max_chars )
{
    return skip_until( r_alphabet, '\0', max_chars );
}

size_t dsl_pa::skip_escaped_until( const alphabet & r_alphabet, char escape_char )
{
    return skip_until( r_alphabet, escape_char, unbounded );
}

size_t dsl_pa::skip_until( const alphabet & r_alphabet, char escape_char, size_t max_chars )
{
    return read_or_skip_until_handler< writer_skip_mode >( 0, r_alphabet, escape_char, max_chars );
}

size_t dsl_pa::skip( mutator & r_mutator )
{
    return read_or_skip_handler< writer_skip_mode >( 0, r_mutator );
}

bool dsl_pa::accumulate( const alphabet & r_alphabet )
{
    if( r_alphabet.is_sought( get() ) )
    {
        if( p_accumulator )
            *p_accumulator += current();
        return true;
    }
    unget();
    return false;
}

bool dsl_pa::fixed( const char * p_seeking )
{
    return get_fixed( 0, p_seeking );
}

bool dsl_pa::ifixed( const char * p_seeking )
{
    return get_ifixed( 0, p_seeking );
}

bool dsl_pa::get_fixed( std::string * p_output, const char * p_seeking )
{
    if( p_output )
        p_output->clear();
    return read_fixed( p_output, p_seeking );
}

bool dsl_pa::get_ifixed( std::string * p_output, const char * p_seeking )
{
    if( p_output )
        p_output->clear();
    return read_ifixed( p_output, p_seeking );
}

struct compare_fixed
{
    static bool compare( char in_char, char sought_char )
    {
        return in_char == sought_char;
    }
};

struct compare_ifixed
{
    static bool compare( char in_char, char sought_char )
    {
        return (is_7bit( in_char ) && tolower( in_char ) == tolower( sought_char ))
                || (! is_7bit( in_char ) && in_char == sought_char);
    }
};

template< class Tcomparer >
bool dsl_pa::read_fixed_or_ifixed( std::string * p_output, const char * p_seeking )
{
    std::string read;

    location_logger location( r_reader );

    for( ; *p_seeking != '\0'; ++p_seeking )
    {
        if( Tcomparer::compare( get(), *p_seeking ) )
        {
            if( p_output )  // No point updating local store if output not wanted
                read.push_back( current() );
        }
        else
        {
            location.top();
            return false;
        }
    }

    if( p_output )
        p_output->append( read );

    return true;
}

bool dsl_pa::read_fixed( std::string * p_output, const char * p_seeking )
{
    return read_fixed_or_ifixed< compare_fixed >( p_output, p_seeking );
}

bool dsl_pa::read_ifixed( std::string * p_output, const char * p_seeking )
{
    return read_fixed_or_ifixed< compare_ifixed >( p_output, p_seeking );
}

} // End of namespace cl
