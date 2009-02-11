//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class V0Selector
//      see V0Selector.hh for details
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
#include "V0Selector.h"

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
#include "ExtrapolateToPCA.h"

// Class Member definitions -----------



V0Selector::V0Selector()
  : FairTask("V0Selector"), _persistence(kFALSE), _trackBranchName("TrackPreFit"), _V0BranchName("V0"), _mass_pos(511.E-6), _mass_neg(511.E-6)
{
}

V0Selector::~V0Selector()
{}


InitStatus
V0Selector::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("V0Selector::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  
  if(_trackArray==0)
    {
      Error("V0Selector::Init","Track-array not found!");
      return kERROR;
    }

  _vertexArray=new TClonesArray("TVector3");
  ioman->Register(_V0BranchName+"Vertex","V0Selector",_vertexArray,_persistence);

  _V0Array=new TClonesArray("TLorentzVector");
  ioman->Register(_V0BranchName+"Mom","V0Selector",_V0Array,_persistence);

  return kSUCCESS;
}

void
V0Selector::Exec(Option_t* opt)
{
  std::cout << "V0Selector::Exec" << std::endl;

  // Reset output Arrays
  if(_vertexArray==0) Fatal("V0Selector::Exec)","No vertexArray");
  _vertexArray->Delete();
  if(_V0Array==0) Fatal("V0Selector::Exec)","No V0Array");
  _V0Array->Delete();



  int ntrks=_trackArray->GetEntriesFast();
  if(ntrks<2 || ntrks>10) return;
  
  // build charged particle list
  std::vector<Track*> poscands;
  std::vector<Track*> negcands;

  for(int itrk=0;itrk<ntrks;++itrk){
    Track* trk=(Track*)_trackArray->At(itrk);
    //todo: nake this dynamic!
    if(trk->getMom().Mag()<0.1) continue;
    if(trk->getCharge()>0)poscands.push_back(trk);
    else negcands.push_back(trk);
  }
  
  std::cout<<poscands.size()<<" pos candidates"<<std::endl;
  std::cout<<negcands.size()<<" neg candidates"<<std::endl;

  // combinatorics
  unsigned int nposs=poscands.size();
  unsigned int nnegs=negcands.size();
  for(unsigned int ip=0;ip<nposs;++ip){
    
    for(unsigned int ipi=0;ipi<nnegs;++ipi){
      Track* postrk=new Track(*poscands[ip]);
      Track* negtrk=new Track(*negcands[ipi]);
      

      // copy tracks! fo not change its parameters
      // find point of closest approach
      ExtrapolateToPCA(postrk,negtrk);

      std::cout<<"Vertex at:"<<std::endl;
      Int_t size=_vertexArray->GetEntriesFast();
      TVector3* v=new((*_vertexArray)[size]) TVector3(0.5*(postrk->getPos()+negtrk->getPos()));
      v->Print();

      // build 4-vectors
      TVector3 p3=postrk->getMom();
      TLorentzVector pos;
      pos.SetXYZM(p3.X(),p3.Y(),p3.Z(),_mass_pos);
      
      p3=negtrk->getMom();
      TLorentzVector neg;
      neg.SetXYZM(p3.X(),p3.Y(),p3.Z(),_mass_neg);

      Int_t size2=_V0Array->GetEntriesFast();
      new((*_V0Array)[size]) TLorentzVector(pos+neg);
      
      // todo set daughter tracks!
      delete postrk; delete negtrk;
    }
  }
}


ClassImp(V0Selector)
