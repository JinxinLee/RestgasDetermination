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
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtTrkProducer::PndMdtTrkProducer() :
  FairTask(" MDT Tracklet Producer") {
  Reset();
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
  
  SetGeometry();
  
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
  Short_t version = 0;
  Text_t buffer[50]; 
  Text_t lbuffer[255];
  
  // Finding geometry version
  if (gGeoManager->FindVolumeFast("MdtBarrelLayer00")) version = 1; // Torino
  if (gGeoManager->FindVolumeFast("MdtBarrelOct0"))    version = 2; // Dubna
  if (version==0)
    {
      cout << "PndMdtTrkProducer::SetGeometry : Warning - No MDT Barrel Geometry" << endl;
    }
  else
    {
      for (Int_t ll=0; ll<15; ll++)
	{ 
	  mdtLayerPos[0][ll] = -1;
	  Int_t sec = 0;
	  sprintf(buffer,"MDT%is%il%ib%iw%i", 1, sec, ll, 0, 0);
	  TGeoVolume *v = gGeoManager->FindVolumeFast(buffer);
	  if (v)
	    {
	      if (version==1)
		{
		  sec = 0;
		  sprintf(buffer,"MDT%is%il%ib%iw%i", 1, sec, ll, 0, 0);
		  if (ll<10)
		    sprintf(lbuffer,"cave_1/Mdt_1/MdtBarrel_1/MdtBarrelLayer0%i_1/%s_%i",ll,buffer,8*ll);
		  else
		    sprintf(lbuffer,"cave_1/Mdt_1/MdtBarrel_1/MdtBarrelLayer%i_1/%s_%i",ll,buffer,8*ll);
		}
	      if (version==2)
		{ 
		  sec = 2;
		  sprintf(buffer,"MDT%is%il%ib%iw%i", 1, sec, ll, 0, 0);
		  if (ll<10)		 
		    sprintf(lbuffer,"cave_1/MdtBarrel_0/MdtBarrelOct%i_%i/MdtBarrelOct%iLayer0%i_0/BP1%i%i00_0/BA1%i%i00_0/%s_0",sec,sec,sec,ll,sec,ll,sec,ll,buffer);
		  else
		    sprintf(lbuffer,"cave_1/MdtBarrel_0/MdtBarrelOct%i_%i/MdtBarrelOct%iLayer%i_0/BP1%i%i00_0/BA1%i%i00_0/%s_0",sec,sec,sec,ll,sec,ll,sec,ll,buffer);
		}
	      
	      gGeoManager->cd(lbuffer);
	      Double_t local[3] = {0., 0., 0.};
	      Double_t master[3];
	      gGeoManager->LocalToMaster(local, master);
	      mdtLayerPos[0][ll] = master[1];
	      if (fVerbose>1) cout <<  buffer << "\t" << mdtLayerPos[0][ll] << "\t" << master[0] << "\t" << master[1] << "\t" << master[2] << endl;
	    } // end of recognized volume
	} // end of layer loop
    } // end of barrel

  Int_t ec_laymax = 0;
  version = 1; // standard Torino
  // Finding geometry version
  if (gGeoManager->FindVolumeFast("BP20000"))    version = 2; // Dubna
  for (Int_t ll=0; ll<10; ll++)
    { 
      mdtLayerPos[1][ll] = -1;
      sprintf(buffer,"MDT%is%il%ib%iw%i", 2, 0, ll, 0, 0); 
      TGeoVolume *v = gGeoManager->FindVolumeFast(buffer);
      if (v)
	{
	  if (version==1)
	    {
	      sprintf(lbuffer,"cave_1/Mdt_1/MdtEndcap_1/MdtEndcapLayer0%i_1/%s_%i",ll,buffer,200+8*ll);
	    }
	  if (version==2)
	    { 
	      sprintf(lbuffer,"cave_1/MdtEndcap_0/MdtEndcapLayer0%i_0/BP20%i00_0/BA20%i00_0/%s_0",ll,ll,ll,buffer);
	    }
	  
	  gGeoManager->cd(lbuffer);
	  Double_t local[3] = {0., 0., 0.};
	  Double_t master[3];
	  gGeoManager->LocalToMaster(local, master);
	  mdtLayerPos[1][ll] = master[2];
	  if (fVerbose>1) cout <<  buffer << "\t" << mdtLayerPos[1][ll] << "\t" << master[0] << "\t" << master[1] << "\t" << master[2] << endl;
	  ec_laymax++;
	} // end of recognized volume
    } // end of layer loop
  
  // Muon Filter
  version = 1; // standard Torino
  // Finding geometry version
  //if (gGeoManager->FindVolumeFast("BP20000"))    version = 2; // Dubna
  for (Int_t ll=0; ll<10; ll++)
    { 
      mdtLayerPos[1][ll+ec_laymax] = -1;
      sprintf(buffer,"MDT%is%il%ib%iw%i", 3, 0, ll, 0, 0); 
      TGeoVolume *v = gGeoManager->FindVolumeFast(buffer);
      if (v)
	{
	  if (version==1)
	    {
	      sprintf(lbuffer,"cave_1/Mdt_1/MdtMuonFilter_1/MdtMuonFilterLayer0%i_1/%s_%i",ll,buffer,300+8*ll);
	    }
	  if (version==2)
	    { 
	      //sprintf(lbuffer,"cave_1/MdtEndcap_0/MdtEndcapLayer0%i_0/BP20%i00_0/BA20%i00_0/%s_0",ll,ll,ll,buffer);
	    }
	  
	  gGeoManager->cd(lbuffer);
	  Double_t local[3] = {0., 0., 0.};
	  Double_t master[3];
	  gGeoManager->LocalToMaster(local, master);
	  mdtLayerPos[1][ll+ec_laymax] = master[2];
	  if (fVerbose>1) cout <<  buffer << "\t" << mdtLayerPos[1][ll+ec_laymax] << "\t" << master[0] << "\t" << master[1] << "\t" << master[2] << endl;
	} // end of recognized volume
    } // end of layer loop

  
  // Forward
  version = 0; 
  // Finding geometry version
  if (gGeoManager->FindVolumeFast("Forward"))    version = 2; // Dubna
  for (Int_t ll=0; ll<20; ll++)
    { 
      mdtLayerPos[2][ll] = -1;
      sprintf(buffer,"MDT%is%il%ib%iw%i", 4, 0, ll, 0, 0); 
      TGeoVolume *v = gGeoManager->FindVolumeFast(buffer);
      if (v)
	{
	  if (version==1)
	    {
	      //sprintf(lbuffer,"cave_1/Mdt_1/MdtMuonFilter_1/MdtMuonFilterLayer0%i_1/%s_%i",ll,buffer,300+8*ll);
	    }
	  if (version==2)
	    { 
	      if (ll<10)
		sprintf(lbuffer,"cave_1/Forward_0/MdtForwardLayer0%i_0/BP40%i00_%i/BA40%i00_0/%s_0",ll,ll,ll,ll,buffer);
	      else
		sprintf(lbuffer,"cave_1/Forward_0/MdtForwardLayer%i_0/BP40%i00_%i/BA40%i00_0/%s_0",ll,ll,ll,ll,buffer);
		
	    }
	  
	  gGeoManager->cd(lbuffer);
	  Double_t local[3] = {0., 0., 0.};
	  Double_t master[3];
	  gGeoManager->LocalToMaster(local, master);
	  mdtLayerPos[2][ll] = master[2];
	  if (fVerbose>1) cout <<  buffer << "\t" << mdtLayerPos[2][ll] << "\t" << master[0] << "\t" << master[1] << "\t" << master[2] << endl;
	} // end of recognized volume
    } // end of layer loop
  
}


