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
// - Neither the name Codalogic nor the names of its contributors may be used
//   to endorse or promote products derived from this software without
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

class reader_factory_mem_buf : public reader_factory
{
public:
    virtual reader * create( const char * p_data ) const
    {
        return new reader_mem_buf( p_data, strlen( p_data ) );
    }
    virtual const char * form_name() const
    {
        return "Mem buf";
    }
};

class reader_factory_file : public reader_factory
{
public:
    virtual reader * create( const char * p_data ) const
    {
        const char * p_test_data_file_name = "FoxdfEr43De.txt";

        {
        std::ofstream fout( p_test_data_file_name, std::ios::binary );

        TTEST( fout.is_open() );

        if( ! fout.is_open() )
            return 0;

        fout << p_data;
        }

        return new reader_file( p_test_data_file_name );
    }
    virtual const char * form_name() const
    {
        return "File";
    }
};

void reader_basic_test( reader_factory & r_reader_factory )
{
    TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader basic tests").c_str() );

    TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abc d" ) ) );
    TCRITICALTEST( p_reader.get() != 0 );

    TTEST( p_reader->get() == 'a' );
    TTEST( p_reader->current() == 'a' );
    TTEST( p_reader->current() == 'a' );    // Can read current() multiple times
    TTEST( p_reader->get() == 'b' );
    TTEST( p_reader->current() == 'b' );

    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->get() == ' ' );
    TTEST( p_reader->get() == 'd' );

    TTEST( p_reader->get() == reader::R_EOI );
    TTEST( p_reader->current() == reader::R_EOI );

    TSETUP( std::auto_ptr< reader > p_reader_empty( r_reader_factory.create( "" ) ) );
    TCRITICALTEST( p_reader_empty.get() != 0 );

    TTEST( p_reader_empty->get() == reader::R_EOI );
    TTEST( p_reader_empty->current() == reader::R_EOI );
}

void reader_location_test( reader_factory & r_reader_factory )
{
    TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader location tests").c_str() );

    TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abcdef" ) ) );
    TCRITICALTEST( p_reader.get() != 0 );

    TTEST( p_reader->get() == 'a' );
    TTEST( p_reader->current() == 'a' );
    TTEST( p_reader->current() == 'a' );    // Can read current() multiple times
    TTEST( p_reader->get() == 'b' );
    TTEST( p_reader->current() == 'b' );

    TTEST( p_reader->location_push() ); // Not really a test, but we want to ensure it returns true

    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->get() == 'd' );

    TTEST( p_reader->location_push() ); // Not really a test, but we want to ensure it returns true

    TTEST( p_reader->get() == 'e' );
    TTEST( p_reader->get() == 'f' );
    TTEST( p_reader->get() == reader::R_EOI );
    TTEST( p_reader->get() == reader::R_EOI );  // Should be able to repeatedly call get() once read end of input

    TTEST( ! p_reader->location_top() );  // Not really a test, but we want to ensure it returns false
    TTEST( p_reader->get() == 'e' );
    TTEST( p_reader->get() == 'f' );
    TTEST( ! p_reader->location_top() );  // Not really a test, but we want to ensure it returns false
    TTEST( p_reader->get() == 'e' );
    TTEST( p_reader->get() == 'f' );

    TTEST( p_reader->location_pop() );  // Not really a test, but we want to ensure it returns true
    TTEST( ! p_reader->location_top() );  // Not really a test, but we want to ensure it returns false
    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->get() == 'd' );
}

void reader_location_with_newline_test( reader_factory & r_reader_factory )
{
    // These are primarily for the file input mode, but might as well test with string also

    TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader location with newline tests").c_str() );

    {
    TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abc\ndef" ) ) );
    TCRITICALTEST( p_reader.get() != 0 );

    TTEST( p_reader->get() == 'a' );
    TTEST( p_reader->get() == 'b' );

    TTEST( p_reader->location_push() ); // locatioin_push() before newline

    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->get() == '\n' );
    TTEST( p_reader->get() == 'd' );

    TTEST( p_reader->location_push() ); // locatioin_push() after newline

    TTEST( p_reader->get() == 'e' );
    TTEST( p_reader->get() == 'f' );

    TTEST( ! p_reader->location_top() );
    TTEST( p_reader->get() == 'e' );

    TTEST( p_reader->location_pop() );

    TTEST( ! p_reader->location_top() );
    TTEST( p_reader->get() == 'c' );
    }

    {
    TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abc\r\ndef" ) ) );
    TCRITICALTEST( p_reader.get() != 0 );

    TTEST( p_reader->get() == 'a' );
    TTEST( p_reader->get() == 'b' );

    TTEST( p_reader->location_push() ); // locatioin_push() before newline

    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->get() == '\r' );
    TTEST( p_reader->get() == '\n' );
    TTEST( p_reader->get() == 'd' );

    TTEST( p_reader->location_push() ); // locatioin_push() after newline

    TTEST( p_reader->get() == 'e' );
    TTEST( p_reader->get() == 'f' );

    TTEST( ! p_reader->location_top() );
    TTEST( p_reader->get() == 'e' );

    TTEST( p_reader->location_pop() );

    TTEST( ! p_reader->location_top() );
    TTEST( p_reader->get() == 'c' );
    }
}

