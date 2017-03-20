// Task for the exercise 2 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairGeaneTrT.h"
#include "TGeant3TGeo.h"
#include "TGeant3.h"
#include "TVector3.h"
//#include "CbmPlanePoint.h"
#include "TTree.h"
#include "TDatabasePDG.h"
#include "PndSttTrack.h"
#include "PndSttHit.h"
#include "PndSttPoint.h"
#include "DetPlane.h"
#include "FairRunAna.h"
#include "PndSttHelixTrackFitter.h"
#include "FairTrackParH.h"
#include "FairTrackParP.h"
#include "TCanvas.h"
#include "TH1F.h"

#include <iostream>

using namespace std;
// TCanvas *can2;
// TH1F *dstvtx, *momvtx;

// -----   Default constructor   -------------------------------------------
FairGeaneTrT::FairGeaneTrT() :
  FairTask("Test") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
FairGeaneTrT::~FairGeaneTrT() { }
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus FairGeaneTrT::Init() {
 
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- FairGeaneTrT::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
 //  can2 = new TCanvas();
//   can2->Divide(1,2);
//   dstvtx = new TH1F("dstvtx","dstvtx",100,0,10);
//   momvtx = new TH1F("momvtx","momvtx",100,0,0.5);


  fTrackParGeane = new TClonesArray("FairTrackParP");
  ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);
  
  fTrackParIni = new TClonesArray("FairTrackParP");
  ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);
  
  fTrackParFinal = new TClonesArray("FairTrackParP");
  ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);
  
  // Get hit Array
  fHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  if (!fHitArray)   cout << "-W- FairGeaneTrT::Init: No Hit array!" << endl;
  // Get point Array
  fPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if (!fPointArray) cout << "-W- FairGeaneTrT::Init: No Point array!" << endl;
 
  // Get SttTrack array
  fTrackArray  = (TClonesArray*) ioman->GetObject("STTTrack"); 
  if (!fTrackArray) {
    cout << "-E- FairGeaneTrT::Init: No SttTrack array!" << endl;
    return kERROR;
  }

  fPro = new FairGeanePro();
  fUtil = new FairGeaneUtil();
  //  fPro->PropagateToVolume("mL3",0,1);

  return kSUCCESS;

}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void FairGeaneTrT::Exec(Option_t*) {
  //	cout << "FairGeaneTrT::Exec" << endl;
   fTrackParGeane->Delete();
   fTrackParIni->Delete();
   fTrackParFinal->Delete();

   if (!fTrackArray ) return;

  
   Int_t nTracks = fTrackArray->GetEntriesFast();

   for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) 
     {

       PndSttTrack* pTrack = (PndSttTrack*) fTrackArray->At(iTrack);
       if(!pTrack) continue;
       if(pTrack->GetFlag() < 3) continue; // per ora voglio solo le tracce perfette
       
       
       Int_t iHit1 = pTrack->GetHitIndex(0);
       PndSttHit *currenthit1 = (PndSttHit*) fHitArray->At(iHit1);
       if(!currenthit1) continue;
       if(currenthit1->GetXint() == -999 || currenthit1->GetYint() == -999) continue;
       Int_t refindex1 = currenthit1->GetRefIndex(); 
       // get point
       PndSttPoint *fPoint1 = (PndSttPoint*) fPointArray->At(refindex1);
       
       TVector3 StartPos    = TVector3 (fPoint1->GetXtot(),fPoint1->GetYtot(),fPoint1->GetZtot());
       //       TVector3 StartPos    = TVector3(0,0,0);
       TVector3 StartPosErr = TVector3(0,0,0);
       TVector3 StartMom    = TVector3 (fPoint1->GetPXtot(),fPoint1->GetPYtot(),fPoint1->GetPZtot());
       //       TVector3 StartMom    = TVector3(1.,0.1,0.1);
       TVector3 StartMomErr = TVector3(0,0,0);
       
       
       // tracking from vertex ..........................
       // using prefit result from vertex
       // PndSttHelixTrackFitter *fFitter;
       //        TVector3 *vertex = new TVector3(0.,0.,0.); // CHECK to set the vertex in (0.,0.,0.)
       //        TVector3 *recovtx = fFitter->PCAToPoint(pTrack, vertex);
       //        TVector3 *momeatvtx;
       //        momeatvtx = fFitter->MomentumAtPoint(pTrack, recovtx);
       //        // ERRORS ON VERTEX FROM checkMomentumPCA.C
       //        // MOMENTUM 0.018, 0.018, 0.04
       //        // POSITION 0.007, 0.007, 0.46
       //        TVector3 StartPos    = *recovtx;
       //        TVector3 StartPosErr = TVector3(0.018,0.018,0.04); 
       //        TVector3 StartMom    = *momeatvtx;
       //        TVector3 StartMomErr = TVector3(0.007,0.007,0.46);
//        dstvtx->Fill(StartPos.Mag());
//        momvtx->Fill(StartMom.Mag() - 1.5);
//        if(StartPos.Mag() > 1.) continue; // cut on distance from vertex
//        if((StartMom.Mag() - 1.5) > 0.05) continue; // cut on momentum at vertex
       // ..................................................

       //tracking up to the last hit on track on the PCA to the wire
       Int_t hitcounter = pTrack->GetNofHits();
       Int_t iHit = pTrack->GetHitIndex(hitcounter - 1);
       PndSttHit *currenthit = (PndSttHit*) fHitArray->At(iHit);
       if(!currenthit) continue;
       if(currenthit->GetXint() == -999 || currenthit->GetYint() == -999) continue;
       Int_t refindex = currenthit->GetRefIndex(); 
       // get point
       PndSttPoint *iPoint = (PndSttPoint*) fPointArray->At(refindex);
       if(TMath::Sqrt(iPoint->GetXInLocal()*iPoint->GetXInLocal() + iPoint->GetYInLocal()*iPoint->GetYInLocal()) < 0.4999) continue;
      
       // out 
       // let' s take (approx!) the medium point 
       // position --> (posout - posin) /2
       // momentum --> (momout - momin) / 2
       TVector3 EndPos      = TVector3(iPoint->GetXtot(), iPoint->GetYtot(), iPoint->GetZtot());
       TVector3 EndPosErr   = TVector3(0,0,0);
       TVector3 EndMom      = TVector3(iPoint->GetPXtot(), iPoint->GetPYtot(), iPoint->GetPZtot());
       TVector3 EndMomErr   = TVector3(0,0,0);
       
       Int_t PDGCode= 13;
       TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
       TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
       Double_t  fCharge= fParticle->Charge();
       
       TClonesArray& clref1 = *fTrackParIni;
       Int_t size1 = clref1.GetEntriesFast();
       //       FairTrackParH *fStart = new (clref1[size1]) FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
       FairTrackParP *fStart = new (clref1[size1]) FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, TVector3(0.,0.,0.), TVector3(0.,1.,0.), TVector3(0.,0.,1.));
	
       TClonesArray& clref = *fTrackParGeane;
       Int_t size = clref.GetEntriesFast();
       FairTrackParP *fRes = new(clref[size]) FairTrackParP();
       
       TClonesArray& clref2 = *fTrackParFinal;
       Int_t size2 = clref2.GetEntriesFast();
       FairTrackParP *fFinal = new(clref2[size2]) FairTrackParP();
       FairTrackParH *fFinalH = new FairTrackParH(EndPos, EndMom, EndPosErr, EndMomErr, fCharge);
    
       // ----- propagation: using method propagate to closest ------
       TVector3 v0 = TVector3(0, 0, 0);
       fPro->SetPoint(v0);

       TVector3 center = TVector3(currenthit->GetX(), currenthit->GetY(), currenthit->GetZ());
       TVector3 wiredir = currenthit->GetWireDirection();
       wiredir *= 75.;
       TVector3 second = center + wiredir;
       TVector3 first = center - wiredir;

       fPro->SetWire(first, second);
       fPro->PropagateToVirtualPlaneAtPCA(2);   // 1 if point; 2 if wire

       Bool_t rc = fPro->Propagate(fStart, fRes, PDGCode); 

       if(rc == kTRUE) {
	 Double_t PD[3], RD[15], H[3], CH, SPU, DJ[3], DK[3], PC[3], RC[15];
	 Int_t IERR;
       
	 PC[0] = fFinalH->GetQp()/fFinalH->GetQ();
	 PC[1] = fFinalH->GetLambda();
	 PC[2] = fFinalH->GetPhi();
       
	 fFinalH->GetCov(RC);
        
	 H[0] = 0; H[1] = 0; H[2] = 20;
	 CH = fRes->GetQ()/TMath::Abs(fRes->GetQ());
        
	 DJ[0] = fRes->GetJVer().X();
	 DJ[1] = fRes->GetJVer().Y();
	 DJ[2] = fRes->GetJVer().Z();
	 DK[0] = fRes->GetKVer().X();
	 DK[1] = fRes->GetKVer().Y();
	 DK[2] = fRes->GetKVer().Z();
	     
	 fUtil->FromSCToSD(PC, RC, H, CH, DJ, DK, IERR, SPU, PD, RD);
       
	 TVector3 positionsd = fUtil->FromMARSToSDCoord(TVector3(fFinalH->GetX(), fFinalH->GetY(), fFinalH->GetZ()), fRes->GetOrigin(), fRes->GetIVer(), fRes->GetJVer(), fRes->GetKVer());
	 Double_t u = positionsd.X(); 
	 Double_t v = positionsd.Y();  
	 Double_t w = positionsd.Z(); 

	 fFinal->SetTrackPar(v, w, PD[1], PD[2], CH*PD[0], RD,
	 fRes->GetOrigin(), fRes->GetIVer(), fRes->GetJVer(),
	 fRes->GetKVer(),SPU);
       }
      

// -----------------------------------------------------



     }
   fTrackArray->Delete();
   fHitArray->Delete();
   fPointArray->Delete();
 
  //  can2->cd(1);
//    dstvtx->Draw();
//    can2->cd(2);
//    momvtx->Draw();

}



ClassImp(FairGeaneTrT)
