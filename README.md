---
# https://rstudio.github.io/visual-markdown-editing/markdown.html
editor_options:
  markdown:
    mode: markdown
    extensions: +tex_math_dollars+raw_tex
    wrap: 72
---

<!-- -*- mode: markdown; extensions: +raw_tex -*- -->

# Bit Plane Raster Library

The Bit Plane Raster Library is a lightweight C++ toolkit designed for
high-performance manipulation of binary raster images. By leveraging
bit-plane techniques, it enables efficient storage, transformation, and
rendering of bitmap data, making it suitable for graphics, embedded
systems, and image processing applications.

## Overview

The *Bit Plane Raster Library* provides efficient classes and utilities
for manipulating arbitrary raster bitmaps using two-dimensional
bit-plane techniques: phase alignment, scan masking, unary and binary
raster operations. The library encapsulates all core classes within the
`raster` namespace for clarity and modularity.

## Features

The library offers a focused set of features for binary raster image
manipulation, emphasising speed, memory efficiency, and flexibility. Its
core components are designed to handle common raster graphics tasks,
including bit-level image storage, logical operations, and optimised
data transfer between bit planes. Key features include:

`BitPlane` class

:   Represents a two-dimensional array of bits for binary image
    operations.

`Blt` class

:   Encapsulates blit (bit-block transfer) operations between bit
    planes.

`PhaseAlign` class

:   Handles phase alignment during bit-plane operations.

`Rop` type

:   Enumerates raster operations for binary *and* unary logic.

`scanbyte` type

:   Defines the scan byte type for bit-plane storage.

### Planes of bits

Bit planes are layers within a bitmap image where each plane contains a
single bit of information for every pixel. In binary raster graphics,
each bit plane represents a specific significance (such as the least or
most significant bit) across the entire image. Manipulating bit planes
allows for efficient operations like masking, pattern generation, and
logical raster transformations.

In the Bit Plane Raster Library, bit planes are managed as
two-dimensional arrays of bits, enabling compact storage and fast
access. This approach is beneficial for embedded systems and graphics
applications where memory and performance are critical.

### Blitting

Blitting, or bit-block transfer, is a fundamental operation in raster
graphics that involves copying rectangular blocks of bits from a source
bit plane to a destination bit plane. The Bit Plane Raster Library
provides robust support for blitting, allowing you to efficiently move,
combine, and transform bitmap regions.

Blitting operations can include simple copying, logical raster
operations (such as AND, OR, XOR), and masking. The library's `Blt`
class encapsulates these operations, handling alignment, clipping, and
phase adjustments automatically. This enables high-performance rendering
and manipulation of binary images, which is essential for tasks like
sprite drawing, pattern filling, and image compositing.

By abstracting the complexity of bit-level transfers, the library
ensures that blitting is both fast and easy to use, making it suitable
for real-time graphics and resource-constrained environments.

### Phase alignment

Phase alignment refers to the adjustment of bit positions when
performing operations between bit planes that may not be byte-aligned.
In raster graphics, bit-level manipulations often require precise
control over how bits shift and merge during transfer, especially when
copying or combining regions that start at arbitrary pixel coordinates.

The Bit Plane Raster Library's `PhaseAlign` class automates phase
alignment, ensuring that source and destination bit planes correctly
synchronise during blits. Aligning the "phase" prevents unwanted
artefacts and maintains data integrity by handling partial-byte overlaps
and aligning bits across scanlines. Proper phase alignment is essential
for accurate rendering and efficient memory access in binary image
processing.

### Raster operations

Raster operations (ROPs) are logical functions applied to pixel data
during bit-plane manipulations. These operations determine how source
and destination bits are combined, enabling effects such as masking,
inversion, and pattern generation.

The Bit Plane Raster Library defines a comprehensive set of raster
operations through the `Rop` enumeration. Common ROPs include:

-   `srcCopy`: Copies bits directly from the source to the destination.
-   `srcAnd`: Performs a bitwise AND between source and destination
    bits.
-   `srcPaint`: Performs a bitwise OR between source and destination
    bits.
-   `srcInvert`: Performs a bitwise XOR between source and destination
    bits.
-   `notSrcCopy`: Inverts the source bits before applying them.
-   `ropDn`: Inverts the destination bits.

These operations are essential for tasks such as masking regions,
combining images, and implementing graphical effects. The library's
blitting functions allow you to specify the desired raster operation,
ensuring flexible and efficient manipulation of binary raster images.

## Usage

Include the headers from the `raster` namespace:

``` cpp
#include <raster/bit_plane.hxx>
```

Create and manipulate bit planes:

``` cpp
using namespace raster;

scanbyte vPatBits[] = { 0x40U, 0x80U };
BitPlane imagePat(2, 2, vPatBits);
BitPlane image;
assert(image.create(8, 8));

for (int y = 0; y < image.getHeight(); y += imagePat.getHeight()) {
    for (int x = 0; x < image.getWidth(); x += imagePat.getWidth()) {
        image.bitBlt(x, y, imagePat.getWidth(), imagePat.getHeight(), imagePat, 0, 0, BitPlane::srcCopy);
    }
}
```

This example outputs an 8-by-8 pattern-rendered bit plane:

    Output:
    ----------------------------------------------------------
    .#.#.#.#
    #.#.#.#.
    .#.#.#.#
    #.#.#.#.
    .#.#.#.#
    #.#.#.#.
    .#.#.#.#
    #.#.#.#.
    <end of output>
    Test time =   0.00 sec
    ----------------------------------------------------------
    Test Passed.

## Building

This project uses CMake. To build:

``` sh
cmake -S . -B build
cmake --build build
```

## Testing

Run the tests with:

``` sh
ctest --test-dir build
```

## License

SPDX-License-Identifier: MIT

Copyright © Roy Ratcliffe

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sub-license, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial
    portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
