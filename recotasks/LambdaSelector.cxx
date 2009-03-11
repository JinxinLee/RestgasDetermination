//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class LambdaSelector
//      see LambdaSelector.hh for details
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
#include "LambdaSelector.h"

// C/C++ Headers ----------------------
#include <vector>
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "Track.h"
#include "TrackCand.h"
#include "AbsTrackRep.h"
#include "trackProximity.h"

// Class Member definitions -----------



LambdaSelector::LambdaSelector()
  : FairTask("LambdaSelector"), _persistence(kFALSE), _trackBranchName("TrackPreFit")
{
}

LambdaSelector::~LambdaSelector()
{}


InitStatus
LambdaSelector::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("LambdaSelector::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  
  if(_trackArray==0)
    {
      Error("LambdaSelector::Init","Track-array not found!");
      return kERROR;
    }

  _vertexArray=new TClonesArray("TVector3");
  ioman->Register("LambdaVertex","LambdaSelector",_vertexArray,_persistence);

  _lambdaArray=new TClonesArray("TLorentzVector");
  ioman->Register("LambdaMom","LambdaSelector",_lambdaArray,_persistence);

  return kSUCCESS;
}

void
LambdaSelector::Exec(Option_t* opt)
{
  std::cout << "LambdaSelector::Exec" << std::endl;

  // Reset output Arrays
  if(_vertexArray==0) Fatal("LambdaSelector::Exec)","No vertexArray");
  _vertexArray->Delete();
  if(_lambdaArray==0) Fatal("LambdaSelector::Exec)","No lambdaArray");
  _lambdaArray->Delete();



  int ntrks=_trackArray->GetEntriesFast();
  if(ntrks<2 || ntrks>10) return;
  
  // build charged particle list
  std::vector<Track*> protoncands;
  std::vector<Track*> pioncands;

  for(int itrk=0;itrk<ntrks;++itrk){
    Track* trk=(Track*)_trackArray->At(itrk);
    if(trk->getMom().Mag()<0.1) continue;
    if(trk->getCharge()>0)protoncands.push_back(trk);
    else pioncands.push_back(trk);
  }
  
  std::cout<<protoncands.size()<<" proton candidates"<<std::endl;
  std::cout<<pioncands.size()<<" pion candidates"<<std::endl;

  // combinatorics
  unsigned int nprotons=protoncands.size();
  unsigned int npions=pioncands.size();
  for(unsigned int ip=0;ip<nprotons;++ip){
    Track* protontrk=protoncands[ip];
    for(unsigned int ipi=0;ipi<npions;++ipi){
      Track* piontrk=pioncands[ipi];
      
      // find point of closest approach
      trackProximity(protontrk,piontrk);

      std::cout<<"Vertex at:"<<std::endl;
      Int_t size=_vertexArray->GetEntriesFast();
      TVector3* v=new((*_vertexArray)[size]) TVector3(0.5*(protontrk->getPos()+piontrk->getPos()));
      v->Print();

      // build 4-vectors
      TVector3 p3=protontrk->getMom();
      TLorentzVector proton;
      proton.SetXYZM(p3.X(),p3.Y(),p3.Z(),0.93827203);
      
      p3=piontrk->getMom();
      TLorentzVector pion;
      pion.SetXYZM(p3.X(),p3.Y(),p3.Z(),0.13957018);

      Int_t size2=_lambdaArray->GetEntriesFast();
      new((*_lambdaArray)[size]) TLorentzVector(proton+pion);
      
      // todo set daughter tracks!
    }
  }
}


ClassImp(LambdaSelector)
