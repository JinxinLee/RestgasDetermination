// -------------------------------------------------------------------------
// -----                PndMdtTrkProducer source file                  -----
// -----                  Created 16/06/08  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtTrkProducer.h"
#include "PndMdtMuonFilter.h"
#include "PndMdtGeoConstructorTo.h"
#include "PndMdtTrk.h"
#include "PndMdtHit.h"
#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TVector3.h"
#include "TMath.h"
#include "TClonesArray.h"
#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtTrkProducer::PndMdtTrkProducer() :
  FairTask(" MDT Tracklet Producer") {
  Reset();
  SetGeometry();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtTrkProducer::~PndMdtTrkProducer() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndMdtTrkProducer::Init() {
  
  cout << "-I- PndMdtTrkProducer::Init: "
       << "INITIALIZATION *********************" << endl;
  
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
    
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndMdtTrkProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  fHitArray = (TClonesArray*) ioman->GetObject("MdtHit");
  if ( ! fHitArray ) {
    cout << "-W- PndMdtTrkProducer::Init: "
	 << "No MdtHit array!" << endl;
    return kERROR;
  }
  
  // Create and register output array
  fTrkArray = new TClonesArray("PndMdtTrk");
  
  ioman->Register("MdtTrk","Mdt",fTrkArray,kTRUE);
  
  cout << "-I- PndMdtTrkProducer: Intialization successfull" << endl;
  
  return kSUCCESS;
  
}
// -------------------------------------------------------------------------

//______________________________________________________
void PndMdtTrkProducer::SetParContainers() {
  
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("PndMdtTrkProducer:: SetParContainers", "No analysis run");
  
  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("PndMdtTrkProducer:: SetParContainers", "No runtime database");

  // Get Mdt Reconstruction parameter container
  fRecoPar = (PndMdtRecoPar*) db->getContainer("PndMdtRecoPar");
  
}

//______________________________________________________
void PndMdtTrkProducer::SetGeometry() {
  // Setting the position of muon layers
  mdtLayerPos[0][0] =  PndMdt_SV100;
  mdtLayerPos[0][1] =  PndMdt_SV101;
  mdtLayerPos[0][2] =  PndMdt_SV103;
  mdtLayerPos[0][3] =  PndMdt_SV104;
  mdtLayerPos[0][4] =  PndMdt_SV105;
  mdtLayerPos[0][5] =  PndMdt_SV106;
  mdtLayerPos[0][6] =  PndMdt_SV107;
  mdtLayerPos[0][7] =  PndMdt_SV108;
  mdtLayerPos[0][8] =  PndMdt_SV109; 
  mdtLayerPos[0][9] =  PndMdt_SV110;
  mdtLayerPos[0][10]=  PndMdt_SV111; 
  mdtLayerPos[0][11]=  PndMdt_SV112;

  mdtLayerPos[1][0] =  PndMdt_SV200;
  mdtLayerPos[1][1] =  PndMdt_SV201;
  mdtLayerPos[1][2] =  PndMdt_SV202;
  mdtLayerPos[1][3] =  PndMdt_SV203;
  mdtLayerPos[1][4] =  PndMdt_SV204;
  mdtLayerPos[1][5] =  PndMdt_SV300;
  mdtLayerPos[1][6] =  PndMdt_SV301;
  mdtLayerPos[1][7] =  PndMdt_SV302;
  mdtLayerPos[1][8] =  PndMdt_SV303;
  mdtLayerPos[1][9] =  PndMdt_SV304;
  mdtLayerPos[1][10]=  PndMdt_SV305;
}

// -----   Public method Exec   --------------------------------------------
void PndMdtTrkProducer::Exec(Option_t* opt)
{
  // Reset output array
  fTrkArray->Clear();
  
  if (!MdtMapping()) return; // exit if the event contains no Mdt hits
  
  if (mapMdtBarrel.size()>0)
    {
      vector<Int_t>vecMdt0 = mapMdtBarrel[0];
      TVector3 oldPos(0., 0., 0.);
      TVector3 newPos(0., 0., 0.);
      
      for (Int_t iMap = 0; iMap < vecMdt0.size(); iMap++) // loop over hits in layer0
	{
	  Int_t layerCount = 1;
	  PndMdtTrk *mdtTrk = new PndMdtTrk();
	  mdtTrk->SetHitNumber(0, vecMdt0[iMap]);
	  mdtTrk->SetModule(1);
	  
	  PndMdtHit* mdtHit0  = (PndMdtHit*) fHitArray->At(vecMdt0[iMap]);
	  mdtHit0->Position(oldPos);
	  
	  map<Int_t, vector<Int_t> >::const_iterator layer_iter;
	  for (layer_iter=mapMdtBarrel.begin();layer_iter!=mapMdtBarrel.end();++layer_iter) // layer loop
	    {
	      if (((*layer_iter).first)==0) continue; // skip first layer
	      
	      vector<Int_t>vecMdt = (*layer_iter).second;
	      Float_t corrDist = -1;
	      Int_t corrId = -1;
	      TVector3 corrPos(0., 0., 0.);
	      for (Int_t hit_iter = 0; hit_iter < vecMdt.size(); ++hit_iter)
		{
		  PndMdtHit* mdtHit  = (PndMdtHit*) fHitArray->At(vecMdt[hit_iter]);
		  mdtHit->Position(newPos);
		  Float_t hitDist = (oldPos-newPos).Mag2();
		  if ( (corrDist<0.) || (corrDist > hitDist) ) // find closes hit
		    {
		      corrDist = hitDist;
		      corrId = vecMdt[hit_iter];
		      corrPos = newPos;
		    }
		}
	      
	      if ( (corrDist>0.) && ((TMath::Sqrt(corrDist)*10./(mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first] - mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first-1])) < 2.5) ) // if there in one correlated hit closer than 2.5 layer distance
		{
		  mdtTrk->SetHitNumber((*layer_iter).first, corrId);
		  mdtTrk->SetHitDist((*layer_iter).first, corrDist);
		  mdtTrk->SetHitMult((*layer_iter).first, mdtTrk->GetHitMult((*layer_iter).first)+1);
		  layerCount++;
		  oldPos = corrPos; // reset position for next mdt layer
		}
	      
	    } // end of layer loop
	  
	    mdtTrk->SetLayerCount(layerCount);
            AddTrk(mdtTrk); // storing the PndMdtTrk object  
	    } // end of layer0 loop
    }
  
  if (mapMdtEndcap.size()>0)
    {
      vector<Int_t>vecMdt0 = mapMdtEndcap[0];
      TVector3 oldPos(0., 0., 0.);
      TVector3 newPos(0., 0., 0.);
      
      for (Int_t iMap = 0; iMap < vecMdt0.size(); iMap++) // loop over hits in layer0
	{
	  Int_t layerCount = 1;
	  PndMdtTrk *mdtTrk = new PndMdtTrk();
	  mdtTrk->SetHitNumber(0, vecMdt0[iMap]);
	  mdtTrk->SetModule(2);
	  
	  PndMdtHit* mdtHit0  = (PndMdtHit*) fHitArray->At(vecMdt0[iMap]);
	  mdtHit0->Position(oldPos);
	  
	  map<Int_t, vector<Int_t> >::const_iterator layer_iter;
	  for (layer_iter=mapMdtEndcap.begin();layer_iter!=mapMdtEndcap.end();++layer_iter) // layer loop
	    {
	      if (((*layer_iter).first)==0) continue; // skip first layer
	      
	      vector<Int_t>vecMdt = (*layer_iter).second;
	      Float_t corrDist = -1;
	      Int_t corrId = -1;
	      TVector3 corrPos(0., 0., 0.);
	      for (Int_t hit_iter = 0; hit_iter < vecMdt.size(); ++hit_iter)
		{
		  PndMdtHit* mdtHit  = (PndMdtHit*) fHitArray->At(vecMdt[hit_iter]);
		  mdtHit->Position(newPos);
		  Float_t hitDist = (oldPos-newPos).Mag2();
		  if ( (corrDist<0.) || (corrDist > hitDist) ) // find closes hit
		    {
		      corrDist = hitDist;
		      corrId = vecMdt[hit_iter];
		      corrPos = newPos;
		    }
		}
	      
	        if ( (corrDist>0.) && ((TMath::Sqrt(corrDist)*10./(mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first] - mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first-1])) < 2.5) ) // if there in one correlated hit closer than 2.5 later distance
		{
		  mdtTrk->SetHitNumber((*layer_iter).first, corrId);
		  mdtTrk->SetHitDist((*layer_iter).first, corrDist);
		  mdtTrk->SetHitMult((*layer_iter).first, mdtTrk->GetHitMult((*layer_iter).first)+1);
		  layerCount++;
		  oldPos = corrPos; // reset position for next mdt layer
		}
	      
	    } // end of layer loop
	    
	  mdtTrk->SetLayerCount(layerCount);
	  AddTrk(mdtTrk); // storing the PndMdtTrk object  
	} // end of layer0 loop
    } // end of endcap block
      
}

