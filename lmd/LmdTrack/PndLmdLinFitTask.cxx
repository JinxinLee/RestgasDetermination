//-----------------------------------------------------------
//
// Description:
//      3D Straight Line fitter
//
// Author List:
//      Mathias Michel
//      Anastasia Karavdina
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
#include "PndSdsMergedHit.h"
#include "TrackData/PndTrackCandHit.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "FairBaseParSet.h"
#include "FairRuntimeDb.h"

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
#include <FairRun.h>
using namespace ROOT::Math;
using namespace std;

PndLmdLinFitTask* PndLmdLinFitTask::fInstance= 0;
//_____________________________________________________________________________
PndLmdLinFitTask* PndLmdLinFitTask::Instance()
{
  return fInstance;
}

PndLmdLinFitTask::PndLmdLinFitTask()
  : FairTask("3D-Straight-Line-Fit")
{
  fTCandBranchName = "LMDTrackCand";
  // fRecoBranchName = "LMDHitsStrip";
  fRecoBranchName = "LMDHitsPixel";
  //  fRecoBranchName = "LmdHits";
  fTruePointBranch = "LMDPoint";  //True Points only for drawing!
  fPbeam = 0;
  fPDGCode = -2212; //barp
  fCharge = -1;//barp
  fsigmaMS = 0;
  PndGeoHandling::Instance();
  fInstance=this;
  //  ttal = new TNtuple("ttal","kink angles","alx0:alx1:alx2:alx3:aly0:aly1:aly2:aly3:npoints") ;
  ttal = new TNtuple("ttal","kink angles","alx0:alx1:alx2:aly0:aly1:aly2:erralx1:erralx2:erraly1:erraly2:npoints") ;
 TVirtualFitter::SetDefaultFitter("Minuit");
  fmin = TVirtualFitter::Fitter(0,15);
  for(int ih=0;ih<4;ih++)
    hitMergedfl[ih] = false;
}
PndLmdLinFitTask::PndLmdLinFitTask(TString tTCandBranchName, TString tRecoBranchName)
  : FairTask("3D-Straight-Line-Fit")
{
  fTCandBranchName = tTCandBranchName;
  fRecoBranchName = tRecoBranchName;
  fTruePointBranch = "LMDPoint";  //True Points only for drawing!
  fPbeam = 0;
  fPDGCode = -2212; //barp
  fCharge = -1;//barp
  fsigmaMS = 0;
  PndGeoHandling::Instance();
  fInstance=this;
  //  ttal = new TNtuple("ttal","kink angles","alx0:alx1:alx2:alx3:aly0:aly1:aly2:aly3:npoints") ;
  ttal = new TNtuple("ttal","kink angles","alx0:alx1:alx2:aly0:aly1:aly2:erralx1:erralx2:erraly1:erraly2:npoints") ;
  //  ttal = new TNtuple("ttal","kink angles","alx0:alx1:alx2:alx3:erralx0:erralx1:erralx2:aly0:aly1:aly2:aly3:erraly0:erraly1:erraly2") ;
  TVirtualFitter::SetDefaultFitter("Minuit");
  fmin = TVirtualFitter::Fitter(0,15);
  for(int ih=0;ih<4;ih++)
    hitMergedfl[ih] = false;
}

