#include "TGeoManager.h"
double Pi = TMath::Pi();



int hypGe_GeoBuilder_template()
{
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libHypGe");

	TString outfile= "testgeo.root";

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

	TGeoMedium *Ge = gGeoManager->GetMedium("germanium");
  
  
  TGeoMedium *Al = gGeoManager->GetMedium("HYPaluminium");
	
	

  TGeoManager *geom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TGeoVolume *top = new TGeoVolumeAssembly("hpGe");
	TGeoVolume *Realtop = new TGeoVolumeAssembly("Realtop");
	   
//cout<<" geom "<<geom<<endl;           
  geom->SetTopVolume(top);

	
	PndGeoHypGeModifiedBeamPipe *MBP = new PndGeoHypGeModifiedBeamPipe(Al, geom);
	MBP -> PlaceBeamPipe(Realtop);

	PndGeoHypGeModifiedCTFrame *MCTF = new PndGeoHypGeModifiedCTFrame(Al, geom);
	MCTF -> PlaceCTFrame(Realtop);

	Int_t CrystalNumber = 1;
	cout << "CrystalNumber: " << CrystalNumber << endl;
		PndGeoHypGeDoubleCluster *DoubleCluster = new PndGeoHypGeDoubleCluster(Ge,Al,1);
	//cout << "ClusterNumber 1: " << DoubleCluster->GetClusterNumber()<<endl;

	//DoubleCluster ->PlaceCluster(Realtop, 0,30,30,-30,0,&CrystalNumber);
	
	cout << "CrystalNumber: " << CrystalNumber << endl;
	//DoubleCluster ->PlaceCluster(Realtop, 0,30,30,-60,0,&CrystalNumber);
	
	cout << "CrystalNumber: " << CrystalNumber << endl;
	PndGeoHypGeTripleCluster *TripleCluster = new PndGeoHypGeTripleCluster(Ge,Al,DoubleCluster->GetClusterNumber());
	TripleCluster ->PlaceCluster(Realtop, 0,30,30,30,0,&CrystalNumber);
	cout << "CrystalNumber: " << CrystalNumber << endl;
	//TripleCluster ->PlaceCluster(Realtop, 0,30,30,60,0,&CrystalNumber);
	cout << "CrystalNumber: " << CrystalNumber << endl;
	//TripleCluster ->PlaceCluster(Realtop, 0,30,30,90,0,&CrystalNumber);
	cout << "CrystalNumber: " << CrystalNumber << endl;
	
	//PndGeoHypGeTripleCluster *TripleCluster2 = new PndGeoHypGeTripleCluster(Ge,Al,1);
	//TripleCluster2 ->PlaceCluster(Realtop, new TGeoTranslation(-20,0,0));
	top->AddNode(Realtop,0);
	
	Realtop->PrintNodes();
	geom->CloseGeometry();	
	cout << "# of Nodes: " << Realtop->CountNodes(10,1) << endl;
	//TripleCluster->PrintNodes(1);
	top->Write();
	fi->Close();
	
	//Realtop->Browse(new TBrowser);
	//c1 = new TCanvas("c1","hypGe",800,600);Realtop->Draw("");//	Realtop->Raytrace();
	Realtop->Draw("ogl");
	

	//c1->x3d();
	geom->CheckOverlaps(0.0000000001,option="d"); //1 Overlap von Strahlrohr + Kugel ist normal!!!!!!!!!!!!!!!!!
	geom->PrintOverlaps();
	
	
  return 0;
}


