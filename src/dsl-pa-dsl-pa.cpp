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

#include "dsl-pa/dsl-pa-dsl-pa.h"

namespace cl {

using namespace alphabet_helpers;

size_t dsl_pa::get( std::string * p_output, const alphabet & r_alphabet )
{
	return get( p_output, r_alphabet, unbounded );
}

size_t dsl_pa::get( std::string * p_output, const alphabet & r_alphabet, size_t max_chars )
{
	size_t n_chars;
	
	for( n_chars = 0; n_chars < max_chars; ++n_chars )
	{
		if( ! r_alphabet.is_sought( get() ) )
			break;
			
		p_output->push_back( current() );
	}
	
	if( n_chars < max_chars )
		unget();
	
	return n_chars;
}

size_t dsl_pa::get_until( std::string * p_output, const alphabet & r_alphabet )
{
	return get_until( p_output, r_alphabet, '\0', unbounded );
}

size_t dsl_pa::get_bounded_until( std::string * p_output, const alphabet & r_alphabet, size_t max_chars )
{
	return get_until( p_output, r_alphabet, '\0', max_chars );
}

size_t dsl_pa::get_escaped_until( std::string * p_output, const alphabet & r_alphabet, char escape_char )
{
	return get_until( p_output, r_alphabet, escape_char, unbounded );
}

size_t dsl_pa::get_until( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars )
{
	size_t n_chars;
	bool is_escaped = false;
	
	for( n_chars = 0; n_chars < max_chars; ++n_chars )
	{
		if( get() == reader::R_EOI )
			return n_chars;
			
		if( ! is_escaped )
		{
			if( r_alphabet.is_sought( current() ) )	// For get_until(), 'sought' chars are unwanted!
				break;

			if( current() == escape_char )
				is_escaped = true;	// Escape chars are not collected in output
			else
				p_output->push_back( current() );
		}
		else
		{
			p_output->push_back( current() );
			is_escaped = false;
		}
	}
	
	if( n_chars < max_chars )
		unget();
	
	return n_chars;
}

bool dsl_pa::ws()
{
	char c = get();
	if( ! is_space( c ) )
	{
		unget();
		return false;
	}
	
	while( is_space( get() ) )
	{}
	
	unget();
	
	return true;
}

bool dsl_pa::opt_ws()
{
	return optional( ws() );
}

} // End of namespace cl