PndLmdLinFitTask::~PndLmdLinFitTask()
{
  delete  fmin;
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

  // fTrackArray = new TClonesArray("PndLinTrack");
  // ioman->Register("LMDTrack", "PndLmd", fTrackArray, kTRUE);

  fTrackArray = new TClonesArray("PndTrack");
  ioman->Register("LMDPndTrack", "PndLmd", fTrackArray, kTRUE);


  //read beam momentum from base
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairBaseParSet* par=(FairBaseParSet*)
    (rtdb->findContainer("FairBaseParSet"));
  fPbeam = par->GetBeamMom();
  fPDGCode = -2212; //barp
  fCharge = -1;//barp

  fGeoH = PndGeoHandling::Instance();
  //  double totRadLen = 1.5*0.00306;//rad.length of 1 plane
  // double totRadLen = 0.00306;//rad.length of 1 plane
  // totRadLen +=0.000175;// +flex-cable
  double totRadLen = 0.0044;
  fsigmaMS = ScatteredAngle(totRadLen);
  // totRadLen +=0.5*0.00306;// + more (half-plane for merged hits)
  // fsigmaMSmerged = ScatteredAngle(totRadLen);
  // totRadLen +=0.5*0.00306; //2pl
  // totRadLen -=0.000175;// -flex-cable
  // totRadLen -=0.00053;// -HV-MAPS
  // //  totRadLen +=0.00165;// +CDV diamond
  // fsigmaMS = ScatteredAngle(totRadLen);
  // //  fsigmaMScone = ScatteredAngle(0.00027); //rad.length of cone support
  // double lenConePl = 0.00027+0.5*0.00306; 
  // //  double lenConePl = 0.00027; 
  // fsigmaMScone = ScatteredAngle(lenConePl); //rad.length of cone support
  // //  fsigmaMScone = ScatteredAngle(totRadLen);
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
  if(fVerbose>1)std::cout<<" -I- PndLmdLinFitTask: contains "<<ntcand<<" RhoCandidates"<<std::endl;
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
  // std::cout<<" -I- PndLmdLinFitTask: start Fitting "<<std::endl;
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
    //  std::cout << "Track: "<< track<< " Points: "<< numPts <<std::endl;
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
      //      PndSdsHit* addHit = (PndSdsHit*) fRecoArray->At(index);
      PndSdsMergedHit* addHit = (PndSdsMergedHit*) fRecoArray->At(index);
      hitMergedfl[ihit] = addHit->GetIsMerged();
      TVector3 addPos = addHit->GetPosition();
      //      addPos.Print();
      fitme.SetPoint(ihit, addPos.X(), addPos.Y(), addPos.Z());
      fitme.SetPointError(ihit, addHit->GetDx(), addHit->GetDy(), addHit->GetDz());
    }//end of Hits in TCand


    // Double_t parFit[6]; //fit-parameter
    //    TMatrixDSym *COVmatrix = new TMatrixDSym(6);
    //    TMatrixDSym *COVmatrix = new TMatrixDSym(14);
    //    TMatrixDSym *COVmatrix = new TMatrixDSym(14);
    //Double_t accuracy = line3Dfit(numPts, &fitme, posSeed, dirSeed, parFit, COVmatrix); //MS taking into account by hits errors
    Double_t parFit[15]; //fit-parameter
    TMatrixDSym *COVmatrix = new TMatrixDSym(6);
    Double_t accuracy = line3DfitMS(numPts, &fitme, posSeed, dirSeed, parFit, COVmatrix);//with kink angles
    
    // //save as LinTrk
    // //  if(accuracy>0 && accuracy<1e3){ 
    // PndLinTrack* trackfit = new PndLinTrack("Lumi", parFit[0], parFit[1], parFit[2], parFit[3], parFit[4], parFit[5], 
    // 					    accuracy, firstHit, lastHit, track);
    // trackfit->SetCovarianceMatrix(*COVmatrix);
    // new((*fTrackArray)[rec_tkr]) PndLinTrack(*(trackfit)); //save Track

    //save as PndTrack
    TVector3 FitPoint(parFit[0], parFit[2], parFit[4]);
    TVector3 FitDir(parFit[1], parFit[3], parFit[5]);
    // FitDir *= 1./FitDir.Mag();
    TVector3 FitMom =  fPbeam*FitDir;
    Double_t COVmatrixPosMom[6][6];
    // int iconver[6]={0, 2, 4, 1, 3, 5};
    int iconver[6]={3,0,4,1,5,2};
    for(int ij=0;ij<6;ij++){
      for(int ji=0;ji<6;ji++){
	if(ij==1 || ij==3 || ij==5) (*COVmatrix)(ij,ji) *=fPbeam;
	if(ji==1 || ji==3 || ji==5) (*COVmatrix)(ij,ji) *=fPbeam;
	int km =  iconver[ij];
	int mk = iconver[ji];
	COVmatrixPosMom[km][mk] = (*COVmatrix)(ij,ji);

      }
    }

    //Read info about 1st plane(sensor)
    PndTrackCandHit theHit = trcnd->GetSortedHit(0); //get 1st hit
    Int_t hitID = theHit.GetHitId();
    int sysID = theHit.GetDetId();
    PndSdsHit* myHit = (PndSdsHit*)(fRecoArray->At(hitID));
    Int_t id =  myHit->GetSensorID();
    // cout<<"myHit id: "<<id<<" sysID: "<<sysID<<endl;
    // fGeoH->Print();
    TString path = fGeoH->GetPath(id);
    TVector3 oo, uu, vv;
     fGeoH->GetOUVShortId(id, oo,uu,vv);
     // TVector3 o = oo;
     TVector3 o = FitPoint;
     TVector3 dj = uu;
     TVector3 dk = vv;

     // TVector3 o = (0,0,0);
     // TVector3 dj = (0,1,0);
     // TVector3 dk = (1,0,0);
     // cout<<"in PndTracks goes pos:"<<endl;
     // FitPoint.Print();
     // cout<<"in PndTracks goes dir:"<<endl;
     // FitDir.Print();

     FairTrackParP *trkfitp = new  FairTrackParP(FitPoint,FitMom,COVmatrixPosMom,fCharge,o,dj,dk);
     int flagpndtrk=0;
     if(accuracy>1e3) flagpndtrk=-1;//quality of track: bad if chi2 is too large
     int ndf = 2*(trcnd->GetNHits())-4;//number d.o.f
     int pid = fPDGCode;
     
     PndTrack *trackfit = new PndTrack(*trkfitp,*trkfitp, *trcnd,flagpndtrk,accuracy,ndf,pid,track,sysID);//TODO: FairTrackParP at 1st and last point???
     new((*fTrackArray)[rec_tkr]) PndTrack(*(trackfit)); //save Track

    // // TVector3 dirTEST = trackfit->GetDirectionVec();
    // // cout<<"dirTEST:"<<endl;
    // // dirTEST.Print();
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
  if((p[1]*p[1]+p[3]*p[3])<1) p[5]=sqrt(1-(p[1]*p[1]+p[3]*p[3]));
  //  else p[5]=0.99;
  else return 1e6;
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
    double chi2 = distance_l(x[i],y[i],z[i],errx[i],erry[i],errz[i],par); 
    //    double chi2 = distance_perp(x[i],y[i],z[i],errx[i],erry[i],errz[i],par); 
    sum += chi2;
  }
  // par[5] = sqrt(1-par[1]*par[1]-par[3]*par[3]);
}

