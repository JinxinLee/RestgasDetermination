#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndTrackID.h"

#include "PndFtofHit.h"
#include "PndFtofPoint.h"

#include "FairTrackParH.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

Bool_t PndPidCorrelator::GetFtofInfo(FairTrackParH* helix, PndPidCandidate* pidCand)
{
  if(!helix)
    {
      std::cerr << "<Error> PndPidCorrelator::GetFtofInfo: FairTrackParH NULL pointer parameter."<<std::endl;
      return kFALSE;
    }
  if(!pidCand)
    {
      std::cerr << "<Error> PndPidCorrelator::GetFtofInfo: pidCand NULL pointer parameter."<<std::endl;
      return kFALSE;
    }
 
  if(helix->GetZ() <  fCorrPar->GetZLastPlane())
    {
      if (fVerbose>0) std::cout << "-W- PndPidCorrelator::GetFtofInfo: Skipping tracks not reaching the last FTS layer" << std::endl;
      return kFALSE;
    }
  
  if(helix->GetPz() <= 0.)
    {
      std::cout << "-W- PndPidCorrelator::GetFtofInfo: Skipping tracks going backward" << std::endl;
      return kFALSE;
    }
 
  FairGeanePro *fProTof = new FairGeanePro();
  if (!fCorrErrorProp) fProTof->PropagateOnlyParameters(); 
  FairGeanePro *fProVertex = new FairGeanePro();
  fProVertex->PropagateOnlyParameters();

  PndFtofHit *tofHit = NULL;
  Int_t tofEntries = fFtofHit->GetEntriesFast();
  Int_t tofIndex = -1;
  Float_t tofTof = 0., tofLength = -1000, tofGLength = -1000, tofTrackLength = -1000;;
  Float_t tofQuality = 1000000;

  Float_t chi2 = 0;
  TVector3 vertex(0., 0., -10000.);
  TVector3 vertexrec(0., 0., -10000.); 
  TVector3 momrec(0., 0., -10000.);
  TVector3 tofPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);

  if (fGeanePro) // Overwrites vertex if Geane is used
    {
      // calculates track length from (0,0,0) to last point
      fProVertex->SetPoint(TVector3(0,0,0));
      fProVertex->PropagateToPCA(1, -1);
      FairTrackParH *fRes= new FairTrackParH();
      Bool_t rc =  fProVertex->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge());
      if (rc) 
	{
	  tofTrackLength = fProVertex->GetLengthAtPCA(); 
	  vertexrec.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ()); 
	  momrec.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());
	}
    }
  
  for (Int_t tt = 0; tt<tofEntries; tt++)
    {
      tofHit = (PndFtofHit*)fFtofHit->At(tt);
      if ( fIdeal && ( ((PndFtofPoint*)fFtofPoint->At(tofHit->GetRefIndex()))->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      tofHit->Position(tofPos);

      Float_t propX = helix->GetX() + (fCorrPar->GetFtofZ() - helix->GetZ()) * helix->GetPx() / helix->GetPz();
      Float_t propY = helix->GetY() + (fCorrPar->GetFtofZ() - helix->GetZ()) * helix->GetPy() / helix->GetPz();
      Float_t propZ = fCorrPar->GetFtofZ();
      vertex.SetXYZ(propX, propY, propZ);
      tofGLength = (vertex-helix->GetPosition()).Mag();
      
      if (fGeanePro) // Overwrites vertex if Geane is used
	{ 
	  fProTof->SetPoint(tofPos);
	  fProTof->PropagateToPCA(1, 1);
	  FairTrackParH *fRes= new FairTrackParH();
	  Bool_t rc =  fProTof->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge());	
	  if (!rc) continue;
	  tofGLength = fProTof->GetLengthAtPCA(); 
      	  vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	}
      
      Float_t dist = (tofPos-vertex).Mag2();
      
      if ( tofQuality > dist)
        {
          tofIndex = tt;
          tofQuality = dist;
          tofTof = tofHit->GetTime(); 
	  tofLength = tofTrackLength+tofGLength;
	}
      
      if (fDebugMode)
        {
          Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(),
			      vertexrec.X(), vertexrec.Y(), vertexrec.Z(), 
			      momrec.X(), momrec.Y(), momrec.Z(),
                              helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
                              tofPos.X(), tofPos.Y(), tofPos.Z(),
                              dist, tofLength, tofGLength,tofTrackLength};
          ftofCorr->Fill(ntuple);
        }
    }

  if ( (tofQuality<fCorrPar->GetFTofCut()) || (fIdeal && tofIndex!=-1) )
    {
      pidCand->SetTofQuality(tofQuality);
      pidCand->SetTofStopTime(tofTof);
      pidCand->SetTofTrackLength(tofLength);
      pidCand->SetTofIndex(tofIndex);
      if (tofLength>0.)
	{
	  // mass^2 = p^2 * ( 1/beta^2 - 1 )
	  Float_t mass2 = helix->GetMomentum().Mag()*helix->GetMomentum().Mag()*(30.*30.*tofTof*tofTof/tofLength/tofLength-1.);
	  pidCand->SetTofM2(mass2);
	}
    }
  
  return kTRUE;
}
