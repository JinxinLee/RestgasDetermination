#include "PndTpcLheTrackFitter.h"
#include "PndTpcLheTrack.h"

#include "lhe.h"

#include "CbmTrackParH.h"
#include "CbmField.h"
#include "CbmMCApplication.h"
#include "CbmRunAna.h"
#include "CbmRootManager.h"

#include "TGeoTrack.h"

#include "TH2F.h"

#include <cmath>

// ---------------------------------------------------------------
// --- Interface with TrackFinder and output ---


PndTpcLheTrackFitter* PndTpcLheTrackFitter::ftInstance = NULL;

//___________________________________________________________
PndTpcLheTrackFitter* PndTpcLheTrackFitter::Instance() {
  //---
  return ftInstance;
}

//___________________________________________________________
PndTpcLheTrackFitter::~PndTpcLheTrackFitter() {
  //
  CbmRootManager *fManger =CbmRootManager::Instance();
  fManger->Write();
}

//___________________________________________________________
PndTpcLheTrackFitter::PndTpcLheTrackFitter() {
  //---
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  if( !ftInstance ) ftInstance = this;
}

//___________________________________________________________
PndTpcLheTrackFitter::PndTpcLheTrackFitter(const char *name, const char *title)
  :CbmTask(name) {
  //---
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  if( !ftInstance ) ftInstance = this;
  //  fOutputTracks = new TClonesArray("TpcFFTrack");

}

//___________________________________________________________
InitStatus PndTpcLheTrackFitter::Init() {

  //  cout << "InitStatus PndTpcLheTrackFitter::Init()" << endl;

  CbmRootManager *fManager =CbmRootManager::Instance();	

  fTpcHits = (TClonesArray *)fManager->GetObject("LheHit");

  fTpcPoints   = (TClonesArray *)fManager->GetObject("PndTpcPoint");
  if ( fTpcPoints != 0) {
    cout << "-I- PndTpcLheTrackFitter::Init: TpcPoint array! Switching simulation ON" << endl;
    fSimulation = kTRUE;
    //cout << "-I- "<< GetName() << "::Init: No TpcPoint array!" << endl;
    //return kERROR;
  }

  fTpcTracks = (TClonesArray *)fManager->GetObject("PndTpcLheTrack");
  if ( ! fTpcTracks ) {
    cout << "-I- "<< GetName() << "::Init: No PndTpcLheTrack array!" << endl;
    return kERROR;
  }

  fTpcTrCand = new TClonesArray("TGeoTrack");
  fTpcTrFit  = new TClonesArray("TGeoTrack");

  fManager->Register("TrackCand", "Cand",fTpcTrCand,  kTRUE);
  fManager->Register("TrackFit",  "Fit", fTpcTrFit,  kTRUE);

  //  get the field in Memory:
  CbmRunAna *fRun=CbmRunAna::Instance();
  fMagField = (CbmField*) fRun->GetField();

  fTrackCuts =PndTpcLheTrackCuts::Instance();

  fXYG= new TH2F("XY Geant","XY",100,0,100,100,0,100);
  fXYF= new TH2F("XY Fit","XY",100,0,100,100,0,100);

  fYZG= new TH2F("YZ Geant","YZ",100,0,100,100,0,100);
  fYZF= new TH2F("YZ Fit","YZ",100,0,100,100,0,100);

  fXZG= new TH2F("XZ Geant","XZ",100,0,100,100,0,100);
  fXZF= new TH2F("XZ Fit","XZ",100,0,100,100,0,100);

  return kSUCCESS;
}

