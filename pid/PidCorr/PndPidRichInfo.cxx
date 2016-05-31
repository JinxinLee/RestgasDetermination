#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndTrackID.h"

#include "PndRichHit.h"
#include "PndRichBarPoint.h"
#include "PndRichResolution.h"

#include "FairTrackParH.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "TRandom.h"

#include <iomanip>

Bool_t PndPidCorrelator::GetRichInfo(FairTrackParH* helix, PndPidCandidate* pidCand)
{
  if(!helix)
    {
      std::cerr << "<Error> PndPidCorrelator::GetRichInfo: FairTrackParH NULL pointer parameter."<<std::endl;
      return kFALSE;
    }
  if(!pidCand)
    {
      std::cerr << "<Error> PndPidCorrelator::GetRichInfo: pidCand NULL pointer parameter."<<std::endl;
      return kFALSE;
    }
 
  if(helix->GetZ() <  fCorrPar->GetZLastPlane())
    {
      if (fVerbose>0) std::cout << "-W- PndPidCorrelator::GetRichInfo: Skipping tracks not reaching the last FTS layer" << std::endl;
      return kFALSE;
    }
  
  if(helix->GetPz() <= 0.)
    {
      std::cout << "-W- PndPidCorrelator::GetRichInfo: Skipping tracks going backward" << std::endl;
      return kFALSE;
    }

  Int_t richIndex = -1, richPhot = 0;
  Float_t richThetaC = -1000, richThetaCErr = 0, richGLength = -1000;
  Float_t richQuality = 1000000;
  Float_t chi2 = 0;
  Float_t richThetaC0 = -1000;
   
  TVector3 vertex(0., 0., -10000.);
  TVector3 momentum(0., 0., 0.);

  if (fGeanePro) // Overwrites vertex if Geane is used
   {
      // calculates track length from (0,0,0) to last point
      fGeanePropagator->PropagateToVolume("RichAerogelSensor",0,1);
      fGeanePropagator->SetPoint(TVector3(0,0,0));
      FairTrackParH *fRes= new FairTrackParH();
      Bool_t rc =  fGeanePropagator->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge());
      if (!rc) return kFALSE; 
      //richGLength = fGeanePropagator->GetLengthAtPCA();
      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ()); 
      momentum.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());
   }
   
   if ( fIdeal || (fRichMode==3) ) // without reconstruction, from simulated track parameters
   {
      PndRichBarPoint *richHit = NULL;
      Int_t richEntries = fRichBarPoint->GetEntriesFast();
      TVector3 richPos(0., 0., 0.);
      TVector3 richMom(0., 0., 0.);
      Double_t richMass, richBeta;
   
      for (Int_t dd = 0; dd<richEntries; dd++)
      {
         // 
         richHit = (PndRichBarPoint*)fRichBarPoint->At(dd); 
         if ( richHit->GetTrackID() != pidCand->GetMcIndex() ) continue;
         
         richHit->Position(richPos);
         richHit->Momentum(richMom);
         richMass = richHit->GetMass();
         richBeta = richMom.Mag()/sqrt(richMom.Mag()*richMom.Mag()+richMass*richMass);

         // 
         dbpoint pnt;
         pnt.mass = richMass;
         pnt.beta = richBeta;
         pnt.x = vertex.X();
         pnt.y = vertex.Y();
         pnt.theta = momentum.Theta();
         pnt.phi = momentum.Phi();

         Float_t dX = vertex.X()-richPos.X();
         Float_t dY = vertex.Y()-richPos.Y();
         Float_t dist = dX*dX+dY*dY;
         Double_t eff = fRichResolution->Efficiency(pnt); // efficiency of reconstruction
         if ((richQuality > dist) && (gRandom->Uniform()<eff))
         {
            richIndex = dd;
            richQuality = dist;
            richThetaCErr = fRichResolution->Sigma(pnt); // sigma of beta
            // inside richThetaC is value of beta
            richThetaC0 = richHit->GetThetaC();
            richThetaC = gRandom->Gaus(richHit->GetThetaC(),richThetaCErr); 
            richPhot = 0;
         }
         if (fDebugMode)
         {
            Float_t ntuple[] = {static_cast<Float_t>(vertex.X()), static_cast<Float_t>(vertex.Y()), static_cast<Float_t>(vertex.Z()), static_cast<Float_t>(vertex.Phi()), 
               static_cast<Float_t>(helix->GetMomentum().Mag()), static_cast<Float_t>(helix->GetQ()), static_cast<Float_t>(helix->GetMomentum().Theta()), static_cast<Float_t>(helix->GetZ()),
               static_cast<Float_t>(richPos.X()), static_cast<Float_t>(richPos.Y()), static_cast<Float_t>(richPos.Phi()),
               dist, static_cast<Float_t>(richHit->GetThetaC()), 0., static_cast<Float_t>(vertex.DeltaPhi(richPos)), richGLength, 
               static_cast<Float_t>(pidCand->GetFitStatus())
            };
            // Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex_z, vertex.Phi(),  
            // 		      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
            // 		      richPos.X(), richPos.Y(), richPos.Phi(), 
            // 		      dist, richHit->GetThetaC(), 0., vertex.DeltaPhi(drcPos), richGLength,
            // 		      pidCand->GetFitStatus()
            // };
            richCorr->Fill(ntuple);
         }
      }
      richQuality = std::sqrt(richQuality);
   }
   if ( fRichMode == 2 ) // full reconstruction
   {
      // reconstruction returns value of beta and sigma of beta
      // richQuality   - chi2
      // richThetaC    - beta
      // richThetaCErr - sigma of beta
      // richPhot      - number of cherenkov photons
      //fRichReco = new PndRichReco();
      Float_t ts = 21.8; // time of hit to the aerogel [ns]
      fRichReco->RichFullReconstruction(vertex,momentum.Unit(),ts,
                                        richQuality,richThetaC,richThetaCErr,richPhot);
      richThetaC0 = richThetaC;
      dbpoint pnt;
      pnt.mass = 0.1056583715;
      pnt.beta = richThetaC;
      pnt.x = vertex.X();
      pnt.y = vertex.Y();
      pnt.theta = momentum.Theta();
      pnt.phi = momentum.Phi();
      pnt.beta += fRichResolution->Shift(pnt);
      
      richThetaCErr = fRichResolution->Sigma(pnt);
      richThetaC += fRichResolution->Shift(pnt);
      richIndex = 1;
   }
   
  if (((richPhot>=3) || (fIdeal) || (fRichMode==3)) && (richIndex!=-1))
    {
      pidCand->SetRichQuality(richQuality);
      pidCand->SetRichThetaC(richThetaC);
      pidCand->SetRichThetaCErr(richThetaCErr);
      pidCand->SetRichNumberOfPhotons(richPhot);
      pidCand->SetRichIndex(richIndex);
    }
   
  return kTRUE;
}
