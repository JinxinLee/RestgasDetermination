/*
 * CreateLambdaGeometry.C
 *
 *  Created on: Mar 21, 2013
 *      Author: stockman
 */
MvdTBGeometry()
{
	  //-----------------------------
	Double_t positionStage1 = 10.0;		//in cm
	Double_t positionStage2 = 16.0;		//in cm
	Double_t positionStage3 = 22.0;		//in cm
	Double_t positionStage4 = 28.0;		//in cm

	  //--------------------------------------------------------------------

	  
	  TString outfile= "../../geometry/MvdTBGeometry.root";
	  TFile* fi = new TFile(outfile,"RECREATE");  
	  
	  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
	  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	  geoFace->setMediaFile("../../geometry/media_pnd.geo");
	  geoFace->readMedia();
	  geoFace->print();

	  FairGeoMedia *Media =  geoFace->getMedia();
	  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

	  FairGeoMedium *CbmMediumSilicon  = Media->getMedium("silicon");
	  
	  Int_t nmed=geobuild->createMedium(CbmMediumSilicon);

	  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

	  TGeoVolume *top = new TGeoVolumeAssembly("top");
	                    
	  gGeoMan->SetTopVolume(top);
	  

	TGeoVolumeAssembly* TBSetup = new TGeoVolumeAssembly("MvdTestbeamSetup");
	TGeoVolumeAssembly* module = new TGeoVolumeAssembly("ToPix4Module");

	TGeoShape* ToPix4Shape = new TGeoBBox("ToPix4Shape", 0.2/2, 0.32/2, 0.0285/2);			//todo: fix dimensions of ToPix4
	TGeoVolume* ToPix4Volume = new TGeoVolume("ToPix4Volume", ToPix4Shape, gGeoMan->GetMedium("silicon"));

	TGeoShape* EpiSensorShape = new TGeoBBox("EpiSensorShape", 0.2/2, 0.32/2, 0.0100/2);
	TGeoVolume* EpiSensorVolume = new TGeoVolume("EpiSensorVolume", EpiSensorShape, gGeoMan->GetMedium("silicon"));


	TGeoTranslation transToPix("ToPixTranslation", .0, .0, 0.0285/2 + 0.0100/2 + 0.002);

	module->AddNode(EpiSensorVolume, 0);
	module->AddNode(ToPix4Volume, 0, new TGeoHMatrix(transToPix));

	TBSetup->AddNode(module, 0, new TGeoTranslation(.0,.0,positionStage1));
	TBSetup->AddNode(module, 1, new TGeoTranslation(.0,.0,positionStage2));
	TBSetup->AddNode(module, 2, new TGeoTranslation(.0,.0,positionStage3));
	TBSetup->AddNode(module, 3, new TGeoTranslation(.0,.0,positionStage4));

	top->AddNode(TBSetup,0);

	  gGeoMan->CloseGeometry();
	  top->Write();
	  fi->Close();
	//   gGeoManager->Export(outfile);
	  gGeoManager->SetVisLevel(30);
	  top->Draw("ogl"); 
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
