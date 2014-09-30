// --------------------------------------------------------
// ----			PndLmdDigiQ header file  	---
// ----			Created 19.09.14 by A.Karavdina ---
// --------------------------------------------------------

/** PndLmdDigiQ.h
 *@author A.Karavdina <karavdin@kph.uni-mainz.de>
 **
 ** Add signal/background flag to PndSdsDigiPixel
 **/


#ifndef PNDLMDDIGIQ_H
#define PNDLMDDIGIQ_H

#include "PndSdsDigiPixel.h"

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>


class PndLmdDigiQ : public PndSdsDigiPixel
  {
      
  public: 
    PndLmdDigiQ();
    PndLmdDigiQ(const PndSdsDigiPixel &digiIn, bool sigfl);
    void SetFlSig(){fSigfl = true;}
    void SetFlBkg(){fSigfl = false;}
    void SetPDG(int pdgcode){fPDG = pdgcode;}
    void SetPlane(int pl){fPl = pl;}
    void SetModule(int mod){fModule = mod;}
    void SetSide(int side){fSide = side;}
    void SetThMC(double th){fThMC = th;}
    void SetEvT(double t){fEvTime = t;}
    bool GetFlSig(){ return fSigfl;}
    int GetPDG(){ return fPDG;}
    int GetPlane(){return fPl;}
    int GetModule(){return fModule;}
    int GetSide(){return fSide;}
    double GetThMC(){return fThMC;}
    double GetEvT(){return fEvTime;}
    virtual ~PndLmdDigiQ(){};
    
    virtual void Print() {
      std::cout << *this;
    }
    
   protected: 
    bool fSigfl; //signal?
    int fPDG; //PDG Code of particle
    int fPl; //plane number
    int fModule; //module number
    int fSide; //side of plane
    double fThMC; //theta MC 
    double fEvTime; //event time
    ClassDef(PndLmdDigiQ,5);
};

#endif
