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

#include "../Test/clunit.h"

#include <iostream>
#include <fstream>

using namespace cl;

class example_parser : public dsl_pa
{
public:
    example_parser( reader & r_reader_in ) : dsl_pa( r_reader_in ) {}

    void example1( std::ostream & fout );
};

void example1( std::ostream & fout )
{
    fout << "Example 1\n==============\n";

    reader_string my_reader( " width=10, height = 5" );
    example_parser my_parser( my_reader );
    my_parser.example1( fout );
}

void example_parser::example1( std::ostream & fout )
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
        fout << "Example 1 OK: w=" << width << " & h=" << height << "\n";
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
}
