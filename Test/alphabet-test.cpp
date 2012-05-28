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

#include "clunit.h"

#include "dsl-pa/dsl-pa.h"

using namespace cl;

TFUNCTION( alphabet_test )
{
	TBEGIN( "Alphabet tests" );
	
	TDOC( "Word char alphabet" );
	alphabet_word_char my_alphabet;
	
	TTEST( my_alphabet.is_sought( 'b' ) );
	TTEST( ! my_alphabet.is_sought( '$' ) );
	TTEST( ! my_alphabet.is_sought( ' ' ) );
	
	TDOC( "Check word char alphabet polymorphism!" );
	alphabet * p_my_alphabet = &my_alphabet;
	
	TTEST( p_my_alphabet->is_sought( 'b' ) );
	TTEST( ! p_my_alphabet->is_sought( '$' ) );
	TTEST( ! p_my_alphabet->is_sought( ' ' ) );
	
	TDOC( "Whitespace alphabet" );
	alphabet_space my_space_alphabet;
	
	TTEST( my_space_alphabet.is_sought( ' ' ) );
	TTEST( my_space_alphabet.is_sought( '\t' ) );
	TTEST( ! my_space_alphabet.is_sought( 'A' ) );
	
	TDOC( "Digit alphabet" );
	alphabet_digit my_digit_alphabet;
	TTEST( my_digit_alphabet.is_sought( '0' ) );
	TTEST( my_digit_alphabet.is_sought( '5' ) );
	TTEST( my_digit_alphabet.is_sought( '9' ) );
	TTEST( ! my_digit_alphabet.is_sought( 'a' ) );
	
	TDOC( "Hex alphabet" );
	alphabet_hex my_hex_alphabet;
	TTEST( my_hex_alphabet.is_sought( '0' ) );
	TTEST( my_hex_alphabet.is_sought( '5' ) );
	TTEST( my_hex_alphabet.is_sought( '9' ) );
	TTEST( my_hex_alphabet.is_sought( 'A' ) );
	TTEST( my_hex_alphabet.is_sought( 'C' ) );
	TTEST( my_hex_alphabet.is_sought( 'F' ) );
	TTEST( my_hex_alphabet.is_sought( 'a' ) );
	TTEST( my_hex_alphabet.is_sought( 'c' ) );
	TTEST( my_hex_alphabet.is_sought( 'f' ) );
	TTEST( ! my_hex_alphabet.is_sought( 'G' ) );
}

