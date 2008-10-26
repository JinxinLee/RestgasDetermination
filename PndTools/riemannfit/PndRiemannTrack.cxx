//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndRiemannTrack
//      see PndRiemannTrack.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndRiemannTrack.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>
#include <cmath>

// Collaborating Class Headers --------
#include "PndRiemannHit.h"
#include "TGraph.h"
#include "TMath.h"
#include "TMatrixTSym.h"
#include "TF1.h"
// Class Member definitions -----------

void MatrixOutput(TMatrixD mat)
{
	for (int col = 0; col < mat.GetNcols(); col++)
	{
		for (int row = 0; row < mat.GetNrows(); row++)
		{
			std::cout << mat[row][col] << " ";
		}
		std::cout << std::endl;
	}
}

ClassImp(PndRiemannTrack);

PndRiemannTrack::PndRiemannTrack() :
	fn(3),fav(3), fc(0), fcovPlane(4,4), fjacRXY(3,4), fcovRXY(3,3),
	fVerbose(0), fFitDone(false), fSZFitDone(false)
{}


void
PndRiemannTrack::init(double x0_, double y0_, double R_, 
			 double dip, double z0){
/*  double x0=x0_/100;
  double y0=y0_/100;
  double R=R_/100;

  double R2=R*R;
  double lambda=R2-x0*x0-y0*y0;
  double D=lambda/sqrt(4*R2+(1-lambda)*(1-lambda));
  double DC=-D/lambda;
  double C=DC-D;
  double A=-2*x0*DC;
  double B=-2*y0*DC;
  
  fn[0]=A;
  fn[1]=B;
  fn[2]=C;
  fc=D;
  */

	fn[1] = TMath::Sqrt(4*y0_*y0_/(4*y0_*y0_+4*x0_*x0_+1));
	fn[0] = x0_/y0_*fn[1];
	fn[2] = TMath::Sqrt(1 - fn[0]*fn[0] - fn[1]*fn[1]);
	double fn2 = fn[2] * fn[2];
	fc = fn2*4*R_*R_-1+fn2/(-4*fn[2]);
	
	fm = tan(acos(dip));
	
}


void
PndRiemannTrack::addHit(PndRiemannHit* hit){
  int nbefore=fHits.size();
  fHits.push_back(*hit);
  //fav*=(double)nbefore;
  fav[0]+=hit->x().X() /(hit->sigmaXY()*hit->sigmaXY());
  fav[1]+=hit->x().Y() /(hit->sigmaXY()*hit->sigmaXY());;
  fav[2]+=hit->x().Z() /(hit->sigmaXY()*hit->sigmaXY());;
  //fav*=1./(double)(nbefore+1);
  fweight += 1/(hit->sigmaXY()*hit->sigmaXY());
  fFitDone = false;
  fSZFitDone = false;
}

double
PndRiemannTrack::dist(PndRiemannHit* hit){
  double d2=fc;
  d2+=hit->x().X()*fn[0];
  d2+=hit->x().Y()*fn[1];
  d2+=hit->x().Z()*fn[2];
  return d2;
}


