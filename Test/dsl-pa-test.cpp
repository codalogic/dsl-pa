//----------------------------------------------------------------------
//                        Copyright (c) 2012 Codalogic.
//
// This file is currently not licensed for public use.
//----------------------------------------------------------------------

#define CLUNIT_HOME
#include "clunit.h"

void basic_test()
{
	TTODO( "Master line counting" );
	TTODOX( 1 == 0 );
}

static cl::clunit t1(basic_test);

int main( int argc, char * argv[] )
{
	TRUNALL();
}
