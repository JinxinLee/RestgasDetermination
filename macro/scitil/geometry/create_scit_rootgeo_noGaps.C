void create_scit_rootgeo_noGaps(Double_t distance = 750){
  // Forward tof geometry parameters
  //-----------------------------
  //-- macro created by Alicia S. based on step file
  //-- barrel-SciTil_18122012
  //-- edited by D. Steinschaden 
  //-- last update 05.2015


 //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  TString outfile= "../../../geometry/SciTil_noGaps.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *MediumSci  = Media->getMedium("polyvinyltoluene"); 
  FairGeoMedium *MediumSiPm = Media->getMedium("silicon"); 
  FairGeoMedium *MediumCard = Media->getMedium("pcbmvd");

  const Double_t pi = 3.1415926535;
  Int_t nmed=geobuild->createMedium(MediumSci); // ----why is the return value saved?
   nmed=geobuild->createMedium(MediumSiPm);
  nmed=geobuild->createMedium(MediumCard);

   TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
   TGeoVolume *top = new TGeoVolumeAssembly("SciT");
   
                    
  gGeoMan->SetTopVolume(top);
  

//-------------------- creating Tile and SiPm and Readout Card --------------------

  //Volumes containing SENSOR in the name will be taken into account in the MC hit production
  //check PndSciT.h and PndSciT.cxx and the listofsensitives 

  TGeoTube* TileShape;
  TileShape = new TGeoTube("SciTil_logicTile", 50.3, 50.8, 89.925.);// dimension is always given from the mid to the end e.g. only the half length

  TGeoVolume* TileVol;
  TileVol = new TGeoVolume("SciTil_SENSOR",TileShape, gGeoMan->GetMedium("polyvinyltoluene"));
  TileVol->SetLineColor(kBlue);
  
  

  // --------------creating module (Line)-------------
  
  TGeoVolumeAssembly* AssVol = new TGeoVolumeAssembly("SciTil_Line");// 2 tiles + 16 SiPm + 3 connection cards


  AssVol->AddNode(TileVol,0,new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));

	  

  top->AddNode(AssVol,0, new TGeoCombiTrans(0.,0.,32.,new TGeoRotation()));


	  gGeoMan->SetVisLevel(5);
	  gGeoMan->CloseGeometry();
	  top->Write();
	  fi->Close();
	  // gGeoManager->Export(outfile);
	  top->Draw("ogl");
	  
  }  
  
