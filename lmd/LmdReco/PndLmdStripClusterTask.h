// -------------------------------------------------------------------------
// -----                 PndLmdStripClusterTask header file             -----
// -------------------------------------------------------------------------
//Notes
/*Notes for update on Jun30. 2010 by h.xu@fz-juelich.de
 * Reload the function of SetCalculators() and SetClusterType()
 */


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
  virtual void SetClusterType() {fClusterType = kMVDClusterStrip;}
  inline void SetyRotation(const Double_t inrot){fyRotation=inrot;};
  
  virtual void SetCalculators();
//  Bool_t Backmap( TVector2 meantopPoint, Double_t toperr , TVector2 meanbotPoint, Double_t boterr, TVector3 &hitpos, TVector3 &hiterr, TString &detname);

  ClassDef(PndLmdStripClusterTask,2);

private:

  Double_t fyRotation; //angle [rad] for y-rotated detector geometry

};

#endif /* LMDCLUSTERTASK_H */

