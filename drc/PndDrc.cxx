
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
#include "PndGeoDrcPar.h"
#include "PndDetectorList.h"
#include "PndDrc.h"

#include "TString.h"
//#include <iostream>
using std::endl;
using std::cout;

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
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

#include <cmath>

// -----   Default constructor   -------------------------------------------
PndDrc::PndDrc() 
 :  FairDetector("PndDrcDefault",kTRUE),
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
    flside(-999.),
    fbarwidth(-999.),
 
    fRunCherenkov(kTRUE),            //!  Switch ON/OFF Cherenkov propagation
    fTrackID(-1),         //!  track index
    fCurrentTrackID(-1),         //!  track index
    fCopyNo(-1),          //!  volume id
    fPos(TLorentzVector(0,0,0)),             //!  position
    fMom(TLorentzVector(0,0,0)),             //!  momentum
    fTime(-1),           //!  time
    fLength(-1),          //!  length 
    fAngIn(0),
    fNBar(0),
    fPosIndex(-1),                 //! 
    volDetector(0),               //!  MC volume ID of drc
    fMass(-1),
    fMom1(TLorentzVector(0,0,0)), 
    fMom2(TLorentzVector(0,0,0)),  //! for transport efficiency calculation
    fPos2(TLorentzVector(0,0,0)),  //! for transport efficiency calculation
    fBarEnd(0),
    fMirrorGap(0),
  
  // for efficiency calculation:
    //fLambda({0}),
    //fEfficiency({0}),
    //fEfficiencyR({0}),
    fLambdaMin(0.),
    fLambdaMax(0.),
    fLambdaStep(0.),
    fAngleStep(0.),
    fLambdaPoints(0),
  
  // used in ProcessHits function:
    fbarID(0),	   //!  ID number of DrcBarSensors
    fpdID(0),		   //!  ID number of DrcPdSensor
    flens3ID(0),	   //!  ID number of third lenses
    flens2ID(0),
    flens1ID(0),
    fbboxID(0),      	   //!  ID number of DrcBarBoxes
    fevID(0),		   //!  ID number of Expansion Volume
  
    fDetEff(0x0),          //!  Detector Efficiency as a function of photon wavelength
    fDetEffAtProduction(kFALSE),
    fTransportEffAtProduction(kFALSE),
    frand(),
    fLastTrackID(-1),
    fCollectionEff(-1),//Collection Efficiency 
    fPackingFraction(-1),//Packing Efficiency 
 
    fStopTime(kFALSE),
    fPhoMaxTime(-1),
    fTakeDirect(kFALSE), 
    fTakeReflected(kFALSE),     

    fFocusing(-1), 
    fTakeRealReflectivity(kFALSE),
    fStopSecondaries(kFALSE),

    fGeo(new PndGeoDrc()),             //! Pointer to basic DRC geometry data

    fPdgCode(-1),
    fThetaC(-1.),

    fDrcPDCollection(new TClonesArray("PndDrcPDPoint")),        //! Hit collection
    fDrcBarCollection (new TClonesArray("PndDrcBarPoint")),        //! Hit collection in the bar
    fEventID(0),    
    fSenId1(0), 
    fSenId2(0), 
    fSenIdBar(0)

