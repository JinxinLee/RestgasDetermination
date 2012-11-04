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
#include <TF2.h>
#include <TH1.h>
#include <TVirtualFitter.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <TMatrixTSym.h>
#include <TMatrixDSym.h>
#include <TCanvas.h>
#include <TGraph2D.h>
#include <TGraph2DErrors.h>
#include <TLine.h>
#include <TMultiGraph.h>
#include <TPolyLine3D.h>
using namespace ROOT::Math;
using namespace std;

PndLmdLinFitTask::PndLmdLinFitTask()
  : FairTask("3D-Straight-Line-Fit")
{
  fTCandBranchName = "LMDTrackCand";
  fRecoBranchName = "LMDHitsStrip";
  //  fRecoBranchName = "LmdHits";
  fTruePointBranch = "LMDPoint";  //True Points only for drawing!
}
PndLmdLinFitTask::PndLmdLinFitTask(TString tTCandBranchName, TString tRecoBranchName)
  : FairTask("3D-Straight-Line-Fit")
{
  fTCandBranchName = tTCandBranchName;
  fRecoBranchName = tRecoBranchName;
  fTruePointBranch = "LMDPoint";  //True Points only for drawing!
}

PndLmdLinFitTask::~PndLmdLinFitTask()
{
  cout<<"PndLmdLinFitTask::~PndLmdLinFitTask()"<<endl;
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
  //  std::cout<<"PndLmdLinFitTask::Exec"<<std::endl;
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
  //  if(fVerbose>1) 
  std::cout<<" -I- PndLmdLinFitTask: start Fitting "<<std::endl;
  int rec_tkr=0;
  for(Int_t track=0; track<ntcand; track++)
  {
    PndTrackCand* trcnd = (PndTrackCand*)fTCandArray->At(track);
    const int numPts = trcnd->GetNHits(); //read how many points in this track
    /// Obtain first approximation ----------
    TVector3 posSeed = trcnd->getPosSeed();
    TVector3 dirSeed = trcnd->getDirSeed();
    // if(dirSeed.Theta()<3e-2 || dirSeed.Theta()>5e-2 ||  fabs(dirSeed.Phi())>0.3){
    //   if(fVerbose>2) cout<<"Trk-cand doesn't pass throw limit (dirSeed.Theta() = "<<dirSeed.Theta()<<" dirSeed.Phi() = "<<dirSeed.Phi()<<")"<<endl;
    //   continue;
    // }
    //  if(fVerbose>2) 
    std::cout << "Track: "<< track<< " Points: "<< numPts <<std::endl;
    ///--------------------------------------

    TGraph2DErrors fitme(numPts); //new graph for fitting
    Int_t firstHit=-1, lastHit=-1;
    for(unsigned int ihit=0; ihit<numPts; ihit++){ //fill Graph
      PndTrackCandHit theHit = trcnd->GetSortedHit(ihit); //get hit
      Int_t index = theHit.GetHitId();
      Int_t detId = theHit.GetDetId();
      // if(fVerbose>2) std::cout << "Point: "<< ihit<< " index: "<< index <<std::endl;

      if(ihit==0)
         firstHit=index;
      else if(ihit==numPts-1)
         lastHit=index;
      PndSdsHit* addHit = (PndSdsHit*) fRecoArray->At(index);
      TVector3 addPos = addHit->GetPosition();
      //      addPos.Print();
      fitme.SetPoint(ihit, addPos.X(), addPos.Y(), addPos.Z());
      fitme.SetPointError(ihit, addHit->GetDx(), addHit->GetDy(), addHit->GetDz());
    }//end of Hits in TCand


    Double_t parFit[6]; //fit-parameter
    TMatrixDSym *COVmatrix = new TMatrixDSym(6);
    Double_t accuracy = line3Dfit(numPts, &fitme, posSeed, dirSeed, parFit, COVmatrix);
    
    //  if(accuracy>0 && accuracy<1e3){ 
    PndLinTrack* trackfit = new PndLinTrack("Lumi", parFit[0], parFit[1], parFit[2], parFit[3], parFit[4], parFit[5], 
					    accuracy, firstHit, lastHit, track);
    trackfit->SetCovarianceMatrix(*COVmatrix);
    new((*fTrackArray)[rec_tkr]) PndLinTrack(*(trackfit)); //save Track
    delete trackfit;//TEST
    rec_tkr++;
    //}
  }// end of TCand's

  // Done--------------------------------------------------------------------------------------
  if(fVerbose>2) std::cout<<"Fitting done"<<std::endl;
  return;
}


