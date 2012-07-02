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

#ifndef CL_DSL_PA_INLINE
#define CL_DSL_PA_INLINE

#include "dsl-pa-reader.h"
#include "dsl-pa-dsl-pa.h"

namespace cl {

class dsl_pa_inline
{
private:
    reader_string my_reader;
    dsl_pa pa;
    bool is_matched;
    bool wanted_result;
    
    struct params
    {
        char c;
        const alphabet * p_alphabet;
    };
    struct no_params
    {
    };
    struct char_params
    {
        char c;
        char_params( char c_in ) : c( c_in ) {}
    };

    template< class Taction, class Tparams >
    dsl_pa_inline & invoke( const Taction &, const Tparams & params_in )
    {
        if( is_matched )
        {
            is_matched &= (action( Taction(), params_in ) == wanted_result);
            wanted_result = true;
        }
        return *this;
    }
    
public:
    dsl_pa_inline( const std::string & r_in )
        :
        my_reader( r_in ),
        pa( my_reader ),
        is_matched( true ),
        wanted_result( true )
    {}
    dsl_pa_inline( const char * const p_in )
        :
        my_reader( p_in ),
        pa( my_reader ),
        is_matched( true ),
        wanted_result( true )
    {}
    bool result() const { return is_matched; }
    operator bool () const { return result(); }
    dsl_pa_inline & not()
    {
        wanted_result = ! wanted_result;
        return *this;
    }

private:
    struct action_is_end {};
    bool action( const action_is_end &, const no_params & )
    {
        return pa.is_end();
    }
public:
    dsl_pa_inline & is_end()
    {
        return invoke( action_is_end(), no_params() );
    }

private:
    struct action_is_char {};
    bool action( const action_is_char &, const char_params & params_in )
    {
        return pa.is_char( params_in.c );
    }
public:
    dsl_pa_inline & is_char( char c )
    {
        return invoke( action_is_char(), char_params( c ) );
    }
};

} // End of namespace cl

#endif // CL_DSL_PA_INLINE