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
	
	TTEST( my_alphabet.is_wanted( 'b' ) );
	TTEST( ! my_alphabet.is_wanted( '$' ) );
	TTEST( ! my_alphabet.is_wanted( ' ' ) );
	
	TDOC( "Check word char alphabet polymorphism!" );
	alphabet * p_my_alphabet = &my_alphabet;
	
	TTEST( p_my_alphabet->is_wanted( 'b' ) );
	TTEST( ! p_my_alphabet->is_wanted( '$' ) );
	TTEST( ! p_my_alphabet->is_wanted( ' ' ) );
	
	TDOC( "Whitespace alphabet" );
	alphabet_space my_space_alphabet;
	
	TTEST( my_space_alphabet.is_wanted( ' ' ) );
	TTEST( my_space_alphabet.is_wanted( '\t' ) );
	TTEST( ! my_space_alphabet.is_wanted( 'A' ) );
	
	TDOC( "Digit alphabet" );
	alphabet_digit my_digit_alphabet;
	TTEST( my_digit_alphabet.is_wanted( '0' ) );
	TTEST( my_digit_alphabet.is_wanted( '5' ) );
	TTEST( my_digit_alphabet.is_wanted( '9' ) );
	TTEST( ! my_digit_alphabet.is_wanted( 'a' ) );
	
	TDOC( "Hex alphabet" );
	alphabet_hex my_hex_alphabet;
	TTEST( my_hex_alphabet.is_wanted( '0' ) );
	TTEST( my_hex_alphabet.is_wanted( '5' ) );
	TTEST( my_hex_alphabet.is_wanted( '9' ) );
	TTEST( my_hex_alphabet.is_wanted( 'A' ) );
	TTEST( my_hex_alphabet.is_wanted( 'C' ) );
	TTEST( my_hex_alphabet.is_wanted( 'F' ) );
	TTEST( my_hex_alphabet.is_wanted( 'a' ) );
	TTEST( my_hex_alphabet.is_wanted( 'c' ) );
	TTEST( my_hex_alphabet.is_wanted( 'f' ) );
	TTEST( ! my_hex_alphabet.is_wanted( 'G' ) );
}