// // define the parameteric line equation 
// void PndLmdLinFitTask::line(double t, double *p, double &x, double &y, double &z) { 
//    // a parameteric line is define from 6 parameters but 4 are independent
//    // x0,y0,z0,z1,y1,z1 which are the coordinates of two points on the line
//    // can choose z0 = 0 if line not parallel to x-y plane and z1 = 1; 
//    x = p[0] + p[1]*t; 
//    y = p[2] + p[3]*t;
//    z = t; 
//    //  std::cout<<"PndLmdLinFitTask::line z = "<<z<<std::endl;
// } 

// calculate distance line-point 
double PndLmdLinFitTask::distance2(double x,double y,double z, double *p) { 
   // distance line point is D= | (xp-x0) cross  ux | 
   // where ux is direction of line and x0 is a point in the line (like t = 0) 
   XYZVector xp(x,y,z); 
   // XYZVector x0(p[0], p[2], 0. ); 
   // XYZVector x1(p[0] + p[1], p[2] + p[3], 1. ); 
   //   XYZVector x0(p[0], p[2], fz0 ); //Move origin of local coordinates to first lumi plane
   //   XYZVector x1(p[0] + p[1], p[2] + p[3], fz0+1. ); 
   XYZVector x0(p[0], p[2], p[4] ); //! Move origin of local coordinates to first lumi plane
   XYZVector x1(p[0] + p[1], p[2] + p[3], p[5] ); //!
   XYZVector u = (x1-x0).Unit(); 
   double d2 = ((xp-x0).Cross(u)) .Mag2(); 
   return d2; 
}

// function to be minimized 
void PndLmdLinFitTask::SumDistance2(int &, double *, double & sum, double * par, int ) { 
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
}

// calculate distance line-point in local coordinates
double PndLmdLinFitTask::distance_perp(double x,double y,double z, double errx,double erry,double errz, double *p) { 
  Double_t t_min = (z-p[4]);
  Double_t fdx = pow((p[0]+p[1]*t_min-x)/errx,2);
  Double_t fdy = pow((p[2]+p[3]*t_min-y)/erry,2);
  Double_t fchi2 = fdx + fdy;
  return fchi2; 
}

// calculate distance line-point in local coordinates
double PndLmdLinFitTask::distance_l(double x,double y,double z, double errx,double erry,double errz, double *p) { 
  Double_t t_min = p[1]*(x-p[0])+p[3]*(y-p[2])+p[5]*(z-p[4]);
  double fdx = TMath::Power((x-(p[0] + p[1]*t_min))/errx,2);
  double fdy = TMath::Power((y-(p[2] + p[3]*t_min))/erry,2);
  double fdz = TMath::Power((z-(p[4] + p[5]*t_min))/errz,2);
  double fchi2 = fdx + fdy +fdz;
  return fchi2; 
}

// function to be minimized in local coordinates
void PndLmdLinFitTask::LocalFCN(int &, double *, double & sum, double * par, int ) { 
  TGraph2DErrors * gr = dynamic_cast<TGraph2DErrors*>( (TVirtualFitter::GetFitter())->GetObjectFit() );
  assert(gr != 0);
  double * x = gr->GetX();
  double * y = gr->GetY();
  double * z = gr->GetZ();
  double * errx = gr->GetEX();
  double * erry = gr->GetEY();
  double * errz = gr->GetEZ();
  int npoints = gr->GetN();
  sum = 0;
  for (int i  = 0; i < npoints; ++i) { 
    //   double chi2 = distance_l(x[i],y[i],z[i],errx[i],erry[i],errz[i],par); 
    double chi2 = distance_perp(x[i],y[i],z[i],errx[i],erry[i],errz[i],par); 
    sum += chi2;
  }
  // par[5] = sqrt(1-par[1]*par[1]-par[3]*par[3]);
}

