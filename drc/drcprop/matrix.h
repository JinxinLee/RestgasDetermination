
#ifndef MATRIX_H
#define MATRIX_H

#include "complex.h"

class Matrix
{
  public:  
  
  Matrix();
  
  Matrix(complex <double> m11, complex <double> m12, complex <double> m21, complex <double> m22);
  
  void product(complex <double> &out1, complex <double> &out2, complex <double> in1, complex <double> in2);
  
  private:

  complex <double> fM11,fM12,fM21,fM22;
  
  
};
#endif

