#ifndef PNDJPSI2PIANA_H
#define PNDJPSI2PIANA_H 1


#include "PndAnaTask.h"
#include <map>
#include <string>

class TH1F;

class RhoMassParticleSelector;

class PndJpsi2PiAna : public PndAnaTask
{

 public:
  /** Default constructor **/  
  PndJpsi2PiAna();

  /** Destructor **/
  ~PndJpsi2PiAna();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  //void CreateStructure();

 protected:
  
  // **** mass selectors for the resonances/composites
  //
  RhoMassParticleSelector *jpsiMSel;

  /**book all the histograms**/
  TH1F *jpsimass;
  TH1F *jpsimassf;
  TH1F *ppmass;
  TH1F *ppmassf;
  TH1F *mcmass;
  
 private:
   UInt_t fEvtCount;

 
  ClassDef(PndJpsi2PiAna,1);
  
};

#endif
