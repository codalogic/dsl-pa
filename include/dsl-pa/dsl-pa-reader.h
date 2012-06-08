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

//----------------------------------------------------------------------------
// dsl-pa is a Domain Specific Language Parsing Assistant library designed to
// take advantage of the C++ logic shortcut operators such as && and ||.
// See the brief overview in dsl-pa.h or, for more information, README.html
// at https://github.com/codalogic/dsl-pa
//----------------------------------------------------------------------------

#ifndef CL_DSL_PA_READER
#define CL_DSL_PA_READER

#include <string>
#include <vector>
#include <stack>
#include <fstream>

namespace cl {

typedef std::stack< char > unget_buffer_t;

class line_counter
{
    // This is a simple class for keeping track of the line number (and in
    // future, column number) from which parsed input is read.  The intention
    // is that a better implementation can be installed in future without
    // changing the way the reader class uses it.
    // The policy of this implementation is to ignore column numbers and also
    // ignore complexities associated with ungetting and then re-getting
    // ungotten chars.  Hence, there may be some situations where the
    // returned line number is slightly wrong!
public:
    static const int UNKNOWN = -1;

private:
    int line_number;
    char last_nl_char;

public:
    line_counter()
        :
        line_number( 1 ),
        last_nl_char( '\0' )
    {}
    
    // line_counter & operator = ( const line_counter & rhs ) = default;

    void got_char( char c );
    void ungot_char( char c )
    {
        // For now, ignore ungot chars
    }
    void retrieved_ungot_char( char c )
    {
        // For now, ignore retrieved ungot chars
    }

    int get_line_number() const
    {
        return line_number;
    }
    int get_column_number() const
    {
        // Tracking column numbers not implemented by this class
        return UNKNOWN;
    }
};

class reader_location_stack
{
private:
    struct location_stack_item
    {
        unget_buffer_t unget_buffer;
        line_counter line_info;
    };
    std::stack< location_stack_item > location_stack;

public:
    void push( const unget_buffer_t & r_unget_buffer, const line_counter & r_line_info )
    {
        // Push empty object to avoid multiple copies of unget_buffer
        location_stack.push( location_stack_item() );
        location_stack.top().unget_buffer = r_unget_buffer;
        location_stack.top().line_info = r_line_info;
    }
    void top( unget_buffer_t * p_unget_buffer, line_counter * p_line_info ) const
    {
        *p_unget_buffer = location_stack.top().unget_buffer;
        *p_line_info = location_stack.top().line_info;
    }
    void pop()
    {
        location_stack.pop();
    }
};

class reader
{
private:
    line_counter line_info;
    unget_buffer_t unget_buffer;
    char current_char;
    reader_location_stack location_stack;

    virtual char get_next_input() = 0;

    virtual void source_location_push() = 0;
    virtual void source_location_top() = 0;
    virtual void source_location_pop() = 0;

public:
    enum { R_EOI = 0 }; // Constant for "Reader End Of Input"

    reader() : current_char( R_EOI ) {}
    virtual ~reader() {}

    char get();
    char current() const { return current_char; }
    void unget() { unget( current() ); }    // Unget with argument ungets current char
    void unget( char c )
    {
        if( c != R_EOI )
        {
            line_info.ungot_char( c );
            unget_buffer.push( c );
        }
    }
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
        location_stack.push( unget_buffer, line_info );
        source_location_push();
        return true;
    }

    bool location_top()
    {
        source_location_top();
        location_stack.top( &unget_buffer, &line_info );
        return true;
    }

    bool location_pop()
    {
        source_location_pop();
        location_stack.pop();
        return true;
    }

    int get_line_number() const { return line_info.get_line_number(); }
    int get_column_number() const { return line_info.get_column_number(); }
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
    reader_mem_buf( const std::vector< char > & r_in )  // A vector seems a good place to collect data from a socket etc.
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
