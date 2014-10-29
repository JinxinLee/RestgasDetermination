#include "TGeoManager.h"
double Pi = TMath::Pi();



void CrystalShapeTest()
{

gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");


 FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *medGe  = Media->getMedium("germanium");
  Int_t nmedGe=geobuild->createMedium(medGe);
    
  // aluminum capsule
  FairGeoMedium *medcap  = Media->getMedium("HYPaluminium");
  Int_t nmedcap=geobuild->createMedium(medcap);

	TGeoMedium *Ge = gGeoManager->GetMedium("germanium");
  
  //capsule
  TGeoMedium *Al_sol = gGeoManager->GetMedium("HYPaluminium");


  TGeoManager *geom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TGeoVolume *top = new TGeoVolumeAssembly("hpGe");
	TGeoVolume *strange = new TGeoVolumeAssembly("strange");
	   
cout<<" geom "<<geom<<endl;           
  geom->SetTopVolume(top);
	
	Double_t GlobalZOffset = -55;
	int ObjektNr = 0;
	
	Double_t 						CapsuleThickness;
	Double_t 						VacuumThickness;
	Double_t						CrystalLenght;
	Double_t						CrystalFrontPgonInnerRadius;
	
	Double_t						CrystalInnerPgonRadius;
	//objects to build the main crystal shape
	TGeoTube 						*CrystalTube;
	TGeoPgon 						*CrystalPgon;
	//objects to build the inner crystal hole
	TGeoTube						*CrystalInnerTube;
	TGeoTranslation			*CrystalInnerTubeZTranslation;
	TGeoTorus						*CrystalInnerTubeTorus;
	TGeoTranslation			*CrystalInnerTubeTorusZTranslation;
	TGeoTube						*CrystalInnerTubeMiniExtraTube;
	TGeoTranslation			*CrystalInnerTubeMiniExtraTubeZTranslation;
	//objects to get the top rounding of the crystal
	TGeoTube						*CrystalTopRoundingTube;
	TGeoTranslation			*CrystalTopRoundingTubeZTranslation;
	TGeoTorus						*CrystalTopRoundingTorus;
	TGeoCompositeShape	*CrystalTopRoundingTooling;
	TGeoTranslation			*CrystalTopRoundingToolingZTranslation;

	//objects to build the guard ring of the crystal
	TGeoTube						*CrystalGuardRingTube;
	TGeoTranslation			*CrystalGuardRingTubeZTranslation;
	TGeoTorus						*CrystalGuardRingTorus;
	TGeoTorus						*CrystalGuardRingTorusInner;
	TGeoTranslation			*CrystalGuardRingTorusZTranslation;
	TGeoTube						*CrystalGuardRingMiniExtraTube;
	TGeoTranslation			*CrystalGuardRingMiniExtraTubeZTranslation;

	//building the whole crystal
	TGeoCompositeShape *CrystalShape;
	TGeoVolume *Crystal;

	
	


	CapsuleThickness = 0.08;
	VacuumThickness = 0.08;
	CrystalLenght = 7.85;
	CrystalFrontPgonInnerRadius = 2.9485;
	
	//building the main shape of the crystal
	CrystalTube = new TGeoTube("CrystalTube",0,3.5,CrystalLenght/2);
	CrystalPgon = new TGeoPgon("CrystalPgon",0.,360.,6,2);
		CrystalPgon->DefineSection(0,-CrystalLenght/2,CrystalFrontPgonInnerRadius,3.8);
		CrystalPgon->DefineSection(1,8-CrystalLenght/2,CrystalFrontPgonInnerRadius+8*TMath::Tan(Pi/180*4.12),3.8);

	//building the inner hole of the crystal
	CrystalInnerTube = new TGeoTube("CrystalInnerTube",0,0.6,3.05);
	CrystalInnerTubeZTranslation = new TGeoTranslation("CrystalInnerTubeZTranslation",0,0,CrystalLenght/2-3.05);
		CrystalInnerTubeZTranslation->RegisterYourself();
	CrystalInnerTubeTorus = new TGeoTorus("CrystalInnerTubeTorus",0.2,0,0.4,0,360);
	CrystalInnerTubeTorusZTranslation = new TGeoTranslation("CrystalInnerTubeTorusZTranslation",0,0,CrystalLenght/2-3.05*2);
		CrystalInnerTubeTorusZTranslation->RegisterYourself();
	CrystalInnerTubeMiniExtraTube = new TGeoTube("CrystalInnerTubeMiniExtraTube",0,0.2,0.2);
	CrystalInnerTubeMiniExtraTubeZTranslation = new TGeoTranslation("CrystalInnerTubeMiniExtraTubeZTranslation",0,0,CrystalLenght/2-3.05*2-0.2);
		CrystalInnerTubeMiniExtraTubeZTranslation ->RegisterYourself();

	//building the top rounding of the crystal
	CrystalTopRoundingTube = new TGeoTube("CrystalTopRoundingTube",2.9,3.5,0.3);
	CrystalTopRoundingTubeZTranslation = new TGeoTranslation("CrystalTopRoundingTubeZTranslation",0,0,0.3);
		CrystalTopRoundingTubeZTranslation->RegisterYourself();
	CrystalTopRoundingTorus = new TGeoTorus("CrystalTopRoundingTorus",2.9,0,0.6,0,360);
	CrystalTopRoundingTooling = new TGeoCompositeShape ("CrystalTopRoundingTooling"	,"CrystalTopRoundingTube:CrystalTopRoundingTubeZTranslation-CrystalTopRoundingTorus");
	CrystalTopRoundingToolingZTranslation = new TGeoTranslation("CrystalTopRoundingToolingZTranslation",0,0,CrystalLenght/2-0.6);
		CrystalTopRoundingToolingZTranslation->RegisterYourself();

	//building the guard ring of the crystal
	CrystalGuardRingTube = new TGeoTube("CrystalGuardRingTube",0.9,1.4,0.15);
	CrystalGuardRingTubeZTranslation = new TGeoTranslation("CrystalGuardRingTubeZTranslation",0,0,CrystalLenght/2-0.15);
		CrystalGuardRingTubeZTranslation->RegisterYourself();
	CrystalGuardRingTorus = new TGeoTorus("CrystalGuardRingTorus",1.2,0,0.2,0,360);
	CrystalGuardRingTorusInner = new TGeoTorus("CrystalGuardRingTorusInner",1.1,0,0.2,0,360);
	CrystalGuardRingTorusZTranslation = new TGeoTranslation("CrystalGuardRingTorusZTranslation",0,0,CrystalLenght/2-0.15*2);
		CrystalGuardRingTorusZTranslation->RegisterYourself();
	CrystalGuardRingMiniExtraTube = new TGeoTube("CrystalGuardRingMiniExtraTube",1.1,1.2,0.2);
	CrystalGuardRingMiniExtraTubeZTranslation = new TGeoTranslation("CrystalGuardRingMiniExtraTubeZTranslation",0,0,CrystalLenght/2-0.15*2);
		CrystalGuardRingMiniExtraTubeZTranslation ->RegisterYourself();

	//combining the final shape of the crystal		
	CrystalShape= new TGeoCompositeShape("CrystalShape","(CrystalTube-CrystalPgon)-CrystalTopRoundingTooling:CrystalTopRoundingToolingZTranslation-(CrystalInnerTube:CrystalInnerTubeZTranslation+CrystalInnerTubeTorus:CrystalInnerTubeTorusZTranslation+CrystalInnerTubeMiniExtraTube:CrystalInnerTubeMiniExtraTubeZTranslation)-(CrystalGuardRingTube:CrystalGuardRingTubeZTranslation+CrystalGuardRingTorus:CrystalGuardRingTorusZTranslation+CrystalGuardRingTorusInner:CrystalGuardRingTorusZTranslation+CrystalGuardRingMiniExtraTube:CrystalGuardRingMiniExtraTubeZTranslation)"); //	(main shape) - (inner tube) - (top rounding) - (guard ring)
	Crystal = new TGeoVolume("Crystal",CrystalShape,Ge);
	Crystal->SetLineColor(kGreen);
	Crystal->SetFillColor(kGreen);

	
//TGeoTranslation *CryTrans = new TGeoTranslation(0,0,5-1.5+CapsuleThickness+VacuumThickness);
//CryTrans->RegisterYourself();
strange->AddNode(Crystal,6);



	top->AddNode(strange,0);
	geom->CloseGeometry();

	
	//strange->Draw("");	strange->Raytrace();
	strange->Draw("ogl");
	
	
}


