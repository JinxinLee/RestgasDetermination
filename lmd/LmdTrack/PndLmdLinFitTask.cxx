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
#include <TFitterMinuit.h>
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
  fsigmaMSa = 0;
  fsigmaMSb = 0;
  PndGeoHandling::Instance();
  fInstance=this;
  //  ttal = new TNtuple("ttal","kink angles","alx0:alx1:alx2:alx3:aly0:aly1:aly2:aly3:npoints") ;
  //  ttal = new TNtuple("ttal","kink angles","alx0:alx1:alx2:aly0:aly1:aly2:erralx1:erralx2:erraly1:erraly2:npoints") ;
  ttal = new TTree("ttal","kink angles");
  ttal->Branch("alx0a",& falx0a);
  ttal->Branch("alx0b",& falx0b);
  ttal->Branch("alx1a",& falx1a);
  ttal->Branch("alx1b",& falx1b);
  ttal->Branch("alx2a",& falx2a);
  ttal->Branch("alx2b",& falx2b);
  ttal->Branch("alx3a",& falx3a);
  ttal->Branch("alx3b",& falx3b);
  ttal->Branch("aly0a",& faly0a);
  ttal->Branch("aly0b",& faly0b);
  ttal->Branch("aly1a",& faly1a);
  ttal->Branch("aly1b",& faly1b);
  ttal->Branch("aly2a",& faly2a);
  ttal->Branch("aly2b",& faly2b);
  ttal->Branch("aly3a",& faly3a);
  ttal->Branch("aly3b",& faly3b);
 ttal->Branch("erralx0a",& ferralx0a);
  ttal->Branch("erralx0b",& ferralx0b);
  ttal->Branch("erralx1a",& ferralx1a);
  ttal->Branch("erralx1b",& ferralx1b);
  ttal->Branch("erralx2a",& ferralx2a);
  ttal->Branch("erralx2b",& ferralx2b);
  ttal->Branch("erralx3a",& ferralx3a);
  ttal->Branch("erralx3b",& ferralx3b);
  ttal->Branch("erraly0a",& ferraly0a);
  ttal->Branch("erraly0b",& ferraly0b);
  ttal->Branch("erraly1a",& ferraly1a);
  ttal->Branch("erraly1b",& ferraly1b);
  ttal->Branch("erraly2a",& ferraly2a);
  ttal->Branch("erraly2b",& ferraly2b);
  ttal->Branch("erraly3a",& ferraly3a);
  ttal->Branch("erraly3b",& ferraly3b);
  ttal->Branch("npoints",& fnpoints);
  ttal->Branch("chi2",& fchi2);
 TVirtualFitter::SetDefaultFitter("Minuit2");
  fmin = TVirtualFitter::Fitter(0,25);
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
  //  fsigmaMS = 0;
  fsigmaMSa = 0;
  fsigmaMSb = 0;
  PndGeoHandling::Instance();
  fInstance=this;
  ttal = new TTree("ttal","kink angles");
  ttal->Branch("alx0a",& falx0a);
  ttal->Branch("alx0b",& falx0b);
  ttal->Branch("alx1a",& falx1a);
  ttal->Branch("alx1b",& falx1b);
  ttal->Branch("alx2a",& falx2a);
  ttal->Branch("alx2b",& falx2b);
  ttal->Branch("alx3a",& falx3a);
  ttal->Branch("alx3b",& falx3b);
  ttal->Branch("aly0a",& faly0a);
  ttal->Branch("aly0b",& faly0b);
  ttal->Branch("aly1a",& faly1a);
  ttal->Branch("aly1b",& faly1b);
  ttal->Branch("aly2a",& faly2a);
  ttal->Branch("aly2b",& faly2b);
  ttal->Branch("aly3a",& faly3a);
  ttal->Branch("aly3b",& faly3b);
 ttal->Branch("erralx0a",& ferralx0a);
  ttal->Branch("erralx0b",& ferralx0b);
  ttal->Branch("erralx1a",& ferralx1a);
  ttal->Branch("erralx1b",& ferralx1b);
  ttal->Branch("erralx2a",& ferralx2a);
  ttal->Branch("erralx2b",& ferralx2b);
  ttal->Branch("erralx3a",& ferralx3a);
  ttal->Branch("erralx3b",& ferralx3b);
  ttal->Branch("erraly0a",& ferraly0a);
  ttal->Branch("erraly0b",& ferraly0b);
  ttal->Branch("erraly1a",& ferraly1a);
  ttal->Branch("erraly1b",& ferraly1b);
  ttal->Branch("erraly2a",& ferraly2a);
  ttal->Branch("erraly2b",& ferraly2b);
  ttal->Branch("erraly3a",& ferraly3a);
  ttal->Branch("erraly3b",& ferraly3b);
  ttal->Branch("npoints",& fnpoints);
 ttal->Branch("chi2",& fchi2);
 ttal->Branch("zhit0",&fzhit0);

  TVirtualFitter::SetDefaultFitter("Minuit2");
  fmin = TVirtualFitter::Fitter(0,25);
  //fmin->SetPrintLevel(fVerbose);//0=suppression of printing
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
  double totRadLen = 0.00306;//rad.length of whole plane
  totRadLen -=2*0.000175;// -flex-cable
  totRadLen -=0.00053;// -sensor
  fsigmaMSb = ScatteredAngle(totRadLen);
  double totRadLenCable = 2*0.000175;// 2*flex-cable only
  totRadLenCable +=0.00053;
  fsigmaMSa = ScatteredAngle(totRadLenCable);

  if(lmddim!=0 ) lmddim->Cleanup();
  lmddim = PndLmdDim::Instance();
  TString mtx_perfect = "../../../pandaroot/input/trafo_matrices_lmd.dat";
  lmddim -> Read_transformation_matrices(mtx_perfect.Data(), false);
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
   

    TGraph2DErrors fitme(numPts); //new graph for fitting
    Int_t firstHit=-1, lastHit=-1;
    TVector3 hit0,hit1;
    for(unsigned int ihit=0; ihit<numPts; ihit++){ //fill Graph
      PndTrackCandHit theHit = trcnd->GetSortedHit(ihit); //get hit
      Int_t index = theHit.GetHitId();
      Int_t detId = theHit.GetDetId();
      // if(fVerbose>2) std::cout << "Point: "<< ihit<< " index: "<< index <<std::endl;

      //      PndSdsHit* addHit = (PndSdsHit*) fRecoArray->At(index);
      PndSdsMergedHit* addHit = (PndSdsMergedHit*) fRecoArray->At(index);
      //      cout<<"IsMerged??? "<<addHit->GetIsMerged()<<endl;
      hitMergedfl[ihit] = addHit->GetIsMerged();


      TVector3 addPos = addHit->GetPosition();
      double xhit = addPos.X();       double yhit = addPos.Y();       double zhit = addPos.Z();
      //      lmddim->Transform_global_to_lmd_local(xhit,yhit,zhit,false);
      TVector3 addPos2(xhit,yhit,zhit);
      if(ihit==0){
         firstHit=index;
	 hit0 = addPos2;
      }
      else{
	if(ihit==2) 	  hit1 = addPos2;
	if(ihit==numPts-1){
          lastHit=index;
	  //	  hit1 = addPos2;
	}
      }
      double errxhit = addHit->GetDx();       double erryhit = addHit->GetDy();       double errzhit = addHit->GetDz();
      fitme.SetPoint(ihit, xhit,yhit,zhit);
      fitme.SetPointError(ihit, errxhit,erryhit,errzhit);
    }//end of Hits in TCand

    TVector3 dirSeed =  hit1 - hit0;
    TVector3 posSeed =  hit0;
    // cout<<"DirSeed before fit and norm:"<<endl;
    // dirSeed.Print();
    dirSeed *=1./dirSeed.Mag();
    Double_t parFit[22]; //fit-parameter
    TMatrixDSym *COVmatrix = new TMatrixDSym(6);
    Double_t accuracy = line3DfitMS(numPts, &fitme, posSeed, dirSeed, parFit, COVmatrix);//with kink angles
    //    Double_t accuracy = line3Dfit(numPts, &fitme, posSeed, dirSeed, parFit, COVmatrix); //MS taking into account by hits errors
    
    //save as PndTrack
    TVector3 FitPoint(parFit[0], parFit[2], parFit[4]);
    TVector3 FitDir(parFit[1], parFit[3], parFit[5]);
    TVector3 FitMom = FitDir*fPbeam;
    Double_t COVmatrixPosMom[6][6];
    
    for(int ij=0;ij<6;ij++){
      for(int ji=0;ji<6;ji++)
	COVmatrixPosMom[ij][ji] = 0;
    }  

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
    TString path = fGeoH->GetPath(id);
    TVector3 oo, uu, vv;
     fGeoH->GetOUVShortId(id, oo,uu,vv);
     // TVector3 o = oo;
     TVector3 o = FitPoint;
     TVector3 dj = uu;
     TVector3 dk = vv;

     FairTrackParP *trkfitp = new  FairTrackParP(FitPoint,FitMom,COVmatrixPosMom,fCharge,o,dj,dk);
     int flagpndtrk=0;
     if(accuracy>1e3) flagpndtrk=-1;//quality of track: bad if chi2 is too large
     int ndf = 2*(trcnd->GetNHits())-4;//number d.o.f
     int pid = fPDGCode;
     
     PndTrack *trackfit = new PndTrack(*trkfitp,*trkfitp, *trcnd,flagpndtrk,accuracy,ndf,pid,track,sysID);//TODO: FairTrackParP at 1st and last point???
     new((*fTrackArray)[rec_tkr]) PndTrack(*(trackfit)); //save Track

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
  double THfunc[8] = {1,1,1,1,1,1,1,1};
  Double_t t_min = p[1]*(x-p[0])+p[3]*(y-p[2])+p[5]*(z-p[4]);
  for(int iz=0;iz<8;iz++)
    if(((p[4]+t_min)-zpr[iz])<=0) THfunc[iz]=0;

  double funcX = p[0]+p[1]*t_min
    +(-p[6]+p[7])*((p[4]+t_min)-zpr[0])*THfunc[0]
    +(-p[8]+p[9])*((p[4]+t_min)-zpr[2])*THfunc[2]
    +(-p[10]+p[11])*((p[4]+t_min)-zpr[4])*THfunc[4]
    +(-p[12]+p[13])*((p[4]+t_min)-zpr[6])*THfunc[6];
  double funcY = p[2]+p[3]*t_min
    +(-p[14]+p[15])*((p[4]+t_min)-zpr[0])*THfunc[0]
    +(-p[16]+p[17])*((p[4]+t_min)-zpr[2])*THfunc[2]
    +(-p[18]+p[19])*((p[4]+t_min)-zpr[4])*THfunc[4]
    +(-p[20]+p[21])*((p[4]+t_min)-zpr[6])*THfunc[6];

  double fdx = TMath::Power((x-funcX)/errx,2);
  double fdy = TMath::Power((y-funcY)/erry,2);
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
  double erralA = PndLmdLinFitTask::Instance()->GetSigmaMS(0);
  double erralB = PndLmdLinFitTask::Instance()->GetSigmaMS(1);
  double zpr[8];
  int izcur = 0;
    for(int izpl = 0;izpl<4;izpl++){
      zpr[izcur] = z[izpl];
      izcur++;
      zpr[izcur] = z[izpl];
      izcur++;
    }
    for (int i  = 0; i < npoints; ++i) {  
      double chi2 = distance_MS(x[i],y[i],z[i],errx[i],erry[i],errz[i],par,zpr); 
      sum += chi2;
    }
  double fdal = 0;
  for(int jms=0;jms<8;jms=jms+2){
    double errTot = sqrt(erralA*erralA+erralB*erralB);
    fdal += TMath::Power(((par[7+jms]-par[6+jms])/errTot),2);
    fdal += TMath::Power(((par[15+jms]-par[14+jms])/errTot),2);
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
  min->SetFCN(*LocalFCN);//using local coordinate in FCN
  Double_t arglist[100];
  arglist[0] = 1;
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
  min->SetFCN(*LocalFCN);//using local coordinate in FCN
  Double_t arglist[100];
  arglist[0] = 1;
  min->ExecuteCommand("SET PRINT",arglist,0);//no output

  if(fVerbose>2){
    cout<<"posSeed:"<<endl;
    posSeed.Print();
    cout<<"dirSeed:"<<endl;
    dirSeed.Print();
  }
  double l = 1/dirSeed.Mag();

  double pStart[6] = {posSeed.X(),dirSeed.X(),posSeed.Y(),dirSeed.Y(),posSeed.Z(),dirSeed.Z()};
  // //  //go out of middle of plane!
  // double pStart[6] = {posSeed.X(),l*dirSeed.X(),posSeed.Y(),l*dirSeed.Y(),posSeed.Z()-0.0085,l*dirSeed.Z()};//85 mkm!!!
  
  double pStartErr[6] = {ErrX1,errRx,ErrY1,errRy,ErrZ1,errRz};
  if(fVerbose>2){
    for(int i=0;i<6;i++)
      cout<<"pStartErr["<<i<<"]="<<pStartErr[i]<<endl;
  }
  min->SetParameter(0,"x0",pStart[0],pStartErr[0],0,0);
  min->SetParameter(1,"Ax",pStart[1],pStartErr[1],0,0);
  min->SetParameter(2,"y0",pStart[2],pStartErr[2],0,0);
  min->SetParameter(3,"Ay",pStart[3],pStartErr[3],0,0);
  min->SetParameter(4,"z0",pStart[4],0,0,0);
  min->SetParameter(5,"Az",pStart[5],0,0,0);
  
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
  
   if((fitpar[1]*fitpar[1]+fitpar[3]*fitpar[3])<1.){
   fitpar[5]=sqrt(1-fitpar[1]*fitpar[1]-fitpar[3]*fitpar[3]);
   }
   else{
     fitpar[5]=0;
     amin=1e9;
   }

   for(size_t i=0;i<4;i++){
     for(size_t j=0;j<4;j++){
       (*covmatrix)(i,j)= min->GetCovarianceMatrixElement(i,j);
     }
   }
      (*covmatrix)(4,4) =  gr->GetErrorZ(0)*gr->GetErrorZ(0)/12.;
   double dp5_dp1 = fitpar[1]/fitpar[5];
   double dp5_dp3 = fitpar[3]/fitpar[5];
   double errdz2 = pow(dp5_dp1,2)*(*covmatrix)(1,1) + pow(dp5_dp3,2)*(*covmatrix)(3,3) + 
     2*fabs(dp5_dp1*dp5_dp3*(*covmatrix)(1,3));
   (*covmatrix)(5,5) = errdz2;

   Double_t chi2 = amin/(2.*Npoint-4);
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
  Double_t thetaMS = 13.6*1e-3*TMath::Sqrt(X_to_X0)*(1+0.038*TMath::Log(X_to_X0))/(beta*fPbeam);
  //cout<<"for Pbeam = "<<fPbeam<<" thetaMS="<<thetaMS<<endl;
  return thetaMS;
}

