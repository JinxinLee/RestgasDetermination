// -------------------------------------------------------------------------
// -----                 PNDMCTESTEMCHITCOMPARE header file             -----
// -----                  Created 20/04/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCTESTEMCHITCOMPARE.h
 *@author B.Spruck <bjoern.spruck@exp2.physik.uni-giessen.de>
 **
 ** Displays EMC Cluster information comaring to MC info
 **/


#ifndef PNDMCTESTEMCHITCOMPARE_H
#define PNDMCTESTEMCHITCOMPARE_H


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"
#include "PndDetectorList.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCTestEmcClusterCompare : public FairTask
{
 public:

  /** Default constructor **/
	PndMCTestEmcClusterCompare();

  /** Destructor **/
  virtual ~PndMCTestEmcClusterCompare();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:
  PndMCMatch* fMCMatch;
  TClonesArray* fEmcCluster;
  TClonesArray* fMCTrack;

  void Register();

  void Reset();

  void ProduceHits();

  void cleansortmclist( std::vector <Int_t> &newlist,TClonesArray* mcTrackArray);

  ClassDef(PndMCTestEmcClusterCompare,1);

};

#endif
