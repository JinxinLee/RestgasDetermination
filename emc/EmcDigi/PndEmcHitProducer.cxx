/////////////////////////////////////////////////////////////
//
//  PndEmcHitProducer
//
//  Filler of PndEmcHit
//
//  Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////// 

#include "PndEmcHitProducer.h"

#include "PndEmcStructure.h"
#include "PndEmcHit.h"
#include "PndEmcPoint.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"		
#include "PndEmcDigiNonuniformityPar.h"		
#include "PndMCTrack.h"

#include "PndEmcXtal.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TROOT.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TSystem.h"
#include "TString.h"

using std::cout;
using std::endl;
using std::map;


// -----   Default constructor   -------------------------------------------
PndEmcHitProducer::PndEmcHitProducer() :
  FairTask("Ideal EMC hit Producer"),
fUse_nonuniformity(0), fNonuniformityFile(""), fPointArray(new TClonesArray()), fMCTrackArray(new TClonesArray()), fDigiArray(new TClonesArray()), fVolumeArray(new TObjArray()), fMapVersion(0), emcX(), emcY(), emcZ(), fEmcStr(), fMapper(), fDigiPar(new PndEmcDigiPar()), fGeoPar(new PndEmcGeoPar()), fNonuniformityPar(new PndEmcDigiNonuniformityPar()), fStoreHits(kTRUE), fEnergyThreshold(0)
{
  fNonuniformityFile=gSystem->Getenv("VMCWORKDIR");
  fNonuniformityFile+="/input/EmcDigiNoniformityPars.root";
}
// -------------------------------------------------------------------------

PndEmcHitProducer::PndEmcHitProducer(Bool_t val) :
  FairTask("Ideal EMC hit Producer"),
fUse_nonuniformity(0), fNonuniformityFile(""), fPointArray(new TClonesArray()), fMCTrackArray(new TClonesArray()), fDigiArray(new TClonesArray()), fVolumeArray(new TObjArray()), fMapVersion(0), emcX(), emcY(), emcZ(), fEmcStr(), fMapper(), fDigiPar(new PndEmcDigiPar()), fGeoPar(new PndEmcGeoPar()), fNonuniformityPar(new PndEmcDigiNonuniformityPar()), fStoreHits(val), fEnergyThreshold(0)
 { 
  fNonuniformityFile=gSystem->Getenv("VMCWORKDIR");
  fNonuniformityFile+="/input/EmcDigiNoniformitypars.root";
}

// -----   Destructor   ----------------------------------------------------
PndEmcHitProducer::~PndEmcHitProducer() { delete fEmcStr;}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndEmcHitProducer::Init(){
  
  cout << " INITIALIZATION *********************" << endl;
  
  //FairDetector::Initialize();
  //FairRun* sim = FairRun::Instance();
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman ){
    cout << "-E- PndEmcHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("EmcPoint");
  if (! fPointArray ){
    cout << "-W- PndEmcHitProducer::Init: "
	 << "No EmcPoint array!" << endl;
    return kERROR;
  }
  
  // Get input array
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if (! fMCTrackArray ){
    cout << "-W- PndEmcMakeCluster::Init: "
	 << "No MCTrack array! Needed for MC Truth" << endl;
    //return kERROR;
  }
  
  // Create and register output array
  fDigiArray = new TClonesArray("PndEmcHit");
  
  ioman->Register("EmcHit","Emc",fDigiArray,fStoreHits);
  
  fGeoPar->InitEmcMapper();
  fMapper=PndEmcMapper::Instance();
  fEmcStr=PndEmcStructure::Instance();
  
  emcX=fEmcStr->GetEmcX();
  emcY=fEmcStr->GetEmcY();
  emcZ=fEmcStr->GetEmcZ();;
  
  fEnergyThreshold =fDigiPar->GetEnergyHitThreshold();
  fUse_nonuniformity = fDigiPar->GetUse_nonuniformity();

  if(fUse_nonuniformity){
	  cout << "-I- PndEmcHitProducer: Using nonuniform lightoutput" << endl;
  }
  if(fUse_nonuniformity && fNonuniformityFile.Length()>0){
	  TFile *nonuniformityfile = new TFile(fNonuniformityFile);
	  if(nonuniformityfile==NULL){
		  cout << "-E- PndEmcHitProducer: Could not open file " << fNonuniformityFile.Data() << " for Nonuniformity Information" << endl;
	  } else {
		  PndEmcDigiNonuniParObject *parObject;
		  nonuniformityfile->GetObject("PndEmcDigiNonuniParObject",parObject);
		  if(parObject == NULL){
			  cout << "-E- PndEmcHitProducer: Could not get Nonuniformity information from file " << fNonuniformityFile.Data() << endl;
		  } else {
			  fNonuniformityPar->SetNonuniParObject(parObject);
		  }
	  }
  }


//  printf("HitProducer has EnergyHitThreshold of %f GeV and Use_nonuniformity %i\n", fEnergyThreshold, fUse_nonuniformity);
  
  cout << "-I- PndEmcHitProducer: Intialization successfull" << endl;
  
  return kSUCCESS;
}

