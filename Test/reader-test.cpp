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

TFUNCTION( string_reader_basic_test )
{
	TBEGIN( "String reader basic tests" );
	
	TSETUP( reader_string my_reader( "abcd" ) );
	
	TTEST( my_reader.get() == 'a' );
	TTEST( my_reader.current() == 'a' );
	TTEST( my_reader.current() == 'a' );	// Can read current() multiple times
	TTEST( my_reader.get() == 'b' );
	TTEST( my_reader.current() == 'b' );

	TTEST( my_reader.get() == 'c' );
	TTEST( my_reader.get() == 'd' );
	
	TTEST( my_reader.get() == reader::R_EOI );
	TTEST( my_reader.current() == reader::R_EOI );
	
	TSETUP( reader_string my_reader_empty( "" ) );
	
	TTEST( my_reader_empty.get() == reader::R_EOI );
	TTEST( my_reader_empty.current() == reader::R_EOI );
}

TFUNCTION( string_reader_location_test )
{
	TBEGIN( "String reader location tests" );
	
	TSETUP( reader_string my_reader( "abcdef" ) );
	
	TTEST( my_reader.get() == 'a' );
	TTEST( my_reader.current() == 'a' );
	TTEST( my_reader.current() == 'a' );	// Can read current() multiple times
	TTEST( my_reader.get() == 'b' );
	TTEST( my_reader.current() == 'b' );

	TTEST( my_reader.location_push() );	// Not really a test, but we want to ensure it returns true
	
	TTEST( my_reader.get() == 'c' );
	TTEST( my_reader.get() == 'd' );

	TTEST( my_reader.location_push() );	// Not really a test, but we want to ensure it returns true

	TTEST( my_reader.get() == 'e' );
	TTEST( my_reader.get() == 'f' );
	TTEST( my_reader.get() == reader::R_EOI );
	TTEST( my_reader.get() == reader::R_EOI );	// Should be able to repeatedly call get() once read end of input
	
	TTEST( my_reader.location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( my_reader.get() == 'e' );
	TTEST( my_reader.get() == 'f' );
	TTEST( my_reader.location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( my_reader.get() == 'e' );
	TTEST( my_reader.get() == 'f' );
	
	TTEST( my_reader.location_pop() );	// Not really a test, but we want to ensure it returns true
	TTEST( my_reader.location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( my_reader.get() == 'c' );
	TTEST( my_reader.get() == 'd' );
}

TFUNCTION( string_reader_unget_test )
{
	TBEGIN( "String reader unget/peek tests" );
	
	TSETUP( reader_string my_reader( "abcdef" ) );
	
	TTEST( my_reader.get() == 'a' );
	TTEST( my_reader.get() == 'b' );
	TSETUP( my_reader.unget() );
	TTEST( my_reader.current() == 'b' );	// current() char not affected by unget() (i.e. doesn't make current() return 'a' here)
	TTEST( my_reader.get() == 'b' );
	TSETUP( my_reader.unget() );
	TTEST( my_reader.current() == 'b' );	// current() char not affected by unget()
	TTEST( my_reader.get() == 'b' );
	TSETUP( my_reader.unget( 'z' ) );
	TSETUP( my_reader.unget( 'y' ) );
	TTEST( my_reader.current() == 'b' );	// current() char not affected by unget()
	TTEST( my_reader.get() == 'y' );		// ungetted chars returned in reverse order to what they were unget()ted
	TTEST( my_reader.get() == 'z' );
	
	TDOC( "Test peek()" );
	TTEST( my_reader.get() == 'c' );
	TTEST( my_reader.peek() == 'd' );
	TTEST( my_reader.peek() == 'd' );
	TTEST( my_reader.get() == 'd' );
	TTEST( my_reader.peek() == 'e' );
	TTEST( my_reader.get() == 'e' );
}

TFUNCTION( string_reader_location_logger_test )
{
	TBEGIN( "String reader location logger tests" );
	
	
	TSETUP( reader_string my_reader( "abcdef" ) );
	
	TTEST( my_reader.get() == 'a' );
	TTEST( my_reader.current() == 'a' );
	TTEST( my_reader.current() == 'a' );	// Can read current() multiple times
	TTEST( my_reader.get() == 'b' );
	TTEST( my_reader.current() == 'b' );

	TTEST( my_reader.location_push() );	// Not really a test, but we want to ensure it returns true
	
	TTEST( my_reader.get() == 'c' );
	TTEST( my_reader.get() == 'd' );
	
	{	// Control scope of my_location_logger
	TSETUP( location_logger my_location_logger( my_reader ) );

	TTEST( my_reader.get() == 'e' );
	TTEST( my_reader.get() == 'f' );
	TTEST( my_reader.get() == reader::R_EOI );
	TTEST( my_reader.get() == reader::R_EOI );	// Should be able to repeatedly call get() once read end of input
	
	TTEST( my_reader.location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( my_reader.get() == 'e' );
	TTEST( my_reader.get() == 'f' );
	TTEST( my_reader.location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( my_reader.get() == 'e' );
	}	// End of my_location_logger scope
	
	TTEST( my_reader.get() == 'f' );
	TTEST( my_reader.location_top() );	// Not really a test, but we want to ensure it returns true
	TTEST( my_reader.get() == 'c' );
	TTEST( my_reader.get() == 'd' );
}

TFUNCTION( file_reader_test )
{
	TBEGIN( "File reader tests" );
	
	TTODO( "File reader tests" );
}