void
PndRiemannTrack::refit()
{
  
  TMatrixT<double> av(3,1);
  if (fVerbose > 1) std::cout << "fweight: " << fweight << std::endl;
  fav *= TMath::Power(fweight,-1); //TS
  av[0][0]=fav[0];// *fweight; //TS *fweight added
  av[1][0]=fav[1];// *fweight;
  av[2][0]=fav[2];// *fweight;
  
  if (fVerbose > 0) std::cout << "av: " << fav[0] << " " << fav[1] << " " << fav[2] << std::endl;

  TMatrixD sampleCov(3,3);
  
  int nh=fHits.size();
  for(int i=0;i<nh;++i){
    TMatrixD h(3,1);
    h[0][0]=fHits[i].x().X();
    h[1][0]=fHits[i].x().Y();
    h[2][0]=fHits[i].x().Z();
    TMatrixD d(3,1);
    d=h-av;
    TMatrixD dt(TMatrixD::kTransposed,d);
    TMatrixD ddt(d,TMatrixD::kMult,dt);
    ddt *= 1/(fHits[i].sigmaXY()*fHits[i].sigmaXY());  //TS
    sampleCov+=ddt;  
  }

  sampleCov*=1./(double)nh;
  
  if (fVerbose > 1) std::cout << "sampleCov: " << std::endl;
  if (fVerbose > 1) MatrixOutput(sampleCov);
  
  TVectorD eigenValues(3);
  TMatrixD eigenVec=sampleCov.EigenVectors(eigenValues);
    
  // find smallest eigenvalue
  double val=eigenValues[0]; int g=0;
  for(int k=1;k<3;++k){
    if(eigenValues[k]<val){
      val=eigenValues[k];
      g=k;
    }
  }
  
  fn=TMatrixDColumn(eigenVec,g);
  
  if (fVerbose > 1) std::cout << "eigenVec: " << std::endl;
  if (fVerbose > 1) MatrixOutput(eigenVec);
  
  if (fVerbose > 1) std::cout << "eigenValues: ";
  for (int i = 0; i < 3; i++){
	  if (fVerbose > 1) std::cout << eigenValues[i] << " ";
  }
  if (fVerbose > 1) std::cout << std::endl;
 
  double norm=1./TMath::Sqrt(fn.Norm2Sqr());
  fn*=norm;
  fc=-1.*fn*fav;
  
  if (fVerbose > 1) std::cout << "fn: " << fn[0] << " " << fn[1] << " " << fn[2] << std::endl;
  if (fVerbose > 1) std::cout << "fc: " << fc << std::endl;
  //------------ Calculation of the full covariance matrix -----------
  
  // 1. Calculation of the covarianz matrix of the normal vector
  
  TMatrixD  CovNorm(3,3);
  
  for (int i = 0; i < 3; i++){
	if (i != g){
		TMatrixD res = eigenVec.GetSub(0,2,i,i)*eigenVec.GetSub(0,2,i,i).T();
		CovNorm += res * (val * eigenValues[i]/TMath::Power(val-eigenValues[i],2));
	}
  }
  if (fHits.size() > 5)
	  CovNorm *= TMath::Power(fHits.size()-5,-1);  //-5 is very strange. According to the paper this value has to be fixed with simulation???
  else 
	  CovNorm *= TMath::Power(fHits.size(),-1);
  if (fVerbose > 1) std::cout << "CovNorm: " << std::endl;
  if (fVerbose > 1) MatrixOutput(CovNorm);

  
  // 2. Calculation of the covarianz matrix of fav
  
  TMatrixD covAv(3,3);
  for (int i = 0; i < fHits.size();i++){
	  covAv[0][0] += fHits[i].covX(0,0)/(TMath::Power(fHits[i].sigmaXY(),4));
	  covAv[1][1] += fHits[i].covX(1,1)/(TMath::Power(fHits[i].sigmaXY(),4));
	  covAv[1][0] += fHits[i].covX(1,0)/(TMath::Power(fHits[i].sigmaXY(),4));
	  covAv[2][0] += 2 * (fHits[i].covX(0,0) * fHits[i].x().X() + fHits[i].covX(1,0) * fHits[i].x().Y())
	  				 /(TMath::Power(fHits[i].sigmaXY(),4));
	  covAv[2][1] += 2 * (fHits[i].covX(1,1) * fHits[i].x().Y() + fHits[i].covX(1,0) * fHits[i].x().X())
		             /(TMath::Power(fHits[i].sigmaXY(),4));
	  covAv[2][2] += 4 * (fHits[i].covX(0,0) * fHits[i].x().X() * fHits[i].x().X() +
			  			  2 * fHits[i].covX(1,0) * fHits[i].x().X() * fHits[i].x().Y() +
			  			  fHits[i].covX(1,1) * fHits[i].x().Y() * fHits[i].x().Y())
			  		 /(TMath::Power(fHits[i].sigmaXY(),4));
  }
  covAv[0][1] = covAv[1][0];
  covAv[0][2] = covAv[2][0];  
  covAv[1][2] = covAv[2][1];
  
  covAv *= TMath::Power(fweight,-2);

  
  if (fVerbose > 1) std::cout << "covAv: " << std::endl;
  if (fVerbose > 1) MatrixOutput(covAv);
  
  
  // 3. Calculation of var(fc)
  
  double nCrn = (fn * (covAv * fn));
  double rCnr = (fav * (CovNorm * fav));
  
  TMatrixD CnCr(CovNorm,TMatrixD::kMult,covAv);
  double trCnCr = 0;
  for (int i = 0; i < CnCr.GetNcols(); i++){
	  trCnCr += CnCr[i][i];
  }
  if (fVerbose > 1) std::cout << "nCrn: " << nCrn << " rCnr: " << rCnr << " trCnCr: " << trCnCr << std::endl;
  double varc = nCrn + rCnr + trCnCr;

  TVectorD corr_cn = CovNorm * fav;
  corr_cn *= -1;
  
  // 4. Calculation of the covarianz matrix of c,n
  
  fcovPlane[0][0] = varc;
  fcovPlane[1][1] = CovNorm[0][0];
  fcovPlane[2][1] = CovNorm[1][0];
  fcovPlane[2][2] = CovNorm[1][1];
  fcovPlane[3][1] = CovNorm[2][0];
  fcovPlane[3][2] = CovNorm[2][1];
  fcovPlane[3][3] = CovNorm[2][2];
  fcovPlane[1][0] = corr_cn[0];
  fcovPlane[2][0] = corr_cn[1];
  fcovPlane[3][0] = corr_cn[2];
  
  //5. Convert plane covariance in start parameter(r,x0,y0) covariances
  calcJacRXY();
  
  if (fVerbose > 1) std::cout << "jacRXY: " << std::endl;
  if (fVerbose > 1) MatrixOutput(fjacRXY);
  
  //fcovRXY = fjacRXY * fcovPlane * fjacRXY.T();
  
  TMatrixD temp(fjacRXY,TMatrixD::kMult, fcovPlane);
  if (fVerbose > 1) std::cout << "temp: " << temp.GetNrows() << "x" << temp.GetNcols() << std::endl;
  
  if (fVerbose > 1) std::cout << "temp: " << std::endl;
  if (fVerbose > 1) MatrixOutput(temp);
  
  fcovRXY = temp * fjacRXY.T();  // J * fcovPlane * J.T()

  if (fVerbose > 1) std::cout << "covPlane: " << std::endl;
  if (fVerbose > 1) MatrixOutput(fcovPlane);
  
  if (fVerbose > 1) std::cout << "covRXY: " << std::endl;
  if (fVerbose > 1) MatrixOutput(fcovRXY);
  
  fFitDone = true;
  fSZFitDone = false;
}


