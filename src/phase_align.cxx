/// SPDX-License-Identifier: MIT
///
/// \copyright 2025, Roy Ratcliffe, Northumberland, United Kingdom
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
/// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sub-license, and/or sell copies of the Software, and to
/// permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
///     The above copyright notice and this permission notice shall be included in all copies or substantial
///     portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
/// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
/// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
/// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///
/// \file phase_align.cxx
/// \brief Phase alignment for bit-plane operations.
/// \details This file contains the implementation of the PhaseAlign class and its derived classes,
///          which handle the fetching and storing of bits with respect to their phase alignment.

#include "phase_align.hxx"

// Exchange and shift --- some in-line assembler macros.  GNU has a
// tricky optimizer.  Watch the operand constraints!
//
//              xchgl(r,g)              32-bit exchange
//              shrdl(c,r,g)            64-bit right shift
//              shldl(c,r,g)            64-bit left shift
//
// 32-bit exchange swaps the contents of a register with a register or
// memory.  It reads operands r and g then writes operands r and g.
// Double shift instructions input c, r and g then output g; clobbering
// the flags along the way.
inline void xchgl(scanbyte &r, scanbyte &g) {
  scanbyte t = r;
  r = g;
  g = t;
}
inline void shrdl(int c, scanbyte &r, scanbyte &g) { g = (r << (8 - c)) | (g >> c); }
inline void shldl(int c, scanbyte &r, scanbyte &g) { g = (g << c) | (r >> (8 - c)); }

scanbyte RightShift::fetch() {
  scanbyte hi;
  scanbyte lo;
  hi = lo = *store++; // post-increment
  xchgl(hi, carry);
  shrdl(shiftCount, hi, lo);
  return lo;
}

scanbyte LeftShift::fetch() {
  // Load 64 bits of the scan line into registers.  In hexadecimal
  // nybbles: FEDCBA9876543210.  Prefetch or the last fetch loaded F..8,
  // the high bits.  This fetch loads 7..0, the low bits.  Carry 7..0
  // over to the next fetch.  Shift all 64 bits left by shiftCount and
  // return the high 32 bits.
  scanbyte hi;
  scanbyte lo;
  hi = lo = *++store; // pre-increment
  xchgl(hi, carry);
  shldl(shiftCount, lo, hi); // FEDCBA98 76543210 << shiftCount
  return hi;
}
