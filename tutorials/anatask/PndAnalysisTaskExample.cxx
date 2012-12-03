/******************************************************

JPsi->e+e-
Reads the TPC tracks and reconstruct the InvariantMass 
of J/Psi: Dipak
*******************************************************/ 

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
//#include "PndLheCandidate.h"

#include "TVector3.h"
#include "TH1F.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndAnalysisTaskExample.h"
#include <string>
#include <iostream>

//RHO stuff
#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoSelector/TPidSelector.h"
#include "RhoBase/TFactory.h"
		
		
using std::cout;
using std::endl;

		
// -----   Default constructor   -------------------------------------------
PndAnalysisTaskExample::PndAnalysisTaskExample() :
  FairTask("Panda Analysis Task") { 
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndAnalysisTaskExample::~PndAnalysisTaskExample() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndAnalysisTaskExample::Init() {
 
  //cout << " Inside the Init function****" << endl;
  
  //FairDetector::Initialize();
  //FairRun* sim = FairRun::Instance();
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndEmcHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
 // Get input array
  fChargedArray = (TClonesArray*) ioman->GetObject("PndChargedCandidates");
  fNeutralArray = (TClonesArray*) ioman->GetObject("PndNeutralCandidates");
  
  if ( !fChargedArray && !fNeutralArray) {
    cout << "-W- PndAnalysisTaskExample::Init: "
	 << "No PndChargedCandidates && PndNeutralCandidates array!" << endl;
    return kERROR;
  }

  // Create and register output array
  cout << "-I- PndAnalysisTaskExample: Intialization successfull" << endl;

  phimass = new TH1F("phimass","phi cands",100,0.95,1.1);
  pi0mass = new TH1F("pi0mass","pi0 cands",100,0.135-0.03,0.135+0.03);
  dsmass  = new TH1F("dsmass","Ds cands",100,1.968-0.03,1.968+0.03); 
  ds0mass = new TH1F("ds0mass","Ds0 cands",100,2.317-0.05,2.317+0.05);
  ppmass = new TH1F("ppmass","pbarp cands",100,4.306-0.1,4.306+0.1);

  nmult=new TH1F("nmult","# neutrals",15,0,15);
  
  phimass->SetMinimum(0);
  pi0mass->SetMinimum(0);
  dsmass->SetMinimum(0);
  ds0mass->SetMinimum(0);
  ppmass->SetMinimum(0);
  
  // **** create and configure the selectors/filters we'd like to use later
  //
  //chargedSel = new TPidChargedSelector;
  neutralSel = new TPidNeutralSelector;
  plusSel    = new TPidPlusSelector;
  minusSel   = new TPidMinusSelector;
  
  // **** mass selectors for the resonances/composites
  //
  phiMSel       = new TPidMassSelector("phiSelector" , 1.0195 , 0.01);
  pi0MSel       = new TPidMassSelector("pi0Selector" , 0.135  , 0.005);
  dsMSel        = new TPidMassSelector("dsSelector"  , 1.9685 , 0.01);

  kSel    = new TPidSimpleKaonSelector();
  kSel->SetCriterion("loose");
  piSel   = new TPidSimplePionSelector();
  piSel->SetCriterion("veryLoose");
 
  evcount=0;
 
  return kSUCCESS;

}

void PndAnalysisTaskExample::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");

 
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndAnalysisTaskExample::Exec(Option_t* opt) {

  TFactory::Instance()->Reset();
  
  if (!(++evcount%100)) cout <<"evt "<<evcount<<endl;
  
  // **** create all the particle lists we'll need for rebuilding the decay tree
  //
  TCandList neutralCands,chargedCands, plusCands,minusCands;

  TCandList kpCands,kmCands,piCands;

  TCandList phiCands,pi0Cands,dsCands,ds0Cands,ppCands;


  TCandidate *tc;
  
  // **** loop over all Candidates and add them to the list allCands
  //    
  chargedCands.Cleanup();
  neutralCands.Cleanup();
  
  for (Int_t i1=0; i1<fChargedArray->GetEntriesFast(); i1++){
    tc = (TCandidate *)fChargedArray->At(i1);
    chargedCands.Add(*tc);
  }
  
  for (Int_t i1=0; i1<fNeutralArray->GetEntriesFast(); i1++){
    tc = (TCandidate *)fNeutralArray->At(i1);
    neutralCands.Add(*tc);
  }
  
  //cout <<"c:"<<chargedCands.GetLength()<<" n:"<<neutralCands.GetLength()<<endl;
    

  // **** select all the basic lists
  //
  nmult->Fill(neutralCands.GetLength());

  plusCands.Select(chargedCands  ,plusSel);
  minusCands.Select(chargedCands ,minusSel);

  // **** pid selection
  //
  kpCands.Select(plusCands    ,kSel);
  kmCands.Select(minusCands   ,kSel);
  piCands.Select(chargedCands ,piSel);
  
  // **** now start combining all composits; inbetween plot masses
  //      before using the mass selectors
  //
  phiCands.Combine(kpCands,kmCands);
    
  TCandListIterator iterPhi(phiCands);
  while (tc=iterPhi.Next()) phimass->Fill(tc->M());
  phiCands.Select(phiMSel);
  
  dsCands.Combine(phiCands,piCands);

  TCandListIterator iterDs(dsCands);
  while (tc=iterDs.Next()) dsmass->Fill(tc->M()); 
  dsCands.Select(dsMSel);
    
  pi0Cands.Combine(neutralCands,neutralCands);

  TCandListIterator iterPi0(pi0Cands);
  while (tc=iterPi0.Next()) pi0mass->Fill(tc->M()); 
  pi0Cands.Select(pi0MSel);
    

  ds0Cands.Combine(dsCands,pi0Cands);

  TCandListIterator iterDs0(ds0Cands);
  while (tc=iterDs0.Next()) ds0mass->Fill(tc->M());  

  ppCands.Combine(ds0Cands,dsCands);
  ppCands.Select(neutralSel);
    
  // **** since we didn't care about the D_s charge, combinations might appear in two
  //      different ways; RemoveClones removes double candidates based on the same final states
  ppCands.RemoveClones();
    
  TCandListIterator iterPp(ppCands);
  while (tc=iterPp.Next()) ppmass->Fill(tc->M());  
   

}
// -------------------------------------------------------------------------

void PndAnalysisTaskExample::Finish()
{
  phimass->Write();
  pi0mass->Write();
  dsmass->Write();
  ds0mass->Write();
  ppmass->Write();
  nmult->Write();

}

ClassImp(PndAnalysisTaskExample)