void PndEmcHitProducer::SetParContainers(){
  
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if (! run ) Fatal("SetParContainers", "No analysis run");
  
  FairRuntimeDb* db = run->GetRuntimeDb();
  if (! db ) Fatal("SetParContainers", "No runtime database");
  
  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");
  
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");

  fNonuniformityPar = (PndEmcDigiNonuniformityPar*) db->getContainer("PndEmcDigiNonuniformityPar");
 
}

// -------------------------------------------------------------------------

// Helper function, does not depend on class, identical to the one in PndEmcMakeCluster
void PndEmcHitProducer::cleansortmclist( std::vector <Int_t> &newlist,TClonesArray* mcTrackArray)
{
  std::vector <Int_t> tmplist;
  // Sort list...
  std::sort( newlist.begin(), newlist.end());
  // and copy every id only once (even so it might be in the list several times)
  std::unique_copy( newlist.begin(), newlist.end(), std::back_inserter( tmplist ) );
  
  // Now check if mother or (grand)^x-mother are already in the list
  // (which means i am a secondary)... if so, remove myself
  for(Int_t j=tmplist.size()-1; j>=0; j--){
    bool flag;
    PndMCTrack *pt;
    pt=((PndMCTrack*)mcTrackArray->At(tmplist[j]));
    if(pt->GetMotherID()<0) continue;
    flag=false;
    while(!flag){
      Int_t id;
      id=pt->GetMotherID();
      if(id<0) break;
      pt=(PndMCTrack*)mcTrackArray->At(id);
      
      for(Int_t k=j-1; k>=0; k--){
	if(tmplist[k]==id){
	  tmplist.erase(tmplist.begin()+j);
	  flag=true;
	  break;
	}
      }
    }
  }
  newlist=tmplist;
}

