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
    size_t is_optional;
    size_t last_size;

    struct action_is_end {};
    struct action_is_char {};
    struct action_get {};
    struct action_get_until {};
    struct action_get_bounded_until {};
    struct action_get_escaped_until {};
    struct action_fixed {};
    struct action_ifixed {};
    struct action_get_fixed {};
    struct action_get_ifixed {};
    struct action_space {};
    struct action_opt_space {};
    struct action_wsp {};
    struct action_opt_wsp {};
    struct action_get_bool {};
    struct action_get_int {};
    struct action_get_uint {};
    struct action_get_float {};
    struct action_get_sci_float {};

    struct params_none
    {
        params_none() {}
    };

    struct params_char
    {
        char c;
        params_char( char c_in ) : c( c_in ) {}
    };

    struct params_string_alphabet
    {
        std::string * p_output;
        const alphabet & r_alphabet;
        params_string_alphabet( std::string * p_output_in, const alphabet & r_alphabet_in )
            :
            p_output( p_output_in ), r_alphabet( r_alphabet_in ) {}
    };

    struct params_string_alphabet_size_t
    {
        std::string * p_output;
        const alphabet & r_alphabet;
        size_t max_chars;
        params_string_alphabet_size_t( std::string * p_output_in, const alphabet & r_alphabet_in, size_t max_chars_in )
            :
            p_output( p_output_in ), r_alphabet( r_alphabet_in ), max_chars( max_chars_in ) {}
    };

    struct params_string_alphabet_char
    {
        std::string * p_output;
        const alphabet & r_alphabet;
        char escape_char;
        params_string_alphabet_char( std::string * p_output_in, const alphabet & r_alphabet_in, char escape_char_in )
            :
            p_output( p_output_in ), r_alphabet( r_alphabet_in ), escape_char( escape_char_in ) {}
    };

    struct params_string_alphabet_char_size_t
    {
        std::string * p_output;
        const alphabet & r_alphabet;
        char escape_char;
        size_t max_chars;
        params_string_alphabet_char_size_t(
                std::string * p_output_in,
                const alphabet & r_alphabet_in,
                char escape_char_in,
                size_t max_chars_in )
            :
            p_output( p_output_in ),
            r_alphabet( r_alphabet_in ),
            escape_char( escape_char_in ),
            max_chars( max_chars_in )
        {}
    };

    struct params_char_p
    {
        const char * p_seeking;
        params_char_p( const char * p_seeking_in ) : p_seeking( p_seeking_in ) {}
    };

    struct params_string_char_p
    {
        std::string * p_output;
        const char * p_seeking;
        params_string_char_p( std::string * p_output_in, const char * p_seeking_in )
            :
            p_output( p_output_in ), p_seeking( p_seeking_in ) {}
    };

    struct params_string
    {
        std::string * p_string;
        params_string( std::string * p_string_in ) : p_string( p_string_in ) {}
    };

    struct params_bool
    {
        bool * p_bool;
        params_bool( bool * p_bool_in ) : p_bool( p_bool_in ) {}
    };

    struct params_int
    {
        int * p_int;
        params_int( int * p_int_in ) : p_int( p_int_in ) {}
    };

    struct params_unsigned_int
    {
        unsigned int * p_int;
        params_unsigned_int( unsigned int * p_int_in ) : p_int( p_int_in ) {}
    };

    struct params_double
    {
        double * p_float;
        params_double( double * p_float_in ) : p_float( p_float_in ) {}
    };

    struct params_float
    {
        float * p_float;
        params_float( float * p_float_in ) : p_float( p_float_in ) {}
    };

protected:  // Give the user the chance to derive from this class and write their own handlers
    template< class Taction, class Tparams >
    dsl_pa_inline & invoke( const Taction &, const Tparams & params_in )
    {
        if( is_matched )
        {
            bool is_parsed = action( Taction(), params_in );
            if( ! is_optional )
                is_matched &= (is_parsed == wanted_result);
            wanted_result = true;
            is_optional = false;
        }
        return *this;
    }

    template< class Taction, class Tparams >
    dsl_pa_inline & invoke_sized( const Taction &, const Tparams & params_in )
    {
        if( is_matched )
        {
            last_size = action( Taction(), params_in );
            if( ! is_optional )
                is_matched &= ((last_size > 0) == wanted_result);
            wanted_result = true;
            is_optional = false;
        }
        return *this;
    }

