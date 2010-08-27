//-----------------------------------------------------------
//
// Description:
//      3D Straight Line fitter
//
// Author List:
//      Mathias Michel
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndLmdLinFitTask.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndLinTrack.h"
#include "TrackData/PndTrackCand.h"
#include "PndSdsHit.h"
#include "TrackData/PndTrackCandHit.h"

#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"

// Fit Classes -----------
#include <TMath.h>
#include <TVector3.h>
#include <TRandom2.h>
//#include <TStyle.h>
//#include <TCanvas.h>
#include <TF2.h>
#include <TH1.h>
#include <TVirtualFitter.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <TMatrixTSym.h>

using namespace ROOT::Math;
using namespace std;

PndLmdLinFitTask::PndLmdLinFitTask()
  : FairTask("3D-Straight-Line-Fit")
{
  fTCandBranchName = "LMDTrackCand";
  fRecoBranchName = "LMDHitsStrip";
}


PndLmdLinFitTask::~PndLmdLinFitTask()
{

}

InitStatus PndLmdLinFitTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndLmdLinFitTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fTCandArray=(TClonesArray*) ioman->GetObject(fTCandBranchName);

  if(fTCandArray==0)
    {
      Error("PndLmdLinFitTask::Init","trackcand-array not found!");
      return kERROR;
    }

  fRecoArray=(TClonesArray*) ioman->GetObject(fRecoBranchName);

  if(fRecoArray==0)
    {
      Error("PndLmdLinFitTask::Init","reco-array not found!");
      return kERROR;
    }

  fTrackArray = new TClonesArray("PndLinTrack");
  ioman->Register("LMDTrack", "PndLmd", fTrackArray, kTRUE);

  std::cout << "-I- PndLmdLinFitTask: Initialisation successfull" << std::endl;

  return kSUCCESS;
}


void PndLmdLinFitTask::Exec(Option_t* opt)
{
  std::cout<<"PndLmdLinFitTask::Exec"<<std::endl;
  // Reset output Array
  if(fTrackArray==0) Fatal("PndLmdLinFitTask::Exec","No TrackArray");
   fTrackArray->Delete();

  Int_t ntcand=fTCandArray->GetEntriesFast();

  // Detailed output
  if(fVerbose>1)std::cout<<" -I- PndLmdLinFitTask: contains "<<ntcand<<" TCandidates"<<std::endl;
  if(fVerbose>2){
    std::cout<< " Detailed Debug info on the candidates:"<<std::endl;
    unsigned int detid=12345, index=12345;
    for(Int_t itr=0;itr<ntcand;++itr){
      PndTrackCand* trcnd = (PndTrackCand*)fTCandArray->At(itr);
      std::cout<< "TrackCand no. "<<itr<<" has "<<trcnd->GetNHits()<<" hits."<<std::endl;
      std::cout << "Point: \t Index: "<<std::endl;
      for(unsigned int ihit=0; ihit<trcnd->GetNHits(); ihit++){ //fill Graph
        PndTrackCandHit theHit = trcnd->GetSortedHit(ihit); //get hit
        index = theHit.GetHitId();
        detid = theHit.GetDetId();
        std::cout << ihit << "\t" << index <<std::endl;
      }
    }
  }

  // Cut evil event
  //if(ntcand>20){
  //  std::cout<<"ntcand="<<ntcand<<" Evil Event! skipping"<<std::endl;
  //  return;
  //}

  // Fitting ----------------------------------------------------------------------------------
  if(fVerbose>1)std::cout<<" -I- PndLmdLinFitTask: start Fitting "<<std::endl;

  for(Int_t track=0; track<ntcand; track++)
  {
    PndTrackCand* trcnd = (PndTrackCand*)fTCandArray->At(track);
    const int numPts = trcnd->GetNHits(); //read how many points in this track

    if(fVerbose>2) std::cout << "Track: "<< track<< " Points: "<< numPts <<std::endl;

    /// Obtain first approcsimation
    TVector3 posSeed = trcnd->getPosSeed();
    cout<<"posSeed = "<<endl;
    posSeed.Print();
    TVector3 dirSeed = trcnd->getDirSeed();
    cout<<"dirSeed = "<<endl;
    dirSeed.Print();
    ///------------------------------

    TGraph2DErrors fitme(numPts); //new graph for fitting
    Int_t firstHit=-1, lastHit=-1;
    for(unsigned int ihit=0; ihit<numPts; ihit++){ //fill Graph
      PndTrackCandHit theHit = trcnd->GetSortedHit(ihit); //get hit
      Int_t index = theHit.GetHitId();
      Int_t detId = theHit.GetDetId();
      //if(fVerbose>2) std::cout << "Point: "<< ihit<< " index: "<< index <<std::endl;

      if(ihit==0)
         firstHit=index;
      else if(ihit==numPts-1)
         lastHit=index;
      PndSdsHit* addHit = (PndSdsHit*) fRecoArray->At(index);
      TVector3 addPos = addHit->GetPosition();

      fitme.SetPoint(ihit, addPos.X(), addPos.Y(), addPos.Z());
      fitme.SetPointError(ihit, addHit->GetDx(), addHit->GetDy(), addHit->GetDz());
    }//end of Hits in TCand


    Double_t parFit[4]; //fit-parameter
    Double_t parFitErr[4]; //errors of parameters
    //    Double_t accuracy = line3Dfit(numPts, &fitme, parFit, parFitErr);
    Double_t accuracy = line3Dfit(numPts, &fitme, posSeed, dirSeed, parFit, parFitErr);

    PndLinTrack* trackfit = new PndLinTrack("Lumi", fz0, parFit[0], parFit[1], parFit[2], parFit[3],
					    parFitErr[0], parFitErr[1], parFitErr[2], parFitErr[3],
					    accuracy, firstHit, lastHit, track);

    new((*fTrackArray)[track]) PndLinTrack(*(trackfit)); //save Track

  }// end of TCand's

  // Done--------------------------------------------------------------------------------------

  std::cout<<"Fitting done"<<std::endl;

  return;
}


