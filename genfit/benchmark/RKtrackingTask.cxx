
// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TPolyMarker3D.h"
#include "RKtrackingTask.h"


#include "PndGemMCPoint.h"
#include "PndMvdMCPoint.h"
#include "PndDchPoint.h"

#include"assert.h"

// -----   Default constructor   -------------------------------------------
RKtrackingTask::RKtrackingTask() :
  FairTask("RK tracking"),field(NULL) { 
}

// -----   Constructor   ---------------------------------------------------
RKtrackingTask::RKtrackingTask(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose),field(NULL) { 
}


// -----   Destructor   ----------------------------------------------------
RKtrackingTask::~RKtrackingTask() {
}

// -----   Private method SetParContainers   -------------------------------
void RKtrackingTask::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus RKtrackingTask::Init() {
  gRandom->SetSeed(4);

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    std::cout << "-E- RKtrackingTask::Init: "
	      << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Get input array
  fMvdPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
  if( !fMvdPointArray ) {
    std::cout << "-W- RKtrackingTask::Init: "
	      << "Array of MVDPoints not found!" << std::endl;
    return kERROR;
  }
  fGemPointArray = (TClonesArray*) ioman->GetObject("GEMPoint");
  if( !fGemPointArray ) {
    std::cout << "-W- RKtrackingTask::Init: "
	      << "Array of GEMPoints not found!" << std::endl;
    return kERROR;
  }
  fDchPointArray = (TClonesArray*) ioman->GetObject("PndDchPoint");
  if( !fDchPointArray ) {
    std::cout << "-W- RKtrackingTask::Init: "
	      << "Array of PndDchPoints not found!" << std::endl;
    return kERROR;
  }
  

  
  std::cout << "-I- RKtrackingTask: Intialization successfull" << std::endl;
  return kSUCCESS;

}

// -----   Public method Exec   --------------------------------------------
void RKtrackingTask::Exec(Option_t* opt) {

  assert(field!=NULL);
  std::vector<TVector3> points;

  for ( Int_t iPoint = 0 ; iPoint < fMvdPointArray->GetEntriesFast() ; iPoint++ ) {
    FairMCPoint* point = (PndMvdMCPoint*)fMvdPointArray->At(iPoint);
    TVector3 pos(point->GetX(),point->GetY(),point->GetZ());
    points.push_back(pos);
  }  // end of loop over Points
  for ( Int_t iPoint = 0 ; iPoint < fGemPointArray->GetEntriesFast() ; iPoint++ ) {
    FairMCPoint* point = (PndGemMCPoint*)fGemPointArray->At(iPoint);
    TVector3 pos(point->GetX(),point->GetY(),point->GetZ());
    points.push_back(pos);
  }  // end of loop over Points
  for ( Int_t iPoint = 0 ; iPoint < fDchPointArray->GetEntriesFast() ; iPoint++ ) {
    FairMCPoint* point = (PndDchPoint*)fDchPointArray->At(iPoint);
    TVector3 pos(point->GetX(),point->GetY(),point->GetZ());
    points.push_back(pos);
  }  // end of loop over Points

  TPolyMarker3D *drawpoints = new TPolyMarker3D(points.size(),20);
  for(int i=0;i<points.size();++i){
    drawpoints->SetPoint(i,points.at(i).X(),points.at(i).Y(),points.at(i).Z());
  }
  TCanvas *c1 = new TCanvas("c1");
  drawpoints->Draw();
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();


}

// -----   Private method Finish   -----------------------------------------
void RKtrackingTask::Finish() {
}

ClassImp(RKtrackingTask)
