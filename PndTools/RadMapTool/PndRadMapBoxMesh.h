#ifndef __PndRadMapMESH_HH__
#define __PndRadMapMESH_HH__ 1
#include <TH2D.h>
#include <TROOT.h>
#include <TFormula.h>
#include <TMatrixD.h>
#include "FairRadMapPoint.h"


enum axis{Xx=1, Yy=2, Zz=3}; 
enum orientation{XY=1, YX=2, XZ=3, ZX=4, YZ=5, ZY=6}; 
enum quantity{Edep=1, Dose=2, Fluence=3, EnergyFluence=4, Flux=5, Kerma=6, Density=7, Mass=8, SimpleFluence=9}; 

struct Corner{
  TVector3 corner1;
  TVector3 corner2;
  TVector3 corner3;
};
void InvMatVecProd(TMatrixD mat, TVector3 vec, TVector3& res);


class PndRadMapBoxMesh{
 public:
  PndRadMapBoxMesh(){};
  PndRadMapBoxMesh(PndRadMapBoxMesh& m);//copy cons
  PndRadMapBoxMesh(const char* Name,
          int Xbins, Double_t Xlow, Double_t Xhigh,
          int Ybins, Double_t Ylow, Double_t Yhigh,
          int Zbins, Double_t Zlow, Double_t Zhigh);
  PndRadMapBoxMesh(const char* name,
          int xbins, Double_t xlow, Double_t xhigh,
          int ybins, Double_t ylow, Double_t yhigh,
          Double_t zlow, Double_t zhigh,
          orientation plane = ZX,
          quantity Quantity=Edep);
  ~PndRadMapBoxMesh();


  void SetFilter(const char* filter);
  void SetQuantity(quantity Quantity=Edep);
  void SetOrientation(orientation plane, 
                      Double_t rotate = 99999,
                      axis Ax = Xx);
  void SetOrientation(Double_t rotate = 99999,
                      axis Ax = Xx);
  void Fill(FairRadMapPoint *p);
  void Transform(Double_t X, Double_t Y, Double_t Z);
  void Transform(Double_t X, Double_t Y, Double_t Z,
                 Double_t& X0, Double_t& Y0, Double_t& Z0);

  void Scale(Double_t sca);
  void Save(TFile* fout);
  void Save();
  TH2D* GetHisto();
  Double_t CalcFluence(FairRadMapPoint *p);

 protected:
  bool IsInside(Double_t X, Double_t Y, Double_t Z);
  bool IsInside(FairRadMapPoint *p);
  void Fill(Int_t gBin, Double_t val);
  void Fill(Double_t X, Double_t Y, Double_t Z, Double_t we);
  void makeHisto(const char* Orient, Double_t rotate,
                 int Hbins, Double_t Hlow, Double_t Hhigh,
                 int Vbins, Double_t Vlow, Double_t Vhigh);

  TH2D*    _MeshHisto;
  TH2I*    _StatHisto;
  TString  _Name;

  orientation _orientation;
  Double_t _rotate;
  axis     _axis;
  Double_t _volume;
  quantity _quantity;

  int      _Xbins;
  Double_t _Xlow, _Xhigh;
  int      _Ybins;
  Double_t _Ylow, _Yhigh;
  int      _Zbins;
  Double_t _Zlow, _Zhigh;

  Double_t _X, _Y, _Z;
  Double_t _tX, _tY, _tZ;//back-shifted, back-rotated coordinate

  /* UInt_t    oldId; */
  /* Double_t _Xold, _Yold, _Zold; */
  TFormula _filter;
  //ClassDef(PndRadMapBoxMesh,1); // Class for PndSds
};


/* class BoxMesh : public Mesh{   */

/*  public: */
/*   Mesh(int Xbins, Double_t Xlow, Double_t Xhigh, */
/*        int Ybins, Double_t Ylow, Double_t Yhigh, */
/*        int Zbins, Double_t Zlow, Double_t Zhigh); */
/*   //default values World mins, and maxs */
/*   void SetOrientation(const char* plane, Double_t shift, Double_t rotate); */
/*   //XY, YX */
/*   //XZ, ZX */
/*   //YZ, ZY */
/*   //shift along the 'first letter axis' of the 'plane' (at "XY", shift along the X axis) */
/*   //rotate along th 'second letter axis' */
/*   virtual bool IsInside(Double_t X, Double_t Y, Double_t Z); */
/*   virtual TVector3 Which(Double_t X, Double_t Y, Double_t Z); */
/*   virtual Double_t Volume(Double_t X, Double_t Y, Double_t Z); */
/*   virtual Double_t Volume(Double_t R = 0);//the volume does not depend on the position */

/*  private: */
/*   emum orientation{XY, YX, XZ, ZX, YZ, XY}; */
/*   orientation Orient; */

/* }; */

/* class CylinderMesh : public Mesh{  //(where is the 0,0,0 point?) */
/*   Mesh(int Zbins, Double_t Zlow, Double_t Zhigh, */
/*        int Rbins, Double_t Rlow, Double_t Rhigh,//sqrt(xx+yy) */
/*        int Phibins, Double_t Philow, Double_t Phihigh);//going from 0 to 2pi */
/*   virtual bool IsInside(Double_t R, Double_t Theta, Double_t Phi); */
/*   virtual TVector3 Which(Double_t R, Double_t Theta, Double_t Phi); */
/*   virtual Double_t Volume(Double_t R, Double_t Theta, Double_t Phi); */
/*   virtual Double_t Volume(Double_t R);//the volume depends on the R only */
/*   //default values World mins, and maxs */


/*   //RPhi, Phi */
/*   //XZ, ZX */
/*   //YZ, ZY */


/* }; */

/* class CSpericalMesh : public Mesh{    //(where is the 0,0,0 point?) */
/*   Mesh(int Rbins, Double_t Rlow, Double_t Rhigh,//sqrt(xx+yy+zz) */
/*        int Thetabins, Double_t Thetalow, Double_t Thetahigh,//from 0 to 2pi */
/*        int Phibins, Double_t Philow, Double_t Phihigh);//going from 0 to 2pi */
/*   //default values World mins, and maxs */
/* }; */

#endif
