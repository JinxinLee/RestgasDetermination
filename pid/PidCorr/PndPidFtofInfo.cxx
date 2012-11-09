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
      std::cout << "-W- PndPidCorrelator::GetFtofInfo: Skipping tracks not reaching the last FTS layer" << std::endl;
      return kFALSE;
    }
  
  if(helix->GetPz() <= 0.)
    {
      std::cout << "-W- PndPidCorrelator::GetFtofInfo: Skipping tracks going backward" << std::endl;
      return kFALSE;
    }

  FairGeanePro *fProVertex = new FairGeanePro();
  if (!fCorrErrorProp) fProVertex->PropagateOnlyParameters();

  PndFtofHit *tofHit = NULL;
  Int_t tofEntries = fFtofHit->GetEntriesFast();
  Int_t tofIndex = -1;
  Float_t tofTof = 0., tofLength = -1000, tofGLength = -1000;
  Float_t tofQuality = 1000000;

  Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.);
  TVector3 tofPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t tt = 0; tt<tofEntries; tt++)
    {
      tofHit = (PndFtofHit*)fFtofHit->At(tt);
      if ( fIdeal && ( ((PndFtofPoint*)fFtofPoint->At(tofHit->GetRefIndex()))->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      tofHit->Position(tofPos);

      Float_t propX = helix->GetX() + (fCorrPar->GetFtofZ() - helix->GetZ()) * helix->GetPx() / helix->GetPz();
      Float_t propY = helix->GetY() + (fCorrPar->GetFtofZ() - helix->GetZ()) * helix->GetPy() / helix->GetPz();
      Float_t propZ = fCorrPar->GetFtofZ();
      vertex.SetXYZ(propX, propY, propZ);

      if (fGeanePro) // Overwrites vertex if Geane is used
        {
          fProVertex->SetPoint(TVector3(0,0,0));
          fProVertex->PropagateToPCA(1, -1);
          FairTrackParH *fRes= new FairTrackParH();
          Bool_t rc =  fProVertex->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge());
          if (rc) tofLength = fProVertex->GetLengthAtPCA();
        }
    
      Float_t dist = (tofPos-vertex).Mag2();
      tofGLength = (vertex-helix->GetPosition()).Mag();
      tofLength += tofGLength;

      if ( tofQuality > dist)
        {
          tofIndex = tt;
          tofQuality = dist;
          tofTof = tofHit->GetTime();
	  tofGLength = (vertex-helix->GetPosition()).Mag();
	}
      
      if (fDebugMode)
        {
          Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),
                              helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
                              tofPos.X(), tofPos.Y(), tofPos.Z(), tofPos.Phi(),
                              dist, vertex.DeltaPhi(tofPos), tofLength, tofGLength};
          ftofCorr->Fill(ntuple);
        }
    }

  if ( (tofQuality<fCorrPar->GetTofCut()) || (fIdeal && tofIndex!=-1) )
    {
      pidCand->SetTofQuality(tofQuality);
      pidCand->SetTofStopTime(tofTof);
      pidCand->SetTofTrackLength(tofLength);
      pidCand->SetTofIndex(tofIndex);
    }


  return kTRUE;
}
