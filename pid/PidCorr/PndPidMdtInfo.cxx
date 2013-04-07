#include "PndPidCorrelator.h"
#include "PndMdtHit.h"
#include "PndMdtPoint.h"

#include <cmath>

//_________________________________________________________________
Bool_t PndPidCorrelator::MdtMapping() {
  
  Int_t nHits = fMdtHit->GetEntriesFast();
  if (nHits==0) return kFALSE;
  
  PndMdtHit *mdtHit = NULL;
  for (Int_t iHit=0; iHit<nHits; iHit++) 
    {  
      mdtHit  = (PndMdtHit*) fMdtHit->At(iHit);
      Int_t mdtModule = -1, mdtLayer = -1;
      //std::cout << mdtHit->GetModule() << "\t" << mdtHit->GetLayerID() << "\t" << iHit << std::endl;
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
	  std::cout << "-E- PndPidCorrelator::MdtMapping: Wrong MDT Module" << std::endl;
	  return kFALSE;
	}
    }
  
  return kTRUE;
}

//_________________________________________________________________
Bool_t PndPidCorrelator::MdtGeometry()
{
  //fVerbose = 3;
  // Thichness of barrel iron layers [cm]
  mdtIronThickness[0][0] = 6.;
  for (Int_t ll=1; ll<12;ll++)    mdtIronThickness[0][ll] = 3;
  mdtIronThickness[0][12] = 6.;
  // Thichness of endcap+mf iron layers [cm]
  for (Int_t ll=0; ll<10;ll++)    mdtIronThickness[1][ll] = 6;
  // Thichness of forward iron layers [cm]
  for (Int_t ll=0; ll<17;ll++)    mdtIronThickness[2][ll] = 6;
  
  Text_t buffer[250]; 
  
  for (Int_t ll=0; ll<5; ll++)
    {
      sprintf(buffer,"cave_1/Mdt_1/MdtEndcap_1/MdtEndcapLayer0%i_1/MDT2s0l%ib0w0_%i",ll,ll,200+(8*ll));     
      gGeoManager->cd(buffer);
      Double_t local[3] = {0., 0., 0.};
      Double_t master[3];
      gGeoManager->LocalToMaster(local, master);
      mdtLayerPos[1][ll] = master[2];
      if (fVerbose>1)	std::cout <<  "mdtLayerPos[1][" << ll << "] = " << mdtLayerPos[1][ll] << ";" << std::endl;
    }
  for (Int_t ll=0; ll<6; ll++)
    {
      sprintf(buffer,"cave_1/Mdt_1/MdtMuonFilter_1/MdtMuonFilterLayer0%i_1/MDT3s0l%ib0w0_%i",ll,ll,300+(8*ll));     
      gGeoManager->cd(buffer);
      Double_t local[3] = {0., 0., 0.};
      Double_t master[3];
      gGeoManager->LocalToMaster(local, master);
      mdtLayerPos[1][ll+5] = master[2];
      if (fVerbose>1)	std::cout <<  "mdtLayerPos[1][" << ll+5 << "] = " << mdtLayerPos[1][ll+5] << ";" << std::endl;
    }
  for (Int_t ll=0; ll<17; ll++)
    {
      sprintf(buffer,"cave_1/Mdt_1/MdtForward_1/MDT4s0l%ib0w0_%i",ll,ll);     
      gGeoManager->cd(buffer);
      Double_t local[3] = {0., 0., 0.};
      Double_t master[3];
      gGeoManager->LocalToMaster(local, master);
      mdtLayerPos[2][ll] = master[2];
      if (fVerbose>1)  std::cout <<  "mdtLayerPos[2][" << ll << "] = " << mdtLayerPos[2][ll] << ";" << std::endl;
    }
 
  return kTRUE;
}

//_________________________________________________________________
Bool_t PndPidCorrelator::GetMdt2Info(FairTrackParH* helix, PndPidCandidate* pidCand) {
  
  return kTRUE;
}

