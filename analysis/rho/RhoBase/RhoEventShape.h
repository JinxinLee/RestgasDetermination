// Definition of the RhoEventShape class
// Author: Jens Brose, TUD, Aug. 1999
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring) 
//--------------------------------------------------------------------------

#ifndef RHOEVENTSHAPE_H
#define RHOEVENTSHAPE_H

#include "TObject.h"
#include "TLorentzVector.h"
#include "RhoCandidate.h"
#include "RhoCandList.h"

class RhoEventShape : public TObject
{
  public:

    RhoEventShape();
    RhoEventShape( RhoCandList& l );
    virtual ~RhoEventShape();

    inline TLorentzVector P4() { return fTot4Mom;}
    //inline Double_t FoxWolfram() { return _fw2; }
    inline Double_t M() { return fTotM;}
    inline Double_t E() { return fTotE;}
    inline Double_t P() { return fTotP;}
    inline Double_t Pt() { return fTotPt;}
    inline Double_t Psum() { return fTotAbsMom;}

  private :

    TLorentzVector fTot4Mom;
    //Double_t _fw2;
    Double_t fTotM;
    Double_t fTotE;
    Double_t fTotP;
    Double_t fTotPt;
    Double_t fTotAbsMom;

  public:
    ClassDef(RhoEventShape,1) //Calculate event shape variables
};

#endif
