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

#ifndef CL_DSL_PA_DSL_PA
#define CL_DSL_PA_DSL_PA

#include <string>

#include "dsl-pa-reader.h"
#include "dsl-pa-alphabet.h"

namespace cl {

class dsl_pa_recoverable_exception : public std::exception
{
public:
	dsl_pa_recoverable_exception( const char * const what ) : std::exception( what )
	{}
};

class dsl_pa_fatal_exception : public std::exception
{
public:
	dsl_pa_fatal_exception( const char * const what ) : std::exception( what )
	{}
};

class dsl_pa
{
private:
	const static size_t unbounded = ~0;

	reader & r_reader;

public:
	dsl_pa( reader & r_reader_in ) : r_reader( r_reader_in ) {}
	virtual ~dsl_pa() {}

	// parse() provides a hook to allow use with factories that return
	// 'abstract' parsers.  If desired, implement parse() in a derived class to
	// initiate parsing.
	virtual bool parse() { return false; }

	// optional() essentially ignore the result of the function that generated
	// the input argument. e.g. allows optional( ws() ); etc.
	static bool optional( bool ) { return true; }
	static bool optional( size_t ) { return true; }	// Overloads to avoid performance warnings due to convertin size_t to bool
	static bool optional( int ) { return true; }

	// set() allows setting of state information within a set of 
	// shortcut operators
	template< typename T >
	static bool set( T & r_variable, const T & r_value )
	{
		r_variable = r_value;
		return true;
	}
	template< typename T >
	static bool clear( T & r_value )
	{
		r_value.clear();
		return true;
	}
	template< typename T >
	static bool error( const T & r_exception )	// To throw custom exceptions
	{
		throw T( r_exception );
		return true;	// Won't be called!
	}
	static bool error( const char * p_what )
	{
		throw dsl_pa_recoverable_exception( p_what );
		return true;	// Won't be called!
	}
	static bool error_fatal( const char * p_what )
	{
		throw dsl_pa_fatal_exception( p_what );
		return true;	// Won't be called!
	}
	
	// The primary workhorse functions
	size_t get( std::string * p_output, const alphabet & r_alphabet );
	size_t get( std::string * p_output, const alphabet & r_alphabet, size_t max_chars );
	size_t get_until( std::string * p_output, const alphabet & r_alphabet );
	size_t get_bounded_until( std::string * p_output, const alphabet & r_alphabet, size_t max_chars );
	size_t get_escaped_until( std::string * p_output, const alphabet & r_alphabet, char escape_char );
	size_t get_until( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars );

	// Type specific parsing functions
	bool ws();
	bool opt_ws();
	
	size_t /*num chars read*/ int_num( std::string * p_num );
	size_t /*num chars read*/ int_num( int * p_int );
	size_t /*num chars read*/ uint_num( std::string * p_num );
	size_t /*num chars read*/ uint_num( int * p_int );
	size_t /*num chars read*/ float_num( std::string * p_num );
	size_t /*num chars read*/ float_num( float * p_float );
	size_t /*num chars read*/ float_num( double * p_float );
	size_t /*num chars read*/ sci_num( std::string * p_num );
	size_t /*num chars read*/ sci_num( float * p_float );
	size_t /*num chars read*/ sci_num( double * p_float );

	// Low-level reader access
	reader & get_reader() { return r_reader; }	// Primarily for use with location_logger class
	char get() { return r_reader.get(); }
	char current() const { return r_reader.current(); }
	bool unget() { r_reader.unget(); return true; }
	bool unget( char c ) { r_reader.unget( c ); return true; }
	char peek() { return r_reader.peek(); }
	bool is_char( char c ) { return r_reader.is_char( c ); }
	bool location_push() { return r_reader.location_push(); }	// See reader class for documentation
	bool location_top() { return r_reader.location_top(); }
	bool location_pop() { return r_reader.location_pop(); }
};

} // End of namespace cl

#endif // CL_DSL_PA_DSL_PA
