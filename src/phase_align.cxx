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