public:
    dsl_pa_inline( const std::string & r_in )
        :
        my_reader( r_in ),
        pa( my_reader ),
        is_matched( true ),
        wanted_result( true ),
        is_optional( false ),
        last_size( 0 )
    {}
    dsl_pa_inline( const char * const p_in )
        :
        my_reader( p_in ),
        pa( my_reader ),
        is_matched( true ),
        wanted_result( true ),
        is_optional( false ),
        last_size( 0 )
    {}
    bool result() const { return is_matched; }
    operator bool () const { return result(); }
    dsl_pa_inline & not()
    {
        wanted_result = ! wanted_result;
        return *this;
    }
    dsl_pa_inline & optional()  // Call optional() BEFORE an operation
    {
        is_optional = true;
        return *this;
    }
    dsl_pa_inline & min_size( size_t min_size_in )  // Call min_size() AFTER a size-based operation
    {
        if( is_matched )
        {
            is_matched &= ((min_size_in <= last_size) == wanted_result);
            wanted_result = true;
        }
        return *this;
    }
    dsl_pa_inline & max_size( size_t max_size_in )  // Call min_size() AFTER a size-based operation
    {
        if( is_matched )
        {
            is_matched &= ((last_size <= max_size_in) == wanted_result);
            wanted_result = true;
        }
        return *this;
    }
    dsl_pa_inline & size( size_t min_size_in, size_t max_size_in )  // Call size() AFTER a size-based operation
    {
        // Can't do "min_size(...); return max_size(...);" here because wanted_result would be incorrectly modified in the middle
        if( is_matched )
        {
            is_matched &= ((min_size_in <= last_size && last_size <= max_size_in) == wanted_result);
            wanted_result = true;
        }
        return *this;
    }

private:
    bool action( const action_is_end &, const params_none & )
    {
        return pa.is_end();
    }
public:
    dsl_pa_inline & is_end()
    {
        return invoke( action_is_end(), params_none() );
    }

private:
    bool action( const action_is_char &, const params_char & params_in )
    {
        return pa.is_char( params_in.c );
    }
public:
    dsl_pa_inline & is_char( char c )
    {
        return invoke( action_is_char(), params_char( c ) );
    }

private:
    size_t action( const action_get &, const params_string_alphabet & params )
    {
        return pa.get( params.p_output, params.r_alphabet );
    }
public:
    dsl_pa_inline & get( std::string * p_output, const alphabet & r_alphabet )
    {
        return invoke_sized( action_get(), params_string_alphabet( p_output, r_alphabet ) );
    }

private:
    size_t action( const action_get &, const params_string_alphabet_size_t & params )
    {
        return pa.get( params.p_output, params.r_alphabet, params.max_chars );
    }
public:
    dsl_pa_inline & get( std::string * p_output, const alphabet & r_alphabet, size_t max_chars )
    {
        return invoke_sized( action_get(), params_string_alphabet_size_t( p_output, r_alphabet, max_chars ) );
    }

private:
    size_t action( const action_get_until &, const params_string_alphabet & params )
    {
        return pa.get_until( params.p_output, params.r_alphabet );
    }
public:
    dsl_pa_inline & get_until( std::string * p_output, const alphabet & r_alphabet )
    {
        return invoke_sized( action_get_until(), params_string_alphabet( p_output, r_alphabet ) );
    }

private:
    size_t action( const action_get_bounded_until &, const params_string_alphabet_size_t & params )
    {
        return pa.get_bounded_until( params.p_output, params.r_alphabet, params.max_chars );
    }
public:
    dsl_pa_inline & get_bounded_until( std::string * p_output, const alphabet & r_alphabet, size_t max_chars )
    {
        return invoke_sized( action_get_bounded_until(), params_string_alphabet_size_t( p_output, r_alphabet, max_chars ) );
    }

private:
    size_t action( const action_get_escaped_until &, const params_string_alphabet_char & params )
    {
        return pa.get_escaped_until( params.p_output, params.r_alphabet, params.escape_char );
    }
public:
    dsl_pa_inline & get_escaped_until( std::string * p_output, const alphabet & r_alphabet, char escape_char )
    {
        return invoke_sized( action_get_escaped_until(), params_string_alphabet_char( p_output, r_alphabet, escape_char ) );
    }

private:
    size_t action( const action_get_until &, const params_string_alphabet_char_size_t & params )
    {
        return pa.get_until( params.p_output, params.r_alphabet, params.escape_char, params.max_chars );
    }
public:
    dsl_pa_inline & get_until( std::string * p_output, const alphabet & r_alphabet, char escape_char, size_t max_chars )
    {
        return invoke_sized( action_get_until(), params_string_alphabet_char_size_t( p_output, r_alphabet, escape_char, max_chars ) );
    }

private:
    bool action( const action_fixed &, const params_char_p & params )
    {
        return pa.fixed( params.p_seeking );
    }
public:
    dsl_pa_inline & fixed( const char * p_seeking )
    {
        return invoke( action_fixed(), params_char_p( p_seeking ) );
    }

private:
    bool action( const action_ifixed &, const params_char_p & params )
    {
        return pa.ifixed( params.p_seeking );
    }
public:
    dsl_pa_inline & ifixed( const char * p_seeking )
    {
        return invoke( action_ifixed(), params_char_p( p_seeking ) );
    }

private:
    bool action( const action_get_fixed &, const params_string_char_p & params )
    {
        return pa.get_fixed( params.p_output, params.p_seeking );
    }
