#include "iomanip.h"

void create1StationsGem_firstStation()
{ // create only GEM with one station
  //----------------------------------------------------------------------------------------------------------------------------------------------
  // Gem disk geometry parameters (R.Karabowicz) 
  //----------------------------------------------------------------------------------------------------------------------------------------------
  // Done Some Changes By Nazila Divani-Veis ( since  May 2015 ... ) create GEM first station at the ZPosition=119.40cm , (it is the smallest one )
  //----------------------------------------------------------------------------------------------------------------------------------------------
  // Try to get close to the CAD Geometry ( After could not use CadConverter ) //Some parameters from DL.
  //----------------------------------------------------------------------------------------------------------------------------------------------
  // Units are in [cm]
  //---------------------------------------------------------------------------------------------------------------------------------------------

    const Int_t     kNofDisks = 1;   //number of GEM stations in form of disk

    const Double_t  kDiskVolInnerRadius[kNofDisks]    = { 4.50  };    // InnerRadius for one Station in form of disk
    const Double_t  kDiskVolOuterRadius[kNofDisks]    = { 45.00  };   // OuterRadius for one Station in form of disk
    const Double_t  kDiskZPosition  [kNofDisks]    = { 119.40 };      // ZPosition   for one Station in form of disk           
 

    const Int_t     kDiskNFoils     [kNofDisks]    = {  2    };   // For modifying misalignment
    const Double_t  kMiddleROBarHfTh[kNofDisks]    = {  2.35 };   // half thickness of space in the middle of foils [cm]
    const Double_t  kHalfStationThickness = 6.00;

    const Double_t carbonRingInnerRadius[kNofDisks]    = { 40.0 };
    const Double_t carbonRingOuterRadius[kNofDisks]    = { 40.2 };
    const Double_t carbonRingHalfThickness = 1.5;

    const Double_t copperRingInnerRadius[kNofDisks]    = { 39.9 };
    const Double_t copperRingOuterRadius[kNofDisks]    = { 40.1 };
    const Double_t copperRingHalfThickness = 3.75;

    const Double_t AlRingInnerRadius[kNofDisks]    = { 40.1 };
    const Double_t AlRingOuterRadius[kNofDisks]    = { 44.7 };
    const Double_t AlRingHalfThickness = 3.75;

    const Double_t  SegmentHalfThickness = 0.25; 
    const Int_t     NofSegments = 24;
    const Double_t  SegmentDeltaAngle = 360./(Double_t(NofSegments));
    const Double_t  FirstSegmentAngle = 7.5;
    const TString   newsegment [NofSegments] = { "seg1" ,"seg2" ,"seg3" ,"seg4" ,"seg5" ,"seg6" ,"seg7" ,"seg8" ,
                                                 "seg9" ,"seg10","seg11","seg12","seg13","seg14","seg15","seg16",
                                                 "seg17","seg18","seg19","seg20","seg21","seg22","seg23","seg24"  };

    const Double_t moduleRingInnerRadius[kNofDisks]    = { 40.15 };
    const Double_t moduleRingOuterRadius[kNofDisks]    = { 44.65 };
    const Double_t moduleRingHalfThickness = 3.75;

    const Double_t  moduleSegmentHalfThickness = 1.30; 
    const Int_t     NofmoduleSegments = 24;
    const Double_t  moduleSegmentDeltaAngle = 360./(Double_t(NofmoduleSegments));
    const Double_t  FirstmoduleSegmentAngle = 0.0 ;
    const TString   newmodulesegment [NofmoduleSegments] = { "moduleseg1" ,"moduleseg2" ,"moduleseg3" ,"moduleseg4" ,"moduleseg5" ,"moduleseg6" ,"moduleseg7" ,"moduleseg8" ,
                                                             "moduleseg9" ,"moduleseg10","moduleseg11","moduleseg12","moduleseg13","moduleseg14","moduleseg15","moduleseg16",
                                                             "moduleseg17","moduleseg18","moduleseg19","moduleseg20","moduleseg21","moduleseg22","moduleseg23","moduleseg24"  };

    const Double_t AlumiRingInnerRadius[kNofDisks]    = { 44.7 };
    const Double_t AlumiRingOuterRadius[kNofDisks]    = { 45.0 };
    const Double_t AlumiRingHalfThickness = 3.75;
    
    const Double_t coverRingInnerRadius[kNofDisks]    = { 39.9 };
    const Double_t coverRingOuterRadius[kNofDisks]    = { 45.0 };
    const Double_t coverRingHalfThickness = 0.2;
    
  //------------------------------------------------------------ main layers in shape of Disk----------------------------------------------------------------------
    const Int_t     kNofLayers = 47;
    const TString   kLayerName [kNofLayers] = { "window_foil_out_aluminium","WindowF_kapton",               
    					        "space",                                           // these "spaces" belong to holding structure layers
                                                "WindowF_aluminium","CathodeF_kapton","CathodeF_aluminium", 
                                                "space",
    					        "Gem1_Sensor_GEMmixture",      // sensitive layer
    					        "space",
                                                "Gem1F_copper","Gem1_kapton","Gem1B_copper",             
    					        "space",
    					        "Gem2F_copper","Gem2_kapton","Gem2B_copper",                                   
    					        "space",
    					        "Gem3F_copper","Gem3_kapton","Gem3B_copper",                                   
    					        "space",
					        "space",
    					        "PadF_copper","Pad_kapton","PadB_copper", 
    					        "space",
					        "space",
    					        "Gem4F_copper","Gem4_kapton","Gem4B_copper",             
    					        "space",
    					        "Gem5F_copper","Gem5_kapton","Gem5B_copper",                                  
    					        "space",
    					        "Gem6F_copper","Gem6_kapton","Gem6B_copper", 
					        "space",                                 
    					        "Gem6_Sensor_GEMmixture",    // sensitive layer
					        "space",
					        "CathodeB_aluminium","CathodeB_kapton","WindowB_aluminium",                                   
    					        "space",
    					        "WindowB_kapton","window_foil_in_aluminium" };               
    const Double_t kLayerThick[kNofLayers] = {  0.0001,   0.0007,                                                              //  2 = 2  window
					        1.00,                       // windowF_ring_carbon=holding structure           // +1 = 3  space
                                                0.0001,   0.0007,   0.0001,                                                    // +3 = 6  window + cathode
    					        0.80,                       // cathodeF_ring_GlassFiber=holding structure      // +1 = 7  space
    					        1.0020,                                                                        // +1 = 8  SENSOR
					        0.0050,                     // gem_ring1_GlassFiber=holding structure          // +1 = 9  space
                                                0.0005,   0.0050,   0.0005,                                                    // +3 = 12 GEM Foil
    					        0.0050,                     // gem_ring2_GlassFiber=holding structure          // +1 = 13 space                
    					        0.0005,   0.0050,   0.0005,                                                    // +3 = 16 GEM Foil
                                                0.0050,                     // gem_ring3_GlassFiber=holding structure          // +1 = 17 space             
    					        0.0005,   0.0050,   0.0005,                                                    // +3 = 20 GEM Foil
                                                0.0050,                     // gem_ring4_GlassFiber=holding structure          // +1 = 21 space
                                                0.10,                       // padplaneF_support_GlassFiber=holding structure  // +1 = 22 space
                                                0.001,    0.001,    0.001,                                                     // +3 = 25 PAD plane
					        0.10,                       // padplaneB_support_GlassFiber=holding structure  // +1 = 26 space
    					        0.0050,                     // gem_ring5_GlassFiber=holding structure          // +1 = 27 space
                                                0.0005,   0.0050,   0.0005,                                                    // +3 = 30 GEM Foil
                                                0.0050,                     // gem_ring6_GlassFiber=holding structure          // +1 = 31 space
                                                0.0005,   0.0050,   0.0005,                                                    // +3 = 34 GEM Foil
    					        0.0050,                     // gem_ring7_GlassFiber=holding structure          // +1 = 35 space
                                                0.0005,   0.0050,   0.0005,                                                    // +3 = 38 GEM Foil
                                                0.0050,                     // gem_ring8_GlassFiber=holding structure          // +1 = 39 space
                                                1.0020,                                                                        // +1 = 40 SENSOR
                                                0.80,                       // cathodeB_ring_GlassFiber=holding structure      // +1 = 41 space
                                                0.0001,   0.0007,  0.0001,                                                     // +3 = 44 cathode + window
                                                1.00,                       // windowB_ring_carbon=holding structure           // +1 = 45 space 
                                                0.0007,   0.0001 };                                                            // +2 = 47 window
   const Double_t kDiskOuterRadius[kNofLayers][kNofDisks] = {  38.95,       39.40, 
							       39.90, 
							       38.45,       39.40,    38.45, 
							       39.40, 
							       38.45, 
							       39.05,  
							       38.10,       39.05,    38.10, 
							       39.05, 
							       38.10,       39.05,    38.10, 
							       39.05,
							       38.10,       39.05,    38.10, 
							       39.05,                                                    
							       45.00, 
							       44.90,       44.90,    44.90, 
							       45.00, 
							       39.05,  
							       38.10,       39.05,    38.10,
							       39.05, 
							       38.10,       39.05,    38.10, 
							       39.05, 
							       38.10,       39.05,    38.10, 
							       39.05,                                                         
							       38.45, 
							       39.40, 
							       38.45,       39.40,    38.45, 
							       39.40, 
							       39.40,       38.95  };
   const Double_t kDiskInnerRadius[kNofLayers][kNofDisks] = {  4.50,        4.50, 
      							       4.50,   
      							       4.50,        4.50,       4.50, 
      							       4.50,  
      							       4.50,  
      							       4.50,  
      							       4.50,        4.50,       4.50,  
     							       4.50,   
      							       4.50,        4.50,        4.50, 
      							       4.50,  
     							       4.50,        4.50,        4.50,  
      							       4.50,                                                     
      							       4.50,  
      							       4.50,        4.50,        4.50,  
      							       4.50,  
      							       4.50,  
     							       4.50,        4.50,       4.50,  
     							       4.50,  
     							       4.50,        4.50,      4.50,  
      							       4.50,  
      							       4.50,        4.50,        4.50,  
      							       4.50,                                                          
     							       4.50,   
      							       4.50,   
     							       4.50,        4.50,       4.50,  
     							       4.50,  
      							       4.50,        4.50   };

  const Double_t  HoleTZ = 0.0;         // (top and down) holes Translation parameters
  const Double_t  HoleTX = 0.0;
  const Double_t  HoleTY = 31.50; 

//----------------------- holding structure layers ------------------------------------------------------------------------------------------------
    const Int_t     NofHLayers = 14;
    const TString   HLayersName [NofHLayers] = {  "windowF_ring_carbon",             
   					          "cathodeF_ring_GlassFiber",         
    					          "gem_ring1_GlassFiber",
                                                  "gem_ring2_GlassFiber",           
   					          "gem_ring3_GlassFiber",    
    					          "gem_ring4_GlassFiber",                 
    					          "padplaneF_support_GlassFiber",
                                                  "padplaneB_support_GlassFiber",
    					          "gem_ring5_GlassFiber",
                                                  "gem_ring6_GlassFiber",           
    					          "gem_ring7_GlassFiber",                                  
   					          "gem_ring8_GlassFiber",                                   
   					          "cathodeB_ring_GlassFiber",                                 
    					          "windowB_ring_carbon"  };               
    const Double_t HLayersThick[NofHLayers] =  {    1.00,                                               
    					            0.80,                                                       
    					            0.0050,                                     
    					            0.0050,                                             
    					            0.0050,         
    					            0.0050,                                   
    					            0.10,     
    					            0.10,                                   
    					            0.0050,  
    					            0.0050,                                   
    				                    0.0050,           
    					            0.0050,                                                                      
    					            0.80,                                                     
    					            1.00    }; 
    const Double_t  HZPosition[NofHLayers][kNofDisks] = {  -1.9426,  
                                                           -1.5417,  
                                                           -0.1372,  
                                                           -0.1262,  
                                                           -0.1152,  
                                                           -0.1042,  
                                                           -0.0517,  
                                                            0.0517,   
                                                            0.1042,  
                                                            0.1152,  
                                                            0.1262,  
                                                            0.1372,   
                                                            1.5417,  
                                                            1.9426    }; 
   const Double_t HOuterRadius[NofHLayers][kNofDisks] = {  39.90,    
                                                           39.40,    
                                                           39.05,    
                                                           39.05,    
                                                           39.05,    
                                                           39.05,    
                                                           45.00,    
                                                           45.00,    
                                                           39.05,    
                                                           39.05,    
                                                           39.05,    
                                                           39.05,    
                                                           39.40,   
                                                           39.90     };
    const Double_t HInnerRadius[NofHLayers][kNofDisks] = { 39.65,    
                                                           38.50,    
                                                           38.05,    
                                                           38.05,    
                                                           38.05,    
                                                           38.05,    
                                                           38.50,    
                                                           38.50,    
                                                           38.05,    
                                                           38.05,    
                                                           38.05,     
                                                           38.05,    
                                                           38.50,     
                                                           39.65     };
 
   const Double_t  HXBoxWidth = 2.30;         // Using to define holding structure layers holes
   const Double_t  HXPlateWidth = 1.90;
   const Double_t  HYPlateWidth = 10.0;

   const Double_t  HTZ = 0.0;         // Translation parameters for vertical holes
   const Double_t  HTX = 0.0;    
   const Double_t  HTY[NofHLayers][kNofDisks] = { 25.0,
                                                  25.0, 
                                                  25.0,
                                                  25.0, 
                                                  25.0,
                                                  25.0, 
                                                  25.0, 
                                                  25.0,
                                                  25.0,
                                                  25.0, 
                                                  25.0,
                                                  25.0, 
                                                  25.0, 
                                                  25.0  };
//-------------------------------------------------------------------------------------------------------------------------------------------------
  const Int_t    kSensorStripType [2]    = {  3  ,  2   };  
  
  const Double_t kSensorStripAngle[2][2] = {  0.  ,  0.  ,  0.  ,  0.  };
  const Double_t kSensorStripPitch[2][2] = {  0.04,  0.04,  0.04,  0.04}; ///////////////////////

  Double_t firstLayerOffset = 0.;

  for ( Int_t ilayer = 0 ; ilayer < kNofLayers ; ilayer++ ) {
    cout << kLayerName[ilayer].Data() << " -> " << kLayerThick[ilayer] << endl;
    firstLayerOffset += kLayerThick[ilayer];
  }

  cout << "total thickness is " << firstLayerOffset << endl;
  firstLayerOffset = firstLayerOffset/2.;
  firstLayerOffset = -0.001*(TMath::Floor(1000.*firstLayerOffset));
  cout << "first layer offset is " << firstLayerOffset << endl;
  //-----------------------------------------------------------------------------------------------

  TString vmcWorkdir = getenv("VMCWORKDIR");
  
  TString outfile= "../../geometry/gem_1Station_first_realistic_v1.root"; // it means GEM with one station which is its' first station
  
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  cout << "created output file" << endl;
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  cout << "geoface setmediafile" << endl;
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  cout << "geoface readmedia" << endl;
  geoFace->readMedia();
  cout << "geoface print" << endl;
  geoFace->print();
  cout << "geoface done" << endl;
 
  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumAir       = Media->getMedium("air");
  FairGeoMedium *CbmMediumPWO       = Media->getMedium("PWO");
  FairGeoMedium *CbmMediumCarbon    = Media->getMedium("carbon");
  FairGeoMedium *CbmMediumAluminium = Media->getMedium("aluminium");
  FairGeoMedium *CbmMediumCopper    = Media->getMedium("copper");
  FairGeoMedium *CbmMediumKapton    = Media->getMedium("kapton");
  FairGeoMedium *CbmMediumArCO2     = Media->getMedium("GEMmixture"); 
  FairGeoMedium *CbmMediumUranium   = Media->getMedium("uranium");
  FairGeoMedium *CbmMediumGlassFiber= Media->getMedium("GlassFiber");

  Int_t nmed=geobuild->createMedium(CbmMediumAir);
  nmed=geobuild->createMedium(CbmMediumPWO);
  nmed=geobuild->createMedium(CbmMediumCarbon);
  nmed=geobuild->createMedium(CbmMediumAluminium);
  nmed=geobuild->createMedium(CbmMediumCopper);
  nmed=geobuild->createMedium(CbmMediumKapton);
  nmed=geobuild->createMedium(CbmMediumArCO2);
  nmed=geobuild->createMedium(CbmMediumUranium);
  nmed=geobuild->createMedium(CbmMediumGlassFiber);


  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  TGeoVolume *top = new TGeoVolumeAssembly("Gem");
                    
  gGeoMan->SetTopVolume(top);
  
  cout << "-------------------------------------------------------------------" << endl;
  TList* mediaList = (TList*)gGeoMan->GetListOfMedia();
   cout << "media known: " << mediaList->GetEntries() << "." << endl;
  for ( Int_t itemp = 0 ; itemp < mediaList->GetEntries() ; itemp++ ) {
    TGeoMedium* medium = (TGeoMedium*)mediaList->At(itemp);
    cout << "medium " << itemp << " name is " << medium->GetName() << endl;
  }
  cout << "-------------------------------------------------------------------" << endl;

  TGeoRotation* dummyrot = new TGeoRotation();

  TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Gem_Disks");

  //----------------------------------------------------------------------------------------
   TGeoShape          *DiskShape[kNofDisks];
   TGeoVolume         *DiskVol  [kNofDisks];
   TGeoTranslation    *DiskTrans[kNofDisks];
   TGeoCombiTrans     *DiskCombi[kNofDisks];
  //--------------------------------------------------------------------------------------------
   TGeoShape          *carbonRingShape[kNofDisks];
   TGeoVolume         *carbonRingVol  [kNofDisks];  
   TGeoTranslation    *carbonRingTrans[kNofDisks];
   TGeoCombiTrans     *carbonRingCombi[kNofDisks];
  //------------------------------------------------------------------------------------------
   TGeoShape          *copperRingShape[kNofDisks];
   TGeoVolume         *copperRingVol  [kNofDisks];  
   TGeoTranslation    *copperRingTrans[kNofDisks];
   TGeoCombiTrans     *copperRingCombi[kNofDisks];
  //-----------------------------------------------------------------------------------------
   TGeoShape          *coverRingShape[kNofDisks];
   TGeoVolume         *coverRingVol  [kNofDisks];  
   TGeoTranslation    *coverRingTrans[kNofDisks];
   TGeoCombiTrans     *coverRingCombi[kNofDisks];
    //-----------------------------------------------------------------------------------------
   TGeoShape          *AlRingShape[kNofDisks][NofSegments];
   TGeoVolume         *AlRingVol  [kNofDisks][NofSegments];  
   TGeoTranslation    *AlRingTrans[kNofDisks][NofSegments];
   TGeoCombiTrans     *AlRingCombi[kNofDisks][NofSegments];
  //-----------------------------------------------------------------------------------------
   TGeoShape          *moduleRingShape[kNofDisks][NofmoduleSegments];
   TGeoVolume         *moduleRingVol  [kNofDisks][NofmoduleSegments];  
   TGeoTranslation    *moduleRingTrans[kNofDisks][NofmoduleSegments];
   TGeoCombiTrans     *moduleRingCombi[kNofDisks][NofmoduleSegments];
  //-----------------------------------------------------------------------------------------
   TGeoShape          *AlumiRingShape[kNofDisks];
   TGeoVolume         *AlumiRingVol  [kNofDisks];  
   TGeoTranslation    *AlumiRingTrans[kNofDisks];
   TGeoCombiTrans     *AlumiRingCombi[kNofDisks];
 //----------------------------------------------------------------------------------------------
   TGeoShape          *DiskLayersShapeA     [kNofLayers][kNofDisks][4]; // 4 is number of seg
   TGeoShape          *DiskLayersShapeB     [kNofLayers][kNofDisks][4];
   TGeoSubtraction    *DiskLayersSubtr      [kNofLayers][kNofDisks][4];
   TGeoShape          *DiskLayersShapeC     [kNofLayers][kNofDisks][4]; // final, C = A-B
   TGeoShape          *DiskLayersShapeHole  [kNofLayers][kNofDisks][4];
   TGeoCompositeShape *DiskLayersShapecompos[kNofLayers][kNofDisks][4];
   TGeoVolume         *DiskLayersVol        [kNofLayers][kNofDisks][4];
   TGeoTranslation    *DiskLayersTranshA    [kNofLayers][kNofDisks][4];
   TGeoTranslation    *DiskLayersTranshB    [kNofLayers][kNofDisks][4];
   TGeoTranslation    *DiskLayersTrans      [kNofLayers][kNofDisks][4];
   TGeoCombiTrans     *DiskLayersCombi      [kNofLayers][kNofDisks][4];
 //------------------------------------------------------------------------------------------------
   TGeoShape          *HLayersShapeTube  [NofHLayers][kNofDisks];
   TGeoShape          *HLayersShapeBox   [NofHLayers][kNofDisks];
   TGeoShape          *HLayersShapePlate [NofHLayers][kNofDisks];  
   TGeoShape          *HLayersShapeHT    [NofHLayers][kNofDisks];  
   TGeoShape          *HLayersShapeHTM   [NofHLayers][kNofDisks];  
   TGeoShape          *HLayersShapeHTD   [NofHLayers][kNofDisks]; 
   TGeoCompositeShape *HLayersShapecompos[NofHLayers][kNofDisks]; 
   TGeoVolume         *HLayersVolcomp    [NofHLayers][kNofDisks];
   TGeoTranslation    *HLayersTranstA    [NofHLayers][kNofDisks];
   TGeoTranslation    *HLayersTranstB    [NofHLayers][kNofDisks];  
   TGeoTranslation    *HLayersTranstC    [NofHLayers][kNofDisks];  
   TGeoTranslation    *HLayersTranstD    [NofHLayers][kNofDisks];  
   TGeoTranslation    *HLayersTranstE    [NofHLayers][kNofDisks];  
   TGeoTranslation    *HLayersTranstF    [NofHLayers][kNofDisks];  
   TGeoRotation       *HLayersRotrA      [NofHLayers][kNofDisks];
   TGeoShape          *HLayersShape      [NofHLayers][kNofDisks];
   TGeoVolume         *HLayersVol        [NofHLayers][kNofDisks];
   TGeoTranslation    *HLayersTrans      [NofHLayers][kNofDisks];
   TGeoCombiTrans     *HLayersCombi      [NofHLayers][kNofDisks];
 //----------------------------------------------------------------------------------------------
    TString outParFileName = Form("%s/macro/params/gem_1Station_first_realistic_v1.digi.par",vmcWorkdir.Data());
   
  cout << "parameter file = \"" << outParFileName.Data() << "\"" << endl;

  ofstream pout(outParFileName.Data());
  pout.setf(ios::fixed);

  pout << "#################################################################" << endl;
  pout << "# Digitization parameters for GEM                                " << endl;
  pout << "# with 3 Stations                                                " << endl;
  pout << "# Format:                                                        " << endl;
  pout << "# Description of parameters:                                     " << endl;
  pout << "# [PndGemDetectors]                                              " << endl;
  pout << "# parameters:d station_number, ZPos, rotation_angle, number_of_sensors, \\" << endl;
  pout << "#                 sensor_number, sensor_type, pos_x, pos_y, pos_z, rotAngle, inn_rad, out_rad, thick, str_ang_0, str_ang_1, barWidth, pitch_0, pitch_1, \\" << endl;
  pout << "#                 sensor_number, ...." << endl;
  pout << "#             station_number, ..." << endl;
  pout << "#################################################################" << endl;
  pout << "[PndGemDetectors]" << setprecision(4) << endl;

  pout << "parameters:Double_t \\" << endl;

  for ( Int_t istat = 0 ; istat < kNofDisks ; istat++ ) {

    pout << "                   " << istat+1 << ",  "
	 << setw(9) << kDiskZPosition[istat]
	 << ",  0.0, " << 2 << ", \\" << endl;
    //-----------------------------------GEM Disk------------------------------------------------------------------------------------------------------
    DiskShape[istat] = new TGeoTube  (Form("disk%dshape",istat+1),kDiskVolInnerRadius[istat],kDiskVolOuterRadius[istat],kHalfStationThickness);
    DiskVol  [istat] = new TGeoVolume(Form("Gem_Disk%d_Volume",istat+1),DiskShape[istat],gGeoMan->GetMedium("GEMmixture"));
    DiskTrans[istat] = new TGeoTranslation(0.,0.,kDiskZPosition[istat]);
    cout << "station " << kDiskVolInnerRadius[istat] << " " << kDiskVolOuterRadius[istat] << " at " << kDiskZPosition[istat] << endl;
    DiskCombi[istat] = new TGeoCombiTrans(*DiskTrans[istat],*dummyrot); 
    DiskCombi[istat]->SetName(Form("Gem_Disk%d_Volume",istat+1));
    DiskCombi[istat]->RegisterYourself();
    DiskVol[istat]->SetLineColor(kYellow);
    //------------------------------------------------------------------------------------------------------------------------------------------
    //------------------------- Gas container Ring Bottom -------------------------------------------------------------------------------------
    carbonRingShape[istat] = new TGeoTube  (Form("carbonRing%dshape",istat+1),carbonRingInnerRadius[istat],carbonRingOuterRadius[istat],carbonRingHalfThickness);
    carbonRingVol  [istat] = new TGeoVolume(Form("Gem_carbonRing%d_Volume",istat+1),carbonRingShape[istat],gGeoMan->GetMedium("carbon"));
    carbonRingTrans[istat] = new TGeoTranslation(0.,0.,-1.5);
    cout << "carbonRing " << carbonRingInnerRadius[istat] << " " << carbonRingOuterRadius[istat] << endl;
    carbonRingCombi[istat] = new TGeoCombiTrans(*carbonRingTrans[istat],*dummyrot); 
    carbonRingCombi[istat]->SetName(Form("Gem_carbonRing%d_Volume",istat+1));
    carbonRingCombi[istat]->RegisterYourself();
    carbonRingVol[istat]->SetLineColor(kMagenta);
    //----------------------------------------------------------------------------------------------------------------------------------------
    DiskVol[istat]->AddNode(carbonRingVol  [istat],0,carbonRingCombi[istat]);
    //------------------------------------------------------------------------------------------------------------------------------------------
    //------------------------- Gas container Ring top -----------------------------------------------------------------------------------------
    copperRingShape[istat] = new TGeoTube  (Form("copperRing%dshape",istat+1),copperRingInnerRadius[istat],copperRingOuterRadius[istat],copperRingHalfThickness);
    copperRingVol  [istat] = new TGeoVolume(Form("Gem_copperRing%d_Volume",istat+1),copperRingShape[istat],gGeoMan->GetMedium("copper"));
    copperRingTrans[istat] = new TGeoTranslation(0.,0.,3.75);
    cout << "copperRing " << copperRingInnerRadius[istat] << " " << copperRingOuterRadius[istat] << endl;
    copperRingCombi[istat] = new TGeoCombiTrans(*copperRingTrans[istat],*dummyrot); 
    copperRingCombi[istat]->SetName(Form("Gem_copperRing%d_Volume",istat+1));
    copperRingCombi[istat]->RegisterYourself();
    copperRingVol[istat]->SetLineColor(kYellow-5);
    //----------------------------------------------------------------------------------------------------------------------------------------
    DiskVol[istat]->AddNode(copperRingVol  [istat],0,copperRingCombi[istat]);
    //----------------------------------------------------------------------------------------------------------------------------------------
    //----------------------segments for electronics --------------------------------------------------------------------------------------------------------
      Double_t segmentAngularSize = SegmentHalfThickness/AlRingInnerRadius[istat]*360.;
           for ( Int_t isegm = 0 ; isegm < NofSegments ; isegm++ ) {
      		 cout << " Segment " << isegm << " with name " << newsegment[isegm] << " will be put at " << FirstSegmentAngle+isegm*SegmentDeltaAngle << " (in fact from " << FirstSegmentAngle+isegm*SegmentDeltaAngle-segmentAngularSize/2. << " to " << FirstSegmentAngle+isegm*SegmentDeltaAngle+segmentAngularSize/2. << ")" << endl;
      	   AlRingShape[istat][isegm] = new TGeoTubeSeg (Form("AlRing%dshape",istat+1,isegm+1),AlRingInnerRadius[istat],AlRingOuterRadius[istat],AlRingHalfThickness,FirstSegmentAngle+isegm*SegmentDeltaAngle-segmentAngularSize/2.,FirstSegmentAngle+isegm*SegmentDeltaAngle+segmentAngularSize/2. );
      	   AlRingVol[istat][isegm]   = new TGeoVolume (Form("Gem_AlRing%d_Volume",istat+1,isegm+1),AlRingShape[istat][isegm],gGeoMan->GetMedium("aluminium")); 
           AlRingTrans[istat][isegm] = new TGeoTranslation (0.,0.,3.75);
            cout << "AlRing " << AlRingInnerRadius[istat] << " " << AlRingOuterRadius[istat] << endl;
           AlRingCombi[istat][isegm] = new TGeoCombiTrans (*AlRingTrans[istat][isegm],*dummyrot);
           AlRingCombi[istat][isegm]->SetName(Form("Gem_AlRing%d_Volume",istat+1,isegm+1));
           AlRingCombi[istat][isegm]->RegisterYourself(); 
           AlRingVol[istat][isegm]->SetLineColor(kCyan-9);
       //-----------------------------------------------------------------------------------------------------------------------------------------
	   DiskVol[istat]->AddNode(AlRingVol[istat][isegm],0,AlRingCombi[istat][isegm]);
       //------------------------------------------------------------------------------------------------------------------------------------------      
       }
    //----------------------------------------------------------------------------------------------------------------------------------------
    //---------------------- electronic module  --------------------------------------------------------------------------------------------------------
      Double_t modulesegmentAngularSize = moduleSegmentHalfThickness/moduleRingInnerRadius[istat]*360.;
           for ( Int_t imodulesegm = 0 ; imodulesegm < NofmoduleSegments ; imodulesegm++ ) {
      		 cout << " moduleSegment " << imodulesegm << " with name " << newmodulesegment[imodulesegm] << " will be put at " << FirstmoduleSegmentAngle+imodulesegm*moduleSegmentDeltaAngle << " (in fact from " << FirstmoduleSegmentAngle+imodulesegm*moduleSegmentDeltaAngle-modulesegmentAngularSize/2. << " to " << FirstmoduleSegmentAngle+imodulesegm*moduleSegmentDeltaAngle+modulesegmentAngularSize/2. << ")" << endl;
      	  moduleRingShape[istat][imodulesegm] = new TGeoTubeSeg (Form("moduleRing%dshape",istat+1,imodulesegm+1),moduleRingInnerRadius[istat],moduleRingOuterRadius[istat],moduleRingHalfThickness,FirstmoduleSegmentAngle+imodulesegm*moduleSegmentDeltaAngle-modulesegmentAngularSize/2.,FirstmoduleSegmentAngle+imodulesegm*moduleSegmentDeltaAngle+modulesegmentAngularSize/2. );
      	  moduleRingVol[istat][imodulesegm]   = new TGeoVolume (Form("Gem_moduleRing%d_Volume",istat+1,imodulesegm+1),moduleRingShape[istat][imodulesegm],gGeoMan->GetMedium("copper")); 
          moduleRingTrans[istat][imodulesegm] = new TGeoTranslation (0.,0.,3.65);
            cout << "moduleRing " << moduleRingInnerRadius[istat] << " " << moduleRingOuterRadius[istat] << endl;
          moduleRingCombi[istat][imodulesegm] = new TGeoCombiTrans (*moduleRingTrans[istat][imodulesegm],*dummyrot);
          moduleRingCombi[istat][imodulesegm]->SetName(Form("Gem_moduleRing%d_Volume",istat+1,imodulesegm+1));
          moduleRingCombi[istat][imodulesegm]->RegisterYourself(); 
          moduleRingVol[istat][imodulesegm]->SetLineColor(kGreen);
       //-----------------------------------------------------------------------------------------------------------------------------------------
	  DiskVol[istat]->AddNode(moduleRingVol[istat][imodulesegm],0,moduleRingCombi[istat][imodulesegm]);
       //------------------------------------------------------------------------------------------------------------------------------------------      
       }
   //---------------------------------Cooling Ring-------------------------------------------------------------------------------------------------------------
     AlumiRingShape[istat] = new TGeoTube  (Form("AlumiRing%dshape",istat+1),AlumiRingInnerRadius[istat],AlumiRingOuterRadius[istat],AlumiRingHalfThickness);
     AlumiRingVol  [istat] = new TGeoVolume(Form("Gem_AlumiRing%d_Volume",istat+1),AlumiRingShape[istat],gGeoMan->GetMedium("aluminium"));
     AlumiRingTrans[istat] = new TGeoTranslation(0.,0.,3.75);
     cout << "AlumiRing " << AlumiRingInnerRadius[istat] << " " << AlumiRingOuterRadius[istat] << endl;
     AlumiRingCombi[istat] = new TGeoCombiTrans(*AlumiRingTrans[istat],*dummyrot); 
     AlumiRingCombi[istat]->SetName(Form("Gem_AlumiRing%d_Volume",istat+1));
     AlumiRingCombi[istat]->RegisterYourself();
     AlumiRingVol[istat]->SetLineColor(kCyan-9);
   //----------------------------------------------------------------------------------------------------------------------------------------
     DiskVol[istat]->AddNode(AlumiRingVol[istat],0,AlumiRingCombi[istat]);
   //----------------------------------------------------------------------------------------------------------------------------------------
 //------------------------- GEM tracker cover electronic module  -----------------------------------------------------------------------------------------
    coverRingShape[istat] = new TGeoTube  (Form("coverRing%dshape",istat+1),coverRingInnerRadius[istat],coverRingOuterRadius[istat],coverRingHalfThickness);
    coverRingVol  [istat] = new TGeoVolume(Form("Gem_coverRing%d_Volume",istat+1),coverRingShape[istat],gGeoMan->GetMedium("GlassFiber"));
    coverRingTrans[istat] = new TGeoTranslation(0.,0.,7.75);
    cout << "coverRing " << coverRingInnerRadius[istat] << " " << coverRingOuterRadius[istat] << endl;
    coverRingCombi[istat] = new TGeoCombiTrans(*coverRingTrans[istat],*dummyrot); 
    coverRingCombi[istat]->SetName(Form("Gem_coverRing%d_Volume",istat+1));
    coverRingCombi[istat]->RegisterYourself();
    coverRingVol[istat]->SetLineColor(kGreen+3);
    //----------------------------------------------------------------------------------------------------------------------------------------
    DiskVol[istat]->AddNode(coverRingVol  [istat],0,coverRingCombi[istat]);
    //----------------------------------------------------------------------------------------------------------------------------------------        
  ///////////////////////////////////////////////holding structure layers////////////////////////////////////////////////////////////////////////////////////////////////
    for ( Int_t jlay = 0 ; jlay < NofHLayers ; jlay++ ) {
       cout << "doing Hlayers " << jlay << endl;

    HLayersShapeTube[jlay][istat] = new TGeoTube  (Form("T%dT%s",istat+1,HLayersName[jlay].Data()),HInnerRadius[jlay][istat],HOuterRadius[jlay][istat],HLayersThick[jlay]);
    HLayersShapeHT[jlay][istat]   = new TGeoTube  (Form("H%dH%s",istat+1,HLayersName[jlay].Data()),0.0,4.60,HLayersThick[jlay]);
    HLayersShapeHTM[jlay][istat]  = new TGeoTube  (Form("HTM%dHTM%s",istat+1,HLayersName[jlay].Data()),0.0,4.40,HLayersThick[jlay]+0.0002);
    HLayersShapeHTD[jlay][istat]  = new TGeoTube  (Form("HTD%dHTD%s",istat+1,HLayersName[jlay].Data()),0.0,1.90,HLayersThick[jlay]+0.0001);
	       cout << "Tube name is " << HLayersShapeTube[jlay][istat]->GetName() << endl;
	      
    HLayersShapeBox[jlay][istat]   = new TGeoBBox(Form("B%dB%s",istat+1,HLayersName[jlay].Data()),HXBoxWidth[jlay][istat],HOuterRadius[jlay][istat],HLayersThick[jlay]);
    HLayersShapePlate[jlay][istat] = new TGeoBBox(Form("P%dP%s",istat+1,HLayersName[jlay].Data()),HXPlateWidth[jlay][istat],HYPlateWidth[jlay][istat],HLayersThick[jlay]+0.0001);

    HLayersTranstA[jlay][istat] = new TGeoTranslation("tA",HTX,HTY[jlay][istat],HTZ);
     HLayersTranstA[jlay][istat] ->RegisterYourself();
    HLayersTranstB[jlay][istat] = new TGeoTranslation("tB",HTX,-HTY[jlay][istat],HTZ);
     HLayersTranstB[jlay][istat] ->RegisterYourself();
    HLayersTranstC[jlay][istat] = new TGeoTranslation("tC",HTX,HTY[jlay][istat]+9.0,HTZ);
     HLayersTranstC[jlay][istat] ->RegisterYourself();
    HLayersTranstD[jlay][istat] = new TGeoTranslation("tD",HTX,HTY[jlay][istat]-9.0,HTZ);
     HLayersTranstD[jlay][istat] ->RegisterYourself();
    HLayersTranstE[jlay][istat] = new TGeoTranslation("tE",HTX,-HTY[jlay][istat]+9.0,HTZ);
     HLayersTranstE[jlay][istat] ->RegisterYourself();
    HLayersTranstF[jlay][istat] = new TGeoTranslation("tF",HTX,-HTY[jlay][istat]-9.0,HTZ);
     HLayersTranstF[jlay][istat] ->RegisterYourself();

    HLayersShapecompos[jlay][istat] = new TGeoCompositeShape(Form("compos%dcompos%s",istat+1,HLayersName[jlay].Data()),
							     Form("T%dT%s+B%dB%s+H%dH%s-HTM%dHTM%s-(P%dP%s:tA)-(HTD%dHTD%s:tC)-(HTD%dHTD%s:tD)-(P%dP%s:tB)-(HTD%dHTD%s:tE)-(HTD%dHTD%s:tF)",
							   	  istat+1,HLayersName[jlay].Data(),istat+1,HLayersName[jlay].Data(),istat+1,HLayersName[jlay].Data(),istat+1,HLayersName[jlay].Data(),
                                                                  istat+1,HLayersName[jlay].Data(),istat+1,HLayersName[jlay].Data(),istat+1,HLayersName[jlay].Data(),istat+1,HLayersName[jlay].Data(),
							     	  istat+1,HLayersName[jlay].Data(),istat+1,HLayersName[jlay].Data()));
							   
      cout << "composite name is " <<  HLayersShapecompos[jlay][istat] -> GetName() << endl;
                    
    	TString HlayersMaterial = HLayersName[jlay].Data();
    	HlayersMaterial.Remove(0,HlayersMaterial.Last('_')+1);
    		cout << "THE HMATERIAL IS \"" << HlayersMaterial.Data() << "\"" << endl;
		HLayersVolcomp[jlay][istat] = new TGeoVolume(Form("GEMHLayersCOMP%dGEMHLayersCOMP%s",istat+1,HLayersName[jlay].Data()),HLayersShapecompos[jlay][istat],gGeoMan->GetMedium(HlayersMaterial.Data()));
		cout << "COMP name is " <<  HLayersVolcomp[jlay][istat]  -> GetName() << endl;
     	cout << "Hlayersmaterial = " << HlayersMaterial.Data() << endl;

          if ( HlayersMaterial.Contains("carbon" ) )
        	  HLayersVolcomp[jlay][istat]->SetLineColor(kPink);
         	if ( HlayersMaterial.Contains("GlassFiber" ) )
        	  HLayersVolcomp[jlay][istat]->SetLineColor(kGreen+3);

		cout << "STATION " << istat << " LAYER " << jlay << " POSITION " << HZPosition[jlay][istat] << endl;

		HLayersTrans[jlay][istat] = new TGeoTranslation(0.0,0.0,HZPosition[jlay][istat]);
          HLayersCombi[jlay][istat] = new TGeoCombiTrans(*HLayersTrans[jlay][istat],*dummyrot); 
          HLayersCombi[jlay][istat]->SetName(Form("GEMHLayersCOMP%dGEMHLayersCOMP%s",istat+1,HLayersName[jlay].Data()));
          HLayersCombi[jlay][istat]->RegisterYourself();
	  DiskVol[istat]->AddNode( HLayersVolcomp[jlay][istat],0,HLayersCombi[jlay][istat] );
		  } 
   ///////////////////////////////////////////////////////main layers////////////////////////////////////////////////////////////////////////////////////////////////

    Double_t layerPosition = firstLayerOffset;//-kLayerThick[0]/2.;

    Int_t sensorNumber = 0;
    
    for ( Int_t ilay = 0 ; ilay < kNofLayers ; ilay++ ) {
      cout << "doing layer " << ilay << endl;
      layerPosition += kLayerThick[ilay]/2.;
	if ( kLayerName[ilay].Contains("space") && kLayerThick[ilay] > 0.7 ) {
	  cout << "***** THE THICK SPACE LAYER IS AT " << layerPosition << endl;
	}
      if ( kLayerName[ilay].Contains("space") && kLayerName[ilay].Length() == 5 ) {
	layerPosition += kLayerThick[ilay]/2.;
	continue;
      }

                 cout << " HAHA, got layer " << kLayerName[ilay].Data() << endl;

      Double_t segPhiSpan = 360./(Double_t(kDiskNFoils[istat]));
      Double_t segBegin   =  90.;
      cout << "will do loop over segments" << endl;
      for ( Int_t iseg = 0 ; iseg < 1 ; iseg++ ) {
	cout << "segment " << iseg << endl;
	DiskLayersShapeA[ilay][istat][iseg] = new TGeoTube(Form("disk%dseg%d%sshape",istat+1,iseg+1,kLayerName[ilay].Data()),
							   kDiskInnerRadius[ilay][istat],kDiskOuterRadius[ilay][istat],
							   kLayerThick[ilay]/2.);
	DiskLayersShapeB[ilay][istat][iseg] = new TGeoBBox   (Form("robo%dseg%d%sshape",istat+1,iseg+1,kLayerName[ilay].Data()),
							      kMiddleROBarHfTh[istat]/2.,
							      kDiskOuterRadius[ilay][istat],
							      kLayerThick[ilay]);
	DiskLayersSubtr[ilay][istat][iseg] = new TGeoSubtraction(DiskLayersShapeA[ilay][istat][iseg],
								  DiskLayersShapeB[ilay][istat][iseg]);
       

	DiskLayersShapeC[ilay][istat][iseg] = new TGeoCompositeShape(Form("comp%dseg%d%sshape",istat+1,iseg+1,kLayerName[ilay].Data()),
								     DiskLayersSubtr[ilay][istat][iseg]);
	segBegin += segPhiSpan;
	cout << " segBegin " << segBegin << endl;

        DiskLayersShapeHole[ilay][istat][iseg] = new TGeoTube(Form("Hole%dseg%d%sshape",istat+1,iseg+1,kLayerName[ilay].Data()),0.0,3.80,kLayerThick[ilay]+0.001);

	 DiskLayersTranshA[ilay][istat][iseg] = new TGeoTranslation("hA",HoleTX,HoleTY,HoleTZ);
	   DiskLayersTranshA[ilay][istat][iseg] ->RegisterYourself();
	 DiskLayersTranshB[ilay][istat][iseg] = new TGeoTranslation("hB",HoleTX,-HoleTY,HoleTZ);
	   DiskLayersTranshB[ilay][istat][iseg] ->RegisterYourself();

        DiskLayersShapecompos[ilay][istat][iseg] = new TGeoCompositeShape(Form("compos%dseg%d%sshape",istat+1,iseg+1,kLayerName[ilay].Data()),
							                  Form("comp%dseg%d%sshape-(Hole%dseg%d%sshape:hA)-(Hole%dseg%d%sshape:hB)",
									       istat+1,iseg+1,kLayerName[ilay].Data(),istat+1,iseg+1,kLayerName[ilay].Data(),istat+1,iseg+1,kLayerName[ilay].Data()));

    
	TString layerMaterial = kLayerName[ilay].Data();
	layerMaterial.Remove(0,layerMaterial.Last('_')+1);
		cout << "THE MATERIAL IS \"" << layerMaterial.Data() << "\"" << endl;
	DiskLayersVol[ilay][istat][iseg] = new TGeoVolume(Form("Gem_Disk%d_Seg%d_%s",istat+1,iseg+1,kLayerName[ilay].Data()),
							    DiskLayersShapecompos[ilay][istat][iseg],
							    gGeoMan->GetMedium(layerMaterial.Data()));

      
	cout << "layer material = " << layerMaterial.Data() << endl;
	if ( layerMaterial.Contains("air" ) )
	  DiskLayersVol[ilay][istat][iseg]->SetLineColor(kGray+1);
	if ( layerMaterial.Contains("copper" ) )
	  DiskLayersVol[ilay][istat][iseg]->SetLineColor(kOrange+1);
	if ( layerMaterial.Contains("kapton" ) )
	  DiskLayersVol[ilay][istat][iseg]->SetLineColor(kOrange+4);
	if ( layerMaterial.Contains("aluminium" ) )
	  DiskLayersVol[ilay][istat][iseg]->SetLineColor(kCyan-9);
	if ( layerMaterial.Contains("GEMmixture" ) )
	  DiskLayersVol[ilay][istat][iseg]->SetLineColor(kYellow);
        if ( layerMaterial.Contains("carbon" ) )
       	  DiskLayersVol[jlay][istat][iseg]->SetLineColor(kPink);
       	if ( layerMaterial.Contains("GlassFiber" ) )
       	  DiskLayersVol[jlay][istat][iseg]->SetLineColor(kGreen+3);

	DiskLayersTrans[ilay][istat][iseg] = new TGeoTranslation(0.,0.,layerPosition);
	DiskLayersCombi[ilay][istat][iseg] = new TGeoCombiTrans(*DiskLayersTrans[ilay][istat][iseg],*dummyrot);
	DiskLayersCombi[ilay][istat][iseg]->SetName(Form("Gem_Disk%d_Seg%d_%s",istat+1,iseg+1,kLayerName[ilay].Data()));
	DiskLayersCombi[ilay][istat][iseg]->RegisterYourself();
	DiskVol[istat]->AddNode(DiskLayersVol[ilay][istat][iseg],0,DiskLayersCombi[ilay][istat][iseg]);       

      }
   //-------------------------------------------------------------------------------------------------------------------------------
      cout << "volume " << kLayerName[ilay] << " from " 
	   << setprecision(10) << kDiskZPosition[istat]+layerPosition-kLayerThick[ilay]/2. << " to "
	   << setprecision(10) << kDiskZPosition[istat]+layerPosition+kLayerThick[ilay]/2. << endl;
      
      if ( kLayerName[ilay].Contains("Gem") && kLayerName[ilay].Contains("Sensor") ) {
	Double_t newRadius = kDiskInnerRadius[ilay][istat];
	Double_t nofStrips = 0;

	cout << "rad = " << kDiskInnerRadius[ilay][istat] << " pitch = " << kSensorStripPitch[sensorNumber][0] << " for sensor " << sensorNumber << endl;
	if (  kSensorStripType[sensorNumber] != 2 ) {
	  nofStrips = TMath::Ceil(2.*TMath::Pi()*kDiskInnerRadius[ilay][istat]/kSensorStripPitch[sensorNumber][0]);
	  newRadius = nofStrips*kSensorStripPitch[sensorNumber][0]/2./TMath::Pi();
	}
	cout << "!!!! " << istat << " " << ilay << " > there shall be " << nofStrips << " strips here so the radius should be " << newRadius << endl;
	pout << "                        " << sensorNumber+1 << ",  " << kSensorStripType[sensorNumber] << ",  " 
	     << setw(9) << 0. << ",  "
	     << setw(9) << 0. << ",  " 
	     << setw(9) << kDiskZPosition[istat]+layerPosition << ",  "
	     << setw(9) << 0. << ",  "
	     << setw(9) << newRadius << ",  "
	     << setw(9) << kDiskOuterRadius[ilay][istat] << ",  "
	     << setw(9) << kLayerThick[ilay] << ",  "
	     << setw(9) << kSensorStripAngle[sensorNumber][0] << ",  "
	  // << setw(9) << kSensorStripAngle[sensorNumber][1] << ",  "
	     << setw(9) << kMiddleROBarHfTh[istat] << ",  "
	     << setw(9) << kSensorStripPitch[sensorNumber][0] << ",  "
	     << setw(9) << kSensorStripPitch[sensorNumber][1] << ((istat==kNofDisks-1&&sensorNumber==1)?"":", \\") 
	     << endl;
	sensorNumber++;
      }

	layerPosition += kLayerThick[ilay]/2.;
    }

    SubunitVol->AddNode(DiskVol[istat],0,DiskCombi[istat]);
    
    }
  
  pout << "TrackFinderOnHits_ParThetaA: Double_t  59.4" << endl
       << "TrackFinderOnHits_ParThetaB: Double_t -0.02" << endl
       << "TrackFinderOnHits_ParTheta0: Double_t  56.1372" << endl
       << "TrackFinderOnHits_ParTheta1: Double_t -0.000564362" << endl
       << "TrackFinderOnHits_ParTheta2: Double_t -0.181828" << endl
       << "TrackFinderOnHits_ParTheta3: Double_t  0.284289" << endl
       << "TrackFinderOnHits_ParRadPhi0:Double_t  0.9944432" << endl
       << "TrackFinderOnHits_ParRadPhi2:Double_t -0.000590706" << endl
       << "TrackFinderOnHits_ParMat0:   Double_t \\" << endl
       << "                                      -2.31333e-6,    0.00067035,    0.10173" << endl
       << "TrackFinderOnHits_ParMat1:   Double_t \\" << endl
       << "                                      -7.46844e-10,   -6.6696e-7,    0.000736672" << endl
       << "##########################################################################################" << flush;
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  

  top->AddNode(SubunitVol,0,new TGeoCombiTrans());
     
  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
  top->Draw("ogl");
  pout.close();

}

