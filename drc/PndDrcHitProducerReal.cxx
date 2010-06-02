// -------------------------------------------------------------------------
// -----                   PndDrcHitProducerReal source file               -----
// -----               Created 30/10/09  by Dipanwita Dutta            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "stdio.h"

#include "PndGeoDrc.h"
#include "PndDrcHitProducerReal.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "TVector3.h"
#include "TRandom.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "TString.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "PndGeoDrcPar.h"
#include "TFormula.h"
#include "TMath.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TGeoManager.h"


using std::endl;
using std::cout;

// -----   Default constructor   -------------------------------------------
PndDrcHitProducerReal::PndDrcHitProducerReal() 
:FairTask("PndDrcHitProducerReal")
{
  fGeo = new PndGeoDrc();
  SetParameters();
  fisDetEff= kTRUE; 
  fisPixel= kTRUE; 

}
// -------------------------------------------------------------------------

// -----   Standard constructor with verbosity level  -------------------------------------------

PndDrcHitProducerReal::PndDrcHitProducerReal(Int_t verbose, Int_t det_type) 
  :FairTask("PndDrcHitProducerReal")
{
  SetParameters();
  fVerbose = verbose;  
  fDetType= det_type; 
  fisDetEff= kTRUE; 
  fisPixel= kTRUE; 
  fGeo = new PndGeoDrc();

}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndDrcHitProducerReal::~PndDrcHitProducerReal()
{
  if (fGeo) delete fGeo;
}

