//c++ `root-config --glibs --cflags` -o orthonormal orthonormal.cxx

#include"TMatrixT.h"
#include"TVector3.h"

#include<iostream>

int main(){

  TMatrixT<double> M1(3,3);

  TVector3 u(1.,1.,0.);
  u.SetMag(1.);

  TVector3 v = u.Cross(TVector3(1.,0.,0.));
  v.SetMag(1.);

  TVector3 w = u.Cross(v);
  w.SetMag(1.);

  M1[0][0] = u.X();
  M1[1][0] = u.Y();
  M1[2][0] = u.Z();
  M1[0][1] = v.X();
  M1[1][1] = v.Y();
  M1[2][1] = v.Z();
  M1[0][2] = w.X();
  M1[1][2] = w.Y();
  M1[2][2] = w.Z();



  u.Print();
  v.Print();
  w.Print();
  std::cout << "u*v=" << u*v << std::endl;
  std::cout << "u*w=" << u*w << std::endl;
  std::cout << "v*w=" << v*w << std::endl;


  M1.Print();
  TMatrixT<double> Mtran = M1;
  Mtran.T();
  TMatrixT<double> Minv = M1;
  Minv.Invert();

  M1.Print();
  Mtran.Print();
  Minv.Print();
}
