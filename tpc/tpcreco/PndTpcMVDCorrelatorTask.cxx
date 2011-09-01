//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndMVDCorrelatorTask
//      see PndMVDCorrelatorTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcMVDCorrelatorTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>
#include <set>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"

#include "PndTpcCluster.h"
#include "PndTpcPlanarRecoHit.h"
#include "PndTpcSPHit.h"

#include "GFRecoHitFactory.h"
#include "GFException.h"

#include "GFAbsRecoHit.h"

#include "GeaneTrackRep.h"
#include "RKTrackRep.h"
//#include "FairGeanePro.h"

#include "PndDetectorList.h"
#include "PndSdsHit.h"
#include "PndSdsRecoHit.h"

#include "TH1D.h"
#include "TGraph.h"
#include "TVector2.h"

// Class Member definitions -----------


bool sortByR(const PndSdsHit* hl, const PndSdsHit* hr) {
  TVector3 posL;
  TVector3 posR;
  hl->Position(posL);
  hr->Position(posR);
  return (posL.Perp() < posR.Perp());
}

DetPlaneWrapper::DetPlaneWrapper(const GFDetPlane& pl)
  :
  fPl(pl)
{;}

bool
operator== (const DetPlaneWrapper& lhs, const DetPlaneWrapper& rhs) {
  double eps = 1.E-5;
  TVector3 lO = lhs.fPl.getO();
  TVector3 rO = rhs.fPl.getO();
  TVector3 lN = lhs.fPl.getNormal();
  TVector3 rN = rhs.fPl.getNormal();
  double rDist = fabs(lO.Perp() - rO.Perp());
  if(rDist < eps) { // && dPhi < eps && dTheta < eps) {
    double dPhi = fabs(lN.Phi() - rN.Phi());
    double dTheta = fabs(lN.Theta() - rN.Theta());
    if(dPhi < eps && dTheta < eps) 
      return true;
    TVector3 rNneg = (-1.)*rN;
    dPhi = fabs(lN.Phi() - rNneg.Phi());
    dTheta = fabs(lN.Theta() - rNneg.Theta());
    if(dPhi < eps && dTheta < eps) 
      return true;
  }
  return false;
}

bool
operator< (const DetPlaneWrapper& lhs, const DetPlaneWrapper& rhs) {
  if(lhs==rhs)
    return false;
  else
    return lhs.fPl.getO().Perp() < rhs.fPl.getO().Perp();
}


PndTpcMVDCorrelatorTask::PndTpcMVDCorrelatorTask()
  : FairTask("TPC-MVD Correlator"), fPersistence(kFALSE), fMatchDistance(0.2), fAngleCut(TMath::PiOver2()),
    fMinMVDHits(0), fWindow(10.), fScan(false), fScanSteps(0), fScanMin(0.), fScanMax(0.), fNPhys(0), ft0(0.)
{
  fOutTrackBranchName = "TrackPreFitComplete";
  fTrackBranchName = "TrackPostFit";
  fPixelBranchName = "MVDHitsPixelMixed";
  fStripBranchName = "MVDHitsStripMixed";
  fClusterBranchName = "PndTpcCluster";
  fVerbose = 0;
}


PndTpcMVDCorrelatorTask::~PndTpcMVDCorrelatorTask()
{
  delete fResHistU;
  delete fResHistV;
  //delete fResHistZ;
}

