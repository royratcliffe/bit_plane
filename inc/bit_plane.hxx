//**    $Id: BitPlane.hh,v 1.4 2002/03/22 16:17:31 royr Exp $
//
//      Copyright (C) 1996, 1998, 1999, 2002, Roy Ratcliffe, United Kingdom.
//      All rights reserved.
//
//      Permission is hereby granted, free of charge,  to any person obtaining a
//      copy  of  this  software  and    associated   documentation  files  (the
//      "Software"), to deal in  the   Software  without  restriction, including
//      without limitation the rights to  use,   copy,  modify,  merge, publish,
//      distribute, sublicense, and/or sell  copies  of   the  Software,  and to
//      permit persons to whom the Software is   furnished  to do so, subject to
//      the following conditions:
//
//          The above copyright notice and this permission notice shall be
//          included in all copies or substantial portions of the Software.
//
//      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT  WARRANTY OF ANY KIND, EXPRESS
//      OR  IMPLIED,  INCLUDING  BUT  NOT   LIMITED    TO   THE   WARRANTIES  OF
//      MERCHANTABILITY, FITNESS FOR A PARTICULAR   PURPOSE AND NONINFRINGEMENT.
//      IN NO EVENT SHALL THE AUTHORS  OR   COPYRIGHT  HOLDERS BE LIABLE FOR ANY
//      CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER   IN  AN ACTION OF CONTRACT,
//      TORT OR OTHERWISE, ARISING FROM,  OUT  OF   OR  IN  CONNECTION  WITH THE
//      SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//**    Name
//
//      BitPlane --- rectangular arrays of bits
//
//**    Description
//
//      A ``bit plane'' is a rectangular array of bits mapped one-to-one
//      with pixels.  It is a collection of bits in memory representing
//      an image to be displayed or printed.
//
//              +----------------------+
//              |                      |
//              |       BitPlane       |
//              |                      |
//              |----------------------|       +-------+
//              | width                |       |       |
//              | height               |       | lword |
//              |----------------------|-------|       |
//              | BitPlane()           |       | array |
//              | BitPlane(cx,cy,v)    |       |       |
//              | create(cx,cy)        |       +-------+
//              | bitBlt(...,rop2)     |
//              | bitBlt(...,rop1)     |
//              | ~BitPlane()          |
//              +----------------------+
//
//      BitPlanes don't possess colour tables.  Nominally however, plane
//      bits have the sense: 0 is black, 1 is white.
//
//**********************************************************************

#include "rop.hxx"
#include "scan.hxx"

class BitPlane {

public:
  BitPlane();
  BitPlane(int cx, int cy, scanbyte v[]);
  // Don't bit-wise copy a BitPlane!
  BitPlane(const BitPlane &copy);

  bool create(int cx, int cy);

  bool bitBlt(int, int, int, int, const BitPlane &, int, int, Rop2); // binary
  bool bitBlt(int, int, int, int, Rop1);                             // unary

  ~BitPlane() {
    width = 0;
    height = 0;
    if (autoDelete) {
      delete[] store;
      autoDelete = false;
    }
    store = nullptr;
  }

  int getWidth() const { return width; }
  int getHeight() const { return height; }

protected:
  int width = 0;
  int height = 0;
  int widthScanBytes = 0;
  scanbyte *store = nullptr;
  bool autoDelete = false;
  scanbyte *findBits(int x, int y) const;

public:
  const scanbyte *bits(int x, int y) const;
};
