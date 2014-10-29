#include "TGeoManager.h"
double Pi = TMath::Pi();


void hypGeCableAbsorptionTest(Double_t FoilThickness_Ext = 2)
{
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libHypGe");

	TString outfile= "../../../../geometry/hypGeGeoCableAbsorptionTest_";
	outfile += FoilThickness_Ext;
	outfile += "cm.root";
	cout << outfile << endl;

  TFile* fi = new TFile(outfile,"RECREATE");

 FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../../../geometry/media_pnd.geo");
  geoFace->readMedia();
  //geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *medGe  = Media->getMedium("germanium");
  Int_t nmedGe=geobuild->createMedium(medGe);
    

  FairGeoMedium *medcap  = Media->getMedium("HYPaluminium");
  Int_t nmedcap=geobuild->createMedium(medcap);

  FairGeoMedium *medkapton  = Media->getMedium("kapton");
  Int_t nmedcap=geobuild->createMedium(medkapton);

  
	TGeoMedium *Ge = gGeoManager->GetMedium("germanium");
  
  TGeoMedium *Al = gGeoManager->GetMedium("HYPaluminium");
	
	TGeoMedium *Kapton = gGeoManager->GetMedium("kapton")

  TGeoManager *geom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TGeoVolume *top = new TGeoVolumeAssembly("hpGe");
	TGeoVolume *Realtop = new TGeoVolumeAssembly("Realtop");
	   
//cout<<" geom "<<geom<<endl;           
  geom->SetTopVolume(top);


	Double_t FoilThickness = FoilThickness_Ext;
	TGeoVolume* Foil = geom->MakeBox("Foil",Kapton,10,10,FoilThickness/2);
	
	Realtop->AddNode(Foil,1,new TGeoTranslation(0,0,-60));
	
	top->AddNode(Realtop,0);
	geom->CloseGeometry();	
	cout << "# of Nodes: " << Realtop->CountNodes(10,1) << endl;

	top->Write();
	fi->Close();
	
	//Realtop->Browse(new TBrowser);
	//c1 = new TCanvas("c1","hypGe",800,600);Realtop->Draw("");//	Realtop->Raytrace();
	Realtop->Draw("ogl");
	

	//c1->x3d();
	//geom->CheckOverlaps(0.0000000001,option="d"); //1 Overlap von Strahlrohr + Kugel ist normal!!!!!!!!!!!!!!!!!
	//geom->PrintOverlaps();
	
	
}