public:
    dsl_pa_inline & get_fixed( std::string * p_output, const char * p_seeking )
    {
        return invoke( action_get_fixed(), params_string_char_p( p_output, p_seeking ) );
    }

private:
    bool action( const action_get_ifixed &, const params_string_char_p & params )
    {
        return pa.get_ifixed( params.p_output, params.p_seeking );
    }
public:
    dsl_pa_inline & get_ifixed( std::string * p_output, const char * p_seeking )
    {
        return invoke( action_get_ifixed(), params_string_char_p( p_output, p_seeking ) );
    }

private:
    size_t action( const action_space &, const params_none & params )
    {
        return pa.space();
    }
public:
    dsl_pa_inline & space()
    {
        return invoke_sized( action_space(), params_none() );
    }

private:
    bool action( const action_opt_space &, const params_none & params )
    {
        return pa.opt_space();
    }
public:
    dsl_pa_inline & opt_space()
    {
        return invoke( action_opt_space(), params_none() );
    }

private:
    size_t action( const action_wsp &, const params_none & params )
    {
        return pa.wsp();
    }
public:
    dsl_pa_inline & wsp()
    {
        return invoke_sized( action_wsp(), params_none() );
    }

private:
    bool action( const action_opt_wsp &, const params_none & params )
    {
        return pa.opt_wsp();
    }
public:
    dsl_pa_inline & opt_wsp()
    {
        return invoke( action_opt_wsp(), params_none() );
    }

private:
    bool action( const action_get_bool &, const params_string & params )
    {
        return pa.get_bool( params.p_string );
    }
public:
    dsl_pa_inline & get_bool( std::string * p_string )
    {
        return invoke( action_get_bool(), params_string( p_string ) );
    }

private:
    bool action( const action_get_bool &, const params_bool & params )
    {
        return pa.get_bool( params.p_bool );
    }
public:
    dsl_pa_inline & get_bool( bool * p_bool )
    {
        return invoke( action_get_bool(), params_bool( p_bool ) );
    }

private:
    size_t action( const action_get_int &, const params_string & params )
    {
        return pa.get_int(params.p_string);
    }
public:
    dsl_pa_inline & get_int( std::string * p_string )
    {
        return invoke_sized( action_get_int(), params_string( p_string ) );
    }

private:
    size_t action( const action_get_int &, const params_int & params )
    {
        return pa.get_int( params.p_int );
    }
public:
    dsl_pa_inline & get_int( int * p_int )
    {
        return invoke_sized( action_get_int(), params_int( p_int ) );
    }

private:
    size_t action( const action_get_uint &, const params_string & params )
    {
        return pa.get_uint( params.p_string );
    }
public:
    dsl_pa_inline & get_uint( std::string * p_string )
    {
        return invoke_sized( action_get_uint(), params_string( p_string ) );
    }

private:
    size_t action( const action_get_uint &, const params_unsigned_int & params )
    {
        return pa.get_uint( params.p_int );
    }
public:
    dsl_pa_inline & get_uint( unsigned int * p_int )
    {
        return invoke_sized( action_get_uint(), params_unsigned_int( p_int ) );
    }

private:
    bool action( const action_get_float &, const params_string & params )
    {
        return pa.get_float( params.p_string );
    }
public:
    dsl_pa_inline & get_float( std::string * p_string )
    {
        return invoke( action_get_float(), params_string( p_string ) );
    }

private:
    bool action( const action_get_float &, const params_double & params )
    {
        return pa.get_float( params.p_float );
    }
public:
    dsl_pa_inline & get_float( double * p_float )
    {
        return invoke( action_get_float(), params_double( p_float ) );
    }

private:
    bool action( const action_get_float &, const params_float & params )
    {
        return pa.get_float( params.p_float );
    }
public:
    dsl_pa_inline & get_float( float * p_float )
    {
        return invoke( action_get_float(), params_float( p_float ) );
    }

private:
    bool action( const action_get_sci_float &, const params_string & params )
    {
        return pa.get_sci_float( params.p_string );
    }
public:
    dsl_pa_inline & get_sci_float( std::string * p_string )
    {
        return invoke( action_get_sci_float(), params_string( p_string ) );
    }

private:
    bool action( const action_get_sci_float &, const params_double & params )
    {
        return pa.get_sci_float( params.p_float );
    }
public:
    dsl_pa_inline & get_sci_float( double * p_float )
    {
        return invoke( action_get_sci_float(), params_double( p_float ) );
    }

private:
    bool action( const action_get_sci_float &, const params_float & params )
    {
        return pa.get_sci_float( params.p_float );
    }
public:
    dsl_pa_inline & get_sci_float( float * p_float )
    {
        return invoke( action_get_sci_float(), params_float( p_float ) );
    }
};

} // End of namespace cl

#endif // CL_DSL_PA_INLINE