// calculate distance line-point in local coordinates
double PndLmdLinFitTask::distance_MS(double x,double y,double z, double errx,double erry,double errz, double *p, double *zpr) { 
  if((p[1]*p[1]+p[3]*p[3])<1) p[5]=sqrt(1-(p[1]*p[1]+p[3]*p[3]));
  //  else p[5]=0.99;
  else return 1e6;

  //  cout<<"erral = "<<erral<<endl;
  // for(int crz=0;crz<4;crz++)
  //   cout<<"zcr["<<crz<<"]="<<zpr[crz]<<endl;
  double THfunc[4] = {1,1,1,1};
 
  //  Double_t t_min = (z-p[4]);
  Double_t t_min = p[1]*(x-p[0])+p[3]*(y-p[2])+p[5]*(z-p[4]);
  for(int iz=0;iz<4;iz++){
    if(((zpr[0]+t_min)-zpr[iz])<0) THfunc[iz]=0;
  }
  //  Double_t t_min = p[1]*(x-p[0])+p[3]*(y-p[2])+(z-p[4]);
  // double funcX = p[0]+p[1]*t_min+p[6]*(z-zpr[0])*THfunc[0]+p[7]*(z-zpr[1])*THfunc[1]+p[8]*(z-zpr[2])*THfunc[2]+p[9]*(z-zpr[3])*THfunc[3];
  // double funcY = p[2]+p[3]*t_min+p[10]*(z-zpr[0])*THfunc[0]+p[11]*(z-zpr[1])*THfunc[1]+p[12]*(z-zpr[2])*THfunc[2]+p[13]*(z-zpr[3])*THfunc[3];
  double funcX = p[0]+p[1]*t_min
    +p[6]*((zpr[0]+t_min)-zpr[0])*THfunc[0]
    +p[7]*((zpr[0]+t_min)-zpr[1])*THfunc[1]
    +p[8]*((zpr[0]+t_min)-zpr[2])*THfunc[2]
    +p[9]*((zpr[0]+t_min)-zpr[3])*THfunc[3];
  double funcY = p[2]+p[3]*t_min
    +p[10]*((zpr[0]+t_min)-zpr[0])*THfunc[0]
    +p[11]*((zpr[0]+t_min)-zpr[1])*THfunc[1]
    +p[12]*((zpr[0]+t_min)-zpr[2])*THfunc[2]
    +p[13]*((zpr[0]+t_min)-zpr[3])*THfunc[3];
  double fdx = TMath::Power((x-funcX)/errx,2);
  double fdy = TMath::Power((y-funcY)/erry,2);
  // double fdz = TMath::Power((z-(p[4] + p[5]*t_min))/errz,2);
  double fdz = 0;
  double fchi2 = fdx + fdy +fdz;
  return fchi2; 
}

