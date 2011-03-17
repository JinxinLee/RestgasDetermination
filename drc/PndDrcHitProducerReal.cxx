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
  fHitArray->Delete();
  if ( ! fPDHitArray ) Fatal("Exec", "No Photon HitArray");
  fPDHitArray->Delete();
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
    Int_t s = (fDetectorID /10);// correction DD
    Int_t b =  (fDetectorID % 10);
    
    //cout<<"-I- HitProducerReal: s = "<<s<<", b = "<<b<<endl;

    Double_t pi            =  TMath::Pi();
    Double_t radius        =  fGeo->radius();//radius in the middle of the bar = 50.cm
    Double_t hthick        =  fGeo->barHalfThick();//half thickness of the bars=1.7/2 cm
    Double_t pipehAngle    =  fGeo->PipehAngle(); 
    Double_t bbGap         =  fGeo->BBoxGap();
    Double_t bbnum         =  fGeo->BBoxNum();
    Double_t barnum	   =  fGeo->barNum();
   
    Double_t bbAngle       =  ( 180. - 2.*pipehAngle - bbGap/radius/pi*180.*(bbnum/2.-1.) )/(bbnum/2.);  
    Double_t bbX           =  radius*bbAngle/180.*pi;  
    Double_t phi0          =  (180.-2.*pipehAngle)/bbnum + pipehAngle;
    Double_t dphi          =  (180.-2.*pipehAngle)/bbnum*2.;

    Double_t phi_curr = (90. - phi0 - dphi*(s-1))/180.*pi;    
    if(s > bbnum/2){ phi_curr = (90. - phi0 - dphi*(s-1) - 2.*pipehAngle)/180.*pi; }
    //cout<<"-I- HitProducerReal: phi_curr = "<< phi_curr/pi*180.<<endl;
    Double_t Xs = radius * cos(phi_curr);
    Double_t Ys = radius * sin(phi_curr);
    //cout<<"-I- HitProducerReal: Xs = "<<Xs<<", Ys = "<<Ys<<endl;
    Double_t Xb = bbX/2.*((2.*b-1.)/barnum - 1.)*sin(phi_curr);
    Double_t Yb = bbX/2.*((2.*b-1.)/barnum - 1.)*cos(phi_curr);
    //cout<<"-I- HitProducerReal: ((2.*b-1.)/barnum - 1.)"<< ((2.*b-1.)/barnum - 1.)<<endl;
    //cout<<"-I- HitProducerReal: Xb = "<<Xb<<", Yb = "<<Yb<<endl;       
    Double_t fXHit = Xs+Xb;
    Double_t fYHit = Ys+Yb;
    //Double_t fZHit = 0.;
    Double_t fZHit = fGeo->barBoxZUp() - fGeo->EVlen();
   
    //cout<<"-I- HitProducerReal: bar center = "<< fXHit << ", "<< fYHit <<endl;
 
    //cout << "-I- HitProducerReal: hit phi: "<< acos(fXHit/radius)/pi*180. << endl;
    
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
  Double_t efficiency[800];
  SetPhotonDetEffNew(lambda_min,lambda_max,lambda_step,efficiency);
  //  SetPhotonDetEff(lambda_min,lambda_max,lambda_step,efficiency);
  //  SetPhotonDetEffOld(lambda_min,lambda_max,lambda_step,efficiency);

  
  PndDrcPDPoint* Ppt=NULL;

