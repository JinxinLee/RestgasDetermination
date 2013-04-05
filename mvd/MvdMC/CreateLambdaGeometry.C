/*
 * CreateLambdaGeometry.C
 *
 *  Created on: Mar 21, 2013
 *      Author: stockman
 */
CreateLambdaGeometry()
{
	  //-----------------------------
	Double_t positionOfDisk1inZ = 47.0;		//in cm
	Double_t positionOfDisk2inZ = 53.0;		//in cm

	  //--------------------------------------------------------------------
	  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	  
	  // Load this libraries
	  gSystem->Load("libGeoBase");
	  gSystem->Load("libParBase");
	  gSystem->Load("libBase");
	  gSystem->Load("libPndData");
	  gSystem->Load("libPassive");
	  
	  TString outfile= "../../geometry/LambdaDisksNew.root";
	  TFile* fi = new TFile(outfile,"RECREATE");  
	  
	  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
	  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	  geoFace->setMediaFile("../../geometry/media_pnd.geo");
	  geoFace->readMedia();
	  geoFace->print();

	  FairGeoMedia *Media =  geoFace->getMedia();
	  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

	  FairGeoMedium *CbmMediumSilicon  = Media->getMedium("silicon");
	  FairGeoMedium *CbmMediumCarbon  = Media->getMedium("carbon");
	  FairGeoMedium *CbmMediumCarbonFoam  = Media->getMedium("carbonfoam");
	  FairGeoMedium *CbmMediumAluminium = Media->getMedium("aluminium");
	  
	  Int_t nmed=geobuild->createMedium(CbmMediumSilicon);
	  nmed=geobuild->createMedium(CbmMediumCarbon);
	  nmed=geobuild->createMedium(CbmMediumCarbonFoam);
	  nmed=geobuild->createMedium(CbmMediumAluminium);

	  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

	  TGeoVolume *top = new TGeoVolumeAssembly("top");
	                    
	  gGeoMan->SetTopVolume(top);
	  
	TGeoVolume *combinedLambdaDisks = new TGeoVolumeAssembly("CombinedLambdaDisks");
	TGeoVolumeAssembly* lambdaDisk = new TGeoVolumeAssembly("LambdaDisk");
	TGeoVolumeAssembly* largeRing = new TGeoVolumeAssembly("LargeRing");
	TGeoVolumeAssembly* smallRing = new TGeoVolumeAssembly("SmallRing");

//	lambdaDisk->AddNode(largeSensorVolume,0,trc1);
//	TGeoShape* largeSensorShape  = new TGeoTrd1("StripSensorActiveLargeShape", 3.4593/2, 1.9936/2,0.03/2, 5.5665/2);
	Double_t points [16];
	points[0] = -1.7297;		points[1] = 2.7833;
	points[2] = 1.7297; 		points[3] = 2.7833;
	points[4] = 0.9968; 		points[5] = -2.7833;
	points[6] = -0.9968; 		points[7] = -2.7833;

	points[8] = -1.7297; 		points[9] = 2.7833;
	points[10] = 1.7297; 		points[11] = 2.7833;
	points[12] = 0.9968; 		points[13] = -2.7833;
	points[14] = -0.9968; 		points[15] = -2.7833;

	TGeoShape* largeSensorShape  = new TGeoArb8("StripSensorActiveLargeShape", 0.03/2, points);
	TGeoVolume* largeSensorVolume = new TGeoVolume("StripSensorActiveLargeTrap",largeSensorShape,gGeoMan->GetMedium("silicon"));

//	TGeoRotation rotSensor("rotSensor",0,90,0);
	TGeoRotation rotSensor("rotSensor",0,0,0);
	TGeoTranslation transLargeSensor(0,(14.8669 + 5.5665)/2,0);
	TGeoCombiTrans combined1(transLargeSensor, rotSensor);

	for (int i = 0; i < 12; i++){
		TString rotName("rotLargeSens");
		rotName.Append(i+1);
		TGeoRotation rotSens(rotName.Data(), 0,0, i*2 * 360/24);
		TGeoHMatrix sens = rotSens * combined1;
		largeRing->AddNode(largeSensorVolume, i, new TGeoHMatrix(sens));
	}

	Double_t pointsSmall [16];
	pointsSmall[0] = -1.9579/2;		pointsSmall[1] = 5.3358/2;
	pointsSmall[2] = 1.9579/2; 		pointsSmall[3] = 5.3358/2;
	pointsSmall[4] = 0.5529/2; 		pointsSmall[5] = -5.3358/2;
	pointsSmall[6] = -0.5529/2; 		pointsSmall[7] = -5.3358/2;

	pointsSmall[8] = -1.9579/2;		pointsSmall[9] = 5.3358/2;
	pointsSmall[10] = 1.9579/2; 	pointsSmall[11] = 5.3358/2;
	pointsSmall[12] = 0.5529/2; 	pointsSmall[13] = -5.3358/2;
	pointsSmall[14] = -0.5529/2; 	pointsSmall[15] = -5.3358/2;

//	TGeoShape* smallSensorShape = new TGeoTrd1("StripSensorActiveSmallShape", 1.9579/2, 0.5529/2, 0.03/2, 5.3358/2);
	TGeoShape* smallSensorShape = new TGeoArb8("StripSensorActiveSmallShape", 0.03/2, pointsSmall);
	TGeoVolume* smallSensorVolume = new TGeoVolume("StripSensorActiveSmallTrap", smallSensorShape, gGeoMan->GetMedium("silicon"));

	TGeoTranslation transSmallSensor(0,5.3358/2+2.1,0);
	TGeoCombiTrans combined2(transSmallSensor, rotSensor);

	for (int i = 0; i < 12; i++){
		TString rotName("rotSmallSens");
		rotName.Append(i+1);
		TGeoRotation rotSens(rotName.Data(), 0,0, i*2 * 360/24);
		TGeoHMatrix sens = rotSens * combined2;
		smallRing->AddNode(smallSensorVolume, i+12, new TGeoHMatrix(sens));
	}

	lambdaDisk->AddNode(largeRing, 0, new TGeoTranslation(0,0,-0.5));
	lambdaDisk->AddNode(largeRing, 1, new TGeoRotation("largeRingRot",0,0,360/24));

	TGeoCombiTrans combiSmallRing2(TGeoTranslation(0,0,-1.5), TGeoRotation("smallRingRot",0,0,360/24));
	lambdaDisk->AddNode(smallRing, 0, new TGeoTranslation(0,0,-1.0));
	lambdaDisk->AddNode(smallRing, 1, new TGeoHMatrix(combiSmallRing2));

	TGeoVolumeAssembly* supportDisk = new TGeoVolumeAssembly("SupportDisk");

	TGeoVolumeAssembly* supportLarge = new TGeoVolumeAssembly("SupportLarge");
		TGeoShape* carbonFoamShape = new TGeoBBox(3.4/2, 0.2/2, 3.5/2);
		TGeoVolume* carbonFoam = new TGeoVolume("CarbonFoam", carbonFoamShape, gGeoMan->GetMedium("carbonfoam"));
		TGeoShape* carbonFiberShape = new TGeoBBox(3.4/2, 0.02/2, 3.5/2);
		TGeoVolume* carbonFiber = new TGeoVolume("CarbonFiber", carbonFiberShape, gGeoMan->GetMedium("carbon"));
		TGeoShape* siliconReadoutShape = new TGeoBBox(3.4/2, 0.02/2, 3.5/2);
		TGeoVolume* siliconReadout = new TGeoVolume("SiliconReadout", siliconReadoutShape, gGeoMan->GetMedium("silicon"));
		TGeoShape* aluCablesShape = new TGeoBBox(3.4/2, 0.4/2, 3.5/2);
		TGeoVolume* aluCables = new TGeoVolume("AluCables", aluCablesShape, gGeoMan->GetMedium("aluminium"));

		supportLarge->AddNode(carbonFoam, 1);
		supportLarge->AddNode(carbonFiber, 1, new TGeoTranslation(0,(0.2 + 0.02)/2, 0));
		supportLarge->AddNode(carbonFiber, 2, new TGeoTranslation(0,-(0.2 + 0.02)/2, 0));
		supportLarge->AddNode(siliconReadout, 1, new TGeoTranslation(0,(0.2 + 0.02 + 0.02)/2, 0));
		supportLarge->AddNode(siliconReadout, 2, new TGeoTranslation(0,-(0.2 + 0.02 + 0.02)/2, 0));
		supportLarge->AddNode(aluCables, 1, new TGeoTranslation(0,(0.2 + 0.02 + 0.02 + 0.4)/2, 0));
		supportLarge->AddNode(aluCables, 2, new TGeoTranslation(0,-(0.2 + 0.02 + 0.02 + 0.4)/2, 0));

		TGeoTranslation transSupportLarge(0,(14.8669)/2 + 5.5665 + 1,0);

		for (int i = 0; i < 24; i++){
			TString rotName("rotSupportLarge");
			rotName.Append(i+1);
			TGeoRotation rotSens(rotName.Data(), 0,0, i * 360/24);
			TGeoHMatrix sens = rotSens * transSupportLarge;
			supportDisk->AddNode(supportLarge, i+1, new TGeoHMatrix(sens));
		}

		TGeoVolumeAssembly* supportSmall = new TGeoVolumeAssembly("SupportSmall");
		TGeoShape* carbonFoamShapeSmall = new TGeoBBox(1.8/2, 0.2/2, 3.5/2);
		TGeoVolume* carbonFoamSmall = new TGeoVolume("CarbonFoamSmall", carbonFoamShapeSmall, gGeoMan->GetMedium("carbonfoam"));
		TGeoShape* carbonFiberShapeSmall = new TGeoBBox(1.8/2, 0.02/2, 3.5/2);
		TGeoVolume* carbonFiberSmall = new TGeoVolume("CarbonFiberSmall", carbonFiberShapeSmall, gGeoMan->GetMedium("carbon"));
		TGeoShape* siliconReadoutShapeSmall = new TGeoBBox(1.8/2, 0.02/2, 3.5/2);
		TGeoVolume* siliconReadoutSmall = new TGeoVolume("SiliconReadoutSmall", siliconReadoutShapeSmall, gGeoMan->GetMedium("silicon"));
		TGeoShape* aluCablesShapeSmall = new TGeoBBox(1.8/2, 0.4/2, 3.5/2);
		TGeoVolume* aluCablesSmall = new TGeoVolume("AluCablesSmall", aluCablesShapeSmall, gGeoMan->GetMedium("aluminium"));

		supportSmall->AddNode(carbonFoamSmall, 1);
		supportSmall->AddNode(carbonFiberSmall, 1, new TGeoTranslation(0,(0.2 + 0.02)/2, 0));
		supportSmall->AddNode(carbonFiberSmall, 2, new TGeoTranslation(0,-(0.2 + 0.02)/2, 0));
		supportSmall->AddNode(siliconReadoutSmall, 1, new TGeoTranslation(0,(0.2 + 0.02 + 0.02)/2, 0));
		supportSmall->AddNode(siliconReadoutSmall, 2, new TGeoTranslation(0,-(0.2 + 0.02 + 0.02)/2, 0));
		supportSmall->AddNode(aluCablesSmall, 1, new TGeoTranslation(0,(0.2 + 0.02 + 0.02 + 0.4)/2, 0));
		supportSmall->AddNode(aluCablesSmall, 2, new TGeoTranslation(0,-(0.2 + 0.02 + 0.02 + 0.4)/2, 0));

		TGeoTranslation transSupportSmall(0,2.1 + 5.3358 + 0.5,0);

		for (int i = 0; i < 24; i++){
			TString rotName("rotSupportSmall");
			rotName.Append(i+1);
			TGeoRotation rotSens(rotName.Data(), 0,0, i * 360/24);
			TGeoHMatrix sens = rotSens * transSupportSmall;
			supportDisk->AddNode(supportSmall, i+1, new TGeoHMatrix(sens));
		}

	combinedLambdaDisks->AddNode(lambdaDisk,1,new TGeoTranslation(0,0,positionOfDisk1inZ));
	combinedLambdaDisks->AddNode(lambdaDisk,2,new TGeoTranslation(0,0,positionOfDisk2inZ));
	combinedLambdaDisks->AddNode(supportDisk,1, new TGeoTranslation(0,0,(positionOfDisk1inZ + positionOfDisk2inZ)/2));

	top->AddNode(combinedLambdaDisks,0);

	  gGeoMan->CloseGeometry();
	  top->Write();
	  fi->Close();
	//   gGeoManager->Export(outfile);
	  gGeoManager->SetVisLevel(30);
	  top->Draw("ogl"); 
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