TVectorD
PndRiemannTrack::orig() const {
  TVectorD o(2);
  double den=TMath::Power((2 * fn[2]),-1);  // modified for paraboloid
  o[0]=-fn[0]*den;
  o[1]=-fn[1]*den;
  return o;
}


double 
PndRiemannTrack::r() const {
  if(fc==0)return 0;
  if(fabs(fc)>100)return 0;
  double a=2.*fn[2];  // modified for paraboloid
  //if(a==0){
  //  if (fVerbose > 0) std::cout<<"PndRiemannTrack:: a==0 cannot calc r! set r=1E4"<<std::endl;
  //  return 1.E-4;
  //}
  //if (fVerbose > 0) std::cout<<fn[0]<<"   "<<fn[1]<<"   "<<fc<<"   "<<a<<std::endl;
  double nom = 1-fn[2]*fn[2]-4*fc*fn[2];  // modified for paraboloid
  if(nom<0.0){
    nom=1.E-4;
    if (fVerbose > 1) std::cout<<"PndRiemannTrack::nom<0! set r=1E-4!"<<std::endl;
  }
  return sqrt(nom)/TMath::Abs(a);
}

double PndRiemannTrack::dR()
{
	return fcovRXY[0][0];
}

void
PndRiemannTrack::szFit(){
	if (fFitDone == false)
		refit();
  unsigned int n=getNumHits();  
  if (fVerbose > 0) std::cout << "szFit() for " << n << " Points!" << std::endl;
  
  TGraph g(n);
  // get s'es and zs
  for(unsigned int i=0;i<n;++i){
	if (fVerbose > 0) std::cout << "Point: " << i << ": ";
    fHits[i].calcPosOnTrk(this);
    if (fVerbose > 0) std::cout << fHits[i].s() << " " << fHits[i].z() << std::endl;
    g.SetPoint(i,fHits[i].s(),fHits[i].z());
  }

  g.Fit("pol1","Q0"); // << std::endl;
  TF1* f = g.GetFunction("pol1");
  //std::cout << "f: " << f << std::endl;
  ft = f->GetParameter(0);
  fm = f->GetParameter(1);
  ftError = f->GetParError(0);
  fmError = f->GetParError(1);
  fChi2   = f->GetChisquare();
  fSZFitDone = true;
  
  if (fVerbose > 0) std::cout << "t, m: " << ft << " +/- " << ftError << " / " << fm << " +/- " << fmError << " Chi2: " << fChi2 << std::endl;

  return;
}