//______________________________________________________
void PndTpcLheTrackFitter::Exec(Option_t * option) {

  cout << " =====   PndTpcLheTrackFitter   ===== " << endl;

  fTpcTrCand->Delete();
  fTpcTrFit->Delete();

  if (fTpcTracks) {
    cout << " Number of tracks for fitting " <<
      fTpcTracks->GetEntriesFast() << endl;
  }

  Int_t nTracks = fTpcTracks->GetEntriesFast();

  for (Int_t i = 0; i < nTracks; i++) {

    PndTpcLheTrack* track = (PndTpcLheTrack*) fTpcTracks->At(i);
    if (fVerbose) cout << "\n\n Track " << i << "\n"; 
    //    cout << "\n\n Track " << track->GetTrackNumber() << "\n"; 

    //    track->Print();

    TClonesArray& clref = *fTpcTrCand;
    Int_t size = clref.GetEntriesFast();
    fTrCan= new(clref[size]) TGeoTrack();
    
    TClonesArray& clref1 = *fTpcTrFit;
    Int_t size1 = clref1.GetEntriesFast();
    fTrFit= new(clref1[size1]) TGeoTrack();

    if (HelixFit(track)>1)
      {   
	Info4Fit(track);
      }
    else
      {
	track->SetGood(kFALSE);
      }
    
  }

#if 0
  /** Constructor with all track variables  (x,y,z in SC) **/
  CbmTrackParH(Double_t x,  Double_t y,  Double_t z,
	       Double_t lambda, Double_t phi, Double_t qp,
	       Double_t CovMatrix[15]);

  /** Constructor track parameters with position (LAB) momentum **/
  
  CbmTrackParH(TVector3 pos, TVector3 Mom, TVector3 posErr,
	       TVector3 MomErr, Double_t q);

#endif


}

//_____________________________________________________________________________
void PndTpcLheTrackFitter::Info4Fit(PndTpcLheTrack *track) {
  //---

  TObjArray *rhits = (TObjArray* )track->GetRHits();
  Int_t nHits = rhits->GetEntries();
  
  // alpha = .2998 * magfield / 100 <- for obtain moment in GeV/c
  Double_t alpha = .2998 * .02 ;
  Double_t Q = double(TMath::Sign(1, track->GetCharge()));
  
  PndTpcLhePoint hel = track->GetCircle();
  
  Double_t xc = hel.GetX();
  Double_t yc = hel.GetY();
  Double_t rad = hel.GetZ();
  
  TVector2 centre(xc, yc);
  Double_t fi0 = centre.Phi();    // 0 - 2Pi
  Double_t d0 = Q*(TMath::Sqrt(xc*xc + yc*yc) - hel.GetZ());
  
  TVector2 pivot(-xc, -yc);
  
  Double_t lam = track->GetTanDipAngle();
  
  // now suppose that all tracks are primary
  Double_t z0 = track->GetZ0();
  
  // loop over hits
  
  for (int ih = 0; ih < nHits; ih++) {
    
    PndTpcLheHit* hit = (PndTpcLheHit*)rhits->At(ih);
    if (NULL==hit) break;
    TVector2 pnt(hit->GetX() - xc, hit->GetY() - yc);
    
    Double_t phi = pnt.DeltaPhi(pivot);
    //  Double_t phi = pnt.DeltaPhi(centre);
    
    Double_t x = d0*TMath::Cos(fi0) +
      rad *(TMath::Cos(fi0) - TMath::Cos(fi0 + phi));   
    
    Double_t y = d0*TMath::Sin(fi0) +
      rad *(TMath::Sin(fi0) - TMath::Sin(fi0 + phi));   
    
    //Double_t z = z0 - rad*lam*(phi)*TMath::Sign(-1.,phi);  // OLD
    Double_t z = z0 - rad*lam*phi; // STE
    
    Double_t px = -Q*alpha*rad*TMath::Sin(fi0 + phi);
    Double_t py =  Q*alpha*rad*TMath::Cos(fi0 + phi);
    //Double_t pz =  Q*alpha*rad*lam*TMath::Sign(-1.,phi); // OLD
    Double_t pz =  -alpha*rad*lam; // STE
    
    // write momentum into output tree, /sl/ 29.08.07
    
    track->SetPx(px);
    track->SetPy(py);
    track->SetPz(pz);
    
    if (ih==0) track->SetFirstHit(hit->GetX(),hit->GetY(),hit->GetZ());
    track->SetLastHit(hit->GetX(),hit->GetY(),hit->GetZ());

    if (fVerbose) cout <<  " Fitted x, y, z: " << x << " " << y << " " << z
		       << " px, py, pz: " << px << " " << py << " " << pz << endl;
    if (fSimulation)
      {
	PndTpcPoint *point =(PndTpcPoint *) fTpcPoints->At(hit->GetRefIndex());
	if (NULL==point) break; 
	//if ((NULL==hit) || (NULL==point)) break; // JGM hack to avoid segfault
	
	if (fVerbose) cout <<  "\n  Geant x, y, z: " 
			   << point->GetX() << " " << point->GetY()<< " " << point->GetZ()
			   << " px, py, pz: " << point->GetPx() << " " << point->GetPy() <<
			" " << point->GetPz() << endl;
	
	fTrCan->AddPoint(point->GetX(),  point->GetY(), point->GetZ(),0 ); 
	//  calculate deflection angle between point and phi0
	//
	fXYG->Fill(point->GetX(),point->GetY());
	fYZG->Fill(point->GetY(),point->GetZ());
	fXZG->Fill(point->GetX(),point->GetZ()); 

	fTrFit->AddPoint(x, y, z, 0 );
	fXYF->Fill(x,y);
	fYZF->Fill(y,z);
	fXZF->Fill(x,z);  
	
	if (fVerbose) cout << "SOEREN vertex distance " << point->GetPx()-x << "\n";
      }
  }                    
  
  
}