// define the parameteric line equation 
void PndLmdLinFitTask::line(double t, double *p, double &x, double &y, double &z) { 
   // a parameteric line is define from 6 parameters but 4 are independent
   // x0,y0,z0,z1,y1,z1 which are the coordinates of two points on the line
   // can choose z0 = 0 if line not parallel to x-y plane and z1 = 1; 
   x = p[0] + p[1]*t; 
   y = p[2] + p[3]*t;
   z = t; 
   //  std::cout<<"PndLmdLinFitTask::line z = "<<z<<std::endl;
} 

// calculate distance line-point 
double distance2(double x,double y,double z, double *p) { 
   // distance line point is D= | (xp-x0) cross  ux | 
   // where ux is direction of line and x0 is a point in the line (like t = 0) 
   XYZVector xp(x,y,z); 
   // XYZVector x0(p[0], p[2], 0. ); 
   // XYZVector x1(p[0] + p[1], p[2] + p[3], 1. ); 
   XYZVector x0(p[0], p[2], fz0 ); //Move origin of local coordinates to first lumi plane
   XYZVector x1(p[0] + p[1], p[2] + p[3], fz0+1. ); 
   XYZVector u = (x1-x0).Unit(); 
   double d2 = ((xp-x0).Cross(u)) .Mag2(); 
   return d2; 
}

// function to be minimized 
void SumDistance2(int &, double *, double & sum, double * par, int ) { 
   TGraph2D * gr = dynamic_cast<TGraph2D*>( (TVirtualFitter::GetFitter())->GetObjectFit() );
   assert(gr != 0);
   double * x = gr->GetX();
   double * y = gr->GetY();
   double * z = gr->GetZ();
   int npoints = gr->GetN();
   sum = 0;
   for (int i  = 0; i < npoints; ++i) { 
      double d = distance2(x[i],y[i],z[i],par); 
      sum += d;
   }
   //if (firstIt && fVerbose>1) 
   //   std::cout << "Total sum2 = " << sum << std::endl;
   //firstIt = false;
}

