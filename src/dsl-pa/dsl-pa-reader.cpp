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
// See dsl-pa.h and README.html at https://github.com/codalogic/dsl-pa for
// more information.
//----------------------------------------------------------------------------

#include "dsl-pa/dsl-pa-reader.h"

namespace cl {

void line_counter_with_stack::got_char( char c )
{
    int line_number = get_line_number();
    int column_number = get_column_number();

    if( c == '\r' || c == '\n' )
    {
        column_number = 0;
        if( current.last_nl_char == '\0' || current.last_nl_char == c )
            ++line_number;

        current.last_nl_char = c;
    }
    else
    {
        ++column_number;

        current.last_nl_char = '\0';
    }

    set_position( line_number, column_number );
}

char reader::get()
{
    if( ! unget_buffer.empty() )
    {
        current_char = unget_buffer.reget();
        line_counter.retrieved_ungot_char( current_char );
    }
    else
    {
        current_char = get_next_input();
        line_counter.got_char( current_char );
    }
    return current_char;
}

} // End of namespace cl
