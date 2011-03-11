#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "PndTpcCluster.h"

#include "FairTrackParH.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "TObjArray.h"
#include "TVector3.h"
#include "TGeoManager.h"

#include <cmath>

#include "PndPidCorrelator.h"

//_________________________________________________________________
Bool_t PndPidCorrelator::GetTpcInfo(PndTrack* track, PndPidCandidate* pidCand) 
{
  std::vector<Double_t> dedxvec;
  dedxvec.clear();

  Int_t tpcCounts = 0;
  PndTrackCand trackCand = track->GetTrackCand();
  for (Int_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
      if (candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("PndTpcCluster")) continue;
     
      // Central cluster
      PndTpcCluster * cluCenter = (PndTpcCluster*)fTpcCluster->At(candHit.GetHitId());
      if (cluCenter==0) continue;
      TVector3 vecCenter = cluCenter->pos();
      Double_t energy = cluCenter->amp();
      Double_t dxBefore =-1, dxAfter = -1., dx = 0., dedx = 0.;

      // previous Cluster
      if (ii>0)
	{ // "is the first hit?"
	  PndTrackCandHit candHitBef = trackCand.GetSortedHit(ii-1);
	  if (candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("PndTpcCluster"))
	    {  // "is the previous hit a tpc cluster?"
	      PndTpcCluster *cluBefore = (PndTpcCluster*)fTpcCluster->At(candHitBef.GetHitId()); 
	      if (cluBefore!=0)  
		{ // "is the previous cluster existing?"
		  TVector3 vecBefore =  cluBefore->pos();
		  dxBefore = 0.5* (vecCenter - vecBefore).Mag();
		  dx += dxBefore;
		} // end of "is the previous cluster existing?"
	    } // end of "is the previous hit a tpc cluster?"
	} // end of "is the first hit?"
      
      // next Cluster 
      if (ii<(trackCand.GetNHits()-1))
	{ // "is the last hit?"
	  PndTrackCandHit candHitAft = trackCand.GetSortedHit(ii+1);
	  if (candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("PndTpcCluster"))
	    {  // "is the next hit a tpc cluster?"
	      PndTpcCluster *cluAfter = (PndTpcCluster*)fTpcCluster->At(candHitAft.GetHitId()); 
	      if (cluAfter!=0)  
		{ // "is the next cluster existing?"
		  TVector3 vecAfter =  cluAfter->pos();
		  dxAfter = 0.5* (vecCenter - vecAfter).Mag();
		  dx += dxAfter;
		} // end of "is the next cluster existing?"
	    } // end of "is the next hit a tpc cluster?"
	} // end of "is the last hit?"
      
      if ((dxBefore<0.) || (dxAfter<0.))
	{
	  dx = 2.*dx;
	}
      if (dx>0.)
	{
	  dedx = energy/dx;
	}
      
      if (fVerbose>2) std::cout << "Energy: " << energy << "\tBefore: " << dxBefore << "\tAfter: " << dxAfter << "\tds: " << dx << "\tdedx: " << dedx << std::endl;
      if (dedx>0.)
	{
	  dedxvec.push_back(dedx);
	  tpcCounts++;
	}
    } // end of TrackCand loop

  if( tpcCounts > 0) {
    // truncated mean
    Double_t perc = 0.60;
    // sort
    std::sort(dedxvec.begin(), dedxvec.end());
    
    //truncated mean
    Double_t sum = 0;
    Int_t endnum = int(floor(tpcCounts * perc));
    
    // ****************************************
    // CUT on n of hits: to have a meaningful
    // truncated mean we require not to have less 
    // than 5 hits (in the already truncated list)
    // ****************************************
    if(endnum > 5) {
      for(Int_t m = 0; m < endnum; m++)
	{
	  sum += dedxvec[m];
	}
      if (fVerbose>2) std::cout << "TPC DE/DX: " << (sum/(Double_t) endnum) << std::endl;
      pidCand->SetTpcMeanDEDX(sum/(Double_t) endnum); // else default in pidCand is TpcDEDXMean = 0
    }
  } 
  
  pidCand->SetTpcHits(tpcCounts);
}

