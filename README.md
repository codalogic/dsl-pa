dsl-pa
======

dsl-pa is a C++ Domain Specific Language Parsing Assistant library designed to
take advantage of the C++ logic shortcut operators such as && and ||.
dsl-pa function calls extract sections of language input based on a
specified alphabet.  Calls to dsl-pa functions are combined using the C++
shortcut operators in order to parse a language in a reasonably compact
form.  Consequently all dsl-pa functions return either a boolean value or
a value that can be implicitly converted to a bool, such as an integer.
Various reader classes can be used to read from file, std::string and
memory buffer sources.

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
    if( pa.opt_ws() &&
            pa.fixed( "width" ) &&
            pa.opt_ws() && pa.is_char( '=' ) && pa.opt_ws() &&
            pa.get_uint( &width ) &&
            pa.opt_ws() && pa.is_char( ',' ) && pa.opt_ws() &&
            pa.fixed( "height" ) &&
            pa.opt_ws() && pa.is_char( '=' ) && pa.opt_ws() &&
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

    if( opt_ws() &&
            fixed( "width" ) &&
            opt_ws() && is_char( '=' ) && opt_ws() &&
            get_uint( &width ) &&
            opt_ws() && is_char( ',' ) && opt_ws() &&
            fixed( "height" ) &&
            opt_ws() && is_char( '=' ) && opt_ws() &&
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