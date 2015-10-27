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

TFEATURE( "class converter" )
{
    // The converter class allows a character to be converted into an
    // alternative (possibly empty) character sequence.  Return NULL
    // when not interested in further input.  The class deriving from
    // converter can be stateful.

    class example_mutator : public mutator
    {
    private:
        size_t n_ds;
        bool m_is_eof;

    public:
        example_mutator() : n_ds( 0 ), m_is_eof( false ) {}

        virtual const char * operator() ( char c )
        {
            if( c == 'a' )
                return "AAA";
            else if( c == 'b' )
                return "b";
            else if( c == 'c' )
                return "";
            else if( c == 'd' )
                return from_char( ++n_ds + '0' );
            else if( c == 'e' )
                return 0;
            else
                return "x";
        }
        virtual bool got_eof() { m_is_eof = true; return true; }

        bool is_eof() const { return m_is_eof; }
    };

    {
    example_mutator my_mutator;

    reader_string my_reader( "abdcdbde" );
    dsl_pa my_pa( my_reader );

    std::string out;

    TTEST( my_pa.get( &out, my_mutator ) == 7 );

    TTEST( out == "AAAb12b3" );
    TTEST( ! my_mutator.is_eof() );

    my_pa.get_char( &out );

    TTEST( out == "e" );
    }

    {
    example_mutator my_mutator;

    reader_string my_reader( "abdcdb" );
    dsl_pa my_pa( my_reader );

    std::string out;

    my_pa.get( &out, my_mutator );

    TTEST( out == "AAAb12b" );
    TTEST( my_mutator.is_eof() );
    }
}
