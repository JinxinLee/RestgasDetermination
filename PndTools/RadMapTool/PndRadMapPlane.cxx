
#include "PndRadMapPlane.h"
#include <TMath.h>
#include <TFile.h>

#include <iostream>
#include <iomanip>
#include <utility>

//#include "G4UnitsTable.hh"

#define verbose true

//ClassImp(PndRadMapPlane);

PndRadMapPlane::PndRadMapPlane(){
  SetNormal(TVector3(0, 0, 0));
  SetDistance(0);
  corner1 = TVector3(0, 0, 0);
  corner2 = TVector3(0, 0, 0);
  corner3 = TVector3(0, 0, 0);
}

PndRadMapPlane::PndRadMapPlane(TVector3 _corner1, TVector3 _corner2, TVector3 _corner3,
             double dist){
  corner1 = _corner1;
  corner2 = _corner2;
  corner3 = _corner3;
  TVector3 dpv1 = corner2-corner1;
  TVector3 dpv2 = corner3-corner1;
  TVector3 n = (dpv1.Cross(dpv2)).Unit();
  SetNormal(n);
  SetDistance(dist);

  // std::cout << "p1: " << corner1.X() << ' ' << corner1.Y() << ' ' << corner1.Z() << std::endl;
  // std::cout << "p2: " << corner2.X() << ' ' << corner2.Y() << ' ' << corner2.Z() << std::endl;
  // std::cout << "p3: " << corner3.X() << ' ' << corner3.Y() << ' ' << corner3.Z() << std::endl;
  // std::cout << "dp1: " << dpv1.X() << ' ' << dpv1.Y() << ' ' << dpv1.Z() << std::endl;
  // std::cout << "dp2: " << dpv2.X() << ' ' << dpv2.Y() << ' ' << dpv2.Z() << std::endl;
  // std::cout << "normal: " << n.X() << ' ' << n.Y() << ' ' << n.Z() << std::endl;
}

TVector3 PndRadMapPlane::GetCorner(int i){
  switch(i){
  case 1:
    return Corner1();
  case 2:
    return Corner2();
  case 3:
    return Corner3();
  default:
    return TVector3(0, 0, 0);
  }
}



TVector3 PndRadMapPlane::LineIntersection(TVector3 begline, TVector3 endline){
  // plane: (p-corner1)*n = 0
  //    line: p=t*(end-beg)+beg
  //    (t*(end-beg) + beg - corner1)*n=0
  //    t*(end-beg)*n+(beg-corner1)*n=0
  //    n*(corner1-beg)/(n*(end-beg))=t


  TVector3 diffl = endline-begline;
  float nDotBeg = normal*(corner1-begline);
  float nDotDiff = normal*diffl;
  float tt = nDotBeg/nDotDiff;

  TVector3 ixp = begline + tt*diffl;

  // std::cout << diffl.X() << ' ' << diffl.Y() << ' ' << diffl.Z() << "   "
  //           << normal.X() << ' ' << normal.Y() << ' ' << normal.Z() << "   "
  //           << nDotBeg << ' ' << nDotDiff << ' ' << tt << std::endl;
  return ixp;

}



