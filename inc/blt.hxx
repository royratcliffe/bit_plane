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
/// \file blt.hxx
/// \brief Blit operations for bit planes.
/// \details This file contains the declaration of the Blt class, which encapsulates
///          blit operations for bit planes.

#pragma once

#include "phase_align.hxx"
#include "rop.hxx"

#include <cassert>

namespace raster {

// Blt functor
// ~~~ ~~~~~~~
// The Blt class encapsulates blit behaviour in a functor object.  Blt
// functors wrap up the raster fetch-logic-store operation.  BitBlt
// executes a fetch-logic-store on every scan byte intersecting the destin-
// ation rectangle.  Blt functor's primary operation therefore is
// fetchLogicStore.  Calling it fetches 8 phase-aligned bits from the
// source, performs the raster logic, stores the resulting bits at the
// destination, and steps along the scan line to the next 8 bits.  The
// operation is overloaded for masked and unmasked storing.  Ones in the
// mask specify which bits to store.
//
// FetchLogicStore asks fetchLogic to do the fetch and the logic.  In
// turn, fetchLogic runs the raster operation selected at construction
// time.  When the raster operation evaluates its source operand, Blt
// fetches source bits by calling the fetch method of the PhaseAlign
// functor.  Bits are not fetched unless the raster operation includes a
// source operand.

class Blt {
public:
  Blt(Rop2 rop2) : rop(vrop[rop2]) {
    assert(0 <= rop2 && rop2 < Rop2::ropMax); // ropMax == 16
  }
  void fetchLogicStore(scanbyte mask) { *store++ = (*store & ~mask) | (mask & fetchLogic()); }
  void fetchLogicStore() { *store++ = fetchLogic(); }
  scanbyte fetchLogic() {
    // Invoke the Boolean raster-operation; it calls fetch() by
    // referencing the source operand.
    return (this->*rop)();
  }
  scanbyte fetch() { return phaseAlign->fetch(); }
  scanbyte rop0();
  scanbyte ropDSon();
  scanbyte ropDSna();
  scanbyte ropSn();
  scanbyte ropSDna();
  scanbyte ropDn();
  scanbyte ropDSx();
  scanbyte ropDSan();
  scanbyte ropDSa();
  scanbyte ropDSxn();
  scanbyte ropD();
  scanbyte ropDSno();
  scanbyte ropS();
  scanbyte ropSDno();
  scanbyte ropDSo();
  scanbyte rop1();
  typedef scanbyte (Blt::*Rop)();
  static Rop vrop[];
  PhaseAlign *phaseAlign; // fetch: phaseAlign->fetch()
  Rop rop;                // logic: (this->*rop)()
  scanbyte *store;        // store: *store
};

} // namespace raster
