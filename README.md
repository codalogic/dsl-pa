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
            pa.opt_space() && pa.is_get_char( '=' ) && pa.opt_space() &&
            pa.get_uint( &width ) &&
            pa.opt_space() && pa.is_get_char( ',' ) && pa.opt_space() &&
            pa.fixed( "height" ) &&
            pa.opt_space() && pa.is_get_char( '=' ) && pa.opt_space() &&
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
            opt_space() && is_get_char( '=' ) && opt_space() &&
            get_uint( &width ) &&
            opt_space() && is_get_char( ',' ) && opt_space() &&
            fixed( "height" ) &&
            opt_space() && is_get_char( '=' ) && opt_space() &&
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
    bool eq() { return opt_space() && is_get_char( '=' ) && opt_space(); }
    bool comma() { return opt_space() && is_get_char( ',' ) && opt_space(); }
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
    // locator is a class that uses RAII to ensure that a
    // location_push() has a corresponding location_pop() when the
    // function exits.
    locator my_location( get_reader() );

    if( get_int( &p_date->year ) == 4 &&
            is_get_char( '-' ) &&
            get_int( &p_date->month ) == 2 &&
            is_get_char( '-' ) &&
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

## QStrings

dsl_pa supports reading JSON-like QStrings into a UTF-8 encoded std::string
using the `dsl_pa::get_qstring_contents( std::string * p_string, char delimter = '"' )` method.  The
method assumes that the opening quotation mark has already been read, and
consumes the closing quotation mark.

By default, `dsl_pa::get_qstring_contents()` assumes the string is delimted
by `"` characters.  Use the `delimiter` parameter to set an alternative
delimiter.

## Accumulators

With a more literal translation of an ABNF grammar to a parser, common
sub-expressions such as DIGIT may be used in a number of places.  The
`accumulate()` method along with the `accumulator` and `accumulator_deferred`
classes support directing this input to the desired string.

Use the `accumulator` and `accumulator_deferred` classes to specify the string
that should receive the input.
```c++
void example_parser::example8( std::ostream & fout )
{
    // Input is ABC3D
    accumulator accumulated( this );

    while( ALPHA() || DIGIT() )
    {}
    if( accumulated.get() == "ABC3D" )
        fout << "Example OK: Accumulated " << accumulated.get() << "\n";
    else
        fout << "Example Failed: Accumulated " << accumulated.get() << "\n";
}

bool example_parser::ALPHA()
{
    return accumulate( alphabet_alpha() );
}

bool example_parser::DIGIT()
{
    return accumulate( alphabet_digit() );
}
```

An `accumulator` object selects itself as the active character accumulator
immediately, whereas an `accumulator_deferred` object does not.

`accumulator::select()` makes the accumulator object the active accumulator.
The `select()` method needs to be called on an `accumulator_deferred` object
for it to be made the active accumulator.
`accumulator::previous()` restores the accumulator that was active when
the new accumulator was constructed to being the
active accumulator.  `accumulator::none()` stops any accumulation.
`accumulator::clear()` clears the characters that have been accumulated by the
object on which is it called, but does not change which is the active accumulator.
`accumulator::select_and_clear()` does the `select()` and `clear()` operations
in one method for convenience.  All of these method return `true` so they can be
chained together along with other `dsl_pa` methods.

When an accumulator is destructed it calls the `previous()` method.  This is because
accumulators are assumed to be stack-based objects that are strictly nested.

There are a number of `dsl_pa::accumlate()` variants.  These are:
`dsl_pa::accumlate( const alphabet & )`, `dsl_pa::accumlate( char )`,
and `dsl_pa::accumlate_all( const alphabet & )`.  The `accumlate()`
variants only accumulate a single character at a time, whereas the `accumlate_all()`
variant will accumulate all characters that match the input `alphabet`.

There is also the `dsl_pa::acumulator_append()` methods.  These methods unconditionally
append the specified char or string to the active accumulator.  Their use-case is for
code such as `is_get_char( 'n' ) && accumulator_append( '\n' ) || is_get_char( 'r' )
&& accumulator_append( '\r' )`.

`dsl_pa::accumulator_to_previous()` will append the contents of the current
accumulator to the previous accumulator, if one is present.

`dsl_pa::accumulate_atomic( bool is_valid )` will append the contents of the
current accumulator to the previous accumulator if `is_valid` is `true`, or
clear its contents if `is_valid` is `false`.  It returns the value of
`is_valid`.  It is intended to be used in statements such as:

```c++
bool fragment()
{
    accumulator acc( this );

    return rewind_on_reject( accumulate_atomic( /* dsl-pa calls */ ) ) ||
            rewind_on_reject( accumulate_atomic( /* dsl-pa calls */ ) ) ||;
            rewind_on_reject( accumulate_atomic( /* dsl-pa calls */ ) );
}
```

To retrieve the accumulated value from an accumulator as a string, use the `get()`
method.  The accumulated value can also be retrieved as an int using the `to_int()`
method and as a float / double using the `to_float()` method.

## Parser Function Return Codes

As stated above, each parser function (either from the library or user generated) returns a
Boolean.  It's worth exploring a bit further exactly what that Boolen value represents.
When you have code of the form:

```c++
if( path_a() || path_b() )
```

then returning `true` for `path_a()` indicates that *path_a* was taken.  It does not necessarily
mean that *path_a* was successfuly parsed without errors.  This is particularly the case when
you are not trying to do a simple 'this message is a valid' parsing (in which case you can let
both paths indicate that they are not the correct path), but trying to recover from
errors and continue processing, as you might of you are trying to present parsing results to a
human.

As a result, there are a number of scenarios for what to return:

| Path correct?  | Errors occured? | Action                                                          |
|----------------|-----------------|-----------------------------------------------------------------|
| Path correct   | No errors       | Return true                                                     |
| Path correct   | Errors          | Record error, re-sync & return `true` **or** use retreat action |
| Path incorrect | Not relevant    | Return false                                                    |

dsl-pa-lite
===========
dsl-pa-lite is a wrapper class that uses dsl-pa for situations where only simple parsing
of input is required.  In some respects it is a light-weight alternative to parsing
input via regular expressions.
```c++
    std::string my_input( "foo:123;" );
    ...
    int i;
    if( dsl_pa_lite( my_input ).skip( alphabet_alpha() ).
            opt_space().is_char( ':' ).opt_space().
            get_int( &i ).is_char( ';' ) == true )
    {
        // Do something
    }
```
In addition to combining matches via standard method concatenation, overloads of
operator [] can be used to specify patterns, for example:
```c++
    std::string my_input( "date:   2012-02-09" );
    ...
    int year;
    unsigned int month;
    std::string dom;
    if( dsl_pa_lite( my_input )
            ["date"][""][':'][" "][&year]['-'][&month]['-'][&dom] == true )
    {
        // Do something
    }
```
Here '[""]' is used to mean "skip whitespace".

Parser creation from ABNF
=========================

`abnf2dsl-pa.py` in the `abnf2dsl-pa` directory is provided to assist with writing a parser from an ABNF
definition.  `abnf2dsl-pa.py` is not a comprehensive solution to this problem.  It only creates outline
C++ code based on the ABNF definition.  Much additional work will be requyired to make a complete parser.

A suggested workflow to create a parser from ABNF is to first use `abnf2dsl-pa.py` to create outline code.
Fill in the generated methods with suitable content so that example messages can be successfully parsed,
but not actually record any of the message details.  Once this is done, work through the methods again
adding code to record the information stored in the message format into appropriate C++ class members.

For more information see <https://github.com/codalogic/dsl-pa/blob/master/README.html>.
