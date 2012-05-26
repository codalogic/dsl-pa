//----------------------------------------------------------------------
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
// - Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright 
//   notice, this list of conditions and the following disclaimer in 
//   the documentation and/or other materials provided with the 
//   distribution.
// - Neither the name Codalogic Ltd nor the names of its contributors 
//   may be used to endorse or promote products derived from this 
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------

#include "clunit.h"

#include "dsl-pa/dsl-pa.h"

using namespace cl;

static bool is_false_optional_test_helper()
{
	return false;
}

static bool is_true_optional_test_helper()
{
	return true;
}

static size_t size_0_optional_test_helper()
{
	return 0;
}

static size_t size_12_optional_test_helper()
{
	return 12;
}

static size_t int_minus_2_optional_test_helper()
{
	return -2;
}

TFUNCTION( dsl_pa_optional_test )
{
	TBEGIN( "dsl_pa::optional() operation" );
	
	TTEST( dsl_pa::optional( is_false_optional_test_helper() ) );
	TTEST( dsl_pa::optional( is_true_optional_test_helper() ) );
	TTEST( dsl_pa::optional( size_0_optional_test_helper() ) );
	TTEST( dsl_pa::optional( size_12_optional_test_helper() ) );
	TTEST( dsl_pa::optional( int_minus_2_optional_test_helper() ) );
}

TFUNCTION( dsl_pa_set_test )
{
	TBEGIN( "dsl_pa::set() operation" );
	
	TSETUP( int i = 0 );
	TSETUP( dsl_pa::set( i, 1 ) );
	TTEST( i == 1 );
	
	enum { HOT, COLD } e = HOT;
	TSETUP( dsl_pa::set( e, COLD ) );
	TTEST( e == COLD );
}

TFUNCTION( dsl_pa_clear_test )
{
	TBEGIN( "dsl_pa::clear() operation" );
	
	std::string v( "abc" );
	TTEST( ! v.empty() );
	TTEST( dsl_pa::clear( v ) );	// Test to ensure it returns true
	TTEST( v.empty() );
}

TFUNCTION( dsl_pa_error_test )
{
	TBEGIN( "dsl_pa::error() operation" );

	{
	bool is_thrown = false;
	try
	{
		dsl_pa::error( "Expected other input" );
	}
	catch( const dsl_pa_recoverable_exception & )
	{
		is_thrown = true;
	}
	TTEST( is_thrown );
	}

	{
	bool is_thrown = false;
	try
	{
		dsl_pa::error_fatal( "Expected other input" );
	}
	catch( const dsl_pa_fatal_exception & )
	{
		is_thrown = true;
	}
	TTEST( is_thrown );
	}

	{
	class my_exception
	{
	private:
		int error_code;
	public:
		my_exception( int error_code_in ) : error_code( error_code_in ) {}
		int get_code() const { return error_code; }
	};
	
	bool is_thrown = false;
	try
	{
		dsl_pa::error( my_exception( 12 ) );
	}
	catch( const my_exception & r_exception )
	{
		is_thrown = true;
		TTEST( r_exception.get_code() == 12 );
	}
	TTEST( is_thrown );
	}
}

TFUNCTION( dsl_pa_test )
{
	TBEGIN( "dsl pa class tests" );
}