TFUNCTION( alphabet_combiners_test )
{
	TBEGIN( "Alphabet combiners tests" );
	
	TTEST( alphabet_digit().is_sought( '1' ) );
	TTEST( alphabet_not( alphabet_digit() ).is_sought( 'x' ) );
	TTEST( alphabet_or( alphabet_space(), alphabet_digit() ).is_sought( '5' ) );
	TTEST( alphabet_or( alphabet_space(), alphabet_digit() ).is_sought( ' ' ) );
	TTEST( ! alphabet_or( alphabet_space(), alphabet_digit() ).is_sought( 'x' ) );
	TTEST( alphabet_not( alphabet_or( alphabet_space(), alphabet_digit() ) ).is_sought( 'x' ) );
	
	// OR together more than two alphabets as follows:
	TTEST( alphabet_or( alphabet_space(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_sought( ' ' ) );
	TTEST( alphabet_or( alphabet_space(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_sought( '5' ) );
	TTEST( alphabet_or( alphabet_space(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_sought( '\x80' ) );
	TTEST( ! alphabet_or( alphabet_space(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_sought( '$' ) );
}

TFUNCTION( alphabet_typedefed_test )
{
	TBEGIN( "Alphabet typedefed tests" );
	
	// At times it my be appropriate to assign a name to a alphabet that is
	// more appropriate to the language being pasred.  This can be done with
	// typedefs.
	
	typedef alphabet_digit	number;
	typedef alphabet_alpha	name;
	typedef alphabet_space	whitespace;
	typedef alphabet_not	invert;
	typedef alphabet_or		either;
	
	TTEST( number().is_sought( '1' ) );
	TTEST( name().is_sought( 'g' ) );
	TTEST( ! name().is_sought( '1' ) );
	TTEST( invert( number() ).is_sought( 'x' ) );
	TTEST( either( name(), number() ).is_sought( '5' ) );
	TTEST( either( name(), number() ).is_sought( 'j' ) );
}

TFUNCTION( alphabet_short_alphabets_test )
{
	TBEGIN( "Alphabet short_alphabets tests" );
	
	// As a general library it is important to be aware of the potential for
	// name clashes. However, optionally using namespace cl::short_alphabets
	// allows you to use shorted names.  Note that using namespace can be 
	// used on a per function basis.
	
	using namespace cl::short_alphabets;

	TTEST( digit().is_sought( '1' ) );
	TTEST( not( digit() ).is_sought( 'x' ) );
	TTEST( or( space(), digit() ).is_sought( '5' ) );
	TTEST( alpha().is_sought( 'a' ) );
	
	TTEST( linear_space().is_sought( ' ' ) );
	TTEST( linear_space().is_sought( '\t' ) );
	TTEST( ! linear_space().is_sought( 'x' ) );
	TTEST( sign().is_sought( '+' ) );
	TTEST( sign().is_sought( '-' ) );
	TTEST( ! sign().is_sought( 'x' ) );
	TTEST( point().is_sought( '.' ) );
	TTEST( ! point().is_sought( 'x' ) );
	TTEST( dot().is_sought( '.' ) );
	TTEST( ! dot().is_sought( 'x' ) );
	TTEST( exponent().is_sought( 'e' ) );
	TTEST( exponent().is_sought( 'E' ) );
	TTEST( ! exponent().is_sought( 'x' ) );
	TTEST( dash().is_sought( '-' ) );
	TTEST( ! dash().is_sought( 'x' ) );
	TTEST( colon().is_sought( ':' ) );
	TTEST( ! colon().is_sought( 'x' ) );
	TTEST( semicolon().is_sought( ';' ) );
	TTEST( ! semicolon().is_sought( 'x' ) );
	TTEST( comma().is_sought( ',' ) );
	TTEST( ! comma().is_sought( 'x' ) );
}

TFUNCTION( alphabet_char_to_size_t_test )
{
	TBEGIN( "Alphabet char_to_size_t tests" );
	
	TTEST( alphabet_helpers::char_to_size_t( 0 ) == 0 );
	TTEST( alphabet_helpers::char_to_size_t( 10 ) == 10 );
	TTEST( alphabet_helpers::char_to_size_t( -2 ) == 254 );
	
	using namespace cl::alphabet_helpers;

	TTEST( char_to_size_t( 30 ) == 30 );
}

TFUNCTION( alphabet_char_map_test )
{
	TBEGIN( " Alphabet char_map tests" );
	
	char_map my_map;
	
	TTEST( ! my_map.is_set( 'a' ) );
	TTEST( ! my_map.is_set( 'b' ) );
	TTEST( ! my_map.is_set( 'c' ) );
	TTEST( ! my_map.is_set( 'd' ) );
	TTEST( ! my_map.is_set( 'e' ) );
	TTEST( ! my_map.is_set( 'f' ) );
	
	my_map.set_range( 'b', 'e' );
	
	TTEST( ! my_map.is_set( 'a' ) );
	TTEST( my_map.is_set( 'b' ) );
	TTEST( my_map.is_set( 'c' ) );
	TTEST( my_map.is_set( 'd' ) );
	TTEST( my_map.is_set( 'e' ) );
	TTEST( ! my_map.is_set( 'f' ) );
	
	my_map.set_range( 'g', 'j' );
	
	TTEST( ! my_map.is_set( 'a' ) );
	TTEST( my_map.is_set( 'b' ) );
	TTEST( my_map.is_set( 'c' ) );
	TTEST( my_map.is_set( 'd' ) );
	TTEST( my_map.is_set( 'e' ) );
	TTEST( ! my_map.is_set( 'f' ) );
	TTEST( my_map.is_set( 'g' ) );
	TTEST( my_map.is_set( 'h' ) );
	TTEST( my_map.is_set( 'i' ) );
	TTEST( my_map.is_set( 'j' ) );
	TTEST( ! my_map.is_set( 'k' ) );
	
	my_map.set_inverted_range( 'w', 'z' );
	
	TTEST( my_map.is_set( 'a' ) );
	TTEST( my_map.is_set( 'b' ) );
	TTEST( my_map.is_set( 'c' ) );
	TTEST( my_map.is_set( 'd' ) );
	TTEST( my_map.is_set( 'e' ) );
	TTEST( my_map.is_set( 'f' ) );
	TTEST( my_map.is_set( 'g' ) );
	TTEST( my_map.is_set( 'h' ) );
	TTEST( my_map.is_set( 'i' ) );
	TTEST( my_map.is_set( 'j' ) );
	TTEST( my_map.is_set( 'k' ) );
	TTEST( ! my_map.is_set( 'w' ) );
	TTEST( ! my_map.is_set( 'x' ) );
	TTEST( ! my_map.is_set( 'y' ) );
	TTEST( ! my_map.is_set( 'z' ) );
	
	TTEST( ! my_map.is_set( 'x' ) );
	my_map.set( 'x' );
	TTEST( my_map.is_set( 'x' ) );
	
	my_map.invert();
	
	TTEST( ! my_map.is_set( 'a' ) );
	TTEST( ! my_map.is_set( 'b' ) );
	TTEST( ! my_map.is_set( 'c' ) );
	TTEST( ! my_map.is_set( 'd' ) );
	TTEST( ! my_map.is_set( 'e' ) );
	TTEST( ! my_map.is_set( 'f' ) );
	TTEST( ! my_map.is_set( 'g' ) );
	TTEST( ! my_map.is_set( 'h' ) );
	TTEST( ! my_map.is_set( 'i' ) );
	TTEST( ! my_map.is_set( 'j' ) );
	TTEST( ! my_map.is_set( 'k' ) );
	TTEST( my_map.is_set( 'w' ) );
	TTEST( ! my_map.is_set( 'x' ) );
	TTEST( my_map.is_set( 'y' ) );
	TTEST( my_map.is_set( 'z' ) );
	
	char_map other_index;
	
	other_index.set_range( 'b', 'e' );
	
	my_map.merge( other_index );

	TTEST( ! my_map.is_set( 'a' ) );
	TTEST( my_map.is_set( 'b' ) );
	TTEST( my_map.is_set( 'c' ) );
	TTEST( my_map.is_set( 'd' ) );
	TTEST( my_map.is_set( 'e' ) );
	TTEST( ! my_map.is_set( 'f' ) );
	
	my_map.clear();
	TTEST( ! my_map.is_set( 'a' ) );
	TTEST( ! my_map.is_set( 'b' ) );
	TTEST( ! my_map.is_set( 'c' ) );
	TTEST( ! my_map.is_set( 'd' ) );
	TTEST( ! my_map.is_set( 'e' ) );
	TTEST( ! my_map.is_set( 'f' ) );
	TTEST( ! my_map.is_set( 'g' ) );
	TTEST( ! my_map.is_set( 'h' ) );

	TDOC( "Test chained operations" );
	
	my_map.merge( char_map().set_range( 'b', 'd' ).set_range( 'f', 'h' ).invert() );
	
	TTEST( my_map.is_set( 'a' ) );
	TTEST( ! my_map.is_set( 'b' ) );
	TTEST( my_map.is_set( 'e' ) );
	TTEST( ! my_map.is_set( 'f' ) );
}

TFUNCTION( alphabet_char_class_test )
{
	TBEGIN( " Alphabet_char_class tests" );
	
	{
	TSETUP( alphabet_char_class my_char_class( "a-z" ) );
	TTEST( my_char_class.is_sought( 'a' ) );
	TTEST( ! my_char_class.is_sought( '$' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "a-dm-oxyz" ) );
	TTEST( my_char_class.is_sought( 'a' ) );
	TTEST( my_char_class.is_sought( 'd' ) );
	TTEST( ! my_char_class.is_sought( 'e' ) );
	TTEST( ! my_char_class.is_sought( 'l' ) );
	TTEST( my_char_class.is_sought( 'm' ) );
	TTEST( my_char_class.is_sought( 'o' ) );
	TTEST( ! my_char_class.is_sought( 'p' ) );
	TTEST( my_char_class.is_sought( 'x' ) );
	TTEST( my_char_class.is_sought( 'y' ) );
	TTEST( my_char_class.is_sought( 'z' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "^a-dm-oxyz" ) );
	TTEST( ! my_char_class.is_sought( 'a' ) );
	TTEST( ! my_char_class.is_sought( 'd' ) );
	TTEST( my_char_class.is_sought( 'e' ) );
	TTEST( my_char_class.is_sought( 'l' ) );
	TTEST( ! my_char_class.is_sought( 'm' ) );
	TTEST( ! my_char_class.is_sought( 'o' ) );
	TTEST( my_char_class.is_sought( 'p' ) );
	TTEST( ! my_char_class.is_sought( 'x' ) );
	TTEST( ! my_char_class.is_sought( 'y' ) );
	TTEST( ! my_char_class.is_sought( 'z' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "\\da-f" ) );
	TTEST( my_char_class.is_sought( 'a' ) );
	TTEST( my_char_class.is_sought( 'd' ) );
	TTEST( my_char_class.is_sought( '2' ) );
	TTEST( ! my_char_class.is_sought( 'l' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "~d" ) );
	TTEST( my_char_class.is_sought( '2' ) );
	TTEST( ! my_char_class.is_sought( 'l' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "~D" ) );
	TTEST( ! my_char_class.is_sought( '2' ) );
	TTEST( my_char_class.is_sought( 'l' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "~w" ) );
	TTEST( my_char_class.is_sought( '2' ) );
	TTEST( ! my_char_class.is_sought( '$' ) );
	TTEST( ! my_char_class.is_sought( '\0' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "~W" ) );
	TTEST( ! my_char_class.is_sought( '2' ) );
	TTEST( my_char_class.is_sought( '$' ) );
	TTEST( ! my_char_class.is_sought( '\0' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "~s" ) );
	TTEST( my_char_class.is_sought( ' ' ) );
	TTEST( ! my_char_class.is_sought( '$' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "~S" ) );
	TTEST( ! my_char_class.is_sought( ' ' ) );
	TTEST( my_char_class.is_sought( '$' ) );
	TTEST( ! my_char_class.is_sought( '\0' ) );
	}
	
	{
	TSETUP( alphabet_char_class my_char_class( "~d~w" ) );
	TTEST( my_char_class.is_sought( '2' ) );
	TTEST( my_char_class.is_sought( 'l' ) );
	}
	
	{
	// Combining two inverted alphabets is a bit non-sensical!
	// (Here \S would effectively overwrite the chars selected by \D.
	// Use "^\d\s" instead.)
	//TSETUP( alphabet_char_class my_char_class( "~D~S" ) );
	//TTEST( ! my_char_class.is_sought( '2' ) );
	//TTEST( ! my_char_class.is_sought( 'l' ) );
	}
}
