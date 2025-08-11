// SPDX-License-Identifier: MIT
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
/// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
/// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
/// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///
/// \file phase_align.hxx
/// \brief Phase alignment for bit-plane operations.
/// \details PhaseAlign actively aligns source bits with destination bits during bit-plane operations.
/// When source bits occupy a different position within the scan byte than the destination bits,
/// PhaseAlign shifts them into alignment. It provides two main operations: prefetch and fetch.
/// Prefetching prepares the functor for phase alignment at the start of a scan line.
/// Fetching then loads and aligns the source bits in 8-bit chunks as the scan progresses.

#pragma once

#include "scan.hxx"

#include <cstddef>

namespace raster {

// PhaseAlign functor
// ~~~~~~~~~~ ~~~~~~~
// The PhaseAlign functor fetches source bits, aligning them with the
// destination bits.  If the source bits sit in a different part of the
// scan byte, PhaseAlign's job is to line them up with the destination bits.
// Its two operations are prefetch and fetch.  Calling prefetch at the
// start of a scan line prepares the functor for phase alignment.  Call-
// ing fetch thereafter loads and aligns the source in 8-bit chunks.
//
// The base PhaseAlign class does straight fetching---use it when source
// and destination are in line.  Subclasses LeftShift and RightShift
// derive from PhaseAlign; they line up the bits by shifting left or
// right.  Which way to shift depends on where, relative to the scan byte
// boundary, the first source bit lies compared with the first destinat-
// ion bit.  When to the right, source bits need shifting to the left,
// and vice versa.  When in phase, shifting is unnecessary.

class PhaseAlign {
public:
  virtual ~PhaseAlign() = default;
  virtual void prefetch() {}
  virtual scanbyte fetch() // Fetch() retrieves the next
  {                        // phase-aligned scan byte and steps
    return *store++;       // along the scan line.
  }
  scanbyte *store = nullptr;
};

class RightShift : public PhaseAlign {
public:
  scanbyte fetch() override;
  scanbyte carry = 0x00U;
  int shiftCount = 0;
};

class LeftShift : public RightShift {
public:
  void prefetch() override { carry = *store; }
  scanbyte fetch() override;
};

} // namespace raster
