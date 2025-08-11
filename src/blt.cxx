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
/// \file blt.cxx
/// \brief Blit operations for bit planes.
/// \details This file contains the implementation of the Blt class, which encapsulates
///          blit operations for bit planes.

#include "blt.hxx"

#define D (*store) // 8-bit destination operand
#define S fetch()  // 8-bit source operand

#define ropRevPolish(revPolish, x)                                                                                     \
  scanbyte Blt::rop##revPolish() { return x; }

ropRevPolish(0, 0x00U) ropRevPolish(DSon, ~(D | S)) ropRevPolish(DSna, D & ~S) ropRevPolish(Sn, ~S)
    ropRevPolish(SDna, S & ~D) ropRevPolish(Dn, ~D) ropRevPolish(DSx, D ^ S) ropRevPolish(DSan, ~(D &S))
        ropRevPolish(DSa, D &S) ropRevPolish(DSxn, ~(D ^ S)) ropRevPolish(D, D) ropRevPolish(DSno, D | ~S)
            ropRevPolish(S, S) ropRevPolish(SDno, S | ~D) ropRevPolish(DSo, D | S) ropRevPolish(1, 0xffU)

    // A BitPlane::Rop2 code translates to one of 16 rop() functions.  The
    // rop() functions execute their Boolean operation simultaneously on two 8-bit
    // operands, returning the 8-bit answer.  Static Blt-member vrop is an
    // array of pointers to the 16 Blt-class member functions implementing
    // the 16 raster operations.
    Blt::Rop Blt::vrop[] = {
        &Blt::rop0,   &Blt::ropDSon, &Blt::ropDSna, &Blt::ropSn,   &Blt::ropSDna, &Blt::ropDn,
        &Blt::ropDSx, &Blt::ropDSan, &Blt::ropDSa,  &Blt::ropDSxn, &Blt::ropD,    &Blt::ropDSno,
        &Blt::ropS,   &Blt::ropSDno, &Blt::ropDSo,  &Blt::rop1,
};
