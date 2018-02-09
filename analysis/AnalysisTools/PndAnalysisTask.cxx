/******************************************************

JPsi->e+e-
Reads the TPC tracks and reconstruct the InvariantMass
of J/Psi: Dipak
*******************************************************/

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TVector3.h"
#include "TH1F.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndAnalysisTask.h"
#include <string>
#include <iostream>

//RHO stuff
#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoCandList.h"
#include "RhoBase/RhoCandListIterator.h"
#include "RhoBase/RhoFactory.h"


using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndAnalysisTask::PndAnalysisTask() :
  FairTask("Panda Analysis Task")
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndAnalysisTask::~PndAnalysisTask() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndAnalysisTask::Init()
{

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
    cout << "-W- PndAnalysisTask::Init: "
         << "No PndChargedCandidates && PndNeutralCandidates array!" << endl;
    return kERROR;
  }

  // Create and register output array
  cout << "-I- PndAnalysisTask: Intialization successfull" << endl;

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
  //chargedSel = new RhoChargedParticleSelector;
  neutralSel = new RhoNeutralParticleSelector;
  plusSel    = new RhoPlusParticleSelector;
  minusSel   = new RhoMinusParticleSelector;

  // **** mass selectors for the resonances/composites
  //
  phiMSel       = new RhoMassParticleSelector("phiSelector" , 1.0195 , 0.01);
  pi0MSel       = new RhoMassParticleSelector("pi0Selector" , 0.135  , 0.005);
  dsMSel        = new RhoMassParticleSelector("dsSelector"  , 1.9685 , 0.01);

  kSel    = new RhoSimpleKaonSelector();
  kSel->SetCriterion("loose");
  piSel   = new RhoSimplePionSelector();
  piSel->SetCriterion("veryLoose");

  evcount=0;

  return kSUCCESS;

}

void PndAnalysisTask::SetParContainers()
{

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) { Fatal("SetParContainers", "No analysis run"); }

  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");


}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndAnalysisTask::Exec(Option_t*)
{

  RhoFactory::Instance()->Reset();

  if (!(++evcount%100)) { cout <<"evt "<<evcount<<endl; }

  // **** create all the particle lists we'll need for rebuilding the decay tree
  //
  RhoCandList neutralCands,chargedCands, plusCands,minusCands;

  RhoCandList kpCands,kmCands,piCands;

  RhoCandList phiCands,pi0Cands,dsCands,ds0Cands,ppCands;


  RhoCandidate* tc;

  // **** loop over all Candidates and add them to the list allCands
  //
  chargedCands.Cleanup();
  neutralCands.Cleanup();

  for (Int_t i1=0; i1<fChargedArray->GetEntriesFast(); i1++) {
    tc = (RhoCandidate*)fChargedArray->At(i1);
    chargedCands.Add(tc);
  }

  for (Int_t i1=0; i1<fNeutralArray->GetEntriesFast(); i1++) {
    tc = (RhoCandidate*)fNeutralArray->At(i1);
    neutralCands.Add(tc);
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

  RhoCandListIterator iterPhi(phiCands);
  while ((tc=iterPhi.Next())) { phimass->Fill(tc->M()); }
  phiCands.Select(phiMSel);

  dsCands.Combine(phiCands,piCands);

  RhoCandListIterator iterDs(dsCands);
  while ((tc=iterDs.Next())) { dsmass->Fill(tc->M()); }
  dsCands.Select(dsMSel);

  pi0Cands.Combine(neutralCands,neutralCands);

  RhoCandListIterator iterPi0(pi0Cands);
  while ((tc=iterPi0.Next())) { pi0mass->Fill(tc->M()); }
  pi0Cands.Select(pi0MSel);


  ds0Cands.Combine(dsCands,pi0Cands);

  RhoCandListIterator iterDs0(ds0Cands);
  while ((tc=iterDs0.Next())) { ds0mass->Fill(tc->M()); }

  ppCands.Combine(ds0Cands,dsCands);
  ppCands.Select(neutralSel);

  // **** since we didn't care about the D_s charge, combinations might appear in two
  //      different ways; RemoveClones removes double candidates based on the same final states
  ppCands.RemoveClones();

  RhoCandListIterator iterPp(ppCands);
  while ((tc=iterPp.Next())) { ppmass->Fill(tc->M()); }


}
// -------------------------------------------------------------------------

void PndAnalysisTask::Finish()
{
  phimass->Write();
  pi0mass->Write();
  dsmass->Write();
  ds0mass->Write();
  ppmass->Write();
  nmult->Write();

}

ClassImp(PndAnalysisTask)
