// -------------------------------------------------------------------------
// -----                 PndLmdStripClusterTask header file             -----
// -------------------------------------------------------------------------



#ifndef PNDLMDSTRIPCLUSTERTASK_H
#define PNDLMDSTRIPCLUSTERTASK_H

#include "PndSdsStripClusterTask.h"

class PndLmdStripClusterTask : public PndSdsStripClusterTask
{
 public:

    /** Default constructor **/

    PndLmdStripClusterTask();


    /** Destructor **/
    virtual ~PndLmdStripClusterTask();

  virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
  virtual void SetBranchNames();
  virtual void SetParContainers();
  virtual void SetCalculators();
  void SetClusterType(){fClusterType = kMVDClusterStrip;}
  TVector3 AddMSErr(TVector3 hit,TVector3 hiterr);
  Bool_t Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr,
		  TVector3 &hitPos, TMatrixD &hitCov, Int_t &sensorID);
  // inline void SetyRotation(const Double_t inrot=0){fyRotation=inrot;};

  //  void Exec(Option_t* opt);
  ClassDef(PndLmdStripClusterTask,2);
  
 private:
  
  // Double_t fyRotation;

};

#endif /* LMDCLUSTERTASK_H */

