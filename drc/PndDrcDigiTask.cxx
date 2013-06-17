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
  SetParameters();
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

   


  cout << "-I- PndDrcDigiTask: Intialization successfull" << endl;  	  
  
  return kSUCCESS;
   
}
// -----   Execution of Task   ---------------------------------------------
void PndDrcDigiTask::Exec(Option_t* option)
{
  Reset();
  if ( ! fDrcDigiArray ) Fatal("Exec", "No Photon DigiArray");
  fDrcDigiArray->Delete();
  nevents++;
  if (fVerbose > 1) printf("\n\n=====> Event No. %d\n", nevents);
  std::cout<<"=====> Event No. "<<nevents<<endl; 
  
  ProcessPhotonPoint();  
 
}

//-------- Photon Detector Hit production with efficiency-------------   
void PndDrcDigiTask::ProcessPhotonPoint()
{  
  // Reset output array
  fGeoH->SetVerbose(fVerbose);
  
  if (fVerbose > 0) {
    cout <<" Number of Photon MC Points in Photon Detector Plane : "<<fPDPointArray->GetEntries()<<endl;
  }
     
  Double_t lambda_min,lambda_max,lambda_step;
  Double_t DetEfficiency[800];
  //SetFakeDetEff(lambda_min,lambda_max,lambda_step,DetEfficiency);
  SetPhotonDetEffNew(lambda_min,lambda_max,lambda_step,DetEfficiency); 
  
  Double_t lambda_min_tr,lambda_max_tr,lambda_step_tr, angle_step_tr;
  Int_t lambda_points_tr;
  Double_t TransportEfficiency[798];
  SetPhotonTransportEff(lambda_min_tr,lambda_max_tr,lambda_step_tr,angle_step_tr, lambda_points_tr,TransportEfficiency);
  
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
    //cout<<"N bouncesX = "<<NbouncesX<<", NbouncesY = "<<NbouncesY<<", angleX = "<<angleX<<", angleY = "<<angleY<<endl;
      
    Double_t PPx= Ppt->GetPx();
    Double_t PPy= Ppt->GetPy();
    Double_t PPz= Ppt->GetPz();

    Double_t etot = sqrt(PPx*PPx + PPy*PPy +PPz*PPz);// in GeV
    Double_t lambda=197.0*2.0*fpi/(etot*1.0E9);//wavelength of photon in nm
    //cout<<"1. lambda = "<<lambda<<endl;
       
    if(fisDetEff){
      //cout<<"det eff!!! lam_min = "<<lambda_min<<", lam_max = "<<lambda_max<<endl;
      if (lambda >= lambda_min && lambda < lambda_max) {
        Int_t ilambda=(Int_t)((lambda-lambda_min)/lambda_step);
	    Double_t rand = gRandom->Rndm();
	    fDetection = 0;
        if (DetEfficiency[ilambda]*fCollectionEff*fPackingFraction > rand ) fDetection = 1;
            
	    detEffLam->Fill(lambda, DetEfficiency[ilambda]*fCollectionEff);
	    //cout<<"filled lam = "<<lambda<<", eff = "<<DetEfficiency[ilambda]<<endl;
	  }	 
    }
    if(!fisDetEff){ 
      fDetection=1;
    }
    //cout<<"fDetection = "<<fDetection<<endl;
    //####################################
    // transport efficiency
    // assume that detection efficiency above CAN NOT be used together with transport efficiency
    // Maria Patsyuk
    //cout<<"do transport eff? "<<fisTransportEff<<", lambda = "<<lambda<<endl;
    if(fisTransportEff){
      if (lambda >= lambda_min_tr && lambda < lambda_max_tr) {
	    Int_t ilambda=(Int_t)((lambda-lambda_min_tr)/lambda_step_tr); 
	    Int_t iangleX =(Int_t)(angleX/angle_step_tr);	    
	    Int_t iangleY =(Int_t)(angleY/angle_step_tr);
	    //cout<<"iangleX = "<<iangleX<<", iangleY = "<<iangleY<<", ilam = "<<ilambda<<", lambda_points_tr = "<<lambda_points_tr<<endl;          
	    Double_t rand = gRandom->Rndm();
	    fDetection = 0;
	    //cout<<"Bounce eff = "<<TransportEfficiency[ilambda]<<endl;
	    Double_t TotalTrProb = 1.;
	    //cout<<"before ref prob. angleX = "<<angleX<<", angleY = "<<angleY<<endl;
	    Double_t ReflectionProbX = TransportEfficiency[ilambda+ iangleX*lambda_points_tr];
	    Double_t ReflectionProbY = TransportEfficiency[ilambda+ iangleY*lambda_points_tr];
	    TotalTrProb = pow(ReflectionProbX, (Int_t)NbouncesX)*pow(ReflectionProbY, (Int_t)NbouncesY);
	    //cout<<"tr eff X = "<<ReflectionProbX<<", tr eff Y = "<<ReflectionProbY<<", total = "<<TotalTrProb<<endl;
	    if(TotalTrProb > rand) fDetection = 1;
       }
     }
     if(!fisTransportEff && !fisDetEff){ 
       fDetection=1;
     }      
     //####################################  
     

     if(fDetection==1){
     
     	Double_t xP= Ppt->GetX();
     	Double_t yP= Ppt->GetY();
     	Double_t zP= Ppt->GetZ();
            
     	//std::cout<<" -I- PndDrcDigiTask: detName : "<<fGeoH->GetPath(Ppt->GetDetectorID())<<std::endl;
     	//std::cout<<" -I- PndDrcDigiTask: sensor ID : "<<Ppt->GetDetectorID()<<std::endl;
     	// transform to local sensor system... (mc point has the ID not the path to the volume)
     	TVector3 PptPosition;
     	PptPosition.SetXYZ(xP, yP, zP);
     	TVector3 posL = fGeoH->MasterToLocalShortId(PptPosition,Ppt->GetDetectorID());
	 
	//std::cout<<"-I- PndDrcDigiTask: global position: x = "<<xP<<", y = "<<yP<<", z = "<<zP<<std::endl;
	//std::cout<<"-I- PndDrcDigiTask: local position: x = "<<posL.X()<<", y = "<<posL.Y()<<", z = 	"<<posL.Z()<<std::endl;
	 
    	TVector3 sensorDim=GetSensorDimensions(Ppt->GetDetectorID());	
 
     	if (fVerbose > 1){
	      std::cout << "SensorDimension for: " << Ppt->GetDetectorID() << std::endl;
	      std::cout << sensorDim.X() << " " << sensorDim.Y() << " " << sensorDim.Z() << std::endl;
	}
	 
	//usually sensors have origin in the middle, let's move it to the left lower corner:
     	TVector3 posLshifted;
     	posLshifted.SetXYZ(posL.X()+sensorDim.X(), posL.Y()+sensorDim.Y(), posL.Z()+sensorDim.Z());
     	//std::cout<<"-I- PndDrcDigiTask: local shifted position: x = "<<posLshifted.X()<<", y = "<<posLshifted.Y()<<", z = "<<posLshifted.Z()<<std::endl;
     	//std::cout<<" find out the pixelStep: posLshifted/fPixelStep = "<<posLshifted.X()/fPixelStep<<std::endl;
	 
     	// calculate the number of the fired pixel
     	Int_t Ncol = (Int_t)TMath::Floor(posLshifted.X()/fPixelStep);
     	Int_t Nrow = (Int_t)TMath::Floor(posLshifted.Y()/fPixelStep);
     	Int_t NpixelLocal = Ncol + Nrow  * fNpix;
     	//cout<<"Ncol = "<<Ncol<<", Nrow = "<<Nrow<<", NpixelLocal = "<<NpixelLocal<<endl;
     	//cout<<"fPixelGap = "<<fPixelGap<<", sPixelStep = "<<fPixelStep<<endl;
     	//cout<<"fMcpActiveArea = "<<fMcpActiveArea<<", fPixelSize = "<<fPixelSize<<", fNpix = "<<fNpix<<endl;
     	fDetectorID = Ppt->GetDetectorID() * 100 + NpixelLocal;
     	//cout<<"det ID = "<<fDetectorID<<endl;
	 
	Double_t time=Ppt->GetTime();
     	Smear(time,fSigmat);	
     	fTime=(Int_t)(time/fTimeGranularity)*fTimeGranularity;  // time is smeared and digitized = has granularity
	     
        if(fChargeSharing == kFALSE){
       		//PndDrcDigi* aNewDigi = AddDrcDigi(fDetectorID, fPixelID, k ,fTime, fTimeError); // k - MC array index
       		//PndDrcDigi* aNewDigi = AddDrcDigi(k, fDetectorID, 0., fTime); // k - MC array index
		ActivatePixel(fDetectorID, fTime, k, 0);
		//ActivatePixel(fDetectorID, fTime, k);
     	}
     
     	if(fChargeSharing == kTRUE){
	
		ActivatePixel(fDetectorID, fTime, k, 0);
		//cout<<"pixel "<<fDetectorID<<" is original: Nrow = "<<Nrow<<", Ncol = "<<Ncol<<endl;
     		
		// find fired pixels:
     		Double_t distance = 999.;
     		TVector3 corner;
		TVector3 point;
		
		// left pixel
     		distance =  posLshifted.X() - TMath::Floor(posLshifted.X()/fPixelStep)*fPixelStep; //[cm]		
     		if(exp(- distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	
			if((Ncol-1) > 0){
				ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol-1+Nrow*fNpix, fTime, k, 1);
				//cout<<"left pixel is activated: distance = "<<distance <<", prob = "<<exp(- distance/fPixelSigma)<<endl;	
			}
			distance = 999.;
		}
     		// right pixel
     		distance =  TMath::Ceil(posLshifted.X()/fPixelStep)*fPixelStep - posLshifted.X();		
     		if(exp(- distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	
			if(Ncol+1 < fNpix){
				ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+1+Nrow*fNpix, fTime, k, 1);
				//cout<<"right pixel is activated: distance = "<<distance <<", prob = "<<exp(- distance/fPixelSigma)<<endl;		
			}
			distance = 999.;
		}
     		// lower pixel
     		distance =  posLshifted.Y() - TMath::Floor(posLshifted.Y()/fPixelStep)*fPixelStep;		
     		if(exp(- distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	
			if(Nrow-1 > 0){
				ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+(Nrow-1)*fNpix, fTime, k, 1);
				//cout<<"lower pixel is activated: distance = "<<distance <<", prob = "<<exp(- distance/fPixelSigma)<<endl;	
			}
			distance = 999.;
		}
     		// upper pixel
     		distance =  TMath::Ceil(posLshifted.Y()/fPixelStep)*fPixelStep - posLshifted.Y();     		
		if(exp(- distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	
			if(Nrow+1 < fNpix){
				ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+(Nrow+1)*fNpix, fTime, k, 1);
				//cout<<"upper pixel is activated: distance = "<<distance <<", prob = "<<exp(- distance/fPixelSigma)<<endl;	
			}
			distance = 999.;
		}
		
		point.SetXYZ(posLshifted.X(),posLshifted.Y(),0.);
		
		// upper left pixel
		corner.SetXYZ(TMath::Floor(posLshifted.X()/fPixelStep)*fPixelStep,
			      TMath::Ceil(posLshifted.Y()/fPixelStep)*fPixelStep,0.);
        	distance = (point-corner).Mag();		
		if(exp(-distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	
			if(Ncol-1 > 0 && Nrow+1 < fNpix){
				ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol-1+(Nrow+1)*fNpix, fTime, k, 1);
				//cout<<"upper left pixel is activated: distance = "<<distance <<", prob = "<<exp(- distance/fPixelSigma)<<endl;	
			}
			distance = 999.;
		}
		
		// bottom left pixel
		corner.SetXYZ(TMath::Floor(posLshifted.X()/fPixelStep)*fPixelStep,
		              TMath::Floor(posLshifted.Y()/fPixelStep)*fPixelStep,0.);
        	distance = (point-corner).Mag();		
		if(exp(-distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	
			if(Ncol-1 > 0 && Nrow-1 > 0){
				ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol-1+(Nrow-1)*fNpix, fTime, k, 1);
				//cout<<"bottom left pixel is activated: distance = "<<distance <<", prob = "<<exp(- distance/fPixelSigma)<<endl;	
			}
			distance = 999.;
		}
		
		// bottom right pixel
		corner.SetXYZ(TMath::Ceil(posLshifted.X()/fPixelStep)*fPixelStep,
		              TMath::Floor(posLshifted.Y()/fPixelStep)*fPixelStep,0.);
        	distance = (point-corner).Mag();		
		if(exp(-distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	
			if(Ncol+1 < fNpix && Nrow-1 > 0){
				ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+1+(Nrow-1)*fNpix, fTime, k, 1);
				//cout<<"bottom right pixel is activated: distance = "<<distance <<", prob = "<<exp(- distance/fPixelSigma)<<endl;	
			}
			distance = 999.;
		}
		
		// upper right pixel
		corner.SetXYZ(TMath::Ceil(posLshifted.X()/fPixelStep)*fPixelStep,
		              TMath::Ceil(posLshifted.Y()/fPixelStep)*fPixelStep,0.);
        	distance = (point-corner).Mag();		
		if(exp(-distance/fPixelSigma) > gRandom->Uniform(0.,1.) && exp(- distance/fPixelSigma) > fThreshold){	
			if(Ncol+1 < fNpix && Nrow+1 < fNpix){
				ActivatePixel(Ppt->GetDetectorID() * 100 +Ncol+1+(Nrow+1)*fNpix, fTime, k, 1);
				//cout<<"upper right pixel is activated: distance = "<<distance <<", prob = "<<exp(- distance/fPixelSigma)<<endl;	
			}
			distance = 999.;
		}
		
     	} // if charge sharing true     
     
     	
     } // if detection
  } 
}