double
PndRiemannTrack::calcSZChi2(PndRiemannHit* hit){
  // get s'es and zs
  unsigned int n=getNumHits();
  if (fVerbose > 0) std::cout << "szFit(hit) for " << n+1 << " Points!" << std::endl;
  TGraph g(n+1);
  for(unsigned int i=0;i<n;++i){
	if (fVerbose > 0) std::cout << "Point: " << i<< ": ";     
    fHits[i].calcPosOnTrk(this);
    if (fVerbose > 0) std::cout << fHits[i].s() << " " << fHits[i].z() << std::endl;
    g.SetPoint(i,fHits[i].s(),fHits[i].z());
  }
  if (fVerbose > 0) std::cout << "Additional hit: ";
  hit->calcPosOnTrk(this);
  if (fVerbose > 0) std::cout << hit->s() << " " << hit->z() << std::endl;
  g.SetPoint(n, hit->s(), hit->z());
  g.Fit("pol1","Q0");
  TF1* f = g.GetFunction("pol1");

//  fChi2 = chis;
  if (fVerbose > 0) std::cout << "t, m: " << f->GetParameter(0) << " +/- " << f->GetParError(0)
  							  << " / "    << f->GetParameter(1) << " +/- " << f->GetParError(1)
  							  << "Chi2: " << f->GetChisquare()  << std::endl;
//  delete(f);
  return f->GetChisquare();
}

double
PndRiemannTrack::szDist(PndRiemannHit* hit){
	
  if (fSZFitDone == false)
		szFit();
  hit->calcPosOnTrk(this);
  double hits=hit->s();
  double predz=hits*fm+ft;
  return predz-hit->z();
}

double PndRiemannTrack::szError(PndRiemannHit* hit){
	if (fSZFitDone == false)
		szFit();
	hit->calcPosOnTrk(this);
	double hits=hit->s();
	double result = TMath::Sqrt(TMath::Power((hits*fmError),2)+TMath::Power(ftError,2));
	//if (fVerbose > 0) std::cout << "Error on szDist is: " << result;
	return result;
}

// only after szFit!
double
PndRiemannTrack::dip() {
	if (fSZFitDone == false)
		szFit();
	return cos(atan(fm)); 
}

double PndRiemannTrack::dDip()
{
	if (fSZFitDone == false)
		szFit();
	return (fabs(sin(1/(1+fm*fm))) * fmError);
}


// Todo: check for backward going tracks!!!
double
PndRiemannTrack::sign() const {
  double s=fHits[3].s(); 
  return s<0 ? -1. : 1;
}

void PndRiemannTrack::calcJacRXY()
{
	double val = (TMath::Sqrt(1-fn[2]*fn[2]-4*fc*fn[2]));
	fjacRXY.Clear();
	fjacRXY.ResizeTo(3,4);
	fjacRXY[0][0] = -1/val;
	fjacRXY[0][3] = -1/(2*fn[2]) * (fn[2]+2*fc)/val - val/(2*fn[2]*fn[2]);
	fjacRXY[1][1] = -fn[0]/(2*fn[2]);
	fjacRXY[1][3] = fn[0]/(2*fn[2]*fn[2]);
	fjacRXY[2][2] = -fn[1]/(2*fn[2]);
	fjacRXY[2][3] = fn[1]/(2*fn[2]*fn[2]);
}
