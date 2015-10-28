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
// take advantage of the C++ logic shortcircuit operators such as && and ||.
// See the brief overview in dsl-pa.h or, for more information, README.html
// at https://github.com/codalogic/dsl-pa
//----------------------------------------------------------------------------

#ifndef CL_DSL_PA_ALPHABET
#define CL_DSL_PA_ALPHABET

#include <cctype>

#include "dsl-pa-reader.h"

namespace cl {

namespace /*cl::*/alphabet_helpers {
    // Helper functions.  <cctype> functions are often not 8-bit safe
    inline bool is_7bit( char c )
    {
        return c >= 0 && c <= 0x7f;
    }
    inline bool is_space( char c )
    {
        return is_7bit( c ) && isspace( c );
    }
    inline bool is_line_space( char c )
    {
        return c == ' ' || c == '\t';
    }
    inline bool is_eol( char c )
    {
        return c == '\r' || c == '\n';
    }
    inline bool is_digit( char c )
    {
        return '0' <= c && c <= '9';
    }
    inline bool is_alpha( char c )
    {
        return 'a' <= c && c <= 'z' ||
                'A' <= c && c <= 'Z';
    }
    inline bool is_hex( char c )
    {
        return is_digit( c ) ||
                'a' <= c && c <= 'f' ||
                'A' <= c && c <= 'F';
    }
    inline bool is_utf8_multibyte( char c )
    {
        return ! is_7bit( c );
    }
    inline size_t char_to_size_t( char c )
    {
        // Convert char to range 0 to 255. Helper for indexing into array.
        return static_cast< size_t >( static_cast< unsigned char >( c ) );
    }
}   // End of namespace cl::*/alphabet_helpers

class char_map
{
private:
    typedef char char_map_array[256];
    char_map_array index;

public:
    char_map();
    void clear();
    char_map & clear( char c );
    char_map & set( char c );
    char_map & set_range( char start, char end );
    char_map & set_inverted_range( char start, char end );
    char_map & invert();
    void merge( const char_map & r_rhs );

    bool is_set( char c ) const;
};

// Specialist char maps corresponding to the Perl \w, \d and \s expressions

class char_map_w : public char_map
{
public:
    char_map_w();
};

class char_map_d : public char_map
{
public:
    char_map_d();
};

class char_map_s : public char_map
{
public:
    char_map_s();
};

class char_map_l : public char_map
{
public:
    char_map_l();
};

class alphabet
{
public:
    virtual bool is_sought( char c ) const = 0;
};

class alphabet_char : public alphabet
{
private:
    char sought;

public:
    alphabet_char( char sought_in )
        : sought( sought_in )
    {}
    virtual bool is_sought( char c ) const
    {
        return c == sought;
    }
};

// This alphabet class takes a specification that mirrors a Perl character
// class, such as in /[a-fA-F\d]/.  To specify that char class you would
// do:
//
//      alphabet_char_class my_char_class( "a-fA-F\\d" );
//
// Note the C++ requirement for a double \ in the spec string.  To make this
// easier a ~ character can be used instead of \, e,g.:
//
//      alphabet_char_class my_char_class( "a-fA-F~d" );
//
// As in Perl, inverted char classes are supported by including a leading ^,
// e.g.:
//
//      alphabet_char_class my_char_class( "^a-f" );
//
// The Perl special char classes of \w, \W, \d, \D, \s and \S are supported.
// To specify \ do "\\\\", to specify ~ do "~~".

class alphabet_char_class : public alphabet
{
private:
    char_map wanted_chars;

public:
    alphabet_char_class( const char * p_char_class_spec );
    virtual bool is_sought( char c ) const
    {
        return wanted_chars.is_set( c );
    }

private:
    bool add_range( char start, char end );
    bool add_special_char_class( char key );
};

// Common alphabets

class alphabet_space : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return alphabet_helpers::is_space( c );
    }
};

class alphabet_line_space : public alphabet   // Non-newline space chars
{
public:
    virtual bool is_sought( char c ) const
    {
        return alphabet_helpers::is_line_space( c );
    }
};

class alphabet_eol : public alphabet    // End of Line
{
public:
    virtual bool is_sought( char c ) const
    {
        return alphabet_helpers::is_eol( c );
    }
};

class alphabet_digit : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return alphabet_helpers::is_digit( c );
    }
};

class alphabet_hex : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return alphabet_helpers::is_hex( c );
    }
};

