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
// See the brief overview in dsl-pa.h or, for more information, README.html
// at https://github.com/codalogic/dsl-pa
//----------------------------------------------------------------------------

#ifndef CL_DSL_PA_LINE_COUNTER
#define CL_DSL_PA_LINE_COUNTER

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
    int column_number;
    char last_nl_char;

public:
    line_counter()
        :
        line_number( 1 ),
        column_number( UNKNOWN ),
        last_nl_char( '\0' )
    {}
    
    void got_char( char c )
    {
        if( c == '\r' || c == '\n' )
        {
            if( last_nl_char == '\0' || last_nl_char == c )
            {
                ++line_number;
                last_nl_char = c;
                return;
            }
        }
        last_nl_char = '\0';
    }
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

#endif // CL_DSL_PA_LINE_COUNTER
