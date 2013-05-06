#ifndef PNDANALYSISTASKEXAMPLE_H
#define PNDANALYSISTASKEXAMPLE_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;

class RhoChargedParticleSelector;
class RhoNeutralParticleSelector;
class RhoPlusParticleSelector;
class RhoMinusParticleSelector;
class RhoMassParticleSelector;
class RhoSimpleKaonSelector;
class RhoSimplePionSelector;

class PndAnalysisTaskExample : public FairTask
{

 public:
	typedef std::map<Int_t, Float_t> mapper;

  /** Default constructor **/  
  PndAnalysisTaskExample();


  /** Destructor **/
  ~PndAnalysisTaskExample();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  //void CreateStructure();

 protected:

  int evcount;

  

  // **** some basic selectors
  //
  //RhoChargedParticleSelector *chargedSel;
  RhoNeutralParticleSelector *neutralSel;
  RhoPlusParticleSelector    *plusSel;
  RhoMinusParticleSelector   *minusSel;
  
  // **** mass selectors for the resonances/composites
  //
  RhoMassParticleSelector *phiMSel;
  RhoMassParticleSelector *pi0MSel;
  RhoMassParticleSelector *dsMSel;
  RhoSimpleKaonSelector *kSel;
  RhoSimplePionSelector *piSel;

  /**book all the histograms**/
  TH1F *phimass;
  TH1F *pi0mass;
  TH1F *dsmass; 
  TH1F *ds0mass;
  TH1F *ppmass;

  TH1F *nmult;

 private: 
   
  /** Input array of LheTrack **/
  TClonesArray* fChargedArray;
  TClonesArray* fNeutralArray;
 
 
  /** Geo file to use **/

	/** Get parameter containers **/
	virtual void SetParContainers();

 
  ClassDef(PndAnalysisTaskExample,1);
  
};

#endif
