  //-*- Mode: C++ -*-
  // *****************************************************************************
  //                                                                             *
  // @Autors: I.Kulakov; M.Zyzak; I.Kisel                                        *
  // @e-mail: I.Kulakov@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
  //                                                                             *
  // *****************************************************************************

#ifndef PNDCAStation_H
#define PNDCAStation_H

struct PndCAStripInfo {
  float sin, cos;
};

struct PndCAStation{
  float r;
  float ErrY;
  float ErrZ;

  PndCAStripInfo f, b;
  float xOverX0;
  float xTimesRho;

  char NDF;
  char CellLength;
};

#endif