// -----   Initialization   -----------------------------------------------
// -------------------------------------------------------------------------
InitStatus PndDrcHitProducerReal::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDrcHitProducerReal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fBarPointArray = (TClonesArray*) ioman->GetObject("DrcBarPoint");
   if ( ! fBarPointArray ) {
    cout << "-W- PndDrcHitProducerReal::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

// Get Photon point array
   fPDPointArray = (TClonesArray*) ioman->GetObject("DrcPDPoint");
      if ( ! fPDPointArray ) {
          cout << "-W- PndDrcAna::Init: "
                   << "No DrcPDPoint array!" << endl;
                       return kERROR;
      }

   // Create and register output array
   fHitArray = new TClonesArray("PndDrcHit");
   ioman->Register("DrcHit","Drc",fHitArray, kTRUE);
  
   // Create and register output array
    fPDHitArray = new TClonesArray("PndDrcPDHit");
    ioman->Register("DrcPDHit","Drc",fPDHitArray, kTRUE);
//
   cout << "-I- PndDrcHitProducerReal: Intialization successfull" << endl;
  
  return kSUCCESS;
   
}
// -----   Execution of Task   ---------------------------------------------
void PndDrcHitProducerReal::Exec(Option_t* option)
{
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  fHitArray->Clear();
  if ( ! fPDHitArray ) Fatal("Exec", "No Photon HitArray");
  fPDHitArray->Clear();
  nevents++;
  if (fVerbose > 1) printf("\n\n=====> Event No. %d\n", nevents); 

  ProcessBarPoint();
  ProcessPhotonPoint();
}
// -----   Process Bar Point   ---------------------------------------------
void PndDrcHitProducerReal::ProcessBarPoint()
{
  PndDrcBarPoint* pt=NULL;
   
  if (fVerbose > 0) {
    cout << " ----------------- DRC Hit Producer --------------------" << endl;
    cout <<" Number of input MC points in the bar: "<<fBarPointArray->GetEntries()<<endl;
  }

  // fNHits = 0;

  // Loop over PndDrcPoints
  for(Int_t j=0; j<fBarPointArray->GetEntriesFast(); j++) {
    
    pt = (PndDrcBarPoint*)fBarPointArray->At(j);
  
    Double_t Px= pt->GetPx();
    Double_t Py= pt->GetPy();
    Double_t Pz= pt->GetPz();
    Double_t P = sqrt(Px*Px + Py*Py +Pz*Pz);
    Double_t mass = pt->GetMass();
    Double_t energy = TMath::Sqrt(P*P + mass*mass); 

    Double_t beta;
    if(energy != 0) {
      beta = P/energy;
    }
    else {
      beta = -1.;
      if (fVerbose >0) cout << "Beta not calculated " << endl; 
    }

    if (pt->GetThetaC() != -1. && beta > 1/1.47){  
    fDetectorID = pt->GetNBar();
  
    // calculate the center of the bars from the detectorID
    Int_t s = (fDetectorID /10)-1;// correction DD
    Int_t b =  (fDetectorID % 10)-1;

    Double_t rad = TMath::Pi()/180.;
    Double_t r = fGeo->radius();//radius in the middle of the bar =51.2cm
    Double_t hthick        =  fGeo->barHalfThick();//half thickness of the bars=1.7/2 cm
    Double_t rad_out = (r-hthick)/cos((22.5/2)*rad); // radius at corner - thickness ###
    Double_t lside   = 2*rad_out*sin((22.5/2)*rad);
    Double_t phis = (s*22.5+ 258.75 )*rad; // 22.5 degrees are 360/16
    Double_t Xs =  r*cos(phis);
    Double_t Ys = r*sin(phis);
    Double_t thts = phis- TMath::Pi()/2;
    Double_t Xb =  (5*lside/12 - (lside/6)*(b))*cos(thts);
    Double_t Yb =  (5*lside/12 - (lside/6)*(b))*sin(thts);
   
    Double_t fXHit = Xs+Xb;
    Double_t fYHit = -(Ys+Yb);
    Double_t fZHit = 0.;
 
    //  cout << "hit phi: "<< acos(fXHit/r) << endl;
    
    fPosHit.SetXYZ(fXHit,fYHit,fZHit);

    Double_t fDPosXHit = 0.5; //mm
    Double_t fDPosYHit = 0.5;
    Double_t fDPosZHit = 0.;
    fDPosHit.SetXYZ(fDPosXHit,fDPosYHit,fDPosZHit);

    fThetaC = gRandom->Gaus(pt->GetThetaC(),0.008);
    fErrThetaC = 0.; //rad

    fRefIndex = j;

   // PndMCTrack* tr = NULL;

    AddHit(fDetectorID, 
	   fPosHit, 
	   fDPosHit,
	   fThetaC,
	   fErrThetaC,
	   fRefIndex);

    }
  }
}
//-------- Photon Detector Hit production with efficiency-------------   
void PndDrcHitProducerReal::ProcessPhotonPoint()
{  
  if (fVerbose > 0) {
    cout <<" Number of Photon MC Points in Photon Detector Plane : "<<fPDPointArray->GetEntries()<<endl;
  }
  Double_t lambda_min,lambda_max,lambda_step;
  Double_t efficiency[40];
  SetPhotonDetEff(lambda_min,lambda_max,lambda_step,efficiency);

  
  PndDrcPDPoint* Ppt=NULL;

// Loop over PndDrcPDPoints
   for(Int_t k=0; k<fPDPointArray->GetEntriesFast(); k++) {
       Ppt = (PndDrcPDPoint*)fPDPointArray->At(k);

       Double_t PPx= Ppt->GetPx();
       Double_t PPy= Ppt->GetPy();
       Double_t PPz= Ppt->GetPz();

       Double_t etot = sqrt(PPx*PPx + PPy*PPy +PPz*PPz);// in GeV
       Double_t lambda=197.0*2.0*TMath::Pi()/nRefrac/(etot*1.0E9);//wavelength of photon in nm
       detection=0;
       if(fisDetEff){
         if (lambda >= lambda_min && lambda < lambda_max) {
            Int_t ilambda=(Int_t)((lambda-lambda_min)/lambda_step);
	    Double_t rand = gRandom->Rndm();
	    detection = 0;
            if (efficiency[ilambda]*fCollectionEff > rand ) detection = 1;
          }
       }else{ detection=1;}
       Double_t xP= Ppt->GetX();
       Double_t yP= Ppt->GetY();
       Double_t zP= Ppt->GetZ();
    
       Double_t xHit, yHit, zHit;
       Int_t pmtID;

//      fPosPDHit.SetXYZ(xP,yP,zP);
    
      zHit=zP;
      pmtID=k;
      if(fisPixel){
         FindDrcHitPosition(xP, yP, xHit, yHit, pmtID);
      }else{
         xHit=xP;
         yHit=yP;
      }
      fPosPDHit.SetXYZ(xHit,yHit,zHit);

      Double_t fDPosXPDHit = fPixelDim/2; //mm
      Double_t fDPosYPDHit = fPixelDim/2; //mm
      Double_t fDPosZPDHit = 0.;

      fDPosPDHit.SetXYZ(fDPosXPDHit,fDPosYPDHit,fDPosZPDHit);
     
      Double_t time=Ppt->GetTime();
      Smear(time,fSigmat);
      fTime=time;

      fTimeThreshold = 500.; 
      fPDRefIndex = k;

     if(detection==1){
      AddPDHit(fDetectorID, 
	   fPosPDHit, 
	   fDPosPDHit,
	   fTime,
	   fTimeThreshold,
	   fPDRefIndex);
     }	   
  }
}

