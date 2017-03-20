// -------------------------------------------------------------------------
// -----                 FairGeaneTrKalStt source file             -----
// -----                  Created 28/09/06  by M. Al-Turany               -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairGeaneTrKalStt.h"
#include "TGeant3TGeo.h"
#include "TGeant3.h"
#include "TVector3.h"
#include "CbmPlanePoint.h"
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
Int_t eventnum3;
// -----   Default constructor   -------------------------------------------
FairGeaneTrKalStt::FairGeaneTrKalStt() :
  FairTask("Test") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
FairGeaneTrKalStt::~FairGeaneTrKalStt() { }
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus FairGeaneTrKalStt::Init() {
 
  eventnum3 = 0;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- FairGeaneTrKalStt::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
  welldone = 0; notdone = 0;
  notconsidered = 0;
  notbackprocessedhit = 0; notprocessedhit = 0;
  tothits = 0; total = 0;

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
  if (!fHitArray)   cout << "-W- FairGeaneTrKalStt::Init: No Hit array!" << endl;
  // Get point Array
  fPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if (!fPointArray) cout << "-W- FairGeaneTrKalStt::Init: No Point array!" << endl;
 
  // Get SttTrack array
  fTrackArray  = (TClonesArray*) ioman->GetObject("STTTrack"); 
  if (!fTrackArray) {
    cout << "-E- FairGeaneTrKalStt::Init: No SttTrack array!" << endl;
    return kERROR;
  }

  fPro = new FairGeanePro();
  fUtil = new FairGeaneUtil();
  //  fPro->PropagateToVolume("mL3",0,1);


  return kSUCCESS;

}
// -------------------------------------------------------------------------

void FairGeaneTrKalStt::FinishTask() {
  cout << "FairGeaneTrKalStt::FinishTask" << " total " << total << " total hits " << tothits << endl;
  cout << "WELL DONE " << welldone << " NOT DONE " << notdone << endl;
  cout << "notconsidered " << notconsidered << " notprocessedhit " << notprocessedhit << " notbackprocessedhit " << notbackprocessedhit << endl;
}

