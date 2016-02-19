#ifndef __PndRadMapPlane_HH__
#define __PndRadMapPlane_HH__ 1
#include <TH2D.h> 
#include <TROOT.h>
#include <TFormula.h>
#include <TMatrixD.h>
#include <FairRadMapPoint.h>

class PndRadMapPlane {
 public:
  PndRadMapPlane();
  PndRadMapPlane(TVector3 _corner1, TVector3 _corner2, TVector3 _corner3, double dist = 0);
  TVector3 LineIntersection(TVector3 begline, TVector3 endline);
  void SetNormal(TVector3 n){normal = n;};
  void SetDistance(double d){distance = d;};
  TVector3 GetCorner(int i);
  TVector3 Corner1(){return corner1;};
  TVector3 Corner2(){return corner2;};
  TVector3 Corner3(){return corner3;};
  double Distance(){return distance;};
  TVector3 Normal(){return normal;};
 private:
  TVector3 normal;
  TVector3 corner1;
  TVector3 corner2;
  TVector3 corner3;
  double distance;
  //ClassDef(PndRadMapPlane,1);
};


#endif
