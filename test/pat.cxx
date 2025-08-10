#include <bit_plane.hxx>

extern "C" int test_pat() {
  longword vPatBits[] = {
      0x40, 0x00, 0x00, 0x00, // #. (black-white)
      0x80, 0x00, 0x00, 0x00, // .# (white-black)
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
  return 0;
}
