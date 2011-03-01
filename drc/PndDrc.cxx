
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
#include "TGeoTrd2.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TObject.h"
#include "TColor.h"

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
PndDrc::PndDrc() {
  fDrcPDCollection = new TClonesArray("PndDrcPDPoint");
  fDrcBarCollection = new TClonesArray("PndDrcBarPoint");
  fPosIndex   = 0;
  volDetector = 0;
  fRunCherenkov = kTRUE;
  fGeo         = new PndGeoDrc();
  fStopTime = kFALSE;
  fTakeDirect = kFALSE; 
  fDetEffAtProduction = kFALSE;
  fprizm = kFALSE;  
  fListOfSensitives.push_back("Sensor");
  if(fVerboseLevel > 0){
    std::cout<<"-I- PndBarrelDIRC: fListOfSensitives contains:";
    for(Int_t k=0; k<fListOfSensitives.size(); k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  } 
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDrc::PndDrc(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fDrcPDCollection = new TClonesArray("PndDrcPDPoint");
    fDrcBarCollection = new TClonesArray("PndDrcBarPoint");
    fPosIndex   = 0;
    volDetector = 0;
    fEventID    = -1;
    fSenId1 =0;
    fSenId2 =0;
    fSenIdBar = 0;
    fRunCherenkov = kTRUE;
    fGeo         = new PndGeoDrc();
    fStopTime = kFALSE;
    fTakeDirect = kFALSE; 
    fprizm = kFALSE;
    fDetEffAtProduction = kFALSE;   
    fListOfSensitives.push_back("Sensor");
    if(fVerboseLevel > 0){
      std::cout<<"-I- PndBarrelDIRC: fListOfSensitives contains:";
      for(Int_t k=0; k<fListOfSensitives.size(); k++)
        std::cout<<"\n\t"<<fListOfSensitives[k];
      std::cout<<std::endl;
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
  
  
  // print out for debugging all names and pointers stored in manager
  // manager->Print();
  
  // bar ID number:  
  TGeoVolume *v2 = gGeoManager->GetVolume("DrcAirBox");   
  TGeoNode *n2 = v2->FindNode("DrcBarSensor_1");
  //v2->PrintNodes();

  fbarID = n2->GetVolume()->GetNumber();
  cout<<"bar id = "<<n2->GetVolume()->GetNumber()<<", node id = "<<n2->GetNumber()<<endl;
    
  // focusing system - for now there is no:
  flensID = fbarID;
  if(fFocusingSystem == 1){
    TGeoNode *n5 = v2->FindNode("DrcLENS3Sensor_1");
    flensID = n5->GetVolume()->GetNumber();
    //cout<<"lens1 = "<<v2->FindNode("DrcLENS1_1")->GetVolume()->GetNumber()<<
    //    ", lens2 = "<<v2->FindNode("DrcLENS2_1")->GetVolume()->GetNumber()<<
    //	", lens3 = "<<v2->FindNode("DrcLENS3Sensor_1")->GetVolume()->GetNumber()<<endl;
  }
  cout<<"lens id = "<<flensID<<endl;
    
  // PD id number
  TGeoVolume *v3 = gGeoManager->GetVolume("BarrelDIRC");
  //v3->PrintNodes();
  TGeoNode *n3 = v3->FindNode("DrcPDSensor_1");
  fpdID = n3->GetVolume()->GetNumber();
  //cout<<"pd id = "<<fpdID<<", node id = "<<n3->GetNumber()<<endl;
    
  // to find out which barbox charged particle hit:
  TGeoNode *n4 = v3->FindNode("DrcBarBox_1");
  fbboxID = n4->GetVolume()->GetNumber();
  //cout<<"bbox id = "<<n4->GetVolume()->GetNumber()<<", node id = "<<n4->GetNumber()<<endl;
  //cout<<"abox id = "<<v2->GetNumber()<<endl;
  
  // create a detector efficiency function:
  if(fDetEffAtProduction == kTRUE){
    fCollectionEff=0.65;//Collection Efficiency 
    fPackingFraction=0.80;//Packing Efficiency 
    Double_t fEfficiency[1000];
    
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
    
    Double_t lambda[1000];
    for(Int_t i=0; i<1000; i++){
      lambda[i] = i;
    }
   
    // still need to convert from percent and cut values below credible limit
    for (Int_t iBin=0;iBin<1000;iBin++) 
      {
        if (iBin<(Int_t)(credibleLimit) || iBin > 500)
	  {
	    fEfficiency[iBin]=0.;
	  } 
        else
	  {
	    // total detector efficiency
	    fEfficiency[iBin]=fEfficiency[iBin]/100.*fCollectionEff*fPackingFraction;
	  }
        //      cout << "efficiency is " << fEfficiency[iBin] << " at " << fLambdaMin+iBin<<endl;
      }   
  
    fDetEff = new TGraph(1000, lambda,fEfficiency);
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
  Int_t num = vol->getMCid();
   
   //Register points in the barrel (PndDrcBarPoints)
  fEventID = gMC->CurrentEvent();
  fPdgCode = gMC->TrackPid();
   
  //TLorentzVector fPos, fMom;
  gMC->TrackPosition(fPos);
    
  if (fPdgCode == 50000050){  
    if (fRunCherenkov==kFALSE ) {
      gMC->StopTrack();
      if (fVerboseLevel >0) cout<< "Photon killed" << endl;
    }
     
    // apply detector efficiency at the production stage:    
    if(fDetEffAtProduction && fLastTrackID != gMC->GetStack()->GetCurrentTrackNumber()){     
      gMC->TrackMomentum(fMom1);
      Double_t Px= fMom1.Px();
      Double_t Py= fMom1.Py();
      Double_t Pz= fMom1.Pz();
      Double_t fP = sqrt(Px*Px + Py*Py +Pz*Pz);
      Double_t lambda=197.0*2.0*TMath::Pi()/(fP*1.0E9);      
      Double_t ra = frand.Uniform(0., 1.);
      if(ra > fDetEff->Eval(lambda)){               
        gMC->StopTrack();
      }
    }
    
  /*
  // check whether function 'ConstructOpGeometry' works:
  if(gMC->IsTrackExiting()==1){
    //if(nam.BeginsWith("DrcBar")){
    if(num == fbarID){  
      if(fPos.Z() > -110. && fPos.Z() < -109. && aaa < 11){
        cout<<"track is exiting the bar at z = "<<fPos.Z()<<endl;
	aaa = aaa+1;
      }
    }  
  }
  //----------------------------   
  */  
   // take only direct photons:
   if(fTakeDirect){      
     if (gMC->IsTrackExiting()==1 ){       
        if(nam.BeginsWith(fAtBarEnd)){                      
	//if(num == flensID && fPos.Z() < fSlabEnd + 0.001){
	//if(fPos.Z() < fSlabEnd + 0.0001){
	  //std::cout<<"!!! Track is exiting volume "<<nam<<", "<<num<<" at "<<fPos.Z()<<", fSlabEnd = "<<fSlabEnd<<std::endl;
	  gMC->TrackMomentum(fMom);	    
          if ((fPos.X()*fMom.X() + fPos.Y()*fMom.Y()) < 0.){
	     //cout<<"track is stopped!"<<endl;	              
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
	}
  }
  
  ResetParameters(); 
  fLastTrackID = gMC->GetStack()->GetCurrentTrackNumber(); 
  return kTRUE; 
}

// ---------------------------------------------------------------------------



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
  /*
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")){
    ConstructRootGeometry();
  } else{
    std::cout<<"Geometry format not supported!"<<std::endl;
  }   
  cout<< " ============================================= " << endl;
  
  */
  const Double_t pi = 3.1415926535;

  FairGeoLoader*    drcgeoLoad = FairGeoLoader::Instance();
  FairGeoInterface* drcgeoFace = drcgeoLoad->getGeoInterface();

  FairGeoMedia *Media =  drcgeoFace->getMedia();
  FairGeoBuilder *geobuild = drcgeoLoad->getGeoBuilder();

  // Call materials
  FairGeoMedium *fusedSil  = Media->getMedium("FusedSil");
  Int_t nFusedSil = geobuild->createMedium(fusedSil);
  FairGeoMedium *nlak33a  = Media->getMedium("NLAK33A");
  Int_t nNlak33a = geobuild->createMedium(nlak33a);
  FairGeoMedium *air  = Media->getMedium("DIRCair");
  Int_t nAir = geobuild->createMedium(air);
  FairGeoMedium *airNoSens  = Media->getMedium("DIRCairNoSens");
  Int_t nAirNoSens = geobuild->createMedium(airNoSens);
  FairGeoMedium *mirror  = Media->getMedium("Mirror");
  Int_t nMirror = geobuild->createMedium(mirror);
  FairGeoMedium *marcol82  = Media->getMedium("Marcol82");
  Int_t nMarcol82 = geobuild->createMedium(marcol82);
  FairGeoMedium *carbonfiber = Media->getMedium("DIRCcarbonFiber");
  Int_t nCarbonFiber = geobuild->createMedium(carbonfiber);

  TGeoVolume *cave = gGeoManager->GetTopVolume();
  
  
  // Rotations:
  TGeoRotation rot1;
  rot1.RotateZ(90.);
  
  // units = cm

  Double_t eps           = 0.01;                                  // epsilon
  Double_t mirr_hthick   = 0.01;
    
  Double_t bbnum         =  fGeo->BBoxNum();       //16.;	 // total number of sides = barboxes
  Double_t bbGap         =  fGeo->BBoxGap();       //1.5;  // gap btw the neighboring barboxes (at the middle height)
  Double_t pipehAngle    =  fGeo->PipehAngle();    //3.6;  // [degrees] half of the angular space needed for the target pipe
     
  Double_t radius        =  fGeo->radius();       // radius in middle of the barbox (x and y)
  Double_t hthick        =  fGeo->barHalfThick(); // half thickness of the bars
  Double_t bbox_zdown    =  fGeo->barBoxZDown();  // bar box z downstream
  Double_t bbox_zup      =  fGeo->barBoxZUp();    // bar box z upstream
  Double_t bbox_hlen     =  0.5*(bbox_zdown - bbox_zup);           // bar box half length
  Double_t bbox_shift    =  bbox_zup + bbox_hlen; // bar box shift
  Double_t bargap        =  fGeo->barGap();       // half gap between bars
  Double_t barnum        =  fGeo->barNum();       // number of bars per barbox
  Double_t boxgap        =  fGeo->boxGap(); 	  // gap between bars and the bar box
  Double_t boxthick	 =  fGeo->boxThick();     // thickness of the bar box
  Double_t len           =  0.;                   // length of the lenses block. see further
  
  // Expansion Volume:
  Double_t EVlen         =  fGeo->EVlen();        // 30. in current version
  Double_t EVdrop	 =  fGeo->EVdrop();//0.5;  		   // [cm] drop of the EV - inner radius
  Double_t EVoffset	 =  fGeo->EVoffset();//1.;    		   // [cm] offset of the EV - outer radius
  Double_t EVangle	 =  fGeo->EVangle();//60.;
  Double_t EVshift       =  -bbox_hlen + bbox_shift - EVlen;  
  Double_t EVRout        =  radius + hthick + EVlen*tan(EVangle/180.*pi)/cos(pi/bbnum/2.);
  
  Double_t bbAngle       =  ( 180. - 2.*pipehAngle - bbGap/radius/pi*180.*(bbnum/2.-1.) )/(bbnum/2.);  
  Double_t bbX           =  radius*bbAngle/180.*pi;  
  Double_t phi0          =  (180.-2.*pipehAngle)/bbnum + pipehAngle;
  Double_t dphi          =  (180.-2.*pipehAngle)/bbnum*2.;
  
  // prizm:  
  Double_t phlength	 =  fGeo->PrismhLength();//4.5;  		   // half length of the prizm
  Double_t pangle	 =  fGeo->PrismAngle();//30.;
  Double_t pdrop	 =  fGeo->PrismDrop();//0.5;   		   // drop of the prizm - inner side
  Double_t poffset	 =  fGeo->PrismOffset();//1.;     		   // prizm offset - outer side
  Double_t pheight	 =  2.*phlength * tan(pangle/180.*pi);
    
  // outer radius of the EV in case of the prism:
  Double_t EVRprizm      =  radius + hthick + poffset + pheight + EVoffset + (EVlen-2.*phlength)*tan(EVangle/180.*pi);
    
  //----------------------------------------------------------
  // F O C U S S I N G      O P T I O N S:
  TGeoVolume *block1, *block2;
  TGeoVolume *lens1, *lens2, *lens3;
  // 0. NO FOCUSING:
  if(fFocusingSystem == 0){
    cout<<"NO FOCUSING!!!"<<endl;
    Double_t flen = 0.;
    fSlabEnd = -bbox_hlen + bbox_shift + flen;  
    cout<<"bar ends at = "<<fSlabEnd<<endl;
    
    fAtBarEnd = "DrcBarSensor";
  }
  //-----------------------------------------------------------
  // 1. LENS:  
  if (fFocusingSystem == 1){  // L E N S E S  (no airgaps, thin nlak33)
    // some notes to lens operations (revision 9649) is in
    // ~carsten/work/documents/software/PandaRoot/lens_definitions_2.pdf
    
    Double_t r = 75.18; // first lens radius (cm)    
    Double_t alpha = TMath::ASin(bbX/12/r);  // lside -> bbX
    Double_t a = r - r*TMath::Cos(alpha);
    Double_t b = a + 0.5; // box dimension  .6 instead of .5 due to strong curvature

    //cout<<" DIRC a,b = "<<a<<" "<<b<<endl;
  

    Double_t r2 = 75.18; // radius second lens (cm)
    Double_t b2 = 0.2;   // + a2;
     
    Double_t r3 = 17.95; // third lens radius (cm)
    Double_t alpha3 = TMath::ASin(bbX/12/r3);  // lside -> bbX
    Double_t a3 = r3 - r3*TMath::Cos(alpha3);
    Double_t b3 = a3 + 0.0; // box dimension  .6 instead of .5 due to strong curvature
  
    // lens1 (b) + lens2 (0.6) + lens3 (b3) + gap (0.5) 
   
    len = b + 0.2 + b3 + 0.5;//+ a2; // dimension of the box containing both lenses
    
    //cout<<"DIRC len= "<<len<<endl;
  
    fSlabEnd = -bbox_hlen + bbox_shift + len - 0.5; // used in processHits

    // Lenses
 
    // Lens 1
    Double_t t = -r +b/2;

    TGeoSphere* logicSphere= new TGeoSphere("S",0.,r, 0. ,180.,0.,360.);
    TGeoBBox* lBox = new TGeoBBox("B", (bbX/barnum)/2-bargap, hthick, b/2.); // lside->bbX
    TGeoTranslation *tr1 = new TGeoTranslation("tr1", 0.,0., t);
    tr1->RegisterYourself();
    TGeoCompositeShape *cs = new TGeoCompositeShape("cs","S*(B:tr1)");
    lens1 = new TGeoVolume("DrcLENS1",cs, gGeoManager->GetMedium("FusedSil"));
    lens1->SetLineColor(kRed-8);
    lens1->SetTransparency(40); 
     
    // position lens within already shifted bar container at -(bbox_hlen-eps)+len, the lens base is -r + b
    // with -0.01 one can make a gap visible (.1mm) for orientation   
    //barContainer->AddNode(lens1, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen-eps)+len -(-r+b) /*-0.01*/, new TGeoRotation (0)));
/*    barContainer->AddNode(lens1, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen)+len -(-r+b), new TGeoRotation (0)));
*/  fdz_lens1 = -(bbox_hlen)+len -(-r+b);
  
    //Lens 2
    Double_t t2 = -r2;// +b2/2 r2  is the reference point (concave lens) 
    TGeoSphere* logicSphere2 = new TGeoSphere("S2",0 ,r2, 0. ,180.,0.,360.);
    TGeoBBox*   lBox2        = new TGeoBBox("B2", (bbX/barnum)/2.-bargap, hthick, b2/2.); // lside -> bbX
    TGeoTranslation *tr2     = new TGeoTranslation("tr2", 0.,0., t2);
    tr2->RegisterYourself();
    TGeoCompositeShape *cs2 = new TGeoCompositeShape("cs2","(B2:tr2)-S2");
    lens2 = new TGeoVolume("DrcLENS2",cs2, gGeoManager->GetMedium("NLAK33A"));
    lens2->SetLineColor(kRed+2);
    lens2->SetTransparency(40); 

    // place the lens exactly on lens1
    // position lens within already shifted bar container at -(bbox_hlen-eps)+len, the lens base is -r2
    // the tip of lens1 is at b
    // with -0.02 one can make a gap visible (.1mm due to lens1) for orientation   
    //barContainer->AddNode(lens2, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen-eps)+len -(-r2) -b /*-0.02*/, new TGeoRotation (0)));
/*    barContainer->AddNode(lens2, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen)+len -(-r2) -b, new TGeoRotation (0)));
*/  fdz_lens2 = -(bbox_hlen)+len -(-r2) -b;

    //Lens3 (like lens1, same treatment)
    Double_t t3 = -r3+b3/2;
    TGeoSphere* logicSphere3= new TGeoSphere("S3",0.,r3, 0. ,180.,0.,360.);
    TGeoBBox* lBox3 = new TGeoBBox("B3", (bbX/barnum)/2-bargap, hthick, b3/2.); // lside -> bbX
    TGeoTranslation *tr3 = new TGeoTranslation("tr3", 0.,0., t3);
    tr3->RegisterYourself();
    TGeoCompositeShape *cs3 = new TGeoCompositeShape("cs3","S3*(B3:tr3)");
    lens3 = new TGeoVolume("DrcLENS3Sensor",cs3, gGeoManager->GetMedium("NLAK33A"));
    lens3->SetLineColor(kRed-6);
    lens3->SetTransparency(40); 
     
    // place the lens exactly on lens2 plane side
    // position lens within already shifted bar container at -(bbox_hlen-eps)+len, the lens base is -r3 + b3
    // with -0.03 one can make a gap visible (.1mm due to lens 1&2) for orientation   
    // b2/2 is the thickness of lens2 in the middle 
    //barContainer->AddNode(lens3, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen-eps)+len -(-r3+b3) -b - b2/2 /*-0.03*/, new TGeoRotation (0)));
/*    barContainer->AddNode(lens3, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen)+len -(-r3+b3) -b - b2/2 , new TGeoRotation (0)));
    //cout<<" DIRC r,r2,r3 = "<<r<<" "<<r2<<" "<<r3<<endl;    
*/
    AddSensitiveVolume(lens3);
  
    fdz_lens3 = -(bbox_hlen)+len -(-r3+b3) -b - b2/2. ;  
    fAtBarEnd = "DrcLENS3Sensor";
  }   // E N D      O F      N E W      L E N S E S  
  //--------------------------------------------------------------------------------
  // 2. MIRROR
  if (fFocusingSystem == 2){  // Mirrors at front
     // Put some mirrors at the downstream end with a focal plane at the PD.
      
     double zpos=120;
     
     // The bar is produced with " bbox_hlen-fabs(len)/2.-mirr_hthick"
     len = -130 + zpos + 1;
     //len = -247.0; // negative to make space at downstream end of bar.
     
     Double_t len1 = 1.5; // 1st block
     
     Double_t mirror_angle  = 0.0;// 0.0 is pointing upstream, -90.0 is pointing to the beam axis
     Double_t focal_length  = 2 * bbox_hlen + EVlen - fabs(len) + len1;
     Double_t mirror_radius = 2 * focal_length;
 
     cout<<" mirror radius: "<<mirror_radius<<endl;
     

     // no angle for the moment
     // block
     TGeoSphere* logicSphere = new TGeoSphere("S",0.,mirror_radius, 0. ,180.,0.,360.);
     TGeoBBox*   lBox        = new TGeoBBox("B", (bbX/barnum)/2-bargap, hthick, fabs(len1)/2.); // lside -> bbX
      
     Double_t t = mirror_radius - len1/2;

     TGeoTranslation *tr1 = new TGeoTranslation("tr1", 0.,0., t);
     tr1->RegisterYourself();
     TGeoCompositeShape *cs = new TGeoCompositeShape("cs","S*(B:tr1)");
 
     block1 = new TGeoVolume("DrcBlock1",cs, gGeoManager->GetMedium("FusedSil"));
     block1->SetLineColor(kRed);
     block1->SetTransparency(40);
      

     Double_t shift1 = len1-mirror_radius; // Now the start of block1 is at zero
     shift1         += bbox_hlen-fabs(len)-2*mirr_hthick;
      
/*     
     barContainer->AddNode(block1, 1, new TGeoCombiTrans(0., 0., shift1, new TGeoRotation (0)));
*/     AddSensitiveVolume(block1);
     fdz_mirr1 = shift1;

     Double_t gap  = 0;
     Double_t len2 = fabs(len) - len1 - gap;
   
     // no angle for the moment
     // block
     TGeoSphere* logicSphere2 = new TGeoSphere("S2",0,mirror_radius,             0. ,180.,0.,360.);
     TGeoBBox*   lBox2        = new TGeoBBox("B2", (bbX/barnum)/2-bargap, hthick, fabs(len2)/2.); // lside -> bbX

     //  make radius part of block
     Double_t t2 = mirror_radius + len2/2 - 1;

     TGeoTranslation *tr2 = new TGeoTranslation("tr2", 0.,0., t2);
     tr2->RegisterYourself();
     TGeoCompositeShape *cs2 = new TGeoCompositeShape("cs2","(B2:tr2)-S2");

     block2 = new TGeoVolume("DrcBlock2Sensor",cs2, gGeoManager->GetMedium("Mirror"));
     block2->SetLineColor(kGreen);
     block2->SetTransparency(40);
      

     Double_t shift2  = -mirror_radius; // Now the start of the block is at zero
     shift2          += bbox_hlen-fabs(len)-2*mirr_hthick; // at end of bar
     shift2          += len1 + gap;
     
     fdz_mirr2 = shift2; 
      
/*       
     barContainer->AddNode(block2, 
			    1,
			    new TGeoCombiTrans(0., 
					       0.,
					       shift2,
					       //  bbox_hlen-mirror_radius-2*mirr_hthick-len2+len1+1,
					       //bbox_hlen-mirror_radius-len2+0.1,
					       new TGeoRotation (0)
					       )
			    );
*/			    
     AddSensitiveVolume(block2);

     Double_t flen = 0.;
    // fSlabEnd = -bbox_hlen + bbox_shift + flen;  
     fSlabEnd = -bbox_hlen + bbox_shift;
     cout<<"bar ends at = "<< fSlabEnd <<endl;
      
     fAtBarEnd = "DrcBarSensor";    
   }   // E N D      O F      MIRRORS
  //----------------------------------------------------------
  
  // create pre-top volume:
  TGeoVolume* vLocalMother;
  TGeoPcon*   lLocalMother = new TGeoPcon("BarrelDIRC", 0, 360., 4);
  lLocalMother->DefineSection(0, bbox_zdown, 45., 55.);
  lLocalMother->DefineSection(1, bbox_zup, 45., 55.);
  lLocalMother->DefineSection(2, bbox_zup - EVlen, 45., EVRout+poffset+pheight+EVoffset);
  lLocalMother->DefineSection(3, bbox_zup - EVlen - 0.1, 45., EVRout+poffset+pheight+EVoffset);
  vLocalMother = new TGeoVolume("BarrelDIRC", lLocalMother, gGeoManager->GetMedium("DIRCairNoSens"));
  cave->AddNode(vLocalMother, 0, 0);
  
   // create BarBoxes:  
  TGeoBBox* logicbbL;
  TGeoVolume *bbox;
  if(fprizm == kFALSE){
    logicbbL = new TGeoBBox("logicbbL", bbX/2.+boxthick, hthick+boxgap+boxthick, bbox_hlen);
    bbox = new TGeoVolume("DrcBarBox", logicbbL, gGeoManager->GetMedium("DIRCcarbonFiber")); 
  }
  if(fprizm == kTRUE){
    logicbbL = new TGeoBBox("logicbbL", bbX/2.+boxthick, hthick+boxgap+boxthick, bbox_hlen-0.5*(boxgap+boxthick));
    TGeoTrap* logicPrizmBox = new TGeoTrap("logicPrizmBox", phlength+0.5*(boxgap+boxthick), pangle/2., 270.,
    hthick+boxgap+boxthick+0.5*(poffset+pdrop+pheight), bbX/2., bbX/2., 0., hthick+boxgap+boxthick+0.5*(poffset+pdrop), bbX/2., bbX/2., 0.);
      
    TGeoTranslation* trprb = new TGeoTranslation("trprb", 0., 0.5*(poffset-pdrop)+pheight/4., -bbox_hlen-phlength);
    trprb->RegisterYourself();
    TGeoCompositeShape *cspb = new TGeoCompositeShape("cspb","logicbbL + logicPrizmBox:trprb");
    bbox = new TGeoVolume("DrcBarBox", cspb, gGeoManager->GetMedium("DIRCcarbonFiber"));
  }
  bbox->SetLineColor(30);
  
  // create air boxes:
  TGeoBBox* logicbbS;
  TGeoVolume *abox;
  if(fprizm == kFALSE){
    logicbbS = new TGeoBBox("logicbbS", bbX/2., hthick+boxgap, bbox_hlen);
    abox = new TGeoVolume("DrcAirBox", logicbbS, gGeoManager->GetMedium("DIRCairNoSens"));
    bbox->AddNode(abox, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  }
  if(fprizm == kTRUE){
    logicbbS = new TGeoBBox("logicbbS", bbX/2., hthick+boxgap, bbox_hlen-0.5*boxgap);
    TGeoTrap* logicPrizmContainer = new TGeoTrap("logicPrizmContainer", phlength+0.5*boxgap, pangle/2., 270.,
    hthick+boxgap+0.5*(poffset+pdrop+pheight), bbX/2., bbX/2., 0., hthick+boxgap+0.5*(poffset+pdrop), bbX/2., bbX/2., 0.);
    
    TGeoTranslation* trprc = new TGeoTranslation("trprc", 0., 0.5*(poffset-pdrop)+pheight/4., -bbox_hlen-phlength);
    trprc->RegisterYourself();
    TGeoCompositeShape *cspc = new TGeoCompositeShape("cspc","logicbbS + logicPrizmContainer:trprc");  
    abox = new TGeoVolume("DrcAirBox", cspc, gGeoManager->GetMedium("DIRCairNoSens"));
    bbox->AddNode(abox, 0, new TGeoCombiTrans(0., 0., -0.5*boxthick, new TGeoRotation(0)));
  }
  abox->SetLineColor(19);
  
  Double_t dx_bbox, dy_bbox, dz_bbox, phi_curr;    
      
  for(Int_t m = 0; m < bbnum; m ++){       
    phi_curr = (90. - phi0 - dphi*m)/180.*pi;    
    if(m > bbnum/2-1){ phi_curr = (90. - phi0 - dphi*m - 2.*pipehAngle)/180.*pi; }
    dx_bbox = radius * cos(phi_curr);
    dy_bbox = radius * sin(phi_curr);
    if(fprizm == 0){
      dz_bbox = bbox_shift;
    } 
    if(fprizm == 1){
      dz_bbox = bbox_shift + 0.5*(boxgap+boxthick);
    }  
    TGeoRotation rot_bbox;    
    rot_bbox.RotateZ( -phi0 - m*dphi - (TMath::Floor(2.*m/bbnum))*(2.*pipehAngle));    
    vLocalMother->AddNode(bbox, m+1, new TGeoCombiTrans(dx_bbox, dy_bbox, dz_bbox, new TGeoRotation(rot_bbox)));    
  }

  // create logic bar:  
  TGeoBBox* logicBar = new TGeoBBox("logicBar",  ((bbX/barnum)/2.)-bargap, hthick, bbox_hlen-fabs(len)/2.-mirr_hthick);
  TGeoVolume* bar;
  if(fprizm == kFALSE){  
    bar = new TGeoVolume("DrcBarSensor",logicBar, gGeoManager->GetMedium("FusedSil"));
  }
  if(fprizm == kTRUE){
     TGeoTrap* logicPrizm = new TGeoTrap("logicPrizm", phlength, pangle/2., 270., hthick+0.5*(poffset+pdrop+pheight),
    (bbX/barnum/2.)-bargap, (bbX/barnum/2.)-bargap, 0., hthick+0.5*(poffset+pdrop), (bbX/barnum/2.)-bargap, (bbX/barnum/2.)-bargap, 0.);
   
    TGeoTranslation* trpr = new TGeoTranslation("trpr", 0., 0.5*(poffset-pdrop)+pheight/4., -(bbox_hlen-fabs(len)/2.-mirr_hthick)-phlength);
    trpr->RegisterYourself();
    TGeoCompositeShape *csbp = new TGeoCompositeShape("csbp","logicBar + logicPrizm:trpr");
    bar = new TGeoVolume("DrcBarSensor",csbp, gGeoManager->GetMedium("FusedSil"));
    fAtBarEnd = "DrcBarSensor";
    fSlabEnd = -bbox_hlen + bbox_shift + 2.*phlength;
  }
  bar->SetLineColor(kCyan-9);
  bar->SetTransparency(50);    
  AddSensitiveVolume(bar);
  
  // create logic mirror:
  TGeoBBox* logicMirror  = new TGeoBBox("logicMirror", bbX/barnum/2.-bargap, hthick, mirr_hthick);
  TGeoVolume *mirr  = new TGeoVolume("DrcMirr", logicMirror,  gGeoManager->GetMedium("Mirror"));
  mirr->SetLineColor(5);
    
  Double_t dx, dy, dz_bar, dz_mirr;
  
  for(Int_t j=0; j<barnum; j++){
    dx      = - (bbX/2.) + (bbX/barnum)/2. + j * (bbX/barnum); 
    dy  = 0.;
    if(fprizm == kFALSE){
      dz_bar = len/2.-mirr_hthick;
      dz_mirr = bbox_hlen - mirr_hthick;
      if(fFocusingSystem == 1){ // lens
        abox->AddNode(lens1, 1+j ,new TGeoCombiTrans(dx, dy, fdz_lens1, new TGeoRotation (0)));
	abox->AddNode(lens2, 1+j ,new TGeoCombiTrans(dx, dy, fdz_lens2, new TGeoRotation (0)));
        abox->AddNode(lens3, 1+j ,new TGeoCombiTrans(dx, dy, fdz_lens3, new TGeoRotation (0)));
      }
      if(fFocusingSystem == 2){ // mirror
        abox->AddNode(block1, 1+j, new TGeoCombiTrans(dx, dy, fdz_mirr1, new TGeoRotation (0)));
        abox->AddNode(block2, 1+j, new TGeoCombiTrans(dx, dy, fdz_mirr2, new TGeoRotation (0)));
      }
    }
    if(fprizm == kTRUE){
      dz_bar  = -mirr_hthick - 0.5*boxgap;
      dz_mirr = bbox_hlen -0.5*boxgap - mirr_hthick;
    }        
    abox->AddNode(bar,  1+j, new TGeoCombiTrans(dx, dy, dz_bar, new TGeoRotation(0)));
    if(fFocusingSystem != 2){
     abox->AddNode(mirr, 1+j, new TGeoCombiTrans(dx, dy, dz_mirr, new TGeoRotation(0)));
    }
  }

  // Expansion volume:
  TGeoPcon* logicEV = new TGeoPcon("logicEV", 0., 360., 2);
  if(fprizm == kFALSE){
    logicEV->DefineSection(0, 0.,      radius-hthick,  EVRout);
    logicEV->DefineSection(1, EVlen, radius-hthick,  (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi));
  }
  if(fprizm == kTRUE){
    logicEV->DefineSection(0, 0.,      radius-hthick-pdrop-EVdrop,  EVRprizm);
    logicEV->DefineSection(1, EVlen-2.*phlength, radius-hthick-pdrop-EVdrop,  radius+hthick+poffset+pheight+EVoffset);
  }
  TGeoVolume* baseEV = new TGeoVolume("DrcEV", logicEV, gGeoManager->GetMedium("Marcol82"));
  baseEV->SetLineColor(kMagenta+2);
  baseEV->SetTransparency(50);
  vLocalMother->AddNode(baseEV, 1, new TGeoCombiTrans(0.,0.,EVshift, new TGeoRotation(0)));
  
  // PhotoDetector:
  TGeoPcon *logicPD = new TGeoPcon("logicPD", 0., 360., 2);
  if(fprizm == kFALSE){
    logicPD->DefineSection(0, 0.0, radius-hthick, EVRout);
    logicPD->DefineSection(1, 0.1, radius-hthick, EVRout);  
  }
  if(fprizm == kTRUE){
    logicPD->DefineSection(0, 0.0, radius-hthick, EVRprizm);
    logicPD->DefineSection(1, 0.1, radius-hthick, EVRprizm);  
  }
  TGeoVolume *pd = new TGeoVolume("DrcPDSensor", logicPD, gGeoManager->GetMedium("FusedSil"));
  pd->SetLineColor(kGreen-6);
  vLocalMother->AddNode(pd, 1,new TGeoCombiTrans(0., 0., EVshift-0.1, new TGeoRotation (rot1)));  
  AddSensitiveVolume(pd);
    
  cout<<"bars ends at = "<<fSlabEnd<<endl;    
 // gGeoManager->CloseGeometry();

}

// -----   Public Method Construct Optical Geometry ---------------------------
void PndDrc::ConstructOpGeometry()
{
  cout<< " ==================================================== " << endl;
  cout<< " =======  DRC::  ConstructOpticalGeometry()  ======== " << endl; 
  
  Int_t npoints = 2;
  
  Double_t ephoton[npoints];
  ephoton[0] = 1.0e-09;  // 1 eV
  ephoton[1] = 10.0e-09; // 10 eV
  Double_t reflectivity[npoints];
  reflectivity[0] = 1.;
  reflectivity[1] = 1.;
  Double_t efficiency[npoints];
  efficiency[0] = 0.5;
  efficiency[1] = 0.5;
    
  gMC->DefineOpSurface("BarMirSurface",kUnified, kDielectric_dielectric, kPolished, 0.1);
  for(Int_t i=0; i<fGeo->barNum(); i++){  
    gMC->SetBorderSurface("BarMirSurface", "DrcBarSensor", i+1, "DrcMirr", i+1, "BarSurface");  
  } 
/*    
  gMC->DefineOpSurface("EVSurface", kGlisur, kDielectric_dielectric, kPolished, 0.1);
  gMC->SetBorderSurface("EVSurface", "DrcEV",1, "BarrelDIRC",0,"EVSurface");
*/   
  gMC->SetMaterialProperty("BarSurface", "REFLECTIVITY", npoints, ephoton, reflectivity);
  gMC->SetMaterialProperty("BarSurface", "EFFICIENCY",   npoints, ephoton, efficiency);
/*  
  gMC->SetMaterialProperty("EVSurface", "REFLECTIVITY", npoints, ephoton, reflectivity);
  gMC->SetMaterialProperty("EVSurface", "EFFICIENCY",   npoints, ephoton, efficiency);
*/       
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
