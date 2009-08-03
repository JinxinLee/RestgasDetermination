#include <iomanip>
#include <iostream>
#include <TString.h>
void createSetup()
{

   TFile * f = TFile::Open("transformations.root");
  // Gem/SI/Scint disk geometry parameters
  //-----------------------------
  const Int_t     kNofGEMs = 2;
  const Double_t  kGEMdX[kNofGEMs]             = { 5.12, 5.12};
  const Double_t  kGEMdY[kNofGEMs]             = { 5.12, 5.12};
  const Double_t  kGEMZPosition  [kNofGEMs]    = { 10., 70.0};

  const Int_t     kNofSIs = 2;
  const Double_t  kSIdX[kNofSIs]             = { 0.96, 0.96};
  const Double_t  kSIdY[kNofSIs]             = { 0.96, 0.96};
  const Double_t  kSIZPosition  [kNofSIs]    = { 30., 50.0};

  const Int_t     kNofScints=2;
  const Double_t  kScintdX[kNofScints]             = { 1.0, 1.0};
  const Double_t  kScintdY[kNofScints]             = { 1.0, 1.0};
  const Double_t kHalfStationThickness = 2.85;
  //GEM layers
  //*************************************************************************************************************
  const Int_t    kNofLayersGEM = 20;
  const TString  kLayerGEMName [kNofLayersGEM] = {"WindowF_kapton","WindowF_copper",
						  "space",
						  "CathodeF_kapto1n","CathodeF_copper",
						  "space",
						  "Gem1F_copper","Gem1_Sensor_kapton","Gem1B_copper",
						  "space",
						  "Gem2F_copper","Gem2_kapton","Gem2B_copper",
						  "space",
						  "Gem3F_copper","Gem3_kapton","Gem3B_copper",
						  "space",
						  "PadF_copper","Pad_Sensor_G10"};
  const Double_t kLayerGEMThick[kNofLayersGEM] = {0.0050,  0.0005,          //     2 window
						  1.9235,                   // +1= 3 space
						  0.0050,  0.0005,          // +2= 5 cathode
						  0.3000,                   // +1= 6 space
						  0.0005,  0.0050,  0.0005, // +3= 9 gemfoil
						  0.2000,                   // +1=10 space
						  0.0005,  0.0050,  0.0005, // +3=13 gemfoil
						  0.2000,                   // +1=14 space
						  0.0005,  0.0050,  0.0005, // +3=17 gemfoil
						  0.2000,                   // +1=18 space
						  0.0012,  0.0500};        // +2=20 padplane

  const Double_t kGEMStripAngle[1][2] = {  0.  ,  90. };
  const Double_t kGEMStripPitch[1][2] = {  0.04,  0.04};
  //SI Layers
  //************************************************************************************************************
  const Int_t    kNofLayersSI = 9;
  const TString  kLayerSIName [kNofLayersSI] = {"WindowF_mylar","WindowF_aluminium",    //2      window
						"space",                                //+1 = 3 space
						"SIWaferF_silicon",                       //+1 = 4 wafer
						"space",                                //+1 = 5 space
						"SIWaferB_silicon",                       //+1 = 6 wafer
						"space",                                //+1 = 7 space
						"WindowB_aluminium","WindowB_mylar"};   //+2 = 9 window
  const Double_t kLayerSIThick[kNofLayersSI] = {  0.0050,  0.0005,          //     2 window
						  4.9545,                   // +1= 3 space
						  0.0300,                   // +1= 4 wafer
						  0.3700,                   // +1= 5 space
						  0.0300,                   // +1= 6 wafer
						  3.0045,                   // +1= 7 space
						  0.0005,  0.0050};         // +2= 9 window


  const Double_t kSIStripAngle[1][2] = {  0.  ,  90. };
  const Double_t kSIStripPitch[1][2] = {  0.005,  0.005};
  //Scint Layers
  //*************************************************************************************************************
  const Int_t kNofLayersScint = 1;
  const TString kLayerScintName[kNofLayersScint] = {"scint_G10"};
  
  const Double_t kLayerScintThick[kNofLayersScint] = {0.4};
  //*******************************************************************************************************				  
  
  //calculating totel thickness to find z-center of detector
  Double_t firstLayerOffsetGEM = 0.;
  Double_t firstLayerOffsetSI = 0.;
  Double_t firstLayerOffsetScint = 0.;
  
  for ( Int_t ilayer = 0 ; ilayer < kNofLayersGEM ; ilayer++ ) {
    cout << kLayerGEMName[ilayer].Data() << " -> " << kLayerGEMThick[ilayer] << endl;
    firstLayerOffsetGEM += kLayerGEMThick[ilayer];
  }

  for ( Int_t ilayer = 0 ; ilayer < kNofLayersSI ; ilayer++ ) {
    cout << kLayerSIName[ilayer].Data() << " -> " << kLayerSIThick[ilayer] << endl;
    firstLayerOffsetSI += kLayerSIThick[ilayer];
  }
  for ( Int_t ilayer = 0 ; ilayer < kNofLayersScint ; ilayer++ ) {
    cout << kLayerScintName[ilayer].Data() << " -> " << kLayerScintThick[ilayer] << endl;
    firstLayerOffsetScint += kLayerScintThick[ilayer];
  }

  cout << "total GEM thickness is " << firstLayerOffsetGEM << endl;
  firstLayerOffsetGEM = firstLayerOffsetGEM/2.;
  firstLayerOffsetGEM = -0.001*(TMath::Floor(1000.*firstLayerOffsetGEM)); //rounding
  cout << "first GEMlayer offset is " << firstLayerOffsetGEM << endl;

  cout << "total SI thickness is " << firstLayerOffsetSI << endl;
  firstLayerOffsetSI = firstLayerOffsetSI/2.;
  firstLayerOffsetSI = -0.001*(TMath::Floor(1000.*firstLayerOffsetSI)); //rounding
  cout << "first SIlayer offset is " << firstLayerOffsetSI << endl;

  //**********************************************************************************************************************
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  TString vmcWorkdir = getenv("VMCWORKDIR");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  TString outfile= "beamTelescope.root";
  TFile *fi = new TFile(outfile,"RECREATE");  

  FairGeoLoader *geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  //Creating media for setup
  geoFace->setMediaFile("../../../../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();
  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();
  FairGeoMedium *CbmMediumAir       = Media->getMedium("air");
  FairGeoMedium *CbmMediumPWO       = Media->getMedium("PWO");
  FairGeoMedium *CbmMediumCarbon    = Media->getMedium("carbon");
  FairGeoMedium *CbmMediumAluminium = Media->getMedium("aluminium");
  FairGeoMedium *CbmMediumCopper    = Media->getMedium("copper");
  FairGeoMedium *CbmMediumKapton    = Media->getMedium("kapton");
  FairGeoMedium *CbmMediumArCO2     = Media->getMedium("arco2_91_2atm");
  FairGeoMedium *CbmMediumG10       = Media->getMedium("G10");
  FairGeoMedium *CbmMediumSIlicon   = Media->getMedium("silicon");
  FairGeoMedium *CbmMediumMylar     = Media->getMedium("mylar");
  
  Int_t nmed=geobuild->createMedium(CbmMediumAir);
  nmed=geobuild->createMedium(CbmMediumPWO);
  nmed=geobuild->createMedium(CbmMediumCarbon);
  nmed=geobuild->createMedium(CbmMediumAluminium);
  nmed=geobuild->createMedium(CbmMediumCopper);
  nmed=geobuild->createMedium(CbmMediumKapton);
  nmed=geobuild->createMedium(CbmMediumArCO2);
  nmed=geobuild->createMedium(CbmMediumG10);
  nmed=geobuild->createMedium(CbmMediumSIlicon);
  nmed=geobuild->createMedium(CbmMediumMylar);

  TGeoManager *gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom"); // do I need this?
  TGeoVolume *top = new TGeoVolumeAssembly("beamTelescope");
  gGeoMan->SetTopVolume(top);
  
  cout << "-------------------------------------------------------------------" << endl;
  TList *mediaList = (TList*)gGeoMan->GetListOfMedia();
  cout << "media known: " << mediaList->GetEntries() << "." << endl;
  for ( Int_t itemp = 0 ; itemp < mediaList->GetEntries() ; itemp++ ) {
    TGeoMedium *medium = (TGeoMedium*)mediaList->At(itemp);
    cout << "medium " << itemp << " name is " << medium->GetName() << endl;
  }
  cout << "-------------------------------------------------------------------" << endl;

  TGeoRotation *dummyrot = new TGeoRotation();//rotation and translation

  TGeoVolumeAssembly *SubunitVol = new TGeoVolumeAssembly("Gem_Disks");

  TGeoShape       *DiskShape[kNofGEMs+kNofSIs];
  TGeoVolume      *DiskVol  [kNofGEMs+kNofSIs];
  TGeoTranslation *DiskTrans[kNofGEMs+kNofSIs];
  TGeoCombiTrans  *DiskCombi[kNofGEMs+kNofSIs];
  //okey, since kNofLayersGEM>kNofLayersGEM
  TGeoShape       *DiskLayersShape[kNofGEMs+kNofSIs][kNofLayersGEM];
  TGeoVolume      *DiskLayersVol  [kNofGEMs+kNofSIs][kNofLayersGEM];
  TGeoTranslation *DiskLayersTrans[kNofGEMs+kNofSIs][kNofLayersGEM];
  TGeoCombiTrans  *DiskLayersCombi[kNofGEMs+kNofSIs][kNofLayersGEM];

  TString outParFileName = Form("%s/macro/params/gem_2stations.digi.par",vmcWorkdir.Data());
  
  cout << "parameter file = \"" << outParFileName.Data() << "\"" << endl;

  ofstream pout(outParFileName.Data());
  pout.setf(ios::fixed);

  pout << "###################################################################" << endl;
  pout << "# Digitization parameters for Beam Telescoope                      " << endl;
  pout << "# with 2 GEM stations at BLABLA cm from the target                 " << endl;
  pout << "# and  2 SI stations at BLABLA cm from the target                  " << endl;
  pout << "# Format:                                                          " << endl;
  pout << "#Description of parameters:                                        " << endl;
  pout << "#[PndGemDetectors]                                                 " << endl;
  pout << "#parameters:d station_number, rotation_angle, number_of_sensors, \\" << endl;
  pout << "#                 sensor_number, sensor_type, pos_x, pos_y, pos_z, rotAngle, inn_rad, out_rad, thick, str_ang_0, str_ang_1, pitch_0, pitch_1, \\" << endl;
  pout << "#                 sensor_number, ...." << endl;
  pout << "#                 station_number, ..." << endl;
  pout << "##################################################################" << endl;
  pout << "[PndGemDetectors]" << setprecision(4) << endl;
  pout << "parameters:Double_t \\" << endl;
  //****************************************************************************************************************
  //
  //GEM creation
  //
  //****************************************************************************************************************
  for ( Int_t istat = 0 ; istat < kNofGEMs ; istat++ ) {

    pout << "                    " << istat+1 << ",  "
	 << setw(9) << kGEMZPosition[istat]
	 << ",  0.0, " << 2 << ", \\" << endl;
    
    DiskShape[istat] = new TGeoBBox (Form("disk%dshape",istat+1),
				     kGEMdX[istat],kGEMdY[istat],
				     kHalfStationThickness);
    DiskVol  [istat] = new TGeoVolume(Form("Gem_Disk%d_Volume",istat+1),
				      DiskShape[istat],
				      gGeoMan->GetMedium("arco2_91_2atm"));
    //    DiskVol  [istat] = new TGeoVolume(Form("Gem_Disk%d_Volume",istat+1),DiskShape[istat],gGeoMan->GetMedium("air"));
    if(istat==0){
      TGeoTranslation * gm1xTrans = (TGeoTranslation *) f->Get("gm1xTrans");
      TGeoRotation * gm1xRot = (TGeoRotation *) f->Get("gm1xRot");
      DiskTrans[istat] = new TGeoTranslation(*gm1xTrans);
      cout << "station " << kGEMdX[istat] << " " << kGEMdY[istat] << " at " << kGEMZPosition[istat] << endl;
      DiskCombi[istat] = new TGeoCombiTrans(*DiskTrans[istat],*gm1xRot); 
      DiskCombi[istat]->SetName(Form("Gem_Disk%d_Volume",istat+1));
      DiskCombi[istat]->RegisterYourself();
    }else{
      TGeoTranslation * gm2xTrans = (TGeoTranslation *) f->Get("gm2xTrans");
      TGeoRotation * gm2xRot = (TGeoRotation *) f->Get("gm2xRot");
      DiskTrans[istat] = new TGeoTranslation(*gm2xTrans);
      cout << "station " << kGEMdX[istat] << " " << kGEMdY[istat] << " at " << kGEMZPosition[istat] << endl;
      DiskCombi[istat] = new TGeoCombiTrans(*DiskTrans[istat],*gm2xRot); 
      DiskCombi[istat]->SetName(Form("Gem_Disk%d_Volume",istat+1));
      DiskCombi[istat]->RegisterYourself();
    }
    Double_t layerPosition = firstLayerOffsetGEM;//-kLayerThick[0]/2.;

    Int_t sensorNumber = 0;
    
    for ( Int_t ilay = 0 ; ilay < kNofLayersGEM ; ilay++ ) {
      layerPosition += kLayerGEMThick[ilay]/2.;
      if ( kLayerGEMName[ilay].Contains("space") && kLayerGEMName[ilay].Length() == 5 ) {
	layerPosition += kLayerGEMThick[ilay]/2.;
	continue;
      }

      DiskLayersShape[istat][ilay] = new TGeoBBox  (Form("disk%d%sshape",istat+1, kLayerGEMName[ilay].Data()),
						    kGEMdX[istat],kGEMdY[istat],
						    kLayerGEMThick[ilay]/2.);
      TString layerMaterial = kLayerGEMName[ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      DiskLayersVol  [istat][ilay] = new TGeoVolume(Form("Gem_Disk%d_%s",istat+1,kLayerGEMName[ilay].Data()),
						    DiskLayersShape[istat][ilay],
						    gGeoMan->GetMedium(layerMaterial.Data()));
      
      cout << "volume " << kLayerGEMName[ilay] << " from " 
	   << setprecision(10) << kGEMZPosition[istat]+layerPosition-kLayerGEMThick[ilay]/2. << " to "
	   << setprecision(10) << kGEMZPosition[istat]+layerPosition+kLayerGEMThick[ilay]/2. << endl;

      if (kLayerGEMName[ilay].Contains("Gem") && kLayerGEMName[ilay].Contains("Sensor")) {
	Double_t nofStrips = TMath::Ceil(2.*kGEMdX[istat]/kGEMStripPitch[sensorNumber][0]);
	Double_t newRadius = nofStrips*kGEMStripPitch[sensorNumber][0]/2.;
	cout << "!!!! " << istat << " " << ilay 
	     << " > there shall be " << nofStrips 
	     << " strips here so the radius should be " << newRadius << endl;
	pout << "                        " << sensorNumber+1 << ",  1,  " 
	     << setw(9) << 0. << ",  "
	     << setw(9) << 0. << ",  " 
	     << setw(9) << kGEMZPosition[istat]+layerPosition << ",  "
	     << setw(9) << 0. << ",  "
	     << setw(9) << newRadius << ",  "
	     << setw(9) << kGEMdY[istat] << ",  "
	     << setw(9) << kLayerGEMThick[ilay] << ",  "
	     << setw(9) << kGEMStripAngle[sensorNumber][0] << ",  "
	     << setw(9) << kGEMStripAngle[sensorNumber][1] << ",  "
	     << setw(9) << kGEMStripPitch[sensorNumber][0] << ",  "
	     << setw(9) << kGEMStripPitch[sensorNumber][1] 
	     << ((istat==kNofGEMs-1&&sensorNumber==1)?" ":",") << " \\" << endl;
	sensorNumber++;
      }
      //      cout << "layer material = " << layerMaterial.Data() << endl;
      if ( layerMaterial.Contains("copper" ) ){
	DiskLayersVol[istat][ilay]->SetLineColor(2);
      }
      if ( layerMaterial.Contains("kapton" ) ){
	DiskLayersVol[istat][ilay]->SetLineColor(3);
      }
      if ( layerMaterial.Contains("aluminium" ) ){
	DiskLayersVol[istat][ilay]->SetLineColor(4);
      }
      if ( layerMaterial.Contains("G10" ) ){
	DiskLayersVol[istat][ilay]->SetLineColor(5);
      }
      
      DiskLayersTrans[istat][ilay] = new TGeoTranslation(0.,0.,layerPosition);
      DiskLayersCombi[istat][ilay] = new TGeoCombiTrans(*DiskLayersTrans[istat][ilay],*dummyrot);
      DiskLayersCombi[istat][ilay]->SetName(Form("Gem_Disk%d_%s",istat+1,kLayerGEMName[ilay].Data()));
      DiskLayersCombi[istat][ilay]->RegisterYourself();
      DiskVol[istat]->AddNode(DiskLayersVol  [istat][ilay],0,DiskLayersCombi[istat][ilay]);
      
      layerPosition += kLayerGEMThick[ilay]/2.;
    } //end looping over GEM-layers
    SubunitVol->AddNode(DiskVol[istat],0,DiskCombi[istat]);
  }//end looping over gems
    
  //****************************************************************************************************************
  //
  //Silicon Creation
  //
  //****************************************************************************************************************
for ( Int_t istat = 0 ; istat < kNofSIs ; istat++ ) {

    pout << "                    " << istat+1 << ",  "
	 << setw(9) << kSIZPosition[istat]
	 << ",  0.0, " << 2 << ", \\" << endl;
    
    DiskShape[istat+kNofGEMs] = new TGeoBBox (Form("disk%dshape",istat+kNofGEMs+1),
				     kSIdX[istat],kSIdY[istat],
				     kHalfStationThickness);
    DiskVol  [istat+kNofGEMs] = new TGeoVolume(Form("Gem_Disk%d_Volume",istat+kNofGEMs+1),
				      DiskShape[istat],
				      gGeoMan->GetMedium("air"));
    //    DiskVol  [istat] = new TGeoVolume(Form("Gem_Disk%d_Volume",istat+1),DiskShape[istat],gGeoMan->GetMedium("air"));
    if(istat==0){
      TGeoTranslation * si1xTrans = (TGeoTranslation *) f->Get("si1xTrans");
      TGeoRotation * si1xRot = (TGeoRotation *) f->Get("si1xRot");
      DiskTrans[istat+kNofGEMs] = new TGeoTranslation(*si1xTrans);
      cout << "station " << kSIdX[istat] << " " << kSIdY[istat] << " at " << kSIZPosition[istat] << endl;
      DiskCombi[istat+kNofGEMs] = new TGeoCombiTrans(*DiskTrans[istat+kNofGEMs],*si1xRot); 
      DiskCombi[istat+kNofGEMs]->SetName(Form("Gem_Disk%d_Volume",istat+kNofGEMs+1));
      DiskCombi[istat+kNofGEMs]->RegisterYourself();
    }else{
      TGeoTranslation * si2xTrans = (TGeoTranslation *) f->Get("si2xTrans");
      TGeoRotation * si2xRot = (TGeoRotation *) f->Get("si2xRot");
      DiskTrans[istat+kNofGEMs] = new TGeoTranslation(*si2xTrans);
      cout << "station " << kSIdX[istat] << " " << kSIdY[istat] << " at " << kSIZPosition[istat] << endl;
      DiskCombi[istat+kNofGEMs] = new TGeoCombiTrans(*DiskTrans[istat+kNofGEMs],*si2xRot); 
      DiskCombi[istat+kNofGEMs]->SetName(Form("Gem_Disk%d_Volume",istat+kNofGEMs+1));
      DiskCombi[istat+kNofGEMs]->RegisterYourself();
    }

    Double_t layerPosition = firstLayerOffsetSI;//-kLayerThick[0]/2.;

    Int_t sensorNumber = 0;
    
    for ( Int_t ilay = 0 ; ilay < kNofLayersSI ; ilay++ ) {
      layerPosition += kLayerSIThick[ilay]/2.;
      if ( kLayerSIName[ilay].Contains("space") && kLayerSIName[ilay].Length() == 5 ) {
	layerPosition += kLayerSIThick[ilay]/2.;
	continue;
      }

      DiskLayersShape[istat+kNofGEMs][ilay] = new TGeoBBox  (Form("disk%d%sshape",istat+kNofGEMs+1, kLayerSIName[ilay].Data()),
						    kSIdX[istat],kSIdY[istat],
						    kLayerSIThick[ilay]/2.);
      TString layerMaterial = kLayerSIName[ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      DiskLayersVol  [istat+kNofGEMs][ilay] = new TGeoVolume(Form("Gem_Disk%d_%s",istat+kNofGEMs+1,kLayerSIName[ilay].Data()),
						    DiskLayersShape[istat+kNofGEMs][ilay],
						    gGeoMan->GetMedium(layerMaterial.Data()));
      
      cout << "volume " << kLayerSIName[ilay] << " from " 
	   << setprecision(10) << kSIZPosition[istat]+layerPosition-kLayerSIThick[ilay]/2. << " to "
	   << setprecision(10) << kSIZPosition[istat]+layerPosition+kLayerSIThick[ilay]/2. << endl;

      if (kLayerSIName[ilay].Contains("Gem") && kLayerSIName[ilay].Contains("Sensor")) {
	Double_t nofStrips = TMath::Ceil(2.*kSIdX[istat]/kSIStripPitch[sensorNumber][0]);
	Double_t newRadius = nofStrips*kSIStripPitch[sensorNumber][0]/2.;
	cout << "!!!! " << istat+kNofGEMs << " " << ilay 
	     << " > there shall be " << nofStrips 
	     << " strips here so the radius should be " << newRadius << endl;
	pout << "                        " << sensorNumber+1 << ",  1,  " 
	     << setw(9) << 0. << ",  "
	     << setw(9) << 0. << ",  " 
	     << setw(9) << kSIZPosition[istat]+layerPosition << ",  "
	     << setw(9) << 0. << ",  "
	     << setw(9) << newRadius << ",  "
	     << setw(9) << kSIdY[istat] << ",  "
	     << setw(9) << kLayerSIThick[ilay] << ",  "
	     << setw(9) << kSIStripAngle[sensorNumber][0] << ",  "
	     << setw(9) << kSIStripAngle[sensorNumber][1] << ",  "
	     << setw(9) << kSIStripPitch[sensorNumber][0] << ",  "
	     << setw(9) << kSIStripPitch[sensorNumber][1] 
	     << ((istat+kNofGEMs==kNofGEMs-1&&sensorNumber==1)?" ":",") << " \\" << endl;
	sensorNumber++;
      }
      //      cout << "layer material = " << layerMaterial.Data() << endl;
      if ( layerMaterial.Contains("mylar" ) ){
	DiskLayersVol[istat+kNofSIs][ilay]->SetLineColor(6);
      }
      if ( layerMaterial.Contains("silicon" ) ){
	DiskLayersVol[istat+kNofSIs][ilay]->SetLineColor(7);
      }
      if ( layerMaterial.Contains("aluminium" ) ){
	DiskLayersVol[istat+kNofSIs][ilay]->SetLineColor(4);
      }
      
      DiskLayersTrans[istat+kNofGEMs][ilay] = new TGeoTranslation(0.,0.,layerPosition);
      DiskLayersCombi[istat+kNofGEMs][ilay] = new TGeoCombiTrans(*DiskLayersTrans[istat+kNofSIs][ilay],*dummyrot);
      DiskLayersCombi[istat+kNofGEMs][ilay]->SetName(Form("Gem_Disk%d_%s",istat+kNofGEMs+1,kLayerSIName[ilay].Data()));
      DiskLayersCombi[istat+kNofGEMs][ilay]->RegisterYourself();
      DiskVol[istat+kNofGEMs]->AddNode(DiskLayersVol  [istat+kNofGEMs][ilay],0,DiskLayersCombi[istat+kNofGEMs][ilay]);
      
      layerPosition += kLayerSIThick[ilay]/2.;
    } //end looping over SI-layers
    SubunitVol->AddNode(DiskVol[istat+kNofGEMs],0,DiskCombi[istat+kNofGEMs]);
  }//end looping over SIs

  //****************************************************************************************************************
  //
  //scintillator Creation
  //
  //****************************************************************************************************************
for ( Int_t istat = 0 ; istat < kNofGEMs ; istat++ ) {

    
    DiskShape[istat+kNofGEMs] = new TGeoBBox (Form("disk%dshape",istat+kNofGEMs+1),
				     kScintdX[istat],kScintdY[istat],
				     kHalfStationThickness);
    DiskVol  [istat+kNofGEMs] = new TGeoVolume(Form("Gem_Disk%d_Volume",istat+kNofGEMs+1),
				      DiskShape[istat],
				      gGeoMan->GetMedium("air"));
    //    DiskVol  [istat] = new TGeoVolume(Form("Gem_Disk%d_Volume",istat+1),DiskShape[istat],gGeoMan->GetMedium("air"));
    if(istat==0){
      TGeoTranslation * scint1Trans = (TGeoTranslation *) f->Get("scint1Trans");
      TGeoRotation * scintRot = (TGeoRotation *) f->Get("scintRot");
      DiskTrans[istat+kNofGEMs] = new TGeoTranslation(*scint1Trans);
      cout << "station " << kScintdX[istat] << " " << kScintdY[istat] <<endl;
      DiskCombi[istat+kNofGEMs] = new TGeoCombiTrans(*DiskTrans[istat+kNofGEMs],*scintRot); 
      DiskCombi[istat+kNofGEMs]->SetName(Form("Gem_Disk%d_Volume",istat+kNofGEMs+1));
      DiskCombi[istat+kNofGEMs]->RegisterYourself();
    }else{
      TGeoTranslation * scint2Trans = (TGeoTranslation *) f->Get("scint2Trans");
      TGeoRotation * scintRot = (TGeoRotation *) f->Get("scintRot");
      DiskTrans[istat+kNofGEMs] = new TGeoTranslation(*scint2Trans);
      cout << "station " << kScintdX[istat] << " " << kScintdY[istat]  << endl;
      DiskCombi[istat+kNofGEMs] = new TGeoCombiTrans(*DiskTrans[istat+kNofGEMs],*scintRot); 
      DiskCombi[istat+kNofGEMs]->SetName(Form("Gem_Disk%d_Volume",istat+kNofGEMs+1));
      DiskCombi[istat+kNofGEMs]->RegisterYourself();
    }

    Double_t layerPosition = firstLayerOffsetScint;//-kLayerThick[0]/2.;

    Int_t sensorNumber = 0;
    
    for ( Int_t ilay = 0 ; ilay < kNofLayersScint ; ilay++ ) {
      layerPosition += kLayerScintThick[ilay]/2.;
      if ( kLayerScintName[ilay].Contains("space") && kLayerScintName[ilay].Length() == 5 ) {
	layerPosition += kLayerScintThick[ilay]/2.;
	continue;
      }

      DiskLayersShape[istat+kNofGEMs][ilay] = new TGeoBBox  (Form("disk%d%sshape",istat+kNofGEMs+1, kLayerScintName[ilay].Data()),
						    kScintdX[istat],kScintdY[istat],
						    kLayerScintThick[ilay]/2.);
      TString layerMaterial = kLayerScintName[ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      DiskLayersVol  [istat+kNofGEMs][ilay] = new TGeoVolume(Form("Gem_Disk%d_%s",istat+kNofGEMs+1,kLayerScintName[ilay].Data()),
						    DiskLayersShape[istat+kNofGEMs][ilay],
						    gGeoMan->GetMedium(layerMaterial.Data()));
      
      cout << "volume " << kLayerScintName[ilay] << " from " 
	   << setprecision(10) << layerPosition-kLayerScintThick[ilay]/2. << " to "
	   << setprecision(10) << layerPosition+kLayerScintThick[ilay]/2. << endl;

  
      //      cout << "layer material = " << layerMaterial.Data() << endl;
      if ( layerMaterial.Contains("mylar" ) ){
	DiskLayersVol[istat+kNofScints][ilay]->SetLineColor(6);
      }
      if ( layerMaterial.Contains("silicon" ) ){
	DiskLayersVol[istat+kNofScints][ilay]->SetLineColor(7);
      }
      if ( layerMaterial.Contains("aluminium" ) ){
	DiskLayersVol[istat+kNofScints][ilay]->SetLineColor(4);
      }
      
      DiskLayersTrans[istat+kNofGEMs][ilay] = new TGeoTranslation(0.,0.,layerPosition);
      DiskLayersCombi[istat+kNofGEMs][ilay] = new TGeoCombiTrans(*DiskLayersTrans[istat+kNofScints][ilay],*dummyrot);
      DiskLayersCombi[istat+kNofGEMs][ilay]->SetName(Form("Gem_Disk%d_%s",istat+kNofGEMs+1,kLayerScintName[ilay].Data()));
      DiskLayersCombi[istat+kNofGEMs][ilay]->RegisterYourself();
      DiskVol[istat+kNofGEMs]->AddNode(DiskLayersVol  [istat+kNofGEMs][ilay],0,DiskLayersCombi[istat+kNofGEMs][ilay]);
      
      layerPosition += kLayerScintThick[ilay]/2.;
    } //end looping over Scint-layers
    SubunitVol->AddNode(DiskVol[istat+kNofGEMs],0,DiskCombi[istat+kNofGEMs]);
  }//end looping over Scints





  top->AddNode(SubunitVol,0,new TGeoCombiTrans());
  
  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
  //   gGeoManager->Export(outfile);
  top->Draw("ogl");

  pout.close();

}  
