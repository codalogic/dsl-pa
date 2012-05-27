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

char_index::char_index()
{
	clear();
}

void char_index::clear()
{
	for( size_t i=0; i<sizeof( char_index_array ); ++i )
		index[i] = 0;
}

void char_index::set( char c )
{
	index[ char_to_size_t( c ) ] = 1;
}

void char_index::set_range( char start, char end )
{
	size_t start_index = char_to_size_t( start );
	size_t end_index = char_to_size_t( end );
	for( size_t i = start_index; i <= end_index; ++i )
		index[i] = 1;
}

void char_index::set_inverted_range( char start, char end )
{
	char_index non_invert_char_index;

	non_invert_char_index.set_range( start, end );
	non_invert_char_index.invert();
	merge( non_invert_char_index );
}

void char_index::invert()
{
	for( size_t i=0; i<sizeof( char_index_array ); ++i )
	{
		if( index[i] == 0 )
			index[i] = 1;
		else
			index[i] = 0;
	}
}

void char_index::merge( const char_index & r_rhs )
{
	for( size_t i=0; i<sizeof( char_index_array ); ++i )
		index[i] |= r_rhs.index[i];
}

bool char_index::is_set( char c ) const
{
	return index[ char_to_size_t( c ) ] != 0;
}

alphabet_char_class::alphabet_char_class( const char * p_class_spec )
{
}

} // End of namespace cl