double PndLmdLinFitTask::line3Dfit(Int_t nd, TGraph2DErrors* gr, Double_t* fitpar, Double_t* fitparerr)
{
  Int_t Npoint = gr->GetN();

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
  min->SetParameter(0,"x0",pStart[0],pStartErr[0],0,0);
  min->SetParameter(1,"Ax",pStart[1],pStartErr[1],0,0);
  min->SetParameter(2,"y0",pStart[2],pStartErr[2],0,0);
  min->SetParameter(3,"Ay",pStart[3],pStartErr[3],0,0);
  // //minimize step 1
  arglist[0] = 1000; //number of functiona calls
  arglist[1] = 0.001; //tolerance
  min->ExecuteCommand("MIGRAD", arglist ,2);

   
  //if (minos) min->ExecuteCommand("MINOS",arglist,0);
   int nvpar,nparx; 
   double amin,edm, errdef;
   min->GetStats(amin,edm,errdef,nvpar,nparx);
   if(fVerbose>1)
     min->PrintResults(1,amin);

   // get fit parameters and errors
   for (int i = 0; i <4; ++i){
      fitpar[i] = min->GetParameter(i); 
      fitparerr[i] = min->GetParError(i);
   }
   // return amin; 
   Double_t chi2 = amin/(2.*Npoint-4);
   // cout<<"Chi^2 = "<<chi2<<endl;
   return chi2; 
}