//_____________________________________________________________________________
Int_t PndTpcLheTrackFitter::CircleFit(PndTpcLheTrack *track) {

  //---  From Ososkov CircleCOP() Comp.Phys.Com 33, p.329

  TObjArray *rhits = (TObjArray* )track->GetRHits();
  Int_t NHits = rhits->GetEntries();

  double M0,Mx,My;
  M0 = NHits;
  Mx=My=0.;

  for (Int_t lh = 0; lh < NHits; lh++) {
    PndTpcLheHit* hit = (PndTpcLheHit*) rhits->At(lh);
    Mx += hit->GetX();
    My += hit->GetY();
  }

  Mx /= M0;
  My /= M0;

  //     computing moments (note: all moments are normed, i.e. divided
  //     by N)



  double Xi,Yi,Zi;
  double Mxy, Mxx, Myy, Mxz, Myz, Mzz;

  Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

  for (Int_t lh = 0; lh < NHits; lh++) {
    PndTpcLheHit* hit = (PndTpcLheHit*) rhits->At(lh);
    //  for (i=0; i<data.n; i++) {
    Xi = hit->GetX() - Mx;
    Yi = hit->GetY() - My;
    Zi = Xi*Xi + Yi*Yi;

    Mxy += Xi*Yi;
    Mxx += Xi*Xi;
    Myy += Yi*Yi;
    Mxz += Xi*Zi;
    Myz += Yi*Zi;
    Mzz += Zi*Zi;
  }

  Mxx /= M0;
  Myy /= M0;
  Mxy /= M0;
  Mxz /= M0;
  Myz /= M0;
  Mzz /= M0;

  //    computing the coefficients of the characteristic polynomial

  double Mz,Mxz2,Myz2,Cov_xy; //,temp;
  double A0, A1, A2, A22, epsilon = 0.000000000001;
  double Dy, xnew, xold, ynew, yold = 100000000000.;

  Mz = Mxx + Myy;
  Cov_xy = Mxx*Myy - Mxy*Mxy;
  Mxz2 = Mxz*Mxz;
  Myz2 = Myz*Myz;

  A2 = 4.*Cov_xy - 3.*Mz*Mz - Mzz;
  A1 = Mzz*Mz + 4.*Cov_xy*Mz - Mxz2 - Myz2 - Mz*Mz*Mz;
  A0 = Mxz2*Myy + Myz2*Mxx - Mzz*Cov_xy - 2.*Mxz*Myz*Mxy + Mz*Mz*Cov_xy;

  A22 = A2 + A2;
  //  iter = 0;
  xnew = 0.;

  //    Newton's method starting at x=0

  int iter, iterMax = 20;
  for (iter=0; iter < iterMax; iter++) {
    ynew = A0 + xnew*(A1 + xnew*(A2 + 4.*xnew*xnew));

    if (fabs(ynew)>fabs(yold)) {
      //          printf("Newton2 goes wrong direction: ynew=%f
      //          yold=%f\n",ynew,yold);
      xnew = 0.;
      break;
    }

    Dy = A1 + xnew*(A22 + 16.*xnew*xnew);
    xold = xnew;
    xnew = xold - ynew/Dy;
    
    if (fabs(xnew) < epsilon) break;  
    if (fabs((xnew-xold)/xnew) < epsilon) break;
  }

  if (iter == iterMax-1) {
    //      printf("Newton2 does not converge in %d
    //      iterations\n",iterMax);
    xnew = 0.;
  }

  if (xnew < 0.) {
    iter=30;
    //      printf("Negative root: x=%f\n",xnew);
  }

  //    computing the circle parameters

  double GAM,DET;
  double Xcenter,Ycenter,Radius;

  GAM = - Mz - xnew - xnew;
  DET = xnew*xnew - xnew*Mz + Cov_xy;
  if (DET==0) return 0;

  Xcenter = (Mxz*(Myy-xnew) - Myz*Mxy)/DET/2.;
  Ycenter = (Myz*(Mxx-xnew) - Mxz*Mxy)/DET/2.;
  Radius = sqrt(Xcenter*Xcenter+Ycenter*Ycenter-GAM);

//   cout << " x center " <<  Xcenter + Mx <<
//     " y center " << Ycenter + My <<
//     " radius " << Radius << endl;

  track->SetCircle(Xcenter + Mx, Ycenter + My, Radius);

  return 1;
}


