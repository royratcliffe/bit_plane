// SPDX-License-Identifier: MIT
///
/// \copyright 1996, 1998, 1999, 2002, 2025, Roy Ratcliffe, Northumberland, United Kingdom
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
/// \file bit_plane.cxx
/// \brief BitPlane class implementation.
/// \details This file contains the implementation of the BitPlane class, which represents a rectangular array of bits
///          mapped one-to-one with pixels. The class provides operations for creating, manipulating, and
///          destroying bit planes.

#include "raster/bit_plane.hxx"
#include "raster/blt.hxx"

#include <cassert> // for assert()
#include <cstring> // for memcpy()

namespace raster {

//**    Name
//
//      BitPlane --- rectangular arrays of bits
//
//**    Description
//
//      In windowing terminology, a BitPlane object is a top-down uncompr-
//      essed monochrome bitmap without a logical palette.  Vertical
//      indices map to pixel scan-lines starting at the top.  The first
//      row of the bit array corresponds to the top row of pixels.
//      BitPlane operations are listed below:
//
//              BitPlane()              constructs dynamic bit-planes
//              BitPlane(cx, cy, v)     constructs static bit-planes
//              create(cx,cy)           allocates free store
//              bitBlt(..., rop2)       blits two bit-plane operands
//              bitBlt(..., rop1)       blits one bit-plane operand
//              ~BitPlane()             de-allocates free store
//              getWidth()              gets the width
//              getHeight()             gets the height
//
//      You construct a BitPlane two ways: dynamically or statically.
//      The default constructor makes an empty bit plane.  Its initial
//      dimensions are zero width by zero height; nothing will transfer
//      if you attempt to blit to it or from it.  You ask create to take
//      memory out of the free store and turn it into a bit plane of
//      specified width and height.  Create returns false or throws an
//      allocation exception if there's not enough memory.  You stati-
//      cally initialize a BitPlane by giving the parameterized const-
//      ructor its width, height and array address.  The extract below
//      illustrates the alternative ways of making BitPlanes.
//
//              scanbyte vPatBits[] =
//              {
//                0x40U, // #. (black-white)
//                0x80U, // .# (white-black)
//              };
//              BitPlane imagePat(2, 2, vPatBits);
//              BitPlane image;
//              image.create(32, 32);
//              int y = 0;
//              while ( y < image.getHeight() )
//              {
//                int x = 0;
//                while ( x < image.getWidth() )
//                {
//                  image.bitBlt(x, y,
//                    imagePat.getWidth(), imagePat.getHeight(),
//                    imagePat, 0, 0, BitPlane::srcCopy);
//                  x += imagePat.getWidth();
//                }
//                y += imagePat.getHeight();
//              }
//
//**********************************************************************

BitPlane::BitPlane() : width(0), height(0), store(0) {
  // Setting width and height to zero makes the BitPlane valid and
  // safe for blitting.  When one or both are zero, the value of
  // widthScanBytes doesn't matter: blit operations won't get past
  // clipping.  When store is NULL, autoDelete is really a don't-
  // care because delete []NULL is okay.
  autoDelete = false;
}

BitPlane::BitPlane(int cx, int cy, scanbyte v[]) {
  if (cx < 0)
    cx = -cx;
  if (cy < 0)
    cy = -cy;
  if (cx > 0 && cy > 0) {
    width = cx;
    height = cy;
    widthScanBytes = cx >> 3;
    if (cx & 7)
      ++widthScanBytes;
  } else {
    width = 0;
    height = 0;
  }
  store = v;
  autoDelete = false;
}

// Don't bit-wise copy a BitPlane!
BitPlane::BitPlane(const BitPlane &copy) {
  scanbyte *v;
  if (copy.autoDelete) {
    v = new scanbyte[copy.widthScanBytes * copy.height];
    if (v == nullptr)
      return;
    (void)memcpy(v, copy.store, sizeof(scanbyte) * copy.widthScanBytes * copy.height);
  } else
    v = copy.store;
  width = copy.width;
  height = copy.height;
  widthScanBytes = copy.widthScanBytes;
  store = v;
  autoDelete = copy.autoDelete;
}

//**********************************************************************
//                                                      BitPlane::create
//**********************************************************************
//
//**    Synopsis
//
//      void create(cx, cy)
//      int cx;
//      int cy;
//
//**    Description
//
//      The create operation dynamically creates a bit plane on the free
//      store with the specified width and height.  The destructor
//      releases the bit block memory.  To free it another way, execute
//      create(0, 0).  In the event of allocation failure, create either
//      returns false or throws xalloc, and in any case the BitPlane is
//      empty (0 by 0).
//
//**********************************************************************

bool BitPlane::create(int cx, int cy) {
  if (cx < 0) // Permit negative widths and
    cx = -cx; // heights by negating them just
  if (cy < 0) // like extents.
    cy = -cy;
  if (cx <= 0 || cy <= 0) // <= traps INT_MIN
    return false;

  // How to create a new bit plane: first, dispose of the old one; next,
  // compute the scan line size in double-words; finally, allocate free-
  // storage for the bits.
  this->~BitPlane();
  widthScanBytes = cx >> 3;
  if (cx & 7)
    ++widthScanBytes;
  store = new scanbyte[widthScanBytes * cy];
  if (store == nullptr)
    return false;
  autoDelete = true;
  width = cx;
  height = cy;
  return true;
}

// BitPlane::findBits(x,y)
// ~~~~~~~~~~~~~~~~~~~~~~~
// Given the co-ordinate of a bit, findBits returns the address of its
// scan byte.  The calculation assumes one bit per pixel and scan byte-aligned
// scan lines --- BitPlane class constraints.  Expression x & 7 gives
// the bit's position within the scan byte; where 0 corresponds to the most
// significant bit, 7 to bit zero.  The x and y co-ordinates aren't
// clipped.  FindBits is a protected helper.

inline scanbyte *BitPlane::findBits(int x, int y) const { return store + widthScanBytes * y + (x >> 3); }

inline const scanbyte *BitPlane::bits(int x, int y) const { return findBits(x, y); }

//**********************************************************************
//                                                      BitPlane::bitBlt
//**********************************************************************
//
//**    Synopsis
//
//      bool bitBlt(x, y, cx, cy, bitPlaneSrc, xSrc, ySrc, rop2)
//      bool bitBlt(x, y, cx, cy, rop1)
//      int x;                          // horizontal destination origin
//      int y;                          // vertical destination origin
//      int cx;                         // horizontal extent
//      int cy;                         // vertical extent
//      const BitPlane& bitPlaneSrc;    // source bit-plane
//      int xSrc;                       // horizontal source origin
//      int ySrc;                       // vertical source origin
//      BitPlane::Rop2 rop2;            // binary raster-operation
//      BitPlane::Rop1 rop1;            // unary raster-operation
//
//**    Description
//
//      BitBlt performs a bit-block transfer from a given source plane
//      to ``this'' bit plane.  Parameter bitPlaneSrc specifies the source
//      from where bitBlt transfers a rectangular block of bits.  ``This''
//      BitPlane is the destination.  Other arguments indicate the dest-
//      ination origin, rectangle extent, source origin, and raster
//      operation.
//
//      Parameters x, y, cx and cy specify the destination rectangle.
//      BitBlt writes to bits enclosed by the destination rectangle;
//      bits outside the rectangle are unmodified.  BitBlt clips the
//      destination rectangle against the bit plane's borders.  If it
//      extends beyond any border, the rectangle clips to the bit-plane
//      intersection.  If clipping moves the destination's origin bitBlt
//      offsets the source origin by an equal amount, so the bits trans-
//      fer to the position you specify --- rather than its intersection
//      with the plane.  Parameters xSrc and ySrc specify the source
//      origin.  BitBlt only fetches those bits intersecting the source
//      plane.
//
//      BitBlt transfers bits from one plane to the other using a raster
//      operation defining how to logically combine source bits with
//      destination bits.  The rop2 parameter selects one of 16 binary
//      raster-operations.  They are Boolean functions presented in rev-
//      erse Polish notation, wherein D means the destination bit-plane
//      operand, S the source operand, and lower-case letters a, n, o
//      and x represent bitwise operators AND, NOT, OR and XOR respecti-
//      vely.  Operation ``DSon'' for example means bitwise-OR destination
//      and source then invert.
//
//**********************************************************************

bool BitPlane::bitBlt(int x, int y, int cx, int cy, const BitPlane &bitPlaneSrc, int xSrc, int ySrc, Rop2 rop2) {
  // Normalize the extents.  Extents are normally positive.  A negative
  // extent means the destination and source origins specify the far
  // edge of the rectangle.  Two's-complement negative extents and put
  // the origins at the rectangle's origins.
  if (cx < 0) {
    cx = -cx;
    x -= cx;
    xSrc -= cx;
  }
  assert(cx >= 0);
  if (cy < 0) {
    cy = -cy;
    y -= cy;
    ySrc -= cy;
  }
  assert(cy >= 0);

  // What if the destination rectangle (x, y, cx, cy) falls outside the
  // destination plane?  A plane's origin is 0, 0; its width and height
  // define its extent.  Similarly, the source rectangle (xSrc, ySrc,
  // cx, cy) may not be entirely inside the source plane, bitPlaneSrc.
  // There are two origins and one extent --- the transfer rectangle has
  // one size but may be in different places in the two planes.  Clip
  // the origins; the destination origin against the destination plane,
  // the source origin against the source plane.
  //
  // If an origin is negative, offset both origins and proportionately
  // deflate the extent.  Calculate the amount of offset first.  The
  // offset is zero if neither origin is negative.  The rectangle does
  // not intersect the plane at all if the amount of offset isn't less
  // than the extent; the extent must be greater (this catches zero ext-
  // ents too).  Compress the remaining extent even further if it exten-
  // ds beyond the boundaries of the bit planes.
  int xOff = x < 0 ? (x < xSrc ? -x : -xSrc) : (xSrc < 0 ? -xSrc : 0);
  assert(xOff >= 0);
  if (xOff >= cx)
    return false;
  x += xOff;
  xSrc += xOff;
  cx -= xOff;
  assert(x >= 0 && xSrc >= 0 && 0 < cx);
  // cxMax is the difference betw-
  // een the width and the origin,
  // i.e. the maximum transfer
  // extent possible.  If zero or
  // negative, the origin is bey-
  // ond the plane.
  int cxMax = width - x;
  if (0 >= cxMax)
    return false;
  if (cxMax < cx)
    cx = cxMax;
  cxMax = bitPlaneSrc.width - xSrc;
  if (0 >= cxMax)
    return false;
  if (cxMax < cx)
    cx = cxMax;
  int yOff = y < 0 ? (y < ySrc ? -y : -ySrc) : (ySrc < 0 ? -ySrc : 0);
  assert(yOff >= 0);
  if (yOff >= cy)
    return false;
  y += yOff;
  ySrc += yOff;
  cy -= yOff;
  assert(y >= 0 && ySrc >= 0 && 0 < cy);
  int cyMax = height - y;
  if (0 >= cyMax)
    return false;
  if (cyMax < cy)
    cy = cyMax;
  cyMax = bitPlaneSrc.height - ySrc;
  if (0 >= cyMax)
    return false;
  if (cyMax < cy)
    cy = cyMax;

  // Decide how to fetch the source bits.  There are three PhaseAlign
  // functors to choose from, based on how the bits are out of phase.
  // The destination alignment is x & 7, i.e. how many bits from the
  // left side of the scan byte.  Expression xSrc & 7 gives the source
  // alignment.  The sign and magnitude of the difference between the
  // alignments determines the direction and amount of shift.
  Blt blt(rop2);
  PhaseAlign fetch;
  RightShift fetchRightShift;
  LeftShift fetchLeftShift;
  int shiftCount = (x & 7) - (xSrc & 7);
  if (shiftCount < 0) {
    fetchLeftShift.shiftCount = -shiftCount;
    blt.phaseAlign = &fetchLeftShift;
  } else if (shiftCount == 0)
    blt.phaseAlign = &fetch;
  else {
    fetchRightShift.shiftCount = shiftCount;
    blt.phaseAlign = &fetchRightShift;
  }

  // This blit implementation always iterates down the scan lines top-
  // to-bottom, and steps across the scan-line scan bytes left-to-right.
  // Stepping directions matter when the source plane is ``this'' plane
  // and the source area overlaps the destination.  Because the algor-
  // ithm scans top-to-bottom and steps left-to-right regardless, the
  // outcome is undefined if source and destination overlap --- likely
  // the resulting bit pattern is not what you want or expect so beware!
  //
  // The position of the last bit in the scan line is given by x+cx-1
  // (assuming cx is positive, i.e. 0<cx).  Dividing this by 8 (right-
  // shifting three times) gives you the position of the last scan byte in
  // the scan line, relative to the start of the scan line.  How many
  // scan bytes in each scan line intersect the blitting region?  There's
  // always at least one because 0<cx.  Expression (xMax>>3) - (x>>3)
  // yields how many ``extra'' scan bytes per scan line after the first.
  const int xMax = x + cx - 1;
  const int extraScanByteCount = (xMax >> 3) - (x >> 3);
  const scanbyte scanOrgMask = 0xffU >> (x & 7);
  const scanbyte scanExtMask = 0xffU << (7 - (xMax & 7));
  const int displace = widthScanBytes - 1 - extraScanByteCount;
  const int displaceSrc = bitPlaneSrc.widthScanBytes - 1 - extraScanByteCount;
  blt.store = findBits(x, y);
  blt.phaseAlign->store = bitPlaneSrc.findBits(xSrc, ySrc);
  if (extraScanByteCount == 0) {
    // The scan line's bits begin and end in the same scan byte.  There's
    // just one fetchLogicStore every scan line, so optimize the blit
    // by merging the masks for a single-step fetch-logic-store.
    const scanbyte scanMask = scanOrgMask & scanExtMask;
    while (cy--) {
      blt.phaseAlign->prefetch();
      blt.fetchLogicStore(scanMask);
      blt.store += displace;
      blt.phaseAlign->store += displaceSrc;
    }
  } else {
    while (cy--) {
      blt.phaseAlign->prefetch();
      blt.fetchLogicStore(scanOrgMask);
      int scanByteCount = extraScanByteCount;
      while (--scanByteCount)
        blt.fetchLogicStore();
      blt.fetchLogicStore(scanExtMask);
      blt.store += displace;
      blt.phaseAlign->store += displaceSrc;
    }
  }
  return true;
}

bool BitPlane::bitBlt(int x, int y, int cx, int cy, Rop1 rop1) {
  // Unary operations aren't fully optimized in this version of BitPlane.
  // Convert it to a binary raster-operation, specifying the destination
  // as the source.  BitBlt will set up the PhaseAlign functor unnecess-
  // arily, but because it is unary the operation will not fetch bits
  // from the source.
  return bitBlt(x, y, cx, cy, *this, x, y, Rop2(rop1));
}

////////////////////////////////////////////////////////////////////////

} // namespace raster