// calculate distance line-point in local coordinates
double distance_perp(double x,double y,double z, double errx,double erry,double errz, double *p) { 
  // cout<<" "<<endl;
  // cout<<" --- --- --- --- --- ---"<<endl;
  Double_t t_min = (p[1]*(x-p[0])+p[3]*(y-p[2])+(z-fz0))/(p[1]*p[1]+p[3]*p[3]+1);
  // cout<<"tmin = "<<t_min<<endl;
  // cout<<"x_trk = "<<p[0]+p[1]*t_min<<" x="<<x<<" errx="<<errx<<endl;
  // cout<<"y_trk = "<<p[2]+p[3]*t_min<<" y="<<y<<" erry="<<erry<<endl;
  // cout<<"Delta_z = "<<fz0+t_min-z
  //     <<" z_trk = "<<fz0+t_min<<" z="<<z<<" errz="<<errz<<endl;
  Double_t fdz = (fz0+t_min-z)/errz;
  Double_t fdx = (p[0]+p[1]*t_min-x)/errx;
  Double_t fdy = (p[2]+p[3]*t_min-y)/erry;
  Double_t fchi2 = fdx*fdx + fdy*fdy + fdz*fdz;
  // cout<<"p0="<<p[0]<<" p1="<<p[1]<<" p2="<<p[2]<<" p3="<<p[3]<<endl;
  // cout<<"fdx = "<<fdx<<" fdy = "<<fdy<<" fdz = "<<fdz<<" fchi2="<<fchi2<<endl;

  // // ///---- TEST ------------------------------
  // TVector3 trk(p[1]*t_min,p[3]*t_min,t_min);
  // TVector3 hit(x-(p[0]+p[1]*t_min),y-(p[2]+p[3]*t_min),z-(fz0+t_min));
  // cout<<"hit.Mag()="<<hit.Mag()<<endl;
  // // TVector3 cross = trk.Cross(hit); // vector product
  // // cout<<"cross.Mag = "<<cross.Mag()<<endl;
  // // if(cross.Mag()>0.1) return 1e6;
  // // TVector3 hitnorm((x-(p[0]+p[1]*t_min))/errx,(y-(p[2]+p[3]*t_min))/erry,(z-(fz0+t_min))/errz);
  // // Double_t fchi2 = hitnorm.Mag2();
  // // cout<<"fchi2="<<fchi2<<endl;
  // // // TVector3 cross = trk.Cross(hit); // vector product
  // // // cout<<"cross.Mag = "<<cross.Mag()<<endl;
  // Double_t scal = trk.Dot(hit); // vector product
  // cout<<"scal = "<<scal<<endl;
  // // ///----------------------------------------
  return fchi2; 
}

// calculate distance line-point in local coordinates
double distance_l(double x,double y,double z, double errx,double erry,double errz, double *p) { 
  // cout<<"(p[1]*p[1]+p[3]*p[3]) = "<<(p[1]*p[1]+p[3]*p[3])<<endl;
  // if((p[1]*p[1]+p[3]*p[3])>1) return 1e6;

  double fdx = TMath::Power((x-(p[0] + p[1]*(z-fz0)))/errx,2);
  double fdy = TMath::Power((y-(p[2] + p[3]*(z-fz0)))/erry,2);
  double fchi2 = fdx + fdy;
  std::cout<<"z = "<<z<<" fchi2 = "<<fchi2<<" fdx = "<<fdx<<" fdy = "<<fdy<<std::endl;
  return fchi2; 
}
// function to be minimized in local coordinates
void LocalFCN(int &, double *, double & sum, double * par, int ) { 
  TGraph2DErrors * gr = dynamic_cast<TGraph2DErrors*>( (TVirtualFitter::GetFitter())->GetObjectFit() );
  assert(gr != 0);
  double * x = gr->GetX();
  double * y = gr->GetY();
  double * z = gr->GetZ();
  double * errx = gr->GetEX();
  double * erry = gr->GetEY();
  double * errz = gr->GetEZ();
  //  cout<<" - - - - - - - "<<endl;
  // std::cout<<"x[0]"<<x[0]<<" errx[0]"<<errx[0]<<std::endl;
  // std::cout<<"y[0]"<<y[0]<<" erry[0]"<<erry[0]<<std::endl;
  // std::cout<<"z[0]"<<z[0]<<" errz[0]"<<errz[0]<<std::endl;
  // std::cout<<"x[1]"<<x[1]<<" errx[1]"<<errx[1]<<std::endl;
  // std::cout<<"y[1]"<<y[1]<<" erry[1]"<<erry[1]<<std::endl;
  // std::cout<<"z[1]"<<z[1]<<" errz[1]"<<errz[1]<<std::endl;
  // std::cout<<"x[2]"<<x[2]<<" errx[2]"<<errx[2]<<std::endl;
  // std::cout<<"y[2]"<<y[2]<<" erry[2]"<<erry[2]<<std::endl;
  // std::cout<<"z[2]"<<z[2]<<" errz[2]"<<errz[2]<<std::endl;
  // std::cout<<"x[3]"<<x[3]<<" errx[3]"<<errx[3]<<std::endl;
  // std::cout<<"y[3]"<<y[3]<<" erry[3]"<<erry[3]<<std::endl;
  // std::cout<<"z[3]"<<z[3]<<" errz[3]"<<errz[3]<<std::endl;

  int npoints = gr->GetN();
  //  cout<<"LocalFCN npoints="<<npoints<<endl;
  sum = 0;
  for (int i  = 0; i < npoints; ++i) { 
    //double d = distance_l(x[i],y[i],z[i],errx[i],erry[i],errz[i],par); 
    double chi2 = distance_perp(x[i],y[i],z[i],errx[i],erry[i],errz[i],par); 
    sum += chi2;
  }
  //if (firstIt && fVerbose>1) 
  //   std::cout << "Total sum2 = " << sum << std::endl;
  //firstIt = false;
}

