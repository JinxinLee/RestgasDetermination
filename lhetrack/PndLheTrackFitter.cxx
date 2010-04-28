#include "PndLheTrackFitter.h"
#include "PndLheCandidate.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "PndTrackCand.h"
//#include "lhe.h"

#include "FairTrackParP.h"
#include "FairField.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"

#include <cmath>
#include "TMinuit.h"

// ---------------------------------------------------------------
// --- Interface with TrackFinder and output ---


PndLheTrackFitter* PndLheTrackFitter::ftInstance = NULL;

//___________________________________________________________
PndLheTrackFitter* PndLheTrackFitter::Instance() {
  //---
  return ftInstance;
}

//___________________________________________________________
PndLheTrackFitter::~PndLheTrackFitter() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndLheTrackFitter::PndLheTrackFitter() {
  //---
  fPndTracks = new TClonesArray("PndTrack");
  fPndTrackIds = new TClonesArray("PndTrackID");
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  fCircleFit = 0;
  if( !ftInstance ) ftInstance = this;
}

//___________________________________________________________
PndLheTrackFitter::PndLheTrackFitter(const char *name, const char *title)
  :FairTask(name) {
  //---
  fPndTracks = new TClonesArray("PndTrack");
  fPndTrackIds = new TClonesArray("PndTrackID");
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  fCircleFit = 0;
  fPersistence = kTRUE;
  if( !ftInstance ) ftInstance = this;
}

//_________________________________________________________________
void PndLheTrackFitter::Register() {
  //---
  FairRootManager::Instance()->Register("LheTrack",
  			 "Lhe", fPndTracks, fPersistence);
 FairRootManager::Instance()->Register("LheTrackID",
  			 "Lhe", fPndTrackIds, fPersistence);
}

//________________________________________________________________
void PndLheTrackFitter::Reset() {
  //---
  if (fPndTracks->GetEntriesFast() != 0)  fPndTracks->Clear("C");
  if (fPndTrackIds->GetEntriesFast() != 0)  fPndTrackIds->Clear("C");
}
//___________________________________________________________
InitStatus PndLheTrackFitter::Init() {

  //  cout << "InitStatus PndLheTrackFitter::Init()" << endl;

  FairRootManager *fManager =FairRootManager::Instance();	

  fTpcHits = (TClonesArray *)fManager->GetObject("LheHit");

  fTpcPoints   = (TClonesArray *)fManager->GetObject("PndTpcPoint");
  if ( fTpcPoints != 0) {
    cout << "-I- PndLheTrackFitter::Init: TpcPoint array! Switching simulation ON" << endl;
    fSimulation = kTRUE;
    //cout << "-I- "<< GetName() << "::Init: No TpcPoint array!" << endl;
    //return kERROR;
  }

  fTpcTracks = (TClonesArray *)fManager->GetObject("LheCandidate");
  if ( ! fTpcTracks ) {
    cout << "-I- "<< GetName() << "::Init: No LheCandidate array!" << endl;
    return kERROR;
  }

  switch (fCircleFit)
    {
    case 0:
      cout << "-I- PndLheTrackFitter::Init: Using Oleg's fit" << endl;
      break;
    case 1:
      cout << "-I- PndLheTrackFitter::Init: Using TMinuit fit" << endl;
      break;
    default:
      cout << "-E- PndLheTrackFitter::Init: Wrong fitting method" << endl;
      return kERROR;
      break;
    }
  
  //  get the field in Memory:
  FairRunAna *fRun=FairRunAna::Instance();
  fMagField = (FairField*) fRun->GetField();

  fTrackCuts =PndLheTrackCuts::Instance();
  Register();
  
  return kSUCCESS;
}

