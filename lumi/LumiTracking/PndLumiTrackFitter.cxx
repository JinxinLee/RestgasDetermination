/*
 * PndLumiTrackFitter.cxx
 *
 *  Created on: Aug 14, 2010
 *      Author: tsito
 */

// This Class' Header ------------------
#include "PndLumiTrackFitter.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndLinTrack.h"
#include "PndLumiTrack.h"

#include "TFile.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"


// Fit Classes -----------
#include "TMath.h"
#include "TMinuit.h"
#include "TMatrixD.h"

#include <TRandom2.h>
#include <algorithm>

#include <iostream>
#include <sstream>
//using namespace ROOT::Math;
using namespace std;

const int  n = 4;
/***
 *  Correlation Coefficient between planes :
 *   need to be upgraded according to the beam momentum
 ***/
Double_t corr[n][n];

/***
 *  Sigma on the residual distributions of the measured position
 *  and the impact point of the projection of the initial momentum
 *  vector at each plane
 ***/
Double_t sigma[n];

/***
 * Measured point coordinates
 ***/
Double_t xmes[n];
Double_t ymes[n];
Double_t zmes[n];

Double_t offsetZ;
void CHISQR(Int_t &npar, Double_t *gin, Double_t &f, Double_t *p, Int_t iflag)
{
	//Get covariance matrix
	Double_t matrix[n*n];

	for(int k =0; k<n; k++){
		for(int i =0; i<n; i++){
			matrix[(n*k)+i]=corr[i][k]*sigma[i]*sigma[k];
		}
	}

	//get the inverse of the covariance matrix
	Double_t *mat=matrix;

	TMatrixD mymat(n,n, mat);

	Double_t det = mymat.Determinant();

	TMatrixD myinvmat(n,n);

	myinvmat =mymat.Invert(&det);

	Double_t* matinv;

	matinv= myinvmat.GetMatrixArray();

	Double_t invcov[n][n];

	for(int i = 0; i<n ; i++){
		for(int j =0; j<n; j++){
			invcov[i][j]=matinv[(i*n)+j];
		}
	}

	//calcul of the chi2
//	Double_t chisqr = 0.0;
	Double_t DX=0.0, DY=0.0;
	for(Int_t i= 0; i<n; i++){
		Double_t Dx=0.0, Dy=0.0;
		for(Int_t j=0; j<n; j++){
			Dx += (xmes[j]-(p[2]+p[0]*(zmes[j]-offsetZ)))*invcov[j][i];
			Dy += (ymes[j]-(p[3]+p[1]*(zmes[j]-offsetZ)))*invcov[j][i];

		}
		DY += Dy*(ymes[i]-(p[3]+p[1]*(zmes[i]-offsetZ)));
		DX += Dx*(xmes[i]-(p[2]+p[0]*(zmes[i]-offsetZ)));
//		chisqr += (DY+DX);
	}
	Double_t chisqr = DY+DX;

	f=chisqr;


	return;
}


PndLumiTrackFitter::PndLumiTrackFitter()
  : FairTask("Lumi linear track fitter")
{
}

PndLumiTrackFitter::PndLumiTrackFitter(Double_t dx, Double_t dy,Int_t verb)
  : FairTask("Lumi linear track fitter")
{
	fVerbose = verb;
	fdX = dx;
	fdY = dy;
}

PndLumiTrackFitter::~PndLumiTrackFitter()
{

}

void PndLumiTrackFitter::SetParContainers()
{
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	fFitPar = (PndLumiLinFitPar*)(rtdb->getContainer("LumiLinFitPar"));
}

InitStatus PndLumiTrackFitter::ReInit()
{
   SetParContainers();
   return kSUCCESS;
}


