  //-*- Mode: C++ -*-
  // *****************************************************************************
  //                                                                             *
  // @Autors: I.Kulakov; M.Zyzak; I.Kisel                                        *
  // @e-mail: I.Kulakov@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
  //                                                                             *
  // *****************************************************************************

#ifndef PNDCAFIELDVALUE_H
#define PNDCAFIELDVALUE_H

struct PndCAFieldValue{
   public:
  PndCAFieldValue():fcBz(0){}
  
  PndCAFieldValue(float f):fcBz(f){}
  operator float() const { return fcBz; }

 private:
  float fcBz;
};

#endif
