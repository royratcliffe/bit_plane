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
    // rop() functions execute their Boolean operation simultaneously on 16
    // bits, returning the 8-bit answer.  Static Blt-member vrop is an
    // array of pointers to the 16 Blt-class member functions implementing
    // the 16 raster operations.
    Blt::Rop Blt::vrop[] = {
        &Blt::rop0,   &Blt::ropDSon, &Blt::ropDSna, &Blt::ropSn,   &Blt::ropSDna, &Blt::ropDn,
        &Blt::ropDSx, &Blt::ropDSan, &Blt::ropDSa,  &Blt::ropDSxn, &Blt::ropD,    &Blt::ropDSno,
        &Blt::ropS,   &Blt::ropSDno, &Blt::ropDSo,  &Blt::rop1,
};
