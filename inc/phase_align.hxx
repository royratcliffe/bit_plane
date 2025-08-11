#pragma once

#include "scan.hxx"

#include <cstddef>

// PhaseAlign functor
// ~~~~~~~~~~ ~~~~~~~
// The PhaseAlign functor fetches source bits, aligning them with the
// destination bits.  If the source bits sit in a different part of the
// scan byte, PhaseAlign's job is to line them up with the destination bits.
// Its two operations are prefetch and fetch.  Calling prefetch at the
// start of a scan line prepares the functor for phase alignment.  Call-
// ing fetch thereafter loads and aligns the source in 32-bit chunks.
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