// -----   Private method MdtMapping   --------------------------------------------
Bool_t PndMdtTrkProducer::MdtMapping() 
{
  Int_t nHits = fHitArray->GetEntriesFast();
  if (nHits==0) return kFALSE;
  
  Reset();
  PndMdtHit *mdtHit = NULL;
  for (Int_t iHit=0; iHit<nHits; iHit++) 
    {  
      mdtHit  = (PndMdtHit*) fHitArray->At(iHit);
      Int_t mdtModule = -1, mdtLayer = -1;

      switch (mdtHit->GetModule())
	{
	case 1:
	  mdtModule = 1;
	  mdtLayer = mdtHit->GetLayerID();
	  mapMdtBarrel[mdtLayer].push_back(iHit);
	  break;
	  
	case 2:
	  mdtModule = 2;
	  mdtLayer = mdtHit->GetLayerID();
	  mapMdtEndcap[mdtLayer].push_back(iHit);
	  break;

	case 3:
	  mdtModule = 2;
	  mdtLayer = mdtHit->GetLayerID()+5;
	  mapMdtEndcap[mdtLayer].push_back(iHit);
	  break;
	
	default:
	  cout << "-E- PndMdtTrkProducer::Init: Wrong MDT Module" << endl;
	  return kFALSE;
	}
    }
  
  return kTRUE;  
}

// -----   Private method AddTrk   --------------------------------------------
PndMdtTrk* PndMdtTrkProducer::AddTrk(PndMdtTrk* track) {
  // Creates a new hit in the TClonesArray.
  
 //  Float_t chi2=0;
//   for (Int_t ll=1; ll<10; ll++)
//     {
//       if (track->GetHitBit(ll)==0) continue;
//       if (ll==1)
// 	chi2 = chi2 + (track->GetHit(ll)/1.5)*(track->GetHitAngle(ll)/1.5); // manual correction for the first layer
//       else
// 	chi2 = chi2 + track->GetHitAngle(ll)*track->GetHitAngle(ll);
//     }
//   track->SetChi2(chi2);
  
  TClonesArray& trkRef = *fTrkArray;
  Int_t size = trkRef.GetEntriesFast();
  return new(trkRef[size]) PndMdtTrk(*track);
}

// -----   Public method Reset   --------------------------------------------
void PndMdtTrkProducer::Reset() {
  // reset maps
  mapMdtBarrel.clear();
  mapMdtEndcap.clear();
}

ClassImp(PndMdtTrkProducer)