//chi2 with kink angles fit [G.Lutz, NIM A273 (1988)]
double PndLmdLinFitTask::line3DfitMS(Int_t nd, TGraph2DErrors* gr, TVector3 posSeed, TVector3 dirSeed, Double_t* fitpar, TMatrixDSym *covmatrix)
{
  fmin->Clear();
  const int nparams = 22;
  if(fVerbose>2) cout<<"PndLmdLinFitTask::line3Dfit with SEED is used (multiple scattering taking into account with kinks)"<<endl;
  Int_t Npoint = gr->GetN();
  Double_t ErrX1 = gr->GetErrorX(0);
  Double_t ErrY1 = gr->GetErrorY(0);
  Double_t ErrZ1 = gr->GetErrorZ(0);
  TVector3 ErrPosSeed(ErrX1,ErrY1,ErrZ1);
  Double_t ErrX2 = gr->GetErrorX(2);
  Double_t ErrY2 = gr->GetErrorY(2);
  Double_t ErrZ2 = gr->GetErrorY(2);
  
  Double_t errRx = 1*TMath::Hypot(ErrX1,ErrX2);
  Double_t errRy = 1*TMath::Hypot(ErrY1,ErrY2);
  Double_t errRz = 1*TMath::Hypot(ErrZ1,ErrZ2);
  
  fmin->SetObjectFit(gr);
  fmin->SetFCN(*LocalFCN_MS);
  Double_t arglist[100];
  arglist[0] = 1;
  //  fmin->ExecuteCommand("SET PRINT",arglist,10);//output
  //  fmin->ExecuteCommand("SET PRINT",arglist,0);//no output
  ((TFitterMinuit*)fmin->GetFitter())->SetPrintLevel(fVerbose);
  if(fVerbose>5){
    cout<<"Number of hits = "<<Npoint<<endl;
    cout<<"posSeed:"<<endl;
    posSeed.Print();
    cout<<"ErrposSeed:"<<endl;
    ErrPosSeed.Print();
    cout<<"dirSeed:"<<endl;
    dirSeed.Print();
  }
  double l = 1/dirSeed.Mag();
  double pStart[nparams] = {posSeed.X(),l*dirSeed.X(),posSeed.Y(),l*dirSeed.Y(),posSeed.Z(),l*dirSeed.Z(),0,0,0,0,0,0,0,0};
  double pStartErr[nparams] = {ErrX1,errRx,ErrY1,errRy,ErrZ1,errRz,1e-2*fsigmaMSa,1e-2*fsigmaMSb,1e-2*fsigmaMSa,1e-2*fsigmaMSb,1e-2*fsigmaMSa,1e-2*fsigmaMSb,1e-2*fsigmaMSa,1e-2*fsigmaMSb,1e-2*fsigmaMSb,1e-2*fsigmaMSa,1e-2*fsigmaMSb,1e-2*fsigmaMSa,1e-2*fsigmaMSb,1e-2*fsigmaMSa,1e-2*fsigmaMSb};
 
  
  
  fmin->SetParameter(0,"x0",pStart[0],pStartErr[0],0,0);
  fmin->SetParameter(1,"Ax",pStart[1],pStartErr[1],0,0);
  fmin->SetParameter(2,"y0",pStart[2],pStartErr[2],0,0);
  fmin->SetParameter(3,"Ay",pStart[3],pStartErr[3],0,0);

  fmin->SetParameter(4,"z0",pStart[4],0,0,0);
  fmin->SetParameter(5,"Az",pStart[5],0,0,0);
  
  fmin->SetParameter(6,"al0x_a",pStart[6],1e-4*fsigmaMSa,0,0);
  fmin->SetParameter(7,"al0x_b",pStart[7],1e-4*fsigmaMSb,0,0);
  fmin->SetParameter(8,"al1x_a",pStart[8],1e-4*fsigmaMSa,0,0);
  fmin->SetParameter(9,"al1x_b",pStart[9],1e-4*fsigmaMSb,0,0);
  fmin->SetParameter(10,"al2x_a",pStart[10],1e-4*fsigmaMSa,0,0);
  fmin->SetParameter(11,"al2x_b",pStart[11],1e-4*fsigmaMSb,0,0);
  fmin->SetParameter(12,"al3x_a",pStart[12],1e-4*fsigmaMSa,0,0);
  fmin->SetParameter(13,"al3x_b",pStart[13],1e-4*fsigmaMSb,0,0);

  fmin->SetParameter(14,"al0y_a",pStart[14],1e-4*fsigmaMSa,0,0);
  fmin->SetParameter(15,"al0y_b",pStart[15],1e-4*fsigmaMSb,0,0);
  fmin->SetParameter(16,"al1y_a",pStart[16],1e-4*fsigmaMSa,0,0);
  fmin->SetParameter(17,"al1y_b",pStart[17],1e-4*fsigmaMSb,0,0);
  fmin->SetParameter(18,"al2y_a",pStart[18],1e-4*fsigmaMSa,0,0);
  fmin->SetParameter(19,"al2y_b",pStart[19],1e-4*fsigmaMSb,0,0);
  fmin->SetParameter(20,"al3y_a",pStart[20],1e-4*fsigmaMSa,0,0);
  fmin->SetParameter(21,"al3y_b",pStart[21],1e-4*fsigmaMSb,0,0);


  fmin->FixParameter(8);
  fmin->FixParameter(10);
  fmin->FixParameter(12);
  fmin->FixParameter(16);
  fmin->FixParameter(18);
  fmin->FixParameter(20);

  fmin->FixParameter(13);
  fmin->FixParameter(21);
  if(Npoint<4){
  fmin->FixParameter(11);
  fmin->FixParameter(19);
  }

  double recpres = 1e-7;
  // Now ready for minimization step
   arglist[0] = 5000;
  //  arglist[0] = 2;
  arglist[1] = recpres;
  fmin->ExecuteCommand("MIGRAD", arglist,3);
  fmin->ExecuteCommand("SET PRI", arglist,5);
  ///Get results ---------------------------------------------
  int nvpar,nparx; 
  double amin,edm, errdef;
  fmin->GetStats(amin,edm,errdef,nvpar,nparx);
  if(edm>1e2*recpres) return 1e6; 
  if(fVerbose>1){
    cout<<"------------- Final result ---------------- "<<endl;
    fmin->PrintResults(fVerbose,amin);
  }
  // if(fVerbose>1){
  //   cout<<"------------- Final result ---------------- "<<endl;
  //   fmin->PrintResults(1,amin);
  // }
  
   Double_t fitparerr[nparams];
   // get fit parameters
   for (int i = 0; i <nparams; ++i){
      fitpar[i] = fmin->GetParameter(i); 
      fitparerr[i] = fmin->GetParError(i);
   }
  
  
   for(size_t i=0;i<6;i++){
     for(size_t j=0;j<6;j++){
       (*covmatrix)(i,j)= fmin->GetCovarianceMatrixElement(i,j);
     }
   }

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
   ///-------------------------------------------------------------
   if(fVerbose>2){
     falx0a = fitpar[6];
     ferralx0a = fitparerr[6];
     falx1a = fitpar[8];
     ferralx1a = fitparerr[8];
     falx2a = fitpar[10];
     ferralx2a = fitparerr[10];
     falx3a = fitpar[12];
     ferralx3a = fitparerr[12];
     faly0a = fitpar[14];
     ferraly0a = fitparerr[14];
     faly1a = fitpar[16];
     ferraly1a = fitparerr[16];
     faly2a = fitpar[18];
     ferraly2a = fitparerr[18];
     faly3a = fitpar[20];
     ferraly3a = fitparerr[20];
     falx0b = fitpar[7];
     ferralx0b = fitparerr[7];
     falx1b = fitpar[9];
     ferralx1b = fitparerr[9];
     falx2b = fitpar[11];
     ferralx2b = fitparerr[11];
     falx3b = fitpar[13];
     ferralx3b = fitparerr[13];
     faly0b = fitpar[15];
     ferraly0b = fitparerr[15];
     faly1b = fitpar[17];
     ferraly1b = fitparerr[17];
     faly2b = fitpar[19];
     ferraly2b = fitparerr[19];
     faly3b = fitpar[21]; 
     ferraly3b = fitparerr[21];

     fnpoints = Npoint;
     fchi2 =  chi2;
     fzhit0 = (gr->GetZ())[0];
     ttal->Fill();
   }

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
