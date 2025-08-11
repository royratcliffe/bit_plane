#pragma once

/// \brief Binary raster operation.
enum Rop2 {
  rop0,
  ropDSon,
  ropDSna,
  ropSn,
  ropSDna,
  ropDn,
  ropDSx,
  ropDSan,
  ropDSa,
  ropDSxn,
  ropD,
  ropDSno,
  ropS,
  ropSDno,
  ropDSo,
  rop1,
  ropMax,
  notSrcErase = ropDSon,
  notSrcCopy = ropSn,
  srcErase = ropSDna,
  srcInvert = ropDSx,
  srcAnd = ropDSa,
  mergePaint = ropDSno,
  srcCopy = ropS,
  srcPaint = ropDSo,
};

/// \brief Unary raster operation.
enum Rop1 {
  blackness = rop0,
  dstInvert = ropDn,
  whiteness = rop1,
};