InitStatus
PndTpcMVDCorrelatorTask::Init()
{
  
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcMVDCorrelatorTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);

  if(fTrackArray==0)
    {
      Error("PndTpcMVDCorrelatorTask::Init","track-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fOutTrackArray = new TClonesArray("GFTrack"); 
  ioman->Register(fOutTrackBranchName,"PndTpc",fOutTrackArray,fPersistence);
  
  
  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();
  
  fStripArray=(TClonesArray*) ioman->GetObject(fStripBranchName);
  if(fStripArray==0){ 
    TString error = fStripBranchName;
    error.Append(" array not found");
    Error("PndTpcMVDCorrelatorTask::Init", error);
  } 
  else {
    fTheRecoHitFactory->addProducer
      (ioman->GetBranchId(fStripBranchName),new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(fStripArray));
    ;
  }
  
  fPixelArray=(TClonesArray*) ioman->GetObject(fPixelBranchName);
  if(fPixelArray==0){ 
    TString error = fPixelBranchName;
    error.Append(" array not found");
    Error("PndTpcMVDCorrelatorTask::Init", error);
  } 
  else {
    fTheRecoHitFactory->addProducer
      (ioman->GetBranchId(fPixelBranchName),new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(fPixelArray));
    ;
  }

  if(fScan) {
    fClusterArray = (TClonesArray*) ioman->GetObject(fClusterBranchName);
    if(fClusterArray==NULL) {
      TString error = fClusterBranchName;
      error.Append(" array not found");
      Error("PndTpcMVDCorrelatorTask::Init", error);
    }
  }

  fResHistU = new TH1D("resHistU", "Extrapolation residual distribution U",  200, -2, 2);
  fResHistV = new TH1D("resHistV", "Extrapolation residual distribution V",  200, -2, 2);
  //fResHistZ = new TH1D("resHistZ", "Extrapolation residual distribution",  200, -2, 2);
  
  return kSUCCESS;
}


void
PndTpcMVDCorrelatorTask::Exec(Option_t* opt)
{
   //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  std::cout<<"PndTpcMVDCorrelatorTask::Exec"<<std::endl;
  fOutTrackArray->Delete();
  
  //Bookkeeping. All pointers come from the input trees over the IO manager
  //Framework should handly ownership correctly, no manual cleaning up done.
  std::vector<GFAbsRecoHit*> recoHits;
  std::map<DetPlaneWrapper, std::vector<GFAbsRecoHit*>* > recoHitMap;
  std::map<GFAbsRecoHit*, std::pair<TString, unsigned int> > idMap;
  std::map<GFAbsRecoHit*, PndSdsHit*> conMap;
  std::map<PndSdsHit*, GFAbsRecoHit*> backMap;
  
  
  
  unsigned int nPix = fPixelArray->GetEntriesFast();
  unsigned int nStr = fStripArray->GetEntriesFast();
  //create recohits from MVD hits 
  for(unsigned int ipx=0; ipx<nPix; ipx++) {
    GFAbsRecoHit* ihit = fTheRecoHitFactory->createOne(ioman->GetBranchId(fPixelBranchName),
						       ipx);
    PndSdsHit* sdsHit = (PndSdsHit*) fPixelArray->At(ipx);
    
    double timestamp = sdsHit->GetTimeStamp();
    double difftime = fabs(timestamp - ft0);
    if(difftime>fWindow)  
      continue;             //ignore MVD hits outside time window
    recoHits.push_back(ihit);
    
    conMap[ihit] = sdsHit;
    backMap[sdsHit] = ihit;
    
    std::pair<TString, unsigned int> p;
    p.first = fPixelBranchName;
    p.second = ipx;
    idMap[ihit] = p;
  }
  for(unsigned int ist=0; ist<nStr; ist++) {
    GFAbsRecoHit* ihit = fTheRecoHitFactory->createOne(ioman->GetBranchId(fStripBranchName),
						       ist);
    PndSdsHit* sdsHit = (PndSdsHit*) fStripArray->At(ist);
    double timestamp = sdsHit->GetTimeStamp();
    double difftime = fabs(timestamp - ft0);
    if(difftime>fWindow)  
      continue;             //ignore MVD hits outside time window
    recoHits.push_back(ihit);
    
    conMap[ihit] = sdsHit;
    backMap[sdsHit] = ihit;
    
    std::pair<TString, unsigned int> p;
    p.first = fStripBranchName;
    p.second = ist;
    idMap[ihit] = p;
  }

  unsigned int ntracks = fTrackArray->GetEntriesFast();
  std::map<unsigned int, std::vector<double> > fPhysResMap;  //<track nb, list of residuals>
  std::map<unsigned int, std::vector<double> > fBkgResMap;  //<track nb, list of residuals>
 
  
   
  //loop over tracks
  for(unsigned int itr=0;  itr<ntracks; itr++) {
    GFTrack* track = (GFTrack*) (*fTrackArray)[itr];
    bool fPhys = true;
    if(fScan) {
      //decide whether this is a physics or a bg track
      McIdCollection idc;
      GFTrackCand trCand = track->getCand();
      std::vector<unsigned int> detIDs = trCand.GetDetIDs(); 
      std::set<unsigned int> IDset;
      for(unsigned int id=0; id<detIDs.size(); id++)
	IDset.insert(detIDs[id]);
      assert(IDset.size()==1);                       //assuming a pure TPC track
      std::vector<unsigned int> clustIDs = trCand.GetHitIDs(detIDs[0]);
      //loop over clusters
      for(unsigned int icl=0; icl<clustIDs.size(); icl++) {
	PndTpcCluster* cl = (PndTpcCluster*) fClusterArray->At(clustIDs[icl]);
	idc.AddIDCollection(cl->mcId());
      } //end loop over clusters
      //convention: physics event has ID 0, bkg > 0 :
      unsigned int eventID = idc.DominantID().mceventID();
      if(eventID!=0)
	fPhys=false;
    }      
      
    GFAbsTrackRep* rep = track->getCardinalRep();
    TVector3 trkStartPos=track->getPos();
    trkStartPos.SetZ(0.); //we're only interested in the XY projection of the angle
        
    //get (physical) detplanes and organize
    if(itr==0) {
      for(unsigned int ih=0; ih<recoHits.size(); ih++) {
	//this extrapolates, I would rather like to get the physical detplane 
	//directly from the hit(-policy!)
	//wishlist for genfit
	DetPlaneWrapper wr(recoHits[ih]->getDetPlane(rep));
	if(recoHitMap.count(wr)==0) 
	  recoHitMap[wr] = new std::vector<GFAbsRecoHit*>();
	
	recoHitMap[wr]->push_back(recoHits[ih]);
      }
      std::cout<<" PndTpcMVDCorrelatorTask::Exec() Found "<<recoHitMap.size()
	       <<" distinct detplanes from "
	       <<recoHits.size()<<" RecoHits."<<std::endl;
    }
    
    if(fVerbose) std::cout<<"  ... processing TPC track no. "<<itr<<std::endl;

    //end of general preparations -------------------------------------------------
    
    //Now calculate penetration point of track for each hit MVD plane
    std::map<DetPlaneWrapper, TVector2> penetrationMap; //not used now, for more efficient implemetnation later
    std::map<DetPlaneWrapper, std::vector<GFAbsRecoHit*>* >::iterator it;

    std::vector<PndSdsHit*> tempCand;  //selected hits during extrapolation
        
    //loop over planes
    for (it=recoHitMap.begin(); it!=recoHitMap.end(); it++) { 
      bool ok=true;
      TMatrixT<double> res(2,1);
      GFDetPlane pl = it->first.getPlane();
      //pl.Print();
      TVector2 minRes(100.,100.);
      int bestMatch = -1;
      //loop over hits of the current DetPlane
      for(unsigned int ihit=0; ihit<it->second->size(); ihit++) {
	GFAbsRecoHit* exHit = (*it->second)[ihit];
	
	//check for same hemisphere
	TVector3 globPos;
	conMap[exHit]->Position(globPos);
	globPos.SetZ(0.);
	double angle = trkStartPos.Angle(globPos);
	if(fabs(angle)>fAngleCut) {
	  if(fVerbose) 
	    std::cout<<"hit in wrong hemisphere, skipping!"<<  std::endl;
	  continue; 
	}
	
	TMatrixT<double> statePred(5,1);
	TMatrixT<double> covPred(5,5);
	try {
	  rep->extrapolate(pl,statePred,covPred);
	  res = exHit->residualVector(rep,statePred,pl);
	}
	catch(GFException& ex) {
	  ok=false;
	}
	  //calculate penetration point:
	TVector2 negRes(-res[0][0], -res[1][0]);
	if(ok)  //residual calculation went ok for this plane
	  penetrationMap[it->first] = negRes;
	
	else {
	  penetrationMap[it->first] = TVector2(-100,-100);
	  continue;
	}
	
	fResHistU->Fill(negRes.X());
	fResHistV->Fill(negRes.Y());
	//TODO: ONLY THE BEST MATCH!
	if(fPhys)
	  fPhysResMap[itr].push_back(negRes.Mod());
	else
	  fBkgResMap[itr].push_back(negRes.Mod());
	
	if(negRes.Mod() > fMatchDistance)
	  continue;
	if(negRes.Mod() < minRes.Mod()) {
	  minRes = negRes;
	  bestMatch = ihit;
	}
      }
      if(bestMatch > -1)
	tempCand.push_back(conMap[((*it->second)[bestMatch])]);
    } //end loop over all planes
    

    
    // std::cout<<"SECOND LOOP: ----------------------------------------------"<<std::endl;
        
    // //loop over planes again and select the hits inside the roadwidth:
    // TVector3 position;
    // for (it=recoHitMap.begin(); it!=recoHitMap.end(); it++) { 
    //   TVector2 pP = penetrationMap[it->first];
    //   //fResHistU->Fill(pP.X());
    //   //fResHistV->Fill(pP.Y());
    //   double minRes = 100;
    //   GFAbsRecoHit* hitToAdd=NULL;  //only one per plane
    //   GFDetPlane pl = it->first.getPlane();
    //   //pl.Print();
    //   TMatrixT<double> raw2(2,1);
    //   //loop over all hits of that plane
    //   for(unsigned int iHit=0; iHit<it->second.size(); iHit++) {
    // 	GFAbsRecoHit* hit = it->second[iHit];
	
    // 	//try this as the good method fails for some reason ..
    // 	TMatrixT<double> statePred2(5,1);
    // 	TMatrixT<double> covPred2(5,5);
    // 	bool ok=true;
    // 	try {
    // 	  rep->extrapolate(pl,statePred2,covPred2);
    // 	  raw2 = hit->residualVector(rep,statePred2,pl);
    // 	}
    // 	catch(GFException& ex) {
    // 	  ok=false;
    // 	}
	
    // 	//raw = hit->getHitCoord(pl);
    // 	//TVector2 penetrationUV = pP; 
    // 	//fResHistU->Fill(pP.X());
    // 	//fResHistV->Fill(pP.Y());
    // 	//TVector2 posUV(raw[0][0],raw[1][0]);
    // 	//TVector2 res = posUV-pP;
    // 	TVector2 res(raw2[0][0], raw2[1][0]);
    // 	//TVector2 res(0.,0.);
    // 	//Fill Histo
    // 	fResHistU->Fill(res.X());
    // 	fResHistV->Fill(res.Y());
    // 	//fResHistZ->Fill(res.Z());
    // 	if(res.Mod() > fMatchDistance)
    // 	  continue;
    // 	if(res.Mod() < minRes)
    // 	  hitToAdd = hit;
    //   }
    //   if(hitToAdd!=NULL)
    // 	tempCand.push_back(hitToAdd);
    // }
    
    
    std::cout<<"======= TempCand from found MVD pixel/strip hits has size "<<
      tempCand.size()<<" =======\n"<<std::endl;

    if(tempCand.size()<fMinMVDHits)
      continue;  //do not write an output track
    
    
    std::sort(tempCand.begin(), tempCand.end(), sortByR);
    std::vector<GFAbsRecoHit*> cand;
    for(unsigned int i=0; i<tempCand.size(); i++) 
      cand.push_back(backMap[(tempCand[i])]);
      
    //create MVD hit candidate
    GFTrackCand* mvdCand = new GFTrackCand();
    for(unsigned int im=0; im<cand.size(); im++) {
      GFAbsRecoHit* hit = cand[im];
      TString bid = idMap[hit].first;
      unsigned int hitid = idMap[hit].second;
      mvdCand->addHit(ioman->GetBranchId(bid), //detId enum
  		      hitid); //hit in array
    }
    
    int trackID = track->getCand().getMcTrackId();
    mvdCand->setMcTrackId(trackID);
    
    // copy track and set new candidate
    GFTrack* outTrack = new GFTrack(*track);
    outTrack->clearBookkeeping();
    outTrack->setCandidate(*mvdCand);
        
    // merge with MVD hits
    GFTrack* tmpTrack = new GFTrack(*track);
    tmpTrack->clearBookkeeping();
    outTrack->mergeHits(tmpTrack);
    outTrack->blowUpCovs(500.);
    (*fOutTrackArray)[fOutTrackArray->GetEntriesFast()] = outTrack;
    delete tmpTrack;
  } //end loop over tracks

  //analize purities
  if(fScan && fPhysResMap.size() >= fNPhys) {
    for(unsigned int i=0; i<fScanSteps; i++){  //loop over roadwidth intervals
      unsigned int nPhys = 0;
      unsigned int nBkg = 0;
      double cut = fScanMin+(i*fInterval);
      std::map<unsigned int, std::vector<double> >::const_iterator it;
      //loop over physics map of residuals
      for(it = fPhysResMap.begin(); it!=fPhysResMap.end(); it++) {
	unsigned int nInside = 0;
	for(unsigned int nR = 0; nR<it->second.size(); nR++) 
	  if((it->second)[nR] <= cut)
	    nInside++;
	if(nInside>=fMinMVDHits)
	  nPhys++;
      } //end loop over physics residuals
      
      //loop over bkg map of residuals
      for(it = fBkgResMap.begin(); it!=fBkgResMap.end(); it++) {
	unsigned int nInside = 0;
	for(unsigned int nR = 0; nR<it->second.size(); nR++) 
	  if((it->second)[nR] <= cut)
	    nInside++;
	if(nInside>=fMinMVDHits)
	  nBkg++;
      } //end loop over bkg residuals
      
      double purity;
      if(nPhys+nBkg==0) //should never happen
	purity = 0;
      else
	purity = ((double)nPhys)/((double)nPhys+nBkg);
      fGlobalPurities[i].push_back(purity);

    }//end loop over roadwidth intervals
  }

  //clean up
  std::map<DetPlaneWrapper, std::vector<GFAbsRecoHit*>* >::iterator it;
  for(it = recoHitMap.begin(); it!=recoHitMap.end(); it++) 
    delete it->second;
  
  // std::cout <<"### Found "<< fOutTrackArray->GetEntries() << " tracks with MVD correlations." << std::endl; 
  
  return;
}

void
PndTpcMVDCorrelatorTask::WriteHistograms(const TString& fname) {
  TFile* rOut = new TFile(fname, "recreate");
  rOut->cd();
  fResHistU->Write();
  fResHistV->Write();
  //fResHistZ->Write();

  if(fScan) {
    for(unsigned int is=0; is<fScanSteps; is++) {
      double cut = is*fInterval + fScanMin;
      std::vector<double> purities = fGlobalPurities[is];
      double pur=0.;
      for(unsigned int ip=0; ip<purities.size(); ip++) 
	pur+=purities[ip];
      pur = pur/((double)purities.size());
      fPurityGraph->SetPoint(is,cut,pur);
    }
    fPurityGraph->SetName("purityVsRoadWidth");
    fPurityGraph->SetTitle("");
    fPurityGraph->GetHistogram()->GetXaxis()->SetTitle("Correlation roadwith (cm)");
    fPurityGraph->GetHistogram()->GetYaxis()->SetTitle("Mean event purity");
    fPurityGraph->Write();
  }
  
  rOut->Close();
}
  

void
PndTpcMVDCorrelatorTask::SetScanStepping(unsigned int n, double min, double max) {
  fScanSteps = n;
  fScanMin = min;
  fScanMax = max;
  fInterval = (max-min)/((double) n);
  fScan = true;
  fPurityGraph = new TGraph(fScanSteps);
}
  



ClassImp(PndTpcMVDCorrelatorTask);
