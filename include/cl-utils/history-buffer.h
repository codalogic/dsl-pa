//----------------------------------------------------------------------------
// Copyright (c) 2017, Codalogic Ltd (http://www.codalogic.com)
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
// HistoryBuffer implementes a history recording mechanism that allows you to
// move forwards and backwards.  It is stack-like in behaviour in that you
// can `push` data onto the top of it.  However, once the buffer is full, when
// you push stuff onto the top of the stack, the item at the bottom of the
// stack is discarded.
//----------------------------------------------------------------------------

#ifndef CL_UTILS__HISTORY_BUFFER
#define CL_UTILS__HISTORY_BUFFER

#include <vector>
#include <cassert>

namespace clutils {

template< typename T >
class HistoryBuffer
{
private:
    struct Members {
        std::vector< T > buffer;
        size_t top;     // top points to the next free location. i.e. one above the active top of the stack
        size_t pos;     // pos points to the next location. i.e. one above the active location
        size_t bottom;
        Members() : top( 0 ), pos( 0 ), bottom( 0 ) {}
    } m;

public:
    HistoryBuffer( size_t buffer_size )
    {
        m.buffer.reserve( buffer_size + 1 );    // We use an extra place, so when the buffer has buffer_size members we can tell if it's full or empty
    }
    void push( const T & v )
    {
        if( m.top == m.pos && m.buffer.size() < m.buffer.capacity() )
            m.buffer.push_back( v );
        else
            m.buffer[m.pos] = v;
        m.top = m.pos = (m.pos + 1) % m.buffer.capacity();
        if( m.pos == m.bottom )
            m.bottom = (m.bottom + 1) % m.buffer.capacity();
    }
    bool has_back() const { return m.pos != m.bottom && m.pos != (m.bottom + 1) % m.buffer.capacity(); }
    void go_back()
    {
        assert( has_back() );
        if( has_back() )
        {
            if( m.pos == 0 )    // Can't use modulo arithmetic when doing unsigned subtraction
                m.pos = m.buffer.capacity() - 1;
            else
                --m.pos;
        }
    }
    bool has_frwd() const { return m.pos != m.top; }
    void go_frwd()
    {
        assert( has_frwd() );
        if( has_frwd() )
            m.pos = (m.pos + 1) % m.buffer.capacity();
    }
    const T & get() const
    {
        assert( m.pos != m.bottom );
        if( m.pos == 0 )    // Can't use modulo arithmetic when doing unsigned subtraction
            return m.buffer[m.buffer.capacity() - 1];
        else
            return m.buffer[m.pos - 1];
    }
};

} // namespace clutils

#endif // CL_UTILS__HISTORY_BUFFER
