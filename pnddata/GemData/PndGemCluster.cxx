//* $Id: 

// -------------------------------------------------------------------------
// -----                      PndGemCluster source file                   -----
// -------------------------------------------------------------------------
#include "PndGemCluster.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndGemCluster::PndGemCluster() : 
  FairTimeStamp(),
  fDetectorId    (0),
  fChannelNr     (0),
  fClusterBeg    (0),
  fClusterEnd    (0),
  fClusterCharge (0.),
  fClusterCor    (0.0)
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
PndGemCluster::PndGemCluster(Int_t iDetectorId, Double_t iChannel, Int_t bChannel, Int_t eChannel, std::vector<Int_t> index) :
  FairTimeStamp(), 
  fDetectorId    (iDetectorId),
  fChannelNr     (iChannel),
  fClusterBeg    (bChannel),
  fClusterEnd    (eChannel),
  fClusterCharge (0.),
  fClusterCor    (0.0)
{
  AddIndex(index);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
PndGemCluster::PndGemCluster(Int_t iDetectorId, Double_t iChannel, Int_t bChannel, Int_t eChannel, Double_t signal, Double_t time, std::vector<Int_t> index) :
  FairTimeStamp(time), 
  fDetectorId    (iDetectorId),
  fChannelNr     (iChannel),
  fClusterBeg    (bChannel),
  fClusterEnd    (eChannel),
  fClusterCharge (signal),
  fClusterCor    (0.0)
{
  AddIndex(index);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemCluster::~PndGemCluster() { }
// -------------------------------------------------------------------------



ClassImp(PndGemCluster)