// -----   Public method Exec   --------------------------------------------
void PndEmcHitProducer::Exec(Option_t* opt)
{  
  cout << " DIGI EXECUTION *********************" << endl;
  // Reset output array
  if (! fDigiArray ) Fatal("Exec", "No DigiArray");
  
  fDigiArray->Delete();
  
  // Declare some variables
  //PndEmcPoint* point  = NULL;
  Int_t DetId;
  map<Int_t, Float_t> fTrackEnergy;
  map<Int_t, Float_t> fTrackTime;  //time of first point
  map<Int_t, std::vector <Int_t> > fTrackMcTruth;  //McTruth
  map<Int_t, std::vector <Int_t> > fPointMatch; //DetId , PointIds with same DetId

  
  fTrackEnergy.clear();
  fTrackTime.clear();
  fTrackMcTruth.clear();
  fPointMatch.clear();
  
  map<Int_t, Float_t>::const_iterator p;
  
  std::vector<PndEmcPoint*> fPointList;// to pass to EmcHit
  const PndEmcTciXtalMap &XtalMap = fEmcStr->GetTciXtalMap();
  TVector3 frontvec;
  TVector3 normvec;
  TVector3 pointvec;
  TVector3 distvec;
  Double_t zpos;
  Double_t energyscalefactor=1.0;
  Double_t c[3];
  PndEmcXtal *tmpXtal;
  PndEmcTwoCoordIndex *tmpTCI;
  // Loop over EmcPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  
  Double_t point_time = 0.00;
  //------- init containers --- 

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
    PndEmcPoint* point  = (PndEmcPoint*) fPointArray->At(iPoint);
    fTrackEnergy[point->GetDetectorID()] = 0.00;
    fTrackTime  [point->GetDetectorID()] = std::numeric_limits<float>::max();
  }

  //----------------------------


  for (Int_t iPoint=0; iPoint<nPoints; iPoint++)
  {
    PndEmcPoint* point  = (PndEmcPoint*) fPointArray->At(iPoint);
	DetId = point->GetDetectorID();
	if(fUse_nonuniformity !=0 ){
		//light output is z-dependent, so calculate z
		
		tmpTCI = fMapper->GetTCI(DetId);
		if(tmpTCI == NULL){
			printf("no TCI found for DetectorID %d\n",DetId);
			continue;
		}
		tmpXtal =XtalMap.find(tmpTCI)->second;
		point->Position(pointvec);
		frontvec = tmpXtal->frontCentre();
		normvec = tmpXtal->normalToFrontFace();
		distvec = pointvec-frontvec;
		zpos = distvec.Dot(normvec);
		fNonuniformityPar->GetNonuniformityParameters(DetId,c);
		energyscalefactor=c[0]+zpos*(c[1]+zpos*c[2]);
		fTrackEnergy[DetId] += point->GetEnergyLoss() * energyscalefactor;
		fPointMatch[DetId].push_back(iPoint);
//        printf("point with detID %d has z Position %f and energyloss %f scaled with %f\n",DetId,zpos, point->GetEnergyLoss(),energyscalefactor);	
	} else {
		fTrackEnergy[DetId] += point->GetEnergyLoss();
		fPointMatch[DetId].push_back(iPoint);
//        printf("point with detID %d has z Position %f and energyloss %f not scaled\n",DetId,zpos, point->GetEnergyLoss());	
	}
	point_time=point->GetTime();
    
    if (point_time < fTrackTime[point->GetDetectorID()]){
      fTrackTime[point->GetDetectorID()] = point_time;
    }
    
    // Check and save MC truth information
    // Eloss==0 tracks are only stored in point, if track is entering detector from outside
    // and thats what we are interested in...
    if(point->GetEnergyLoss() == 0){
      (fTrackMcTruth[point->GetDetectorID()]).push_back(point->GetTrackID());
      //cout << "ELoss==0 : ID " <<
      //point->GetTrackID()<<","<<point->GetDetectorID()<<","<<point->GetXPad()<<","<<point->GetYPad()<<endl;
    }
  }

  // Loop over EmcPoint
  
  // Loop to register EmcHit
  for( p = fTrackEnergy.begin(); p != fTrackEnergy.end(); ++p){
    if ((*p).second > fEnergyThreshold){
      // Check and save MC truth information B.S.
      // remove MC Truth particles which are not needed (eg grand^x-daugherts)
      if( fMCTrackArray){
		cleansortmclist(fTrackMcTruth[(*p).first],fMCTrackArray);
      }
      PndEmcHit* myHit = AddHit(1, (*p).first, (*p).second, fTrackTime[(*p).first], fTrackMcTruth[(*p).first]);
      myHit->AddLinks(FairMultiLinkedData("EmcPoint", fPointMatch[p->first]));
    }
  }
}

// -------------------------------------------------------------------------
// -----   Private method AddDigi   --------------------------------------------
PndEmcHit* PndEmcHitProducer::AddHit(Int_t trackID,Int_t detID, Float_t energy,
				     Float_t time, std::vector <Int_t> &mctruth)
{
  // It fills the PndEmcHit category
  
  //cout << "PndEmcHitProducer: track " << trackID << " evt " << eventID
  //<< " sec " << sec << " plane " << pla << " strip " << strip << "box
  //" << box << " tube " << tub << endl;
  TClonesArray& clref = *fDigiArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndEmcHit(trackID, detID, energy, time, emcX[detID], 
				    emcY[detID], emcZ[detID], mctruth);
}
// ----

void PndEmcHitProducer::SetStorageOfData(Bool_t val)
{
  fStoreHits=val;
  return;
}

ClassImp(PndEmcHitProducer)