// -----   Public method Exec   --------------------------------------------
void FairGeaneTrKalStt::Exec(Option_t*) {

  //  cout << "EVENT " << eventnum3 << endl;
  eventnum3++;

  if(total > 900) {
    cout << "FairGeaneTrKalStt::Exec" << " total " << total << " total hits " << tothits << endl;
    cout << "WELL DONE " << welldone << " NOT DONE " << notdone << endl;
    cout << "notconsidered " << notconsidered << " notprocessedhit " << notprocessedhit << " notbackprocessedhit " << notbackprocessedhit << endl;
  }

  fTrackParGeane->Delete();
  fTrackParIni->Delete();
  fTrackParFinal->Delete();

  if (!fTrackArray ) return;

  
  Int_t nTracks = fTrackArray->GetEntriesFast();

  for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) 
    {

      PndSttTrack* pTrack = (PndSttTrack*) fTrackArray->At(iTrack);
      if(!pTrack) continue;

      total++;


      if(pTrack->GetFlag() < 3)  {
	notconsidered++;
	continue;
      } // per ora voglio solo le tracce perfette
       
      // STARTING POINT
       
      /*
	Int_t iHit1 = pTrack->GetHitIndex(0);
	PndSttHit *currenthit1 = (PndSttHit*) fHitArray->At(iHit1);
	if(!currenthit1) continue;
	if(currenthit1->GetXint() == -999 || currenthit1->GetYint() == -999) continue;
	Int_t refindex1 = currenthit1->GetRefIndex(); 
	// get point
	PndSttPoint *fPoint1 = (PndSttPoint*) fPointArray->At(refindex1);
	StartPos    = TVector3 (fPoint1->GetXtot(),fPoint1->GetYtot(),fPoint1->GetZtot());
	StartPosErr = TVector3(0.0,0.0,0.0);
	StartMom    = TVector3 (fPoint1->GetPXtot(),fPoint1->GetPYtot(),fPoint1->GetPZtot());
	StartMomErr = TVector3(0.0,0.0,0.0);
      */
       
      // FROM VERTEX
      Bool_t vtx = RetrieveVertex(pTrack);
       
      // traccio fino all' ultimo hit della traccia PCA al wire
      Int_t hitcounter = pTrack->GetNofHits();
      tothits += hitcounter;

      Int_t iHit2 = pTrack->GetHitIndex(0);
      PndSttHit *currenthit2 = (PndSttHit*) fHitArray->At(iHit2);
      if(!currenthit2) continue;
      if(currenthit2->GetXint() == -999 || currenthit2->GetYint() == -999) continue;
      Int_t refindex2 = currenthit2->GetRefIndex(); 
      // get point
      PndSttPoint *iPoint2 = (PndSttPoint*) fPointArray->At(refindex2);
      //       if(TMath::Sqrt(iPoint2->GetXInLocal()*iPoint2->GetXInLocal() + iPoint2->GetYInLocal()*iPoint2->GetYInLocal()) < 0.4998) continue;
      //           cout << "************************************" << endl;
      //       cout << "HIT CENTER      " << currenthit2->GetX() << " " <<  currenthit2->GetY() << " " <<  currenthit2->GetZ() << endl;
        
      // out 
      // let' s take (approx!) the medium point 
      // position --> (posout - posin) /2
      // momentum --> (momout - momin) / 2
      TVector3 EndPos      = TVector3(iPoint2->GetXtot(), iPoint2->GetYtot(), iPoint2->GetZtot());
      TVector3 EndPosErr   = TVector3(0,0,0);
      TVector3 EndMom      = TVector3(iPoint2->GetPXtot(), iPoint2->GetPYtot(), iPoint2->GetPZtot());
      TVector3 EndMomErr   = TVector3(0,0,0);
       
      PDGCode= 13;
      TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
      TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
      Double_t  fCharge= fParticle->Charge();
       
      TClonesArray& clref1 = *fTrackParIni;
      Int_t size1 = clref1.GetEntriesFast();
      FairTrackParP *fStart = new (clref1[size1]) FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, StartPos, TVector3(0.,1.,0.), TVector3(0.,0.,1.));
	
      TClonesArray& clref = *fTrackParGeane;
      Int_t size = clref.GetEntriesFast();
      FairTrackParP *fRes = new(clref[size]) FairTrackParP();
       
      TClonesArray& clref2 = *fTrackParFinal;
      Int_t size2 = clref2.GetEntriesFast();
      FairTrackParP *fFinal = new(clref2[size2]) FairTrackParP();
      FairTrackParH *fFinalH = new FairTrackParH(EndPos, EndMom, EndPosErr, EndMomErr, fCharge);

      Int_t processedhit = 0;
      Int_t backprocessedhit = 0;
  
      FairTrackParP *fRunningRes = new FairTrackParP();
       
      for(Int_t iteration = 0; iteration < 3; iteration++)
	{
	  //	  if(StartMom.Mag() < 0.100) continue;

	  //  cout << "ITERATION " << iteration << endl;
	  // running start point: ogni volta viene riaggiornato col punto di kalman
	  FairTrackParP *fRunningStart = new FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, TVector3(0.,0.,0.), TVector3(0.,1.,0.), TVector3(0.,0.,1.));    
	  if(!fRunningStart) continue;
	  fRunningRes->Reset();
	   
	  //	      cout << "START " << backprocessedhit << " " << hitcounter << endl;
	  // propagationo ok
	  Int_t donecounter = 0;
	  processedhit = 0;
	  // loop on hits
	  // loop on hits
	  Int_t start = 0;
	  if(iteration > 0) start = backprocessedhit + 1;
	  for(int i = start; i < hitcounter; i++)
	    //	   for(int i = backprocessedhit; i < hitcounter; i++)
	    //	     	   for(int i = 1; i < hitcounter; i++)
	    {

	      //  cout << "i " << i << endl;

	      Bool_t rc_prochit = ProcessHit(pTrack, i, fRunningStart, fRunningRes, "FWD");
	      if(rc_prochit == kFALSE) {
		notprocessedhit++;
		continue;
	      }
	      processedhit = i;
	      donecounter++;
	    }
	  if(donecounter == 0) {
	    notdone++;
	    continue; // se nessun estrap/kalman e' andato bene butto la traccia (o tengo il prefit? CHECK!)
	  }	   
	  // BACK TRACKING AL PRIMO PUNTO
	  //	      cout << "STARTING BACK TRACKING" << donecounter << endl;
	  FairTrackParP *fRunningKal = new FairTrackParP();
	  Double_t KD2[15];
	  fRunningRes->GetCov(KD2);
	  fRunningKal->SetTrackPar(fRunningRes->GetV(), fRunningRes->GetW(), 
				   fRunningRes->GetTV(), fRunningRes->GetTW(), 
				   fRunningRes->GetQp(), KD2, fRunningRes->GetOrigin(), 
				   fRunningRes->GetIVer(), fRunningRes->GetJVer(), fRunningRes->GetKVer(),
				   fRunningRes->GetSPU());
	  Int_t backcounter = 0;
	  backprocessedhit = 0;
	  // loop on hits
	  for(int i = (processedhit-1); i >= 0; i--)
	    // for(int i = (donecounter-2); i >= 0; i--)
	    {
	      //	          cout << "i " << i << endl;
	      Bool_t rc_back = ProcessHit(pTrack, i, fRunningKal, fRunningRes, "BKW");
	      if(rc_back == kFALSE) {
		notbackprocessedhit++;
		continue;
	      }
	      backprocessedhit = i;
	      backcounter++;
	      StartPos = TVector3(fRunningRes->GetX(), fRunningRes->GetY(), fRunningRes->GetZ());
	      StartMom = TVector3(fRunningRes->GetPx(), fRunningRes->GetPy(), fRunningRes->GetPz());
	      StartPosErr =  TVector3(fRunningRes->GetDX(), fRunningRes->GetDY(), fRunningRes->GetDZ());
	      StartMomErr = TVector3(fRunningRes->GetDPx(), fRunningRes->GetDPy(), fRunningRes->GetDPz());
	    }
	  if(backcounter == 0) {
	    notdone++;
	    continue;
	  } 
	   
	   

	  //	   cout << "MOME " << StartMom.Mag() << endl;  
	}

      //------------------------------------------------------
      // BACK TRACK TO VERTEX
      Bool_t rc_vtx = BackToVertex(fRunningRes, fRes);
      if(rc_vtx == kFALSE) {
	notdone++;
	continue;
      }
      welldone++;
       

      if(!fRes) continue;
       
      Double_t PD[3], RD[15], H[3], CH, SPU, DJ[3], DK[3], PC[3], RC[15];
      Int_t IERR;
       
      PC[0] = fFinalH->GetQp()/fFinalH->GetQ();
      PC[1] = fFinalH->GetLambda();
      PC[2] = fFinalH->GetPhi();
       
      fFinalH->GetCov(RC);
       
      H[0] = 0; H[1] = 0; H[2] = 20;
              
      if(fRunningRes->GetQ()!=0) CH = fRunningRes->GetQ()/TMath::Abs(fRunningRes->GetQ());
      else continue; 
       
      DJ[0] = fRunningRes->GetJVer().X();
      DJ[1] = fRunningRes->GetJVer().Y();
      DJ[2] = fRunningRes->GetJVer().Z();
      DK[0] = fRunningRes->GetKVer().X();
      DK[1] = fRunningRes->GetKVer().Y();
      DK[2] = fRunningRes->GetKVer().Z();
       
      fUtil->FromSCToSD(PC, RC, H, CH, DJ, DK, IERR, SPU, PD, RD);
       
      TVector3 positionsd = fUtil->FromMARSToSDCoord(TVector3(fFinalH->GetX(), fFinalH->GetY(), fFinalH->GetZ()), fRunningRes->GetOrigin(), fRunningRes->GetIVer(), fRunningRes->GetJVer(), fRunningRes->GetKVer());
      Double_t u = positionsd.X(); // CHECK
      Double_t v = positionsd.Y(); // CHECK 
      Double_t w = positionsd.Z(); // CHECK 
       
      double spu; 
      fFinal->SetTrackPar(v, w, PD[1], PD[2], CH*PD[0], RD, fRunningRes->GetOrigin(), fRunningRes->GetIVer(), fRunningRes->GetJVer(),
      fRunningRes->GetKVer(),spu);
      // -----------------------------------------------------
       
    }


  fTrackArray->Delete();
  fHitArray->Delete();
  fPointArray->Delete();
    
}

