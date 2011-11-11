//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Class containing CDC Cirle 
//      
//
// Environment:
//      Software developed for the Prototype Detector at FOPI
//
// Author List:
//      Robert Münzer          TUM
//
//-----------------------------------------------------------

#ifndef CDCCIRCLE_H
#define CDCCRICLE_H

#include <cmath>
#include "TVector3.h"
#include <TObject.h>

// Collaborating Class Declarations --------------------------------------

class CdcCircle : public TObject {

 public:

  // Constructor ----------------------------------------------

  CdcCircle();			//default constructor
  //  ~CdcCircle()
  
  // Setters --------------------------------------------------
  void SetRXY(Float_t rad,Float_t midx,Float_t midy);
  void SetRXYP(Float_t rad,Float_t midx,Float_t midy,Float_t phi);
  void SetRadius(Float_t rad) { fradius=fabs(rad); }
  void SetMidx(Float_t midx) { fmidx=midx; }
  void SetMidy(Float_t midy) { fmidy=midy; }
  void SetPhi(Float_t phi) { fphi=phi; }
  
  // Accesores
  double GetRadius() { return fradius; }
  double GetMidx() { return fmidx; }
  double GetMidy() { return fmidy; }
  double GetPhi() { return fphi; }
  double GetTpcLength();
  double MatPoint(TVector3 point,Float_t maxdist,Float_t maxphi);

 private:
  Float_t fradius;
  Float_t fmidx;
  Float_t fmidy;
  Float_t fphi;
  Float_t ftpctracklength;
  Float_t fmidxtpc;
  Float_t fmidytpc;
  Float_t fradiustpcinner;
  Float_t fradiustpcouter;
  


public:
  ClassDef(CdcCircle,1)  

};

#endif