// function to be minimized with kink angle due to multiple scattering
void PndLmdLinFitTask::LocalFCN_MS(int &, double *, double & sum, double * par, int ) { 
  TGraph2DErrors * gr = dynamic_cast<TGraph2DErrors*>( (TVirtualFitter::GetFitter())->GetObjectFit());
  assert(gr != 0);
  double * x = gr->GetX();
  double * y = gr->GetY();
  double * z = gr->GetZ();
  double * errx = gr->GetEX();
  double * erry = gr->GetEY();
  double * errz = gr->GetEZ();
  int npoints = gr->GetN();
  sum = 0;
  //  bool *ffhitMerge = PndLmdLinFitTask::Instance()->hitMergedfl;
  // double erral_m = PndLmdLinFitTask::Instance()->GetSigmaMSmerged();
  double erral = PndLmdLinFitTask::Instance()->GetSigmaMS();
  double errAlreal[4];
  //  cout<<"erral = "<<erral<<endl;
  for (int i  = 0; i < npoints; ++i) { 
    //    errAlreal[i] = erral_m;
    errAlreal[i] = erral;
    // if(i>0){
    //   if(ffhitMerge[i-1]){
    // 	errAlreal[i] = erral_m;
    // 	//    	cout<<"prev. Hit was merged: "<<erral_m<<endl;
    //   }
    //   else{
    // 	errAlreal[i] = erral;
    // 	//    	cout<<"prev. Hit is single: "<<erral<<endl;
    //   }
    // }
    // else
    //   errAlreal[i] = PndLmdLinFitTask::Instance()->GetSigmaMScone();
    double zpr[4]={z[0],z[1],z[2],z[2]};
    if(i>2) zpr[3] = z[3];
    double chi2 = distance_MS(x[i],y[i],z[i],errx[i],erry[i],errz[i],par,zpr); 
    sum += chi2;
  }
  double fdal = 0;
  for(int jms=0;jms<npoints-1;jms++){
    fdal += TMath::Power((par[6+jms]/(errAlreal[jms])),2);
    fdal += TMath::Power((par[10+jms]/(errAlreal[jms])),2);
  }
  sum +=fdal;
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
  //min->ExecuteCommand("SET PRINT",arglist,1); //default print
  min->ExecuteCommand("SET PRINT",arglist,0);//no output
   
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
  
  Double_t errRx = 0.1*TMath::Hypot(ErrX1,ErrX2);
  Double_t errRy = 0.1*TMath::Hypot(ErrY1,ErrY2);
  Double_t errRz = 0.1*TMath::Hypot(ErrZ1,ErrZ2);
  
  //The default minimizer is Minuit
  TVirtualFitter::SetDefaultFitter("Minuit");
  TVirtualFitter *min = TVirtualFitter::Fitter(0,5);
  min->SetObjectFit(gr);
  // min->SetFCN( *SumDistance2 );
  min->SetFCN(*LocalFCN);//using local coordinate in FCN
  Double_t arglist[100];
  arglist[0] = 1;
  //  min->ExecuteCommand("SET PRINT",arglist,1);
 min->ExecuteCommand("SET PRINT",arglist,0);//no output

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
  // min->SetParameter(0,"x0",pStart[0],pStartErr[0],pStart[0]-5*pStartErr[0],pStart[0]+5*pStartErr[0]);
  // min->SetParameter(1,"Ax",pStart[1],pStartErr[1],pStart[1]-5*pStartErr[1],pStart[0]+5*pStartErr[1]);
  // min->SetParameter(2,"y0",pStart[2],pStartErr[2],pStart[2]-5*pStartErr[2],pStart[0]+5*pStartErr[2]);
  // min->SetParameter(3,"Ay",pStart[3],pStartErr[3],pStart[3]-5*pStartErr[3],pStart[0]+5*pStartErr[3]);
  min->SetParameter(4,"z0",pStart[4],0,0,0);
  // min->SetParameter(4,"z0",pStart[4],pStartErr[4],0,0);
  min->SetParameter(5,"Az",pStart[5],0,0,0);
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
   if((fitpar[1]*fitpar[1]+fitpar[3]*fitpar[3])<1.){
   fitpar[5]=sqrt(1-fitpar[1]*fitpar[1]-fitpar[3]*fitpar[3]);
   }
   else{
     fitpar[5]=0;
     amin=1e9;
   }
   // double norm_vec = sqrt(fitpar[1]*fitpar[1]+fitpar[3]*fitpar[3]+fitpar[5]*fitpar[5]);
   // double inv_norm_vec =  1./norm_vec;
   // fitpar[1] *= inv_norm_vec;
   // fitpar[3] *= inv_norm_vec;
   // fitpar[5] *= inv_norm_vec;

   // cout<<" after fitpar[5] = "<<fitpar[5]<<endl;
   // cout<<"SEED DIR=("<<dirSeed.X()<<", "<<dirSeed.Y()<<", "<<dirSeed.Z()<<")"<<endl;
   // cout<<"RecTRK DIR=("<<fitpar[1]<<", "<<fitpar[3]<<", "<<fitpar[5]<<")"<<endl;
   for(size_t i=0;i<4;i++){
     for(size_t j=0;j<4;j++){
       (*covmatrix)(i,j)= min->GetCovarianceMatrixElement(i,j);
     }
   }
      (*covmatrix)(4,4) =  gr->GetErrorZ(0)*gr->GetErrorZ(0)/12.;
   //   (*covmatrix)(4,4) =  0.;//TEST
   double dp5_dp1 = fitpar[1]/fitpar[5];
   double dp5_dp3 = fitpar[3]/fitpar[5];
   double errdz2 = pow(dp5_dp1,2)*(*covmatrix)(1,1) + pow(dp5_dp3,2)*(*covmatrix)(3,3) + 
     2*fabs(dp5_dp1*dp5_dp3*(*covmatrix)(1,3));
   (*covmatrix)(5,5) = errdz2;
   //  (*covmatrix)(5,5) = 0.01*errdz2/12.;//TEST
   Double_t chi2 = amin/(2.*Npoint-4);
   //  cout<<"After fit: Chi^2 = "<<chi2<<endl;
   ///-------------------------------------------------------------
  
   return chi2; 
}