InitStatus PndLumiTrackFitter::Init()
{
  //Get ROOT Manager
	FairRun* ana = FairRun::Instance();
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndLumiTrackFitter::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fLumiTrack=(TClonesArray*) ioman->GetObject("LumiTrack");
  if(fLumiTrack==0)
    {
      Error("PndLumiTrackFitter::Init","Track Candidates Array not found!");
      return kERROR;
    }

  fTrackArray = new TClonesArray("PndLinTrack");
  ioman->Register("LinTrack", "Lumi", fTrackArray, kTRUE);

  if(! fFitPar){
	  std::cout<<"-E- PndLumiTrackFitter::FitPar Container does not exist!"<<std::endl;
	  return kERROR;
  }
  fFitPar->Print();

  std::cout << "-I- PndLumiTrackFitter: Initialization successfull" << std::endl;

  return kSUCCESS;
}


void PndLumiTrackFitter::Exec(Option_t* opt)
{
	sigma[0]=fFitPar->sigma1();
	sigma[1]=fFitPar->sigma2();
	sigma[2]=fFitPar->sigma3();
	sigma[3]=fFitPar->sigma4();

	corr[0][0]=fFitPar->r11(); corr[0][1]=fFitPar->r12(); corr[0][2]=fFitPar->r13(); corr[0][3]=fFitPar->r14();
	corr[1][0]=fFitPar->r21(); corr[1][1]=fFitPar->r22(); corr[1][2]=fFitPar->r23(); corr[1][3]=fFitPar->r24();
	corr[2][0]=fFitPar->r31(); corr[2][1]=fFitPar->r32(); corr[2][2]=fFitPar->r33(); corr[2][3]=fFitPar->r34();
	corr[3][0]=fFitPar->r41(); corr[3][1]=fFitPar->r42(); corr[3][2]=fFitPar->r43(); corr[3][3]=fFitPar->r44();

	// Reset output Array
	if(fTrackArray==0) Fatal("PndLumiTrackFitter::Exec","No TrackArray");
	fTrackArray->Delete();

	Int_t nTCand = fLumiTrack->GetEntriesFast();

	// Fitting ----------------------------------------------------------------------------------
	if(fVerbose>1)std::cout<<" -I- PndLumiTrackFitter: start Fitting "<<std::endl;

	std::vector<TVector3> trkcand_i;

	std::vector<TVector3> trkcand;

	std::vector<TVector3> hiterr;

	Int_t tckId;

	Double_t x0, y0, ax, ay;
	Double_t s_x, s_y;
	Int_t ik = fTrackArray->GetEntriesFast();
	for(Int_t itrk=0; itrk<nTCand; itrk++){
		trkcand_i.clear();
		trkcand.clear();
		hiterr.clear();

		PndLumiTrack* myTCand = (PndLumiTrack*)fLumiTrack->At(itrk);

		trkcand_i = myTCand->GetTrackCandidates();
		hiterr = myTCand->GetTrackCandErrors();
		tckId = myTCand->GetMCIndex();

		if (IsGoodCandidate(trkcand_i)){
			AscendingOrder(trkcand_i, trkcand);

			for (Int_t k = 0; k<trkcand.size(); k++){

				xmes[k] = trkcand[k].X();
				ymes[k] = trkcand[k].Y();
				zmes[k] = trkcand[k].Z();
			}
			offsetZ = zmes[0];
			Double_t parFit[4]; Double_t error[4];
			Double_t accuracy =line3Dfit(parFit, error);

			Int_t firstHit = 1; Int_t lastHit = 4;

			new((*fTrackArray)[ik]) PndLinTrack("Lumi", parFit[0], parFit[1], parFit[2], parFit[3],
					error[0], error[1], error[2], error[3], accuracy, firstHit, lastHit, tckId);
		}
	}// end of TCand's
}

void PndLumiTrackFitter::AscendingOrder(std::vector<TVector3> oneCandidate, std::vector<TVector3> &orderedvec)
{
	std::map<Double_t,TVector3> mapcand;
	for(Int_t i = 0; i < oneCandidate.size(); i++) {
		mapcand[oneCandidate[i].Z()]=oneCandidate[i];
	}

	std::map<Double_t,TVector3>::iterator it;
	for(it=mapcand.begin(); it!=mapcand.end(); ++it){
		orderedvec.push_back(it->second);
	}
}

