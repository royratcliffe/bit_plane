#include "phase_align.hxx"
#include "rop.hxx"

// Blt functor
// ~~~ ~~~~~~~
// The Blt class encapsulates blit behaviour in a functor object.  Blt
// functors wrap up the raster fetch-logic-store operation.  BitBlt
// executes a fetch-logic-store on every longword intersecting the destin-
// ation rectangle.  Blt functor's primary operation therefore is
// fetchLogicStore.  Calling it fetches 32 phase-aligned bits from the
// source, performs the raster logic, stores the resulting bits at the
// destination, and steps along the scan line to the next 32 bits.  The
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
    // static_assert(0 <= rop2 && rop2 < Rop2::ropMax); // ropMax == 16
  }
  void fetchLogicStore(longword mask) { *store++ = (*store & ~mask) | (mask & fetchLogic()); }
  void fetchLogicStore() { *store++ = fetchLogic(); }
  longword fetchLogic() {
    // Invoke the Boolean raster-operation; it calls fetch() by
    // referencing the source operand.
    return (this->*rop)();
  }
  longword fetch() { return phaseAlign->fetch(); }
  longword rop0();
  longword ropDSon();
  longword ropDSna();
  longword ropSn();
  longword ropSDna();
  longword ropDn();
  longword ropDSx();
  longword ropDSan();
  longword ropDSa();
  longword ropDSxn();
  longword ropD();
  longword ropDSno();
  longword ropS();
  longword ropSDno();
  longword ropDSo();
  longword rop1();
  typedef longword (Blt::*Rop)();
  static Rop vrop[];
  PhaseAlign *phaseAlign; // fetch: phaseAlign->fetch()
  Rop rop;                // logic: (this->*rop)()
  longword *store;        // store: *store
};