//_________________________________________________________________
Bool_t PndPidCorrelator::GetTpcInfoGeane(PndTrack* track, PndPidCandidate* pidCand) 
{
  std::vector<Double_t> dedxvec;
  dedxvec.clear();
  FairGeanePro *fProTpc = new FairGeanePro();
  if (!fCorrErrorProp) fProTpc->PropagateOnlyParameters();
  
  FairTrackParP par = track->GetParamFirst();
  Int_t ierr = 0;
  FairTrackParH *helix = new FairTrackParH(&par, ierr);
  FairTrackParH *fRes= new FairTrackParH();
  Double_t dxBefore =-1, dxAfter = -1.;
  Bool_t first = kTRUE;
  Int_t tpcCounts = 0;
  PndTrackCand trackCand = track->GetTrackCand();
  for (Int_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
      if (candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("PndTpcCluster")) continue;
     
      // Central cluster
      PndTpcCluster * cluCenter = (PndTpcCluster*)fTpcCluster->At(candHit.GetHitId());
      if (cluCenter==0) continue;
      TVector3 vecCenter = cluCenter->pos();
      Double_t energy = cluCenter->amp();
      Double_t dx = 0., dedx = 0.;
     
      fProTpc->SetPoint(vecCenter);
      fProTpc->PropagateToPCA(1, 1);  
      if (first) Bool_t rc =  fProTpc->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge()); // First propagation at module
      first = kFALSE;
      
      if (ii<(trackCand.GetNHits()-1))
	{ // "is the last hit?"
	  PndTrackCandHit candHitAft = trackCand.GetSortedHit(ii+1);
	  if (candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("PndTpcCluster"))
	    {  // "is the next hit a tpc cluster?"
	      PndTpcCluster *cluAfter = (PndTpcCluster*)fTpcCluster->At(candHitAft.GetHitId()); 
	      if (cluAfter!=0)  
		{ 		  
		  // "is the next cluster existing?"
		  TVector3 vecAfter =  cluAfter->pos();
		  fProTpc->SetPoint(vecAfter);
		  fProTpc->PropagateToPCA(1, 1);
		  Bool_t rc2 =  fProTpc->Propagate(fRes, helix, fPidHyp*pidCand->GetCharge()); // First propagation at module
		  dxAfter = 0.5*fProTpc->GetLengthAtPCA();
		  dx += dxAfter;
		} // end of "is the next cluster existing?"
	    } // end of "is the next hit a tpc cluster?"
	} // end of "is the last hit?"
      
      
      if ((dxBefore<0.) || (dxAfter<0.))
	{
	  dx = 2.*dx;
	}
      if (dx>0.)
	{
	  dedx = energy/dx;
	}
      
      if (fVerbose>2) std::cout << "Energy: " << energy << "\tBefore: " << dxBefore << "\tAfter: " << dxAfter << "\tds: " << dx << "\tdedx: " << dedx << std::endl;
      if (dedx>0.)
	{
	  dedxvec.push_back(dedx);
	  tpcCounts++;
	}
      
      dxBefore = dxAfter;
	
    } // end of TrackCand loop

  if( tpcCounts > 0) {
    // truncated mean
    Double_t perc = 0.60;
    // sort
    std::sort(dedxvec.begin(), dedxvec.end());
    
    //truncated mean
    Double_t sum = 0;
    Int_t endnum = int(floor(tpcCounts * perc));
    
    // ****************************************
    // CUT on n of hits: to have a meaningful
    // truncated mean we require not to have less 
    // than 5 hits (in the already truncated list)
    // ****************************************
    if(endnum > 5) {
      for(Int_t m = 0; m < endnum; m++)
	{
	  sum += dedxvec[m];
	}
      if (fVerbose>2) std::cout << "TPC DE/DX: " << (sum/(Double_t) endnum) << std::endl;
      pidCand->SetTpcMeanDEDX(sum/(Double_t) endnum); // else default in pidCand is TpcDEDXMean = 0
    }
  } 
  
  pidCand->SetTpcHits(tpcCounts);
}

ClassImp(PndPidCorrelator)
