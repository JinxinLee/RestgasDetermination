
// -------------------------------------------------------------------------
// -----                        PndDrc source file                     -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----               Modified 2006++  by Carsten Schwarz             -----
// -----               Modified 2010++  by Maria Patsyuk               -----
// -----                                                               -----
// -------------------------------------------------------------------------


#include "PndGeoDrc.h"
#include "PndDrcPDPoint.h"
#include "PndDrcBarPoint.h"
#include "PndDrcEVPoint.h"
#include "PndGeoDrcPar.h"
#include "PndDetectorList.h"
#include "PndDrc.h"

#include "TString.h"
using std::endl;
using std::cout;

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TGeoMCGeometry.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoBBox.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoTrd2.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TObject.h"
#include "TColor.h"

#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairGeoRootBuilder.h"
#include "PndStack.h"
#include "PndDetectorList.h"

// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRun.h"
//#include "FairRunSim.h"
#include "FairRuntimeDb.h"
#include "PndGeoHandling.h"

#include <cmath>

// -----   Default constructor   -------------------------------------------
PndDrc::PndDrc() 
  : FairDetector("PndDrcDefault",kTRUE),
    fPersistency(kTRUE),
    fpi(TMath::Pi()),			//!
    fzup(-999.),		//!
    fzdown(-999.),
    fradius(-999.),
    fhthick(-999.),
    fpipehAngle(-999.),
    fbbGap(-999.),
    fbbnum(-999.),
    fbarnum(-999.),
    fphi0(-999.),
    fdphi(-999.),
    fbarwidth(-999.),
  
    fGeoH(NULL),
    
    fRunCherenkov(kTRUE),            //!  Switch ON/OFF Cherenkov propagation
    fTrackID(-1),         //!  track index
    fPos(TLorentzVector(0,0,0,0)),             //!  position
    fMom(TLorentzVector(0,0,0,0)),             //!  momentum
    fTime(-1),           //!  time
    fLength(-1),          //!  length 
    fAngIn(0),
    fNBar(0),
    fPosIndex(-1),                 //! 
    volDetector(0),               //!  MC volume ID of drc
    fMass(-1),
     
    fDetEff(0x0),          //!  Detector Efficiency as a function of photon wavelength
    fDetEffAtProduction(kFALSE),
    fTransportEffAtProduction(kFALSE),
    fRand(),
    fCollectionEff(-1),//Collection Efficiency 
 
    fStopTime(kFALSE),
    fPhoMaxTime(-1),

    fTakeRealReflectivity(kFALSE),
    fStopSecondaries(kFALSE),
    fStopChargedTrackAfterDIRC(kFALSE),
    fGeo(new PndGeoDrc()),             //! Pointer to basic DRC geometry data
    
    fPdgCode(-1),
    fThetaC(-1),

    fDrcPDCollection(new TClonesArray("PndDrcPDPoint")),        //! Hit collection
    fDrcEVCollection(new TClonesArray("PndDrcEVPoint")),
    fDrcBarCollection (new TClonesArray("PndDrcBarPoint")),        //! Hit collection in the bar   
    fEventID(0)   
{
  fListOfSensitives.push_back("Sensor");
  if(fVerboseLevel > 0){
    std::cout<<"-I- PndBarrelDIRC: fListOfSensitives contains:";
    for(size_t k=0; k<fListOfSensitives.size(); k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  } 
  
  if(fGeoH == NULL) fGeoH = PndGeoHandling::Instance();
}

// -----   Standard constructor   ------------------------------------------
PndDrc::PndDrc(const char* name, Bool_t active)
  : FairDetector(name, active),
    fPersistency(kTRUE),
    fpi(TMath::Pi()),			//!
    fzup(-999.),		//!
    fzdown(-999.),
    fradius(-999.),
    fhthick(-999.),
    fpipehAngle(-999.),
    fbbGap(-999.),
    fbbnum(-999.),
    fbarnum(-999.),
    fphi0(-999.),
    fdphi(-999.),
    fbarwidth(-999.),
    
    fGeoH(NULL),
 
    fRunCherenkov(kTRUE),            //!  Switch ON/OFF Cherenkov propagation
    fTrackID(-1),         //!  track index
    fPos(TLorentzVector(0,0,0,0)),             //!  position
    fMom(TLorentzVector(0,0,0,0)),             //!  momentu
    fTime(-1),           //!  time
    fLength(-1),          //!  length 
    fAngIn(0),
    fNBar(0),
    fPosIndex(-1),                 //! 
    volDetector(0),               //!  MC volume ID of drc
    fMass(-1),
  
    fDetEff(0x0),          //!  Detector Efficiency as a function of photon wavelength
    fDetEffAtProduction(kFALSE),
    fTransportEffAtProduction(kFALSE),
    fRand(),
    fCollectionEff(-1),//Collection Efficiency 
 
    fStopTime(kFALSE),
    fPhoMaxTime(-1),

    fTakeRealReflectivity(kFALSE),
    fStopSecondaries(kFALSE),

    fStopChargedTrackAfterDIRC(kFALSE),
    fGeo(new PndGeoDrc()),             //! Pointer to basic DRC geometry data
    
    fPdgCode(-1),
    fThetaC(-1),

    fDrcPDCollection(new TClonesArray("PndDrcPDPoint")),        //! Hit collection
    fDrcEVCollection(new TClonesArray("PndDrcEVPoint")),        //! Hit collection
    fDrcBarCollection (new TClonesArray("PndDrcBarPoint")),        //! Hit collection in the bar   
    fEventID(0)   
{
  fListOfSensitives.push_back("Sensor");
  if(fVerboseLevel > 0){
    std::cout<<"-I- PndBarrelDIRC: fListOfSensitives contains:";
    for(size_t k=0; k<fListOfSensitives.size(); k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  }
      
  if(fGeoH == NULL) fGeoH = PndGeoHandling::Instance();
  
}

// -----   Destructor   ----------------------------------------------------
PndDrc::~PndDrc() {
  if (fDrcPDCollection) {
    fDrcPDCollection->Delete();
    delete fDrcPDCollection;
  }
  
  if (fDrcEVCollection) {
    fDrcEVCollection->Delete();
    delete fDrcEVCollection;
  }

  if (fDrcBarCollection) {
    fDrcBarCollection->Delete();
    delete fDrcBarCollection;
  }
    
  if (fGeo) delete fGeo;
  
}

// -----   Public method Intialize   ---------------------------------------
void PndDrc::Initialize() {
  cout << " -I- PndDrc: Intialization started... " << endl;
  FairDetector::Initialize();
  //FairRun       *sim  = FairRun::Instance();
  //FairRuntimeDb *rtdb = sim->GetRuntimeDb();
  //PndGeoDrcPar *par  = (PndGeoDrcPar*)(rtdb->getContainer("PndGeoDrcPar"));

  if (0==gGeoManager) 
    cout << "We do not have gGeoManager" << endl;
  else
    cout << "there is gGeoManager" << endl;
  
    
  cout << "list of sensitives has " << fListOfSensitives.size() << " entries" << endl;
  fGeoH->CreateUniqueSensorId("", fListOfSensitives);
  if(fVerboseLevel>0) fGeoH->PrintSensorNames();

  if (fRunCherenkov==kFALSE) cout << " -I- PndDrc: Switching OFF Cherenkov Propagation" << endl;
 
  // basic DIRC parameters:
  fpi            =  TMath::Pi();
  fzup		 =  fGeo->barBoxZUp();
  fzdown	 =  fGeo->barBoxZDown();
  fradius        =  fGeo->radius();          //radius in the middle of the bar = 50.cm
  fhthick        =  fGeo->barHalfThick();    //half thickness of the bars=1.7/2 cm
  fpipehAngle    =  fGeo->PipehAngle(); 
  fbbGap         =  fGeo->BBoxGap();
  fbbnum         =  fGeo->BBoxNum();
  fbarnum        =  fGeo->barNum();
  fphi0          =  (180.-2.*fpipehAngle)/fbbnum + fpipehAngle;
  fdphi          =  (180.-2.*fpipehAngle)/fbbnum*2.;
  fbarwidth	 =  fGeo->BarWidth();
        
  // create a detector efficiency function:
  if(fDetEffAtProduction == kTRUE){
    fCollectionEff=0.65;//Collection Efficiency 
   
    // quantum efficiency data from Alex Britting, Jan 25, 2011
    // unit is percent
    // first value is at 200 nm, last at 700 nm
    // credible range start around 250nm, >= 280nm to be safe

    Int_t credibleLimit=280;   
    Double_t efficiency[501] = {231.84,615.36,657.4,258.78,9839.92,44.67,67.87,51.01,41.49,5.36,49.4,2.13,35.49,8.66,5.03,7.51,13.27,18.71,3.92,3.66,8.2,0.56,7.68,2.87,10.06,3.47,3.39,6.99,6.01,4.92,6.25,5.97,6.92,8.29,10.45,8.68,8.6,9.79,11.76,9.53,10.98,9.9,10.97,11.31,10.88,10.78,12.16,12.38,12.37,13.04,12.36,13.18,13.7,13.85,13.66,13.98,14.55,14.93,14.82,14.97,14.98,15.14,15.35,15.37,15.43,15.49,15.59,15.84,15.84,15.92,16.01,16.22,16.41,16.42,16.52,16.86,17.1,17.17,17.22,17.46,17.79,17.99,18.13,18.33,18.34,18.53,18.72,18.95,19.02,19.15,19.28,19.45,19.66,19.69,19.77,19.73,19.95,19.98,20.17,20.29,20.33,20.37,20.47,20.48,20.57,20.75,20.8,20.84,20.86,20.88,21.0,21.06,21.0,21.06,21.06,21.04,21.1,21.14,21.08,21.17,21.3,21.38,21.49,21.58,21.69,21.77,21.87,22.02,22.13,22.29,22.35,22.45,22.53,22.55,22.64,22.67,22.73,22.74,22.71,22.79,22.76,22.77,22.76,22.75,22.78,22.7,22.68,22.72,22.66,22.64,22.7,22.67,22.71,22.67,22.75,22.77,22.83,22.84,22.93,22.97,23.0,23.08,23.16,23.27,23.25,23.37,23.44,23.49,23.55,23.52,23.58,23.64,23.63,23.58,23.64,23.63,23.62,23.64,23.63,23.66,23.59,23.59,23.56,23.58,23.63,23.57,23.66,23.62,23.67,23.64,23.54,23.57,23.51,23.53,23.45,23.3,23.41,23.25,23.21,23.08,23.01,22.92,22.9,22.76,22.76,22.61,22.53,22.48,22.39,22.29,22.24,22.2,22.12,22.07,21.96,21.89,21.87,21.76,21.74,21.58,21.49,21.48,21.37,21.29,21.2,21.17,21.03,20.98,20.92,20.85,20.76,20.69,20.58,20.56,20.47,20.37,20.32,20.24,20.13,20.08,19.9,19.84,19.77,19.69,19.63,19.51,19.41,19.27,19.06,19.01,18.87,18.7,18.49,18.41,18.17,17.98,17.84,17.69,17.5,17.25,17.15,16.98,16.79,16.66,16.48,16.32,16.19,16.02,15.88,15.77,15.67,15.5,15.39,15.23,15.09,15.04,14.92,14.75,14.7,14.5,14.45,14.34,14.25,14.16,14.13,14.0,13.92,13.84,13.76,13.73,13.61,13.54,13.52,13.45,13.41,13.39,13.31,13.22,13.17,13.13,13.06,13.2,13.09,12.97,12.92,12.73,12.65,12.4,12.22,12.02,11.79,11.59,11.33,11.03,10.68,10.46,10.14,9.88,9.62,9.36,9.14,8.87,8.63,8.51,8.24,8.07,7.88,7.77,7.65,7.52,7.35,7.27,7.21,7.1,6.92,6.89,6.79,6.74,6.56,6.54,6.5,6.39,6.33,6.25,6.27,6.14,6.06,6.04,6.01,5.91,5.89,5.79,5.75,5.75,5.67,5.61,5.51,5.52,5.43,5.43,5.34,5.31,5.35,5.23,5.2,5.14,5.11,5.11,5.01,4.98,4.93,4.99,4.89,4.82,4.87,4.8,4.7,4.65,4.65,4.61,4.49,4.56,4.44,4.42,4.44,4.35,4.35,4.27,4.29,4.19,4.13,4.08,4.02,4.07,3.92,3.95,3.88,3.82,3.86,3.74,3.71,3.66,3.72,3.62,3.55,3.56,3.57,3.45,3.38,3.36,3.36,3.28,3.25,3.19,3.26,3.13,3.17,3.15,3.04,2.98,2.93,2.98,2.9,2.89,2.9,2.81,2.74,2.81,2.68,2.73,2.7,2.57,2.58,2.55,2.55,2.37,2.39,2.39,2.44,2.37,2.26,2.27,2.27,2.23,2.26,2.14,2.08,2.15,2.06,2.09,2.04,2.0,1.95,2.02,1.87,1.9,1.8,1.87,1.85,1.87,1.81,1.86,1.74,1.74,1.63,1.59,1.5,1.5,1.44,1.47,1.32,1.24,1.28,1.19,1.21,1.21,1.1,1.1,1.05,1.06,0.94,0.92,0.87,0.92,0.81,0.86,0.78,0.77,0.8,0.67,0.7,0.81,0.61,0.64,0.71,0.66,0.67,0.68,0.69,0.68,0.73};

    Double_t lambda[1000];
    for(Int_t i=0; i<1000; i++) lambda[i] = i;
    
    // still need to convert from percent and cut values below credible limit
    for (Int_t iBin=0;iBin<1000;iBin++){
      if (iBin<credibleLimit || iBin > 700){
	fEfficiencyR[iBin]=0.;
      }else{
	// total detector efficiency
	fEfficiencyR[iBin]=efficiency[iBin-200]/100.*fCollectionEff;
      }
    }   
    
    fDetEff = new TGraph(1000, lambda,fEfficiencyR);
  } 
  
  cout << " -I- PndDrc: Intialization successfull" << endl;
}

// -------------------------------------------------------------------------
void PndDrc::BeginEvent() {
  if (fVerboseLevel >0) cout<<" \n\n>>>>>>>>>>>>>>>>>>>>new event in the Barrel DIRC" <<endl;
  fEventID++;
}

//TH1F *hSpec = new TH1F("hSpec","hSpec",1000,1,10);

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndDrc::ProcessHits(FairVolume* vol) {
  TString nam =vol->GetName();
  Int_t num = vol->getMCid();
       
  fEventID = gMC->CurrentEvent();
  fPdgCode = gMC->TrackPid();

  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();	  
  fTime    = gMC->TrackTime() * 1.0e09;
  fLength  = gMC->TrackLength();
  gMC->TrackPosition(fPos);
  gMC->TrackMomentum(fMom);

  // if(nam.BeginsWith("DrcBar") && gMC->IsTrackEntering() && fPdgCode == 50000050){
  //   hSpec->Fill(fMom.Vect().Mag()*1e9);
  //   gMC->StopTrack();
  // }

  
  // // print out info about the charged particle:
  // if(gMC->GetStack()->GetCurrentParentTrackNumber()==0 && fPdgCode == 11){
  //   Int_t nproc = gMC->StepProcesses(fProc);
  //   cout<<"-I- PndDrc:"<<endl;
  //   for(Int_t ii=0; ii<nproc; ii++){
  //     cout<<"track "<<fPdgCode<<" number "<<gMC->GetStack()->GetCurrentTrackNumber()<<" mother id = "<<gMC->GetStack()->GetCurrentParentTrackNumber()<<": Z pos "<<fPos.Z()<<", R =  "<<sqrt(pow(fPos.X(),2) + pow(fPos.Y(),2))<<", "<<ii<<" - "<<fProc[ii]<<", "<<TMCProcessName[fProc[ii]]<<endl;
  //   }
  // }
 
  //stop secondaries so that they do not produce Cherenkov photons
  if(fStopSecondaries){
    if(fPdgCode != 50000050){
      if(gMC->GetStack()->GetCurrentParentTrackNumber() != -1 ){
        if(gMC->IsNewTrack()) gMC->StopTrack();
      }  
    }
  }

  // stop the track after the DIRC:
  if(fStopChargedTrackAfterDIRC){
    if(fPdgCode != 50000050 && gMC->IsTrackExiting() && num == gMC->VolId("DrcBarSensor")){
      cout<<"track is stopped after the Barrel DIRC"<<endl;
      gMC->StopTrack();
    }
    if(nam.BeginsWith("DrcEVSensor") && gMC->IsTrackExiting() && fMom.Z() > 0.) gMC->StopTrack();
  }
  
  if (fPdgCode == 50000050){
    if (fRunCherenkov==kFALSE ) { //|| fabs(fMom.Vect().Mag()*1.0E9-3.18)>0.2
      if (fVerboseLevel >0) cout<< "Photon killed" << endl;
      gMC->StopTrack();
    }  
          
    // apply detector efficiency at the production stage:    
    if(fDetEffAtProduction && gMC->IsNewTrack()){
      Double_t lambda = 197.0*2.0*fpi/(fMom.Vect().Mag()*1.0E9);          
      Double_t ra = fRand.Uniform(0., 1.);
      if(ra > fDetEff->Eval(lambda)) gMC->StopTrack();
      //if(ra > fEfficiencyR[(int)lambda]) gMC->StopTrack();
    }
    
    //if the photon goes backward through the lens, stop it
    if(gMC->IsTrackExiting() && nam.Contains("LENS")){
      if(fMom.Z() > 0.) gMC->StopTrack();
    }
    
    if(fOptionForLUT && fMom.Z() > 0.) gMC->StopTrack();
 
    // apply transport efficiency at production stage (Maria Patsyuk 20.04.2012):
    if(fTransportEffAtProduction && gMC->IsNewTrack()){      
      Double_t lam_tr = (197.0*2.0*fpi/(fMom.Vect().Mag()*1.0E9)) / 1000.;

      // current volume (should be the radiator bar)
      Int_t barId = fGeoH->GetShortID(gMC->CurrentVolPath());  
      //calculate the number of bounces:
      Int_t NbouncesX, NbouncesY;
      Double_t angleX, angleY;
      // photon initial direction
      TVector3 PphoInitBar = fGeoH->MasterToLocalShortId(fMom.Vect(), barId)- fGeoH->MasterToLocalShortId(TVector3(0.,0.,0.), barId); // vector
      NumberOfBounces(fPos.Vect(), PphoInitBar, barId, &NbouncesX, &NbouncesY, &angleX, &angleY);
      // calculate the bounce probability
      Double_t n_quartz = sqrt(1. + (0.696*lam_tr*lam_tr/(lam_tr*lam_tr-pow(0.068,2))) + (0.407*lam_tr*lam_tr/(lam_tr*lam_tr-pow(0.116,2))) + 0.897*lam_tr*lam_tr/(lam_tr*lam_tr-pow(9.896,2)));
      Double_t bounce_probX = 1. - pow(4.*fpi*cos(angleX)*fGeo->Roughness()*n_quartz/lam_tr,2); 
      Double_t bounce_probY = 1. - pow(4.*fpi*cos(angleY)*fGeo->Roughness()*n_quartz/lam_tr,2);
      
      Double_t TotalTrProb = pow(bounce_probX, (Int_t)NbouncesX)*pow(bounce_probY, (Int_t)NbouncesY);
      Double_t ra_tr = fRand.Uniform(0., 1.);
      if(ra_tr > TotalTrProb) gMC->StopTrack();   
    }

    // kill photons older than fPhoMaxTime:  
    if (fStopTime == kTRUE && gMC->TrackTime()*1.0e09 > fPhoMaxTime){
      gMC->StopTrack();
    }  
      
    if(gMC->IsTrackEntering() && num == gMC->VolId("DrcEVSensor")){
      gMC->TrackMomentum(fMomAtEV); 
    }

    if(gMC->IsTrackEntering()){
      if(gMC->IsNewTrack()) fTimeStart = fTime; // charged particle time at entrance of radiator bar         
     
      // if(nam.BeginsWith("DrcLENS1Sensor")){  //DrcEntranceBox
      //   // save the direction of the photon when it enters EntranceBox (for LUT generation) 
      //   Double_t nx,ny,nz;
      //   bool bres = gMC->CurrentBoundaryNormal(nx,ny,nz);
      //   AddEVHit(fTrackID, 9375, fPos.Vect(), fMom.Vect().Unit(),
      // 		fTime, fLength, fPdgCode,
      // 		 fEventID, fTimeStart, fTimeAtEVEntrance, fVeloPhoton, TVector3(nx,ny,nz));
      // }
     
      if(nam.BeginsWith("DrcEVSensor")){
	if(fTimeAtEVEntrance ==0.){
	  fTimeAtEVEntrance= gMC->TrackTime()*1.0e09;
	  fLengthEV=fLength;
	}        
	fVeloPhoton=fLength/(fTime-fTimeStart);

	Double_t nmast[3];
	//bool bres = gMC->CurrentBoundaryNormal(nmast[0],nmast[1],nmast[2]); //[R.K. 01/2017] unused variable?
	//TVector3 lnorm = fGeoH->MasterToLocalShortId(TVector3(nx,ny,nz), fGeoH->GetShortID(gMC->CurrentVolPath()));
	
	Double_t result[3];
	gGeoManager->MasterToLocalVect(nmast, result);
	TVector3 lnorm = TVector3(result[0],result[1],result[2]);
	  
	//if(bres)
	AddEVHit(fTrackID, 0, fPos.Vect(), fMom.Vect(),
		 fTime, fLength, fPdgCode,
		 fEventID, fTimeStart, fTimeAtEVEntrance, fVeloPhoton, lnorm);
	
	fTimeAtEVEntrance = 0.0;
      }
    } 
   
    // if a photon is exiting the readout bar end
    if (gMC->IsTrackExiting() && num == gMC->VolId("DrcBarSensor") && fPos.Z()<-118.99){
      fTimeAtEV = gMC->TrackTime()*1.0e09;
      //cout<<"time at EV = "<<fTimeAtEV<<endl;
    } 
   
    if (gMC->IsTrackEntering()){
      if (nam.BeginsWith("DrcCathodeSensor")){ 
	if(0==fGeoH) {
	  std::cout<<" -E- No PndGeoHandling loaded."<<std::endl;
	  abort();
	}   
	Int_t sensorId = fGeoH->GetShortID(gMC->CurrentVolPath());
	Int_t mcpId, prismId;
	sscanf(gMC->CurrentVolPath(), "/cave_1/BarrelDIRC_0/DrcPDbase_%d/DrcMCP_%d", &prismId, &mcpId);
	if(fTrackID>-2){
	  AddHit(fTrackID, sensorId, mcpId,
		 fPos.Vect(),fMom.Vect(),fMomAtEV.Vect(), fTimeAtEV,
		 fTime, fLength, fPdgCode, fEventID);
	}

	gMC->StopTrack();
      }
      PndStack* stack = (PndStack*) gMC->GetStack();
      stack->AddPoint(kDRC);
    } 
  }

  
  if(gMC->TrackCharge()!=0 || fOptionForLUT){
    if(nam.BeginsWith("DrcBar") && gMC->IsTrackEntering()){

      bool bpass = true;
      // if(fDrcBarCollection->GetEntriesFast()>0){
      // 	PndDrcBarPoint *tBarPoint = (PndDrcBarPoint*) fDrcBarCollection->At(fDrcBarCollection->GetEntriesFast()-1);
      // 	if(tBarPoint->GetTrackID()==fTrackID && fLength - tBarPoint->GetLength()<0.1) bpass = false;
      // }
            
      if(bpass && nam.BeginsWith("DrcBarSensor") ){
	Int_t s=0, b=0; //side and bar
	fNBar=0;
	TString path = gMC->CurrentVolPath();
	if (fVerboseLevel >1) cout<< "Volume: " << nam << endl;
	sscanf(path, "/cave_1/BarrelDIRC_0/DrcBarBox_%d/DrcBarBoxCover_0/DrcBarBoxAir_0/DrcBarSensor_%d", &s, &b);
	
	if(s < 17) fNBar = s*10 + b;
	else std::cout<<"Error: Wrong BarBox Id "<< s <<std::endl;
	
	TVector3 barMom =  fMom.Vect();
	Double_t fP = barMom.Mag();
	fMass = gMC->TrackMass();
	Double_t fEnergy = TMath::Sqrt(fP*fP + fMass*fMass);
	
	if ( fP == 0. || fabs(1./(fGeo->nQuartz()*(fP/fEnergy)))>1){
	  fThetaC = -1;
	}else{ 
	  fThetaC = acos(1/(fGeo->nQuartz()*(fP/fEnergy)));
	}
	
	AddBarHit(fTrackID, fGeoH->GetShortID(path), fPos.Vect(), barMom,
		  fTime, fLength, fPdgCode, fThetaC, fNBar, fEventID, fMass);
	
	PndStack* stack = (PndStack*) gMC->GetStack();
	stack->AddPoint(kDRC);
      }
    }
  }
  
  ResetParameters();
  return kTRUE; 
}

void PndDrc::FinishPrimary(){}

//------   Find Nubmer of Bounces     -----------------------------------------
void PndDrc::NumberOfBounces(TVector3 start, TVector3 dir, Int_t barId, Int_t *n1, Int_t *n2, Double_t *alpha1, Double_t *alpha2) {
  // start - photon production point in global coord system
  // dir - photon direction in bar coord system
    
  // calculates the number of bounces in x and y direction and reflection angles in these directions.
        
  // Find coordinates of X0, Y0:
  Double_t Z0, X0, Y0;
  if(dir.Theta() < 3.1415/2.){
    Z0 = -(fabs(fzup) + 2.*fzdown - start.Z());
  }
  if(dir.Theta() >= 3.1415/2.){
    Z0 = -(start.Z() -  fzup);
  }    
  X0 = Z0*tan(dir.Theta())*cos(dir.Phi());
  Y0 = Z0*tan(dir.Theta())*sin(dir.Phi());
  //cout<<"-I- NumberOfBounces: X0 = "<<X0<<", Y0 = "<<Y0<<endl;
    
  // Find the start position of the photon with respect to the middle of the bar:
  TVector3 startLocal = fGeoH->MasterToLocalShortId(start, barId); // point
    
  // Find the number of bounces in each direction       
  Double_t N1, N2;    
  FindOutPoint(X0, startLocal.X() + fbarwidth/2., fbarwidth, &N1, 0);
  FindOutPoint(Y0, startLocal.Y() + fhthick,      2.*fhthick, &N2, 0);    
  //cout<<"-I- NumberOfBounces: N1 = "<<N1<<", N2 = "<<N2<<endl;
    
  *n1 = (Int_t)N1;
  *n2 = (Int_t)N2;
    
  // calculate the reflection angles in x and y directions:
  TVector3 up_down;
  up_down.SetXYZ(0.,1.,0.);
  TVector3 left_right;
  left_right.SetXYZ(1.,0.,0.);
  Double_t angle1 = dir.Angle(left_right);
  if(angle1 > fpi/2.){angle1 = fpi - dir.Angle(left_right);}
  Double_t angle2 = dir.Angle(up_down);
  if(angle2 > fpi/2.){angle2 = fpi - dir.Angle(up_down);}
  *alpha1 = angle1;
  *alpha2 = angle2;
  //cout<<"-I- NumberOfBounces: angle1 = "<<angle1<<", angle2 = "<<angle2<<endl;
}

//-----------------------------------------------------------------------------
Double_t PndDrc::FindOutPoint(Double_t x0, Double_t xEn, Double_t a, Double_t *NN, Bool_t print) {
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

// -----   Public method EndOfEvent   -----------------------------------------
void PndDrc::EndOfEvent() {
  if (fVerboseLevel)  Print();  
  Reset(); 
}

void PndDrc::FinishRun(){
  //  hSpec->Draw();
}

// -----   Public method Register   -------------------------------------------
void PndDrc::Register() {
  FairRootManager::Instance()->Register("DrcBarPoint","Drc", fDrcBarCollection, fPersistency);
  FairRootManager::Instance()->Register("DrcEVPoint","Drc", fDrcEVCollection, fPersistency);
  FairRootManager::Instance()->Register("DrcPDPoint","Drc", fDrcPDCollection, fPersistency);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndDrc::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fDrcPDCollection;
  if (iColl == 1) return fDrcEVCollection;
  if (iColl == 2) return fDrcBarCollection;
   
  return NULL;
}

// -----   Public method Print   ----------------------------------------------
void PndDrc::Print() const {
  Int_t nPDHits = fDrcPDCollection->GetEntriesFast();
  Int_t nBarHits = fDrcBarCollection->GetEntriesFast();
  Int_t nEVHits = fDrcEVCollection->GetEntriesFast();
    
  cout << "-I- PndDrc: " << nPDHits << " points registered in the photodetector for this event." << endl;
  cout << "-I- PndDrc: " << nEVHits << " points registered in the expansion volume for this event." << endl;
  cout << "-I- PndDrc: " << nBarHits << " points registered in the bar for this event." 	<< endl;
    
  if (fVerboseLevel>1){
    for (Int_t i=0; i<nPDHits; i++) (*fDrcPDCollection)[i]->Print();
    for (Int_t i=0; i<nEVHits; i++) (*fDrcEVCollection)[i]->Print();
    for (Int_t i=0; i<nBarHits; i++) (*fDrcBarCollection)[i]->Print();   
  }
}

// -----   Public method Reset   ----------------------------------------------
void PndDrc::Reset() {
  fDrcPDCollection->Delete();
  fDrcEVCollection->Delete();
  fDrcBarCollection->Delete();   
  fPosIndex = 0;
}

// -----   Public method CopyClones   -----------------------------------------
void PndDrc::CopyClones(TClonesArray* clPD1, TClonesArray* clPD2,TClonesArray* clBar1, TClonesArray* clBar2, Int_t offset ) {
  Int_t nPDEntries = clPD1->GetEntriesFast();
  cout << "-I- PndDrc: " << nPDEntries << " entries to add." << endl;
  TClonesArray& clrefPD = *clPD2;

  Int_t nBarEntries = clBar1->GetEntriesFast();
  cout << "-I- PndDrc: " << nBarEntries << " entries to add." << endl;
  TClonesArray& clrefBar = *clBar2;
  
  PndDrcPDPoint* oldpointPD = NULL;
  PndDrcBarPoint* oldpointBar = NULL;
  
  for (Int_t i=0; i<nPDEntries; i++) {
    oldpointPD = (PndDrcPDPoint*) clPD1->At(i);
    Int_t indexPD = oldpointPD->GetTrackID() + offset;
    oldpointPD->SetTrackID(indexPD);
    new (clrefPD[fPosIndex]) PndDrcPDPoint(*oldpointPD);
    fPosIndex++;
  }

  for (Int_t i=0; i<nBarEntries; i++) {
    oldpointBar = (PndDrcBarPoint*) clBar1->At(i);
    Int_t indexBar = oldpointBar->GetTrackID() + offset;
    oldpointBar->SetTrackID(indexBar);
    new (clrefBar[fPosIndex]) PndDrcBarPoint(*oldpointBar);
    fPosIndex++;
  }
  
  cout << " -I- PndDrc: " << clPD2->GetEntriesFast() << " merged entries." << endl;
  cout << " -I- PndDrc: " << clBar2->GetEntriesFast() << " merged entries." << endl;
}

// -----   Public method ConstructGeometry  -----------------------------------
void PndDrc::ConstructGeometry(){
  cout<< "                                               " << endl;
  cout<< " =======  DRC::  ConstructGeometry()  ======== " << endl;
  cout<< " ============================================= " << endl;
  
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")){
    ConstructRootGeometry();
  } else{
    std::cout<<"Geometry format not supported!"<<std::endl;
  }
  
  cout<< " ============================================= " << endl;
}

// -----   Public Method Construct Optical Geometry ---------------------------
void PndDrc::ConstructOpGeometry() {
  cout<< " ==================================================== " << endl;
  cout<< " =======  DRC::  ConstructOpticalGeometry()  ======== " << endl; 

  // ideal reflectivity:
  Int_t npoints_i = 2;  
  Double_t ephoton_i[npoints_i];
  ephoton_i[0] = 1.907*1.0e-09;  // 1 eV
  ephoton_i[1] = 6.199*1.0e-09; // 10 eV  
  Double_t reflectivity_i[npoints_i];
  reflectivity_i[0] = 1.;
  reflectivity_i[1] = 1.;
  
  // black reflectivity:
  Double_t reflectivity_b[npoints_i];
  reflectivity_b[0] = 0.;
  reflectivity_b[1] = 0.;
  
  // real reflectivity (added 18.05.2011, measured by Jerry for BABAR):
  Int_t npoints_r = 46;
  Double_t ephoton_r[npoints_r];
  ephoton_r[0]  = 1.907*1.0e-09; //eV
  ephoton_r[1]  = 1.937*1.0e-09;
  ephoton_r[2]  = 1.968*1.0e-09;
  ephoton_r[3]  = 2.000*1.0e-09;
  ephoton_r[4]  = 2.033*1.0e-09;
  ephoton_r[5]  = 2.066*1.0e-09;
  ephoton_r[6]  = 2.101*1.0e-09;
  ephoton_r[7]  = 2.138*1.0e-09;
  ephoton_r[8]  = 2.175*1.0e-09;
  ephoton_r[9]  = 2.214*1.0e-09;
  ephoton_r[10] = 2.254*1.0e-09;
  ephoton_r[11] = 2.296*1.0e-09;
  ephoton_r[12] = 2.339*1.0e-09;
  ephoton_r[13] = 2.384*1.0e-09;
  ephoton_r[14] = 2.431*1.0e-09;
  ephoton_r[15] = 2.480*1.0e-09;
  ephoton_r[16] = 2.530*1.0e-09;
  ephoton_r[17] = 2.583*1.0e-09;
  ephoton_r[18] = 2.638*1.0e-09;
  ephoton_r[19] = 2.695*1.0e-09;
  ephoton_r[20] = 2.755*1.0e-09;
  ephoton_r[21] = 2.818*1.0e-09;
  ephoton_r[22] = 2.883*1.0e-09;
  ephoton_r[23] = 2.952*1.0e-09;
  ephoton_r[24] = 3.024*1.0e-09;
  ephoton_r[25] = 3.100*1.0e-09;
  ephoton_r[26] = 3.179*1.0e-09;
  ephoton_r[27] = 3.263*1.0e-09;
  ephoton_r[28] = 3.351*1.0e-09;
  ephoton_r[29] = 3.444*1.0e-09;
  ephoton_r[30] = 3.542*1.0e-09;
  ephoton_r[31] = 3.647*1.0e-09;
  ephoton_r[32] = 3.757*1.0e-09;
  ephoton_r[33] = 3.875*1.0e-09;
  ephoton_r[34] = 3.999*1.0e-09;
  ephoton_r[35] = 4.133*1.0e-09;
  ephoton_r[36] = 4.275*1.0e-09;
  ephoton_r[37] = 4.428*1.0e-09;
  ephoton_r[38] = 4.592*1.0e-09;
  ephoton_r[39] = 4.769*1.0e-09;
  ephoton_r[40] = 4.959*1.0e-09;
  ephoton_r[41] = 5.166*1.0e-09;
  ephoton_r[42] = 5.391*1.0e-09;
  ephoton_r[43] = 5.636*1.0e-09;
  ephoton_r[44] = 5.904*1.0e-09;
  ephoton_r[45] = 6.199*1.0e-09;
  
  Double_t reflectivity_r[npoints_r];
  reflectivity_r[0]  = 0.870;
  reflectivity_r[1]  = 0.880;
  reflectivity_r[2]  = 0.885;
  reflectivity_r[3]  = 0.890;
  reflectivity_r[4]  = 0.895;
  reflectivity_r[5]  = 0.900;
  reflectivity_r[6]  = 0.905;
  reflectivity_r[7]  = 0.910;
  reflectivity_r[8]  = 0.915;
  reflectivity_r[9]  = 0.920;
  reflectivity_r[10] = 0.923;
  reflectivity_r[11] = 0.925;
  reflectivity_r[12] = 0.926;
  reflectivity_r[13] = 0.928;
  reflectivity_r[14] = 0.930;
  reflectivity_r[15] = 0.935;
  reflectivity_r[16] = 0.936;
  reflectivity_r[17] = 0.937;
  reflectivity_r[18] = 0.938;
  reflectivity_r[19] = 0.940;
  reflectivity_r[20] = 0.940;
  reflectivity_r[21] = 0.939;
  reflectivity_r[22] = 0.938;
  reflectivity_r[23] = 0.938;
  reflectivity_r[24] = 0.937;
  reflectivity_r[25] = 0.937;
  reflectivity_r[26] = 0.936;
  reflectivity_r[27] = 0.935;
  reflectivity_r[28] = 0.934;
  reflectivity_r[29] = 0.932;
  reflectivity_r[30] = 0.930;
  reflectivity_r[31] = 0.928;
  reflectivity_r[32] = 0.926;
  reflectivity_r[33] = 0.924;
  reflectivity_r[34] = 0.922;
  reflectivity_r[35] = 0.920;
  reflectivity_r[36] = 0.910;
  reflectivity_r[37] = 0.905;
  reflectivity_r[38] = 0.895;
  reflectivity_r[39] = 0.890;
  reflectivity_r[40] = 0.885;
  reflectivity_r[41] = 0.860;
  reflectivity_r[42] = 0.840;
  reflectivity_r[43] = 0.820;
  reflectivity_r[44] = 0.800;
  reflectivity_r[45] = 0.780;
  
  gMC->DefineOpSurface("LensSurface",  kGlisur, kDielectric_dielectric, kGround, 0.0);
  gMC->DefineOpSurface("MirrSurface",  kGlisur, kDielectric_metal, kPolished, 0.0);
  gMC->DefineOpSurface("EVSurface",    kGlisur, kDielectric_metal, kPolished, 0.0);
  gMC->DefineOpSurface("BlackSurface", kGlisur, kDielectric_dielectric, kPolished, 0.0);
 

  // Double_t refractiveIndex[npoints_i];
  // refractiveIndex[0] = 1.2;
  // refractiveIndex[1] = 1.2;
  // Double_t abs_i[npoints_i];
  // abs_i[0] = 1000.;
  // abs_i[1] = 1000.;

  // // gMC->SetCerenkov(gMC->MediumId("BK7G18"),npoints_i, ephoton_i,abs_i , reflectivity_i, refractiveIndex); 

  // gMC->SetMaterialProperty("LensSurface", "RINDEX", npoints_i, ephoton_i, refractiveIndex);  
  // gMC->SetMaterialProperty("LensSurface", "REFLECTIVITY", npoints_i, ephoton_i, reflectivity_i);
  // gMC->SetSkinSurface("Lens3Sur", "DrcLENS3Sensor", "LensSurface");
  // return;

  gMC->SetMaterialProperty("BlackSurface",  "REFLECTIVITY", npoints_i, ephoton_i, reflectivity_b);

  if(fTakeRealReflectivity == kFALSE){
    gMC->SetMaterialProperty("MirrSurface", "REFLECTIVITY", npoints_i, ephoton_i, reflectivity_i);
  }else{
    gMC->SetMaterialProperty("MirrSurface", "REFLECTIVITY", npoints_r, ephoton_r, reflectivity_r);
  } 

  gMC->SetBorderSurface("BarMirrorSurface", "DrcMirror", 0, "DrcBarBoxAir", 0, "MirrSurface");

  if(fSetBlackLens == kTRUE){ 
    for(Int_t i=0; i<3; i++){ //fGeo->barNum()
      gMC->SetBorderSurface("Lens1AirSurface", "DrcLENS1Sensor", i, "DrcEntrance", 0, "BlackSurface"); 
      gMC->SetBorderSurface("Lens2AirSurface", "DrcLENS2Sensor", i, "DrcEntrance", 0, "BlackSurface");
      //gMC->SetBorderSurface("Lens3AirSurface", "DrcLENS3Sensor", i, "DrcEntrance", 0, "BlackSurface");
    }   
    //gMC->SetBorderSurface("BarboxWindowAirSurface", "DrcBarboxWindowSensor", 0, "BarrelDIRC", 0, "EVSurface");
    //gMC->SetBorderSurface("EVGreaseAirSurface", "DrcEVgrease", 0, "BarrelDIRC", 0, "EVSurface");
  }

  //only direct  
  //gMC->SetBorderSurface("EVAirSurface", "DrcEVSensor",  0, "DrcEVCoverSensor", 0, "BlackSurface");

  gMC->SetSkinSurface("AirMirrorSurface", "DrcMirror", "MirrSurface");

  cout<<" =======  DRC::ConstructOpGeometry -> Finished! ====== "<< endl;     
}  

// -----   Public method CheckIfSensitive   --------------------------------------
bool PndDrc::CheckIfSensitive(std::string name) {
  for (size_t i = 0; i < fListOfSensitives.size(); i++){
    if (name.find(fListOfSensitives[i]) != std::string::npos)    
      return true;
  }
  return false;
}

// -----   Private method AddHit   --------------------------------------------
PndDrcPDPoint* PndDrc::AddHit(Int_t trackID, Int_t copyNo, Int_t mcpId, TVector3 pos, TVector3 mom, TVector3 momAtEV, Double_t timeAtEV, Double_t time, Double_t length, Int_t pdgCode, Int_t eventID) {
  TClonesArray& clrefPD = *fDrcPDCollection;
  Int_t size = clrefPD.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- PndDrc: Adding PD Point at (" << pos.X() << ", " << pos.Y() 
	 << ", " << pos.Z() << ") cm, detector " << copyNo << ", track "
	 << trackID <<" event "<<eventID << "  barId "<< fDrcBarCollection->GetEntriesFast()-1 <<endl;
  return new(clrefPD[size]) PndDrcPDPoint(trackID,
					  copyNo,
					  mcpId,
					  fDrcBarCollection->GetEntriesFast()-1,
					  pos, 
					  mom,
					  momAtEV, 
					  timeAtEV,
					  time, 
					  length, 
					  pdgCode,
					  eventID);

}

PndDrcEVPoint* PndDrc::AddEVHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Int_t eventID, Double_t timestart,Double_t timestartEV,Double_t VeloPhoton,TVector3 normal) {
  TClonesArray& clrefEV = *fDrcEVCollection;
  Int_t size = clrefEV.GetEntriesFast();
  if (fVerboseLevel>2) 
    cout << "-I- PndDrc: Adding EV Point at (" << pos.X() << ", " << pos.Y() 
      	 << ", " << pos.Z() << ") cm, detector " << copyNo << ", track "
      	 << trackID <<" event "<<eventID << endl;
  return new(clrefEV[size]) PndDrcEVPoint(trackID, 
					  copyNo, 
					  pos, 
					  mom, 
					  time, 
					  length, 
					  pdgCode,
					  eventID,
                                          timestart,
                                          timestartEV,
                                          VeloPhoton,normal);

}

PndDrcBarPoint* PndDrc::AddBarHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t BarId, Int_t eventID, Double_t mass) {
  TClonesArray& clrefBar = *fDrcBarCollection;
  Int_t size = clrefBar.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- PndBarDrc: Adding Bar Point at (" << pos.X() << ", " << pos.Y() 
	 << ", " << pos.Z() << ") cm, detector " << copyNo << ", track "
	 << trackID <<" event "<<eventID << "  PDG "<<pdgCode<< endl;
  return new(clrefBar[size]) PndDrcBarPoint(trackID, 
					    copyNo,
					    pos, 
					    mom, 
					    time, 
					    length, 
					    pdgCode,
					    thetaC,
					    BarId,
					    eventID, 
					    mass);
}

ClassImp(PndDrc)
