/* $Id: */

// -------------------------------------------------------------------------
// -----                    PndEventBuilderManager source file               -----
// -----                  Created 23/09/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndEventBuilderManager
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 23.09.2013
 *@version 1.0
 **
 ** FairRoot general task for recreating events in Time-Based reconstruction mode.
 ** Various experiments should implement their own version of Event Builder.
 ** The main member of the task is vector of reconstructed events fRecoEvents.
 ** It also contains a vector of implementations of FairEventBuilders, that are
 ** responsible for feeding fRecoEvents vector via FindEvents() function.
 ** 
 ** The heart of the experiment-specific implemenations is
 ** the AnalyzeAndExtractEvents() function, which should interpret
 ** the experimental data to reconstruct events.
 **/

#include "PndEventBuilderManager.h"

#include "FairRecoEventHeader.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom2.h"

#include <iomanip>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::fixed;
using std::right;
using std::left;
using std::setw;
using std::setprecision;
using std::set;
using std::map;

using std::sort;
using std::iterator;
using std::vector;

// -----   Default constructor   ------------------------------------------
PndEventBuilderManager::PndEventBuilderManager()
  : FairEventBuilderManager(),
    fOutEvent      (NULL)
{
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndEventBuilderManager::PndEventBuilderManager(const char* name, Int_t iVerbose) 
  : FairEventBuilderManager(name, iVerbose),
    fOutEvent      (NULL)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndEventBuilderManager::~PndEventBuilderManager() {
}
// -------------------------------------------------------------------------

// -----   Private method AnalyzeAndExtractEvents   ------------------------
void PndEventBuilderManager::AnalyzeAndExtractEvents(Double_t maxEventTimeAllowed) {
  if ( fVerbose )
    cout << "PndEventBuilderManager::AnalyzeAndExtractEvents(" << maxEventTimeAllowed << ")" << endl;

  Double_t minEventTime = 10.e6;
  Double_t minEventTErr = 0.;
  FairRecoEventHeader* tempREH = NULL;
  while ( 1==1 ) {
    minEventTime = 10.e6;
    // find smallest event time
    for ( Int_t ieb = 0 ; ieb < fEventBuilders.size() ; ieb++ ) {
      for ( Int_t iev = 0 ; iev < fPossibleEvents[ieb].size() ; iev++ ) {
	tempREH = (FairRecoEventHeader*)fPossibleEvents[ieb][iev].second;
	if ( fVerbose )
	  cout << "    fPossibleEvents[" << fEventBuilders[ieb]->GetName() 
	       << "][" << iev << "] . " << fPossibleEvents[ieb][iev].first << endl;
	if ( fPossibleEvents[ieb][iev].second->GetEventTime() < minEventTime ) {// i think it is enough to take .first instead of .second->GetEventTime()
	  minEventTime = fPossibleEvents[ieb][iev].second->GetEventTime();
	  minEventTErr = fPossibleEvents[ieb][iev].second->GetEventTimeError();
	}
      }
    }
    if ( fVerbose )
      cout << "  minEventTime is now " << minEventTime << " with error " << minEventTErr << endl;
    
    // check if can be saved
    if ( maxEventTimeAllowed > -0.5 && minEventTime > maxEventTimeAllowed-5 ) {
      //      cout << "sorry, larger than " << maxEventTimeAllowed-5 << endl;
      break;
    }
    if ( minEventTime > 9.9e6 ) break;
    // calculate mean event time and remove close events from the event times' TCAs
    // in fact it should be a mean event with all event characteristics
    Double_t meanEventTime = 0.;
    Double_t nofOfEvents = 0.;
    Double_t sumTW = 0.;
    Double_t sumW  = 0.;
    Int_t    ident = 0;
    for ( Int_t ieb = 0 ; ieb < fEventBuilders.size() ; ieb++ ) {
      for ( Int_t iev = 0 ; iev < fPossibleEvents[ieb].size() ; iev++ ) {
	tempREH = (FairRecoEventHeader*)fPossibleEvents[ieb][iev].second;
	if ( TMath::Abs(tempREH->GetEventTime()-minEventTime) < minEventTErr+tempREH->GetEventTimeError() )  {
	  if ( tempREH->GetEventTimeError() < 1.e-6 ) {
	    cout << "ERROR ALMOST 0!!!! " << tempREH->GetEventTimeError() << endl;
	    continue;
	  }
	  //	  meanEventTime += tempREH->GetEventTime();
	  sumTW += tempREH->GetEventTime()/tempREH->GetEventTimeError()/tempREH->GetEventTimeError();
	  sumW  +=                      1./tempREH->GetEventTimeError()/tempREH->GetEventTimeError();
	  ident |= tempREH->GetIdentifier();
	  minEventTime = sumTW/sumW;
	  minEventTErr = TMath::Sqrt(1./sumW);
	  if ( fVerbose )
	    cout << "    added event at " << tempREH->GetEventTime() << " +- " << tempREH->GetEventTimeError() << endl;
	  nofOfEvents   += 1.;
	  fPossibleEvents[ieb][iev] = fPossibleEvents[ieb][fPossibleEvents[ieb].size()-1];
	  fPossibleEvents[ieb].pop_back();
	}
      }
    }
    if ( nofOfEvents == 0 ) continue;

    if ( fVerbose ) {
      cout << "  CREATED EVENT AT " << meanEventTime/nofOfEvents << " OUT OF "  << nofOfEvents << " EVENTS" << endl;
      cout << "  WEIGHTED MEAN AT " << sumTW/sumW << " WITH ERROR OF " << TMath::Sqrt(1./sumW) << endl;
    }

    fOutEvent->SetEventTime(sumTW/sumW,TMath::Sqrt(1./sumW));//meanEventTime/nofOfEvents,2.);
    fOutEvent->SetIdentifier(ident);

    CreateAndFillEvent(fOutEvent);
  }
}
// -------------------------------------------------------------------------


// -----   Private method SetParContainers   -------------------------------
void PndEventBuilderManager::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus PndEventBuilderManager::Init() {
  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");

  cout << "*** PndEventBuilderManager. " << fEventBuilders.size() << " event builders registered." << endl; 

  fOutEvent = new FairRecoEventHeader();
  FairRootManager::Instance()->Register("RecoEventHeader.", "RecoEvent", fOutEvent, kTRUE);

  for ( Int_t ieb = 0 ; ieb < fEventBuilders.size() ; ieb++ ) {
    fEventBuilders[ieb]->SetIdentifier(TMath::Power(2,ieb));
    fEventBuilders[ieb]->Init();
  }

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -------------------------------------------
InitStatus PndEventBuilderManager::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------




ClassImp(PndEventBuilderManager)