//_________________________________________________________________
Bool_t PndPidCorrelator::GetFMdtInfo(FairTrackParP* helix, PndPidCandidate* pidCand) {
  
  if(!helix)
    {
      std::cerr << "<Error> PndPidCorrelator::GetFMdtInfo: FairTrackParH NULL pointer parameter."<<std::endl;
      return kFALSE;
    }
  if(!pidCand)
    {
      std::cerr << "<Error> PndPidCorrelator::GetFMdtInfo: pidCand NULL pointer parameter."<<std::endl;
      return kFALSE;
    }
  
  if(helix->GetZ() <  fCorrPar->GetZLastPlane())
    {
      if (fVerbose>0) std::cout << "-W- PndPidCorrelator::GetFMdtInfo: Skipping tracks not reaching the last FTS layer" << std::endl;
      return kFALSE;
    }
  
  if(helix->GetPz() <= 0.)
    {
      std::cout << "-W- PndPidCorrelator::GetFMdtInfo: Skipping tracks going backward" << std::endl;
      return kFALSE;
    }

  FairGeanePro *fProMdt = new FairGeanePro();
  if (!fCorrErrorProp) fProMdt->PropagateOnlyParameters(); 
  
  PndMdtHit *mdtHit = NULL;
  
  Int_t mdtIndex = -1;
  Float_t mdtGLength = -1000;
  Float_t mdtQuality = 1000000;

  Float_t chi2 = 0;
  TVector3 mdtPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  TVector3 vertex(0., 0., 0.);
  TVector3 vertexD(0., 0., 0.);
  Float_t propZ = mdtLayerPos[2][0] ; 
  Float_t propX = helix->GetX() + (propZ - helix->GetZ()) * helix->GetPx() / helix->GetPz();
  Float_t propY = helix->GetY() + (propZ - helix->GetZ()) * helix->GetPy() / helix->GetPz();
 
  vertex.SetXYZ(propX, propY, propZ);
  mdtGLength = (vertex-helix->GetPosition()).Mag();
  
  if (fGeanePro && !fIdeal) // Overwrites vertex if Geane is used
    { 
      TVector3 jj(1,0,0), kk(0,1,0);
      fProMdt->PropagateFromPlane(jj, kk);
      fProMdt->PropagateToPlane(vertex, jj, kk);
      //fProMdt->PropagateToVolume("MDT4s0l0b0w0",0,1);
      FairTrackParP *fRes= new FairTrackParP();
      Bool_t rc =  fProMdt->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge());	
      if (rc)
	{
	  mdtGLength = fProMdt->GetLengthAtPCA(); 
	  vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	  momentum.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());
	  vertexD.SetXYZ(fRes->GetDX(), fRes->GetDY(), fRes->GetDZ());
	}    
      else
	{
	  std::cout << "-W- PndPidCorrelator::GetFMdtInfo: Propagation failed" << std::endl;
	}
    }
  
  vector<Int_t>vecMdt0 = mapMdtForward[0]; // hit in layer0
  
  for (Int_t mm0 = 0; mm0< vecMdt0.size(); mm0++)
    {
      mdtHit = (PndMdtHit*)fMdtHit->At(vecMdt0[mm0]);
      if ( fIdeal && ( ((PndMdtPoint*)fMdtPoint->At(mdtHit->GetRefIndex()))->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      mdtHit->Position(mdtPos);
      Float_t dist = (mdtPos-vertex).Mag2();
      
      if ( mdtQuality > dist)
        {
          mdtIndex = mm0;
          mdtQuality = dist;
	}
      Int_t mdtLayer = 0, mdtHits = 1;
      if (fDebugMode)
        {
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(),
			      vertexD.X(), vertexD.Y(), vertexD.Z(),
			      vertex.Phi(), 
			      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
			      mdtPos.X(), mdtPos.Y(), mdtPos.Z(), mdtPos.Phi(), momentum.Mag(),
			      dist, mdtHit->GetModule(), vertex.DeltaPhi(mdtPos), mdtGLength, mdtLayer, mdtHits};
          mdtCorr->Fill(ntuple);
	}
    }

  if ( (mdtQuality<fCorrPar->GetMdtCut()) || (fIdeal && mdtIndex!=-1) )
    {
      
      pidCand->SetMuoQuality(mdtQuality);
      pidCand->SetMuoIndex(mdtIndex);
      pidCand->SetMuoModule(4);
      pidCand->SetMuoMomentumIn(momentum.Mag());
    } 
  
  return kTRUE;
}

ClassImp(PndPidCorrelator)
