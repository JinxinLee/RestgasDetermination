#ifndef PNDJPSIEXAMPLE_H
#define PNDJPSIEXAMPLE_H 1


#include "FairTask.h"
#include "TH1F.h"
#include "TFile.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;


class PndJPsiExample : public FairTask
{

 public:
	typedef std::map<Int_t, Float_t> mapper;

  /** Default constructor **/  
  PndJPsiExample();


  /** Destructor **/
  ~PndJPsiExample();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  //void CreateStructure();

 protected:

  TLorentzVector track1; 
  TLorentzVector track2; 

  /**book all the histograms**/
  TH1D *fInvMass;

 private: 
   
  /** Input array of TpcLheTrack **/
  TClonesArray* fTrArray;

 
  /** Geo file to use **/

	/** Get parameter containers **/
	virtual void SetParContainers();

 
  ClassDef(PndJPsiExample,1);
  
};

#endif
