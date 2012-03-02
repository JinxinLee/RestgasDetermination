//
//  PndAnaPidSelector.h
//  PANDAROOT
//
//  Created by Ralf Kliemt on 12/16/11.
//

#ifndef PNDANAPIDSELECTOR_H
#define PNDANAPIDSELECTOR_H

#include <iostream>
#include <float.h>
#include "TNamed.h"
#include "TString.h"
#include "RhoBase/VAbsPidSelector.h"

class TCandidate;
class VAbsMicroCandidate;
class PndAnaSelectorPar;

class PndAnaPidSelector : public VAbsPidSelector {
  
public:
  //Constructor
  PndAnaPidSelector(const char *name="PndAnaPidSelector", const char* type="", const char* paramid="Default");
  //Destructor
  virtual ~PndAnaPidSelector() {}
  
  //operations
  virtual Bool_t Accept(TCandidate& b);
  virtual Bool_t Accept(VAbsMicroCandidate& b);
  Bool_t SetCriterion(TString &crit){return SetSelection(crit);};
  Bool_t SetSelection(TString &crit);
  
protected:
  
  PndAnaSelectorPar* fSelectPar; //! Pointer to the current parameter object
  
  //PndAnaFluxPar* fFluxPar; //! Pointer to the current parameter object
  
  Double_t fChargeCrit;
  
  Int_t fPidSelect;
  
public:
  ClassDef(PndAnaPidSelector,1)  // Particle selector
};      



#endif

