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

//----------------------------------------------------------------------------
// See dsl-pa.h and README.html at https://github.com/codalogic/dsl-pa for
// more information.
//----------------------------------------------------------------------------

#include "dsl-pa/dsl-pa-alphabet.h"

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

alphabet_char_class::alphabet_char_class( const char * p_char_class_spec )
{
/*
	char l_last_char = (char)0xff;

	while( *p_char_class_spec != '\0' )
	{
		if( *p_char_class_spec == '-' )			// Allow for ranges
		{
			p_char_class_spec++;

			DBG_ASSERT( *p_char_class_spec != '\0' );	// See note Construct-Assumption

			while( ++l_last_char <= *p_char_class_spec )
				oc_alphabet[l_last_char] = 1;
			p_char_class_spec++;
		}

		else if( *p_char_class_spec == '^' )		// Allow for specials
		{
			switch( *++p_char_class_spec )
			{
			case 'w':
				index.set_range( 'a', 'z' );
				index.set_range( 'A', 'Z' );
				index.set_range( '0', '9' );
				index.set( '_' );
			break;

			case 'l':
				{
				int l_c;
				for( l_c = 'a'; l_c <= 'z'; l_c++ )
					oc_alphabet[l_c] = 1;
				for( l_c = 'A'; l_c <= 'Z'; l_c++ )
					oc_alphabet[l_c] = 1;
				}
			break;

			case 'd':
				{
				int l_c;
				for( l_c = '0'; l_c <= '9'; l_c++ )
					oc_alphabet[l_c] = 1;
				}
			break;

			case 'u':
				{
				int l_c;
				for( l_c = 0x21; l_c <= 0xff; l_c++ )
					oc_alphabet[l_c] = 1;
				}
			break;

			case 's':
				oc_alphabet[' '] = 1;
				oc_alphabet['\t'] = 1;
				oc_alphabet['\r'] = 1;
				oc_alphabet['\n'] = 1;
			break;

			case '^':
				oc_alphabet['^'] = 1;
			break;

			case '-':
				oc_alphabet['-'] = 1;
			break;

			case '\0':
				DBG_ASSERT( 0 );			// An ill-formed alphabet
			break;

			default:
				oc_alphabet[*p_char_class_spec] = 1;
				}

			p_char_class_spec++;
		}

		else
		{
			last_char = *p_char_class_spec;
			index.set( *p_char_class_spec++ );
		}
	}

	if( is_inverted )
		index.invert();
*/
}

} // End of namespace cl
