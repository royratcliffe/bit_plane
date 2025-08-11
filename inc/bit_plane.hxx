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
/// \file bit_plane.hxx
/// \brief BitPlane class definition and operations.
/// \details This file contains the definition of the BitPlane class, which represents a rectangular array of bits
///          mapped one-to-one with pixels. The class provides operations for creating, manipulating, and
///          destroying bit planes.

#pragma once

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

namespace raster {

/// \class BitPlane
/// \brief Represents a two-dimensional bit-plane for binary image operations.
///
/// The BitPlane class provides functionality for creating, copying, and manipulating
/// binary images using bit-block transfers and raster operations. It manages memory
/// for the bit-plane data and supports both unary and binary raster operations.
///
/// \note The class handles dynamic memory allocation for the bit-plane scan bytes,
/// and ensures proper cleanup in the destructor.
///
/// \section Usage
/// - Create an empty or parameterised bit-plane.
/// - Perform bit-block transfers with raster operations.
/// - Access bit-plane dimensions and raw scan bytes.
///
/// \section RasterOperations Raster Operations
/// Supports both unary (Rop1) and binary (Rop2) raster operations for bit-block transfers.
///
/// \section MemoryManagement Memory Management
/// The class manages its own memory for the bit-plane data, with an option to
/// auto-delete the allocated scan bytes.
class BitPlane {

public:
  /// \brief Default constructor.
  /// \details Constructs an empty bit-plane.
  BitPlane();

  /// \brief Parameterised constructor.
  /// \param cx Width of the bit-plane.
  /// \param cy Height of the bit-plane.
  /// \param v Pointer to the bit-plane scan bytes.
  BitPlane(int cx, int cy, scanbyte v[]);

  /// \brief Copy constructor.
  /// \param copy Bit-plane to copy.
  BitPlane(const BitPlane &copy);

  /// \brief Create a new dynamic memory allocated bit-plane.
  /// \param cx Width of the bit-plane.
  /// \param cy Height of the bit-plane.
  /// \return True if successful, false otherwise.
  bool create(int cx, int cy);

  /// \brief Bit-block transfer with binary raster operation.
  /// \param x Destination x-coordinate.
  /// \param y Destination y-coordinate.
  /// \param cx Width of the destination rectangle.
  /// \param cy Height of the destination rectangle.
  /// \param bitPlaneSrc Source bit-plane.
  /// \param xSrc Source x-coordinate.
  /// \param ySrc Source y-coordinate.
  /// \param rop2 Raster operation.
  /// \return True if successful, false otherwise.
  bool bitBlt(int x, int y, int cx, int cy, const BitPlane &bitPlaneSrc, int xSrc, int ySrc, Rop2 rop2);

  /// \brief Bit-block transfer with unary raster operation.
  /// \param x Destination x-coordinate.
  /// \param y Destination y-coordinate.
  /// \param cx Width of the destination rectangle.
  /// \param cy Height of the destination rectangle.
  /// \param rop1 Raster operation.
  /// \return True if successful, false otherwise.
  bool bitBlt(int x, int y, int cx, int cy, Rop1 rop1);

  /// \brief Destructor.
  /// \details Destroys the bit-plane and releases any allocated resources.
  ~BitPlane() {
    width = 0;
    height = 0;
    if (autoDelete) {
      delete[] store;
      autoDelete = false;
    }
    store = nullptr;
  }

  /// \brief Get the width of the bit-plane.
  /// \return Width of the bit-plane.
  int getWidth() const { return width; }

  /// \brief Get the height of the bit-plane.
  /// \return Height of the bit-plane.
  int getHeight() const { return height; }

protected:
  int width = 0;             ///< Width of the bit-plane.
  int height = 0;            ///< Height of the bit-plane.
  int widthScanBytes = 0;    ///< Width of the bit-plane in scan bytes.
  scanbyte *store = nullptr; ///< Pointer to the bit-plane scan bytes.
  bool autoDelete = false;   ///< Flag indicating whether to delete the bit-plane.

  /// \brief Find the scan byte containing the bit at the specified coordinates.
  /// \param x X-coordinate of the bit.
  /// \param y Y-coordinate of the bit.
  /// \return Pointer to the scan byte containing the bit, or nullptr if out of bounds.
  scanbyte *findBits(int x, int y) const;

public:
  /// \brief Get a pointer to the bits at the specified coordinates.
  /// \param x X-coordinate of the bits.
  /// \param y Y-coordinate of the bits.
  /// \return Pointer to the bits at the specified coordinates.
  const scanbyte *bits(int x, int y) const;
};

} // namespace raster