// Loop over PndDrcPDPoints
   for(Int_t k=0; k<fPDPointArray->GetEntriesFast(); k++) {
       Ppt = (PndDrcPDPoint*)fPDPointArray->At(k);

       Double_t PPx= Ppt->GetPx();
       Double_t PPy= Ppt->GetPy();
       Double_t PPz= Ppt->GetPz();

       Double_t etot = sqrt(PPx*PPx + PPy*PPy +PPz*PPz);// in GeV
       Double_t lambda=197.0*2.0*TMath::Pi()/(etot*1.0E9);//wavelength of photon in nm
       detection=0;
       if(fisDetEff){
         if (lambda >= lambda_min && lambda < lambda_max) {
            Int_t ilambda=(Int_t)((lambda-lambda_min)/lambda_step);
	    Double_t rand = gRandom->Rndm();
	    detection = 0;
            if (efficiency[ilambda]*fCollectionEff*fPackingFraction > rand ) detection = 1;
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

  /** Quantum efficiency taken from old Burle data */

  fLambdaMin = 300.;
  fLambdaMax = 660.;
  fLambdaStep = 10.;

  fEfficiency[0]  = 0.1;  
  fEfficiency[1]  = 0.12; 
  fEfficiency[2]  = 0.13; 
  fEfficiency[3]  = 0.15; 
  fEfficiency[4]  = 0.18; 
  fEfficiency[5]  = 0.2;  
  fEfficiency[6]  = 0.22; 
  fEfficiency[7]  = 0.24; 
  fEfficiency[8]  = 0.26; 
  fEfficiency[9]  = 0.28; 
  fEfficiency[10] = 0.29; 
  fEfficiency[11] = 0.288;
  fEfficiency[12] = 0.286;
  fEfficiency[13] = 0.284;
  fEfficiency[14] = 0.282;
  fEfficiency[15] = 0.28; 
  fEfficiency[16] = 0.26; 
  fEfficiency[17] = 0.24; 
  fEfficiency[18] = 0.2;  
  fEfficiency[19] = 0.17; 
  fEfficiency[20] = 0.15; 
  fEfficiency[21] = 0.13; 
  fEfficiency[22] = 0.1;  
  fEfficiency[23] = 0.09; 
  fEfficiency[24] = 0.07; 
  fEfficiency[25] = 0.06; 
  fEfficiency[26] = 0.05; 
  fEfficiency[27] = 0.04; 
  fEfficiency[28] = 0.03; 
  fEfficiency[29] = 0.02; 
  fEfficiency[30] = 0.015;
  fEfficiency[31] = 0.01; 
  fEfficiency[32] = 0.005;
  fEfficiency[33] = 0.002;
  fEfficiency[34] = 0.001;
  fEfficiency[35] = 0.;
  }                 
  else {            
    cout << "ERROR:    photocathode type not specified" << endl;

   fLambdaMin = 100.;
   fLambdaMax = 100.;
   fLambdaStep = 100.;

   fEfficiency[0] = 0.;

   }

}  


void PndDrcHitProducerReal::SetPhotonDetEffNew(Double_t& fLambdaMin, 
                       Double_t& fLambdaMax, Double_t& fLambdaStep, Double_t fEfficiency[])
{
//  if (fVerbose > 0) cout << "SetPhotoDetParamter called for Photocathode type " << fDetType << endl;

  if (fDetType == 1){

  fLambdaMin = 200.;
  fLambdaMax = 700.;
  fLambdaStep = 1.;

  Float_t credibleLimit=280.;

// quantum efficiency data from Alex Britting, Jan 25, 2011
// unit is percent
// first value is at 200 nm, last at 700 nm
// credible range start around 250nm, >= 280nm to be safe

  fEfficiency[0]=  231.84;
  fEfficiency[1]=  615.36;
  fEfficiency[2]=  657.4;
  fEfficiency[3]=  258.78;
  fEfficiency[4]=  9839.92;
  fEfficiency[5]=  44.67;
  fEfficiency[6]=  67.87;
  fEfficiency[7]=  51.01;
  fEfficiency[8]=  41.49;
  fEfficiency[9]=  5.36;
  fEfficiency[10]= 49.4;
  fEfficiency[11]= 2.13;
  fEfficiency[12]= 35.49;
  fEfficiency[13]= 8.66;
  fEfficiency[14]= 5.03;
  fEfficiency[15]= 7.51;
  fEfficiency[16]= 13.27;
  fEfficiency[17]= 18.71;
  fEfficiency[18]= 3.92;
  fEfficiency[19]= 3.66;
  fEfficiency[20]= 8.2;
  fEfficiency[21]= 0.56;
  fEfficiency[22]= 7.68;
  fEfficiency[23]= 2.87;
  fEfficiency[24]= 10.06;
  fEfficiency[25]= 3.47;
  fEfficiency[26]= 3.39;
  fEfficiency[27]= 6.99;
  fEfficiency[28]= 6.01;
  fEfficiency[29]= 4.92;
  fEfficiency[30]= 6.25;
  fEfficiency[31]= 5.97;
  fEfficiency[32]= 6.92;
  fEfficiency[33]= 8.29;
  fEfficiency[34]= 10.45;
  fEfficiency[35]= 8.68;
  fEfficiency[36]= 8.6;
  fEfficiency[37]= 9.79;
  fEfficiency[38]= 11.76;
  fEfficiency[39]= 9.53;
  fEfficiency[40]= 10.98;
  fEfficiency[41]= 9.9;
  fEfficiency[42]= 10.97;
  fEfficiency[43]= 11.31;
  fEfficiency[44]= 10.88;
  fEfficiency[45]= 10.78;
  fEfficiency[46]= 12.16;
  fEfficiency[47]= 12.38;
  fEfficiency[48]= 12.37;
  fEfficiency[49]= 13.04;
  fEfficiency[50]= 12.36;
  fEfficiency[51]= 13.18;
  fEfficiency[52]= 13.7;
  fEfficiency[53]= 13.85;
  fEfficiency[54]= 13.66;
  fEfficiency[55]= 13.98;
  fEfficiency[56]= 14.55;
  fEfficiency[57]= 14.93;
  fEfficiency[58]= 14.82;
  fEfficiency[59]= 14.97;
  fEfficiency[60]= 14.98;
  fEfficiency[61]= 15.14;
  fEfficiency[62]= 15.35;
  fEfficiency[63]= 15.37;
  fEfficiency[64]= 15.43;
  fEfficiency[65]= 15.49;
  fEfficiency[66]= 15.59;
  fEfficiency[67]= 15.84;
  fEfficiency[68]= 15.84;
  fEfficiency[69]= 15.92;
  fEfficiency[70]= 16.01;
  fEfficiency[71]= 16.22;
  fEfficiency[72]= 16.41;
  fEfficiency[73]= 16.42;
  fEfficiency[74]= 16.52;
  fEfficiency[75]= 16.86;
  fEfficiency[76]= 17.1;
  fEfficiency[77]= 17.17;
  fEfficiency[78]= 17.22;
  fEfficiency[79]= 17.46;
  fEfficiency[80]= 17.79;
  fEfficiency[81]= 17.99;
  fEfficiency[82]= 18.13;
  fEfficiency[83]= 18.33;
  fEfficiency[84]= 18.34;
  fEfficiency[85]= 18.53;
  fEfficiency[86]= 18.72;
  fEfficiency[87]= 18.95;
  fEfficiency[88]= 19.02;
  fEfficiency[89]= 19.15;
  fEfficiency[90]= 19.28;
  fEfficiency[91]= 19.45;
  fEfficiency[92]= 19.66;
  fEfficiency[93]= 19.69;
  fEfficiency[94]= 19.77;
  fEfficiency[95]= 19.73;
  fEfficiency[96]= 19.95;
  fEfficiency[97]= 19.98;
  fEfficiency[98]= 20.17;
  fEfficiency[99]= 20.29;
  fEfficiency[100]=20.33;
  fEfficiency[101]=20.37;
  fEfficiency[102]=20.47;
  fEfficiency[103]=20.48;
  fEfficiency[104]=20.57;
  fEfficiency[105]=20.75;
  fEfficiency[106]=20.8;
  fEfficiency[107]=20.84;
  fEfficiency[108]=20.86;
  fEfficiency[109]=20.88;
  fEfficiency[110]=21.0;
  fEfficiency[111]=21.06;
  fEfficiency[112]=21.0;
  fEfficiency[113]=21.06;
  fEfficiency[114]=21.06;
  fEfficiency[115]=21.04;
  fEfficiency[116]=21.1;
  fEfficiency[117]=21.14;
  fEfficiency[118]=21.08;
  fEfficiency[119]=21.17;
  fEfficiency[120]=21.3;
  fEfficiency[121]=21.38;
  fEfficiency[122]=21.49;
  fEfficiency[123]=21.58;
  fEfficiency[124]=21.69;
  fEfficiency[125]=21.77;
  fEfficiency[126]=21.87;
  fEfficiency[127]=22.02;
  fEfficiency[128]=22.13;
  fEfficiency[129]=22.29;
  fEfficiency[130]=22.35;
  fEfficiency[131]=22.45;
  fEfficiency[132]=22.53;
  fEfficiency[133]=22.55;
  fEfficiency[134]=22.64;
  fEfficiency[135]=22.67;
  fEfficiency[136]=22.73;
  fEfficiency[137]=22.74;
  fEfficiency[138]=22.71;
  fEfficiency[139]=22.79;
  fEfficiency[140]=22.76;
  fEfficiency[141]=22.77;
  fEfficiency[142]=22.76;
  fEfficiency[143]=22.75;
  fEfficiency[144]=22.78;
  fEfficiency[145]=22.7;
  fEfficiency[146]=22.68;
  fEfficiency[147]=22.72;
  fEfficiency[148]=22.66;
  fEfficiency[149]=22.64;
  fEfficiency[150]=22.7;
  fEfficiency[151]=22.67;
  fEfficiency[152]=22.71;
  fEfficiency[153]=22.67;
  fEfficiency[154]=22.75;
  fEfficiency[155]=22.77;
  fEfficiency[156]=22.83;
  fEfficiency[157]=22.84;
  fEfficiency[158]=22.93;
  fEfficiency[159]=22.97;
  fEfficiency[160]=23.0;
  fEfficiency[161]=23.08;
  fEfficiency[162]=23.16;
  fEfficiency[163]=23.27;
  fEfficiency[164]=23.25;
  fEfficiency[165]=23.37;
  fEfficiency[166]=23.44;
  fEfficiency[167]=23.49;
  fEfficiency[168]=23.55;
  fEfficiency[169]=23.52;
  fEfficiency[170]=23.58;
  fEfficiency[171]=23.64;
  fEfficiency[172]=23.63;
  fEfficiency[173]=23.58;
  fEfficiency[174]=23.64;
  fEfficiency[175]=23.63;
  fEfficiency[176]=23.62;
  fEfficiency[177]=23.64;
  fEfficiency[178]=23.63;
  fEfficiency[179]=23.66;
  fEfficiency[180]=23.59;
  fEfficiency[181]=23.59;
  fEfficiency[182]=23.56;
  fEfficiency[183]=23.58;
  fEfficiency[184]=23.63;
  fEfficiency[185]=23.57;
  fEfficiency[186]=23.66;
  fEfficiency[187]=23.62;
  fEfficiency[188]=23.67;
  fEfficiency[189]=23.64;
  fEfficiency[190]=23.54;
  fEfficiency[191]=23.57;
  fEfficiency[192]=23.51;
  fEfficiency[193]=23.53;
  fEfficiency[194]=23.45;
  fEfficiency[195]=23.3;
  fEfficiency[196]=23.41;
  fEfficiency[197]=23.25;
  fEfficiency[198]=23.21;
  fEfficiency[199]=23.08;
  fEfficiency[200]=23.01;
  fEfficiency[201]=22.92;
  fEfficiency[202]=22.9;
  fEfficiency[203]=22.76;
  fEfficiency[204]=22.76;
  fEfficiency[205]=22.61;
  fEfficiency[206]=22.53;
  fEfficiency[207]=22.48;
  fEfficiency[208]=22.39;
  fEfficiency[209]=22.29;
  fEfficiency[210]=22.24;
  fEfficiency[211]=22.2;
  fEfficiency[212]=22.12;
  fEfficiency[213]=22.07;
  fEfficiency[214]=21.96;
  fEfficiency[215]=21.89;
  fEfficiency[216]=21.87;
  fEfficiency[217]=21.76;
  fEfficiency[218]=21.74;
  fEfficiency[219]=21.58;
  fEfficiency[220]=21.49;
  fEfficiency[221]=21.48;
  fEfficiency[222]=21.37;
  fEfficiency[223]=21.29;
  fEfficiency[224]=21.2;
  fEfficiency[225]=21.17;
  fEfficiency[226]=21.03;
  fEfficiency[227]=20.98;
  fEfficiency[228]=20.92;
  fEfficiency[229]=20.85;
  fEfficiency[230]=20.76;
  fEfficiency[231]=20.69;
  fEfficiency[232]=20.58;
  fEfficiency[233]=20.56;
  fEfficiency[234]=20.47;
  fEfficiency[235]=20.37;
  fEfficiency[236]=20.32;
  fEfficiency[237]=20.24;
  fEfficiency[238]=20.13;
  fEfficiency[239]=20.08;
  fEfficiency[240]=19.9;
  fEfficiency[241]=19.84;
  fEfficiency[242]=19.77;
  fEfficiency[243]=19.69;
  fEfficiency[244]=19.63;
  fEfficiency[245]=19.51;
  fEfficiency[246]=19.41;
  fEfficiency[247]=19.27;
  fEfficiency[248]=19.06;
  fEfficiency[249]=19.01;
  fEfficiency[250]=18.87;
  fEfficiency[251]=18.7;
  fEfficiency[252]=18.49;
  fEfficiency[253]=18.41;
  fEfficiency[254]=18.17;
  fEfficiency[255]=17.98;
  fEfficiency[256]=17.84;
  fEfficiency[257]=17.69;
  fEfficiency[258]=17.5;
  fEfficiency[259]=17.25;
  fEfficiency[260]=17.15;
  fEfficiency[261]=16.98;
  fEfficiency[262]=16.79;
  fEfficiency[263]=16.66;
  fEfficiency[264]=16.48;
  fEfficiency[265]=16.32;
  fEfficiency[266]=16.19;
  fEfficiency[267]=16.02;
  fEfficiency[268]=15.88;
  fEfficiency[269]=15.77;
  fEfficiency[270]=15.67;
  fEfficiency[271]=15.5;
  fEfficiency[272]=15.39;
  fEfficiency[273]=15.23;
  fEfficiency[274]=15.09;
  fEfficiency[275]=15.04;
  fEfficiency[276]=14.92;
  fEfficiency[277]=14.75;
  fEfficiency[278]=14.7;
  fEfficiency[279]=14.5;
  fEfficiency[280]=14.45;
  fEfficiency[281]=14.34;
  fEfficiency[282]=14.25;
  fEfficiency[283]=14.16;
  fEfficiency[284]=14.13;
  fEfficiency[285]=14.0;
  fEfficiency[286]=13.92;
  fEfficiency[287]=13.84;
  fEfficiency[288]=13.76;
  fEfficiency[289]=13.73;
  fEfficiency[290]=13.61;
  fEfficiency[291]=13.54;
  fEfficiency[292]=13.52;
  fEfficiency[293]=13.45;
  fEfficiency[294]=13.41;
  fEfficiency[295]=13.39;
  fEfficiency[296]=13.31;
  fEfficiency[297]=13.22;
  fEfficiency[298]=13.17;
  fEfficiency[299]=13.13;
  fEfficiency[300]=13.06;
  fEfficiency[301]=13.2;
  fEfficiency[302]=13.09;
  fEfficiency[303]=12.97;
  fEfficiency[304]=12.92;
  fEfficiency[305]=12.73;
  fEfficiency[306]=12.65;
  fEfficiency[307]=12.4;
  fEfficiency[308]=12.22;
  fEfficiency[309]=12.02;
  fEfficiency[310]=11.79;
  fEfficiency[311]=11.59;
  fEfficiency[312]=11.33;
  fEfficiency[313]=11.03;
  fEfficiency[314]=10.68;
  fEfficiency[315]=10.46;
  fEfficiency[316]=10.14;
  fEfficiency[317]=9.88;
  fEfficiency[318]=9.62;
  fEfficiency[319]=9.36;
  fEfficiency[320]=9.14;
  fEfficiency[321]=8.87;
  fEfficiency[322]=8.63;
  fEfficiency[323]=8.51;
  fEfficiency[324]=8.24;
  fEfficiency[325]=8.07;
  fEfficiency[326]=7.88;
  fEfficiency[327]=7.77;
  fEfficiency[328]=7.65;
  fEfficiency[329]=7.52;
  fEfficiency[330]=7.35;
  fEfficiency[331]=7.27;
  fEfficiency[332]=7.21;
  fEfficiency[333]=7.1;
  fEfficiency[334]=6.92;
  fEfficiency[335]=6.89;
  fEfficiency[336]=6.79;
  fEfficiency[337]=6.74;
  fEfficiency[338]=6.56;
  fEfficiency[339]=6.54;
  fEfficiency[340]=6.5;
  fEfficiency[341]=6.39;
  fEfficiency[342]=6.33;
  fEfficiency[343]=6.25;
  fEfficiency[344]=6.27;
  fEfficiency[345]=6.14;
  fEfficiency[346]=6.06;
  fEfficiency[347]=6.04;
  fEfficiency[348]=6.01;
  fEfficiency[349]=5.91;
  fEfficiency[350]=5.89;
  fEfficiency[351]=5.79;
  fEfficiency[352]=5.75;
  fEfficiency[353]=5.75;
  fEfficiency[354]=5.67;
  fEfficiency[355]=5.61;
  fEfficiency[356]=5.51;
  fEfficiency[357]=5.52;
  fEfficiency[358]=5.43;
  fEfficiency[359]=5.43;
  fEfficiency[360]=5.34;
  fEfficiency[361]=5.31;
  fEfficiency[362]=5.35;
  fEfficiency[363]=5.23;
  fEfficiency[364]=5.2;
  fEfficiency[365]=5.14;
  fEfficiency[366]=5.11;
  fEfficiency[367]=5.11;
  fEfficiency[368]=5.01;
  fEfficiency[369]=4.98;
  fEfficiency[370]=4.93;
  fEfficiency[371]=4.99;
  fEfficiency[372]=4.89;
  fEfficiency[373]=4.82;
  fEfficiency[374]=4.87;
  fEfficiency[375]=4.8;
  fEfficiency[376]=4.7;
  fEfficiency[377]=4.65;
  fEfficiency[378]=4.65;
  fEfficiency[379]=4.61;
  fEfficiency[380]=4.49;
  fEfficiency[381]=4.56;
  fEfficiency[382]=4.44;
  fEfficiency[383]=4.42;
  fEfficiency[384]=4.44;
  fEfficiency[385]=4.35;
  fEfficiency[386]=4.35;
  fEfficiency[387]=4.27;
  fEfficiency[388]=4.29;
  fEfficiency[389]=4.19;
  fEfficiency[390]=4.13;
  fEfficiency[391]=4.08;
  fEfficiency[392]=4.02;
  fEfficiency[393]=4.07;
  fEfficiency[394]=3.92;
  fEfficiency[395]=3.95;
  fEfficiency[396]=3.88;
  fEfficiency[397]=3.82;
  fEfficiency[398]=3.86;
  fEfficiency[399]=3.74;
  fEfficiency[400]=3.71;
  fEfficiency[401]=3.66;
  fEfficiency[402]=3.72;
  fEfficiency[403]=3.62;
  fEfficiency[404]=3.55;
  fEfficiency[405]=3.56;
  fEfficiency[406]=3.57;
  fEfficiency[407]=3.45;
  fEfficiency[408]=3.38;
  fEfficiency[409]=3.36;
  fEfficiency[410]=3.36;
  fEfficiency[411]=3.28;
  fEfficiency[412]=3.25;
  fEfficiency[413]=3.19;
  fEfficiency[414]=3.26;
  fEfficiency[415]=3.13;
  fEfficiency[416]=3.17;
  fEfficiency[417]=3.15;
  fEfficiency[418]=3.04;
  fEfficiency[419]=2.98;
  fEfficiency[420]=2.93;
  fEfficiency[421]=2.98;
  fEfficiency[422]=2.9;
  fEfficiency[423]=2.89;
  fEfficiency[424]=2.9;
  fEfficiency[425]=2.81;
  fEfficiency[426]=2.74;
  fEfficiency[427]=2.81;
  fEfficiency[428]=2.68;
  fEfficiency[429]=2.73;
  fEfficiency[430]=2.7;
  fEfficiency[431]=2.57;
  fEfficiency[432]=2.58;
  fEfficiency[433]=2.55;
  fEfficiency[434]=2.55;
  fEfficiency[435]=2.37;
  fEfficiency[436]=2.39;
  fEfficiency[437]=2.39;
  fEfficiency[438]=2.44;
  fEfficiency[439]=2.37;
  fEfficiency[440]=2.26;
  fEfficiency[441]=2.27;
  fEfficiency[442]=2.27;
  fEfficiency[443]=2.23;
  fEfficiency[444]=2.26;
  fEfficiency[445]=2.14;
  fEfficiency[446]=2.08;
  fEfficiency[447]=2.15;
  fEfficiency[448]=2.06;
  fEfficiency[449]=2.09;
  fEfficiency[450]=2.04;
  fEfficiency[451]=2.0;
  fEfficiency[452]=1.95;
  fEfficiency[453]=2.02;
  fEfficiency[454]=1.87;
  fEfficiency[455]=1.9;
  fEfficiency[456]=1.8;
  fEfficiency[457]=1.87;
  fEfficiency[458]=1.85;
  fEfficiency[459]=1.87;
  fEfficiency[460]=1.81;
  fEfficiency[461]=1.86;
  fEfficiency[462]=1.74;
  fEfficiency[463]=1.74;
  fEfficiency[464]=1.63;
  fEfficiency[465]=1.59;
  fEfficiency[466]=1.5;
  fEfficiency[467]=1.5;
  fEfficiency[468]=1.44;
  fEfficiency[469]=1.47;
  fEfficiency[470]=1.32;
  fEfficiency[471]=1.24;
  fEfficiency[472]=1.28;
  fEfficiency[473]=1.19;
  fEfficiency[474]=1.21;
  fEfficiency[475]=1.21;
  fEfficiency[476]=1.1;
  fEfficiency[477]=1.1;
  fEfficiency[478]=1.05;
  fEfficiency[479]=1.06;
  fEfficiency[480]=0.94;
  fEfficiency[481]=0.92;
  fEfficiency[482]=0.87;
  fEfficiency[483]=0.92;
  fEfficiency[484]=0.81;
  fEfficiency[485]=0.86;
  fEfficiency[486]=0.78;
  fEfficiency[487]=0.77;
  fEfficiency[488]=0.8;
  fEfficiency[489]=0.67;
  fEfficiency[490]=0.7;
  fEfficiency[491]=0.81;
  fEfficiency[492]=0.61;
  fEfficiency[493]=0.64;
  fEfficiency[494]=0.71;
  fEfficiency[495]=0.66;
  fEfficiency[496]=0.67;
  fEfficiency[497]=0.68;
  fEfficiency[498]=0.69;
  fEfficiency[499]=0.68;
  fEfficiency[500]=0.73;

  // still need to convert from percent and cut values below credible limit
  for (Int_t iBin=0;iBin<501;iBin++) 
    {
      if (iBin<(Int_t)(credibleLimit-fLambdaMin))
	{
	  fEfficiency[iBin]=0.;
	} 
      else
	{
	  fEfficiency[iBin]=fEfficiency[iBin]/100.;
	}
      //      cout << "efficiency is " << fEfficiency[iBin] << " at " << fLambdaMin+iBin<<endl;
    } 

  }                 
  else {            
    cout << "ERROR:    photocathode type not specified" << endl;

   fLambdaMin = 100.;
   fLambdaMax = 100.;
   fLambdaStep = 100.;

   fEfficiency[0] = 0.;

   }

}  

void PndDrcHitProducerReal::SetPhotonDetEffOld(Double_t& fLambdaMin, 
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
  fVerbose=0;
  fCollectionEff=0.65;//Collection Efficiency 
  fPackingFraction=0.80;//Packing Efficiency 
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
