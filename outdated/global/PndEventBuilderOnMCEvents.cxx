/* $Id: */

// -------------------------------------------------------------------------
// -----                    PndEventBuilderOnMCEvents source file         -----
// -----                  Created 22/08/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndEventBuilderOnMCEvents
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 22.08.2013
 *@version 1.0
 **
 ** PANDA ideal event builder from ideal event
 ** information from FairEventHeader.
 ** Task level RECO
 **/

#include "PndEventBuilderOnMCEvents.h"

#include "FairEventHeader.h"

#include "PndDetectorList.h"
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

// -----   Default constructor   ------------------------------------------
PndEventBuilderOnMCEvents::PndEventBuilderOnMCEvents()
  : FairEventBuilder(),
    fTNofEvents    (0),
    fTNofRecoEvents(0),
    fExecTime      (0.)
{
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndEventBuilderOnMCEvents::PndEventBuilderOnMCEvents(const char* name, Int_t iVerbose) 
  : FairEventBuilder(),
    fTNofEvents    (0),
    fTNofRecoEvents(0),
    fExecTime      (0.)
{
  SetBuilderName(name);
  fVerbose = iVerbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndEventBuilderOnMCEvents::~PndEventBuilderOnMCEvents() {
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
std::vector<std::pair<double, FairRecoEventHeader*> > PndEventBuilderOnMCEvents::FindEvents() {
  if ( fVerbose )
    cout << endl << "======== PndEventBuilderOnMCEvents::FindEvents(Event = " << fTNofEvents << " ) ====================" << endl;

  fTimer.Start();
 
  fTNofEvents += 1;

  fTNofRecoEvents += 1;

  std::vector<std::pair<double, FairRecoEventHeader*> > result;
  std::pair<double,FairRecoEventHeader*> singleResult;

  FairRecoEventHeader* recoEvent = new FairRecoEventHeader();
  recoEvent->SetEventTime(fEventHeader->GetEventTime(),0.001);
  recoEvent->SetIdentifier(GetIdentifier());
  SetMaxAllowedTime(fEventHeader->GetEventTime());

  singleResult.first  = recoEvent->GetEventTime();
  singleResult.second = recoEvent;
  
  result.push_back(singleResult);

  fExecTime += fTimer.RealTime();
  fTimer.Stop();  

  return result;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndEventBuilderOnMCEvents::Print() {
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndEventBuilderOnMCEvents::StoreEventData(FairRecoEventHeader* recoEvent) {
}

std::vector<std::pair<double, FairRecoEventHeader*> > PndEventBuilderOnMCEvents::Modify(std::pair<double, FairRecoEventHeader*> oldData, std::pair<double, FairRecoEventHeader*> newData)
{
  std::vector<std::pair<double, FairRecoEventHeader*> > result;
  result.push_back(newData);
  return result;
}

void PndEventBuilderOnMCEvents::AddNewDataToTClonesArray(FairTimeStamp* data)
{
  // FairRootManager* ioman = FairRootManager::Instance();
  // TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);
  // if (fVerbose > 1) std::cout << "Data Inserted: "  <<  *(FairRecoEventHeader*)(data) << std::endl;
  // new ((*myArray)[myArray->GetEntries()]) FairRecoEventHeader(*(FairRecoEventHeader*)(data));
}

double PndEventBuilderOnMCEvents::FindTimeForData(FairTimeStamp* data) 
{
  // std::map<FairRecoEventHeader, double>::iterator it;
  // FairRecoEventHeader myData = *(FairRecoEventHeader*)data;
  // it = fData_map.find(myData);
  // if (it == fData_map.end())
  //   return -1;
  // else
  //   return it->second;
}
void PndEventBuilderOnMCEvents::FillDataMap(FairTimeStamp* data, double activeTime) 
{
  // FairRecoEventHeader myData = *(FairRecoEventHeader*)data;
  // fData_map[myData] = activeTime;
}

void PndEventBuilderOnMCEvents::EraseDataFromDataMap(FairTimeStamp* data)
{
  // FairRecoEventHeader myData = *(FairRecoEventHeader*)data;
  // if (fData_map.find(myData) != fData_map.end())
  //   fData_map.erase(fData_map.find(myData));
}

// -----   Private method SetParContainers   -------------------------------
void PndEventBuilderOnMCEvents::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
Bool_t PndEventBuilderOnMCEvents::Init() {

  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");

  fEventHeader = (FairEventHeader*) ioman->GetObject("EventHeader.");

  return kTRUE;
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
Bool_t PndEventBuilderOnMCEvents::ReInit() {

  // Create sectorwise digi sets
  //  MakeSets();

  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void PndEventBuilderOnMCEvents::Finish() {

  cout << "-------------------- " << GetBuilderName() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents     << endl;
  cout << " Reco Events:   " << setw(10) << fTNofRecoEvents << endl;
  cout << " . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . " << endl;
  cout << " >>> EB >>> all   time  = " << fExecTime << "s" << endl;
  cout << "---------------------------------------------------------------------" << endl; 
}
// -------------------------------------------------------------------------


ClassImp(PndEventBuilderOnMCEvents)

