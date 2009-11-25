#include "iomanip.h"

void create4StationsGem()
{
  // Gem disk geometry parameters
  //-----------------------------
  const Int_t     kNofDisks = 4;

  const Double_t  kDiskInnerRadius[kNofDisks]    = {  5.0,   5.0,   5.0,   5.0};
  const Double_t  kDiskOuterRadius[kNofDisks]    = { 42.0,  42.0,  66.0,  90.0};
  const Double_t  kDiskZPosition  [kNofDisks]    = { 90.0, 120.0, 150.0, 180.0};

  const Double_t kHalfStationThickness = 3.00;

  const Int_t    kNofLayers = 39;
  const TString  kLayerName [kNofLayers] = {"WindowF_kapton","WindowF_aluminium",
					    "space",
					    "CathodeF_kapton","CathodeF_aluminium",
					    "space",
					    "Gem1F_copper","Gem1_Sensor_kapton","Gem1B_copper",
					    "space",
					    "Gem2F_copper","Gem2_kapton","Gem2B_copper",
					    "space",
					    "Gem3F_copper","Gem3_kapton","Gem3B_copper",
					    "space",
					    "PadF_copper","Pad_kapton","Pad_copper",
					    "space",
					    "Gem4F_copper","Gem4_kapton","Gem4B_copper",
					    "space",
					    "Gem5F_copper","Gem5_kapton","Gem5B_copper",
					    "space",
					    "Gem6F_copper","Gem6_Sensor_kapton","Gem6B_copper",
					    "space",
					    "CathodeB_aluminium","CathodeB_kapton",
					    "space",
					    "WindowB_aluminium","WindowB_kapton"};
  const Double_t kLayerThick[kNofLayers] = {  0.0007,  0.0001,          //     2 window
					      1.0010,                   // +1= 3 space
					      0.0007,  0.0001,          // +2= 5 cathode
					      1.0020,                   // +1= 6 space
					      0.0002,  0.0050,  0.0002, // +3= 9 gemfoil
					      0.2010,                   // +1=10 space
					      0.0002,  0.0050,  0.0002, // +3=13 gemfoil
					      0.2010,                   // +1=14 space
					      0.0002,  0.0050,  0.0002, // +3=17 gemfoil
					      0.2010,                   // +1=18 space
					      0.0012,  0.0050,  0.0012, // +3=21 padplane
					      0.2010,                   // +1=22 space
					      0.0002,  0.0050,  0.0002, // +3=25 gemfoil
					      0.2010,                   // +1=26 space
					      0.0002,  0.0050,  0.0002, // +3=29 gemfoil
					      0.2010,                   // +1=30 space
					      0.0002,  0.0050,  0.0002, // +3=31 gemfoil
					      1.0020,                   // +1=34 space
					      0.0001,  0.0007,          // +2=36 cathode
					      1.0010,                   // +1=37 space
					      0.0001,  0.0007};         // +2=39 window

  const Double_t kSensorStripAngle[2][2] = {  0.  , 90.  , 60.  ,-60.  };
  const Double_t kSensorStripPitch[2][2] = {  0.02,  0.02,  0.03,  0.03};

  Double_t firstLayerOffset = 0.;

  for ( Int_t ilayer = 0 ; ilayer < kNofLayers ; ilayer++ ) {
    cout << kLayerName[ilayer].Data() << " -> " << kLayerThick[ilayer] << endl;
    firstLayerOffset += kLayerThick[ilayer];
  }

  cout << "total thickness is " << firstLayerOffset << endl;
  firstLayerOffset = firstLayerOffset/2.;
  firstLayerOffset = -0.001*(TMath::Floor(1000.*firstLayerOffset));
  cout << "first layer offset is " << firstLayerOffset << endl;

  //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  TString vmcWorkdir = getenv("VMCWORKDIR");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  TString outfile= "../../geometry/gem_4Stations.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
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
  FairGeoMedium *CbmMediumArCO2     = Media->getMedium("GEMmixture");

  Int_t nmed=geobuild->createMedium(CbmMediumAir);
  nmed=geobuild->createMedium(CbmMediumPWO);
  nmed=geobuild->createMedium(CbmMediumCarbon);
  nmed=geobuild->createMedium(CbmMediumAluminium);
  nmed=geobuild->createMedium(CbmMediumCopper);
  nmed=geobuild->createMedium(CbmMediumKapton);
  nmed=geobuild->createMedium(CbmMediumArCO2);

  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  TGeoVolume *top = new TGeoVolumeAssembly("Gem");
                    
  gGeoMan->SetTopVolume(top);
  
  cout << "-------------------------------------------------------------------" << endl;
  TList* mediaList = (TList*)gGeoMan->GetListOfMedia();
  //TList* mediaList = (TList*)Media->getListOfMedia();
  cout << "media known: " << mediaList->GetEntries() << "." << endl;
  for ( Int_t itemp = 0 ; itemp < mediaList->GetEntries() ; itemp++ ) {
    TGeoMedium* medium = (TGeoMedium*)mediaList->At(itemp);
    cout << "medium " << itemp << " name is " << medium->GetName() << endl;
  }
  cout << "-------------------------------------------------------------------" << endl;

  TGeoRotation* dummyrot = new TGeoRotation();

  TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Gem_Disks");

  TGeoShape       *DiskShape[kNofDisks];
  TGeoVolume      *DiskVol  [kNofDisks];
  TGeoTranslation *DiskTrans[kNofDisks];
  TGeoCombiTrans  *DiskCombi[kNofDisks];

  TGeoShape       *DiskLayersShape[kNofDisks][kNofLayers];
  TGeoVolume      *DiskLayersVol  [kNofDisks][kNofLayers];
  TGeoTranslation *DiskLayersTrans[kNofDisks][kNofLayers];
  TGeoCombiTrans  *DiskLayersCombi[kNofDisks][kNofLayers];

  TString outParFileName = Form("%s/macro/params/gem_4Stations.digi.par",vmcWorkdir.Data());
  
  cout << "parameter file = \"" << outParFileName.Data() << "\"" << endl;

  ofstream pout(outParFileName.Data());
  pout.setf(ios::fixed);

  pout << "#################################################################" << endl;
  pout << "# Digitization parameters for GEM                                " << endl;
  pout << "# with 4 Stations at 90, 120, 150, 180 cm from the target        " << endl;
  pout << "# Format:                                                        " << endl;
  pout << "#Description of parameters:                                      " << endl;
  pout << "#[PndGemDetectors]                                            " << endl;
  pout << "#parameters:d station_number, rotation_angle, number_of_sensors, \\" << endl;
  pout << "#                 sensor_number, sensor_type, pos_x, pos_y, pos_z, rotAngle, inn_rad, out_rad, thick, str_ang_0, str_ang_1, pitch_0, pitch_1, \\" << endl;
  pout << "#                 sensor_number, ...." << endl;
  pout << "#             station_number, ..." << endl;
  pout << "#################################################################" << endl;
  pout << "[PndGemDetectors]" << setprecision(4) << endl;

  pout << "parameters:Double_t \\" << endl;

  for ( Int_t istat = 0 ; istat < kNofDisks ; istat++ ) {

    pout << "                    " << istat+1 << ",  "
	 << setw(9) << kDiskZPosition[istat]
	 << ",  0.0, " << 2 << ", \\" << endl;
    
    DiskShape[istat] = new TGeoTube  (Form("disk%dshape",istat+1),kDiskInnerRadius[istat],kDiskOuterRadius[istat],kHalfStationThickness);
    DiskVol  [istat] = new TGeoVolume(Form("Gem_Disk%d_Volume",istat+1),DiskShape[istat],gGeoMan->GetMedium("GEMmixture"));
    DiskTrans[istat] = new TGeoTranslation(0.,0.,kDiskZPosition[istat]);
    cout << "station " << kDiskInnerRadius[istat] << " " << kDiskOuterRadius[istat] << " at " << kDiskZPosition[istat] << endl;
    DiskCombi[istat] = new TGeoCombiTrans(*DiskTrans[istat],*dummyrot); 
    DiskCombi[istat]->SetName(Form("Gem_Disk%d_Volume",istat+1));
    DiskCombi[istat]->RegisterYourself();

    Double_t layerPosition = firstLayerOffset;//-kLayerThick[0]/2.;

    Int_t sensorNumber = 0;
    
    for ( Int_t ilay = 0 ; ilay < kNofLayers ; ilay++ ) {
      layerPosition += kLayerThick[ilay]/2.;
      if ( kLayerName[ilay].Contains("space") && kLayerName[ilay].Length() == 5 ) {
	layerPosition += kLayerThick[ilay]/2.;
	continue;
      }

      DiskLayersShape[istat][ilay] = new TGeoTube  (Form("disk%d%sshape",istat+1,kLayerName[ilay].Data()),
						    kDiskInnerRadius[istat],kDiskOuterRadius[istat],
						    kLayerThick[ilay]/2.);
      TString layerMaterial = kLayerName[ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      DiskLayersVol  [istat][ilay] = new TGeoVolume(Form("Gem_Disk%d_%s",istat+1,kLayerName[ilay].Data()),
						    DiskLayersShape[istat][ilay],
						    gGeoMan->GetMedium(layerMaterial.Data()));

      cout << "volume " << kLayerName[ilay] << " from " 
	   << setprecision(10) << kDiskZPosition[istat]+layerPosition-kLayerThick[ilay]/2. << " to "
	   << setprecision(10) << kDiskZPosition[istat]+layerPosition+kLayerThick[ilay]/2. << endl;


      if ( kLayerName[ilay].Contains("Gem") && kLayerName[ilay].Contains("Sensor") ) {
	Double_t nofStrips = TMath::Ceil(2.*TMath::Pi()*kDiskInnerRadius[istat]/kSensorStripPitch[sensorNumber][0]);
	Double_t newRadius = nofStrips*kSensorStripPitch[sensorNumber][0]/2./TMath::Pi();
	cout << "!!!! " << istat << " " << ilay << " > there shall be " << nofStrips << " strips here so the radius should be " << newRadius << endl;
	pout << "                        " << sensorNumber+1 << ",  1,  " 
	     << setw(9) << 0. << ",  "
	     << setw(9) << 0. << ",  " 
	     << setw(9) << kDiskZPosition[istat]+layerPosition << ",  "
	     << setw(9) << 0. << ",  "
	     << setw(9) << newRadius << ",  "
	     << setw(9) << kDiskOuterRadius[istat] << ",  "
	     << setw(9) << kLayerThick[ilay] << ",  "
	     << setw(9) << kSensorStripAngle[sensorNumber][0] << ",  "
	     << setw(9) << kSensorStripAngle[sensorNumber][1] << ",  "
	     << setw(9) << kSensorStripPitch[sensorNumber][0] << ",  "
	     << setw(9) << kSensorStripPitch[sensorNumber][1] << ((istat==kNofDisks-1&&sensorNumber==1)?"":", \\") 
	     << endl;
	sensorNumber++;
      }

      //      cout << "layer material = " << layerMaterial.Data() << endl;
      if ( layerMaterial.Contains("copper" ) )
	DiskLayersVol[istat][ilay]->SetLineColor(2);
      if ( layerMaterial.Contains("kapton" ) )
	DiskLayersVol[istat][ilay]->SetLineColor(3);
      if ( layerMaterial.Contains("aluminium" ) )
	DiskLayersVol[istat][ilay]->SetLineColor(4);
      DiskLayersTrans[istat][ilay] = new TGeoTranslation(0.,0.,layerPosition);
      DiskLayersCombi[istat][ilay] = new TGeoCombiTrans(*DiskLayersTrans[istat][ilay],*dummyrot);
      DiskLayersCombi[istat][ilay]->SetName(Form("Gem_Disk%d_%s",istat+1,kLayerName[ilay].Data()));
      DiskLayersCombi[istat][ilay]->RegisterYourself();
      DiskVol[istat]->AddNode(DiskLayersVol  [istat][ilay],0,DiskLayersCombi[istat][ilay]);
      
      layerPosition += kLayerThick[ilay]/2.;
    }
    SubunitVol->AddNode(DiskVol[istat],0,DiskCombi[istat]);
  }
  pout << "TrackFinderOnHits_ParThetaA: Double_t 59.4" << endl
       << "TrackFinderOnHits_ParThetaB: Double_t -0.02" << endl
       << "TrackFinderOnHits_ParTheta0: Double_t 56.1372" << endl
       << "TrackFinderOnHits_ParTheta1: Double_t -0.000564362" << endl
       << "TrackFinderOnHits_ParTheta2: Double_t -0.181828" << endl
       << "TrackFinderOnHits_ParTheta3: Double_t  0.284289" << endl
       << "TrackFinderOnHits_ParRadPhi0:Double_t  0.9837086" << endl
       << "TrackFinderOnHits_ParRadPhi2:Double_t -0.000182412" << endl
       << "TrackFinderOnHits_ParMat0:   Double_t \\" << endl
       << "                                      -2.31333e-6,    0.00067035,    0.10173" << endl
       << "TrackFinderOnHits_ParMat1:   Double_t \\" << endl
       << "                                      -7.46844e-10,   -6.6696e-7,    0.000736672" << endl
       << "##########################################################################################" << flush;

  top->AddNode(SubunitVol,0,new TGeoCombiTrans());
  
  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
  //   gGeoManager->Export(outfile);
  top->Draw("ogl");

  pout.close();

}  