Bool_t FairGeaneTrKalStt::ProcessHit(PndSttTrack *pTrack, Int_t i, FairTrackParP *fRunningStart, FairTrackParP *fRunningRes, TString fb)
{
  if(!fRunningStart) return kFALSE;
  Int_t iHit = pTrack->GetHitIndex(i);
  PndSttHit *currenthit = (PndSttHit*) fHitArray->At(iHit);
  if(!currenthit) { cout << "NON HIT" << endl; return kFALSE;}
  if(currenthit->GetXint() == -999 || currenthit->GetYint() == -999) { cout << "NON HIT INT" << endl; return kFALSE; }
  Int_t refindex = currenthit->GetRefIndex(); 
  // get point
  PndSttPoint *iPoint = (PndSttPoint*) fPointArray->At(refindex);
  //  if(TMath::Sqrt(iPoint->GetXInLocal()*iPoint->GetXInLocal()+iPoint->GetYInLocal()*iPoint->GetYInLocal()) < 0.4998){cout << "OUT OF TUBE " << TMath::Sqrt(iPoint->GetXInLocal()*iPoint->GetXInLocal()+iPoint->GetYInLocal()*iPoint->GetYInLocal()) << endl; return kFALSE;}
  
  // check if points are too close
  //  TVector3 p1 = TVector3(fRunningStart->GetX(), fRunningStart->GetY(), 0.);
  //  TVector3 p2 = TVector3(currenthit->GetX(), currenthit->GetY(), 0.);
  //  TVector3 p3 = p1 - p2;
  //  if(p3.Mag() < 1.5) return kFALSE;


//     cout << "START POINT     " << fRunningStart->GetX()<<" "<< fRunningStart->GetY() << " " << fRunningStart->GetZ() << endl;
//     cout << "START POINT ERR " << fRunningStart->GetDX()<<" " << fRunningStart->GetDY()<<" "<<fRunningStart->GetDZ() << endl;
//     cout <<"START MOM       " << fRunningStart->GetPx()<<" "<< fRunningStart->GetPy()<<" " << fRunningStart->GetPz() << endl;
//     cout << "START MOM ERR   " << fRunningStart->GetDPx()<<" "<<fRunningStart->GetDPy()<<" "<<fRunningStart->GetDPz() << endl;
  //    cout << "START POINT     " << fRunningStart->GetV()<<" "<< fRunningStart->GetW() << endl;
  //    cout << "START ERR       " << fRunningStart->GetDV()<<" "<< fRunningStart->GetDW() << endl;

  Bool_t rc_pro = Propagation(currenthit, fRunningStart, fRunningRes, fb);
 
  if(rc_pro == kFALSE) {
    //    cout << "RESET" << endl;
    //    fRunningRes->Reset();
    return kFALSE;
  }

  //   cout << "EXTRAP POINT     " << fRunningRes->GetX()<<" "<< fRunningRes->GetY() << " " << fRunningRes->GetZ() << endl;
  //    cout << "EXTRAP POINT ERR " << fRunningRes->GetDX()<<" " << fRunningRes->GetDY()<<" "<<fRunningRes->GetDZ() << endl;
  //    cout << "EXTRAP MOM       " << fRunningRes->GetPx()<<" "<< fRunningRes->GetPy()<<" " << fRunningRes->GetPz() << endl;
  //    cout << "EXTRAP MOM ERR   " << fRunningRes->GetDPx()<<" "<<fRunningRes->GetDPy()<<" "<<fRunningRes->GetDPz() << endl;
  //    cout << "EXTRAP POINT     " << fRunningRes->GetV()<<" "<< fRunningRes->GetW() << endl;
  //    cout << "EXTRAP ERR       " << fRunningRes->GetDV()<<" "<< fRunningRes->GetDW() << endl;


  // kalman call
  Bool_t rc_kal = Kalman(currenthit, fRunningRes, fRunningStart);
  //  cout << "KAL " << rc_kal << endl;
  if(rc_kal == kFALSE) return kFALSE;
  //......................................
  // per controllare che il segno delle componenti sia corretto (da cancellare)
  // 	   if(premomx * fRunningStart->GetPx() < 0) cout << "AAAAAAAAIUTOOOOOOOOO X" << endl;
  // 	   if(premomy * fRunningStart->GetPy() < 0) cout << "AAAAAAAAIUTOOOOOOOOO Y" << endl;
  // 	   if(premomz * fRunningStart->GetPz() < 0) cout << "AAAAAAAAIUTOOOOOOOOO Z" << endl;
  return kTRUE;

}