double PndLmdLinFitTask::line3Dfit(Int_t nd, TGraph2DErrors* gr, TVector3 posSeed, TVector3 dirSeed, Double_t* fitpar, TMatrixDSym *covmatrix)
{
  if(fVerbose>2) cout<<"PndLmdLinFitTask::line3Dfit with SEED is used"<<endl;
  Int_t Npoint = gr->GetN();
  Double_t ErrX1 = gr->GetErrorX(0);
  Double_t ErrY1 = gr->GetErrorY(0);
  Double_t ErrZ1 = gr->GetErrorZ(0);
  Double_t ErrX2 = gr->GetErrorX(1);
  Double_t ErrY2 = gr->GetErrorY(1);
  Double_t ErrZ2 = gr->GetErrorY(1);
  
  Double_t errRx = TMath::Hypot(ErrX1,ErrX2);
  Double_t errRy = TMath::Hypot(ErrY1,ErrY2);
  Double_t errRz = TMath::Hypot(ErrZ1,ErrZ2);
  
  //The default minimizer is Minuit
  TVirtualFitter::SetDefaultFitter("Minuit");
  TVirtualFitter *min = TVirtualFitter::Fitter(0,5);
  min->SetObjectFit(gr);
  // min->SetFCN( *SumDistance2 );
  min->SetFCN(*LocalFCN);//using local coordinate in FCN
  Double_t arglist[100];
  arglist[0] = 1;
  min->ExecuteCommand("SET PRINT",arglist,1);

  if(fVerbose>2){
    cout<<"posSeed:"<<endl;
    posSeed.Print();
    cout<<"dirSeed:"<<endl;
    dirSeed.Print();
  }
  //  double l = 1/dirSeed.Z();
  double l = 1/dirSeed.Mag();
  //  double pStart[6] = {posSeed.X(),l*dirSeed.X(),posSeed.Y(),l*dirSeed.Y(),posSeed.Z(),1.};

  double pStart[6] = {posSeed.X(),l*dirSeed.X(),posSeed.Y(),l*dirSeed.Y(),posSeed.Z(),l*dirSeed.Z()};
  // //  //go out of middle of plane!
  // double pStart[6] = {posSeed.X(),l*dirSeed.X(),posSeed.Y(),l*dirSeed.Y(),posSeed.Z()-0.0085,l*dirSeed.Z()};//85 mkm!!!
  
  double pStartErr[6] = {ErrX1,errRx,ErrY1,errRy,ErrZ1,errRz};
  if(fVerbose>2){
    for(int i=0;i<6;i++)
      cout<<"pStartErr["<<i<<"]="<<pStartErr[i]<<endl;
  }
  // //stupid and must be removed!
  // if(ErrX1==0 || ErrY1==0)
  //   for(int k=0;k<6;k++)  pStartErr[k]=0;
  
  min->SetParameter(0,"x0",pStart[0],pStartErr[0],0,0);
  min->SetParameter(1,"Ax",pStart[1],pStartErr[1],0,0);
  min->SetParameter(2,"y0",pStart[2],pStartErr[2],0,0);
  min->SetParameter(3,"Ay",pStart[3],pStartErr[3],0,0);
  min->SetParameter(4,"z0",pStart[4],0,0,0);
  min->SetParameter(5,"Az",pStart[5],0,0,0);
  //  min->SetParameter(4,"z0",pStart[4],pStartErr[4],0,0);
  // min->SetParameter(5,"Az",pStart[5],pStartErr[5],0,0);
  
  // Now ready for minimization step
  arglist[0] = 1500;
  arglist[1] = 1.;
  min->ExecuteCommand("MIGRAD", arglist,2);
  
  ///Get results ---------------------------------------------
   int nvpar,nparx; 
   double amin,edm, errdef;
   min->GetStats(amin,edm,errdef,nvpar,nparx);
   if(fVerbose>1)
     min->PrintResults(1,amin);

   Double_t fitparerr[6];
   // get fit parameters
   for (int i = 0; i <6; ++i){
      fitpar[i] = min->GetParameter(i); 
      fitparerr[i] = min->GetParError(i);
   }
  
   // cout<<" before fitpar[5] = "<<fitpar[5]<<endl;
   fitpar[5]=sqrt(1-fitpar[1]*fitpar[1]-fitpar[3]*fitpar[3]);
   // cout<<" after fitpar[5] = "<<fitpar[5]<<endl;
   // cout<<"SEED DIR=("<<dirSeed.X()<<", "<<dirSeed.Y()<<", "<<dirSeed.Z()<<")"<<endl;
   // cout<<"RecTRK DIR=("<<fitpar[1]<<", "<<fitpar[3]<<", "<<fitpar[5]<<")"<<endl;
   for(size_t i=0;i<4;i++){
     for(size_t j=0;j<4;j++){
       (*covmatrix)(i,j)= min->GetCovarianceMatrixElement(i,j);
     }
   }
   //   (*covmatrix)(4,4) =  0.00013*gr->GetErrorZ(0)*gr->GetErrorZ(0);
   (*covmatrix)(4,4) =  0.0001*gr->GetErrorZ(0)*gr->GetErrorZ(0);
   // (*covmatrix)(4,4) = 5e-09;
   //  (*covmatrix)(4,4) += (*covmatrix)(0,0)*pow(tan(2.326*TMath::Pi()/180.),2);
   double dp5_dp1 = fitpar[1]/fitpar[5];
   double dp5_dp3 = fitpar[3]/fitpar[5];
   double errdz2 = pow(dp5_dp1,2)*(*covmatrix)(1,1) + pow(dp5_dp3,2)*(*covmatrix)(3,3) + 
     2*fabs(dp5_dp1*dp5_dp3*(*covmatrix)(1,3));
   // cout<<"pow(dp5_dp1,2)*(*covmatrix)(1,1) = "<<pow(dp5_dp1,2)*(*covmatrix)(1,1)<<endl;
   // cout<<"pow(dp5_dp3,2)*(*covmatrix)(3,3) = "<<pow(dp5_dp3,2)*(*covmatrix)(3,3)<<endl;
   // cout<<"2*dp5_dp1*dp5_dp3*(*covmatrix)(1,3) = "<<2*dp5_dp1*dp5_dp3*(*covmatrix)(1,3)<<endl;
   (*covmatrix)(5,5) = errdz2;
   //(*covmatrix)(5,5) = (fitpar[1]*fitpar[1]*(*covmatrix)(1,1)+fitpar[3]*fitpar[3]*(*covmatrix)(3,3))/pow(fitpar[5],6);
   // for(size_t i=0;i<6;i++){
   //   for(size_t j=0;j<6;j++){
   //     cout<<(*covmatrix)(i,j)<<" ";
   //   }
   // 	 cout<<endl;
   // }

   //  Double_t chi2 = amin/(3.*Npoint-6);
   // Double_t chi2 = amin/(3.*Npoint-3);
   //  Double_t chi2 = amin/(3.*Npoint-5);
   Double_t chi2 = amin/(2.*Npoint-4);
   //Double_t chi2 = amin/(3.*Npoint-4);
   //  Double_t chi2 = amin;
   cout<<"After fit: Chi^2 = "<<chi2<<endl;
   ///-------------------------------------------------------------
  
   return chi2; 
}


ClassImp(PndLmdLinFitTask);