// -----   Private method ActivatePixel   -------------------------------
void PndDrcDigiTask::ActivatePixel(Int_t sensorDetId, Double_t signalTime, Int_t k, Int_t csflag) {
// in case when the same pixel was fired by two different photons this function takes care of which hits from that pixel to write  
  //Int_t sensorDetId;
  //cout<<"-I- ActivatePixel: time = "<<signalTime<<endl;
  if ( fPixelMap.find(sensorDetId) == fPixelMap.end() ) {
    // pixel not yet active, create new digi
    
    	//cout <<"there is no such pixel (case 1) - "<< fNDigis << " : " << sensorDetId << " " << signalTime << endl;
    
    new ((*fDrcDigiArray)[fNDigis]) PndDrcDigi(k, sensorDetId, 0., signalTime, csflag, 0.);

    fPixelMap[sensorDetId] = fNDigis;
    fNDigis++;
  }
  else {    
    // if there was already hit in this pixel...
    //cout<<" there is a hit in the same pixel already (case 2)"<<endl;
    Int_t iDigi = fPixelMap[sensorDetId];
    PndDrcDigi* ddigi = dynamic_cast<PndDrcDigi*>(fDrcDigiArray->At(iDigi));
    // ... check the time difference with another one in the same pixel
    if(fabs(ddigi->GetTimeStamp() - signalTime) > fDeadTime){
      //cout<<"time difference = %% "<<fabs(ddigi->GetTimeStamp() - signalTime)<<endl;
      new ((*fDrcDigiArray)[fNDigis]) PndDrcDigi(k, sensorDetId, 0., signalTime, csflag, 0.);

      fPixelMap[sensorDetId] = fNDigis;
      fNDigis++; 
    }
    
    //ddigi->AddCharge(signalHeight);

    	//cout << "another mc point to this digi, last one " << ddigi->GetIndex(ddigi->GetNIndices()-1) << ", now adding " << k << endl;
    //ddigi->AddIndex(iPoint);
  }
}
// -------------------------------------------------------------------------


