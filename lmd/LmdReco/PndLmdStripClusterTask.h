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
  // inline void SetyRotation(const Double_t inrot=0){fyRotation=inrot;};

  //  void Exec(Option_t* opt);
  // Bool_t Backmap( TVector2 meantopPoint, Double_t toperr , TVector2 meanbotPoint, Double_t boterr, TVector3 &hitpos, TVector3 &hiterr, TString &detname);

  ClassDef(PndLmdStripClusterTask,2);

private:

  // Double_t fyRotation;

};

#endif /* LMDCLUSTERTASK_H */