double PndLmdLinFitTask::line3Dfit(Int_t nd, TGraph2DErrors* gr, Double_t* fitpar, Double_t* fitparerr)
{
   //gStyle->SetOptStat(0);
   //gStyle->SetOptFit();
   //firstIt = true;
  Int_t Npoint = gr->GetN();
  // cout<<"Npoint = "<<Npoint<<endl;
  //The default minimizer is Minuit
  TVirtualFitter::SetDefaultFitter("Minuit");
  TVirtualFitter *min = TVirtualFitter::Fitter(0,4);
  min->SetObjectFit(gr);
  //min->SetFCN( *SumDistance2 );
  min->SetFCN(*LocalFCN);//using local coordinate in FCN
  Double_t arglist[100];
  arglist[0] = 1;
  min->ExecuteCommand("SET PRINT",arglist,1);
   
  double pStart[4] = {25.15,0.04,0.09,8.4e-5};
  double pStartErr[4] = {4.5,0.004,4.5,0.004};
  //  double pStart[4] = {25,0.04,0,0};
  //  double pStartErr[4] = {0.001,0.001,0.001,0.001};
  // min->SetParameter(0,"x0",pStart[0],pStartErr[0],pStart[0]-15*pStartErr[0],pStart[0]+15*pStartErr[0]);
  // min->SetParameter(1,"Ax",pStart[1],pStartErr[1],pStart[1]-15*pStartErr[1],pStart[1]+15*pStartErr[1]);
  // min->SetParameter(2,"y0",pStart[2],pStartErr[2],pStart[2]-15*pStartErr[2],pStart[2]+15*pStartErr[2]);
  // min->SetParameter(3,"Ay",pStart[3],pStartErr[3],pStart[3]-15*pStartErr[3],pStart[3]+15*pStartErr[3]);
  min->SetParameter(0,"x0",pStart[0],pStartErr[0],0,0);
  min->SetParameter(1,"Ax",pStart[1],pStartErr[1],0,0);
  min->SetParameter(2,"y0",pStart[2],pStartErr[2],0,0);
  min->SetParameter(3,"Ay",pStart[3],pStartErr[3],0,0);
  //  min->SetPrecision(1e-10);
  //  min->SetPrecision(1e-5);
  
  


  // // //minimize step 1
  // // arglist[0] = 100; //number of functiona calls
  // // arglist[1] = 0.001; //tolerance
  // // min->ExecuteCommand("HESSE", arglist ,2);

  // //minimize step 2
  arglist[0] = 1000; //number of functiona calls
  arglist[1] = 0.001; //tolerance
  min->ExecuteCommand("MIGRAD", arglist ,2);

   
  //if (minos) min->ExecuteCommand("MINOS",arglist,0);
   int nvpar,nparx; 
   double amin,edm, errdef;
   min->GetStats(amin,edm,errdef,nvpar,nparx);
   if(fVerbose>1)
     min->PrintResults(1,amin);
   // gr->Draw("p0");

   // get fit parameters and errors
   for (int i = 0; i <4; ++i){
      fitpar[i] = min->GetParameter(i); 
      fitparerr[i] = min->GetParError(i);
   }
  
   // return amin; 
   Double_t chi2 = amin/(2.*Npoint-4);
   cout<<"Chi^2 = "<<chi2<<endl;
   return chi2; 
}

