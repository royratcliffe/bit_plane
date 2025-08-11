#include <bit_plane.hxx>

#include <iostream>

extern "C" int test_pat() {
  scanbyte vPatBits[] = {
      0x40U, // #. (black-white)
      0x80U, // .# (white-black)
  };
  BitPlane imagePat(2, 2, vPatBits);
  BitPlane image;
  image.create(32, 32);
  int y = 0;
  while (y < image.getHeight()) {
    int x = 0;
    while (x < image.getWidth()) {
      image.bitBlt(x, y, imagePat.getWidth(), imagePat.getHeight(), imagePat, 0, 0, Rop2::srcCopy);
      x += imagePat.getWidth();
    }
    y += imagePat.getHeight();
  }
  for (int x = 0; x < image.getWidth(); ++x) {
    for (int y = 0; y < image.getHeight(); ++y) {
      scanbyte v[] = {0x00U};
      BitPlane imageBit(1, 1, v);
      imageBit.bitBlt(0, 0, 1, 1, image, x, y, Rop2::srcCopy);
      scanbyte bit = v[0] >> 7;
      std::cout << (bit ? '#' : '.');
      if (bit != (x & 1U) ^ (y & 1U)) {
        return 1;
      }
    }
    std::cout << '\n';
  }
  return 0;
}
