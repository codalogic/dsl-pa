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

class reader_factory
{
public:
	virtual reader * create( const char * p_data ) const = 0;
	virtual const char * form_name() const = 0;
};

class reader_factory_string : public reader_factory
{
public:
	virtual reader * create( const char * p_data ) const
	{
		return new reader_string( p_data );
	}
	virtual const char * form_name() const
	{
		return "String";
	}
};

class reader_factory_file : public reader_factory
{
public:
	virtual reader * create( const char * p_data ) const
	{
		TTODO( "create reader_factory_file::create();" );
	}
	virtual const char * form_name() const
	{
		return "File";
	}
};

void reader_basic_test( reader_factory & r_reader_factory )
{
	TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader basic tests").c_str() );
	
	TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abcd" ) ) );
	
	TTEST( p_reader->get() == 'a' );
	TTEST( p_reader->current() == 'a' );
	TTEST( p_reader->current() == 'a' );	// Can read current() multiple times
	TTEST( p_reader->get() == 'b' );
	TTEST( p_reader->current() == 'b' );

	TTEST( p_reader->get() == 'c' );
	TTEST( p_reader->get() == 'd' );
	
	TTEST( p_reader->get() == reader::R_EOI );
	TTEST( p_reader->current() == reader::R_EOI );
	
	TSETUP( std::auto_ptr< reader > p_reader_empty( r_reader_factory.create( "" ) ) );
	
	TTEST( p_reader_empty->get() == reader::R_EOI );
	TTEST( p_reader_empty->current() == reader::R_EOI );
}

void reader_location_test( reader_factory & r_reader_factory )
{
	TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader location tests").c_str() );
	
	TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abcdef" ) ) );
	
	TTEST( p_reader->get() == 'a' );
	TTEST( p_reader->current() == 'a' );
	TTEST( p_reader->current() == 'a' );	// Can read current() multiple times
	TTEST( p_reader->get() == 'b' );
	TTEST( p_reader->current() == 'b' );

	TTEST( p_reader->location_push() );	// Not really a test, but we want to ensure it returns true
	
	TTEST( p_reader->get() == 'c' );
	TTEST( p_reader->get() == 'd' );

	TTEST( p_reader->location_push() );	// Not really a test, but we want to ensure it returns true

	TTEST( p_reader->get() == 'e' );
	TTEST( p_reader->get() == 'f' );
	TTEST( p_reader->get() == reader::R_EOI );
	TTEST( p_reader->get() == reader::R_EOI );	// Should be able to repeatedly call get() once read end of input
	
	TTEST( p_reader->location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( p_reader->get() == 'e' );
	TTEST( p_reader->get() == 'f' );
	TTEST( p_reader->location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( p_reader->get() == 'e' );
	TTEST( p_reader->get() == 'f' );
	
	TTEST( p_reader->location_pop() );	// Not really a test, but we want to ensure it returns true
	TTEST( p_reader->location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( p_reader->get() == 'c' );
	TTEST( p_reader->get() == 'd' );
}

void reader_unget_test( reader_factory & r_reader_factory )
{
	TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader unget/peek tests").c_str() );
	
	TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abcdef" ) ) );
	
	TTEST( p_reader->get() == 'a' );
	TTEST( p_reader->get() == 'b' );
	TSETUP( p_reader->unget() );
	TTEST( p_reader->current() == 'b' );	// current() char not affected by unget() (i.e. doesn't make current() return 'a' here)
	TTEST( p_reader->get() == 'b' );
	TSETUP( p_reader->unget() );
	TTEST( p_reader->current() == 'b' );	// current() char not affected by unget()
	TTEST( p_reader->get() == 'b' );
	TSETUP( p_reader->unget( 'z' ) );
	TSETUP( p_reader->unget( 'y' ) );
	TTEST( p_reader->current() == 'b' );	// current() char not affected by unget()
	TTEST( p_reader->get() == 'y' );		// ungetted chars returned in reverse order to what they were unget()ted
	TTEST( p_reader->get() == 'z' );
	
	TDOC( "Test peek()" );
	TTEST( p_reader->get() == 'c' );
	TTEST( p_reader->peek() == 'd' );
	TTEST( p_reader->peek() == 'd' );
	TTEST( p_reader->get() == 'd' );
	TTEST( p_reader->peek() == 'e' );
	TTEST( p_reader->get() == 'e' );
}

void string_reader_location_logger_test( reader_factory & r_reader_factory )
{
	TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader location logger tests").c_str() );

	TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abcdef" ) ) );
	
	TTEST( p_reader->get() == 'a' );
	TTEST( p_reader->current() == 'a' );
	TTEST( p_reader->current() == 'a' );	// Can read current() multiple times
	TTEST( p_reader->get() == 'b' );
	TTEST( p_reader->current() == 'b' );

	TTEST( p_reader->location_push() );	// Not really a test, but we want to ensure it returns true
	
	TTEST( p_reader->get() == 'c' );
	TTEST( p_reader->get() == 'd' );
	
	{	// Control scope of my_location_logger
	TSETUP( location_logger my_location_logger( *p_reader ) );

	TTEST( p_reader->get() == 'e' );
	TTEST( p_reader->get() == 'f' );
	TTEST( p_reader->get() == reader::R_EOI );
	TTEST( p_reader->get() == reader::R_EOI );	// Should be able to repeatedly call get() once read end of input
	
	TTEST( p_reader->location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( p_reader->get() == 'e' );
	TTEST( p_reader->get() == 'f' );
	TTEST( p_reader->location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( p_reader->get() == 'e' );
	}	// End of my_location_logger scope
	
	TTEST( p_reader->get() == 'f' );
	TTEST( p_reader->location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( p_reader->get() == 'c' );
	TTEST( p_reader->get() == 'd' );
}

void all_reader_tests( reader_factory & r_reader_factory )
{
	reader_basic_test( r_reader_factory );
}

TFUNCTION( string_reader_test )
{
	TBEGIN( "String reader tests" );
	
}

TFUNCTION( file_reader_test )
{
	TTODO( "File reader tests" );
}
