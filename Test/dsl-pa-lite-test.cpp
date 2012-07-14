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

TFUNCTION( dsl_pa_lite_extension_test )
{
    // dsl_pa_lite extension handlers are done by defining a separate class
    // whose parse() method is called during the relevant phase of the
    // parsing by making an instance of the class an argument of the 
    // template method dsl_pa_lite::x().  If the extension doesn't record any
    // parsed data then it can be created as a temporary object in the parse
    // sequence.  If it does record data then it needs to be created outsde
    // the parse chain and then referred to.
    class colon
    {
    public:
        void parse( dsl_pa_lite & r_dsl_pa_lite ) const
        {
            r_dsl_pa_lite.opt_space().is_char( ':' ).opt_space(); 
        }
    };

    {
    std::string name;
    int i;
    TTEST( dsl_pa_lite( "foo : 123;" ).get( &name, alphabet_alpha() ).
            x( colon() ).get_int( &i ).is_char( ';' ) == true );
    TTEST( name == "foo" );
    TTEST( i == 123 );
    }

    // Experimental external escape mechanism
    {
    std::string name;
    int i;
    TTEST( dsl_pa_lite( "foo : 123;" ).get( &name, alphabet_alpha() )
            [colon()].get_int( &i ).is_char( ';' ) == true );
    TTEST( name == "foo" );
    TTEST( i == 123 );
    }

    {
    std::string name;
    int i;
    TTEST( dsl_pa_lite( "foo - 123;" ).get( &name, alphabet_alpha() ).
            x( colon() ).get_int( &i ).is_char( ';' ) == false );
    }

    class date
    {
    private:
        int year;
        int month;
        int dom;
    public:
        date() {}
        void parse( dsl_pa_lite & r_dsl_pa_lite )
        {
            r_dsl_pa_lite.get_int( &year ).size( 4, 4 ).is_char( '-' ).
                    get_int( &month ).size( 2, 2 ).is_char( '-' ).
                    get_int( &dom ).size( 2, 2 ); 
        }
        int get_year() const { return year; }
        int get_month() const { return month; }
        int get_dom() const { return dom; }
    };

    {
    date my_date;
    TTEST( dsl_pa_lite( "date: 2012-02-09" ).fixed( "date" ).x( colon() ).
            x( my_date ) == true );
    TTEST( my_date.get_year() == 2012 );
    TTEST( my_date.get_month() == 2 );
    TTEST( my_date.get_dom() == 9 );
    }

    // Experimental external escape mechanism
    {
    date my_date;
    TTEST( dsl_pa_lite( "date: 2012-02-09" ).fixed( "date" )[colon()]
            [my_date] == true );
    TTEST( my_date.get_year() == 2012 );
    TTEST( my_date.get_month() == 2 );
    TTEST( my_date.get_dom() == 9 );
    }

    {
    date my_date;
    TTEST( dsl_pa_lite( "date: 2012+02+09" ).fixed( "date" ).x( colon() ).
            x( my_date ) == false );
    }
    
    {
    date my_date;
    TTEST( dsl_pa_lite( "date: 2012-02-09" )["date"][colon()]
            [my_date] == true );
    TTEST( my_date.get_year() == 2012 );
    TTEST( my_date.get_month() == 2 );
    TTEST( my_date.get_dom() == 9 );
    }

    // You can define extension handler in a class and then do all your calls to
    // dsl_pa_lite in that class.  This avoids poluting the global namespace
    // with lots of parser helper classes.
    class my_dsl_pa_lite
    {
    private:
        class dash
        {
        public:
            void parse( dsl_pa_lite & r_dsl_pa_lite ) const
            {
                r_dsl_pa_lite.opt_space().is_char( '-' ).opt_space(); 
            }
        };
    public:
        void my_dsl_pa_lite_test()
        {
            std::string input( "foo : - 123;" );
            std::string name;
            int i;
            TTEST( dsl_pa_lite( input ).get( &name, alphabet_alpha() ).
                    x( colon() ).   // Refer to a 'global' extension
                    x( dash() ).    // Refer to a local in class extension
                    get_int( &i ).is_char( ';' ) );
            TTEST( name == "foo" );
            TTEST( i == 123 );
        }
    };
    
    my_dsl_pa_lite my_pa_lite;
    my_pa_lite.my_dsl_pa_lite_test();
    
    {
    int year;
    unsigned int month;
    std::string dom;
    TTEST( dsl_pa_lite( "date:   2012-02-09" )
            ["date"][""][':'][" "][year]['-'][month]['-'][dom] == true );
    TTEST( year == 2012 );
    TTEST( month == 2 );
    TTEST( dom == "09" );
    TTEST( dsl_pa_lite( "date :   2012-02-09" )
            ["date"][""][':'][" "][year]['-'][month]['-'][dom] == true );
    TTEST( dsl_pa_lite( "date :   2012-02-09" )
            ["date"][colon()][year]['-'][month]['-'][dom] == true );
    }
}

// Sadly, trying to get the virtual function x() to return a reference to the
// deriving class doesn't work!
//
//TFUNCTION( dsl_pa_lite_extension_test )
//{
//    class my_dsl_pa_lite : public dsl_pa_lite
//    {
//    public:
//        my_dsl_pa_lite( const char * p_input ) : dsl_pa_lite( p_input ) {}
//        my_dsl_pa_lite( const std::string & r_input ) : dsl_pa_lite( r_input ) {}
//        my_dsl_pa_lite & x() { return *this; }
//        my_dsl_pa_lite & colon()
//        {
//            opt_space(); is_char( ':' );opt_space();
//            return *this;
//        }
//    };
//
//    int year = 0;
//    TTEST( my_dsl_pa_lite( "year : 2012" ).fixed( "year" ).x().colon().get_int( &year ) );
//    TTEST( year == 2012 );
//    
//    TTEST( my_dsl_pa_lite( std::striing("year : 2012") ).fixed( "year" ).x().colon().get_int( &year ) );
//}