{  
  fListOfSensitives.push_back("Sensor");
  if(fVerboseLevel > 0){
    std::cout<<"-I- PndBarrelDIRC: fListOfSensitives contains:";
    for(Int_t k=0; k<fListOfSensitives.size(); k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  } 
  
  for(Int_t i=0; i<1000; i++){
    fLambda[i] = 0.;
    fEfficiency[i] = 0.;
    fEfficiencyR[i] = 0.;
  }
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDrc::PndDrc(const char* name, Bool_t active)
  : FairDetector(name, active),
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
    flside(-999.),
    fbarwidth(-999.),
 
    fRunCherenkov(kTRUE),            //!  Switch ON/OFF Cherenkov propagation
    fTrackID(-1),         //!  track index
    fCurrentTrackID(-1),         //!  track index
    fCopyNo(-1),          //!  volume id
    fPos(TLorentzVector(0,0,0)),             //!  position
    fMom(TLorentzVector(0,0,0)),             //!  momentum
    fTime(-1),           //!  time
    fLength(-1),          //!  length 
    fAngIn(0),
    fNBar(0),
    fPosIndex(-1),                 //! 
    volDetector(0),               //!  MC volume ID of drc
    fMass(-1),
    fMom1(TLorentzVector(0,0,0)), 
    fMom2(TLorentzVector(0,0,0)),  //! for transport efficiency calculation
    fPos2(TLorentzVector(0,0,0)),  //! for transport efficiency calculation
    fBarEnd(0),
    fMirrorGap(0),
  
  // for efficiency calculation:
    //fLambda({0}),
    //fEfficiency({0}),
    //fEfficiencyR({0}),
    fLambdaMin(0.),
    fLambdaMax(0.),
    fLambdaStep(0.),
    fAngleStep(0.),
    fLambdaPoints(0),
  
  // used in ProcessHits function:
    fbarID(0),	   //!  ID number of DrcBarSensors
    fpdID(0),		   //!  ID number of DrcPdSensor
    flens3ID(0),	   //!  ID number of third lenses
    flens2ID(0),
    flens1ID(0),
    fbboxID(0),      	   //!  ID number of DrcBarBoxes
    fevID(0),		   //!  ID number of Expansion Volume
  
    fDetEff(0x0),          //!  Detector Efficiency as a function of photon wavelength
    fDetEffAtProduction(kFALSE),
    fTransportEffAtProduction(kFALSE),
    frand(),
    fLastTrackID(-1),
    fCollectionEff(-1),//Collection Efficiency 
    fPackingFraction(-1),//Packing Efficiency 
 
    fStopTime(kFALSE),
    fPhoMaxTime(-1),
    fTakeDirect(kFALSE), 
    fTakeReflected(kFALSE),     

    fFocusing(-1), 
    fTakeRealReflectivity(kFALSE),
    fStopSecondaries(kFALSE),

    fGeo(new PndGeoDrc()),             //! Pointer to basic DRC geometry data

    fPdgCode(-1),
    fThetaC(-1.),

    fDrcPDCollection(new TClonesArray("PndDrcPDPoint")),        //! Hit collection
    fDrcBarCollection (new TClonesArray("PndDrcBarPoint")),        //! Hit collection in the bar
    fEventID(0),    
    fSenId1(0), 
    fSenId2(0), 
    fSenIdBar(0)      
{
    
    fListOfSensitives.push_back("Sensor");
    if(fVerboseLevel > 0){
      std::cout<<"-I- PndBarrelDIRC: fListOfSensitives contains:";
      for(Int_t k=0; k<fListOfSensitives.size(); k++)
        std::cout<<"\n\t"<<fListOfSensitives[k];
      std::cout<<std::endl;
    }
    
    for(Int_t i=0; i<1000; i++){
      fLambda[i] = 0.;
      fEfficiency[i] = 0.;
      fEfficiencyR[i] = 0.;
    }
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDrc::~PndDrc() {
  if (fDrcPDCollection) {
    fDrcPDCollection->Delete();
    delete fDrcPDCollection;
  }

  if (fDrcBarCollection) {
    fDrcBarCollection->Delete();
    delete fDrcBarCollection;
  }
  
  if (fGeo) delete fGeo;
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void PndDrc::Initialize() {
  cout << " -I- PndDrc: Intialization started... " << endl;
  FairDetector::Initialize();
  //FairRun       *sim  = FairRun::Instance();
  //FairRuntimeDb *rtdb = sim->GetRuntimeDb();
  //PndGeoDrcPar *par  = (PndGeoDrcPar*)(rtdb->getContainer("PndGeoDrcPar"));

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
  flside 	 =  fGeo->Lside();
  fbarwidth	 =  fGeo->BarWidth();
  
  cout<<"DRC parameters: fpi = "<<fpi<<", fzup = "<<fzup<<", fbarnum = "<<fbarnum<<", flside = "<<flside<<endl;
  
  // print out for debugging all names and pointers stored in manager
  // manager->Print();
  
  //%%%%%%%%%%%%%%5
    nphotons = 0;
  //%%%%%%%%%%%%%%%
  
  // bar ID number:    
  fbarID =gMC->VolId("DrcBarSensor");      
  cout<<"bar 1 id = "<<fbarID<<endl;
    
  // focusing system - for now there is no:
  if(fFocusing == 0){
    flens3ID = fbarID;
    flens2ID = fbarID;
    flens1ID = fbarID;
  }
  if(fFocusing == 1){    
    flens3ID = gMC->VolId("DrcLENS3Sensor");
    flens2ID = gMC->VolId("DrcLENS2Sensor");
    flens1ID = gMC->VolId("DrcLENS1Sensor");
    //cout<<"lens1 = "<<v2->FindNode("DrcLENS1Sensor_1")->GetVolume()->GetNumber()<<
    //    ", lens2 = "<<v2->FindNode("DrcLENS2Sensor_1")->GetVolume()->GetNumber()<<
    // 	", lens3 = "<<v2->FindNode("DrcLENS3Sensor_1")->GetVolume()->GetNumber()<<endl;
  }
  if(fFocusing == 3){        
    flens2ID = gMC->VolId("DrcLENS2Sensor");
    flens1ID = gMC->VolId("DrcLENS1Sensor");
    //cout<<"lens1 = "<<v2->FindNode("DrcLENS1Sensor_1")->GetVolume()->GetNumber()<<
    //    ", lens2 = "<<v2->FindNode("DrcLENS2Sensor_1")->GetVolume()->GetNumber()<<endl;
    flens3ID = flens2ID;
  }
  cout<<"lens1ID = "<<flens1ID<<", flens2ID = "<<flens2ID<<", lens3ID = "<<flens3ID<<endl;
      
  // PD id number  
  fpdID = gMC->VolId("DrcPDSensor");
  cout<<"pd id = "<<fpdID<<endl;
    
  // to find out which barbox charged particle hit:  
  fbboxID = gMC->VolId("DrcBarBox");
  cout<<"bbox id = "<<fbboxID<<endl;
   
  // EV id number  
  fevID = gMC->VolId("DrcEV");
  cout<<"EV id = "<<fevID<<endl;
      
  // create a detector efficiency function:
  if(fDetEffAtProduction == kTRUE){
    fCollectionEff=0.65;//Collection Efficiency 
    fPackingFraction=0.80;//Packing Efficiency 
   
// quantum efficiency data from Alex Britting, Jan 25, 2011
// unit is percent
// first value is at 200 nm, last at 700 nm
// credible range start around 250nm, >= 280nm to be safe

    Float_t credibleLimit=280.;

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
    
    //fLambda[1000];
    for(Int_t i=0; i<1000; i++){
      fLambda[i] = i;
    }
    
    // still need to convert from percent and cut values below credible limit
    for (Int_t iBin=0;iBin<1000;iBin++) 
      {
        if (iBin<(Int_t)(credibleLimit) || iBin > 700)
	  {
	    fEfficiencyR[iBin]=0.;
	  } 
        else
	  {
	    // total detector efficiency
	    fEfficiencyR[iBin]=fEfficiency[iBin-200]/100.*fCollectionEff*fPackingFraction;
	  }
        //      cout << "efficiency is " << fEfficiency[iBin] << " at " << fLambdaMin+iBin<<endl;
      }   
  
    fDetEff = new TGraph(1000, fLambda,fEfficiencyR);    
    fLastTrackID = -2;
  }
      
  cout << " -I- PndDrc: Intialization successfull" << endl;
}

// -------------------------------------------------------------------------
void PndDrc::BeginEvent(){
  
  if (fVerboseLevel >0) cout<<" \n\n>>>>>>>>>>>>>>>>>>>>new event in the Barrel DIRC" <<endl;
  fEventID++;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndDrc::ProcessHits(FairVolume* vol) {
  
  TString nam =vol->GetName();
  //cout<<"-I- PndDrc: vol nam = "<<nam<<endl;
  Int_t num = vol->getMCid();
       
   //Register points in the barrel (PndDrcBarPoints)
  fEventID = gMC->CurrentEvent();
  fPdgCode = gMC->TrackPid();
     
  //TLorentzVector fPos, fMom;
  gMC->TrackPosition(fPos);
  
  //$$$$$$$$$$$$$$$$$$$$$$
  //stop secondaries so that they do not produce Cherenkov photons
  if(fStopSecondaries){
    if(fPdgCode != 50000050){
      if(gMC->GetStack()->GetCurrentParentTrackNumber() != -1 ){
        if(gMC->IsTrackEntering()){
          gMC->StopTrack();
        }
      }  
    }
  }
  //$$$$$$$$$$$$$$$$$$$$$$
  
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 /* // stop muon or pion or kaon immediately after DIRC
  if(fabs(fPdgCode) == 211 || fabs(fPdgCode) == 13 || fabs(fPdgCode) == 321){    
    //cout<<"pos: x = "<<fPos.X()<<", y = "<<fPos.Y()<<", R = "<<sqrt(fPos.X()*fPos.X() + fPos.Y()*fPos.Y()) <<endl;    
    if(gMC->IsTrackExiting()==1){
      //cout<<"track is exiting!!! "<<num<<endl;
      if(num == fbarID){
        if(sqrt(fPos.X()*fPos.X() + fPos.Y()*fPos.Y()) > fradius+1.5){
	  //cout<<"pos: x = "<<fPos.X()<<", y = "<<fPos.Y()<<", R = "<<sqrt(fPos.X()*fPos.X() + fPos.Y()*fPos.Y()) <<endl;
	  //cout<<"particle "<<fPdgCode<<" is stopped"<<endl;
          gMC->StopTrack();
	}
      }	
    }
    if(sqrt(fPos.X()*fPos.X() + fPos.Y()*fPos.Y()) > fradius+5.){
      gMC->StopTrack();
    }
  }
*/    
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    
  if (fPdgCode == 50000050){  
    if (fRunCherenkov==kFALSE ) {
      gMC->StopTrack();
      if (fVerboseLevel >0) cout<< "Photon killed" << endl;
    }  
      
      //$$$$$$$$$$$$$$$$$$$$$$$$$$$
      /* if(num == fbarID && fLastTrackID != gMC->GetStack()->GetCurrentTrackNumber()){ // check how many photons were born  
        nphotons = nphotons + 1;
        cout<<"photon number "<<nphotons<<" is produced!!!"<<endl;
       }*/
             
      //cout<<"photon z coord = "<<fPos.Z()<<endl;      
      
      //cout<<"mother = "<<gMC->MotherID()<<endl;
     /* if(fPos.Z() < -118.5){// && gMC->TrackTime()*1.0e09 < 10.){ // how many photons reach PD plane        
        nphotons = nphotons + 1;
        cout<<"photon number "<<nphotons<<" is produced!!!"<<endl;
        gMC->StopTrack();          
      }*/
      //$$$$$$$$$$$$$$$$$$$$$$$$$$$
     
    // apply detector efficiency at the production stage:    
    if(fDetEffAtProduction && fLastTrackID != gMC->GetStack()->GetCurrentTrackNumber()){
      //cout<<"-I- PndDrc: DET EFF IS APPLIED!!!"<<endl;     
      gMC->TrackMomentum(fMom1);
      Double_t Px= fMom1.Px();
      Double_t Py= fMom1.Py();
      Double_t Pz= fMom1.Pz();
      Double_t fP = sqrt(Px*Px + Py*Py +Pz*Pz);
      Double_t lambda=197.0*2.0*fpi/(fP*1.0E9);      
      Double_t ra = frand.Uniform(0., 1.);
      if(ra > fDetEff->Eval(lambda)){               
        gMC->StopTrack();	
      }
    }
    
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    // apply transport efficiency at production stage:
    // Maria Patsyuk 20.04.2012
    if(fTransportEffAtProduction && fLastTrackID != gMC->GetStack()->GetCurrentTrackNumber()){
      gMC->TrackMomentum(fMom2);
      // initial momentum of the photon
      TVector3 PphoInit;
      PphoInit.SetXYZ(fMom2.Px(),fMom2.Py(),fMom2.Pz());
      Double_t P_tr = sqrt(pow(PphoInit.X(),2) + pow(PphoInit.Y(),2) + pow(PphoInit.Z(),2));
      Double_t lam_tr = (197.0*2.0*fpi/(P_tr*1.0E9)) / 1000.;
      //cout<<"lam = "<<lam_tr<<endl;
      gMC->TrackPosition(fPos2);
      // production point of the photon
      TVector3 StartVertex;
      StartVertex.SetXYZ(fPos2.X(), fPos2.Y(), fPos2.Z());
      //cout<<"production point = "<<fPos2.X()<<", "<< fPos2.Y()<<", "<< fPos2.Z()<<endl;
      //calculate the number of bounces:
      Int_t NbouncesX, NbouncesY;
      Double_t angleX, angleY;
      NumberOfBounces(StartVertex, PphoInit, &NbouncesX, &NbouncesY, &angleX, &angleY);
      // calculate the bounce probability
      Double_t n_quartz = sqrt(1. + (0.696*lam_tr*lam_tr/(lam_tr*lam_tr-pow(0.068,2))) + (0.407*lam_tr*lam_tr/(lam_tr*lam_tr-pow(0.116,2))) + 0.897*lam_tr*lam_tr/(lam_tr*lam_tr-pow(9.896,2)));
      //cout<<"n_quartz = "<<n_quartz<<endl;
      Double_t bounce_probX = 1. - pow(4.*fpi*cos(angleX)*fGeo->Roughness()*n_quartz/lam_tr,2); 
      Double_t bounce_probY = 1. - pow(4.*fpi*cos(angleY)*fGeo->Roughness()*n_quartz/lam_tr,2);
      Double_t TotalTrProb = pow(bounce_probX, (Int_t)NbouncesX)*pow(bounce_probY, (Int_t)NbouncesY);
      //cout<<"tr eff X = "<<bounce_probX<<", tr eff Y = "<<bounce_probY<<", total = "<<TotalTrProb<<endl;
      Double_t ra_tr = frand.Uniform(0., 1.);
      if(ra_tr > TotalTrProb){               
        gMC->StopTrack();	
      }
    }
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
             
    
 /*   // ONLY FOR DESIGN WITH LENSES!!!!
    // kill photons that get out of the lens through the sides
    // air gap is 0.5 cm, EV starts at -120 cm, 0.05 cm - curvature?
    if(fFocusing == 1){    
      if(gMC->IsTrackExiting()==1){
        if(num == flens1ID || num == flens2ID || num == flens3ID){
	  if(fPos.Z() > -119.238){
	    gMC->StopTrack();
	  }
	}
      } 
    }
 */   
    
    // "TakeOnlyDirectPho" option:
    // if photon is exiting the bar - check its direction
    if(fTakeDirect){  
      if(gMC->IsTrackExiting()==1){        
        if(num == flens3ID && fPos.Z() < fBarEnd){
	  //cout<<"fBarEnd = "<<fBarEnd<<endl;
	  //std::cout<<"Track is exiting the "<<num<<", "<<nam<<std::endl;
	  gMC->TrackPosition(fPos);
	  gMC->TrackMomentum(fMom);
	  if ((fPos.X()*fMom.X() + fPos.Y()*fMom.Y()) < 0.){
	    gMC->StopTrack();	    	   
	  }
	}
      }
    }
    // "TakeOnlyReflectedPho" option:
    // if photon is exiting the bar - check its direction
   if(fTakeReflected){  
      if(gMC->IsTrackExiting()==1){        
        if(num == flens3ID && fPos.Z() < fBarEnd){
	  //std::cout<<"Track is exiting the "<<num<<", "<<nam<<std::endl;
	  gMC->TrackPosition(fPos);
	  gMC->TrackMomentum(fMom);
	  if ((fPos.X()*fMom.X() + fPos.Y()*fMom.Y()) > 0.){
	    gMC->StopTrack();	    	   
	  }
	}
      }
    }
   // kill photons older than fPhoMaxTime:  
   if (fStopTime == kTRUE && gMC->TrackTime()*1.0e09 > fPhoMaxTime){          
      gMC->StopTrack();
   }   
    
   if (gMC->IsTrackEntering()==1){
      if( num == fpdID){ 
      //if (nam.BeginsWith("DrcPD")){             
        fCopyNo = vol->getCopyNo();
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber(); //track ID     
        gMC->TrackPosition(fPos);
        gMC->TrackMomentum(fMom); // GeV/c	
        fTime=gMC->TrackTime()*1.0e09; // ns
	fLength = gMC->TrackLength(); // cm ??	
        AddHit(fTrackID,
	     fCopyNo,
	     TVector3(fPos.X(),   fPos.Y(),   fPos.Z()),
	     TVector3(fMom.Px(),  fMom.Py(),  fMom.Pz()),
	     fTime,
	     fLength,
	     fPdgCode,
	     fEventID);
       }
       PndStack* stack = (PndStack*) gMC->GetStack();
       stack->AddPoint(kDRC);
     }
  }else if(gMC->TrackCharge()!=0. && gMC->IsTrackEntering()==1 ){
        //if (nam.BeginsWith("DrcBar")) {
	if(num == fbarID){
	//	Double_t fCharge = gMC->TrackCharge();
		fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
		if(fTrackID!=fCurrentTrackID){
		  fTime  = gMC->TrackTime() * 1.0e09;
		  fLength = gMC->TrackLength();
  //		Int_t  copyNo = vol->getCopyNo();
		  Int_t s=0, b=0; //side and bar
		  fNBar=0;
		  TString path = gMC->CurrentVolPath();
		  //cout<<"+++++++++++++++++++++++++++++++++"<<endl;
		  //cout<< "Volume: " << gMC->CurrentVolPath() << endl;
		  //cout<<"+++++++++++++++++++++++++++++++++"<<endl;    
		  if (fVerboseLevel >0) cout<< "Volume: " << gMC->CurrentVolPath() << endl;
		  sscanf(path, "/cave_1/BarrelDIRC_0/DrcBarBox_%d/DrcAirBox_0/DrcBarSensor_%d", &s, &b);		
		  //cout<<"side no.= "<<s<<" bar no.="<<b<<endl;//		
		  if(s != 0 && s<17){
		     fNBar = s*10 +b;}                 
		  //cout<<"fNBar= "<<fNBar<<endl;  
		  gMC->TrackMomentum(fMom); // GeV/c		
		  Double_t Px= fMom.Px();
		  Double_t Py= fMom.Py();
		  Double_t Pz= fMom.Pz();
		  Double_t fP = sqrt(Px*Px + Py*Py +Pz*Pz);
		  fMass = gMC->TrackMass();
		  Double_t fEnergy = TMath::Sqrt(fP*fP + fMass*fMass); 
		  //Double_t fAngIn;
		  if ( fP==0 ) {fAngIn = -1.;}
		  else if ( fabs(Pz/fP) > 1. ){ fAngIn = -1.;}
		  else { fAngIn = acos(Pz/fP);}
		  if ( fP == 0. || fEnergy == 0.){ fThetaC = -1.;}
		  else if (fabs(1./(1.47*(fP/fEnergy))) > 1. ){ fThetaC = -1.;}
		  else{ fThetaC = acos(1/(1.47*(fP/fEnergy)));} 
		  AddBarHit(fTrackID,
			  fCopyNo,
			  TVector3(fPos.X(),   fPos.Y(),   fPos.Z()),
			  TVector3(fMom.Px(),  fMom.Py(),  fMom.Pz()),
			  fTime,
			  fLength,
			  fPdgCode,
			  fAngIn,
			  fThetaC,
			  fNBar,
			  fEventID,
			  fMass);
		  PndStack* stack = (PndStack*) gMC->GetStack();
       		  stack->AddPoint(kDRC);
		  fCurrentTrackID=fTrackID;
		}	
	}
  }
  
  ResetParameters(); 
  fLastTrackID = gMC->GetStack()->GetCurrentTrackNumber(); 
  return kTRUE; 
}

// ---------------------------------------------------------------------------

//------   Find Nubmer of Bounces     --------------------------------------
void PndDrc::NumberOfBounces(TVector3 start, TVector3 dir, Int_t *n1, Int_t *n2, Double_t *alpha1, Double_t *alpha2){
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
    if(fbarnum > 1){
      // Find which bar in the bar box was hit:
      Int_t NhitBar = (Int_t)((0.5*flside + startBar.Y())/fbarwidth)+1;
      //cout<<"-I- NumberOfBounces: bar "<<NhitBar<<" was hit, "<<((0.5*flside + startBar.Y())/fbarwidth)+1<<endl;
      //cout<<"-I- NumberOfBounces: start bar Y = "<<startBar.Y()<<endl;
    
     //cout<<"-I- NumberOfBounces: start position X = "<< startBar.X() - (fradius-fhthick)<<", Y = "<<startBar.Y() + 0.5*flside-(NhitBar-1)*fbarwidth<<endl;  
      FindOutPoint(X0, startBar.X()-(fradius-fhthick), 		   2.*fhthick, &N1, 0);
      FindOutPoint(Y0, startBar.Y()+0.5*flside-(NhitBar-1)*fbarwidth, fbarwidth, &N2, 0);
      //cout<<"-I- NumberOfBounces: N1 = "<<N1<<", N2 = "<<N2<<endl;
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
    //cout<<"-I- NumberOfBounces: angle1 = "<<angle1<<", angle2 = "<<angle2<<endl;
}

//----------------------------------------------------------------------------------------------
Double_t PndDrc::FindPhiRot(Double_t xx, Double_t yy){ // returns [degrees]

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
Double_t PndDrc::FindOutPoint(Double_t x0, Double_t xEn, Double_t a, Double_t *NN, Bool_t print){	
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
  fLastTrackID = -2;
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void PndDrc::Register() {
  FairRootManager::Instance()->Register("DrcBarPoint","Drc", fDrcBarCollection, kTRUE);
  FairRootManager::Instance()->Register("DrcPDPoint","Drc", fDrcPDCollection, kTRUE);

}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndDrc::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fDrcPDCollection;
   if (iColl == 1) return fDrcBarCollection;
  return NULL;
}
// ----------------------------------------------------------------------------




// -----   Public method Print   ----------------------------------------------
void PndDrc::Print() const {
    Int_t nPDHits = fDrcPDCollection->GetEntriesFast();
    Int_t nbarHits = fDrcBarCollection->GetEntriesFast();
    cout << "-I- PndDrc: " << nPDHits << " points registered in the photodetector for this event." << endl;
    cout << "-I- PndDrc: " << nbarHits << " points registered in the bar for this event." 	<< endl;

 if (fVerboseLevel>1){
   for (Int_t i=0; i<nPDHits; i++) (*fDrcPDCollection)[i]->Print();
   for (Int_t i=0; i<nbarHits; i++) (*fDrcBarCollection)[i]->Print();
 }
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void PndDrc::Reset() {
   fDrcPDCollection->Delete();
   fDrcBarCollection->Delete();
   fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// guarda in FairRootManager::CopyClones
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
// ----------------------------------------------------------------------------



// -----   Public method ConstructGeometry  -----------------------------------
void PndDrc::ConstructGeometry()
{
  cout<< "                                               " << endl;
  cout<< " =======  DRC::  ConstructGeometry()  ======== " << endl;
  cout<< " ============================================= " << endl;
  
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")){
    ConstructRootGeometry();
    if(fileName.Contains("_l0_")){ 
      fBarEnd = -119.999;
      fFocusing = 0;      
    }
    if(fileName.Contains("_l2_")){
      fBarEnd = -119.999;
      fFocusing = 2;      
    }
    if(fileName.Contains("_l1_")){
      fBarEnd = -118.725;
      fFocusing = 1;
    } 
     if(fileName.Contains("_l3_")){
      fBarEnd = -119.8;
      fFocusing = 3;
      //cout<<"focusing = "<<fFocusing<<endl;
    } 
    if(fileName.Contains("mirrorGap")){
      fMirrorGap = 0.1; //[cm]
    }
  } else{
    std::cout<<"Geometry format not supported!"<<std::endl;
  } 
  cout<<"Focusing = "<<fFocusing<<endl;  
  cout<< " ============================================= " << endl;
}

// -----   Public Method Construct Optical Geometry ---------------------------
void PndDrc::ConstructOpGeometry()
{
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
  
//  gMC->DefineOpSurface("BarSurface", kGlisur, kDielectric_dielectric, kPolished, 0.0);
  gMC->DefineOpSurface("MirrSurface", kGlisur, kDielectric_metal, kPolished, 0.0); 
  
  if(fGeo->barNum() > 1){
    for(Int_t i=0; i<fGeo->barNum(); i++){
      //gMC->SetBorderSurface("BarAirSurface", "DrcBarSensor", i+1, "DrcAirBox", 0, "BarSurface");
      if(fFocusing == 1 || fFocusing == 0 || fFocusing == 3){ // lens or no focusing 
        if(fMirrorGap == 0.){     
          //gMC->SetBorderSurface("Lens1AirSurface", "DrcLENS1", i+1, "DrcAirBox", 0, "BarSurface");
          //gMC->SetBorderSurface("Lens2AirSurface", "DrcLENS2", i+1, "DrcAirBox", 0, "BarSurface");
          //gMC->SetBorderSurface("Lens3AirSurface", "DrcLENS3", i+1, "DrcAirBox", 0, "BarSurface");
          gMC->SetBorderSurface("BarMirrSurface", "DrcBarSensor", i+1, "DrcMirr", i+1, "MirrSurface");
	}
      }
    }
  }
  if(fMirrorGap > 0.){
    gMC->SetSkinSurface("AirMirrorSurface", "DrcMirr", "MirrSurface");
  }
  
  if(fTakeRealReflectivity == kFALSE){
    gMC->SetMaterialProperty("MirrSurface", "REFLECTIVITY", npoints_i, ephoton_i, reflectivity_i);
  }
  if(fTakeRealReflectivity == kTRUE){
    gMC->SetMaterialProperty("MirrSurface", "REFLECTIVITY", npoints_r, ephoton_r, reflectivity_r);
  }
    
  cout<<"fbarnum = "<<fbarnum<<endl;
  if( fGeo->barNum() > 1){
    gMC->DefineOpSurface("EVSurface", kGlisur, kDielectric_metal, kPolished, 0.0);
    gMC->SetBorderSurface("EVAirSurface", "DrcEV", 1, "BarrelDIRC", 0, "EVSurface"); 
    gMC->SetMaterialProperty("EVSurface", "REFLECTIVITY", npoints_i, ephoton_i, reflectivity_i);
  
    gMC->DefineOpSurface("PDSurface", kGlisur, kDielectric_dielectric, kPolished, 0.0);
    gMC->SetBorderSurface("EVPDSurface", "DrcEV", 1, "DrcPDSensor", 1, "PDSurface");
    gMC->SetMaterialProperty("PDSurface", "EFFICIENCY", npoints_i, ephoton_i, reflectivity_i);
  }
  if( fGeo->barNum() == 1 ){
    if(fMirrorGap == 0.){
      gMC->SetBorderSurface("BarMirrSurface", "DrcBarSensor", 1, "DrcMirr", 1, "MirrSurface");
    }
    if( fGeo->barNum() > 1){
      gMC->SetSkinSurface("AirMirrorSurface", "DrcMirr", "MirrSurface");
    }
    //cout<<"bar num = "<<fGeo->BBoxNum()<<endl;
    for(int m=1; m<fGeo->BBoxNum(); m++){
        gMC->DefineOpSurface("EVSurface", kGlisur, kDielectric_metal, kPolished, 0.0);
        gMC->SetBorderSurface("EVAirSurface", "DrcEV", m, "BarrelDIRC", 0, "EVSurface");
        gMC->SetMaterialProperty("EVSurface", "REFLECTIVITY", npoints_i, ephoton_i, reflectivity_i);
    }
  }
  
  cout<<" =======  DRC::ConstructOpGeometry -> Finished! ====== "<< endl;     
}  

// -----   Public method CheckIfSensitive   --------------------------------------
 bool PndDrc::CheckIfSensitive(std::string name)
 {
  for (Int_t i = 0; i < fListOfSensitives.size(); i++){
   if (name.find(fListOfSensitives[i]) != std::string::npos)    
    return true;
  }
 return false;
 }

// -----   Private method AddHit   --------------------------------------------
PndDrcPDPoint* PndDrc::AddHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Int_t eventID) {
 
  TClonesArray& clrefPD = *fDrcPDCollection;
  Int_t size = clrefPD.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- PndDrc: Adding Point at (" << pos.X() << ", " << pos.Y() 
	 << ", " << pos.Z() << ") cm, detector " << copyNo << ", track "
	 << trackID <<" event "<<eventID << endl;
  return new(clrefPD[size]) PndDrcPDPoint(trackID, 
					  copyNo, 
					  pos, 
					  mom, 
					  time, 
					  length, 
					  pdgCode,
					  eventID);

}


PndDrcBarPoint* PndDrc::AddBarHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Double_t angIn, Double_t thetaC, Int_t nBar, Int_t eventID, Double_t mass) {
 
  TClonesArray& clrefBar = *fDrcBarCollection;
  Int_t size = clrefBar.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- PndBarDrc: Adding Point at (" << pos.X() << ", " << pos.Y() 
	 << ", " << pos.Z() << ") cm, detector " << copyNo << ", track "
	 << trackID <<" event "<<eventID << endl;
  return new(clrefBar[size]) PndDrcBarPoint(trackID, 
					    copyNo, 
					    pos, 
					    mom, 
					    time, 
					    length, 
					    pdgCode,
					    angIn,
					    thetaC,
					    nBar,
					    eventID, 
					    mass);
  
}


// ----


ClassImp(PndDrc)