void reader_unget_test( reader_factory & r_reader_factory )
{
    TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader unget/peek tests").c_str() );

    TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abcdef" ) ) );
    TCRITICALTEST( p_reader.get() != 0 );

    TTEST( p_reader->get() == 'a' );
    TTEST( p_reader->get() == 'b' );
    TSETUP( p_reader->unget() );
    TTEST( p_reader->current() == 'b' );    // current() char not affected by unget() (i.e. doesn't make current() return 'a' here)
    TTEST( p_reader->get() == 'b' );
    TSETUP( p_reader->unget() );
    TTEST( p_reader->current() == 'b' );    // current() char not affected by unget()
    TTEST( p_reader->get() == 'b' );
    TSETUP( p_reader->unget( 'z' ) );
    TSETUP( p_reader->unget( 'y' ) );
    TTEST( p_reader->current() == 'b' );    // current() char not affected by unget()
    TTEST( p_reader->get() == 'y' );        // ungetted chars returned in reverse order to what they were unget()ted
    TTEST( p_reader->get() == 'z' );

    TDOC( "Test peek()" );
    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->peek() == 'd' );
    TTEST( p_reader->peek() == 'd' );
    TTEST( p_reader->get() == 'd' );
    TTEST( p_reader->peek() == 'e' );
    TTEST( p_reader->get() == 'e' );
}

void reader_location_and_unget_test( reader_factory & r_reader_factory )
{
    TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader location tests").c_str() );

    {
    TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abcdef" ) ) );
    TCRITICALTEST( p_reader.get() != 0 );

    TTEST( p_reader->get() == 'a' );
    TTEST( p_reader->get() == 'b' );
    TTEST( p_reader->get() == 'c' );

    // See note Location_changes in dsl-reader.h
    // This combination of unget() and location_push() interact with each other
    // and requires necessitates care in the implementation. See
    // unget_buffer_size parameter for reader::source_location_push()
    TSETUP( p_reader->unget() );
    TTEST( p_reader->location_push() );

    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->get() == 'd' );
    TTEST( p_reader->get() == 'e' );
    TTEST( ! p_reader->location_top() );
    TTEST( p_reader->get() == 'c' );
    }

    {
    TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abc" ) ) );
    TCRITICALTEST( p_reader.get() != 0 );

    TTEST( p_reader->get() == 'a' );
    TTEST( p_reader->get() == 'b' );
    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->get() == '\0' );

    // See note Location_changes in dsl-reader.h
    // This combination of unget() and location_push() interact with each other
    // and requires necessitates care in the implementation. See
    // unget_buffer_size parameter for reader::source_location_push()
    TSETUP( p_reader->unget() );
    TTEST( p_reader->location_push() );

    TTEST( p_reader->get() == '\0' );
    TTEST( ! p_reader->location_top() );
    TTEST( p_reader->get() == '\0' );
    }
}

void reader_location_logger_test( reader_factory & r_reader_factory )
{
    TBEGIN( (std::string( r_reader_factory.form_name() ) + " reader location logger tests").c_str() );

    TSETUP( std::auto_ptr< reader > p_reader( r_reader_factory.create( "abcdef" ) ) );
    TCRITICALTEST( p_reader.get() != 0 );

    TTEST( p_reader->get() == 'a' );
    TTEST( p_reader->current() == 'a' );
    TTEST( p_reader->current() == 'a' );    // Can read current() multiple times
    TTEST( p_reader->get() == 'b' );
    TTEST( p_reader->current() == 'b' );

    TTEST( p_reader->location_push() ); // Not really a test, but we want to ensure it returns true

    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->get() == 'd' );

    {   // Control scope of my_location_logger
    TSETUP( location_logger my_location_logger( *p_reader ) );

    TTEST( p_reader->get() == 'e' );
    TTEST( p_reader->get() == 'f' );
    TTEST( p_reader->get() == reader::R_EOI );
    TTEST( p_reader->get() == reader::R_EOI );  // Should be able to repeatedly call get() once read end of input

    TTEST( ! p_reader->location_top() );  // Not really a test, but we want to ensure it returns false
    TTEST( p_reader->get() == 'e' );
    TTEST( p_reader->get() == 'f' );
    TTEST( ! p_reader->location_top() );  // Not really a test, but we want to ensure it returns false
    TTEST( p_reader->get() == 'e' );
    }   // End of my_location_logger scope

    TTEST( p_reader->get() == 'f' );
    TTEST( ! p_reader->location_top() );  // Not really a test, but we want to ensure it returns false
    TTEST( p_reader->get() == 'c' );
    TTEST( p_reader->get() == 'd' );
}

