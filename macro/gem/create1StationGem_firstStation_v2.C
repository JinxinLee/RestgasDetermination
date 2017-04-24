#include "iomanip.h"

void create1StationGem_firstStation_v2()
{ // create only GEM with one station (v2)
  //----------------------------------------------------------------------------------------------------------------------------------------------
  // Gem disk geometry parameters (R.Karabowicz) 
  //----------------------------------------------------------------------------------------------------------------------------------------------
  // Done Some Changes By Nazila Divani ( since  May 2015 ... ) (v2)---> updated 1Station GEM geometry (April 2017)
  //----------------------------------------------------------------------------------------------------------------------------------------------
  // create GEM first station at the ZPosition=119.40cm , (This is the smallest GEM station)
  //----------------------------------------------------------------------------------------------------------------------------------------------
  // Try to get close to the CAD Geometry as the realistic one ( After could not use CadConverter to convert CAD to ROOT ) //Some parameters from DL.
  //----------------------------------------------------------------------------------------------------------------------------------------------
  // Units are in [cm]
  //-----------------------------------------------------------------------------------------------------------------------------------------------
    const Int_t     NofDisks = 1;   //number of GEM station in form of disk

    const Double_t  DiskVolInnerRadius[NofDisks]    = { 4.50 };   // InnerRadius for 1 Station in form of disk
    const Double_t  DiskVolOuterRadius[NofDisks]    = { 45.00 };  // OuterRadius for 1 Station in form of disk
    const Double_t  DiskZPosition  [NofDisks]    = { 119.40 };   // ZPosition   for 1 Station in form of disk           
 
    const Int_t     DiskNFoils     [NofDisks]    = {  2    };  // For modifying misalignment
    const Double_t  MiddleROBarHfTh[NofDisks]    = {  2.35 }; // half thickness of space in the middle of foils [cm]
    const Double_t  HalfStationThickness = 6.00;

    const Double_t  carbonRingInnerRadius[NofDisks]    = { 39.95 };
    const Double_t  carbonRingOuterRadius[NofDisks]    = { 40.15 };
    const Double_t  carbonRingHalfThickness = 1.50; 

    const Double_t  copperRingInnerRadius[NofDisks]    = { 39.45 };
    const Double_t  copperRingOuterRadius[NofDisks]    = { 39.65 };
    const Double_t  copperRingHalfThickness = 3.75;

    const Double_t  AlRingInnerRadius[NofDisks]    = { 39.45 };
    const Double_t  AlRingOuterRadius[NofDisks]    = { 44.75 };
    const Double_t  AlRingHalfThickness = 3.75;

    const Double_t  SegmentHalfThickness = 0.25; 
    const Int_t     NofSegments = 24;
    const Double_t  SegmentDeltaAngle = 360./(Double_t(NofSegments));
    const Double_t  FirstSegmentAngle = 7.5;
    const TString   newsegment [NofSegments] = { "seg1" ,"seg2" ,"seg3" ,"seg4" ,"seg5" ,"seg6" ,"seg7" ,"seg8" ,
                                                 "seg9" ,"seg10","seg11","seg12","seg13","seg14","seg15","seg16",
                                                 "seg17","seg18","seg19","seg20","seg21","seg22","seg23","seg24"  };

    const Double_t moduleRingInnerRadius[NofDisks]    = { 40.15 };
    const Double_t moduleRingOuterRadius[NofDisks]    = { 44.65 };
    const Double_t moduleRingHalfThickness = 3.75;

    const Double_t  moduleSegmentHalfThickness = 1.30; 
    const Int_t     NofmoduleSegments = 24;
    const Double_t  moduleSegmentDeltaAngle = 360./(Double_t(NofmoduleSegments));
    const Double_t  FirstmoduleSegmentAngle = 0.0 ;
    const TString   newmodulesegment [NofmoduleSegments] = { "moduleseg1" ,"moduleseg2" ,"moduleseg3" ,"moduleseg4" ,"moduleseg5" ,"moduleseg6" ,"moduleseg7" ,"moduleseg8" ,
                                                             "moduleseg9" ,"moduleseg10","moduleseg11","moduleseg12","moduleseg13","moduleseg14","moduleseg15","moduleseg16",
                                                             "moduleseg17","moduleseg18","moduleseg19","moduleseg20","moduleseg21","moduleseg22","moduleseg23","moduleseg24"  };

    const Double_t AlumiRingInnerRadius[NofDisks]    = { 44.7 };
    const Double_t AlumiRingOuterRadius[NofDisks]    = { 45.0 };
    const Double_t AlumiRingHalfThickness = 3.75;
    
    const Double_t coverRingInnerRadius[NofDisks]    = { 39.45 };
    const Double_t coverRingOuterRadius[NofDisks]    = { 45.00 };
    const Double_t coverRingHalfThickness = 0.2;

    const Double_t rcopperbarx    = 5.00;   ////// right copper bar for cables
    const Double_t rcopperbary    = 1.00;
    const Double_t rcopperbarHalfThickness = 13.6;

    const Double_t lcopperbarx    = 5.00;   ////// left copper bar for cables
    const Double_t lcopperbary    = 1.00;
    const Double_t lcopperbarHalfThickness = 16.2;
 
  //------------------------------------------------------------ main layers in shape of Disk----------------------------------------------------------------------
    const Int_t     NofLayers = 49; 
    
    const TString   LayerName [NofLayers] = {   "space",                                           // these "spaces" belong to holding structure layers
                                                "window1_foil_out_aluminium", "WindowF_kapton", "window2_foil_out_aluminium",               
    					        "space",                                           // these "spaces" belong to holding structure layers
                                                "WindowF_aluminium", "CathodeF_kapton", "CathodeF_aluminium", 
                                                "space",                                           //
    					        //"Gem1_Sensor_GEMmixture",    // sensitive layer
    					        "space",                                           //
                                                "Gem1F_copper", "Gem1_kapton", "Gem1B_copper",             
    					        "space",                                           //
    					        "Gem2F_copper", "Gem2_kapton", "Gem2B_copper",                                   
    					        "space",                                           //
    					        "Gem3F_copper", "Gem3_kapton", "Gem3B_copper",                                   
    					        "space",                                           //
					        "space",                                           //
    					        "PadF_copper", "Pad_kapton", "PadB_copper", 
    					        "space",                                           //
					        "space",                                           //
    					        "Gem4F_copper", "Gem4_kapton", "Gem4B_copper",             
    					        "space",                                           //
    					        "Gem5F_copper", "Gem5_kapton", "Gem5B_copper",                                  
    					        "space",                                           //
    					        "Gem6F_copper", "Gem6_kapton", "Gem6B_copper", 
					        "space",                                           //                               
    					        //"Gem6_Sensor_GEMmixture",    // sensitive layer
					        "space",                                           //
					        "CathodeB_aluminium", "CathodeB_kapton", "WindowB_aluminium",                                   
    					        "space",                                           //
    					        "window1_foil_in_aluminium", "WindowB_kapton", "window2_foil_in_aluminium",
                                                "space"   };
						 
    
    const Double_t LayerThick[NofLayers] = {    1.0,                       // windowF1_ring_carbon=holding structure           //  1 = 1   space
                                                0.0001,   0.0007,   0.0001,                                                    // +3 = 4   window
					        1.00,                      // windowF2_ring_carbon=holding structure           // +1 = 5   space
                                                0.0001,   0.0007,   0.0001,                                                    // +3 = 8   cathode
    					        0.80,                      // cathodeF_ring_GlassFiber=holding structure       // +1 = 9   space
    					      //1.0020, //Sens                                                                 // +1 = 10  SENSOR
					        0.050,                     // gem_ring1_GlassFiber=holding structure           // +1 = 11  space
                                                0.0005,   0.0050,   0.0005,                                                    // +3 = 14  GEM Foil
    					        0.050,                     // gem_ring2_GlassFiber=holding structure           // +1 = 15  space                
    					        0.0005,   0.0050,   0.0005,                                                    // +3 = 18  GEM Foil
                                                0.050,                     // gem_ring3_GlassFiber=holding structure           // +1 = 19  space             
    					        0.0005,   0.0050,   0.0005,                                                    // +3 = 22  GEM Foil
                                                0.050,                     // gem_ring4_GlassFiber=holding structure           // +1 = 23  space
                                                0.10,                      // padplaneF_support_GlassFiber=holding structure   // +1 = 24  space
                                                0.001,    0.001,    0.001,                                                     // +3 = 27  PAD plane
					        0.10,                      // padplaneB_support_GlassFiber=holding structure   // +1 = 28  space
    					        0.050,                     // gem_ring5_GlassFiber=holding structure           // +1 = 29  space
                                                0.0005,   0.0050,   0.0005,                                                    // +3 = 32  GEM Foil
                                                0.050,                     // gem_ring6_GlassFiber=holding structure           // +1 = 33  space
                                                0.0005,   0.0050,   0.0005,                                                    // +3 = 36  GEM Foil
    					        0.050,                     // gem_ring7_GlassFiber=holding structure           // +1 = 37  space
                                                0.0005,   0.0050,   0.0005,                                                    // +3 = 40  GEM Foil
                                                0.050,                     // gem_ring8_GlassFiber=holding structure           // +1 = 41  space
                                              //1.0020, //Sens                                                                 // +1 = 42  SENSOR
                                                0.80,                      // cathodeB_ring_GlassFiber=holding structure       // +1 = 43  space
                                                0.0001,   0.0007,   0.0001,                                                    // +3 = 46  cathode
                                                1.00,                      // windowB1_ring_carbon=holding structure           // +1 = 47  space 
                                                0.0001,   0.0007,   0.0001,                                                    // +3 = 50  window
                                                1.0   };                   // windowB2_ring_carbon=holding structure           // +3 = 51  space
                                               
    
   const Double_t DiskOuterRadius[NofLayers][NofDisks] = {     39.90,  
                                                               38.95,        39.40,      38.95, 
							       39.90,  
							       38.45,        39.40,      38.45, 
							       39.40,  
							     //38.00,  
							       39.05,  
							       38.10,        39.05,      38.10, 
							       39.05,  
							       38.10,        39.05,      38.10,  
							       39.05,  
							       38.10,        39.05,      38.10,  
							       39.05,                                                    
							       45.00,  
							       44.90,        44.90,      44.90,  
							       45.00,  
							       39.05, 
							       38.10,        39.05,      38.10,  
							       39.05, 
							       38.10,        39.05,      38.10,  
							       39.05,  
							       38.10,        39.05,      38.10,  
							       39.05,                                                         
							     //38.00,  
							       39.40,  
							       38.45,        39.40,      38.45,  
							       39.40,  
							       39.40,        38.95,      39.40,  
                                                               39.40    }; 
                                                               
   
   const Double_t DiskInnerRadius[NofLayers][NofDisks] = {     4.50,   
                                                               4.50,         4.50,      4.50,   
      							       4.50,   
      							       4.50,         4.50,      4.50,   
      							       4.50,  
      							     //4.50,   
      							       4.50,   
      							       4.50,         4.50,       4.50,   
     							       4.50,   
      							       4.50,         4.50,       4.50,   
      							       4.50,   
     							       4.50,         4.50,       4.50,   
      							       4.50,                                                     
      							       4.50,   
      							       4.50,         4.50,       4.50,   
      							       4.50,   
      							       4.50,   
     							       4.50,         4.50,       4.50,   
     							       4.50,   
     							       4.50,         4.50,       4.50,   
      							       4.50,   
      							       4.50,         4.50,       4.50,  
      							       4.50,                                                          
     							     //4.50,   
      							       4.50,  
     							       4.50,         4.50,       4.50,   
     							       4.50,   
      							       4.50,         4.50,       4.50,   
                                                               4.50   };
                                                               

  const Double_t  HoleTZ = 0.0;  // (top and down) holes Translation parameters
  const Double_t  HoleTX = 0.0;
  const Double_t  HoleTY = 33.00;
//------------------------------------ sensitive layers ---------------------------------------------------------------------------------------
  const Int_t    NofSensLayers = 2;
  
  const Double_t  sMiddleROBarHfTh[NofDisks]  = { 2.35 }; // half thickness of space in the middle of Sens [cm]
  
  const TString  SensLayerName[NofSensLayers] = {  "Gem1_Sensor_GEMmixture",      // drift layers
						   "Gem6_Sensor_GEMmixture"  };
  
  const Double_t SensLayerThick[NofSensLayers] =  {   0.85,
						      0.85  };
  
  const Double_t SensZPosition[NofSensLayers][NofDisks] = {   -0.6942,
							       0.6948 };  
  
  const Double_t SensOuterRadius[NofSensLayers][NofDisks] = {   38.15,
								38.15 };
  
  const Double_t SensInnerRadius[NofSensLayers][NofDisks] = {   5.00, 
      							        5.00  };
  
  const Double_t  sHoleTZ = 0.0;  // (top and down) holes Translation parameters
  const Double_t  sHoleTX = 0.0;
  const Double_t  sHoleTY = 33.00;
							
//----------------------- holding structure layers ------------------------------------------------------------------------------------------------
    const Int_t     NofHLayers = 16;

    const TString   HLayersName [NofHLayers] = {  "windowF1_ring_carbon",
                                                  //"space",
                                                  "windowF2_ring_carbon",
						  //"space",
   					          "cathodeF_ring_GlassFiber",
						  //"space",
    					          "gem_ring1_GlassFiber",
						  //"space",
                                                  "gem_ring2_GlassFiber",
						  //"space",
   					          "gem_ring3_GlassFiber",
						  //"space",
    					          "gem_ring4_GlassFiber",						  
    					          "padplaneF_support_GlassFiber",
						  //"space",
                                                  "padplaneB_support_GlassFiber",
    					          "gem_ring5_GlassFiber",
						  //"space",
                                                  "gem_ring6_GlassFiber",
						  //"space",
    					          "gem_ring7_GlassFiber",
						  //"space",
   					          "gem_ring8_GlassFiber",
						  //"space",
   					          "cathodeB_ring_GlassFiber",
						  //"space",
    					          "windowB1_ring_carbon",
                                                  //"space",
                                                  "windowB2_ring_carbon"      };
    
    const Double_t HLayersThick[NofHLayers] =  {    1.00,                                                   
                                                    1.00,						    
    					            0.80,						   
    					            0.050,						    
    					            0.050,						   
    					            0.050,						   
    					            0.050,                                   
    					            0.10,						    
    					            0.10,                                   
    					            0.050,						    
    					            0.050,						   
    				                    0.050,						   
    					            0.050,						   
    					            0.80,						    
						    1.00,                                                   
                                                    1.00    };						    
    					               
       
    const Double_t  HZPosition[NofHLayers][NofDisks] = { -2.6216,                                                                  
                                                         -1.6201,						               
							 -0.7192,						              
							 -0.2942,						              
							 -0.2382,						                    
							 -0.1822,					                             
							 -0.1262,                     
							 -0.0512,					                     
							  0.0518,                  
							  0.1268,							                  
						 	  0.1828,							                  
							  0.2388,						                  
							  0.2948,							                
							  0.7198,							                 
                                                          1.6207,                                                                    
							  2.6216   };          
                                                                    
    
   const Double_t HOuterRadius[NofHLayers][NofDisks] = {   39.90,                                                              
                                                           39.90,						        
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
							   39.40,                                                            
                                                           39.40    };
   
   const Double_t HInnerRadius[NofHLayers][NofDisks] = {  38.50,                                                             
                                                          38.50,						     
							  38.50,							     
							  38.15,						      
							  38.15,						      
							  38.15,						     
							  38.15,     
							  38.50,						      
							  38.50,     
							  38.15,							     
							  38.15,						      
							  38.15,						      
							  38.15,							    
							  38.50,						      
                                                          38.00,                                                            
                                                          38.00       };    
 
   const Double_t  HXBoxWidth = 2.30;  // Using to define holes for the holding structure layers 
   const Double_t  HXPlateWidth = 1.90;
   const Double_t  HYPlateWidth = 9.30;

   const Double_t  HTZ = 0.0;         // Translation parameters for vertical holes
   const Double_t  HTX = 0.0;    
   const Double_t  HTY[NofHLayers][NofDisks] = {  25.0,					  
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
                                                  25.0,                                                  
                                                  25.0,					   
                                                  25.0  };
                                                   
   
//-------------------------------------------------------------------------------------------------------------------------------------------------
  const Int_t    SensorStripType [2]    = {  3  ,  2   };  
  
  const Double_t SensorStripAngle[2][2] = {  0.  ,  0.  ,  0.  ,  0.  };
  const Double_t SensorStripPitch[2][2] = {  0.04,  0.04,  0.04,  0.04}; 

  Double_t firstLayerOffset = 0.;

  for ( Int_t ilayer = 0 ; ilayer < NofLayers ; ilayer++ ) {
    cout << LayerName[ilayer].Data() << " -> " << LayerThick[ilayer] << endl;
    firstLayerOffset += LayerThick[ilayer];
  }

  cout << "total thickness is " << firstLayerOffset << endl;
  firstLayerOffset = firstLayerOffset/2.;
  firstLayerOffset = -0.001*(TMath::Floor(1000.*firstLayerOffset));
  cout << "first layer offset is " << firstLayerOffset << endl;
  //-----------------------------------------------------------------------------------------------

  TString vmcWorkdir = getenv("VMCWORKDIR");
  
  TString outfile= "../../geometry/gem_1Station_first_realistic_v2.root"; //v2
  

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
   TGeoShape          *DiskShape[NofDisks];
   TGeoVolume         *DiskVol  [NofDisks];
   TGeoRotation       *DiskRotat[NofDisks];
   TGeoTranslation    *DiskTrans[NofDisks];   
   TGeoCombiTrans     *DiskCombi[NofDisks];   
  //--------------------------------------------------------------------------------------------
   TGeoShape          *carbonRingShape[NofDisks];
   TGeoVolume         *carbonRingVol  [NofDisks];  
   TGeoTranslation    *carbonRingTrans[NofDisks];
   TGeoCombiTrans     *carbonRingCombi[NofDisks];
  //------------------------------------------------------------------------------------------
   TGeoShape          *copperRingShape[NofDisks];
   TGeoVolume         *copperRingVol  [NofDisks];  
   TGeoTranslation    *copperRingTrans[NofDisks];
   TGeoCombiTrans     *copperRingCombi[NofDisks];
  //-----------------------------------------------------------------------------------------
   TGeoShape          *coverRingShape[NofDisks];
   TGeoVolume         *coverRingVol  [NofDisks];  
   TGeoTranslation    *coverRingTrans[NofDisks];
   TGeoCombiTrans     *coverRingCombi[NofDisks];
  //-------------------------------------------------------------------------------------------
   TGeoShape          *tAcopperbarShape;
   TGeoVolume         *tAcopperbarVol  ;  
   TGeoTranslation    *tAcopperbarTrans;
   TGeoRotation       *tAcopperbarRot;
   TGeoCombiTrans     *tAcopperbarCombi;
  //-----------------------------------------------------------------------------------------
   TGeoShape          *tBcopperbarShape;
   TGeoVolume         *tBcopperbarVol  ;  
   TGeoTranslation    *tBcopperbarTrans;
   TGeoRotation       *tBcopperbarRot;
   TGeoCombiTrans     *tBcopperbarCombi;
  //-----------------------------------------------------------------------------------------
   TGeoShape          *dAcopperbarShape;
   TGeoVolume         *dAcopperbarVol  ;  
   TGeoTranslation    *dAcopperbarTrans;
   TGeoRotation       *dAcopperbarRot;
   TGeoCombiTrans     *dAcopperbarCombi;
 //-----------------------------------------------------------------------------------------
   TGeoShape          *dBcopperbarShape;
   TGeoVolume         *dBcopperbarVol  ;  
   TGeoTranslation    *dBcopperbarTrans;
   TGeoRotation       *dBcopperbarRot;
   TGeoCombiTrans     *dBcopperbarCombi;
  //-----------------------------------------------------------------------------------------
   TGeoShape          *AlRingShape[NofDisks][NofSegments];
   TGeoVolume         *AlRingVol  [NofDisks][NofSegments];  
   TGeoTranslation    *AlRingTrans[NofDisks][NofSegments];
   TGeoCombiTrans     *AlRingCombi[NofDisks][NofSegments];
  //-----------------------------------------------------------------------------------------
   TGeoShape          *moduleRingShape[NofDisks][NofmoduleSegments];
   TGeoVolume         *moduleRingVol  [NofDisks][NofmoduleSegments];  
   TGeoTranslation    *moduleRingTrans[NofDisks][NofmoduleSegments];
   TGeoCombiTrans     *moduleRingCombi[NofDisks][NofmoduleSegments];
  //-----------------------------------------------------------------------------------------
   TGeoShape          *AlumiRingShape[NofDisks];
   TGeoVolume         *AlumiRingVol  [NofDisks];  
   TGeoTranslation    *AlumiRingTrans[NofDisks];
   TGeoCombiTrans     *AlumiRingCombi[NofDisks];
 //----------------------------------------------------------------------------------------------
   TGeoShape          *DiskLayersShapeA     [NofLayers][NofDisks][4]; // 4 is number of seg
   TGeoShape          *DiskLayersShapeB     [NofLayers][NofDisks][4];
   TGeoSubtraction    *DiskLayersSubtr      [NofLayers][NofDisks][4];
   TGeoShape          *DiskLayersShapeC     [NofLayers][NofDisks][4]; // final, C = A-B
   TGeoShape          *DiskLayersShapeHole  [NofLayers][NofDisks][4];
   TGeoCompositeShape *DiskLayersShapecompos[NofLayers][NofDisks][4];
   TGeoVolume         *DiskLayersVol        [NofLayers][NofDisks][4];
   TGeoTranslation    *DiskLayersTranshA    [NofLayers][NofDisks][4];
   TGeoTranslation    *DiskLayersTranshB    [NofLayers][NofDisks][4];
   TGeoTranslation    *DiskLayersTrans      [NofLayers][NofDisks][4];
   TGeoCombiTrans     *DiskLayersCombi      [NofLayers][NofDisks][4];
 //---------------------------------------------------------------------------------------------------
   TGeoShape          *SensDiskLayersShapeA     [NofSensLayers][NofDisks][4]; // 4 is number of seg
   TGeoShape          *SensDiskLayersShapeB     [NofSensLayers][NofDisks][4];
   TGeoSubtraction    *SensDiskLayersSubtr      [NofSensLayers][NofDisks][4];
   TGeoShape          *SensDiskLayersShapeC     [NofSensLayers][NofDisks][4]; // final, C = A-B
   TGeoShape          *SensDiskLayersShapeHole  [NofSensLayers][NofDisks][4];
   TGeoCompositeShape *SensDiskLayersShapecompos[NofSensLayers][NofDisks][4];
   TGeoVolume         *SensDiskLayersVol        [NofSensLayers][NofDisks][4];
   TGeoTranslation    *SensDiskLayersTranshA    [NofSensLayers][NofDisks][4];
   TGeoTranslation    *SensDiskLayersTranshB    [NofSensLayers][NofDisks][4];
   TGeoTranslation    *SensDiskLayersTrans      [NofSensLayers][NofDisks][4];
   TGeoCombiTrans     *SensDiskLayersCombi      [NofSensLayers][NofDisks][4];
 //------------------------------------------------------------------------------------------------
   TGeoShape          *HLayersShapeTube  [NofHLayers][NofDisks];
   TGeoShape          *HLayersShapeBox   [NofHLayers][NofDisks];
   TGeoShape          *HLayersShapePlate [NofHLayers][NofDisks];  
   TGeoShape          *HLayersShapeHT    [NofHLayers][NofDisks];  
   TGeoShape          *HLayersShapeHTM   [NofHLayers][NofDisks];  
   TGeoShape          *HLayersShapeHTD   [NofHLayers][NofDisks]; 
   TGeoCompositeShape *HLayersShapecompos[NofHLayers][NofDisks]; 
   TGeoVolume         *HLayersVolcomp    [NofHLayers][NofDisks];
   TGeoTranslation    *HLayersTranstA    [NofHLayers][NofDisks];
   TGeoTranslation    *HLayersTranstB    [NofHLayers][NofDisks];  
   TGeoTranslation    *HLayersTranstC    [NofHLayers][NofDisks];  
   TGeoTranslation    *HLayersTranstD    [NofHLayers][NofDisks];  
   TGeoTranslation    *HLayersTranstE    [NofHLayers][NofDisks];  
   TGeoTranslation    *HLayersTranstF    [NofHLayers][NofDisks];  
   TGeoRotation       *HLayersRotrA      [NofHLayers][NofDisks];
   TGeoShape          *HLayersShape      [NofHLayers][NofDisks];
   TGeoVolume         *HLayersVol        [NofHLayers][NofDisks];
   TGeoTranslation    *HLayersTrans      [NofHLayers][NofDisks];
   TGeoCombiTrans     *HLayersCombi      [NofHLayers][NofDisks];
 //------------------------------------------------------------------------------------------------
   TGeoShape          *RiddleShapeTubeA ; 
   TGeoShape          *RiddleShapeTubeB ; 
   TGeoShape          *RiddleShapeCone  ; 
   TGeoShape          *RiddleShapeTubeC ; 
   TGeoShape          *RiddleShapeTubeD ; 
   TGeoShape          *RiddleShapeTubeE ; 
   TGeoShape          *RiddleShapeTubeF ; 
   TGeoSubtraction    *RiddleSubtr      ;
   TGeoCompositeShape *RiddleShapecompos;
   TGeoVolume         *RiddleVolcomp    ;
   TGeoTranslation    *RiddleTrans      ;
   TGeoTranslation    *RiddleTransTubeA ;
   TGeoTranslation    *RiddleTransCone  ;
   TGeoTranslation    *RiddleTransTubeC ;
   TGeoTranslation    *RiddleTransTubeD ;
   TGeoTranslation    *RiddleTransTubeE ;
   TGeoCombiTrans     *RiddleCombi      ;
   TGeoTranslation    *RiddleTransTubeF[4][100] ;
   TGeoCombiTrans     *RiddleCombiTranshole[4][100] ;
   TGeoRotation       *RiddleRothole[4][100] ;
 //----------------------------------------------------------------------------------------------
   TString outParFileName = Form("%s/macro/params/gem_1Station_first_realistic_v2.digi.par",vmcWorkdir.Data()); //v2
  
  
  cout << "parameter file = \"" << outParFileName.Data() << "\"" << endl;

  ofstream pout(outParFileName.Data());
  pout.setf(ios::fixed);

  pout << "#################################################################" << endl;
  pout << "# Digitization parameters for GEM                                " << endl;
  pout << "# with 1 Station                                                 " << endl;
  pout << "# Format:                                                        " << endl;
  pout << "# Description of parameters:                                     " << endl;
  pout << "# [PndGemDetectors]                                              " << endl;
  pout << "# parameters:d station_number, ZPos, rotation_angle, number_of_sensors, \\" << endl;
  pout << "#              sensor_number, sensor_type, pos_x, pos_y, pos_z, rotAngle, inn_rad, out_rad, thick, str_ang_0, str_ang_1, barWidth, pitch_0, pitch_1, \\" << endl;
  pout << "#              sensor_number, ...." << endl;
  pout << "#              station_number, ..." << endl;
  pout << "#################################################################" << endl;
  pout << "[PndGemDetectors]" << setprecision(4) << endl;

  pout << "parameters:Double_t \\" << endl;

       for ( Int_t istat = 0 ; istat < NofDisks ; istat++ ) {

         pout << "                   " << istat+1 << ",  "
       	 << setw(9) << DiskZPosition[istat]
       	 << ",  0.0, " << 2 << ", \\" << endl;
    
  //-----------------------------------GEM Disk------------------------------------------------------------------------------------------------------
    DiskShape[istat] = new TGeoTube  (Form("disk%dshape",istat+1),DiskVolInnerRadius[istat],DiskVolOuterRadius[istat],HalfStationThickness);
    DiskVol  [istat] = new TGeoVolume(Form("Gem_Disk%d_Volume",istat+1),DiskShape[istat],gGeoMan->GetMedium("GEMmixture"));
    DiskTrans[istat] = new TGeoTranslation(0.,0.,DiskZPosition[istat]);
    cout << "station " << DiskVolInnerRadius[istat] << " " << DiskVolOuterRadius[istat] << " at " << DiskZPosition[istat] << endl;	        
    if(istat<2)
    DiskCombi[istat] = new TGeoCombiTrans(*DiskTrans[istat],*dummyrot);
    else
    {
    DiskRotat[istat] = new TGeoRotation(Form("disk%drotat"), 0.0, 180.0, 0.0);  /// turned over the 3rd station because of the position of the electronic devices
    DiskCombi[istat] = new TGeoCombiTrans(*DiskTrans[istat],*DiskRotat[istat]);
    }
    //DiskCombi[istat] = new TGeoCombiTrans(*DiskTrans[istat],*dummyrot);
    DiskCombi[istat]->SetName(Form("Gem_Disk%d_Volume",istat+1));
    DiskCombi[istat]->RegisterYourself();
    DiskVol[istat]->SetLineColor(kYellow);
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //------------------------- Gas container Ring Bottom -------------------------------------------------------------------------------------
    carbonRingShape[istat] = new TGeoTube  (Form("carbonRing%dshape",istat+1),carbonRingInnerRadius[istat],carbonRingOuterRadius[istat],carbonRingHalfThickness);
    carbonRingVol  [istat] = new TGeoVolume(Form("Gem_carbonRing%d_Volume",istat+1),carbonRingShape[istat],gGeoMan->GetMedium("carbon"));
    carbonRingTrans[istat] = new TGeoTranslation(0.,0.,-1.61);
    cout << "carbonRing " << carbonRingInnerRadius[istat] << " " << carbonRingOuterRadius[istat] << endl;
    carbonRingCombi[istat] = new TGeoCombiTrans(*carbonRingTrans[istat],*dummyrot); 
    carbonRingCombi[istat]->SetName(Form("Gem_carbonRing%d_Volume",istat+1));
    carbonRingCombi[istat]->RegisterYourself();
    carbonRingVol[istat]->SetLineColor(kMagenta);
    //----------------------------------------------------------------------------------------------------------------------------------------
    DiskVol[istat]->AddNode(carbonRingVol[istat],0,carbonRingCombi[istat]);
    //------------------------------------------------------------------------------------------------------------------------------------------
    //------------------------- Gas container Ring top -----------------------------------------------------------------------------------------
    copperRingShape[istat] = new TGeoTube  (Form("copperRing%dshape",istat+1),copperRingInnerRadius[istat],copperRingOuterRadius[istat],copperRingHalfThickness);
    copperRingVol  [istat] = new TGeoVolume(Form("Gem_copperRing%d_Volume",istat+1),copperRingShape[istat],gGeoMan->GetMedium("copper"));
    copperRingTrans[istat] = new TGeoTranslation(0.,0.,3.85);
    cout << "copperRing " << copperRingInnerRadius[istat] << " " << copperRingOuterRadius[istat] << endl;
    copperRingCombi[istat] = new TGeoCombiTrans(*copperRingTrans[istat],*dummyrot); 
    copperRingCombi[istat]->SetName(Form("Gem_copperRing%d_Volume",istat+1));
    copperRingCombi[istat]->RegisterYourself();
    copperRingVol[istat]->SetLineColor(kYellow-5);
    //----------------------------------------------------------------------------------------------------------------------------------------
    DiskVol[istat]->AddNode(copperRingVol[istat],0,copperRingCombi[istat]);
    //----------------------------------------------------------------------------------------------------------------------------------------
    //----------------------segments for electronics --------------------------------------------------------------------------------------------------------
      Double_t segmentAngularSize = SegmentHalfThickness/AlRingInnerRadius[istat]*360.;
           for ( Int_t isegm = 0 ; isegm < NofSegments ; isegm++ ) {
      		 cout << " Segment " << isegm << " with name " << newsegment[isegm] << " will be put at " << FirstSegmentAngle+isegm*SegmentDeltaAngle << " (in fact from " << FirstSegmentAngle+isegm*SegmentDeltaAngle-segmentAngularSize/2. << " to " << FirstSegmentAngle+isegm*SegmentDeltaAngle+segmentAngularSize/2. << ")" << endl;
      	   AlRingShape[istat][isegm] = new TGeoTubeSeg (Form("AlRing%dshape",istat+1,isegm+1),AlRingInnerRadius[istat],AlRingOuterRadius[istat],AlRingHalfThickness,FirstSegmentAngle+isegm*SegmentDeltaAngle-segmentAngularSize/2.,FirstSegmentAngle+isegm*SegmentDeltaAngle+segmentAngularSize/2. );
      	   AlRingVol[istat][isegm]   = new TGeoVolume (Form("Gem_AlRing%d_Volume",istat+1,isegm+1),AlRingShape[istat][isegm],gGeoMan->GetMedium("aluminium")); 
           AlRingTrans[istat][isegm] = new TGeoTranslation (0.,0.,3.85);
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
          moduleRingTrans[istat][imodulesegm] = new TGeoTranslation (0.,0.,3.85);
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
     AlumiRingTrans[istat] = new TGeoTranslation(0.,0.,3.85);
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
    coverRingTrans[istat] = new TGeoTranslation(0.,0.,7.81);
    cout << "coverRing " << coverRingInnerRadius[istat] << " " << coverRingOuterRadius[istat] << endl;
    coverRingCombi[istat] = new TGeoCombiTrans(*coverRingTrans[istat],*dummyrot); 
    coverRingCombi[istat]->SetName(Form("Gem_coverRing%d_Volume",istat+1));
    coverRingCombi[istat]->RegisterYourself();
    coverRingVol[istat]->SetLineColor(kGreen+3);
    //----------------------------------------------------------------------------------------------------------------------------------------
    DiskVol[istat]->AddNode(coverRingVol[istat],0,coverRingCombi[istat]);
    //----------------------------------------------------------------------------------------------------------------------------------------        
  ///////////////////////////////////////////////holding structure layers////////////////////////////////////////////////////////////////////////////////////////////////
    //Double_t firstHLayersOffset = 0.;   

   //  for ( Int_t jlay = 0 ; jlay < NofHLayers ; jlay++ ) {
   //    cout << HLayersName[jlay].Data() << " -> " << HLayersThick[jlay] << endl;
   //    firstHLayersOffset += HLayersThick[jlay];
   //  }

   //  cout << "total Hlayers thickness is " << firstHLayersOffset << endl;
   //  firstHLayersOffset = firstHLayersOffset/2.;
   //  firstHLayersOffset = -0.001*(TMath::Floor(1000.*firstHLayersOffset));//floor: round to nearest integer
   //  cout << "first Hlayer offset is " << firstHLayersOffset << endl;

   //  Double_t HZPosition = firstHLayersOffset;

    for ( Int_t jlay = 0 ; jlay < NofHLayers ; jlay++ ) {
    cout << "doing Hlayers " << jlay << endl;            
       
    // HZPosition += HLayersThick[jlay]/2.;

    // if ( HLayersName[jlay].Contains("space") && HLayersThick[jlay] > 0.001 ) {
    //  	  cout << "***** THE THICK SPACE HLAYER IS AT " <<  HZPosition << endl;
    //  	}
    //    if ( HLayersName[jlay].Contains("space") && HLayersName[jlay].Length() == 5 ) {
    //  	  HZPosition += LayerThick[jlay]/2.;
    //  	continue;
    //    }
       

    cout << "  got Hlayer " << HLayersName[jlay].Data() << endl;
    	
    HLayersShapeTube[jlay][istat] = new TGeoTube  (Form("T%dT%s",istat+1,HLayersName[jlay].Data()),HInnerRadius[jlay][istat],HOuterRadius[jlay][istat],HLayersThick[jlay]/2.);
    HLayersShapeHT[jlay][istat]   = new TGeoTube  (Form("H%dH%s",istat+1,HLayersName[jlay].Data()),0.0,5.00,HLayersThick[jlay]/2.);
    HLayersShapeHTM[jlay][istat]  = new TGeoTube  (Form("HTM%dHTM%s",istat+1,HLayersName[jlay].Data()),0.0,4.50,HLayersThick[jlay]/2.+0.0002);
    HLayersShapeHTD[jlay][istat]  = new TGeoTube  (Form("HTD%dHTD%s",istat+1,HLayersName[jlay].Data()),0.0,1.90,HLayersThick[jlay]/2.+0.0001);
	       cout << "Tube name is " << HLayersShapeTube[jlay][istat]->GetName() << endl;
	       //	       cout << "TubeHTM name is " << HLayersShapeHTM[jlay][istat]->GetName() << endl;
    HLayersShapeBox[jlay][istat]   = new TGeoBBox(Form("B%dB%s",istat+1,HLayersName[jlay].Data()),HXBoxWidth[jlay][istat],HOuterRadius[jlay][istat],HLayersThick[jlay]/2.);
    HLayersShapePlate[jlay][istat] = new TGeoBBox(Form("P%dP%s",istat+1,HLayersName[jlay].Data()),HXPlateWidth[jlay][istat],HYPlateWidth[jlay][istat],HLayersThick[jlay]/2.+0.0001);

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
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //--------------------------------------------------- drift layers-----------------------------------------------------------------------------------------
 
    Int_t sensorNumber = 0;
    
    for ( Int_t slay = 0 ; slay < NofSensLayers ; slay++ ) {
      cout << "doing Senslayer " << slay << endl;
      //SensZPosition[slay][istat] ++ ;
	if ( SensLayerName[slay].Contains("space") && SensLayerThick[slay] > 0.0 ) {
	  cout << "***** THE THICK SPACE SensLAYER IS AT " << SensZPosition[slay][istat] << endl;
	}
      if ( SensLayerName[slay].Contains("space") && SensLayerName[slay].Length() == 5 ) {
	//SensZPosition[slay][istat] ++ ;
	continue;
      }

                 cout << "  got layer : " << SensLayerName[slay].Data() << endl;

    
    Double_t segPhiSpan = 360./(Double_t(DiskNFoils[istat]));
    Double_t segBegin   =  90.;
    cout << "will do loop over segments" << endl;
    
    for ( Int_t iseg = 0 ; iseg < 1 ; iseg++ ) {
    cout << "segment " << iseg << endl;
	SensDiskLayersShapeA[slay][istat][iseg] = new TGeoTube(Form("sdisk%dseg%d%sshape",istat+1,iseg+1,SensLayerName[slay].Data()),
							   SensInnerRadius[slay][istat],SensOuterRadius[slay][istat],
							   SensLayerThick[slay]/2.);
	SensDiskLayersShapeB[slay][istat][iseg] = new TGeoBBox(Form("srobo%dseg%d%sshape",istat+1,iseg+1,SensLayerName[slay].Data()),
							      sMiddleROBarHfTh[istat],
							      SensOuterRadius[slay][istat],
							      SensLayerThick[slay]);
	SensDiskLayersSubtr[slay][istat][iseg] = new TGeoSubtraction(SensDiskLayersShapeA[slay][istat][iseg],
								  SensDiskLayersShapeB[slay][istat][iseg]);       

	SensDiskLayersShapeC[slay][istat][iseg] = new TGeoCompositeShape(Form("scomp%dseg%d%sshape",istat+1,iseg+1,SensLayerName[slay].Data()),
								     SensDiskLayersSubtr[slay][istat][iseg]);
	segBegin += segPhiSpan;
	cout << " segBegin " << segBegin << endl;

        SensDiskLayersShapeHole[slay][istat][iseg] = new TGeoTube(Form("sHole%dseg%d%sshape",istat+1,iseg+1,SensLayerName[slay].Data()),0.0,3.80,SensLayerThick[slay]+0.01);

	 SensDiskLayersTranshA[slay][istat][iseg] = new TGeoTranslation("shA",sHoleTX,sHoleTY,sHoleTZ);
	   SensDiskLayersTranshA[slay][istat][iseg] ->RegisterYourself();
	 SensDiskLayersTranshB[slay][istat][iseg] = new TGeoTranslation("shB",sHoleTX,-sHoleTY,sHoleTZ);
	   SensDiskLayersTranshB[slay][istat][iseg] ->RegisterYourself();

        SensDiskLayersShapecompos[slay][istat][iseg] = new TGeoCompositeShape(Form("scompos%dseg%d%sshape",istat+1,iseg+1,SensLayerName[slay].Data()),
							                  Form("scomp%dseg%d%sshape-(sHole%dseg%d%sshape:shA)-(sHole%dseg%d%sshape:shB)",
								       istat+1,iseg+1,SensLayerName[slay].Data(),istat+1,iseg+1,SensLayerName[slay].Data(),istat+1,iseg+1,SensLayerName[slay].Data()));
	TString SenslayerMaterial = SensLayerName[slay].Data();
	SenslayerMaterial.Remove(0,SenslayerMaterial.Last('_')+1);
		cout << "THE MATERIAL IS \"" << SenslayerMaterial.Data() << "\"" << endl;
	SensDiskLayersVol[slay][istat][iseg] = new TGeoVolume(Form("GemSen_Disk%d_Seg%d_%s",istat+1,iseg+1,SensLayerName[slay].Data()),
							    SensDiskLayersShapecompos[slay][istat][iseg],
							    gGeoMan->GetMedium(SenslayerMaterial.Data()));
	
	cout << "Senslayer material = " << SenslayerMaterial.Data() << endl;
	if ( SenslayerMaterial.Contains("GEMmixture" ) )
	  SensDiskLayersVol[slay][istat][iseg]->SetLineColor(kYellow);        

	SensDiskLayersTrans[slay][istat][iseg] = new TGeoTranslation(0.,0.,SensZPosition[slay][istat]);
	SensDiskLayersCombi[slay][istat][iseg] = new TGeoCombiTrans(*SensDiskLayersTrans[slay][istat][iseg],*dummyrot);
	SensDiskLayersCombi[slay][istat][iseg]->SetName(Form("GemSen_Disk%d_Seg%d_%s",istat+1,iseg+1,SensLayerName[slay].Data()));
	SensDiskLayersCombi[slay][istat][iseg]->RegisterYourself();
	DiskVol[istat]->AddNode(SensDiskLayersVol[slay][istat][iseg],0,SensDiskLayersCombi[slay][istat][iseg]);
    }
//-------------------------------------------------------------------------------------------------------------------------------------------------------
       cout << "Svolume " << SensLayerName[slay] << " from " 
	   << setprecision(10) << DiskZPosition[istat]+SensZPosition[slay][istat]-SensLayerThick[slay]/2. << " to "
	   << setprecision(10) << DiskZPosition[istat]+SensZPosition[slay][istat]+SensLayerThick[slay]/2. << endl;
  
           if ( SensLayerName[slay].Contains("Gem") && SensLayerName[slay].Contains("Sensor") ) {
	 Double_t newRadius = SensInnerRadius[slay][istat];
	 Double_t nofStrips = 0;

	 cout << "srad = " << SensInnerRadius[slay][istat] << " pitch = " << SensorStripPitch[sensorNumber][0] << " for sensor " << sensorNumber << endl;
	 if (  SensorStripType[sensorNumber] != 2 ) {
	   nofStrips = TMath::Ceil(2.*TMath::Pi()*SensInnerRadius[slay][istat]/SensorStripPitch[sensorNumber][0]);
	   newRadius = nofStrips*SensorStripPitch[sensorNumber][0]/2./TMath::Pi();
	 }
	 cout << "!!!! " << istat << " " << slay << " > there shall be " << nofStrips << " strips here so the radius should be " << newRadius << endl;
	 pout << "                        " << sensorNumber+1 << ",  " << SensorStripType[sensorNumber] << ",  " 
	      << setw(9) << 0. << ",  "
	      << setw(9) << 0. << ",  " 
	      << setw(9) << DiskZPosition[istat]+SensZPosition[slay][istat] << ",  "
	      << setw(9) << 0. << ",  "
	      << setw(9) << newRadius << ",  "
	      << setw(9) << SensOuterRadius[slay][istat] << ",  "
	      << setw(9) << SensLayerThick[slay] << ",  "
	      << setw(9) << SensorStripAngle[sensorNumber][0] << ",  "
	     //<< setw(9) << SensorStripAngle[sensorNumber][1] << ",  "
	      << setw(9) << sMiddleROBarHfTh[istat] << ",  "
	      << setw(9) << SensorStripPitch[sensorNumber][0] << ",  "
	      << setw(9) << SensorStripPitch[sensorNumber][1] << ((istat==NofDisks-1 && sensorNumber==1)?"":", \\") 
	      << endl;	 
	 sensorNumber++;
	   }
	   
	   SensZPosition[slay][istat] ++ ;
	   
    }
///////////////////////////////////////////////////////main layers////////////////////////////////////////////////////////////////////////////////////////////////

    Double_t layerPosition = firstLayerOffset;//-LayerThick[0]/2.;

    Int_t msensorNumber = 0;
    
    for ( Int_t ilay = 0 ; ilay < NofLayers ; ilay++ ) {
      cout << "doing layer " << ilay << endl;
      layerPosition += LayerThick[ilay]/2.;
	if ( LayerName[ilay].Contains("space") && LayerThick[ilay] > 0.0 ) {
	  cout << "***** THE THICK SPACE LAYER IS AT " << layerPosition << endl;
	}
      if ( LayerName[ilay].Contains("space") && LayerName[ilay].Length() == 5 ) {
	layerPosition += LayerThick[ilay]/2.;
	continue;
      }

                 cout << "  got layer : " << LayerName[ilay].Data() << endl;

      Double_t msegPhiSpan = 360./(Double_t(DiskNFoils[istat]));
      Double_t msegBegin   =  90.;
      cout << "will do loop over segments" << endl;
      for ( Int_t miseg = 0 ; miseg < 1 ; miseg++ ) {
	cout << "segment " << miseg << endl;
	DiskLayersShapeA[ilay][istat][miseg] = new TGeoTube(Form("disk%dseg%d%sshape",istat+1,miseg+1,LayerName[ilay].Data()),
							   DiskInnerRadius[ilay][istat],DiskOuterRadius[ilay][istat],
							   LayerThick[ilay]/2.);
	DiskLayersShapeB[ilay][istat][miseg] = new TGeoBBox   (Form("robo%dseg%d%sshape",istat+1,miseg+1,LayerName[ilay].Data()),
							      MiddleROBarHfTh[istat],
							      DiskOuterRadius[ilay][istat],
							      LayerThick[ilay]);
	DiskLayersSubtr[ilay][istat][miseg] = new TGeoSubtraction(DiskLayersShapeA[ilay][istat][miseg],
								  DiskLayersShapeB[ilay][istat][miseg]);       

	DiskLayersShapeC[ilay][istat][miseg] = new TGeoCompositeShape(Form("comp%dseg%d%sshape",istat+1,miseg+1,LayerName[ilay].Data()),
								     DiskLayersSubtr[ilay][istat][miseg]);
	msegBegin += msegPhiSpan;
	cout << " msegBegin " << msegBegin << endl;

        DiskLayersShapeHole[ilay][istat][miseg] = new TGeoTube(Form("Hole%dseg%d%sshape",istat+1,miseg+1,LayerName[ilay].Data()),0.0,3.80,LayerThick[ilay]+0.01);

	 DiskLayersTranshA[ilay][istat][miseg] = new TGeoTranslation("hA",HoleTX,HoleTY,HoleTZ);
	   DiskLayersTranshA[ilay][istat][miseg] ->RegisterYourself();
	 DiskLayersTranshB[ilay][istat][miseg] = new TGeoTranslation("hB",HoleTX,-HoleTY,HoleTZ);
	   DiskLayersTranshB[ilay][istat][miseg] ->RegisterYourself();

        DiskLayersShapecompos[ilay][istat][miseg] = new TGeoCompositeShape(Form("compos%dseg%d%sshape",istat+1,miseg+1,LayerName[ilay].Data()),
							                   Form("comp%dseg%d%sshape-(Hole%dseg%d%sshape:hA)-(Hole%dseg%d%sshape:hB)",
									       istat+1,miseg+1,LayerName[ilay].Data(),istat+1,miseg+1,LayerName[ilay].Data(),istat+1,miseg+1,LayerName[ilay].Data()));

    
	TString layerMaterial = LayerName[ilay].Data();
	layerMaterial.Remove(0,layerMaterial.Last('_')+1);
		cout << "THE MATERIAL IS \"" << layerMaterial.Data() << "\"" << endl;
	DiskLayersVol[ilay][istat][miseg] = new TGeoVolume(Form("Gem_Disk%d_Seg%d_%s",istat+1,miseg+1,LayerName[ilay].Data()),
							    DiskLayersShapecompos[ilay][istat][miseg],
							    gGeoMan->GetMedium(layerMaterial.Data()));

 	cout << "layer material = " << layerMaterial.Data() << endl;
	if ( layerMaterial.Contains("air" ) )
	  DiskLayersVol[ilay][istat][miseg]->SetLineColor(kGray+1);
	if ( layerMaterial.Contains("copper" ) )
	  DiskLayersVol[ilay][istat][miseg]->SetLineColor(kOrange+1);
	if ( layerMaterial.Contains("kapton" ) )
	  DiskLayersVol[ilay][istat][miseg]->SetLineColor(kOrange+4);
	if ( layerMaterial.Contains("aluminium" ) )
	  DiskLayersVol[ilay][istat][miseg]->SetLineColor(kCyan-9);
	if ( layerMaterial.Contains("GEMmixture" ) )
	  DiskLayersVol[ilay][istat][miseg]->SetLineColor(kYellow);
        if ( layerMaterial.Contains("carbon" ) )
       	  DiskLayersVol[jlay][istat][miseg]->SetLineColor(kPink);
       	if ( layerMaterial.Contains("GlassFiber" ) )
       	  DiskLayersVol[jlay][istat][miseg]->SetLineColor(kGreen+3);

	DiskLayersTrans[ilay][istat][miseg] = new TGeoTranslation(0.,0.,layerPosition);
	DiskLayersCombi[ilay][istat][miseg] = new TGeoCombiTrans(*DiskLayersTrans[ilay][istat][miseg],*dummyrot);
	DiskLayersCombi[ilay][istat][miseg]->SetName(Form("Gem_Disk%d_Seg%d_%s",istat+1,miseg+1,LayerName[ilay].Data()));
	DiskLayersCombi[ilay][istat][miseg]->RegisterYourself();
	DiskVol[istat]->AddNode(DiskLayersVol[ilay][istat][miseg],0,DiskLayersCombi[ilay][istat][miseg]);       

      }
 //-----------------------------------------------------------------------------------------------------------------------------------------------------------     
       cout << "mvolume " << LayerName[ilay] << " from " 
       	   << setprecision(10) << DiskZPosition[istat]+layerPosition-LayerThick[ilay]/2. << " to "
       	   << setprecision(10) << DiskZPosition[istat]+layerPosition+LayerThick[ilay]/2. << endl;
      
       if ( LayerName[ilay].Contains("Gem") && LayerName[ilay].Contains("Sensor") ) {
       	Double_t mnewRadius = DiskInnerRadius[ilay][istat];
       	Double_t mnofStrips = 0;

       	cout << "mrad = " << DiskInnerRadius[ilay][istat] << " pitch = " << SensorStripPitch[msensorNumber][0] << " for sensor " << msensorNumber << endl;
       	if (  SensorStripType[msensorNumber] != 2 ) {
       	  mnofStrips = TMath::Ceil(2.*TMath::Pi()*DiskInnerRadius[ilay][istat]/SensorStripPitch[msensorNumber][0]);
       	  mnewRadius = mnofStrips*SensorStripPitch[msensorNumber][0]/2./TMath::Pi();
       	}
       	cout << "!!!! " << istat << " " << ilay << " > there shall be " << mnofStrips << " strips here so the radius should be " << mnewRadius << endl;
       	pout << "                        " << msensorNumber+1 << ",  " << SensorStripType[msensorNumber] << ",  " 
       	     << setw(9) << 0. << ",  "   // sets the field width to be used on output operations
       	     << setw(9) << 0. << ",  " 
       	     << setw(9) << DiskZPosition[istat]+layerPosition << ",  "
       	     << setw(9) << 0. << ",  "
       	     << setw(9) << mnewRadius << ",  "
       	     << setw(9) << DiskOuterRadius[ilay][istat] << ",  "
       	     << setw(9) << LayerThick[ilay] << ",  "
       	     << setw(9) << SensorStripAngle[msensorNumber][0] << ",  "
       	  // << setw(9) << SensorStripAngle[msensorNumber][1] << ",  "
       	     << setw(9) << MiddleROBarHfTh[istat] << ",  "
       	     << setw(9) << SensorStripPitch[msensorNumber][0] << ",  "
       	     << setw(9) << SensorStripPitch[msensorNumber][1] << ((istat==NofDisks-1&&msensorNumber==1)?"":", \\") 
       	     << endl;
       	msensorNumber++;
       }

       	layerPosition += LayerThick[ilay]/2.;
    }
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
    SubunitVol->AddNode(DiskVol[istat],0,DiskCombi[istat]);
      
       }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  pout << "TrackFinderOnHits_ParThetaA: Double_t  59.4" << endl
       << "TrackFinderOnHits_ParThetaB: Double_t -0.02" << endl
       << "TrackFinderOnHits_ParTheta0: Double_t  56.1372" << endl
       << "TrackFinderOnHits_ParTheta1: Double_t -0.000564362" << endl
       << "TrackFinderOnHits_ParTheta2: Double_t -0.181828" << endl
       << "TrackFinderOnHits_ParTheta3: Double_t  0.284289" << endl
       << "TrackFinderOnHits_ParRadPhi0:Double_t  0.9944432" << endl
       << "TrackFinderOnHits_ParRadPhi2:Double_t -0.000590706" << endl
       << "TrackFinderOnHits_ParMat0:   Double_t \\" << endl
    // << "                                   -2.31333e-6,    0.00067035,    0.10173" << endl
       << "                                   -2.35034328e-6,    0.0006810756,    0.10335768" << endl  
       << "TrackFinderOnHits_ParMat1:   Double_t \\" << endl
    // << "                                   -7.46844e-10,   -6.6696e-7,    0.000736672" << endl
       << "                                   -7.58793504e-10,   -6.7763136e-7,    0.000748458752" << endl   
       << "##########################################################################################" << flush;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  top->AddNode(SubunitVol,0,new TGeoCombiTrans());
  
   // top->CheckOverlaps(0.0001, "");
   // gGeoManager->CheckOverlaps(0.0001,""); // [cm]
   // gGeoManager->CheckGeometryFull();
  
   // TObjArray *listOfOverlaps = gGeoManager->GetListOfOverlaps();
   // cout << "************************************************" << endl;
   // cout<<listOfOverlaps->GetEntries()<<endl;
   // listOfOverlaps->Print();
   // cout << "************************************************" << endl;

   // gGeoManager->CheckOverlaps();
   // gGeoManager->PrintOverlaps();
  
  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
  //gGeoManager->Export(outfile);

  //top->Raytrace();
  top->Draw("ogl");
  //top->Draw();

  pout.close();

}