TFUNCTION( alphabet_combiners_test )
{
	TBEGIN( "Alphabet combiners tests" );
	
	TTEST( alphabet_digit().is_wanted( '1' ) );
	TTEST( alphabet_not( alphabet_digit() ).is_wanted( 'x' ) );
	TTEST( alphabet_or( alphabet_space(), alphabet_digit() ).is_wanted( '5' ) );
	TTEST( alphabet_or( alphabet_space(), alphabet_digit() ).is_wanted( ' ' ) );
	TTEST( ! alphabet_or( alphabet_space(), alphabet_digit() ).is_wanted( 'x' ) );
	TTEST( alphabet_not( alphabet_or( alphabet_space(), alphabet_digit() ) ).is_wanted( 'x' ) );
	
	// OR together more than two alphabets as follows:
	TTEST( alphabet_or( alphabet_space(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_wanted( ' ' ) );
	TTEST( alphabet_or( alphabet_space(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_wanted( '5' ) );
	TTEST( alphabet_or( alphabet_space(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_wanted( '\x80' ) );
	TTEST( ! alphabet_or( alphabet_space(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_wanted( '$' ) );
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
	
	TTEST( number().is_wanted( '1' ) );
	TTEST( name().is_wanted( 'g' ) );
	TTEST( ! name().is_wanted( '1' ) );
	TTEST( invert( number() ).is_wanted( 'x' ) );
	TTEST( either( name(), number() ).is_wanted( '5' ) );
	TTEST( either( name(), number() ).is_wanted( 'j' ) );
}

TFUNCTION( alphabet_short_alphabets_test )
{
	TBEGIN( "Alphabet short_alphabets tests" );
	
	// As a general library it is important to be aware of the potential for
	// name clashes. However, optionally using namespace cl::short_alphabets
	// allows you to use shorted names.  Note that using namespace can be 
	// used on a per function basis.
	
	using namespace cl::short_alphabets;

	TTEST( digit().is_wanted( '1' ) );
	TTEST( not( digit() ).is_wanted( 'x' ) );
	TTEST( or( space(), digit() ).is_wanted( '5' ) );
	TTEST( alpha().is_wanted( 'a' ) );
	
	TTEST( linear_space().is_wanted( ' ' ) );
	TTEST( linear_space().is_wanted( '\t' ) );
	TTEST( ! linear_space().is_wanted( 'x' ) );
	TTEST( sign().is_wanted( '+' ) );
	TTEST( sign().is_wanted( '-' ) );
	TTEST( ! sign().is_wanted( 'x' ) );
	TTEST( point().is_wanted( '.' ) );
	TTEST( ! point().is_wanted( 'x' ) );
	TTEST( dot().is_wanted( '.' ) );
	TTEST( ! dot().is_wanted( 'x' ) );
	TTEST( exponent().is_wanted( 'e' ) );
	TTEST( exponent().is_wanted( 'E' ) );
	TTEST( ! exponent().is_wanted( 'x' ) );
	TTEST( dash().is_wanted( '-' ) );
	TTEST( ! dash().is_wanted( 'x' ) );
	TTEST( colon().is_wanted( ':' ) );
	TTEST( ! colon().is_wanted( 'x' ) );
	TTEST( semicolon().is_wanted( ';' ) );
	TTEST( ! semicolon().is_wanted( 'x' ) );
	TTEST( comma().is_wanted( ',' ) );
	TTEST( ! comma().is_wanted( 'x' ) );
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

TFUNCTION( alphabet_char_index_test )
{
	TBEGIN( " Alphabet char_index tests" );
	
	char_index my_index;
	
	TTEST( ! my_index.is_set( 'a' ) );
	TTEST( ! my_index.is_set( 'b' ) );
	TTEST( ! my_index.is_set( 'c' ) );
	TTEST( ! my_index.is_set( 'd' ) );
	TTEST( ! my_index.is_set( 'e' ) );
	TTEST( ! my_index.is_set( 'f' ) );
	
	my_index.set_range( 'b', 'e' );
	
	TTEST( ! my_index.is_set( 'a' ) );
	TTEST( my_index.is_set( 'b' ) );
	TTEST( my_index.is_set( 'c' ) );
	TTEST( my_index.is_set( 'd' ) );
	TTEST( my_index.is_set( 'e' ) );
	TTEST( ! my_index.is_set( 'f' ) );
	
	my_index.set_range( 'g', 'j' );
	
	TTEST( ! my_index.is_set( 'a' ) );
	TTEST( my_index.is_set( 'b' ) );
	TTEST( my_index.is_set( 'c' ) );
	TTEST( my_index.is_set( 'd' ) );
	TTEST( my_index.is_set( 'e' ) );
	TTEST( ! my_index.is_set( 'f' ) );
	TTEST( my_index.is_set( 'g' ) );
	TTEST( my_index.is_set( 'h' ) );
	TTEST( my_index.is_set( 'i' ) );
	TTEST( my_index.is_set( 'j' ) );
	TTEST( ! my_index.is_set( 'k' ) );
	
	my_index.set_inverted_range( 'w', 'z' );
	
	TTEST( my_index.is_set( 'a' ) );
	TTEST( my_index.is_set( 'b' ) );
	TTEST( my_index.is_set( 'c' ) );
	TTEST( my_index.is_set( 'd' ) );
	TTEST( my_index.is_set( 'e' ) );
	TTEST( my_index.is_set( 'f' ) );
	TTEST( my_index.is_set( 'g' ) );
	TTEST( my_index.is_set( 'h' ) );
	TTEST( my_index.is_set( 'i' ) );
	TTEST( my_index.is_set( 'j' ) );
	TTEST( my_index.is_set( 'k' ) );
	TTEST( ! my_index.is_set( 'w' ) );
	TTEST( ! my_index.is_set( 'x' ) );
	TTEST( ! my_index.is_set( 'y' ) );
	TTEST( ! my_index.is_set( 'z' ) );
	
	TTEST( ! my_index.is_set( 'x' ) );
	my_index.set( 'x' );
	TTEST( my_index.is_set( 'x' ) );
	
	my_index.invert();
	
	TTEST( ! my_index.is_set( 'a' ) );
	TTEST( ! my_index.is_set( 'b' ) );
	TTEST( ! my_index.is_set( 'c' ) );
	TTEST( ! my_index.is_set( 'd' ) );
	TTEST( ! my_index.is_set( 'e' ) );
	TTEST( ! my_index.is_set( 'f' ) );
	TTEST( ! my_index.is_set( 'g' ) );
	TTEST( ! my_index.is_set( 'h' ) );
	TTEST( ! my_index.is_set( 'i' ) );
	TTEST( ! my_index.is_set( 'j' ) );
	TTEST( ! my_index.is_set( 'k' ) );
	TTEST( my_index.is_set( 'w' ) );
	TTEST( ! my_index.is_set( 'x' ) );
	TTEST( my_index.is_set( 'y' ) );
	TTEST( my_index.is_set( 'z' ) );
	
	char_index other_index;
	
	other_index.set_range( 'b', 'e' );
	
	my_index.merge( other_index );

	TTEST( ! my_index.is_set( 'a' ) );
	TTEST( my_index.is_set( 'b' ) );
	TTEST( my_index.is_set( 'c' ) );
	TTEST( my_index.is_set( 'd' ) );
	TTEST( my_index.is_set( 'e' ) );
	TTEST( ! my_index.is_set( 'f' ) );
	
	my_index.clear();
	TTEST( ! my_index.is_set( 'a' ) );
	TTEST( ! my_index.is_set( 'b' ) );
	TTEST( ! my_index.is_set( 'c' ) );
	TTEST( ! my_index.is_set( 'd' ) );
	TTEST( ! my_index.is_set( 'e' ) );
	TTEST( ! my_index.is_set( 'f' ) );
	TTEST( ! my_index.is_set( 'g' ) );
	TTEST( ! my_index.is_set( 'h' ) );

	TDOC( "Test chained operations" );
	
	my_index.merge( char_index().set_range( 'b', 'd' ).set_range( 'f', 'h' ).invert() );
	
	TTEST( my_index.is_set( 'a' ) );
	TTEST( ! my_index.is_set( 'b' ) );
	TTEST( my_index.is_set( 'e' ) );
	TTEST( ! my_index.is_set( 'f' ) );
}

TFUNCTION( alphabet_char_class_test )
{
	TBEGIN( " Alphabet_char_class tests" );
	
	TSETUP( alphabet_char_class my_char_class( "a-z" ) );
	TTEST( my_char_class.is_wanted( 'a' ) );
	TTEST( ! my_char_class.is_wanted( '$' ) );
}
