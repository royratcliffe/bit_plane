#include <raster/bit_plane.hxx>

#include <cassert>
#include <iostream>

using namespace raster;

extern "C" int test_pat() {
  scanbyte vPatBits[] = {
      0x40U, // #. (black-white)
      0x80U, // .# (white-black)
  };
  BitPlane imagePat(2, 2, vPatBits);
  BitPlane image;
  // Create a new image in heap space. Make it 8x8 pixels.
  assert(image.create(8, 8));

  for (int y = 0; y < image.getHeight(); y += imagePat.getHeight()) {
    for (int x = 0; x < image.getWidth(); x += imagePat.getWidth()) {
      image.bitBlt(x, y, imagePat.getWidth(), imagePat.getHeight(), imagePat, 0, 0, Rop2::srcCopy);
    }
  }

  for (int x = 0; x < image.getWidth(); ++x) {
    for (int y = 0; y < image.getHeight(); ++y) {
      scanbyte v[] = {0x00U};
      BitPlane imageBit(1, 1, v);
      imageBit.bitBlt(0, 0, 1, 1, image, x, y, Rop2::srcCopy);
      scanbyte bit = v[0] >> 7;
      std::cout << (bit ? '#' : '.');
      assert(bit == ((x & 1U) ^ (y & 1U)));
    }
    std::cout << std::endl;
  }
  return 0;
}