class alphabet_alpha : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return alphabet_helpers::is_alpha( c );
    }
};

class alphabet_word_first_char : public alphabet    // Based on Perl's \w
{
public:
    virtual bool is_sought( char c ) const
    {
        return alphabet_helpers::is_alpha( c ) || '_' == c;
    }
};

class alphabet_word_char : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return alphabet_helpers::is_alpha( c ) ||
                alphabet_helpers::is_digit( c ) ||
                '_' == c;
    }
};

class alphabet_name_char : public alphabet_word_char
{
public:
    virtual bool is_sought( char c ) const
    {
        return '-' == c || alphabet_word_char::is_sought( c );
    }
};

class alphabet_uni : public alphabet // char is part of a non-ASCII Unicode sequence
{
public:
    virtual bool is_sought( char c ) const
    {
        return alphabet_helpers::is_utf8_multibyte( c );
    }
};

class alphabet_sign : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return c == '-' || c == '+';
    }
};

class alphabet_point : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return c == '.';
    }
};

typedef alphabet_point alphabet_dot;

class alphabet_dash : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return c == '-';
    }
};

class alphabet_colon : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return c == ':';
    }
};

class alphabet_semicolon : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return c == ';';
    }
};

class alphabet_comma : public alphabet
{
public:
    virtual bool is_sought( char c ) const
    {
        return c == ',';
    }
};

class alphabet_E : public alphabet  // For floating point numbers
{
public:
    virtual bool is_sought( char c ) const
    {
        return c == 'e' || c == 'E';
    }
};

class alphabet_T : public alphabet  // For ISO datetime
{
public:
    virtual bool is_sought( char c ) const
    {
        return c == 'T';
    }
};

// alphabet combiners - These classes allow combining alphabets.  However, it is
// probably more preferable to create your own alphabet classes than rely
// extensively on these classes.

class alphabet_not : public alphabet
{
private:
    const alphabet & r_alphabet;

public:
    alphabet_not( const alphabet & r_alphabet_in )
        : r_alphabet( r_alphabet_in )
    {}
    virtual bool is_sought( char c ) const
    {
        return c != reader::R_EOI && ! r_alphabet.is_sought( c );
    }
};

class alphabet_or : public alphabet
{
private:
    const alphabet & r_alphabet_1;
    const alphabet & r_alphabet_2;

public:
    alphabet_or(
            const alphabet & r_alphabet_1_in,
            const alphabet & r_alphabet_2_in )
        :
        r_alphabet_1( r_alphabet_1_in ),
        r_alphabet_2( r_alphabet_2_in )
    {}
    virtual bool is_sought( char c ) const
    {
        return r_alphabet_1.is_sought( c ) ||
                r_alphabet_2.is_sought( c );
    }
};

class alphabet_and : public alphabet
{
private:
    const alphabet & r_alphabet_1;
    const alphabet & r_alphabet_2;

public:
    alphabet_and(
            const alphabet & r_alphabet_1_in,
            const alphabet & r_alphabet_2_in )
        :
        r_alphabet_1( r_alphabet_1_in ),
        r_alphabet_2( r_alphabet_2_in )
    {}
    virtual bool is_sought( char c ) const
    {
        return r_alphabet_1.is_sought( c ) &&
                r_alphabet_2.is_sought( c );
    }
};

namespace /*cl::*/ short_alphabets {
    typedef alphabet_char               character;
    typedef alphabet_char_class         char_class;
    typedef alphabet_space              space;
    typedef alphabet_line_space         line_space;
    typedef alphabet_eol                end_of_line;
    typedef alphabet_digit              digit;
    typedef alphabet_hex                hex;
    typedef alphabet_alpha              alpha;
    typedef alphabet_word_first_char    word_first_char;
    typedef alphabet_word_char          word_char;
    typedef alphabet_uni                uni;
    typedef alphabet_not                not;
    typedef alphabet_or                 or;
    typedef alphabet_and                and;
    typedef alphabet_sign               sign;
    typedef alphabet_point              point;
    typedef alphabet_dot                dot;
    typedef alphabet_dash               dash;
    typedef alphabet_colon              colon;
    typedef alphabet_semicolon          semicolon;
    typedef alphabet_comma              comma;
    typedef alphabet_E                  exponent;
    // typedef alphabet_T               T;  // Not sensible to have a short version
} // End of namespace cl::short_alphabets

} // End of namespace cl

#endif // CL_DSL_PA_ALPHABET
