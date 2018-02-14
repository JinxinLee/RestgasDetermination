#include "TGeoManager.h"
double Pi = TMath::Pi();

TString BuildOutputNameBeamDump(Double_t BeamDumpDist)
{
	TString outfileWithoutPath = "hypGeGeoCOSYBeamDumpTOF_dist";
	outfileWithoutPath += BeamDumpDist;
	outfileWithoutPath += "cm.root";
	return outfileWithoutPath;
}

TString hypGeGeoCOSYBeamDumpTOF(Double_t BeamDumpDist_ext )//= 200)
{
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libHypGe");
	

	TString outfileWithoutPath = BuildOutputNameBeamDump(BeamDumpDist_ext);

	TString outfile= "../../../../geometry/";
	outfile += outfileWithoutPath;
  TFile* fi = new TFile(outfile,"RECREATE");

  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../MiscFiles/moded_media_pnd.geo");
  geoFace->readMedia();
  //geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *mediron  = Media->getMedium("iron");
  geobuild->createMedium(mediron);
	TGeoMedium *Iron = gGeoManager->GetMedium("iron");

	FairGeoMedium *medconcrete  = Media->getMedium("concrete");
  geobuild->createMedium(medconcrete);
	TGeoMedium *Concrete = gGeoManager->GetMedium("concrete");
	
  TGeoManager *geom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TGeoVolume *top = new TGeoVolumeAssembly("hpGe");
	TGeoVolume *Realtop = new TGeoVolumeAssembly("Realtop");
	 TGeoVolume *BeamDumpAssembly = new TGeoVolumeAssembly("BeamDumpAssembly");  
//cout<<" geom "<<geom<<endl;           
  geom->SetTopVolume(top);

	//Inner part - iron
	
	Double_t BeamTubeRadius = 30;
	TGeoTube *BackTube = new TGeoTube("BackTube",0,100,120);
	TGeoTube *FrontTube = new TGeoTube("FrontTube",BeamTubeRadius,50,20);
	
	TGeoTranslation *FrontTubeTranslation = new TGeoTranslation("FrontTubeTranslation",0,0,-140);
	FrontTubeTranslation->RegisterYourself();
	TGeoCompositeShape *BeamDumpIronShape = new TGeoCompositeShape("BeamDumpIronShape","BackTube+FrontTube : FrontTubeTranslation");
	TGeoVolume *BeamDumpIron = new TGeoVolume("BeamDumpIron",BeamDumpIronShape,Iron);
	BeamDumpIron->SetLineColor(4);
	BeamDumpIron->SetFillColor(4);
	BeamDumpAssembly->AddNode(BeamDumpIron,1);
	
	// Outer part - concrete
	
	TGeoBBox *BeamDumpBox = new TGeoBBox("BeamDumpBox",200,200,200);
	TGeoTube *BeamTube = new TGeoTube ("BeamTube", 0,BeamTubeRadius,30);
	TGeoTranslation *BeamTubeTranslation = new TGeoTranslation ("BeamTubeTranslation",0,0,-190);
	BeamTubeTranslation->RegisterYourself();
	TGeoCompositeShape *BeamDumpConcreteShape = new TGeoCompositeShape("BeamDumpConcreteShape", "BeamDumpBox - BeamDumpIronShape -  BeamTube : BeamTubeTranslation");
	
	TGeoVolume *BeamDumpConcrete = new TGeoVolume("BeamDumpConcrete", BeamDumpConcreteShape, Concrete);
	BeamDumpConcrete->SetLineColor(14);
	BeamDumpConcrete->SetFillColor(14);
	BeamDumpAssembly->AddNode(BeamDumpConcrete,1);
	
	Realtop->AddNode(BeamDumpAssembly,0,new TGeoTranslation(0,0,200+BeamDumpDist_ext));
	top->AddNode(Realtop,0);
	geom->CloseGeometry();	
	cout << "# of Nodes: " << Realtop->CountNodes(10,1) << endl;

	top->Write();
	fi->Close();
	
	Realtop->Draw("ogl");
	
	//Realtop->Draw();

	
	
	
	return outfileWithoutPath; // returns the name of the created geometry file
}


