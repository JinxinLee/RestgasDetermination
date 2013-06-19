// -------------------------------------------------------------------------
// -----                   PndDrcDigiTask source file                  -----
// -----               Created 15/04/13  by M.Patsyuk                  -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <map>
#include "stdio.h"

#include "TGeoManager.h"

#include "FairRootManager.h"
#include "PndDrcDigiTask.h"
#include "PndDrcPDPoint.h"

#include "PndMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoVector.h"
#include "FairGeoNode.h"
#include "FairGeoVolume.h"

#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoBBox.h"

#include "FairBaseParSet.h"
#include "FairGeoTransform.h"

#include "PndGeoDrcPar.h"

using std::endl;
using std::cout;

// -----   Default constructor   -------------------------------------------
PndDrcDigiTask::PndDrcDigiTask() 
:FairTask("PndDrcDigiTask")
{
  fGeo = new PndGeoDrc();
  fGeoH = NULL;  
  SetParameters();
  fisDetEff= kTRUE; 
  fisPixel= kTRUE;
  
  fPDPointArray   = NULL;
  fDrcDigiArray   = NULL;
  
  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor with verbosity level  -------------------------------------------

PndDrcDigiTask::PndDrcDigiTask(Int_t verbose, Int_t det_type) 
  :FairTask("PndDrcDigiTask")
{
  fVerbose = verbose;  
  fDetType= det_type; 
  fisDetEff= kTRUE; 
  fisPixel= kTRUE; 
  fGeo = new PndGeoDrc();
  fGeoH = NULL;  
  SetParameters();
  
  fPDPointArray   = NULL;
  fDrcDigiArray   = NULL;
  
  Reset();
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndDrcDigiTask::~PndDrcDigiTask()
{
  Reset();
  if (fGeo) delete fGeo;
  if ( fDrcDigiArray ) {
    fDrcDigiArray->Delete();
    delete fDrcDigiArray;
  }
  
}

// -----   Initialization   -----------------------------------------------
// -------------------------------------------------------------------------
InitStatus PndDrcDigiTask::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
 
 // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDrcDigiTask::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // PndGeoHandling
  if ( fGeoH == NULL )
    fGeoH = PndGeoHandling::Instance();

  fGeoH->SetParContainers();
  if(fVerbose>1) Info("SetParContainers","done.");
  
  // Get input array
  fMCArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCArray ) {
    cout << "-W- PndDrcRecoLookupMap::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }

  // Get input array
  fBarPointArray = (TClonesArray*) ioman->GetObject("DrcBarPoint");
  if ( ! fBarPointArray ) {
   cout << "-W- PndDrcDigiTask::Init: "
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
  fDrcDigiArray = new TClonesArray("PndDrcDigi");
  ioman->Register("DrcDigi","Drc",fDrcDigiArray, kTRUE);
  
  detEffLam = new TH2F("detEffLam","det. eff. as a function of lambda", 900, 100.,1000., 1000,0.,1. );

  //set efficiencies
  SetPhotonDetEffNew(); 
  SetPhotonTransportEff();

  cout << "-I- PndDrcDigiTask: Intialization successfull" << endl;  	  
  
  return kSUCCESS;   
}
// -----   Execution of Task   ---------------------------------------------
void PndDrcDigiTask::Exec(Option_t* option)
{
  if ( ! fDrcDigiArray ) Fatal("Exec", "No Photon DigiArray");
  Reset();
  fDrcDigiArray->Delete();
  nevents++;
  if (fVerbose > 0) if(nevents%10==0) cout<<"Event No. "<<nevents <<endl;
  
  ProcessPhotonPoint();  
}

//-------- Photon Detector Hit production with efficiency-------------   
void PndDrcDigiTask::ProcessPhotonPoint()
{  
  // Reset output array
  fGeoH->SetVerbose(fVerbose);
  
  if (fVerbose > 1) {
    cout <<" Number of Photon MC Points in Photon Detector Plane : "<<fPDPointArray->GetEntries()<<endl;
  }
  
  PndDrcPDPoint* Ppt=NULL;
  PndMCTrack* tr = NULL;
  
  //Loop over PndDrcPDPoints
  for(Int_t k=0; k < fPDPointArray->GetEntriesFast(); k++) {
  
    Ppt = (PndDrcPDPoint*)fPDPointArray->At(k);
       
    Int_t trID= Ppt->GetTrackID();
    tr = (PndMCTrack*)fMCArray->At(trID);
       
    // start vertex of the photon
    TVector3 StartVertex = tr->GetStartVertex();
    
    // initial momentum of the photon      
    TVector3 PphoInit;
    PphoInit.SetXYZ(tr->GetMomentum().X(), tr->GetMomentum().Y(), tr->GetMomentum().Z());
      
    //calculate the number of bounces:
    Int_t NbouncesX, NbouncesY;
    Double_t angleX, angleY;
    NumberOfBounces(StartVertex, PphoInit, &NbouncesX, &NbouncesY, &angleX, &angleY);
      
    Double_t PPx= Ppt->GetPx();
    Double_t PPy= Ppt->GetPy();
    Double_t PPz= Ppt->GetPz();

    Double_t etot = sqrt(PPx*PPx + PPy*PPy +PPz*PPz);// in GeV
    Double_t lambda=197.0*2.0*fpi/(etot*1.0E9);//wavelength of photon in nm
       
    if(fisDetEff){
      if (lambda >= flambda_min && lambda < flambda_max) {
        Int_t ilambda=(Int_t)((lambda-flambda_min)/flambda_step);
	    Double_t rand = gRandom->Rndm();
	    fDetection = 0;
        if (fDetEfficiency[ilambda]*fCollectionEff*fPackingFraction > rand ) fDetection = 1;
	    detEffLam->Fill(lambda, fDetEfficiency[ilambda]*fCollectionEff);
	  }	 
    }
    if(!fisDetEff){ 
      fDetection=1;
    }

    //####################################
    // transport efficiency
    // assume that detection efficiency above CAN NOT be used together with transport efficiency
    // Maria Patsyuk
    if(fisTransportEff){
      if (lambda >= flambda_min_tr && lambda < flambda_max_tr) {
	    Int_t ilambda=(Int_t)((lambda-flambda_min_tr)/flambda_step_tr); 
	    Int_t iangleX =(Int_t)(angleX/fangle_step_tr);	    
	    Int_t iangleY =(Int_t)(angleY/fangle_step_tr);

	    Double_t rand = gRandom->Rndm();
	    fDetection = 0;
	    Double_t TotalTrProb = 1.;
	    Double_t ReflectionProbX = fTranspEfficiency[ilambda+ iangleX*flambda_points_tr];
	    Double_t ReflectionProbY = fTranspEfficiency[ilambda+ iangleY*flambda_points_tr];
	    TotalTrProb = pow(ReflectionProbX, (Int_t)NbouncesX)*pow(ReflectionProbY, (Int_t)NbouncesY);
	    if(TotalTrProb > rand) fDetection = 1;
       }
     }
     if(!fisTransportEff && !fisDetEff){ 
       fDetection=1;
     }      

     if(fDetection==1){
            
     	// transform to local sensor system... (mc point has the ID not the path to the volume)
     	TVector3 PptPosition;
	Ppt->Position(PptPosition);
     	TVector3 posL = fGeoH->MasterToLocalShortId(PptPosition,Ppt->GetDetectorID());
	 
    	TVector3 sensorDim = GetSensorDimensions(Ppt->GetDetectorID());	
 
     	if (fVerbose > 1){
	      std::cout << "SensorDimension for: " << Ppt->GetDetectorID() << std::endl;
	      std::cout << sensorDim.X() << " " << sensorDim.Y() << " " << sensorDim.Z() << std::endl;
	}
	 
	//usually sensors have origin in the middle, let's move it to the left lower corner:
     	TVector3 posLshifted;
     	posLshifted.SetXYZ(posL.X()+sensorDim.X(), posL.Y()+sensorDim.Y(), posL.Z()+sensorDim.Z());
	 
     	// calculate the number of the fired pixel
     	Int_t Ncol = (Int_t)TMath::Floor(posLshifted.X()/fPixelStep);
     	Int_t Nrow = (Int_t)TMath::Floor(posLshifted.Y()/fPixelStep);
     	Int_t NpixelLocal = Ncol + Nrow  * fNpix;
     
     	fDetectorID = Ppt->GetDetectorID() * 100 + NpixelLocal;
	 
	// time is smeared and digitized = has granularity
	fTime=(Int_t)(Ppt->GetTime()/fTimeGranularity)*fTimeGranularity; 
     	Smear(fTime,fSigmat);
     
	ActivatePixel(fDetectorID, fTime, k, 0);

        if(fChargeSharing == kTRUE){
	  // find fired pixels:
	  Double_t distance = 999.;
	  TVector3 corner;
	  TVector3 point;
		
	  // left pixel
	  distance =  posLshifted.X() - TMath::Floor(posLshifted.X()/fPixelStep)*fPixelStep; //[cm]
	  if(exp(- distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){   
	    if((Ncol-1) >= 0){
	      ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol-1+Nrow*fNpix, fTime, k, 1);	
	    }
	  }
	  // right pixel
	  distance =  TMath::Ceil(posLshifted.X()/fPixelStep)*fPixelStep - posLshifted.X();	  
	  if(exp(- distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){    
	    if(Ncol+1 < fNpix){
	      ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+1+Nrow*fNpix, fTime, k, 1);	
	    }
	  }
	  // lower pixel
	  distance =  posLshifted.Y() - TMath::Floor(posLshifted.Y()/fPixelStep)*fPixelStep;	  
	  if(exp(- distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){    
	    if(Nrow-1 >= 0){
	      ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+(Nrow-1)*fNpix, fTime, k, 1);
	    }
	  }
	  // upper pixel
	  distance =  TMath::Ceil(posLshifted.Y()/fPixelStep)*fPixelStep - posLshifted.Y();
	  if(exp(- distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){    
	    if(Nrow+1 < fNpix){
	      ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+(Nrow+1)*fNpix, fTime, k, 1);
	    }
	  }
		
	  point.SetXYZ(posLshifted.X(),posLshifted.Y(),0.);
		
	  // upper left pixel
	  corner.SetXYZ(TMath::Floor(posLshifted.X()/fPixelStep)*fPixelStep,
	  		TMath::Ceil(posLshifted.Y()/fPixelStep)*fPixelStep,0.);
	  distance = (point-corner).Mag();	  
	  if(exp(-distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	    
	    if(Ncol-1 >= 0 && Nrow+1 < fNpix){
	      ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol-1+(Nrow+1)*fNpix, fTime, k, 1);
				
	    }
	  }
		
	  // bottom left pixel
	  corner.SetXYZ(TMath::Floor(posLshifted.X()/fPixelStep)*fPixelStep,
	  		TMath::Floor(posLshifted.Y()/fPixelStep)*fPixelStep,0.);
	  distance = (point-corner).Mag();	 
	  if(exp(-distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	    
	    if(Ncol-1 >= 0 && Nrow-1 >= 0){
	      ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol-1+(Nrow-1)*fNpix, fTime, k, 1);
	    }
	  }
		
	  // bottom right pixel
	  corner.SetXYZ(TMath::Ceil(posLshifted.X()/fPixelStep)*fPixelStep,
	  		TMath::Floor(posLshifted.Y()/fPixelStep)*fPixelStep,0.);
	  distance = (point-corner).Mag();	  
	  if(exp(-distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	    
	    if(Ncol+1 < fNpix && Nrow-1 >= 0){
	      ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+1+(Nrow-1)*fNpix, fTime, k, 1);
	    }
	  }
		
	  // upper right pixel
	  corner.SetXYZ(TMath::Ceil(posLshifted.X()/fPixelStep)*fPixelStep,
	  		TMath::Ceil(posLshifted.Y()/fPixelStep)*fPixelStep,0.);
	  distance = (point-corner).Mag();	  
	  if(exp(-distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	    
	    if(Ncol+1 < fNpix && Nrow+1 < fNpix){
	      ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+1+(Nrow+1)*fNpix, fTime, k, 1);
	    }
	  }		
     	} // if charge sharing true          	
     } // if detection
  }
}

// -----   Private method ActivatePixel   -------------------------------
void PndDrcDigiTask::ActivatePixel(Int_t sensorDetId, Double_t signalTime, Int_t k, Int_t csflag) {
// in case when the same pixel was fired by two different photons this function takes care of which hits from that pixel to write  

  if ( fPixelMap.find(sensorDetId) == fPixelMap.end() ) {
    // pixel not yet active, create new digi

    new ((*fDrcDigiArray)[fNDigis]) PndDrcDigi(k, sensorDetId, 0., signalTime, csflag, 0.);
    fPixelMap[sensorDetId] = fNDigis;
    fNDigis++;
  }
  else {    
    // if there was already hit in this pixel...

    Int_t iDigi = fPixelMap[sensorDetId];
    PndDrcDigi* ddigi = dynamic_cast<PndDrcDigi*>(fDrcDigiArray->At(iDigi));
    // ... check the time difference with another one in the same pixel

    if(fabs(ddigi->GetTimeStamp() - signalTime) > fDeadTime){
      new ((*fDrcDigiArray)[fNDigis]) PndDrcDigi(k, sensorDetId, 0., signalTime, csflag, 0.);
      fPixelMap[sensorDetId] = fNDigis;
      fNDigis++; 
    }
    //ddigi->AddCharge(signalHeight);
    //ddigi->AddIndex(iPoint);
  }
}
// -------------------------------------------------------------------------


//------   Find Nubmer of Bounces     --------------------------------------
void PndDrcDigiTask::NumberOfBounces(TVector3 start, TVector3 dir, Int_t *n1, Int_t *n2, Double_t *alpha1, Double_t *alpha2){
    // calculates the number of bounces in x and y direction and reflection angles in these directions.
        
    Double_t PhiRot = FindPhiRot(start.X(), start.Y());

    // Photon production point in bar' coordinate system (origin at the corner of the bar):   
    TVector3 startBar;
    startBar.SetXYZ(start.X(), start.Y(), start.Z());
    startBar.RotateZ(-PhiRot/180.*fpi);
    
    // Photon momentum in bar' coord system:
    TVector3 PphoB;
    PphoB = dir.Unit();
    PphoB.RotateZ(-PhiRot/180.*fpi);
    
    // Find coordinates of X0, Y0:
    Double_t Z0, X0, Y0;
    if(dir.Theta() < 3.1415/2.){
      Z0 = -(fabs(fzup) + 2.*fzdown - startBar.Z());
    }
    if(dir.Theta() >= 3.1415/2.){
      Z0 = -(startBar.Z() -  fzup);
    }
    
    X0 = Z0*tan(PphoB.Theta())*cos(PphoB.Phi());
    Y0 = Z0*tan(PphoB.Theta())*sin(PphoB.Phi());
    
    // Find the number of bounces in each direction       
    Double_t N1, N2;
    //frad_out = (fradius-fhthick)/cos(2.*3.1415/16./2.); // radius at corner - thickness ###
    //flside   = 2.*frad_out*sin(2.*3.1415/16./2.) - (2.*fboxthick) - (2.*fboxgap);
    //flside = (180. - 2.*fpipehAngle - fbbGap/fradius*(fbbnum/2. - 1.)/fpi*180.)/(fbbnum/2.) * fradius/ 180.*fpi;
    //fbarwidth = flside/fbarnum;
        
    // Find which bar in the bar box was hit:
    if(fbarnum > 1){    
      Int_t NhitBar = (Int_t)((0.5*flside + startBar.Y())/fbarwidth)+1;
      FindOutPoint(X0, startBar.X()-(fradius-fhthick), 		   2.*fhthick, &N1, 0);
      FindOutPoint(Y0, startBar.Y()+0.5*flside-(NhitBar-1)*fbarwidth, fbarwidth, &N2, 0);
    }
    
    if(fbarnum == 1 && flside > fbarwidth){
      FindOutPoint(X0, startBar.X()-(fradius-fhthick), 		   2.*fhthick, &N1, 0);
      FindOutPoint(Y0, startBar.Y()+0.5*fbarwidth, fbarwidth, &N2, 0);
    }
    
    *n1 = (Int_t)N1;
    *n2 = (Int_t)N2;
    
    // calculate the reflection angles in x and y directions:
    TVector3 up_down;
    up_down.SetXYZ(0.,1.,0.);
    TVector3 left_right;
    left_right.SetXYZ(1.,0.,0.);
    Double_t angle1 = PphoB.Angle(left_right);
    if(angle1 > fpi/2.){angle1 = fpi - PphoB.Angle(left_right);}
    Double_t angle2 = PphoB.Angle(up_down);
    if(angle2 > fpi/2.){angle2 = fpi - PphoB.Angle(up_down);}
    *alpha1 = angle1;
    *alpha2 = angle2;
}

//----------------------------------------------------------------------------------------------
Double_t PndDrcDigiTask::FindPhiRot(Double_t xx, Double_t yy){ // returns [degrees]

    TVector3 hit;
    hit.SetXYZ(xx,yy,0.);
    Double_t startPhi = hit.Phi()/fpi*180.; // [degrees]
    if(startPhi < 0.){startPhi = 360. + hit.Phi()*180./fpi;}

    Double_t PhiRot = 0.; //[degrees]
    if(startPhi >= 0. && startPhi < 90.){
      PhiRot = TMath::Floor(startPhi/fdphi) *fdphi + fdphi/2.;
    }
    if(startPhi >= 90. && startPhi < 270.){
      PhiRot = 90. + fpipehAngle + TMath::Floor((startPhi-90.-fpipehAngle)/fdphi) *fdphi + fdphi/2.;
    } 
    if(startPhi >= 270. && startPhi < 360.){
      PhiRot = 270. + fpipehAngle + TMath::Floor((startPhi-270.-fpipehAngle)/fdphi) *fdphi + fdphi/2.;
    }
    //cout<<"-I- FindPhiRot: PhiRot = "<<PhiRot<<endl;       
    return PhiRot; // degrees
}

//----------------------------------------------------------------------------------------------------------
Double_t PndDrcDigiTask::FindOutPoint(Double_t x0, Double_t xEn, Double_t a, Double_t *NN, Bool_t print){	
	Double_t m=99.;
	Double_t n=TMath::Floor(x0/a);
	m = n;		
	if(print){std::cout<<"n = "<<n<<", NN = "<<*NN<<", x0 = "<<x0<<", a = "<<a<<std::endl;}
	Double_t x1 = x0 - n*a;
	if(x0 < 0.){x1 = x0 - (n+1)*a;}
	if(print){std::cout<<"xy = "<< x1<<std::endl;}
	Double_t xK = 0.;
	if((m/2. - TMath::Floor(m/2.)) == 0.) { // 4etnoe
		if(print){std::cout<<"odd==0"<<std::endl;}
		if(x0 >= 0. && x1 + xEn <= a){xK = x1 + xEn;} 
		if(x0 >= 0. && x1 + xEn >  a){xK = 2*a - x1 - xEn; n = 1. + n;}
		if(x0 < 0. && x1 + xEn >= 0.){xK = a - (x1 + xEn); n = -1. -n;}
		if(x0 < 0. && x1 + xEn < 0.) {xK = a + x1 + xEn; n = -n;}
	if(print){std::cout<<"xK = "<< xK<<", n = "<<n<<std::endl;}
		
	}

	if((m/2. - TMath::Floor(m/2.)) != 0.) { // ne4etnoe
		if(print){std::cout<<"even!=0"<<std::endl;} 
		if(x0 >= 0. && x1 + xEn <= a){xK = a - (x1 + xEn);} 
		if(x0 >= 0. && x1 + xEn >  a){xK = x1 + xEn - a; n = 1. + n;}
		if(x0 < 0. && x1 + xEn >= 0.){xK = x1 + xEn; n = -1. -n;} 
		if(x0 < 0. && x1 + xEn < 0.) {xK = - (x1 + xEn); n = -n;}
	if(print){std::cout<<"xK = "<< xK<<", n = "<<n<<std::endl;}

	}
 	
	*NN = n;		
	return xK;
}


// -----   Add Photon Detector Hit to HitCollection   --------------------------------------
PndDrcDigi* PndDrcDigiTask::AddDrcDigi(Int_t index, Int_t iDetectorId, Double_t charge, Double_t time, Int_t csflag)
{
  TClonesArray& clrefPD = *fDrcDigiArray;
  Int_t size = clrefPD.GetEntriesFast();
  return new(clrefPD[size]) PndDrcDigi(index,  iDetectorId, charge, time, csflag, 0.);
}

//-------------------------------------------------------------------------------
void PndDrcDigiTask::SetFakeDetEff()
{
  if (fDetType == 1){

    /** Quantum efficiency taken from old Burle data */
    flambda_min = 300.;
    flambda_max = 700.;
    flambda_step = 400.;

    fDetEfficiency[0]  = 1.;  
    fDetEfficiency[1]  = 1.;   
  }                 
  else {            
    cout << "ERROR:    photocathode type not specified" << endl;

    flambda_min = 300.;
    flambda_max = 700.;
    flambda_step = 400.;

    fDetEfficiency[0] = 0.;
  }
}
  
//--------------------------------------------------------------------------
void PndDrcDigiTask::SetPhotonDetEffNew()
{
  if (fDetType == 1){

    flambda_min = 200.;
    flambda_max = 700.;
    flambda_step = 1.;

    Float_t credibleLimit=280.;

    // quantum efficiency data from Alex Britting, Jan 25, 2011
    // unit is percent
    // first value is at 200 nm, last at 700 nm
    // credible range start around 250nm, >= 280nm to be safe

    fDetEfficiency[0]=  231.84;
    fDetEfficiency[1]=  615.36;
    fDetEfficiency[2]=  657.4;
    fDetEfficiency[3]=  258.78;
    fDetEfficiency[4]=  9839.92;
    fDetEfficiency[5]=  44.67;
    fDetEfficiency[6]=  67.87;
    fDetEfficiency[7]=  51.01;
    fDetEfficiency[8]=  41.49;
    fDetEfficiency[9]=  5.36;
    fDetEfficiency[10]= 49.4;
    fDetEfficiency[11]= 2.13;
    fDetEfficiency[12]= 35.49;
    fDetEfficiency[13]= 8.66;
    fDetEfficiency[14]= 5.03;
    fDetEfficiency[15]= 7.51;
    fDetEfficiency[16]= 13.27;
    fDetEfficiency[17]= 18.71;
    fDetEfficiency[18]= 3.92;
    fDetEfficiency[19]= 3.66;
    fDetEfficiency[20]= 8.2;
    fDetEfficiency[21]= 0.56;
    fDetEfficiency[22]= 7.68;
    fDetEfficiency[23]= 2.87;
    fDetEfficiency[24]= 10.06;
    fDetEfficiency[25]= 3.47;
    fDetEfficiency[26]= 3.39;
    fDetEfficiency[27]= 6.99;
    fDetEfficiency[28]= 6.01;
    fDetEfficiency[29]= 4.92;
    fDetEfficiency[30]= 6.25;
    fDetEfficiency[31]= 5.97;
    fDetEfficiency[32]= 6.92;
    fDetEfficiency[33]= 8.29;
    fDetEfficiency[34]= 10.45;
    fDetEfficiency[35]= 8.68;
    fDetEfficiency[36]= 8.6;
    fDetEfficiency[37]= 9.79;
    fDetEfficiency[38]= 11.76;
    fDetEfficiency[39]= 9.53;
    fDetEfficiency[40]= 10.98;
    fDetEfficiency[41]= 9.9;
    fDetEfficiency[42]= 10.97;
    fDetEfficiency[43]= 11.31;
    fDetEfficiency[44]= 10.88;
    fDetEfficiency[45]= 10.78;
    fDetEfficiency[46]= 12.16;
    fDetEfficiency[47]= 12.38;
    fDetEfficiency[48]= 12.37;
    fDetEfficiency[49]= 13.04;
    fDetEfficiency[50]= 12.36;
    fDetEfficiency[51]= 13.18;
    fDetEfficiency[52]= 13.7;
    fDetEfficiency[53]= 13.85;
    fDetEfficiency[54]= 13.66;
    fDetEfficiency[55]= 13.98;
    fDetEfficiency[56]= 14.55;
    fDetEfficiency[57]= 14.93;
    fDetEfficiency[58]= 14.82;
    fDetEfficiency[59]= 14.97;
    fDetEfficiency[60]= 14.98;
    fDetEfficiency[61]= 15.14;
    fDetEfficiency[62]= 15.35;
    fDetEfficiency[63]= 15.37;
    fDetEfficiency[64]= 15.43;
    fDetEfficiency[65]= 15.49;
    fDetEfficiency[66]= 15.59;
    fDetEfficiency[67]= 15.84;
    fDetEfficiency[68]= 15.84;
    fDetEfficiency[69]= 15.92;
    fDetEfficiency[70]= 16.01;
    fDetEfficiency[71]= 16.22;
    fDetEfficiency[72]= 16.41;
    fDetEfficiency[73]= 16.42;
    fDetEfficiency[74]= 16.52;
    fDetEfficiency[75]= 16.86;
    fDetEfficiency[76]= 17.1;
    fDetEfficiency[77]= 17.17;
    fDetEfficiency[78]= 17.22;
    fDetEfficiency[79]= 17.46;
    fDetEfficiency[80]= 17.79;
    fDetEfficiency[81]= 17.99;
    fDetEfficiency[82]= 18.13;
    fDetEfficiency[83]= 18.33;
    fDetEfficiency[84]= 18.34;
    fDetEfficiency[85]= 18.53;
    fDetEfficiency[86]= 18.72;
    fDetEfficiency[87]= 18.95;
    fDetEfficiency[88]= 19.02;
    fDetEfficiency[89]= 19.15;
    fDetEfficiency[90]= 19.28;
    fDetEfficiency[91]= 19.45;
    fDetEfficiency[92]= 19.66;
    fDetEfficiency[93]= 19.69;
    fDetEfficiency[94]= 19.77;
    fDetEfficiency[95]= 19.73;
    fDetEfficiency[96]= 19.95;
    fDetEfficiency[97]= 19.98;
    fDetEfficiency[98]= 20.17;
    fDetEfficiency[99]= 20.29;
    fDetEfficiency[100]=20.33;
    fDetEfficiency[101]=20.37;
    fDetEfficiency[102]=20.47;
    fDetEfficiency[103]=20.48;
    fDetEfficiency[104]=20.57;
    fDetEfficiency[105]=20.75;
    fDetEfficiency[106]=20.8;
    fDetEfficiency[107]=20.84;
    fDetEfficiency[108]=20.86;
    fDetEfficiency[109]=20.88;
    fDetEfficiency[110]=21.0;
    fDetEfficiency[111]=21.06;
    fDetEfficiency[112]=21.0;
    fDetEfficiency[113]=21.06;
    fDetEfficiency[114]=21.06;
    fDetEfficiency[115]=21.04;
    fDetEfficiency[116]=21.1;
    fDetEfficiency[117]=21.14;
    fDetEfficiency[118]=21.08;
    fDetEfficiency[119]=21.17;
    fDetEfficiency[120]=21.3;
    fDetEfficiency[121]=21.38;
    fDetEfficiency[122]=21.49;
    fDetEfficiency[123]=21.58;
    fDetEfficiency[124]=21.69;
    fDetEfficiency[125]=21.77;
    fDetEfficiency[126]=21.87;
    fDetEfficiency[127]=22.02;
    fDetEfficiency[128]=22.13;
    fDetEfficiency[129]=22.29;
    fDetEfficiency[130]=22.35;
    fDetEfficiency[131]=22.45;
    fDetEfficiency[132]=22.53;
    fDetEfficiency[133]=22.55;
    fDetEfficiency[134]=22.64;
    fDetEfficiency[135]=22.67;
    fDetEfficiency[136]=22.73;
    fDetEfficiency[137]=22.74;
    fDetEfficiency[138]=22.71;
    fDetEfficiency[139]=22.79;
    fDetEfficiency[140]=22.76;
    fDetEfficiency[141]=22.77;
    fDetEfficiency[142]=22.76;
    fDetEfficiency[143]=22.75;
    fDetEfficiency[144]=22.78;
    fDetEfficiency[145]=22.7;
    fDetEfficiency[146]=22.68;
    fDetEfficiency[147]=22.72;
    fDetEfficiency[148]=22.66;
    fDetEfficiency[149]=22.64;
    fDetEfficiency[150]=22.7;
    fDetEfficiency[151]=22.67;
    fDetEfficiency[152]=22.71;
    fDetEfficiency[153]=22.67;
    fDetEfficiency[154]=22.75;
    fDetEfficiency[155]=22.77;
    fDetEfficiency[156]=22.83;
    fDetEfficiency[157]=22.84;
    fDetEfficiency[158]=22.93;
    fDetEfficiency[159]=22.97;
    fDetEfficiency[160]=23.0;
    fDetEfficiency[161]=23.08;
    fDetEfficiency[162]=23.16;
    fDetEfficiency[163]=23.27;
    fDetEfficiency[164]=23.25;
    fDetEfficiency[165]=23.37;
    fDetEfficiency[166]=23.44;
    fDetEfficiency[167]=23.49;
    fDetEfficiency[168]=23.55;
    fDetEfficiency[169]=23.52;
    fDetEfficiency[170]=23.58;
    fDetEfficiency[171]=23.64;
    fDetEfficiency[172]=23.63;
    fDetEfficiency[173]=23.58;
    fDetEfficiency[174]=23.64;
    fDetEfficiency[175]=23.63;
    fDetEfficiency[176]=23.62;
    fDetEfficiency[177]=23.64;
    fDetEfficiency[178]=23.63;
    fDetEfficiency[179]=23.66;
    fDetEfficiency[180]=23.59;
    fDetEfficiency[181]=23.59;
    fDetEfficiency[182]=23.56;
    fDetEfficiency[183]=23.58;
    fDetEfficiency[184]=23.63;
    fDetEfficiency[185]=23.57;
    fDetEfficiency[186]=23.66;
    fDetEfficiency[187]=23.62;
    fDetEfficiency[188]=23.67;
    fDetEfficiency[189]=23.64;
    fDetEfficiency[190]=23.54;
    fDetEfficiency[191]=23.57;
    fDetEfficiency[192]=23.51;
    fDetEfficiency[193]=23.53;
    fDetEfficiency[194]=23.45;
    fDetEfficiency[195]=23.3;
    fDetEfficiency[196]=23.41;
    fDetEfficiency[197]=23.25;
    fDetEfficiency[198]=23.21;
    fDetEfficiency[199]=23.08;
    fDetEfficiency[200]=23.01;
    fDetEfficiency[201]=22.92;
    fDetEfficiency[202]=22.9;
    fDetEfficiency[203]=22.76;
    fDetEfficiency[204]=22.76;
    fDetEfficiency[205]=22.61;
    fDetEfficiency[206]=22.53;
    fDetEfficiency[207]=22.48;
    fDetEfficiency[208]=22.39;
    fDetEfficiency[209]=22.29;
    fDetEfficiency[210]=22.24;
    fDetEfficiency[211]=22.2;
    fDetEfficiency[212]=22.12;
    fDetEfficiency[213]=22.07;
    fDetEfficiency[214]=21.96;
    fDetEfficiency[215]=21.89;
    fDetEfficiency[216]=21.87;
    fDetEfficiency[217]=21.76;
    fDetEfficiency[218]=21.74;
    fDetEfficiency[219]=21.58;
    fDetEfficiency[220]=21.49;
    fDetEfficiency[221]=21.48;
    fDetEfficiency[222]=21.37;
    fDetEfficiency[223]=21.29;
    fDetEfficiency[224]=21.2;
    fDetEfficiency[225]=21.17;
    fDetEfficiency[226]=21.03;
    fDetEfficiency[227]=20.98;
    fDetEfficiency[228]=20.92;
    fDetEfficiency[229]=20.85;
    fDetEfficiency[230]=20.76;
    fDetEfficiency[231]=20.69;
    fDetEfficiency[232]=20.58;
    fDetEfficiency[233]=20.56;
    fDetEfficiency[234]=20.47;
    fDetEfficiency[235]=20.37;
    fDetEfficiency[236]=20.32;
    fDetEfficiency[237]=20.24;
    fDetEfficiency[238]=20.13;
    fDetEfficiency[239]=20.08;
    fDetEfficiency[240]=19.9;
    fDetEfficiency[241]=19.84;
    fDetEfficiency[242]=19.77;
    fDetEfficiency[243]=19.69;
    fDetEfficiency[244]=19.63;
    fDetEfficiency[245]=19.51;
    fDetEfficiency[246]=19.41;
    fDetEfficiency[247]=19.27;
    fDetEfficiency[248]=19.06;
    fDetEfficiency[249]=19.01;
    fDetEfficiency[250]=18.87;
    fDetEfficiency[251]=18.7;
    fDetEfficiency[252]=18.49;
    fDetEfficiency[253]=18.41;
    fDetEfficiency[254]=18.17;
    fDetEfficiency[255]=17.98;
    fDetEfficiency[256]=17.84;
    fDetEfficiency[257]=17.69;
    fDetEfficiency[258]=17.5;
    fDetEfficiency[259]=17.25;
    fDetEfficiency[260]=17.15;
    fDetEfficiency[261]=16.98;
    fDetEfficiency[262]=16.79;
    fDetEfficiency[263]=16.66;
    fDetEfficiency[264]=16.48;
    fDetEfficiency[265]=16.32;
    fDetEfficiency[266]=16.19;
    fDetEfficiency[267]=16.02;
    fDetEfficiency[268]=15.88;
    fDetEfficiency[269]=15.77;
    fDetEfficiency[270]=15.67;
    fDetEfficiency[271]=15.5;
    fDetEfficiency[272]=15.39;
    fDetEfficiency[273]=15.23;
    fDetEfficiency[274]=15.09;
    fDetEfficiency[275]=15.04;
    fDetEfficiency[276]=14.92;
    fDetEfficiency[277]=14.75;
    fDetEfficiency[278]=14.7;
    fDetEfficiency[279]=14.5;
    fDetEfficiency[280]=14.45;
    fDetEfficiency[281]=14.34;
    fDetEfficiency[282]=14.25;
    fDetEfficiency[283]=14.16;
    fDetEfficiency[284]=14.13;
    fDetEfficiency[285]=14.0;
    fDetEfficiency[286]=13.92;
    fDetEfficiency[287]=13.84;
    fDetEfficiency[288]=13.76;
    fDetEfficiency[289]=13.73;
    fDetEfficiency[290]=13.61;
    fDetEfficiency[291]=13.54;
    fDetEfficiency[292]=13.52;
    fDetEfficiency[293]=13.45;
    fDetEfficiency[294]=13.41;
    fDetEfficiency[295]=13.39;
    fDetEfficiency[296]=13.31;
    fDetEfficiency[297]=13.22;
    fDetEfficiency[298]=13.17;
    fDetEfficiency[299]=13.13;
    fDetEfficiency[300]=13.06;
    fDetEfficiency[301]=13.2;
    fDetEfficiency[302]=13.09;
    fDetEfficiency[303]=12.97;
    fDetEfficiency[304]=12.92;
    fDetEfficiency[305]=12.73;
    fDetEfficiency[306]=12.65;
    fDetEfficiency[307]=12.4;
    fDetEfficiency[308]=12.22;
    fDetEfficiency[309]=12.02;
    fDetEfficiency[310]=11.79;
    fDetEfficiency[311]=11.59;
    fDetEfficiency[312]=11.33;
    fDetEfficiency[313]=11.03;
    fDetEfficiency[314]=10.68;
    fDetEfficiency[315]=10.46;
    fDetEfficiency[316]=10.14;
    fDetEfficiency[317]=9.88;
    fDetEfficiency[318]=9.62;
    fDetEfficiency[319]=9.36;
    fDetEfficiency[320]=9.14;
    fDetEfficiency[321]=8.87;
    fDetEfficiency[322]=8.63;
    fDetEfficiency[323]=8.51;
    fDetEfficiency[324]=8.24;
    fDetEfficiency[325]=8.07;
    fDetEfficiency[326]=7.88;
    fDetEfficiency[327]=7.77;
    fDetEfficiency[328]=7.65;
    fDetEfficiency[329]=7.52;
    fDetEfficiency[330]=7.35;
    fDetEfficiency[331]=7.27;
    fDetEfficiency[332]=7.21;
    fDetEfficiency[333]=7.1;
    fDetEfficiency[334]=6.92;
    fDetEfficiency[335]=6.89;
    fDetEfficiency[336]=6.79;
    fDetEfficiency[337]=6.74;
    fDetEfficiency[338]=6.56;
    fDetEfficiency[339]=6.54;
    fDetEfficiency[340]=6.5;
    fDetEfficiency[341]=6.39;
    fDetEfficiency[342]=6.33;
    fDetEfficiency[343]=6.25;
    fDetEfficiency[344]=6.27;
    fDetEfficiency[345]=6.14;
    fDetEfficiency[346]=6.06;
    fDetEfficiency[347]=6.04;
    fDetEfficiency[348]=6.01;
    fDetEfficiency[349]=5.91;
    fDetEfficiency[350]=5.89;
    fDetEfficiency[351]=5.79;
    fDetEfficiency[352]=5.75;
    fDetEfficiency[353]=5.75;
    fDetEfficiency[354]=5.67;
    fDetEfficiency[355]=5.61;
    fDetEfficiency[356]=5.51;
    fDetEfficiency[357]=5.52;
    fDetEfficiency[358]=5.43;
    fDetEfficiency[359]=5.43;
    fDetEfficiency[360]=5.34;
    fDetEfficiency[361]=5.31;
    fDetEfficiency[362]=5.35;
    fDetEfficiency[363]=5.23;
    fDetEfficiency[364]=5.2;
    fDetEfficiency[365]=5.14;
    fDetEfficiency[366]=5.11;
    fDetEfficiency[367]=5.11;
    fDetEfficiency[368]=5.01;
    fDetEfficiency[369]=4.98;
    fDetEfficiency[370]=4.93;
    fDetEfficiency[371]=4.99;
    fDetEfficiency[372]=4.89;
    fDetEfficiency[373]=4.82;
    fDetEfficiency[374]=4.87;
    fDetEfficiency[375]=4.8;
    fDetEfficiency[376]=4.7;
    fDetEfficiency[377]=4.65;
    fDetEfficiency[378]=4.65;
    fDetEfficiency[379]=4.61;
    fDetEfficiency[380]=4.49;
    fDetEfficiency[381]=4.56;
    fDetEfficiency[382]=4.44;
    fDetEfficiency[383]=4.42;
    fDetEfficiency[384]=4.44;
    fDetEfficiency[385]=4.35;
    fDetEfficiency[386]=4.35;
    fDetEfficiency[387]=4.27;
    fDetEfficiency[388]=4.29;
    fDetEfficiency[389]=4.19;
    fDetEfficiency[390]=4.13;
    fDetEfficiency[391]=4.08;
    fDetEfficiency[392]=4.02;
    fDetEfficiency[393]=4.07;
    fDetEfficiency[394]=3.92;
    fDetEfficiency[395]=3.95;
    fDetEfficiency[396]=3.88;
    fDetEfficiency[397]=3.82;
    fDetEfficiency[398]=3.86;
    fDetEfficiency[399]=3.74;
    fDetEfficiency[400]=3.71;
    fDetEfficiency[401]=3.66;
    fDetEfficiency[402]=3.72;
    fDetEfficiency[403]=3.62;
    fDetEfficiency[404]=3.55;
    fDetEfficiency[405]=3.56;
    fDetEfficiency[406]=3.57;
    fDetEfficiency[407]=3.45;
    fDetEfficiency[408]=3.38;
    fDetEfficiency[409]=3.36;
    fDetEfficiency[410]=3.36;
    fDetEfficiency[411]=3.28;
    fDetEfficiency[412]=3.25;
    fDetEfficiency[413]=3.19;
    fDetEfficiency[414]=3.26;
    fDetEfficiency[415]=3.13;
    fDetEfficiency[416]=3.17;
    fDetEfficiency[417]=3.15;
    fDetEfficiency[418]=3.04;
    fDetEfficiency[419]=2.98;
    fDetEfficiency[420]=2.93;
    fDetEfficiency[421]=2.98;
    fDetEfficiency[422]=2.9;
    fDetEfficiency[423]=2.89;
    fDetEfficiency[424]=2.9;
    fDetEfficiency[425]=2.81;
    fDetEfficiency[426]=2.74;
    fDetEfficiency[427]=2.81;
    fDetEfficiency[428]=2.68;
    fDetEfficiency[429]=2.73;
    fDetEfficiency[430]=2.7;
    fDetEfficiency[431]=2.57;
    fDetEfficiency[432]=2.58;
    fDetEfficiency[433]=2.55;
    fDetEfficiency[434]=2.55;
    fDetEfficiency[435]=2.37;
    fDetEfficiency[436]=2.39;
    fDetEfficiency[437]=2.39;
    fDetEfficiency[438]=2.44;
    fDetEfficiency[439]=2.37;
    fDetEfficiency[440]=2.26;
    fDetEfficiency[441]=2.27;
    fDetEfficiency[442]=2.27;
    fDetEfficiency[443]=2.23;
    fDetEfficiency[444]=2.26;
    fDetEfficiency[445]=2.14;
    fDetEfficiency[446]=2.08;
    fDetEfficiency[447]=2.15;
    fDetEfficiency[448]=2.06;
    fDetEfficiency[449]=2.09;
    fDetEfficiency[450]=2.04;
    fDetEfficiency[451]=2.0;
    fDetEfficiency[452]=1.95;
    fDetEfficiency[453]=2.02;
    fDetEfficiency[454]=1.87;
    fDetEfficiency[455]=1.9;
    fDetEfficiency[456]=1.8;
    fDetEfficiency[457]=1.87;
    fDetEfficiency[458]=1.85;
    fDetEfficiency[459]=1.87;
    fDetEfficiency[460]=1.81;
    fDetEfficiency[461]=1.86;
    fDetEfficiency[462]=1.74;
    fDetEfficiency[463]=1.74;
    fDetEfficiency[464]=1.63;
    fDetEfficiency[465]=1.59;
    fDetEfficiency[466]=1.5;
    fDetEfficiency[467]=1.5;
    fDetEfficiency[468]=1.44;
    fDetEfficiency[469]=1.47;
    fDetEfficiency[470]=1.32;
    fDetEfficiency[471]=1.24;
    fDetEfficiency[472]=1.28;
    fDetEfficiency[473]=1.19;
    fDetEfficiency[474]=1.21;
    fDetEfficiency[475]=1.21;
    fDetEfficiency[476]=1.1;
    fDetEfficiency[477]=1.1;
    fDetEfficiency[478]=1.05;
    fDetEfficiency[479]=1.06;
    fDetEfficiency[480]=0.94;
    fDetEfficiency[481]=0.92;
    fDetEfficiency[482]=0.87;
    fDetEfficiency[483]=0.92;
    fDetEfficiency[484]=0.81;
    fDetEfficiency[485]=0.86;
    fDetEfficiency[486]=0.78;
    fDetEfficiency[487]=0.77;
    fDetEfficiency[488]=0.8;
    fDetEfficiency[489]=0.67;
    fDetEfficiency[490]=0.7;
    fDetEfficiency[491]=0.81;
    fDetEfficiency[492]=0.61;
    fDetEfficiency[493]=0.64;
    fDetEfficiency[494]=0.71;
    fDetEfficiency[495]=0.66;
    fDetEfficiency[496]=0.67;
    fDetEfficiency[497]=0.68;
    fDetEfficiency[498]=0.69;
    fDetEfficiency[499]=0.68;
    fDetEfficiency[500]=0.73;

    // still need to convert from percent and cut values below credible limit
    for (Int_t iBin=0;iBin<501;iBin++) 
      {
	if (iBin<(Int_t)(credibleLimit-flambda_min))
	  {
	    fDetEfficiency[iBin]=0.;
	  } 
	else
	  {
	    fDetEfficiency[iBin]=fDetEfficiency[iBin]/100.;
	  }
	//      cout << "efficiency is " << fDetEfficiency[iBin] << " at " << fLambdaMin+iBin<<endl;
      } 

  }                 
  else {            
    cout << "ERROR:    photocathode type not specified" << endl;

    flambda_min = 100.;
    flambda_max = 100.;
    flambda_step = 100.;

    fDetEfficiency[0] = 0.;
  }
}  

//------------------------------------------------------------------------------------  

Double_t PndDrcDigiTask::FuncD1(Double_t x) {
  
  // refraction index of fused silica as a function of wavelength
  Double_t par0 = 0.696;
  Double_t par1 = 0.068;
  Double_t par2 = 0.407;
  Double_t par3 = 0.116;
  Double_t par4 = 0.897;
  Double_t par5 = 9.896;

  return sqrt(1.+
	      (par0*x*x/(x*x-par1*par1))+
	      (par2*x*x/(x*x-par3*par3))+
	      (par4*x*x/(x*x-par5*par5)));
}

Double_t PndDrcDigiTask::FuncD3(Double_t x, Double_t y) {
  // reflection probability according to the scalar theory (PhD Thesis R.Hohler)
  Double_t par0 = fRoughness;
  return 1. - pow(4.*3.14159*cos(y)*par0*FuncD1(x)/x,2);
}

void PndDrcDigiTask::SetPhotonTransportEff(){ 
  if (fDetType == 1){
    flambda_min_tr = 280.;
    flambda_max_tr = 650.;
    flambda_step_tr = 10.;
  
    fangle_step_tr = fpi/2./20.;
    flambda_points_tr = (flambda_max_tr-flambda_min_tr)/1000./0.01+1;//38;
 
    // refraction index of silica:
    //  TF1 *d1 = new TF1("d1", "sqrt(1 + ([0]*x^2/(x^2-[1]^2)) + ([2]*x^2/(x^2-[3]^2)) + ([4]*x^2/(x^2-[5]^2)))",lambdaMin/1000.,lambdaMax/1000.);
    //  d1->SetParameters(0.696, 0.068, 0.407, 0.116, 0.897, 9.896);
  
    // reflection probability according to the scalar theory
    //  TF2* d3 = new TF2("d3", "1. - pow(4.*3.14159*cos(y)*[0]*d1/x,2)",lambdaMin/1000.,lambdaMax/1000., 0.,fpi/2.); 
    //  d3->SetParameter(0,fRoughness);
    
    //cout<<"-I- SetPhotonTransportEff: reflection coef(l = 0.53, ang = 0.95) = "<<FuncD3(0.53,0.95)<<endl; 
  
    for(int iang=0; iang<21; iang++){ // angle range, 21 points, step = pi/2/20    
      for(int ilam=0; ilam < flambda_points_tr; ilam++){ // lambda range, 38 points, step = (0.65-0.28)/37 = 0.01 = 10 nm      
	fTranspEfficiency[iang * flambda_points_tr + ilam] = FuncD3(flambda_min_tr/1000.+ilam*0.01, iang*fpi/2./20.);
      }
    }    
  }
  else {
    cout << "ERROR: photocathode type not specified" << endl;    
  }
  
}
//--------------------------------------------------------------
TVector3 PndDrcDigiTask::GetSensorDimensions(Int_t sensorID)
{
  //PndGeoHandling GeoH(gGeoManager);
  gGeoManager->cd(fGeoH->GetPath(sensorID));
  TGeoVolume* actVolume = gGeoManager->GetCurrentVolume();
  TGeoBBox* actBox = (TGeoBBox*)(actVolume->GetShape());
  TVector3 result(actBox->GetDX(),actBox->GetDY(),actBox->GetDZ());
  return result;
} 

//-------------Set Parameter------------------------------------
void PndDrcDigiTask::SetParameters(){
  fDetType=1; //  Detector Type =1  
  nRefrac=fGeo->nEV();//1.467;  //Refractive index of SOB   
  fSigmat=0.05;  //Time Resolution is 50 ps ############################
  fVerbose=1;
  fCollectionEff=0.65;//Collection Efficiency 
  fPackingFraction=0.80;//Packing Efficiency 
  fRoughness = 0.001; // 10 A
  fTimeGranularity = 0.098;// [ns] = 98 ps granularity of the time signal
  
  // basic DIRC parameters:
  fpi            =  TMath::Pi();
  fzup		     =  fGeo->barBoxZUp();
  fzdown	     =  fGeo->barBoxZDown();
  fradius        =  fGeo->radius();          //radius in the middle of the bar = 50.cm
  fhthick        =  fGeo->barHalfThick();    //half thickness of the bars=1.7/2 cm
  fpipehAngle    =  fGeo->PipehAngle(); 
  fbbGap         =  fGeo->BBoxGap();
  fbbnum         =  fGeo->BBoxNum();
  fbarnum        =  fGeo->barNum();
  fphi0          =  (180.-2.*fpipehAngle)/fbbnum + fpipehAngle;
  fdphi          =  (180.-2.*fpipehAngle)/fbbnum*2.;
  flside	     =  fGeo->Lside();
  fbarwidth	     =  fGeo->BarWidth();
  fPixelSize	 =  fGeo->PixelSize();
  fMcpActiveArea =  fGeo->McpActiveArea();
  fNpix          =  fGeo->Npixels();
  
  fPixelGap      =  (fMcpActiveArea - (Double_t)fNpix*fPixelSize) / ((Double_t)fNpix - 1.);
  fPixelStep     =  fMcpActiveArea/fNpix; //fPixelSize + 0.5*fPixelGap; 
  
  fPixelSigma 	 =  fGeo->SigmaCharge();
  fDeadTime 	 =  fGeo->DeadTime(); 
  
  fThreshold	 =  0.20; //threshold to detect charge shared hits
}
//-------------Smear Time------------------------------------
void PndDrcDigiTask::Smear(Double_t& time, Double_t sigt){
   time += gRandom->Gaus(0,sigt);
}

// -----   Private method Reset   ------------------------------------------
void PndDrcDigiTask::Reset() {
  fNDigis = 0;
  fPixelMap.clear();
  if ( fDrcDigiArray ) fDrcDigiArray->Clear();  
}

// -------------------------------------------------------------------------

// -----   Finish Task   ---------------------------------------------------
void PndDrcDigiTask::Finish()
{
  if ( fDrcDigiArray ) fDrcDigiArray->Clear();
  cout << "-I- PndDrcDigiTask: Finish" << endl;
 }
// -------------------------------------------------------------------------


ClassImp(PndDrcDigiTask)
