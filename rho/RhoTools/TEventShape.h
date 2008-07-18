// Definition of the TEventShape class 
// Author: Jens Brose, TUD, Aug. 1999
//--------------------------------------------------------------------------

#ifndef TEVENTSHAPE_H
#define TEVENTSHAPE_H

#include "TObject.h"
#include "TLorentzVector.h"

class TCandList;

class TEventShape : public TObject {
public:
    
    TEventShape();
    TEventShape( TCandList &l );
    virtual ~TEventShape();
    
    inline TLorentzVector P4() { return _tot4mom;}
    inline Double_t FoxWolfram() { return _fw2; }
    inline Double_t M() { return _totm;}
    inline Double_t E() { return _totE;}
    inline Double_t P() { return _totp;}
    inline Double_t Pt() { return _totpt;}
    inline Double_t Psum() { return _totabsmom;}
    
private :
    
    TLorentzVector _tot4mom;
    Double_t _fw2;
    Double_t _totm;
    Double_t _totE;
    Double_t _totp;
    Double_t _totpt;
    Double_t _totabsmom;
    
public:
    ClassDef(TEventShape,1) //Calculate event shape variables
};

#endif

