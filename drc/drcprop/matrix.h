/*! 
  \brief Class for matrix operations

  not very complete. Only functions which are necessary.

*/


#ifndef MATRIX_H
#define MATRIX_H

#include "complex.h"

class Matrix
{
  public:  
  
  Matrix(); 
  
  Matrix(complex <double> m11, complex <double> m12, complex <double> m21, complex <double> m22);
  
  void SetMatrix11(complex<double> c) {fM11 = c;}
  void SetMatrix12(complex<double> c) {fM12 = c;}
  void SetMatrix21(complex<double> c) {fM21 = c;}
  void SetMatrix22(complex<double> c) {fM22 = c;}
  

  void product(complex <double> &out1, complex <double> &out2, complex <double> in1, complex <double> in2);
  
  private:

  complex <double> fM11,fM12,fM21,fM22;
  
  
};
#endif