//______________________________________________________
void PndLheTrackFitter::Exec(Option_t * option) {

  cout << " =====   PndLheTrackFitter   ===== " << endl;
 
  Reset();
  if (fTpcTracks) {
    cout << " Number of tracks for fitting " <<
      fTpcTracks->GetEntriesFast() << endl;
  }

  Int_t nTracks = fTpcTracks->GetEntriesFast();

  for (Int_t i = 0; i < nTracks; i++) {

    PndLheCandidate* track = (PndLheCandidate*) fTpcTracks->At(i);
    if (fVerbose) cout << "\n\n Track " << i << "\n"; 
    //    cout << "\n\n Track " << track->GetTrackNumber() << "\n"; 

    //    track->Print();

    if (HelixFit(track)>1)
      {   
	Info4Fit(track, i);
      }
    else
      {
	track->SetGood(kFALSE);
      }
    
  }

#if 0
  /** Constructor with all track variables  (x,y,z in SC) **/
  FairTrackParH(Double_t x,  Double_t y,  Double_t z,
	       Double_t lambda, Double_t phi, Double_t qp,
	       Double_t CovMatrix[15]);

  /** Constructor track parameters with position (LAB) momentum **/
  
  FairTrackParH(TVector3 pos, TVector3 Mom, TVector3 posErr,
	       TVector3 MomErr, Double_t q);

#endif


}

//_____________________________________________________________________________
void PndLheTrackFitter::Info4Fit(PndLheCandidate *track, Int_t idx) {
  //---

  TObjArray *rhits = (TObjArray* )track->GetRHits();
  Int_t nHits = rhits->GetEntriesFast();
  map<Int_t, Int_t> fMCTrackList; // MC TrackId, multiplicity
  map<Int_t, Int_t>::iterator iter;
  
  // alpha = .2998 * magfield / 100 <- for obtain moment in GeV/c
  Double_t alpha = .2998 * .02 ;
  Double_t Q = double(TMath::Sign(1, track->GetCharge()));
  
  PndLhePoint hel = track->GetCircle();
  
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
  
  PndTrackCand* trackCand = new PndTrackCand();
  FairTrackParP* firstPar;
  FairTrackParP* lastPar;
  // loop over hits
  for (int ih = 0; ih < nHits; ih++) {
    PndLheHit* hit = (PndLheHit*)rhits->At(ih);
    if (NULL==hit) break;
    
    trackCand->AddHit(hit->GetDetectorID(), hit->GetRefIndex(), hit->GetX()*hit->GetX()+hit->GetY()*hit->GetY()+hit->GetZ()*hit->GetZ());
    if ((ih==0) || (ih==(nHits-1)) ) // considering only first and last hit
      {
	TVector2 pnt(hit->GetX() - xc, hit->GetY() - yc);
	Double_t phi = pnt.DeltaPhi(pivot);
	//  Double_t phi = pnt.DeltaPhi(centre);
	Double_t x = d0*TMath::Cos(fi0) + rad *(TMath::Cos(fi0) - TMath::Cos(fi0 + phi));   
        Double_t y = d0*TMath::Sin(fi0) + rad *(TMath::Sin(fi0) - TMath::Sin(fi0 + phi));   
	//Double_t z = z0 - rad*lam*(phi)*TMath::Sign(-1.,phi);  // OLD
	Double_t z = z0 - rad*lam*phi; // STE
	
	Double_t px = -Q*alpha*rad*TMath::Sin(fi0 + phi);
	Double_t py =  Q*alpha*rad*TMath::Cos(fi0 + phi);
	//Double_t pz =  Q*alpha*rad*lam*TMath::Sign(-1.,phi); // OLD
	Double_t pz =  -alpha*rad*lam; // STE
	if (fabs(pz<1e-8))
	  {
	    track->SetGood(kFALSE);
	    break;
	  }
	if (ih==0) 
	  {
	    track->SetFirstHit(hit->GetX(),hit->GetY(),hit->GetZ());
	    firstPar = new FairTrackParP(TVector3(x, y, z),
					 TVector3(px, py, pz),
					 TVector3(0.5, 0.5, 0.5),
					 0.1*TVector3(px, py, pz),
					 track->GetCharge(),
					 TVector3(x, y, z),
					 TVector3(1.,0.,0.),
					 TVector3(0.,1.,0.));					 
	  }
	if (ih==(nHits-1))
	  {
	    track->SetLastHit(hit->GetX(),hit->GetY(),hit->GetZ());
	    track->SetPx(px);
	    track->SetPy(py);
	    track->SetPz(pz);
	    lastPar = new FairTrackParP(TVector3(x, y, z),
					TVector3(px, py, pz),
					TVector3(0.5, 0.5, 0.5),
					0.1*TVector3(px, py, pz),
					track->GetCharge(),
					TVector3(x, y, z),
					TVector3(1.,0.,0.),
					TVector3(0.,1.,0.));	
	    if (fVerbose) cout <<  " Fitted x, y, z: " << x << " " << y << " " << z
			       << " px, py, pz: " << px << " " << py << " " << pz << endl;
	  }
      }
    fMCTrackList[hit->GetTrackID()]++;
  }  
  
  multimap<Int_t, Int_t> fMCRevertedList; // multiplicity, MC TrackId
  Int_t count = 0;
  for (iter=fMCTrackList.begin(); iter!=fMCTrackList.end(); ++iter)
    {
      fMCRevertedList.insert(pair<Int_t, Int_t>(-(*iter).second, (*iter).first));
      count++;
    }
  
  TArrayI trackID(count);
  TArrayI multID(count);
  
  count = 0;
  for (iter=fMCRevertedList.begin(); iter!=fMCRevertedList.end(); ++iter)
    {
      trackID[count] = (*iter).second;
      multID[count] = -(*iter).first;
      count++;
    }
  
   track->SetTrackID(trackID, multID);

   // Filling PndTrack TCA
   if (track->IsGood()&&track->GetMomentum().Mag()>0.1&&track->GetMomentum().Mag()<20.)
     {
       TClonesArray &pndtracks = *fPndTracks;
       TClonesArray &pndtrackids = *fPndTrackIds;
       Int_t size = pndtracks.GetEntriesFast();
       trackCand->Sort();
       PndTrack* pndTrack = new(pndtracks[size]) PndTrack(*firstPar, *lastPar, *trackCand);
       pndTrack->SetRefIndex(idx);
       pndTrack->SetLink(FairLink(kLheCand, idx));
       PndTrackID* pndTrackId = new(pndtrackids[size]) PndTrackID(size ,trackID, multID);
     }
}