//_____________________________________________________________________________
Int_t PndTpcLheTrackFitter::DeepFitOleg(PndTpcLheTrack *track) {

  //---  line fit of array of points

  TObjArray *rHits = (TObjArray* )track->GetRHits();

  PndTpcLheHit *first = (PndTpcLheHit *)rHits->First();

  Double_t dx, dy ;
  dx = first->GetX() - track->GetVertex().GetX();
  dy = first->GetY() - track->GetVertex().GetY();;

  PndTpcLhePoint circ = track->GetCircle();

  Double_t radius = circ.GetZ();

  Double_t localPsi = 0.5F * sqrt ( dx*dx + dy*dy ) / radius ;
  Double_t total_s ;

  if ( fabs(localPsi) < 1. ) {
    total_s = 2.0 * radius * asin ( localPsi ) ;
  }
  else {
    total_s = 2.0 * radius * TMath::Pi() ;
  }


  Double_t sum = 0.F ;
  Double_t ss  = 0.F ;
  Double_t sz  = 0.F ;
  Double_t sss = 0.F ;
  Double_t ssz = 0.F ;

  Int_t nHits = rHits->GetEntries();

  Double_t dpsi, s;
  Double_t fS[nHits];
  Double_t fZWeight[nHits];


  for(Int_t i = 0; i < nHits; i++) {
    PndTpcLheHit *hit = (PndTpcLheHit *)rHits->At(i);
    fZWeight[i] = 1./(hit->GetZerr()*hit->GetZerr());

    if(i > 0) {
      PndTpcLheHit *last = (PndTpcLheHit *)rHits->At(i-1);
      dx = hit->GetX() - last->GetX();
      dy = hit->GetY() - last->GetY();
      dpsi = 0.5 * (Double_t)sqrt ( dx*dx + dy*dy ) / radius ;

      if(fabs(dpsi) > 1)
	{
	  cout << "-W-  PndTpcLheTrackFitter ::DeepFit: dpsi>1 -> skipped track" << endl;
	  return 1;
	}
      
      //      track->SetPsierr(dpsi);
      s = fS[i-1] - 2.0 * radius * (Double_t)asin ( dpsi ) ;
      fS[i]=s;
    }
    else
      fS[i] = total_s;

      sum += fZWeight[i];
      ss  += fZWeight[i] * fS[i];
      sz  += fZWeight[i] * hit->GetZ();
      sss += fZWeight[i] * fS[i] * fS[i];
      ssz += fZWeight[i] * fS[i] * hit->GetZ();

    }

  Double_t chi2;

  Double_t det = sum * sss - ss * ss;

  if ( fabs(det) < 1e-20) {
    chi2 = 99999.F ;
    cout << "-W-  PndTpcLheTrackFitter ::DeepFit: det==0 -> skipped track" << endl;
    //track->SetChiSq2(chi2);
    return 0 ;
  }

  // tan \lambda = dSdZ

  Double_t tanl = (Double_t)((sum * ssz - ss * sz ) / det );
  Double_t z0   = (Double_t)((sz * sss - ssz * ss ) / det );

  track->SetTanDipAngle(tanl);
  track->SetZ0(z0);

  //  cout << "tanl " << tanl << "   z0 " << z0 << endl;

  //     calculate chi-square

  chi2 = 0.;
  Double_t r1 ;

  for(Int_t i=0; i < nHits; i++) {
    PndTpcLheHit *hit = (PndTpcLheHit *)rHits->At(i);
    r1   = hit->GetZ() - tanl * fS[i] - z0 ;
    chi2 += (fZWeight[i]) * (r1 * r1);
  }

  track->SetDipChi2(chi2);

  //  should be done later
  //     calculate estimated variance
  //      varsq=chi/(double(n)-2.)
  //     calculate covariance matrix
  //      siga=sqrt(varsq*sxx/det)
  //      sigb=sqrt(varsq*sum/det)
  //

  Double_t dtanl = (Double_t) ( sum / det );
  Double_t dz0   = (Double_t) ( sss / det );

  track->SetTanDipAngleErr(dtanl);
  track->SetZ0Err(dz0);

  if (fVerbose) cout << "soeren track " << track->GetPx() << "\n";

  return 0 ;

}