Bool_t FairGeaneTrKalStt::Propagation(PndSttHit *currenthit, FairTrackParP *fRunningStart, FairTrackParP *fRunningRes, TString fb)
{
  //  cout << "................." << endl;
  //  cout << "O " << fRunningStart->GetOrigin().X()<<" "<< fRunningStart->GetOrigin().Y() << " " << fRunningStart->GetOrigin().Z() << endl;
  //  cout << "I " << fRunningStart->GetIVer().X()<<" "<< fRunningStart->GetIVer().Y() << " " << fRunningStart->GetIVer().Z() << endl; 
  //  cout << "J " << fRunningStart->GetJVer().X()<<" "<< fRunningStart->GetJVer().Y() << " " << fRunningStart->GetJVer().Z() << endl;
  //  cout << "K " << fRunningStart->GetKVer().X()<<" "<< fRunningStart->GetKVer().Y() << " " << fRunningStart->GetKVer().Z() << endl;
  //     cout << "R1START POINT     " << fRunningStart->GetX()<<" "<< fRunningStart->GetY() << " " << fRunningStart->GetZ() << endl;
  //     cout << "R1START POINT ERR " << fRunningStart->GetDX()<<" " << fRunningStart->GetDY()<<" "<<fRunningStart->GetDZ() << endl;
  //     cout << "R1START MOM       " << fRunningStart->GetPx()<<" "<< fRunningStart->GetPy()<<" " << fRunningStart->GetPz() << endl;
  //     cout << "R1START MOM ERR   " << fRunningStart->GetDPx()<<" "<<fRunningStart->GetDPy()<<" "<<fRunningStart->GetDPz() << endl;
  
  // per controllare che il segno delle componenti sia corretto (da cancellare)
  // 	   double premomx = fRunningStart->GetPx();
  // 	   double premomy = fRunningStart->GetPy();
  // 	   double premomz = fRunningStart->GetPz();
  
  // ----- propagation: uso la propagate to closest ------
  TVector3 v0 = TVector3(0., 0., 0.);
  fPro->SetPoint(v0);
  
 
  TVector3 center = TVector3(currenthit->GetX(), currenthit->GetY(), currenthit->GetZ());
  TVector3 wiredir = currenthit->GetWireDirection();
  wiredir *= 75.;
  TVector3 second = (TVector3)(center + wiredir);
  TVector3 first = (TVector3)(center - wiredir);
  
  fPro->SetWire(first, second);
  if(fb == "FWD") fPro->PropagateToVirtualPlaneAtPCA(2);   // 1 if point; 2 if wire
  else if(fb == "BKW") fPro->BackTrackToVirtualPlaneAtPCA(2);   // 1 if point; 2 if wire
  
  Bool_t rc = kFALSE;
  rc = fPro->Propagate(fRunningStart, fRunningRes, PDGCode); 
  //  cout << "PROPAGATION RC " << rc << " " << currenthit->GetX() << " " <<  currenthit->GetY() << " " <<  currenthit->GetZ() << endl;

  // if propagation failed
 // if(fabs(fPro->GetPCAOnWire().X()) > 50. || fabs(fPro->GetPCAOnWire().Y()) > 50. || fabs(fPro->GetPCAOnWire().Z()) > 75.) rc = kFALSE;
  if(fabs(fRunningRes->GetX()) > 50. || fabs(fRunningRes->GetY()) > 50. || fabs(fRunningRes->GetZ()) > 50) rc = kFALSE;



  return rc;

}
Bool_t FairGeaneTrKalStt::Kalman(PndSttHit *currenthit, FairTrackParP *fRunningRes, FairTrackParP *fRunningStart)
{
  // KALMAN
  // punto estrapolato 
  TMatrixT<double> extrap(5,1);
  extrap[0][0] = fRunningRes->GetQp()/fRunningRes->GetQ();
  extrap[1][0] = fRunningRes->GetTV();
  extrap[2][0] = fRunningRes->GetTW();
  extrap[3][0] = fRunningRes->GetV();
  extrap[4][0] = fRunningRes->GetW();
  //  cout << "extrap " << extrap[0][0] << " " << extrap[1][0] << " " << extrap[2][0] << " " <<  extrap[3][0] << " " << extrap[4][0] << endl;

  // punto estrapolato matrice errori
  TMatrixT<double> sigmaex(5,5);
  Double_t SIGMAEX[15], SIGMAEX55[5][5];
  fRunningRes->GetCov(SIGMAEX);
  fUtil->FromVec15ToMat25(SIGMAEX, SIGMAEX55);
  for(int i = 0; i < 5; i++) for(int j = 0; j < 5; j++) sigmaex[i][j] = SIGMAEX55[i][j];
  TMatrixT<double> weightex(5,5);
  weightex = sigmaex;
  Double_t det1 = weightex.Determinant();
  if(det1 != 0) weightex.Invert();
  else {
    cout << "det1 = 0!!!!!" << endl;
    return kFALSE;
  }
  
  // punto misurato
  Double_t rdrift = currenthit->GetIsochrone();
  TMatrixT<double> meas(5,1);
  meas[0][0] = 0.;
  meas[1][0] = 0.;
  meas[2][0] = 0.;
  meas[3][0] = rdrift;
  //  meas[4][0] = currenthit->GetZint();
  if(currenthit->GetWireDirection() == TVector3(0.,0.,1.)) meas[4][0] = 0.;
  else if(currenthit->GetZint() != -999)  meas[4][0] = currenthit->GetZint();
  else meas[4][0] = 0.;


  // punto misurato matrice pesi
  TMatrixT<double> weightmi(5,5);
  for(int i = 0; i < 5; i++) for(int j = 0; j < 5; j++) weightmi[i][j] = 0.;
  weightmi[3][3] = 1./(0.0150 * 0.0150); // CHECK 
  if(meas[4][0] == 0) weightmi[4][4] = 1./(1.5 * 1.5); // CHECK
  //   cout << "MEAS POINT     " << meas[3][0] <<" "<< meas[4][0] << endl;
  //   cout << "MEAS ERR       " << sqrt(1./weightmi[3][3]);
  //   if(weightmi[4][4]!=0) cout << " " << sqrt(1./weightmi[4][4]) << endl;
  //   else cout << " 0" << endl;

  // cout << "meas " << meas[0][0] << " " << meas[1][0] << " " << meas[2][0] << " " <<  meas[3][0] << " " << meas[4][0] << endl;

  // somma pesiex + pesimi --> sigmakal = errori di kalman
  TMatrixT<double> weightsum(5,5);
  weightsum = weightex + weightmi;
  // inverto
  TMatrixT<double> sigmakal(5,5);
  sigmakal = weightsum;
  Double_t det2 = sigmakal.Determinant();
  if(det2 != 0) sigmakal.Invert();
  else {
    cout << "det2 = 0!!!!!" << endl;
    return kFALSE;
  }
  
  // somma pesiex * extr + pesimi * meas
  // pesiex * extr
  TMatrixT<double> wextrap(weightex,TMatrixT<double>::kMult,extrap);
  // pesimi * meas
  TMatrixT<double> wmeas(weightmi,TMatrixT<double>::kMult,meas);
  // sommo
  TMatrixT<double> sum(5,1);
  //   for(int i = 0; i < 5; i++) wextrap[i][0] = fabs(wextrap[i][0]);
  //   for(int i = 0; i < 5; i++) wmeas[i][0] = fabs(wmeas[i][0]);



  sum = wextrap + wmeas;
  
  //  cout << "wextrap " << wextrap[0][0] << " " << wextrap[1][0] << " " << wextrap[2][0] << " " <<  wextrap[3][0] << " " << wextrap[4][0] << endl;
  // cout << "wmeas " << wmeas[0][0] << " " << wmeas[1][0] << " " << wmeas[2][0] << " " <<  wmeas[3][0] << " " << wmeas[4][0] << endl;





  // kalman point
  TMatrixT<double> kalman(sigmakal,TMatrixT<double>::kMult,sum);
  
  // cout << "kalman " << kalman[0][0] << " " << kalman[1][0] << " " << kalman[2][0] << " " <<  kalman[3][0] << " " << kalman[4][0] << endl;

  Double_t SIGMAKAL[5][5], SIGMAKAL15[15];
  for(int i = 0; i < 5; i++) for(int j = 0; j < 5; j++) SIGMAKAL[i][j] = sigmakal[i][j];
  fUtil->FromMat25ToVec15(SIGMAKAL, SIGMAKAL15);
  
  fRunningStart->SetTrackPar(kalman[3][0], kalman[4][0], 
			     kalman[1][0], kalman[2][0], 
			     kalman[0][0] * fRunningRes->GetQ(), SIGMAKAL15, fRunningRes->GetOrigin(), 
			     fRunningRes->GetIVer(), fRunningRes->GetJVer(), fRunningRes->GetKVer(),
			     fRunningRes->GetSPU());
  
  //  cout << "................." << endl;
  //  cout << "O " << fRunningStart->GetOrigin().X()<<" "<< fRunningStart->GetOrigin().Y() << " " << fRunningStart->GetOrigin().Z() << endl;
  //  cout << "I " << fRunningStart->GetIVer().X()<<" "<< fRunningStart->GetIVer().Y() << " " << fRunningStart->GetIVer().Z() << endl; 
  //  cout << "J " << fRunningStart->GetJVer().X()<<" "<< fRunningStart->GetJVer().Y() << " " << fRunningStart->GetJVer().Z() << endl;
  //  cout << "K " << fRunningStart->GetKVer().X()<<" "<< fRunningStart->GetKVer().Y() << " " << fRunningStart->GetKVer().Z() << endl;
  
  //   cout << "KALMAN POINT     " << fRunningStart->GetX()<<" "<< fRunningStart->GetY() << " " << fRunningStart->GetZ() << endl;
  //   cout << "KALMAN POINT ERR " << fRunningStart->GetDX()<<" " << fRunningStart->GetDY()<<" "<<fRunningStart->GetDZ() << endl;
  //   cout << "KALMAN MOM       " << fRunningStart->GetPx()<<" "<< fRunningStart->GetPy()<<" " << fRunningStart->GetPz() << endl;
  //   cout << "KALMAN MOM ERR   " << fRunningStart->GetDPx()<<" "<<fRunningStart->GetDPy()<<" "<<fRunningStart->GetDPz() << endl;
  //   cout << "KALMAN POINT     " << fRunningStart->GetV()<<" "<< fRunningStart->GetW() << endl;
  //   cout << "KALMAN ERR       " << fRunningStart->GetDV()<<" "<< fRunningStart->GetDW() << endl;


  //   cout << "KALMANMOMENTUM " << TMath::Sqrt(fRunningStart->GetPx() * fRunningStart->GetPx() + fRunningStart->GetPy() * fRunningStart->GetPy() + fRunningStart->GetPz() * fRunningStart->GetPz()) << endl;

 
  return kTRUE;


}