double PndLmdLinFitTask::ScatteredAngle(double radLen){
  //Calculation of ThetaMS -------------------------------------
  //Charge & mass of particle
  Int_t PDGCode = -2212; //barp
  Double_t fMass = 0.938272046;
  Double_t Ebeam = TMath::Hypot(fPbeam,fMass);
  TLorentzVector LorMom(0, 0, fPbeam, Ebeam);
  Double_t beta = LorMom.Beta();
  Double_t X_to_X0 =  radLen;
  //  Double_t X_to_X0 = 0.00306;//for one plane: flexcable+(HV-MAPS)+cooling disk+(HV-MAPS)+flexcable
  // if(!isMerged){
  //   X_to_X0 -= 0.00053;//-(HV-MAPS)
  // }
  //X_to_X0 -= 2*0.00053;//-(HV-MAPS)
  //  Double_t thetaMS = 0.2*13.6*1e-3*TMath::Sqrt(X_to_X0)*(1+0.038*TMath::Log(X_to_X0))/(beta*fPbeam);
  Double_t thetaMS = 13.6*1e-3*TMath::Sqrt(X_to_X0)*(1+0.038*TMath::Log(X_to_X0))/(beta*fPbeam);
  cout<<"for Pbeam = "<<fPbeam<<" thetaMS="<<thetaMS<<endl;
  return thetaMS;
}