//------   Find Nubmer of Bounces     --------------------------------------
void PndDrcDigiTask::NumberOfBounces(TVector3 start, TVector3 dir, Int_t *n1, Int_t *n2, Double_t *alpha1, Double_t *alpha2){
    // calculates the number of bounces in x and y direction and reflection angles in these directions.
        
    Double_t PhiRot = FindPhiRot(start.X(), start.Y());
    //cout<<"-I- NumberOfBounces: phi rot = "<<PhiRot<<endl;
    
    // Photon production point in bar' coordinate system (origin at the corner of the bar):
    TVector3 startBar;
    startBar.SetXYZ(start.X(), start.Y(), start.Z());
    //cout<<"-I- NumberOfBounces: start.X = "<<start.X()<<", start.Y = "<<start.Y()<<endl;
    startBar.RotateZ(-PhiRot/180.*fpi);
    //cout<<"-I- NumberOfBounces: startBar.X = "<<startBar.X()<<", startBar.Y = "<<startBar.Y()<<endl;
    
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
    //cout<<"-I- NumberOfBounces: Z0 = "<<Z0<<", Theta = "<<PphoB.Theta()/3.1415*180.<<", tan t = "<<tan(PphoB.Theta())<<", phi = "<<PphoB.Phi()/3.1415*180.<<endl;
    X0 = Z0*tan(PphoB.Theta())*cos(PphoB.Phi());
    Y0 = Z0*tan(PphoB.Theta())*sin(PphoB.Phi());
    //cout<<"-I- NumberOfBounces: X0 = "<<X0<<", Y0 = "<<Y0<<endl;
    
    // Find the number of bounces in each direction       
    Double_t N1, N2;
    //frad_out = (fradius-fhthick)/cos(2.*3.1415/16./2.); // radius at corner - thickness ###
    //flside   = 2.*frad_out*sin(2.*3.1415/16./2.) - (2.*fboxthick) - (2.*fboxgap);
    //flside = (180. - 2.*fpipehAngle - fbbGap/fradius*(fbbnum/2. - 1.)/fpi*180.)/(fbbnum/2.) * fradius/ 180.*fpi;
    //fbarwidth = flside/fbarnum;
    //cout<<"-I- NumberOfBounces: lside = "<<flside<<", bar width = "<<fbarwidth<<endl;
        
    //cout<<"fbarnum = "<<fbarnum<<endl;
    // Find which bar in the bar box was hit:
    if(fbarnum > 1){    
      Int_t NhitBar = (Int_t)((0.5*flside + startBar.Y())/fbarwidth)+1;
      //cout<<"-I- NumberOfBounces: bar "<<NhitBar<<" was hit, "<<((0.5*flside + startBar.Y())/fbarwidth)+1<<endl;
      //cout<<"-I- NumberOfBounces: start bar Y = "<<startBar.Y()<<endl;
    
     //cout<<"-I- NumberOfBounces: start position X = "<< startBar.X() - (fradius-fhthick)<<", Y = "<<startBar.Y() + 0.5*flside-(NhitBar-1)*fbarwidth<<endl;  
      FindOutPoint(X0, startBar.X()-(fradius-fhthick), 		   2.*fhthick, &N1, 0);
      FindOutPoint(Y0, startBar.Y()+0.5*flside-(NhitBar-1)*fbarwidth, fbarwidth, &N2, 0);
      //cout<<"-I- NumberOfBounces: N1 = "<<N1<<", N2 = "<<N2<<endl;
    }
    
    if(fbarnum == 1 && flside > fbarwidth){
      //cout<<"second case!!!"<<endl;
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
    //cout<<"-I- NumberOfBounces: angle1 = "<<angle1<<", angle2 = "<<angle2<<endl;
}

//----------------------------------------------------------------------------------------------
Double_t PndDrcDigiTask::FindPhiRot(Double_t xx, Double_t yy){ // returns [degrees]

    TVector3 hit;
    hit.SetXYZ(xx,yy,0.);
    Double_t startPhi = hit.Phi()/fpi*180.; // [degrees]
    if(startPhi < 0.){startPhi = 360. + hit.Phi()*180./fpi;}
    //cout<<"-I- FindPhoRot: start phi = "<<startPhi<<endl;    
    //cout<<"-I- InBarCoordinateSystem: dphi = "<<fDphi<<endl;
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
void PndDrcDigiTask::SetFakeDetEff(Double_t& fLambdaMin, 
                       Double_t& fLambdaMax, Double_t& fLambdaStep, Double_t fEfficiency[])
{
//  if (fVerbose > 0) cout << "SetPhotoDetParamter called for Photocathode type " << fDetType << endl;

  if (fDetType == 1){

  /** Quantum efficiency taken from old Burle data */

  fLambdaMin = 300.;
  fLambdaMax = 700.;
  fLambdaStep = 400.;

  fEfficiency[0]  = 1.;  
  fEfficiency[1]  = 1.;   
  }                 
  else {            
    cout << "ERROR:    photocathode type not specified" << endl;

   fLambdaMin = 300.;
   fLambdaMax = 700.;
   fLambdaStep = 400.;

   fEfficiency[0] = 0.;

   }

}
  
//--------------------------------------------------------------------------
void PndDrcDigiTask::SetPhotonDetEffNew(Double_t& fLambdaMin, 
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

 void PndDrcDigiTask::SetPhotonTransportEff(
 			Double_t& lambdaMinT, Double_t& lambdaMaxT, Double_t& lambdaStepT, 
			Double_t& angleStepT, Int_t& lambdaPointsT, Double_t fEfficiency[]){ 
  if (fDetType == 1){
  
  	//cout<<"-I- SetPhotonTransportEff: lam = "<<lambda<<", angle = "<<angle<<endl;
  
  	lambdaMinT = 280.;
  	lambdaMaxT = 650.;
  	lambdaStepT = 10.;
  
  	angleStepT = fpi/2./20.;
	  lambdaPointsT = (lambdaMaxT-lambdaMinT)/1000./0.01+1;//38;
 
	  // refraction index of silica:
	//  TF1 *d1 = new TF1("d1", "sqrt(1 + ([0]*x^2/(x^2-[1]^2)) + ([2]*x^2/(x^2-[3]^2)) + 	([4]*x^2/(x^2-[5]^2)))",lambdaMin/1000.,lambdaMax/1000.);
	//  d1->SetParameters(0.696, 0.068, 0.407, 0.116, 0.897, 9.896);
  
	  // reflection probability according to the scalar theory
	//  TF2* d3 = new TF2("d3", "1. - pow(4.*3.14159*cos(y)*[0]*d1/x,2)",lambdaMin/1000.,lambdaMax/1000., 0.,fpi/2.); 
	//  d3->SetParameter(0,fRoughness);
    
	  //cout<<"-I- SetPhotonTransportEff: reflection coef(l = 0.53, ang = 0.95) = "<<FuncD3(0.53,0.95)<<endl; 
  
	  for(int iang=0; iang<21; iang++){ // angle range, 21 points, step = pi/2/20    
	    for(int ilam=0; ilam < lambdaPointsT; ilam++){ // lambda range, 38 points, step = (0.65-0.28)/37 = 0.01 = 10 nm      
	      fEfficiency[iang * lambdaPointsT + ilam] = FuncD3(lambdaMinT/1000.+ilam*0.01, iang*fpi/2./20.);
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
  TVector3 result;
  result.SetX(actBox->GetDX());
  result.SetY(actBox->GetDY());
  result.SetZ(actBox->GetDZ());
  
  //result.Dump();
  
  return result;
} 

//-------------Set Parameter------------------------------------
void PndDrcDigiTask::SetParameters(){
  fDetType=1; //  Detector Type =1  
  nRefrac=fGeo->nEV();//1.467;  //Refractive index of SOB   
  fSigmat=0.05;  //Time Resolution is 50 ps ############################
  fVerbose=0;
  fCollectionEff=0.65;//Collection Efficiency 
  fPackingFraction=0.80;//Packing Efficiency 
  fRoughness = 0.001; // 10 A
  fTimeGranularity = 0.089; //[ns] = 89 ps time granularity of the time signal
  
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
  fPixelStep     =  fMcpActiveArea/fNpix;//fPixelSize + 0.5*fPixelGap; 
  
  fPixelSigma 	 =  fGeo->SigmaCharge();
  fDeadTime 	 =  fGeo->DeadTime(); 
  
  fThreshold	 =  0.1; // 10% threshold to detect charge shared hits    
}
//-------------Smear Time------------------------------------
void PndDrcDigiTask::Smear(Double_t& time, Double_t sigt){
   Double_t t=time;
   Double_t dt;
   dt=gRandom->Gaus(0,sigt);
   t +=dt;
   time=t;
   return;
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
