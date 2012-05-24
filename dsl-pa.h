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
// dsl-pa is a Domain Specific Language Parsing Assistant library designed to
// take advantage of the C++ logic shortcut operators such as && and ||.
// dsl-pa function calls extract sections of language input based on a
// specified alphabet.  Calls to dsl-pa functions are combined using the C++ 
// shortcut operators in order to parse a language in a reasonably compact 
// form.  Consequently all dsl-pa functions return either a boolean value or 
// a value, such as an integer, that can be implicitly converted to a bool. 
// For more information see README.html at https://github.com/codalogic/dsl-pa 
//----------------------------------------------------------------------------

#ifndef CL_DSL_PA
#define CL_DSL_PA

#include <string>
#include <vector>
#include <stack>

namespace cl {

class reader
{
private:
	std::stack< char > unget_buffer;
	char peek_char;

	virtual char get_next_input() = 0;

public:
	enum { R_EOI = 0 };	// Constant for "Reader End Of Input"	

	reader() : peek_char( R_EOI ) {}

	char get();
	char peek() const { return peek_char; }
	void unget( char c ) { unget_buffer.push( c ); }

	// Recording input locations and rewinding is based on stack operations.
	// i.e. you can call location_top() many times and the return location
	// won't be deleted.  When the recorded location is no longer required,
	// do location_pop().  See also class location_logger.
	virtual void location_push() = 0;
	virtual void location_top() = 0;
	virtual void location_pop() = 0;
};

class location_logger
{
	// Allows RAII operation of the reader locations to ensure that
	// location_pop() is not forgotten!

private:
	reader & r_reader;

public:
	location_logger( reader & r_reader_in ) : r_reader( r_reader_in )
	{
		r_reader.location_push();
	}
	~location_logger()
	{
		r_reader.location_pop();
	}
};

class reader_string
{
private:
	const std::string & r_input;
	size_t index;
	std::stack< size_t > location_buffer;

public:
	reader_string( const std::string & r_input_in )
		:
		r_input( r_input_in ),
		index( 0 )
	{}

	virtual char get_next_input()
	{
		if( index < r_input.size() )
			return r_input[index++];
		
		return reader::R_EOI;
	}

	virtual void location_push()
	{
		location_buffer.push( index );
	}

	virtual void location_top()
	{
		if( ! location_buffer.empty() )
			index = location_buffer.top();
	}

	virtual void location_pop()
	{
		location_buffer.pop();
	}
};

class alphabet
{
public:
	virtual bool is_wanted( char c ) = 0;
};

class dsl_pa
{
public:
	// set() allows setting of state information within a set of 
	// shortcut operators
	template< typename T >
	static bool set( T & r_variable, const T & r_value )
	{
		r_variable = r_value;
		return true;
	}
};

} // End of namespace cl

#endif // CL_DSL_PA