//-------------------
Bool_t FairGeaneTrKalStt::RetrieveVertex(PndSttTrack *pTrack)
{

  PndSttHelixTrackFitter *fFitter;
  TVector3 *vertex = new TVector3(0.,0.,0.); // CHECK 
  TVector3 *recovtx = fFitter->PCAToPoint(pTrack, vertex);
  TVector3 *momeatvtx;
  momeatvtx = fFitter->MomentumAtPoint(pTrack, recovtx);
  
  //  Double_t pterr = pTrack->TransverseMomentumError();
  //  Double_t plerr = pTrack->LongitudinalMomentumError();

  Double_t phi  = pTrack->GetParamLast()->GetY();
  Double_t d    = pTrack->GetParamLast()->GetX();
  Double_t cosp = TMath::Cos(phi);
  Double_t sinp = TMath::Sin(phi);
//  Double_t errd = pTrack->GetD0Error();
//  Double_t errphi = pTrack->GetPhi0Error();
  //  Double_t errxp  = sqrt(cosp * cosp * errd * errd + d * d * sinp * sinp * errphi * errphi);
  //  Double_t erryp  = sqrt(sinp * sinp * errd * errd + d * d * cosp * cosp * errphi * errphi);
  //  Double_t errzp  = pTrack->GetZ0Error();

  // ERRORI SUL VERTICE 
  // errors * 10;
  //  errxp *= 10.; erryp *= 10.; errzp *= 10.;
  //  pterr *= 10.; plerr *= 10.;
  StartPos    = *recovtx;
  Double_t pterr, plerr;
  Double_t errxp, erryp, errzp;
  errxp = 10 * 0.02;
  erryp = errxp;
  errzp = 10 * 0.15;
  StartPosErr = TVector3(errxp, erryp, errzp);
  StartMom    = *momeatvtx;

  Double_t momerr; // = 0.04 * 1.5;
  momerr = 0.04 *  2.5;
  
  //  pterr = 10 * 0.02;
  //  plerr = 10 * 0.02;
  pterr = momerr;
  plerr = momerr;

  StartMomErr = TVector3(pterr, pterr, plerr); 
  // ..................................................
  
  //cout << "vertex pos     " << StartPos.X() << " "  << StartPos.Y() << " "  << StartPos.Z() << endl;
  //cout << "vertex pos err " << StartPosErr.X() << " "  << StartPosErr.Y() << " "  << StartPosErr.Z() << endl;
  //cout << "vertex mom     " << StartMom.X() << " "  << StartMom.Y() << " "  << StartMom.Z() << endl;
  //cout << "vertex mom err " << StartMomErr.X() << " "  << StartMomErr.Y() << " "  << StartMomErr.Z() << endl;

  return kTRUE;
 
}