// -----   Public method Exec   --------------------------------------------
void PndMdtTrkProducer::Exec(Option_t* opt)
{
  // Reset output array
  fTrkArray->Clear();
  
  if (!MdtMapping()) return; // exit if the event contains no Mdt hits
  cout << mapMdtBarrel.size() << "\t" << mapMdtEndcap.size() << "\t" << mapMdtForward.size() << endl;
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
	      
	      if ( (corrDist>0.) && ((TMath::Sqrt(corrDist)/(mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first] - mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first-1])) < 2.5) ) // if there in one correlated hit closer than 2.5 layer distance
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
	      
	        if ( (corrDist>0.) && ((TMath::Sqrt(corrDist)/(mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first] - mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first-1])) < 2.5) ) // if there in one correlated hit closer than 2.5 later distance
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
  
  if (mapMdtForward.size()>0)
    {
      vector<Int_t>vecMdt0 = mapMdtForward[0];
      TVector3 oldPos(0., 0., 0.);
      TVector3 newPos(0., 0., 0.);
      
      for (Int_t iMap = 0; iMap < vecMdt0.size(); iMap++) // loop over hits in layer0
	{
	  Int_t layerCount = 1;
	  PndMdtTrk *mdtTrk = new PndMdtTrk();
	  mdtTrk->SetHitNumber(0, vecMdt0[iMap]);
	  mdtTrk->SetModule(4);
	  
	  PndMdtHit* mdtHit0  = (PndMdtHit*) fHitArray->At(vecMdt0[iMap]);
	  mdtHit0->Position(oldPos);
	  
	  map<Int_t, vector<Int_t> >::const_iterator layer_iter;
	  for (layer_iter=mapMdtForward.begin();layer_iter!=mapMdtForward.end();++layer_iter) // layer loop
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
	      
	        if ( (corrDist>0.) && ((TMath::Sqrt(corrDist)/(mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first] - mdtLayerPos[mdtTrk->GetModule()-1][(*layer_iter).first-1])) < 2.5) ) // if there in one correlated hit closer than 2.5 later distance
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
    } // end of Forward block  
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
	  
	case 4:
	  mdtModule = 3;
	  mdtLayer = mdtHit->GetLayerID();
	  mapMdtForward[mdtLayer].push_back(iHit);
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
  mapMdtForward.clear();
}

ClassImp(PndMdtTrkProducer)