Bool_t PndLumiTrackFitter::IsGoodCandidate(std::vector<TVector3> oneCandidate)
{
	std::vector<Double_t> planepos;
	for(Int_t i = 0; i < oneCandidate.size(); i++) {
		planepos.push_back(oneCandidate[i].Z());
	}
	sort(planepos.begin(), planepos.end());

	std::vector<Double_t>::iterator new_end_pos;
	new_end_pos = unique(planepos.begin(), planepos.end());
	planepos.erase(new_end_pos, planepos.end());

	if(planepos.size()!=4) return kFALSE;

	return kTRUE;

}
Double_t PndLumiTrackFitter::line3Dfit(Double_t* fitpar, Double_t* fiterrpar)
{
	const Int_t npar = 4;
	Double_t arglist[10];
	arglist[0] = 1;
	Int_t ierflg = 0;

	Int_t nprm[npar] = {0 , 1 , 2 , 3 };
	Double_t startval[4] = {1e-2,  1e-2, xmes[0], ymes[0] };
	Double_t step[4] = {1e-3, 1e-3, 1e-3, 1e-3 };
	Double_t p1=1;
	Double_t p2=2;
	Double_t p3=3;
	Double_t p4=4;
	Double_t eps=1e-15;

	TMinuit *minuit = new TMinuit(npar);  //initialize TMinuit with a maximum of 4 params
	minuit->mninit(5,6,7);
	minuit->SetFCN(CHISQR);
	Double_t ip = fVerbose-1.0;
	minuit->mnexcm("SET PRINT", &ip, 1, ierflg);
	minuit->mnexcm("SET EPS", &eps, 1, ierflg);
	minuit->mnexcm("SET ERR", arglist ,1,ierflg);
	minuit->mnexcm("SET WARNING", arglist ,-1,ierflg);

	Double_t xmin = xmes[0] - fdX;
	Double_t xmax = xmes[0] + fdX;
	Double_t ymin = ymes[0] - fdY;
	Double_t ymax = ymes[0] + fdY;


	minuit->mnparm(nprm[0], "ax", startval[0], step[0], -5.0 , 5.0, ierflg);
	minuit->mnparm(nprm[1], "ay", startval[1], step[1], -5.0 , 5.0 , ierflg);
	minuit->mnparm(nprm[2], "x0", startval[2], step[2], xmin , xmax , ierflg);
	minuit->mnparm(nprm[3], "y0", startval[3], step[3], ymin , ymax , ierflg);



	arglist[0] = 2000;
	arglist[1] = 1.;

	minuit->mnexcm("FIX", &p3 ,1,ierflg);
	minuit->mnexcm("FIX", &p4 ,1,ierflg);
	minuit->mnexcm("MIGRAD", arglist ,2,ierflg);
	minuit->mnexcm("MINOS", arglist ,2,ierflg);
	minuit->mnexcm("RELEASE", &p3 ,1,ierflg);
	minuit->mnexcm("RELEASE", &p4 ,1,ierflg);
	minuit->mnexcm("MIGRAD", arglist ,2,ierflg);
	minuit->mnexcm("MINOS", arglist ,2,ierflg);

	Int_t nvpar,nparx;
	Double_t amin,edm, errdef;
	Int_t istat;
	minuit->mnstat(amin,edm,errdef,nvpar,nparx, istat);

	Double_t outpar[npar], err[npar];
	// get fit parameters
	for (Int_t k = 0; k <4; ++k){
		minuit->GetParameter(k, outpar[k], err[k]);
		fitpar[k]=outpar[k];
		fiterrpar[k]=err[k];
//		cout<<fitpar[k]<<endl;
	}
	minuit->Clear();
	return amin;

}

ClassImp(PndLumiTrackFitter);