//chi2 with kink angles fit [G.Lutz, NIM A273 (1988)]
double PndLmdLinFitTask::line3DfitMS(Int_t nd, TGraph2DErrors* gr, TVector3 posSeed, TVector3 dirSeed, Double_t* fitpar, TMatrixDSym *covmatrix)
{
  if(fVerbose>2) cout<<"PndLmdLinFitTask::line3Dfit with SEED is used (multiple scattering taking into account with kinks)"<<endl;
  Int_t Npoint = gr->GetN();
  Double_t ErrX1 = gr->GetErrorX(0);
  Double_t ErrY1 = gr->GetErrorY(0);
  Double_t ErrZ1 = gr->GetErrorZ(0);
  Double_t ErrX2 = gr->GetErrorX(1);
  Double_t ErrY2 = gr->GetErrorY(1);
  Double_t ErrZ2 = gr->GetErrorY(1);
  
  Double_t errRx = 1*TMath::Hypot(ErrX1,ErrX2);
  Double_t errRy = 1*TMath::Hypot(ErrY1,ErrY2);
  Double_t errRz = 1*TMath::Hypot(ErrZ1,ErrZ2);
  
  //The default minimizer is Minuit
  //  fmin->Clear();
  //  TVirtualFitter::SetDefaultFitter("Minuit");
  //  TVirtualFitter *min = TVirtualFitter::Fitter(0,12);
  //  TVirtualFitter *min = TVirtualFitter::Fitter(0,15);
  fmin->SetObjectFit(gr);
  fmin->SetFCN(*LocalFCN_MS);
  Double_t arglist[100];
  arglist[0] = 1;
  //  fmin->ExecuteCommand("SET PRINT",arglist,0);//no output
  fmin->ExecuteCommand("SET PRINT",arglist,1);//output

  if(fVerbose>5){
    cout<<"Number of hits = "<<Npoint<<endl;
    cout<<"posSeed:"<<endl;
    posSeed.Print();
    cout<<"dirSeed:"<<endl;
    dirSeed.Print();
  }
  double l = 1/dirSeed.Mag();
  double pStart[14] = {posSeed.X(),l*dirSeed.X(),posSeed.Y(),l*dirSeed.Y(),posSeed.Z(),l*dirSeed.Z(),0,0,0,0,0,0,0,0};
  // double st3MS = fsigmaMS;
  double st3MS = 0;// fitting angels on the last plane doesn't make sense
  double st2MS = fsigmaMS;
  if(Npoint<4) st2MS=0;
  double pStartErr[14] = {ErrX1,errRx,ErrY1,errRy,ErrZ1,errRz,fsigmaMS,fsigmaMS,fsigmaMS,fsigmaMS,fsigmaMS,fsigmaMS,fsigmaMS,fsigmaMS};

  if(fVerbose>5){
    for(int i=0;i<6;i++)
      cout<<"pStartErr["<<i<<"]="<<pStartErr[i]<<endl;
  }
  
  fmin->SetParameter(0,"x0",pStart[0],pStartErr[0],0,0);
  fmin->SetParameter(1,"Ax",pStart[1],pStartErr[1],0,0);
  fmin->SetParameter(2,"y0",pStart[2],pStartErr[2],0,0);
  fmin->SetParameter(3,"Ay",pStart[3],pStartErr[3],0,0);
  fmin->SetParameter(4,"z0",pStart[4],0,0,0);
  fmin->SetParameter(5,"Az",pStart[5],0,0,0);
  // fmin->SetParameter(4,"z0",pStart[4],pStartErr[4],0,0);
  // fmin->SetParameter(5,"Az",pStart[5],pStartErr[5],0,0);
  fmin->SetParameter(6,"al0x",pStart[6],fsigmaMS,0,0);
  //  fmin->SetParameter(6,"al0x",pStart[6],0,0,0);
  fmin->SetParameter(7,"al1x",pStart[6],fsigmaMS,0,0);
  fmin->SetParameter(8,"al2x",pStart[6],st2MS,0,0);
  fmin->SetParameter(9,"al3x",pStart[6],st3MS,0,0);
  fmin->SetParameter(10,"al0y",pStart[6],fsigmaMS,0,0);
  //  fmin->SetParameter(10,"al0y",pStart[6],0,0,0);
  fmin->SetParameter(11,"al1y",pStart[6],fsigmaMS,0,0);
  fmin->SetParameter(12,"al2y",pStart[6],st2MS,0,0);
  fmin->SetParameter(13,"al3y",pStart[6],st3MS,0,0);


  // Now ready for minimization step
  arglist[0] = 3500;
  arglist[1] = 1.;
  fmin->ExecuteCommand("MIGRAD", arglist,2);
  
  ///Get results ---------------------------------------------
   int nvpar,nparx; 
   double amin,edm, errdef;
   fmin->GetStats(amin,edm,errdef,nvpar,nparx);
   if(fVerbose>1)
     fmin->PrintResults(1,amin);

   Double_t fitparerr[14];
   // get fit parameters
   for (int i = 0; i <14; ++i){
      fitpar[i] = fmin->GetParameter(i); 
      fitparerr[i] = fmin->GetParError(i);
   }
  
  
   // for(size_t i=0;i<4;i++){
   //   for(size_t j=0;j<4;j++){
   for(size_t i=0;i<6;i++){
     for(size_t j=0;j<6;j++){
       (*covmatrix)(i,j)= fmin->GetCovarianceMatrixElement(i,j);
     }
   }

   //!!!!!!!!!!!!!!!!!! dx, dy and their errors should be corrected since dx = dx0+alx0; dy = dy0+aly0; 
   // fitpar[1] +=  fitpar[6]+fitpar[7];
   // fitpar[3] +=  fitpar[10]+fitpar[11];
   // fitpar[1] +=  fitpar[6]+fitpar[7]+fitpar[8]+fitpar[9];
   // fitpar[3] +=  fitpar[10]+fitpar[11]+fitpar[12]+fitpar[13];
   int i_dxal0 = 6-2;//index of alx0 parameter: z0,dz - fixed
   int i_dyal0 = 10-2-1;//index of aly0 parameter: z0,dz - fixed, alx3 - fixed,
   if(st2MS==0) i_dyal0 -=1; //alx2 - fixed

   double err_dx_2 = (*covmatrix)(1,1);
   double err_dy_2 = (*covmatrix)(3,3);
   // double err_dx_2, err_dy_2;
   // int Nang = 3;
   // if(st2MS==0) Nang -=1;
   int Nang = 2;
   for(int iadd=0;iadd<Nang;iadd++){
     fitpar[1] += fitpar[6+iadd];
     fitpar[3] += fitpar[10+iadd];
     // err_dx_2+=2*(fmin->GetCovarianceMatrixElement(1,(i_dxal0+iadd)));
     // err_dy_2+=2*(fmin->GetCovarianceMatrixElement(3,(i_dyal0+iadd)));
     // // err_dx_2+=2*(fmin->GetCovarianceMatrixElement(1,(i_dyal0+iadd)));
     // // err_dy_2+=2*(fmin->GetCovarianceMatrixElement(3,(i_dxal0+iadd)));
     // for(int jadd=0;jadd<Nang;jadd++){
     //   // // err_dx_2+=(fmin->GetCovarianceMatrixElement(1,(i_dxal0+iadd)));
     //   // // err_dx_2+=(fmin->GetCovarianceMatrixElement(1,(i_dxal0+jadd)));
     //   // // err_dy_2+=(fmin->GetCovarianceMatrixElement(3,(i_dyal0+iadd)));
     //   // // err_dy_2+=(fmin->GetCovarianceMatrixElement(3,(i_dyal0+jadd)));
     //   err_dx_2+=(fmin->GetCovarianceMatrixElement((i_dxal0+iadd),(i_dxal0+jadd)));
     //   err_dy_2+=(fmin->GetCovarianceMatrixElement((i_dyal0+iadd),(i_dyal0+jadd)));
     //   // // err_dx_2+=(fmin->GetCovarianceMatrixElement((i_dyal0+iadd),(i_dyal0+jadd)));
     //   // // err_dy_2+=(fmin->GetCovarianceMatrixElement((i_dxal0+iadd),(i_dxal0+jadd)));
     // }
   }
   // //   double err_dy_2 = (*covmatrix)(3,3)+(*covmatrix)(i_dyal0,i_dyal0)+2*(*covmatrix)(3,i_dyal0);// fmin->GetCovarianceMatrixElement(i,j);
   cout<<"Before:   (*covmatrix)(1,1) = "<<(*covmatrix)(1,1)<<endl;
   cout<<"Before:   (*covmatrix)(3,3) = "<<(*covmatrix)(3,3)<<endl;
   if(err_dx_2<0) err_dx_2*=-1;
   (*covmatrix)(1,1)=err_dx_2;
   if(err_dy_2<0) err_dy_2*=-1;
   (*covmatrix)(3,3)=err_dy_2;
   cout<<"After:   (*covmatrix)(1,1) = "<<(*covmatrix)(1,1)<<endl;
   cout<<"After:   (*covmatrix)(3,3) = "<<(*covmatrix)(3,3)<<endl;
   // //!!!!!!!!!!!!!!!!!!

   if((fitpar[1]*fitpar[1]+fitpar[3]*fitpar[3])<1.){
   fitpar[5]=sqrt(1-fitpar[1]*fitpar[1]-fitpar[3]*fitpar[3]);
   }
   else{
     fitpar[5]=0;
     amin=1e9;
   }
  
   //!!!!!!!!!!!!!!!!!! z0, dz weren't fitted
   (*covmatrix)(4,4) =  (gr->GetErrorZ(0)*gr->GetErrorZ(0))/12.;
   double dp5_dp1 = fitpar[1]/fitpar[5];
   double dp5_dp3 = fitpar[3]/fitpar[5];
   double errdz2 = pow(dp5_dp1,2)*(*covmatrix)(1,1) + pow(dp5_dp3,2)*(*covmatrix)(3,3) + 
     2*fabs(dp5_dp1*dp5_dp3*(*covmatrix)(1,3));
   (*covmatrix)(5,5) = errdz2;
   //!!!!!!!!!!!!!!!!!!

  

   Double_t chi2 = amin/(2.*Npoint-4);
   //  cout<<"After fit: Chi^2 = "<<chi2<<endl;
   ///-------------------------------------------------------------
   //  ttal = new TNtuple("ttal","kink angles","alx0:alx1:alx2:aly0:aly1:aly2:erralx1:erralx2:erraly1:erraly2:npoints") ;
   if(fVerbose>2)  ttal->Fill(fitpar[6],fitpar[7],fitpar[8],fitpar[10],fitpar[11],fitpar[12],fitparerr[7],fitparerr[8],fitparerr[11],fitparerr[12],Npoint);

   fmin->Clear();
   //delete min;
   return chi2; 
}

void PndLmdLinFitTask::Finish()
{
  if(fVerbose>2){
    TTree *nout1 = ttal->CloneTree();
    nout1->Write();
  }
}

ClassImp(PndLmdLinFitTask);
