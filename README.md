dsl-pa
======

dsl-pa is a C++ Domain Specific Language Parsing Assistant library designed to
take advantage of the C++ logic shortcut operators such as && and ||.
dsl-pa function calls extract sections of language input based on a
specified alphabet.  Calls to dsl-pa functions are combined using the C++
shortcut operators in order to parse a language in a reasonably compact
form.  Consequently all dsl-pa functions return either a boolean value or
a value that can be implicitly converted to a bool, such as an integer.
The idea is that the set of shortcut operations should mirror the
[BNF](http://en.wikipedia.org/wiki/Backus%E2%80%93Naur_Form "BNF Notation") or
similar notation (such as
[ABNF](http://en.wikipedia.org/wiki/Augmented_Backus%E2%80%93Naur_Form  "ABNF Notation"))
that is used to describe the langauge being parsed.
Various reader classes can be used to read from file, std::string and
memory buffer sources.

dsl-pa's license is based on the BSD-3-Clause license.  See
<https://github.com/codalogic/dsl-pa/blob/master/LICENSE> for details.

```c++
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
    if( pa.opt_space() &&
            pa.fixed( "width" ) &&
            pa.opt_space() && pa.is_char( '=' ) && pa.opt_space() &&
            pa.get_uint( &width ) &&
            pa.opt_space() && pa.is_char( ',' ) && pa.opt_space() &&
            pa.fixed( "height" ) &&
            pa.opt_space() && pa.is_char( '=' ) && pa.opt_space() &&
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
```
Or you can inherit the dsl_pa class and implement your parser in the inherited class
```c++
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

    if( opt_space() &&
            fixed( "width" ) &&
            opt_space() && is_char( '=' ) && opt_space() &&
            get_uint( &width ) &&
            opt_space() && is_char( ',' ) && opt_space() &&
            fixed( "height" ) &&
            opt_space() && is_char( '=' ) && opt_space() &&
            get_uint( &height ) )
    {
        fout << "Example 2 OK: w=" << width << " & h=" << height << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}
```
You can make the above syntax easier to parse if you make you own helper functions to
make the space surrounding the key tokens seem as if they are implicit.  For example,
by including the following in your parser class:
```c++
    bool eq() { return opt_space() && is_char( '=' ) && opt_space(); }
    bool comma() { return opt_space() && is_char( ',' ) && opt_space(); }
```
you can change the above code to:
```c++
    if( opt_space() &&
            fixed( "width" ) && eq() && get_uint( &width ) &&
            comma() &&
            fixed( "height" ) && eq() && get_uint( &height ) )
```
For machine-to-machine languages you can usually survive with fairly
coarse error reporting.  For finer-grain error reporting you can use
the dsl_pa::error() methods:
(P.S. The excessive number of brackets in the shortcuts are to avoid
gcc warnings.)
```c++
    try
    {
        if( opt_space() &&
                ((fixed( "width" ) && eq() && get_uint( &width ))
                    || error( "Expected width parameter" )) &&
                ((comma())
                    || error( "Expected comma separator" )) &&
                ((fixed( "height" ) && eq() && get_uint( &height ))
                    || error( "Expected height parameter" )) )
        {
            fout << "Example OK: w=" << width << " & h=" << height << "\n";
        }
        else
        {
            fout << "Unable to parse input\n";
        }
    }
    catch( const dsl_pa_recoverable_exception & e )
    {
        fout << e.what() << "\n";
    }
```
dsl-pa has additional helper functions to record and update state
information in the middle of shortcut sequences.  These are
dsl_pa::set(), record(), clear() and append():
```c++
    if( opt_space() &&
            fixed( "width" ) && eq() && get_uint( &width ) &&
            set( is_width_found, true ) &&
            comma() &&
            fixed( "height" ) && eq() && get_uint( &height ) &&
            set( is_height_found, true ) )
```
If your language syntax is defined in some form of BNF (such as ABNF)
and it indicates that a non-terminal is present, then it is recommended
that you create your own function to parse that non-terminal rather
than expand the non-terminal inline (e.g. get_date() below):
```c++
bool example_parser::get_date( date * p_date )
{
    // location_logger is a class that uses RAII to ensure that a
    // location_push() has a corresponding location_pop() when the
    // function exits.
    location_logger my_location( get_reader() );

    if( get_int( &p_date->year ) == 4 &&
            is_char( '-' ) &&
            get_int( &p_date->month ) == 2 &&
            is_char( '-' ) &&
            get_int( &p_date->dom ) == 2 )
        return true;

    location_top();

    return false;
}

void example_parser::example6( std::ostream & fout )
{
    size_t width;
    date my_date;

    if( opt_space() &&
            fixed( "when" ) && eq() && get_date( &my_date ) &&
            comma() &&
            fixed( "width" ) && eq() && get_uint( &width ) )
    {
        fout << "Example OK: w=" << width << " & year=" << my_date.year << "\n";
    }
    else
    {
        fout << "Unable to parse input (Possibly the year is the wrong format)\n";
    }
}
```
To parse an input corresponding to a specific set of characters you can
use the dsl_pa::get() method and specify the alphabet you want, such as 
the roman_numerals alphabet described below. Alternatively you can use 
dsl_pa::get_until() to get characters by specifying an alphabet that 
specifies the set of characters you don't want.  A number of alphabets 
are built-in, such as alphabet_space as used below.  The built-in 
alphabets are efficient to construct and hence can be constructed in 
place, but alphabet_char_class alphabets require some computation to 
setup and are therefore better setup as const global objects:
```c++
const alphabet_char_class roman_numerals( "IVXLCM" );

void example_parser::example7( std::ostream & fout )
{
    std::string roman_number;
    std::string non_space;

    if( opt_space() &&
            fixed( "year" ) && eq() && get( &roman_number, roman_numerals ) &&
            comma() &&
            get_until( &non_space, alphabet_space() ) )
    {
        fout << "Example OK: w=" << roman_number << " & non_space=" << non_space << "\n";
    }
    else
    {
        fout << "Unable to parse input\n";
    }
}
```
For more information see <https://github.com/codalogic/dsl-pa/blob/master/README.html>.
