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

#include <sstream>

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

bool dsl_pa::is_get_char_in( const alphabet & r_alphabet )    // Will unget() if got char is not c
{
    if( peek_is_in( r_alphabet ) )
        return get_char();
    return false;
}

bool dsl_pa::is_current( int c ) const
{
    return current() == c;
}

bool dsl_pa::is_current_in( const alphabet & r_alphabet ) const
{
    return r_alphabet.is_sought( current() );
}

bool dsl_pa::is_peek( int c )
{
    return peek() == c;
}

bool dsl_pa::is_peek_in( const alphabet & r_alphabet )
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
    locator location( r_reader );

    size_t n_sign_chars = read( p_num, alphabet_sign(), 1 );
    size_t n_digits = read( p_num, alphabet_digit() );

    if( n_digits == 0 )
    {
        location_top();
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
    locator location( r_reader );

    size_t n_digits_before_point = 0, n_digits_after_point = 0;

    optional( read( p_num, alphabet_sign(), 1 ) ) &&
            set( n_digits_before_point, read( p_num, alphabet_digit() ) ) &&
            optional( read( p_num, alphabet_point() ) &&
            set( n_digits_after_point, read( p_num, alphabet_digit() ) ) );

    if( n_digits_before_point + n_digits_after_point == 0 )
    {
        location_top();
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
    locator location( r_reader );

    if( read_float( p_num ) )
    {
        locator exponent_location( r_reader );

        std::string exponent;

        if( read( &exponent, alphabet_E(), 1 ) && read_int( &exponent ) )
            p_num->append( exponent );
        else
            location_top();

        return true;
    }

    location_top();
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

class QStringParser : public dsl_pa
{
    // This is a JSON q-string parser, that can have the delimiters specified.
    //
    //  ; The defintion of a JSON string, from RFC 4627 s 2
    //  q-string        = %x20-21 / %x23-5B / %x5D-10FFFF / "\" (
    //                     %x22 /      ; "  u+0022
    //                     %x5C /      ; \  u+005C
    //                     %x2F /      ; /  u+002F
    //                     %x62 /      ; BS u+0008
    //                     %x66 /      ; FF u+000C
    //                     %x6E /      ; LF u+000A
    //                     %x72 /      ; CR u+000D
    //                     %x74 /      ; HT u+0009
    //                     ( %x75 4HEXDIG ) ) ; uXXXX u+XXXX
    //
    // In this class, %x22/" is the default string delimiter, but an
    // alternative can be specified at construction time.

private:
    class AlphabetIsUnescapedAscii : public cl::alphabet
    {
        char delimiter;
    public:
        AlphabetIsUnescapedAscii( char delimiter_in ) : delimiter( delimiter_in ) {}
        virtual bool is_sought( char c ) const { return (c & 0x80) == 0 && c != reader::R_EOI && c != '\\' && c != delimiter; }
    };

    struct Members
    {
        std::string * p_v;
        char delimiter;
        AlphabetIsUnescapedAscii alphabet_is_unescaped_ascii;
        bool is_errored;

        Members( std::string * p_v_in, char delimiter_in )
            :
            p_v( p_v_in ),
            delimiter( delimiter_in ),
            alphabet_is_unescaped_ascii( delimiter_in ),
            is_errored( false )
        {}
    } m;

public:
    QStringParser( cl::dsl_pa * p_dsl_pa, std::string * p_v, char delimiter = '"' )
        : dsl_pa( p_dsl_pa->get_reader() ), m( p_v, delimiter )
    {}

    bool read() // Assumes we have already consumed the opening quotation mark
    {
        cl::locator loc( this );

        cl::accumulator q_string_accumulator( this );

        star_qs_char();

        if( peek() != m.delimiter )
            error();

        *m.p_v = q_string_accumulator.get();

        if( m.is_errored )
            location_top();

        return ! m.is_errored;
    }

    bool is_errored() const
    {
        return m.is_errored;
    }

private:
    bool star_qs_char()
    {
        while( qs_char() )
        {}

        return true;
    }

    bool qs_char()
    {
        // qs_char     = unescaped /
        //               escape (
        //               %x22 /          ; "    quotation mark  U+0022
        //               %x5C /          ; \    reverse solidus U+005C
        //               %x2F /          ; /    solidus         U+002F
        //               %x62 /          ; b    backspace       U+0008
        //               %x66 /          ; f    form feed       U+000C
        //               %x6E /          ; n    line feed       U+000A
        //               %x72 /          ; r    carriage return U+000D
        //               %x74 /          ; t    tab             U+0009
        //               %x75 4HEXDIG )  ; uXXXX                U+XXXX
        //
        // Where %x22/" is the default string delimiter, but an
        // alternative can be specified at construction time.

        return unescaped() || (escape() && (escaped_code() || (u() && escaped_hex_code()) || error()));
    }

    bool unescaped()
    {
        return unescaped_ascii() || unescaped_utf8();
    }

    bool unescaped_ascii()
    {
        return accumulate( m.alphabet_is_unescaped_ascii );
    }

    static bool is_unescaped_leading_utf8( char c )
    {
        // unescaped-utf8        = %x80-FF

        return static_cast<unsigned char>(c) >= 0xc2;
    }

    static bool is_unescaped_continuation_utf8( char c )
    {
        // unescaped-utf8        = %x80-FF

        return static_cast<unsigned char>(c) >= 0x80 && static_cast<unsigned char>(c) <= 0xbf;
    }

    bool unescaped_utf8()
    {
        // unescaped-utf8        = %x80-10FFFF

        cl::accumulator utf8_accumulator( this );

        if( accumulate( cl::alphabet_function( is_unescaped_leading_utf8 ) ) )
        {
            while( accumulate( cl::alphabet_function( is_unescaped_continuation_utf8 ) ) )
            {}

            unsigned char c1 = static_cast<unsigned char>( utf8_accumulator.get()[0] );
            unsigned char c2 = static_cast<unsigned char>( utf8_accumulator.get()[1] );
            size_t length = utf8_accumulator.get().length();

            if( c1 == 0xef && c2 == 0xbe && ( utf8_accumulator.get()[2] == 0xbe || utf8_accumulator.get()[2] == 0xbf ) ) // U+FFFE & U+FFFF are illegal
                return error();

            // From RFC 3629 - Make sure sequence is valid (e.g. doesn't encode a surrogate etc.)
            // UTF8-octets = *( UTF8-char )
            // UTF8-char   = UTF8-1 / UTF8-2 / UTF8-3 / UTF8-4
            // UTF8-1      = %x00-7F - (Handled outside this function)
            // UTF8-2      = %xC2-DF UTF8-tail
            // UTF8-3      = %xE0 %xA0-BF UTF8-tail / %xE1-EC 2( UTF8-tail ) /
            //               %xED %x80-9F UTF8-tail / %xEE-EF 2( UTF8-tail )
            // UTF8-4      = %xF0 %x90-BF 2( UTF8-tail ) / %xF1-F3 3( UTF8-tail ) /
            //               %xF4 %x80-8F 2( UTF8-tail )
            // UTF8-tail   = %x80-BF
            if( (c1 >= 0xc2 && c1 <= 0xdf && length == 2) ||
                    (c1 == 0xe0 && (c2 >= 0xa0 && c2 <= 0xbf) && length == 3) ||
                    (c1 >= 0xe1 && c1 <= 0xec && length == 3) ||
                    (c1 == 0xed && (c2 >= 0x80 && c2 <= 0x9f) && length == 3) ||
                    (c1 >= 0xee && c1 <= 0xef && length == 3) ||
                    (c1 == 0xf0 && (c2 >= 0x90 && c2 <= 0xbf) && length == 4) ||
                    (c1 >= 0xf1 && c1 <= 0xf3 && length == 4) ||
                    (c1 == 0xf4 && (c2 >= 0x80 && c2 <= 0x8f) && length == 4) )
            {
                utf8_accumulator.append_to_previous();

                return true;
            }

            return error();
        }

        return false;
    }

    bool escape()
    {
        // escape           = %x5C              ; '\'

        return is_get_char( '\\' );
    }

    bool escaped_code()
    {
        char c = peek();
        return ( (c == m.delimiter && accumulator_append( m.delimiter )) ||
                (c == '\\' && accumulator_append( '\\' )) ||
                (c == '/' && accumulator_append( '/' )) ||
                (c == 'b' && accumulator_append( '\b' )) ||
                (c == 'f' && accumulator_append( '\f' )) ||
                (c == 'n' && accumulator_append( '\n' )) ||
                (c == 'r' && accumulator_append( '\r' )) ||
                (c == 't' && accumulator_append( '\t' )) )
                && get();   // Consume peeked character if it's one we want
    }

    bool u()
    {
        return is_get_char( 'u' );
    }

    bool escaped_hex_code()
    {
        cl::accumulator hex_code_accumulator( this );
        if( four_HEXDIG() )
        {
            hex_code_accumulator.previous();
            int code_point = code_point_from_hex( hex_code_accumulator.get() );
            if( is_high_surrogate( code_point ) )
            {
                return complete_surrogate_pair( code_point );
            }
            else if( is_low_surrogate( code_point ) )
            {
                return error();
            }
            else
            {
                accumulator_append( MakeUTF8( code_point_from_hex( hex_code_accumulator.get() ) ).get() );
            }
            return true;
        }

        return false;
    }

    bool complete_surrogate_pair( int high_surrogate_code_point )
    {
        cl::accumulator hex_code_accumulator( this );

        if( escape() && u() && four_HEXDIG() )
        {
            int low_surrogate_code_point = code_point_from_hex( hex_code_accumulator.get() );
            if( ! is_low_surrogate( low_surrogate_code_point ) )
                return error();

            int combined_code_point = code_point_from_surrogates( high_surrogate_code_point, low_surrogate_code_point );
            hex_code_accumulator.previous();
            accumulator_append( MakeUTF8( combined_code_point ).get() );

            return true;
        }

        return error();
    }

    bool four_HEXDIG()
    {
        for( size_t i=0; i<4; ++i )
            if( ! HEXDIG() )
                return error();
        return true;
    }

    bool HEXDIG()
    {
        // HEXDIG() = DIGIT() || "A" || "B" || "C" || "D" || "E" || "F"

        return accumulate( cl::alphabet_hex() );
    }

    bool DIGIT()
    {
        // DIGIT() = %x30-39      ; 0-9

        return accumulate( cl::alphabet_digit() );
    }

    bool error() { m.is_errored = true; return false; }
};

bool dsl_pa::get_qstring_contents( std::string * p_string, char delimiter /*= '"'*/ )
{
    return QStringParser( this, p_string, delimiter ).read();
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
    static void handle_char( std::string * /*p_output*/, char /*c*/ ) {}
    static void handle_string( std::string * /*p_output*/, const char * /*p_new*/ ) {}
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

    locator location( r_reader );

    for( ; *p_seeking != '\0'; ++p_seeking )
    {
        if( Tcomparer::compare( get(), *p_seeking ) )
        {
            if( p_output )  // No point updating local store if output not wanted
                read.push_back( current() );
        }
        else
        {
            location_top();
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

bool dsl_pa::accumulate( char c )
{
    if( is_get_char( c ) )
    {
        if( p_accumulator )
            p_accumulator->append( c );
        return true;
    }
    return false;
}

bool dsl_pa::accumulate( const alphabet & r_alphabet )
{
    if( r_alphabet.is_sought( get() ) )
    {
        if( p_accumulator )
            p_accumulator->append( current() );
        return true;
    }
    unget();
    return false;
}

size_t dsl_pa::accumulate_all( const alphabet & r_alphabet )
{
    size_t num = 0;
    while( accumulate( r_alphabet ) )
        ++num;
    return num;
}

bool dsl_pa::accumulator_append( char c )
{
    if( p_accumulator )
        p_accumulator->append( c );
    return true;
}

bool dsl_pa::accumulator_append( const char * s )
{
    if( p_accumulator )
        p_accumulator->append( s );
    return true;
}

bool dsl_pa::accumulator_append( const std::string & r_s )
{
    if( p_accumulator )
        p_accumulator->append( r_s );
    return true;
}

bool dsl_pa::accumulator_append( const accumulator_deferred & r_a )
{
    if( p_accumulator )
        p_accumulator->append( r_a.get() );
    return true;
}

bool dsl_pa::accumulator_to_previous()
{
    if( p_accumulator )
        p_accumulator->append_to_previous();
    return true;
}

bool dsl_pa::accumulate_atomic( bool is_valid )
{
    if( is_valid )
        accumulator_to_previous();
    return is_valid;
}

//----------------------------------------------------------------------------
//                             accumulator implementation
//----------------------------------------------------------------------------

int64 accumulator_deferred::to_int64() const
{
    int64 v;
    std::istringstream( my_accumulator ) >> v;
    return v;
}

uint64 accumulator_deferred::to_uint64() const
{
    uint64 v;
    std::istringstream( my_accumulator ) >> v;
    return v;
}

//----------------------------------------------------------------------------
//                             Unicode utilities
//----------------------------------------------------------------------------

int code_point_from_hex( const std::string & r_hex_string )
{
    int code_point = 0;

    for( size_t i=0; i < r_hex_string.size(); ++i )
    {
        char c = r_hex_string[i];
        int new_value = 0;
        if( isdigit( c ) )
            new_value = c - '0';
        else if( c >= 'a' && c <= 'f' )
            new_value = c - 'a' + 10;
        else if( c >= 'A' && c <= 'F' )
            new_value = c - 'A' + 10;
        code_point = code_point * 16 + new_value;
    }

    return code_point;
}

bool is_high_surrogate( int code_point )
{
    return code_point >= 0xD800 && code_point <= 0xDBFF;
}

bool is_low_surrogate( int code_point )
{
    return code_point >= 0xDC00 && code_point <= 0xDFFF;
}

int code_point_from_surrogates( int high_surrogate, int low_surrogate )
{
    return ((high_surrogate & 0x3ff) << 10) + (low_surrogate & 0x3ff) + 0x10000;
}

MakeUTF8::MakeUTF8( int code_point )
{
    // From rfc3629:
    // Char. number range  |        UTF-8 octet sequence
    //   (hexadecimal)     |              (binary)
    // --------------------+---------------------------------------------
    // 0000 0000-0000 007F | 0xxxxxxx
    // 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    // 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    // 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

    if( code_point <= 0x7f )
        pack_ascii( code_point );
    else if( code_point >= 0x10000 )
        pack( '\xf0', 4, code_point );
    else if( code_point >= 0x00800 )
        pack( '\xe0', 3, code_point );
    else if( code_point >= 0x00080 )
        pack( '\xc0', 2, code_point );
    else
        utf8[0] = '\0';
}

void MakeUTF8::pack_ascii( int code_point )
{
    utf8[0] = code_point;
    utf8[1] = '\0';
}

void MakeUTF8::pack( char marker, size_t length, int code_point )
{
    utf8[length] = '\0';
    int scaled_code_point = code_point;
    for( int i = length-1; i >= 0; --i )
    {
        utf8[i] = scaled_code_point & 0x3f;
        utf8[i] |= 0x80;
        scaled_code_point >>= 6;
    }
    utf8[0] |= marker;
}

} // End of namespace cl
