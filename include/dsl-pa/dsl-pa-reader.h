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

#ifndef CL_DSL_PA_READER
#define CL_DSL_PA_READER

#include <string>
#include <vector>
#include <stack>
#include <fstream>

namespace cl {

class reader
{
private:
	std::stack< char > unget_buffer;
	char current_char;

	virtual char get_next_input() = 0;

	virtual void source_location_push() = 0;
	virtual void source_location_top() = 0;
	virtual void source_location_pop() = 0;

public:
	enum { R_EOI = 0 };	// Constant for "Reader End Of Input"	

	reader() : current_char( R_EOI ) {}
	virtual ~reader() {}

	char get();
	char current() const { return current_char; }
	void unget() { unget( current() ); }	// Unget with argument ungets current char
	void unget( char c ) { unget_buffer.push( c ); }
	char peek() { get(); unget(); return current(); }
	bool is_char( char c )
	{	
		bool is_c = (c == get());
		if( ! is_c )
			unget();
		return is_c;
	}

	// Recording input locations and rewinding is based on stack operations.
	// i.e. you can call location_top() many times and the return location
	// won't be deleted.  When the recorded location is no longer required,
	// do location_pop().  See also class location_logger.
	bool location_push()
	{
		source_location_push();
		return true;
	}
	
	bool location_top()
	{
		source_location_top();
		return true;
	}
	
	bool location_pop()
	{
		source_location_pop();
		return true;
	}
};

class reader_string : public reader
{
private:
	const char * p_input;
	std::stack< const char * > location_buffer;

public:
	reader_string( const char * p_input_in )
		:
		p_input( p_input_in )
	{}
	reader_string( const std::string & r_input_in )
		:
		p_input( r_input_in.c_str() )
	{}

	virtual char get_next_input()
	{
		if( *p_input )
			return *p_input++;
		
		return reader::R_EOI;
	}

	virtual void source_location_push()
	{
		location_buffer.push( p_input );
	}
	virtual void source_location_top()
	{
		if( ! location_buffer.empty() )
			p_input = location_buffer.top();
	}
	virtual void source_location_pop()
	{
		location_buffer.pop();
	}
};

class reader_mem_buf : public reader
{
private:
	const char * p_begin, * p_current, * p_end;
	std::stack< const char * > location_buffer;

public:
	reader_mem_buf( const char * p_begin_in, size_t size )
		:
		p_begin( p_begin_in ),
		p_current( p_begin_in ),
		p_end( p_begin_in + size )
	{}
	reader_mem_buf( const std::vector< char > & r_in )	// A vector seems a good place to collect data from a socket etc.
		:
		p_begin( &r_in[0] ),
		p_current( &r_in[0] ),
		p_end( &r_in[0] + r_in.size() )
	{}

	virtual char get_next_input()
	{
		if( p_current != p_end )
			return *p_current++;
		
		return reader::R_EOI;
	}

	virtual void source_location_push()
	{
		location_buffer.push( p_current );
	}
	virtual void source_location_top()
	{
		if( ! location_buffer.empty() )
			p_current = location_buffer.top();
	}
	virtual void source_location_pop()
	{
		location_buffer.pop();
	}
};

class reader_file : public reader
{
private:
	std::ifstream fin;
	std::stack< std::ifstream::pos_type > location_buffer;

public:
	reader_file( const char * p_input_in )
		:
		fin( p_input_in, std::ios::binary )
	{}

	virtual char get_next_input()
	{
		int c = fin.get();
		if( c == -1 )
			return reader::R_EOI;
		return static_cast<char>( c );
	}

	virtual void source_location_push()
	{
		location_buffer.push( fin.tellg() );
	}
	virtual void source_location_top()
	{
		if( ! location_buffer.empty() )
		{
			if( fin.eof() )
				fin.clear();
			fin.seekg( location_buffer.top() );
		}
	}
	virtual void source_location_pop()
	{
		location_buffer.pop();
	}
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

} // End of namespace cl

#endif // CL_DSL_PA_READER