//_____________________________________________________________________________
Int_t PndLheTrackFitter::CircleFit(PndLheCandidate *track) {

  //---  From Ososkov CircleCOP() Comp.Phys.Com 33, p.329

  TObjArray *rhits = (TObjArray* )track->GetRHits();
  Int_t NHits = rhits->GetEntriesFast();

  double M0,Mx,My;
  M0 = NHits;
  Mx=My=0.;

  for (Int_t lh = 0; lh < NHits; lh++) {
    PndLheHit* hit = (PndLheHit*) rhits->At(lh);
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
    PndLheHit* hit = (PndLheHit*) rhits->At(lh);
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
    if (fabs(Dy) < epsilon) break;  
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
  if ((Xcenter*Xcenter+Ycenter*Ycenter-GAM) < 0.) return 0;
  Radius = sqrt(Xcenter*Xcenter+Ycenter*Ycenter-GAM);

//   cout << " x center " <<  Xcenter + Mx <<
//     " y center " << Ycenter + My <<
//     " radius " << Radius << endl;

  track->SetCircle(Xcenter + Mx, Ycenter + My, Radius);

  return 1;
}


//_____________________________________________________________________________
Int_t PndLheTrackFitter::DeepFitOleg(PndLheCandidate *track) {

  //---  line fit of array of points

  TObjArray *rHits = (TObjArray* )track->GetRHits();

  PndLheHit *first = (PndLheHit *)rHits->First();

  Double_t dx, dy ;
  dx = first->GetX() - track->GetVertex().GetX();
  dy = first->GetY() - track->GetVertex().GetY();;

  PndLhePoint circ = track->GetCircle();

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

  Int_t nHits = rHits->GetEntriesFast();

  Double_t dpsi, s;
  Double_t fS[nHits];
  Double_t fZWeight[nHits];


  for(Int_t i = 0; i < nHits; i++) {
    PndLheHit *hit = (PndLheHit *)rHits->At(i);
    fZWeight[i] = 1./(hit->GetDz()*hit->GetDz());

    if(i > 0) {
      PndLheHit *last = (PndLheHit *)rHits->At(i-1);
      dx = hit->GetX() - last->GetX();
      dy = hit->GetY() - last->GetY();
      dpsi = 0.5 * (Double_t)sqrt ( dx*dx + dy*dy ) / radius ;

      if(fabs(dpsi) > 1)
	{
	  cout << "-W-  PndLheTrackFitter ::DeepFit: dpsi>1 -> skipped track" << endl;
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
    cout << "-W-  PndLheTrackFitter ::DeepFit: det==0 -> skipped track" << endl;
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
    PndLheHit *hit = (PndLheHit *)rHits->At(i);
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
Int_t PndLheTrackFitter::DeepFit(PndLheCandidate *track) {

  //---  line fit of array of points
  //--- Fit on the R vs z plane

  TObjArray *rHits = (TObjArray* )track->GetRHits();
  Int_t nHits = rHits->GetEntriesFast();
  
  Double_t wsum  = 0.F ;
  Double_t wx  = 0.F ;
  Double_t wy  = 0.F ;
  Double_t wxx = 0.F ;
  Double_t wxy = 0.F ;
  Double_t mm = 0.;
  Double_t qq = 0.;
  
  for(Int_t i = 0; i < nHits; i++) {
    PndLheHit *hit = (PndLheHit *)rHits->At(i);
    
    Double_t rho = sqrt(hit->GetX()*hit->GetX()+hit->GetY()*hit->GetY());
    Double_t fZWeight = 1./(hit->GetDz()*hit->GetDz());
    
    wsum += fZWeight;
    wx  += fZWeight * rho;
    wy  += fZWeight * hit->GetZ();
    wxx += fZWeight * rho * rho;
    wxy += fZWeight * rho * hit->GetZ();
  } 
  
  Double_t det = wxx/wsum - wx * wx / wsum / wsum;
  if (det<=0)
    {
      cout << "-W-  PndLheTrackFitter ::DeepFit: det<=0 -> skipped track" << endl;
      track->SetGood(kFALSE);
      return 0;
    }
  else 
    {
      mm = (wxy / wsum - wy * wx / wsum / wsum) / det;
      qq = (wy * wxx /wsum / wsum - wxy * wx / wsum / wsum) / det;
    }
  
  track->SetTanDipAngle(-mm);
  track->SetZ0(qq);
  
  
  //     calculate chi-square
  Double_t r1 ;
  Double_t chi2 = 0.;
  
  for(Int_t i=0; i < nHits; i++) {
    PndLheHit *hit = (PndLheHit *)rHits->At(i); 
    
    Double_t rho = sqrt(hit->GetX()*hit->GetX()+hit->GetY()*hit->GetY());
    Double_t fZWeight = 1./(hit->GetDz()*hit->GetDz());
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
  
  Double_t dtanl = sqrt( wsum / det );
  Double_t dz0   = sqrt( wxx  / det );

  track->SetTanDipAngleErr(dtanl);
  track->SetZ0Err(dz0);

  if (fVerbose) cout << "soeren track " << track->GetPx() << "\n";

  return 1;

}

void fitCircle(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{

  TMatrixT<Double_t> *mama = (TMatrixT<Double_t> *)gMinuit->GetObjectFit();

  Double_t chisq = 0;
  Double_t delta = 0;
  Int_t hitcounter = mama->GetNrows();
  for (Int_t i = 0; i < hitcounter; i++)
    { 
      delta =sqrt((mama[0][i][0]-par[0])*(mama[0][i][0]-par[0])+(mama[0][i][1]-par[1])*(mama[0][i][1]-par[1])) -par[2];
      chisq += (delta*delta)/(mama[0][i][2] * mama[0][i][2] + mama[0][i][3] * mama[0][i][3]);
    }
  f = chisq; 
}

Int_t PndLheTrackFitter::SetUpFitVector(PndLheCandidate* pTrack, TMatrixT<Double_t> &fitvect)
{
    
  Int_t counter = 0;
  TObjArray* lheList = pTrack->GetRHits();
  Int_t nHits = lheList->GetEntriesFast();
  fitvect.ResizeTo(nHits, 4); // x y errx erry 
  
  for (Int_t lh=0; lh < nHits; lh++)
    {
      PndLheHit* lhit = (PndLheHit*)lheList->At(lh);
      fitvect[counter][0] = lhit->GetX();
      fitvect[counter][1] = lhit->GetY();
      fitvect[counter][2] = lhit->GetDx();
      fitvect[counter][3] = lhit->GetDy();
      counter++;
    }
  
  if(nHits != counter) {
    fitvect.ResizeTo(counter, 4); // x y errx erry
  }
  return counter;
}

//_____________________________________________________________________________
Int_t PndLheTrackFitter::FastCircleFit(PndLheCandidate *track, Double_t prefit[]) {
  //--- Circular fit using only three points
  TObjArray* lheList = track->GetRHits();
  Int_t nHits = lheList->GetEntriesFast();
  
  Double_t x[3], y[3];
  
  x[0] = 0.;   
  x[1] =((PndLheHit*)lheList->At(0))->GetX(); 
  x[2] =((PndLheHit*)lheList->At(nHits))->GetX(); 

  y[0] = 0.;   
  y[1] =((PndLheHit*)lheList->At(0))->GetY(); 
  y[2] =((PndLheHit*)lheList->At(nHits))->GetY(); 
    
  fTrackCuts->Circle3pnts(x, y, prefit);
  
  return 1;
}

//_____________________________________________________________________________
Int_t PndLheTrackFitter::CircleFitMinuit(PndLheCandidate *track) {
  //--- Circular fit on the XY plane using TMinuit
  TMinuit minimizer(3);
  TMatrixT<Double_t> fitVect;
  Int_t nFitHits = SetUpFitVector(track, fitVect);  //  set the object to be fitted
  minimizer.SetFCN(fitCircle);                       // Setting the fit function
  
  Double_t fStart[3];
  //FastCircleFit(track, fStart);
  cout << track->GetVertex().GetX() << "\t" << track->GetVertex().GetY() << "\t" << track->GetRadius() << endl;
  minimizer.DefineParameter(0, "xc", track->GetVertex().GetX(), 0.1, -3000., 3000.); // ???
  minimizer.DefineParameter(1, "yc", track->GetVertex().GetY(), 0.1, -3000., 3000.); // ??? LIMITS ???
  minimizer.DefineParameter(2, "r",  track->GetRadius(), 0.1,     1., 3000.);   // ???
  
  minimizer.SetObjectFit(&fitVect);
  minimizer.SetPrintLevel(-1); 
  minimizer.SetMaxIterations(500);
  minimizer.Migrad();
  
  Double_t chisquare, resultsRadial[3], errorsRadial[3]; 
  
  minimizer.GetParameter(0, resultsRadial[0], errorsRadial[0]);
  minimizer.GetParameter(1, resultsRadial[1], errorsRadial[1]);
  minimizer.GetParameter(2, resultsRadial[2], errorsRadial[2]);
    
  //  minimizer.Eval(3, NULL, chisquare, resultsRadial, 0); // ???
  
  cout << "xc: " << resultsRadial[0] << endl; 
  cout << "yc: " << resultsRadial[1] << endl; 
  cout << "R:  " << resultsRadial[2] << endl;
  
  track->SetCircle( resultsRadial[0], resultsRadial[1], resultsRadial[2]);
  
  return 1;
}

//_____________________________________________________________________________
Int_t PndLheTrackFitter::HelixFit(PndLheCandidate *track) {
  //---  Create helix as fit of array of points

  Int_t isCircle = 0;
  if (fCircleFit==0) isCircle = CircleFit(track);
  if (fCircleFit==1) isCircle = CircleFitMinuit(track);
  
  Int_t isDip = DeepFit(track);

  return (isCircle+isDip);

}

ClassImp(PndLheTrackFitter)
