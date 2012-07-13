//----------------------------------------------------------------------
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
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// - Neither the name Codalogic nor the names of its contributors may be used
//   to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------

#include "clunit.h"

#include "dsl-pa/dsl-pa-lite.h"

using namespace cl;

TFUNCTION( dsl_lite_test )
{
    {
    dsl_pa_lite pal( "" );
    
    TTEST( pal.result() == true );
    }

    {
    TTEST( dsl_pa_lite( "" ).result() == true );
    }

    {
    dsl_pa_lite pal( std::string("") );
    
    TTEST( pal.result() == true );
    }

    {
    dsl_pa_lite pal( "s" );
    
    TTEST( pal.is_char( 's' ).result() == true );
    }

    {
    TTEST( dsl_pa_lite( "s" ).is_char( 's' ).is_end().result() == true );
    TTEST( dsl_pa_lite( "s" ).is_char( 's' ).is_end() == true );
    TTEST( dsl_pa_lite( "s" ).is_char( 's' ).not().is_end().result() == false );
    TTEST( dsl_pa_lite( "s" ).is_char( 's' ).not().is_end() == false );
    }

    {
    TTEST( dsl_pa_lite( "se" ).is_char( 's' ).not().is_end().result() == true );
    TTEST( dsl_pa_lite( "se" ).is_char( 's' ).not().is_end() == true );
    TTEST( dsl_pa_lite( "se" ).is_char( 's' ).is_end().result() == false );
    TTEST( dsl_pa_lite( "se" ).is_char( 's' ).is_end() == false );
    TTEST( dsl_pa_lite( "se" ).optional().is_char( 'v' ).is_char( 's' ).is_end() == false );
    }
    
    {
    int i;
    TTEST( dsl_pa_lite( "se" ).get_int( &i ) == false );
    TTEST( dsl_pa_lite( "se" ).optional().get_int( &i ) == true );
    TTEST( dsl_pa_lite( "se" ).optional().get_int( &i ).is_char( 's' ) == true );
    }
    
    {
    int i = 0;
    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).min_size( 2 ) == true );
    TTEST( i == 123 );
    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).min_size( 3 ) == true );
    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).min_size( 4 ) == false );

    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).max_size( 4 ) == true );
    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).max_size( 3 ) == true );
    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).max_size( 2 ) == false );
    TTEST( i == 123 );

    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).size( 2, 4 ) == true );
    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).size( 3, 4 ) == true );
    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).size( 4, 4 ) == false );
    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).size( 2, 3 ) == true );
    TTEST( dsl_pa_lite( "s123" ).is_char( 's' ).get_int( &i ).size( 2, 2 ) == false );
    }
    
    {
    float f = 0.0;
    TTEST( dsl_pa_lite( "s1.5v" ).is_char( 's' ).get_float( &f ).is_char( 'v' ) == true );
    TTEST( f == 1.5 );
    }
    
    {
    TTEST( dsl_pa_lite( "s123" ).fixed( "s12" ).is_char( '3' ) == true );
    TTEST( dsl_pa_lite( "s123" ).ifixed( "S12" ).is_char( '3' ) == true );
    }
    
    {
    std::string name;
    int i;
    TTEST( dsl_pa_lite( "foo : 123;" ).get( &name, alphabet_alpha() ).
            opt_space().is_char( ':' ).opt_space().get_int( &i ).is_char( ';' ) == true );
    TTEST( name == "foo" );
    TTEST( i == 123 );
    }
    
    {
    std::string name;
    int i;
    TTEST( dsl_pa_lite( "foo: 123;" ).get( &name, alphabet_alpha() ).
            opt_space().is_char( ':' ).opt_space().get_int( &i ).is_char( ';' ) == true );
    TTEST( name == "foo" );
    TTEST( i == 123 );
    }
    
    {
    std::string name;
    int i;
    TTEST( dsl_pa_lite( "foo :123;" ).get( &name, alphabet_alpha() ).
            opt_space().is_char( ':' ).opt_space().get_int( &i ).is_char( ';' ) == true );
    TTEST( name == "foo" );
    TTEST( i == 123 );
    }
    
    {
    std::string name;
    int i;
    TTEST( dsl_pa_lite( "foo 123;" ).get( &name, alphabet_not( alphabet_space() ) ).
            space().get_int( &i ).is_char( ';' ) == true );
    TTEST( name == "foo" );
    TTEST( i == 123 );
    }
    
    {
    std::string name;
    int i;
    TTEST( dsl_pa_lite( "foo:123;" ).get( &name, alphabet_alpha() ).
            opt_space().is_char( ':' ).opt_space().get_int( &i ).is_char( ';' ) == true );
    TTEST( name == "foo" );
    TTEST( i == 123 );
    }
    
    {
    int i;
    TTEST( dsl_pa_lite( "foo:123;" ).skip( alphabet_alpha() ).
            opt_space().is_char( ':' ).opt_space().get_int( &i ).is_char( ';' ) == true );
    TTEST( i == 123 );
    }
}
