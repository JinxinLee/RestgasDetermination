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

using namespace ROOT::Math;

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
  ioman->Register("DMLTrack", "DML", fTrackArray, kTRUE);

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
    Double_t accuracy = line3Dfit(numPts, &fitme, parFit);

    PndLinTrack* trackfit = new PndLinTrack("Lumi", parFit[0], parFit[1], parFit[2], parFit[3],
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
} 

// calculate distance line-point 
double distance2(double x,double y,double z, double *p) { 
   // distance line point is D= | (xp-x0) cross  ux | 
   // where ux is direction of line and x0 is a point in the line (like t = 0) 
   XYZVector xp(x,y,z); 
   XYZVector x0(p[0], p[2], 0. ); 
   XYZVector x1(p[0] + p[1], p[2] + p[3], 1. ); 
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

double PndLmdLinFitTask::line3Dfit(Int_t nd, TGraph2DErrors* gr, Double_t* fitpar)
{
   //gStyle->SetOptStat(0);
   //gStyle->SetOptFit();
   //firstIt = true;
   
   TVirtualFitter *min = TVirtualFitter::Fitter(0,4);
   min->SetObjectFit(gr);
   min->SetFCN( *SumDistance2 );
  
   Double_t arglist[10];
   arglist[0] = 1;
   min->ExecuteCommand("SET PRINT",arglist,1);
  
   double pStart[4] = {0.001,0.001,0.001,0.001};
   min->SetParameter(0,"x0",pStart[0],0.00001,0,0);
   min->SetParameter(1,"Ax",pStart[1],0.00001,0,0);
   min->SetParameter(2,"y0",pStart[2],0.00001,0,0);
   min->SetParameter(3,"Ay",pStart[3],0.00001,0,0);
   min->SetPrecision(1e-8);
    
   arglist[0] = 1000; // number of function calls 
   arglist[1] = 1e-8; // tolerance 
   min->ExecuteCommand("MIGRAD",arglist,2);

  //if (minos) min->ExecuteCommand("MINOS",arglist,0);
   int nvpar,nparx; 
   double amin,edm, errdef;
   min->GetStats(amin,edm,errdef,nvpar,nparx);
   if(fVerbose>1)
     min->PrintResults(1,amin);
  // gr->Draw("p0");

   // get fit parameters
   for (int i = 0; i <4; ++i) 
      fitpar[i] = min->GetParameter(i); 

  return amin; 
}


ClassImp(PndLmdLinFitTask);