Bool_t FairGeaneTrKalStt::BackToVertex(FairTrackParP *fRunningRes, FairTrackParP *fRunningKal)
{

  //   cout << "back to vertex " << fRunningRes->GetQp() << " " << fRunningRes->GetTV() << " " << fRunningRes->GetTW() << " " << fRunningRes->GetV() << " " << fRunningRes->GetW() << endl;


  if(fRunningRes->GetQp() == 0) return kFALSE;
  // BACK TRACK TO VERTEX
  TVector3 v0 = TVector3(0., 0., 0.);
  fPro->SetPoint(v0);
  fPro->SetWire(v0,v0);
  fPro->BackTrackToVertex();
  Bool_t rc_vtx = kFALSE;
  TVector3 lastPos = TVector3(fRunningRes->GetX(),
			      fRunningRes->GetY(),
			      fRunningRes->GetZ());
  TVector3 lastMom = TVector3(fRunningRes->GetPx(),
			      fRunningRes->GetPy(),
			      fRunningRes->GetPz());
  TVector3 lastPosErr = TVector3(fRunningRes->GetDX(),
				 fRunningRes->GetDY(),
				 fRunningRes->GetDZ());
  TVector3 lastMomErr = TVector3(fRunningRes->GetDPx(), 
				 fRunningRes->GetDPy(),
				 fRunningRes->GetDPz());

  FairTrackParH *fKalH1 =  new FairTrackParH(lastPos, lastMom, 
					   lastPosErr, lastMomErr, 
					   fRunningRes->GetQ());


  //  cout << "fKalH1 " << fKalH1->GetQp() << " " << fKalH1->GetX() << " " << fKalH1->GetY() << " " << fKalH1->GetZ() << endl;

  FairTrackParH *fKalH2 =  new FairTrackParH();
  rc_vtx = fPro->Propagate(fKalH1, fKalH2, PDGCode);
  //  cout << "fKalH2 " << fKalH2->GetQp() << " " << fKalH2->GetX() << " " << fKalH2->GetY() << " " << fKalH2->GetZ() << endl;

  if(rc_vtx == kFALSE) return kFALSE;
  Double_t PC[3], RC[15], H[3], CH, DJ[3], DK[3], SPU, PD[3], RD[15];
  Int_t IERR;
  PC[0] = fKalH2->GetQp()/fKalH2->GetQ(); PC[1] = fKalH2->GetLambda(); PC[2] = fKalH2->GetPhi(); 
  fKalH2->GetCov(RC);
  H[0] = 0.; H[1] = 0.; H[2] = 20.;
  CH = fKalH2->GetQ();
  DJ[0] = 0.; DJ[1] = 1.; DJ[2] = 0;
  DK[0] = 0.; DK[1] = 0.; DK[2] = 1;
  fUtil->FromSCToSD(PC,RC, H,CH, DJ, DK, IERR, SPU,PD,RD);
  
  fRunningKal->SetTrackPar(fKalH2->GetX(), fKalH2->GetY(), fKalH2->GetZ(),
			   fKalH2->GetPx(), fKalH2->GetPy(), fKalH2->GetPz(),
			   fKalH2->GetQ(), RD,
			   TVector3(0.,0.,0.), TVector3(1.,0.,0.), TVector3(0.,1.,0.), TVector3(0.,0.,1.));
  //  cout << "fRunningKal " << fRunningKal->GetQp() << " " << fRunningKal->GetTV() << " " << fRunningKal->GetTW() << " " << fRunningKal->GetV() << " " << fRunningKal->GetW() << endl;
  //  cout << "VTX POINT     " << fRunningKal->GetX()<<" "<< fRunningKal->GetY() << " " << fRunningKal->GetZ() << endl;
  //        cout << "VTX POINT ERR " << fRunningKal->GetDX()<<" " << fRunningKal->GetDY()<<" "<<fRunningKal->GetDZ() << endl;
  //  cout << "VTX MOM       " << fRunningKal->GetPx()<<" "<< fRunningKal->GetPy()<<" " << fRunningKal->GetPz() << endl;
  //        cout << "VTX MOM ERR   " << fRunningKal->GetDPx()<<" "<<fRunningKal->GetDPy()<<" "<<fRunningKal->GetDPz() << endl;
  //  cout << "VTX MOMENTUM " << TMath::Sqrt(fRunningKal->GetPx() * fRunningKal->GetPx() + fRunningKal->GetPy() * fRunningKal->GetPy() + fRunningKal->GetPz() * fRunningKal->GetPz()) << endl;
}

