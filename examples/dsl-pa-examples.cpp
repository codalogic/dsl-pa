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

#include "dsl-pa/dsl-pa.h"

#include <iostream>
#include <fstream>

using namespace cl;

void example1( std::ostream & fout )
{
    fout << "Example 1\n==============\n";

    // What we want to parse
    const char * const to_parse = " width=10, height = 5";

	// Create a reader with the desired input
    reader_string my_reader( to_parse );
    
    // Create a parser instance specifying the created reader
    dsl_pa pa( my_reader );

    // Create variables to capture parsed data
    size_t width, height;

    // Parse the 'to_parse' string
    if( pa.opt_ws() &&
            pa.fixed( "width" ) &&
            pa.opt_ws() && pa.is_char( '=' ) && pa.opt_ws() &&
            pa.get_uint( &width ) &&
            pa.opt_ws() && pa.is_char( ',' ) && pa.opt_ws() &&
            pa.fixed( "height" ) &&
            pa.opt_ws() && pa.is_char( '=' ) && pa.opt_ws() &&
            pa.get_uint( &height ) )
    {
        // Report the results
        fout << "Example 1 OK: w=" << width << " & h=" << height << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}

class example_parser : public dsl_pa
{
public:
    example_parser( reader & r_reader_in ) : dsl_pa( r_reader_in ) {}

    void example2( std::ostream & fout );
};

void example2( std::ostream & fout )
{
    fout << "\nExample 2\n==============\n";

    reader_string my_reader( " width=10, height = 5" );
    example_parser my_parser( my_reader );
    my_parser.example2( fout );
}

void example_parser::example2( std::ostream & fout )
{
    size_t width, height;

    if( opt_ws() &&
            fixed( "width" ) &&
            opt_ws() && is_char( '=' ) && opt_ws() &&
            get_uint( &width ) &&
            opt_ws() && is_char( ',' ) && opt_ws() &&
            fixed( "height" ) &&
            opt_ws() && is_char( '=' ) && opt_ws() &&
            get_uint( &height ) )
    {
        fout << "Example 2 OK: w=" << width << " & h=" << height << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}

int main()
{
    std::ofstream fout( "examples-out.txt" );

    example1( fout );
    example2( fout );
}
