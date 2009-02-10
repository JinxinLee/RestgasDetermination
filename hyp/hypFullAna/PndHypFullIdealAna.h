#ifndef PNDHYPFULLIDEALANA_H
#define PNDHYPFULLIDEALANA_H 1


#include "CbmTask.h"
#include <map>
#include <string>
#include "TFile.h"
#include "TVector3.h"
#include "TLorentzVector.h"



class TClonesArray;
class TObjectArray;
class TH1F;class TH2F;

class TPidChargedSelector;
class TPidNeutralSelector;
class TPidPlusSelector;
class TPidMinusSelector;
class TPidMassSelector;
class TPidSimpleKaonSelector;
class TPidSimplePionSelector;
class TPidSimpleProtonSelector;

class PndHypFullIdealAna : public CbmTask
{

 public:
	typedef std::map<Int_t, Float_t> mapper;

  /** Default constructor **/  
  PndHypFullIdealAna();


  /** Destructor **/
  ~PndHypFullIdealAna();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void Finish(TString cat);

  void SetEnergySpectra(int event,int cluster);
  void SetTotESpectra(int clus);
  Int_t GetIonCharge(Int_t Z,Int_t &mass,Int_t &str);
  
  //void CreateStructure();

 protected:

  int evcount;

  

  // **** some basic selectors
  //
  //TPidChargedSelector *chargedSel;
  TPidNeutralSelector *neutralSel;
  TPidPlusSelector    *plusSel;
  TPidMinusSelector   *minusSel;
  
  // **** mass selectors for the resonances/composites
  //
  TPidMassSelector *phiMSel;
  TPidMassSelector *pi0MSel;
  TPidMassSelector *dsMSel;
  TPidMassSelector *LambMSel;
  TPidSimpleKaonSelector *kSel;
  TPidSimplePionSelector *piSel;
  TPidSimpleProtonSelector *pSel;
  
  /**book all the histograms**/
  TH2F *hvtx2[10];
  TH1F *spectra[10];
   
  TH1F *ds0mass;
 TH1F *ximass; TH1F *Lamb;
  TH1F *ppi2mass;
  TH1F *ppi2; TH1F *e;
  TH2F *pid;TH2F *pidh;

  TH1F *nmult;

 private: 
   
  /** Input array of TpcLheTrack **/
  
  TClonesArray* fChargedArray;
  TClonesArray* fMcTr;
  TClonesArray* fMicroArray;
  TClonesArray* fMcCands; 
  TClonesArray* fMc;
  TClonesArray* fGe;
 
  /** Geo file to use **/

	/** Get parameter containers **/
	virtual void SetParContainers();

 
  ClassDef(PndHypFullIdealAna,1);
  
};

#endif
