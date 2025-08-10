#include "scan.hxx"

// PhaseAlign functor
// ~~~~~~~~~~ ~~~~~~~
// The PhaseAlign functor fetches source bits, aligning them with the
// destination bits.  If the source bits sit in a different part of the
// longword, PhaseAlign's job is to line them up with the destination bits.
// Its two operations are prefetch and fetch.  Calling prefetch at the
// start of a scan line prepares the functor for phase alignment.  Call-
// ing fetch thereafter loads and aligns the source in 32-bit chunks.
//
// The base PhaseAlign class does straight fetching---use it when source
// and destination are in line.  Subclasses LeftShift and RightShift
// derive from PhaseAlign; they line up the bits by shifting left or
// right.  Which way to shift depends on where, relative to the longword
// boundary, the first source bit lies compared with the first destinat-
// ion bit.  When to the right, source bits need shifting to the left,
// and vice versa.  When in phase, shifting is unnecessary.

class PhaseAlign {
public:
  virtual void prefetch() {}
  virtual longword fetch() // Fetch() retrieves the next
  {                        // phase-aligned longword and steps
    return *store++;       // along the scan line.
  }
  longword *store;
};

class RightShift : public PhaseAlign {
public:
  virtual longword fetch();
  longword carry;
  int shiftCount;
};

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
inline void xchgl(longword &r, longword &g) {
  longword t = r;
  r = g;
  g = t;
}
inline void shrdl(int c, longword &r, longword &g) { g = (r << (32 - c)) | (g >> c); }
inline void shldl(int c, longword &r, longword &g) { g = (g << c) | (r >> (32 - c)); }

longword RightShift::fetch() {
  longword hi;
  longword lo;
  hi = lo = *store++; // post-increment
  xchgl(hi, carry);
  shrdl(shiftCount, hi, lo);
  return lo;
}

class LeftShift : public RightShift {
public:
  virtual void prefetch() { carry = *store; }
  virtual longword fetch();
};

longword LeftShift::fetch() {
  // Load 64 bits of the scan line into registers.  In hexadecimal
  // nybbles: FEDCBA9876543210.  Prefetch or the last fetch loaded F..8,
  // the high bits.  This fetch loads 7..0, the low bits.  Carry 7..0
  // over to the next fetch.  Shift all 64 bits left by shiftCount and
  // return the high 32 bits.
  longword hi;
  longword lo;
  hi = lo = *++store; // pre-increment
  xchgl(hi, carry);
  shldl(shiftCount, lo, hi); // FEDCBA98 76543210 << shiftCount
  return hi;
}
