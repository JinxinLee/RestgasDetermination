/******************************************************
Converts the MCTracks to a TCA holding TCandidates
*******************************************************/ 

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMcListConverter.h"
#include <string>
#include <iostream>
#include "PndMCTrack.h"

//Root stuff
#include "TClonesArray.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

//RHO stuff
#include "RhoBase/TCandidate.h"
#include "RhoBase/TRho.h"
		
		
using std::cout;
using std::endl;

		
// -----   Default constructor   -------------------------------------------
PndMcListConverter::PndMcListConverter() :
  FairTask("Panda MCTracks to TCandidates Converter") { 
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMcListConverter::~PndMcListConverter() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndMcListConverter::Init() {
 
  //cout << " Inside the Init function****" << endl;
  
  //FairDetector::Initialize();
  //FairRun* sim = FairRun::Instance();
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndMcListConverter::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
 // Get input array
  fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  
  
  if ( !fMcTracks ) {
    cout << "-W- PndMcListConverter::Init: "
	 << "No MCTracks array!" << endl;
    return kERROR;
  }

  fMcCandidates =new TClonesArray("TCandidate");
  ioman->Register("PndMcTracks","PndMcTracks", fMcCandidates, kTRUE);
  
  // Create and register output array
  cout << "-I- PndMcListConverter: Intialization successfull" << endl;
 
  fPdg = TRho::Instance()->GetPDG();
 
  return kSUCCESS;

}

void PndMcListConverter::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");

 
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMcListConverter::Exec(Option_t* opt) {

  if (fMcCandidates->GetEntriesFast() != 0)  fMcCandidates->Clear("C");
  
  if(fVerbose) cout << "-I- Start PndMcListConverter. "<<endl;
  
  if(fVerbose) cout <<"-I- PndMcListConverter: nMcTracks="<<fMcTracks->GetEntriesFast()<<endl;

  // Get the Candidates
  for(Int_t i=0; i<fMcTracks->GetEntriesFast(); i++)
  {
  	PndMCTrack *part = (PndMCTrack*)fMcTracks->At(i);
  	if (part->GetMotherID()!=-1) continue;
  	
    TLorentzVector p4 = part->Get4Momentum();
    TVector3    stvtx = part->GetStartVertex();
    
    TParticlePDG *ppdg = fPdg->GetParticle(part->GetPdgCode());
    
    double charge=0.0;
    
    if (ppdg) charge=ppdg->Charge();
    else if (fVerbose) cout <<"-W- PndMcListConverter: strange PDG code:"<<part->GetPdgCode()<<endl;
    if (fabs(charge)>2) charge/=3.;
    
    TClonesArray& ref = *fMcCandidates;
    Int_t size = ref.GetEntriesFast();
    
   	TCandidate *pmc=new (ref[size]) TCandidate(p4,charge);
   	
    pmc->SetMcIdx(size);
    pmc->SetPos(stvtx);
    pmc->SetType(part->GetPdgCode());
   }

  if(fVerbose) cout <<"-I- PndMcListConverter: found primaries="<<fMcCandidates->GetEntriesFast()<<endl;

}
// -------------------------------------------------------------------------

void PndMcListConverter::Finish()
{
  delete fMcCandidates;
}

ClassImp(PndMcListConverter)
