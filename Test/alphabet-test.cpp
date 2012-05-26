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
	alphabet_ws my_ws_alphabet;
	
	TTEST( my_ws_alphabet.is_wanted( ' ' ) );
	TTEST( my_ws_alphabet.is_wanted( '\t' ) );
	TTEST( ! my_ws_alphabet.is_wanted( 'A' ) );
	
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
	TTEST( alphabet_or( alphabet_ws(), alphabet_digit() ).is_wanted( '5' ) );
	TTEST( alphabet_or( alphabet_ws(), alphabet_digit() ).is_wanted( ' ' ) );
	TTEST( ! alphabet_or( alphabet_ws(), alphabet_digit() ).is_wanted( 'x' ) );
	TTEST( alphabet_not( alphabet_or( alphabet_ws(), alphabet_digit() ) ).is_wanted( 'x' ) );
	
	// OR together more than two alphabets as follows:
	TTEST( alphabet_or( alphabet_ws(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_wanted( ' ' ) );
	TTEST( alphabet_or( alphabet_ws(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_wanted( '5' ) );
	TTEST( alphabet_or( alphabet_ws(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_wanted( '\x80' ) );
	TTEST( ! alphabet_or( alphabet_ws(), alphabet_or( alphabet_digit(), alphabet_uni() ) ).is_wanted( '$' ) );
}

TFUNCTION( alphabet_typedefed_test )
{
	TBEGIN( "Alphabet typedefed tests" );
	
	// At times it my be appropriate to assign a name to a alphabet that is
	// more appropriate to the language being pasred.  This can be done with
	// typedefs.
	
	typedef alphabet_digit	number;
	typedef alphabet_alpha	name;
	typedef alphabet_ws		whitespace;
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
	TTEST( or( wspace(), digit() ).is_wanted( '5' ) );
	TTEST( alpha().is_wanted( 'a' ) );
}
