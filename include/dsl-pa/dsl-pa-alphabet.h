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

#ifndef CL_DSL_PA_ALPHABET
#define CL_DSL_PA_ALPHABET

#include <cctype>

namespace cl {

class alphabet
{
public:
	virtual bool is_wanted( char c ) const = 0;

	// Helper functions.  <cctype> functions are often not 8-bit safe
	static bool is_7bit( char c )
	{
		return c >= 0 && c <= 0x7f;	
	}
	static bool is_space( char c )
	{
		return is_7bit( c ) && isspace( c );	
	}
	static bool is_digit( char c )
	{
		return '0' <= c && c <= '9';
	}
	static bool is_alpha( char c )
	{
		return 'a' <= c && c <= 'z' ||
				'A' <= c && c <= 'Z';
	}
	static bool is_hex( char c )
	{
		return is_digit( c ) ||
				'a' <= c && c <= 'f' ||
				'A' <= c && c <= 'F';
	}
	static bool is_utf8_multibyte( char c )
	{
		return ! is_7bit( c );
	}
};

// Common alphabets

class alphabet_ws : public alphabet
{
public:
	virtual bool is_wanted( char c ) const
	{
		return is_space( c );
	}
};

class alphabet_digit : public alphabet
{
public:
	virtual bool is_wanted( char c ) const
	{
		return is_digit( c );
	}
};

class alphabet_hex : public alphabet
{
public:
	virtual bool is_wanted( char c ) const
	{
		return is_hex( c );
	}
};

class alphabet_alpha : public alphabet
{
public:
	virtual bool is_wanted( char c ) const
	{
		return is_alpha( c );
	}
};

class alphabet_word_char : public alphabet
{
public:
	virtual bool is_wanted( char c ) const
	{
		return is_alpha( c ) ||
				is_digit( c ) ||
				'_' == c;
	}
};

class alphabet_word_first_char : public alphabet
{
public:
	virtual bool is_wanted( char c ) const
	{
		return is_alpha( c ) || '_' == c;
	}
};

class alphabet_uni : public alphabet // char is part of a non-ASCII Unicode sequence
{
public:
	virtual bool is_wanted( char c ) const
	{
		return is_utf8_multibyte( c );
	}
};

// alphabet combiners - These classes allow combining alphabets.  However, it is
// probably more preferable to create your own alphabet classes than rely 
// extensively on these classes.

class alphabet_not : public alphabet
{
private:
	const alphabet & r_alphabet;

public:
	alphabet_not( const alphabet & r_alphabet_in )
		: r_alphabet( r_alphabet_in )
	{}
	virtual bool is_wanted( char c ) const
	{
		return ! r_alphabet.is_wanted( c );
	}
};

class alphabet_or : public alphabet
{
private:
	const alphabet & r_alphabet_1;
	const alphabet & r_alphabet_2;

public:
	alphabet_or(
			const alphabet & r_alphabet_1_in,
			const alphabet & r_alphabet_2_in )
		:
		r_alphabet_1( r_alphabet_1_in ),
		r_alphabet_2( r_alphabet_2_in )
	{}
	virtual bool is_wanted( char c ) const
	{
		return r_alphabet_1.is_wanted( c ) ||
				r_alphabet_2.is_wanted( c );
	}
};

namespace /*cl::*/ short_alphabets {
	typedef alphabet_ws					wspace;
	typedef alphabet_digit				digit;
	typedef alphabet_hex				hex;
	typedef alphabet_alpha				alpha;
	typedef alphabet_word_first_char	word_first_char;
	typedef alphabet_word_char			word_char;
	typedef alphabet_uni				uni;
	typedef alphabet_not				not;
	typedef alphabet_or					or;
} // End of namespace cl::short_alphabets

} // End of namespace cl

#endif // CL_DSL_PA_ALPHABET
