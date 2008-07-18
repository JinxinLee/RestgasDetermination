//rootcint -f DetPlaneDict.cxx -c -DHAVE_CONFIG ../DetPlane.h DetPlaneLinkDef.h; g++ DetPlaneTest.cxx ../DetPlane.cxx `root-config --cflags --glibs` DetPlaneDict.cxx
#include "../DetPlane.h"
#include "TVector3.h"
#include<iostream>


int main(){

  TVector3 O(1.,0.,0.);
  TVector3 n(0.09,0.,1.);
  DetPlane p(O,n);

  TVector3 o,u,v;

  o = p.getO();
  u = p.getU();
  v = p.getV();

  o.Print();
  u.Print();
  v.Print();

  std::cout << u.Mag() << std::endl;
  std::cout << v.Mag() << std::endl;

  std::cout << n*u << std::endl;
  std::cout << n*v << std::endl;
  std::cout << v*u << std::endl;

  p.setNormal(1.,0.3);
  o = p.getO();
  u = p.getU();
  v = p.getV();

  o.Print();
  u.Print();
  v.Print();

  std::cout << u.Mag() << std::endl;
  std::cout << v.Mag() << std::endl;

  std::cout << n*u << std::endl;
  std::cout << n*v << std::endl;
  std::cout << v*u << std::endl;

}
