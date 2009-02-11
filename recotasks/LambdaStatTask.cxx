 //-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class LambdaStatTask
//      see LambdaStatTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "LambdaStatTask.h"

// C/C++ Headers ----------------------
#include <map>
#include <algorithm>
#include <string>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "TMath.h"
#include "TDatabasePDG.h"
#include "MCTruthAnnex.h"

LambdaStatTask::LambdaStatTask()
  : FairTask("LambdaStatistics"), _persistence(kFALSE)
{
  _lambdaBranchName = "LambdaMom";
  _mcBranchName = "MCTrack";
  _vertexBranchName = "LambdaVertex";
  _mcAnnexBranchName = "MCTruthAnnex";
}


LambdaStatTask::~LambdaStatTask()
{
}


InitStatus
LambdaStatTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("TrackReadTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _lambdaArray=(TClonesArray*) ioman->GetObject(_lambdaBranchName);
  
  if(_lambdaArray==0)
    {
      Error("LambdaStatTask::Init","lambda-array not found!");
      return kERROR;
    }


  _mcTrackArray=(TClonesArray*) ioman->GetObject(_mcBranchName);
  if(_mcTrackArray==0)
    {
      Error("LambdaStatTask::Init","MC-Truth not found!");
      return kERROR;
    }

  _mcAnnexArray=(TClonesArray*) ioman->GetObject(_mcAnnexBranchName);
  if(_mcAnnexArray==0)
    {
      Error("LambdaStatTask::Init","MC-Truth Annex not found!");
      return kERROR;
    }

  _vertexArray=(TClonesArray*) ioman->GetObject(_vertexBranchName);
  if(_vertexArray==0)
    {
      Error("LambdaStatTask::Init","vertex array not found!");
      return kERROR;
    }


  // Create output collection
  _mcvertexArray = new TClonesArray("TVector3");
  ioman->Register("mcLambdaVertex","LambdaSelector",_mcvertexArray,_persistence);

  //_efficiency=new TH1D("efficiency","tracking efficiency",110,0,1.1);


  return kSUCCESS;

}


void
LambdaStatTask::Exec(Option_t* opt)
{
  std::cout << "LambdaStatTask::Exec" << std::endl;

  // clear output
  if(_mcvertexArray==0) Fatal("LambdaStat::Exec)","No OutputArray");
  _mcvertexArray->Delete();

  // prepare MonteCarlo Truth info
  int nmctrks=_mcTrackArray->GetEntriesFast();
  // look for protons and pions with enough hits in tpc
  std::vector<PndMCTrack*> protons;
  std::vector<PndMCTrack*> pions;
  
  for(int i=0; i<nmctrks;++i){
    PndMCTrack* mctrk=(PndMCTrack*) _mcTrackArray->At(i);
    MCTruthAnnex* annex=(MCTruthAnnex*) _mcAnnexArray->At(i);
    if(annex->getPndTpcHits()<20)continue;
    if(mctrk->GetPdgCode()==2212)protons.push_back(mctrk);
    if(mctrk->GetPdgCode()==-211)pions.push_back(mctrk);
  }

  int nprotons=protons.size();
  int npions=pions.size();

  // look for reconstructable lambdas
  for(int ip=0;ip<nprotons;++ip){
    TVector3 vertex=protons[ip]->GetStartVertex();
    for(int ipi=0;ipi<npions;++ipi){
      if(vertex==pions[ip]->GetStartVertex()){
	Int_t size=_mcvertexArray->GetEntriesFast();
	new ((*_mcvertexArray)[size]) TVector3(vertex);
      }
    }
  }


  // loop over mc vertices and see if we found them:
  //int nrecolambdas=_lambdaArray->GetEntriesFast();
  




  return;
}


ClassImp(LambdaStatTask)