//_____________________________________________________________________________
Int_t PndTpcLheTrackFitter::DeepFit(PndTpcLheTrack *track) {

  //---  line fit of array of points
  //--- Fit on the R vs z plane

  TObjArray *rHits = (TObjArray* )track->GetRHits();
  Int_t nHits = rHits->GetEntries();
  
  Double_t wsum  = 0.F ;
  Double_t wx  = 0.F ;
  Double_t wy  = 0.F ;
  Double_t wxx = 0.F ;
  Double_t wxy = 0.F ;
  Double_t mm = 0.;
  Double_t qq = 0.;
  
  for(Int_t i = 0; i < nHits; i++) {
    PndTpcLheHit *hit = (PndTpcLheHit *)rHits->At(i);
    
    Double_t rho = sqrt(hit->GetX()*hit->GetX()+hit->GetY()*hit->GetY());
    Double_t fZWeight = 1./(hit->GetZerr()*hit->GetZerr());
    
    wsum += fZWeight;
    wx  += fZWeight * rho;
    wy  += fZWeight * hit->GetZ();
    wxx += fZWeight * rho * rho;
    wxy += fZWeight * rho * hit->GetZ();
  } 
  
  Double_t det = wsum * wxx - wx * wx;
  if (det==0)
    {
      cout << "-W-  PndTpcLheTrackFitter ::DeepFit: det==0 -> skipped track" << endl;
      track->SetGood(kFALSE);
      return 0;
    }
  else 
    {
      mm = (wxy * wsum - wy * wx) / det;
      qq = (wy * wxx - wxy * wx) / det;
    }
  
  track->SetTanDipAngle(-mm);
  track->SetZ0(qq);
  
  
  //     calculate chi-square
  Double_t r1 ;
  Double_t chi2 = 0.;
  
  for(Int_t i=0; i < nHits; i++) {
    PndTpcLheHit *hit = (PndTpcLheHit *)rHits->At(i); 
    
    Double_t rho = sqrt(hit->GetX()*hit->GetX()+hit->GetY()*hit->GetY());
    Double_t fZWeight = 1./(hit->GetZerr()*hit->GetZerr());
    r1   = hit->GetZ() - mm * rho - qq;
    chi2 += fZWeight * (r1 * r1);
  }
  
  track->SetDipChi2(chi2);
  
  //  should be done later
  //     calculate estimated variance
  //      varsq=chi/(double(n)-2.)
  //     calculate covariance matrix
  //      siga=sqrt(varsq*sxx/det)
  //      sigb=sqrt(varsq*sum/det)
  //
  
  Double_t varsq = sqrt(chi2/((Double_t)nHits-2.));
  
  
  Double_t dtanl = varsq * sqrt( wsum / det );
  Double_t dz0   = varsq * sqrt( wxx / det );

  track->SetTanDipAngleErr(dtanl);
  track->SetZ0Err(dz0);

  if (fVerbose) cout << "soeren track " << track->GetPx() << "\n";

  return 1;

}

//_____________________________________________________________________________
Int_t PndTpcLheTrackFitter::HelixFit(PndTpcLheTrack *track) {
  //---  Create helix as fit of array of points

    Int_t isCircle = CircleFit(track);
    Int_t isDip = DeepFit(track);

    return (isCircle+isDip);

}

//_____________________________________________________
void PndTpcLheTrackFitter::Finish() {
  //---

 // CbmRootManager *fManger =CbmRootManager::Instance();
 // fManger->Fill();
  
 /* fXYF->Write();
  fYZF->Write();
  fXZF->Write();

  fXYG->Write();
  fYZG->Write();
  fXZG->Write();
   */
    printf("\n\n  *** Finish ***");

}


ClassImp(PndTpcLheTrackFitter)
