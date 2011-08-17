
#include <cstdlib>

#include "complex.h"
#include "matrix.h"


Matrix::Matrix()
{
  fM11=0;
  fM12=0;
  fM21=0;
  fM22=0;
}
Matrix::Matrix(complex <double> m11, complex <double> m12, complex <double> m21, complex <double> m22)
{
  fM11=m11;
  fM12=m12;
  fM21=m21;
  fM22=m22;
}

void Matrix::product(complex <double> &out1, 
		     complex <double> &out2, 
		     complex <double> in1, 
		     complex <double> in2)
{
  out1 = fM11*in1 + fM12*in2;
  
  out2 = fM21*in1 + fM22*in2;


}