double PndLmdLinFitTask::line3Dfit(Int_t nd, TGraph2DErrors* gr, TVector3 posSeed, TVector3 dirSeed, Double_t* fitpar, Double_t* fitparerr)
{
  cout<<"PndLmdLinFitTask::line3Dfit with SEED is used"<<endl;
   //gStyle->SetOptStat(0);
   //gStyle->SetOptFit();
   //firstIt = true;
  Int_t Npoint = gr->GetN();
  // cout<<"Npoint = "<<Npoint<<endl;
  //The default minimizer is Minuit
  TVirtualFitter::SetDefaultFitter("Minuit");
  TVirtualFitter *min = TVirtualFitter::Fitter(0,4);
  min->SetObjectFit(gr);
  //min->SetFCN( *SumDistance2 );
  min->SetFCN(*LocalFCN);//using local coordinate in FCN
  Double_t arglist[100];
  arglist[0] = 1;
  min->ExecuteCommand("SET PRINT",arglist,1);
 
  double pStart[4] = {posSeed.X(),dirSeed.X(),posSeed.Y(),dirSeed.Y()};
  //  double pStartErr[4] = {4.5,0.004,4.5,0.004};
  //  double pStart[4] = {25,0.04,0,0};
  double pStartErr[4] = {0.01,0.0001,0.01,0.0001};
  // min->SetParameter(0,"x0",pStart[0],pStartErr[0],pStart[0]-15*pStartErr[0],pStart[0]+15*pStartErr[0]);
  // min->SetParameter(1,"Ax",pStart[1],pStartErr[1],pStart[1]-15*pStartErr[1],pStart[1]+15*pStartErr[1]);
  // min->SetParameter(2,"y0",pStart[2],pStartErr[2],pStart[2]-15*pStartErr[2],pStart[2]+15*pStartErr[2]);
  // min->SetParameter(3,"Ay",pStart[3],pStartErr[3],pStart[3]-15*pStartErr[3],pStart[3]+15*pStartErr[3]);
  min->SetParameter(0,"x0",pStart[0],pStartErr[0],0,0);
  min->SetParameter(1,"Ax",pStart[1],pStartErr[1],0,0);
  min->SetParameter(2,"y0",pStart[2],pStartErr[2],0,0);
  min->SetParameter(3,"Ay",pStart[3],pStartErr[3],0,0);
  //  min->SetPrecision(1e-10);
  //  min->SetPrecision(1e-5);
  

  // Now ready for minimization step
  arglist[0] = 1500;
  arglist[1] = 1.;
  min->ExecuteCommand("MINI", arglist,2);
  arglist[0] = 50;
  //arglist[0] = 1;
  min->ExecuteCommand("HESSE", arglist,2);

  // //minimize step 1
  // arglist[0] = 100; //number of functiona calls
  // //arglist[0] = 1; //number of functiona calls
  // arglist[1] = 1e-5; //tolerance
  // min->ExecuteCommand("HESSE", arglist ,2);

  // //minimize step 2
  // arglist[0] = 1000; //number of functiona calls
  // // arglist[0] = 10; //number of functiona calls
  // //  arglist[0] = 1; //number of functiona calls
  // arglist[1] = 1e-5; //tolerance
  // min->ExecuteCommand("MIGRAD", arglist ,2);

   
  //if (minos) min->ExecuteCommand("MINOS",arglist,0);
   int nvpar,nparx; 
   double amin,edm, errdef;
   min->GetStats(amin,edm,errdef,nvpar,nparx);
   if(fVerbose>1)
     min->PrintResults(1,amin);
   // gr->Draw("p0");

   // get fit parameters and errors
   for (int i = 0; i <4; ++i){
      fitpar[i] = min->GetParameter(i); 
      fitparerr[i] = min->GetParError(i);
   }
  
   // return amin; 
   Double_t chi2 = amin/(2.*Npoint-4);
   cout<<"Chi^2 = "<<chi2<<endl;
   return chi2; 
}


ClassImp(PndLmdLinFitTask);