void all_reader_tests( reader_factory & r_reader_factory )
{
    reader_basic_test( r_reader_factory );
    reader_location_test( r_reader_factory );
    reader_location_with_newline_test( r_reader_factory );
    reader_unget_test( r_reader_factory );
    reader_location_and_unget_test( r_reader_factory );
    reader_location_logger_test( r_reader_factory );
}

TFUNCTION( string_reader_test )
{
    TBEGIN( "String reader tests" );

    all_reader_tests( reader_factory_string() );
}

TFUNCTION( file_reader_test )
{
    TBEGIN( "File reader tests" );

    all_reader_tests( reader_factory_file() );
}

TFUNCTION( mem_buf_reader_test )
{
    TBEGIN( "Mem buf reader tests" );

    all_reader_tests( reader_factory_mem_buf() );
}

TFUNCTION( is_get_char_reader_test )
{
    TBEGIN( "is get char reader tests" );

    reader_string my_reader( "abcd" );

    TTEST( ! my_reader.is_get_char( 'x' ) );
    TTEST( my_reader.is_get_char( 'a' ) );
    TTEST( ! my_reader.is_get_char( 'x' ) );
    TTEST( my_reader.is_get_char( 'b' ) );
}

TFUNCTION( reader_line_counting_test )
{
    TBEGIN( "Reader line counting tests" );

    reader_string my_reader( "ab\r\ncd\nef\rgh\r\nij" );

    TTEST( my_reader.get_line_number() == 1 );
    TTEST( my_reader.get() == 'a' );
    TTEST( my_reader.get() == 'b' );
    TTEST( my_reader.get_line_number() == 1 );
    TTEST( my_reader.get() == '\r' );
    TTEST( my_reader.get() == '\n' );
    TTEST( my_reader.get_line_number() == 2 );
    TTEST( my_reader.get() == 'c' );
    TTEST( my_reader.get_line_number() == 2 );
    TSETUP( my_reader.location_push() );
    TTEST( my_reader.get() == 'd' );
    TTEST( my_reader.get_line_number() == 2 );
    TTEST( my_reader.get() == '\n' );
    TTEST( my_reader.get_line_number() == 3 );
    TTEST( my_reader.get() == 'e' );
    TTEST( my_reader.get_line_number() == 3 );
    TTEST( my_reader.get() == 'f' );
    TTEST( my_reader.get_line_number() == 3 );
    TTEST( my_reader.get() == '\r' );
    TTEST( my_reader.get_line_number() == 4 );
    TSETUP( my_reader.unget() );
    TTEST( my_reader.get() == '\r' );
    TTEST( my_reader.get_line_number() == 4 );
    TTEST( my_reader.get() == 'g' );
    TTEST( my_reader.get_line_number() == 4 );
    TTEST( my_reader.get() == 'h' );
    TTEST( my_reader.get_line_number() == 4 );
    TTEST( my_reader.get() == '\r' );
    TTEST( my_reader.get_line_number() == 5 );
    TTEST( my_reader.get() == '\n' );
    TTEST( my_reader.get_line_number() == 5 );
    TTEST( my_reader.get() == 'i' );
    TTEST( my_reader.get_line_number() == 5 );
    TTEST( my_reader.get() == 'j' );

    TSETUP( my_reader.location_top() );
    TTEST( my_reader.get() == 'd' );
    TTEST( my_reader.get_line_number() == 2 );
    TTEST( my_reader.get() == '\n' );
    TTEST( my_reader.get_line_number() == 3 );

    TSETUP( my_reader.location_top() );
    TTEST( my_reader.get() == 'd' );
    TTEST( my_reader.get_line_number() == 2 );
    TTEST( my_reader.get() == '\n' );
    TTEST( my_reader.get_line_number() == 3 );

    TSETUP( my_reader.location_pop() );
}

TFUNCTION( reader_is_open_test )
{
    TBEGIN( "Reader::is_open() tests" );

    {
    reader_string my_reader( "ab\r\ncd\nef\rgh\r\nij" );

    TTEST( my_reader.is_open() );
    }

    {
    reader_file my_reader( "l:\\::::" );    // Illegal file name

    TTEST( ! my_reader.is_open() );
    }

    {
    std::auto_ptr< reader > p_reader( reader_factory_file().create( "abc d" ) );

    TTEST( p_reader->is_open() );
    }
}