Bool_t FairGeaneTrKalStt::BackToVertex2(FairTrackParP *fRunningRes, FairTrackParP *fRunningKal)
{

  if(!fRunningRes) return kFALSE;
  //  cout << "back to vertex " << fRunningRes->GetQp() << " " << fRunningRes->GetTV() << " " << fRunningRes->GetTW() << " " << fRunningRes->GetV() << " " << fRunningRes->GetW() << endl;
  
  // ----- propagation: uso la propagate to closest ------
  TVector3 v0 = TVector3(0., 0., 0.);
  fPro->SetPoint(v0);
  fPro->SetWire(v0,v0);
  
  fPro->BackTrackToVirtualPlaneAtPCA(1);   // 1 if point; 2 if wire
 
  Bool_t rc = kFALSE;
  rc = fPro->Propagate(fRunningRes, fRunningKal, PDGCode); 
  
  //cout << "fRunningKal " << fRunningKal->GetQp() << " " << fRunningKal->GetTV() << " " << fRunningKal->GetTW() << " " << fRunningKal->GetV() << " " << fRunningKal->GetW() << endl;
  // cout << "VTX POINT     " << fRunningKal->GetX()<<" "<< fRunningKal->GetY() << " " << fRunningKal->GetZ() << endl;
  //cout << "VTX POINT ERR " << fRunningKal->GetDX()<<" " << fRunningKal->GetDY()<<" "<<fRunningKal->GetDZ() << endl;
  // cout << "VTX MOM       " << fRunningKal->GetPx()<<" "<< fRunningKal->GetPy()<<" " << fRunningKal->GetPz() << endl;
  //cout << "VTX MOM ERR   " << fRunningKal->GetDPx()<<" "<<fRunningKal->GetDPy()<<" "<<fRunningKal->GetDPz() << endl;
  //  cout << "VTX MOMENTUM " << TMath::Sqrt(fRunningKal->GetPx() * fRunningKal->GetPx() + fRunningKal->GetPy() * fRunningKal->GetPy() + fRunningKal->GetPz() * fRunningKal->GetPz()) << endl;
}


ClassImp(FairGeaneTrKalStt)