// -----   Add Hit to HitCollection   --------------------------------------
PndDrcHit* PndDrcHitProducerReal::AddHit(Int_t detID, 
				     TVector3 posHit, 
				     TVector3 dPosHit, 
				     Double_t thetaC,
				     Double_t errThetaC,
				     Int_t index){
  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndDrcHit(detID, 
				    posHit, 
				    dPosHit, 
				    thetaC,
				    errThetaC,
				    index);
}
// -----   Add Photon Detector Hit to HitCollection   --------------------------------------
PndDrcPDHit* PndDrcHitProducerReal::AddPDHit(Int_t detID, 
				     TVector3 posHit, 
				     TVector3 dPosHit, 
				     Double_t time,
				     Double_t timeThreshold,
				     Int_t index1){
  TClonesArray& clrefPD = *fPDHitArray;
  Int_t size = clrefPD.GetEntriesFast();
  return new(clrefPD[size]) PndDrcPDHit(detID, 
				    posHit, 
				    dPosHit, 
				    time,
				    timeThreshold,
				    index1);
}
// -----   Set Photon Detector parameter   -------------------------------------------
void PndDrcHitProducerReal::SetPhotonDetEff(Double_t& fLambdaMin, 
                       Double_t& fLambdaMax, Double_t& fLambdaStep, Double_t fEfficiency[])
{
//  if (fVerbose > 0) cout << "SetPhotoDetParamter called for Photocathode type " << fDetType << endl;

  if (fDetType == 1){

  /** Quantum efficiencies for Bialkali photocathode
       corresponding range in lambda: 280nm - 600nm in steps of 10nm */

  fLambdaMin = 280.;
  fLambdaMax = 600.;
  fLambdaStep = 10.;

  fEfficiency[0] = 0.020;
  fEfficiency[1] = 0.050;
  fEfficiency[2] = 0.110;
  fEfficiency[3] = 0.170;
  fEfficiency[4] = 0.225;
  fEfficiency[5] = 0.260;
  fEfficiency[6] = 0.275;
  fEfficiency[7] = 0.290;
  fEfficiency[8] = 0.310;
  fEfficiency[9] = 0.305;
  fEfficiency[10] = 0.302;
  fEfficiency[11] = 0.290;
  fEfficiency[12] = 0.279;
  fEfficiency[13] = 0.268;
  fEfficiency[14] = 0.262;
  fEfficiency[15] = 0.250;
  fEfficiency[16] = 0.240;
  fEfficiency[17] = 0.225;
  fEfficiency[18] = 0.210;
  fEfficiency[19] = 0.195;
  fEfficiency[20] = 0.178;
  fEfficiency[21] = 0.170;
  fEfficiency[22] = 0.155;
  fEfficiency[23] = 0.130;
  fEfficiency[24] = 0.112;
  fEfficiency[25] = 0.085;
  fEfficiency[26] = 0.066;
  fEfficiency[27] = 0.055;
  fEfficiency[28] = 0.040;
  fEfficiency[29] = 0.030;
  fEfficiency[30] = 0.025;
  fEfficiency[31] = 0.018;
  fEfficiency[32] = 0.010;
  }
  else {
    cout << "ERROR: photocathode type not specified" << endl;

   fLambdaMin = 100.;
   fLambdaMax = 100.;
   fLambdaStep = 100.;

   fEfficiency[0] = 0.;

   }

}  
// -----   Find Photon Hit Position---------------------------------------------------
void PndDrcHitProducerReal::FindDrcHitPosition(Double_t xPoint, Double_t yPoint,
                                          Double_t& xHit, Double_t& yHit, Int_t& pmtID)
{
   Double_t pixelDim = fPixelDim;
   if(xPoint >= 0)
     xHit= pixelDim/2 + pixelDim*((Int_t)(xPoint/pixelDim));
   else
     xHit= -pixelDim/2 + pixelDim*((Int_t)(xPoint/pixelDim));
     
   if(yPoint >= 0)
     yHit= pixelDim/2 + pixelDim*((Int_t)(yPoint/pixelDim));
   else
     yHit= -pixelDim/2 + pixelDim*((Int_t)(yPoint/pixelDim));

} 
//-------------Set Parameter------------------------------------
void PndDrcHitProducerReal::SetParameters(){
  fDetType=1; //  Detector Type =1
  fPixelDim=0.65; // Pixel Dimension of photocathode is 6.5mm
  nRefrac=1.467;  //Refractive index of SOB
  fSigmat=0.050;  //Time Resolution is 50 ps
  fVerbose=1;
  fCollectionEff=0.65;//Collection Efficiency 
}
//-------------Smear Time------------------------------------
void PndDrcHitProducerReal::Smear(Double_t& time, Double_t sigt){
   Double_t t=time;
   Double_t dt;
   dt=gRandom->Gaus(0,sigt);
   t +=dt;
   time=t;
   return;
}
// -----   Finish Task   ---------------------------------------------------
void PndDrcHitProducerReal::Finish()
{

  cout << "-I- PndDrcHitProducerReal: Finish" << endl;
 }
// -------------------------------------------------------------------------


ClassImp(PndDrcHitProducerReal)
