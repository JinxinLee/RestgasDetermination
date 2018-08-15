#include "TGeoManager.h"
double Pi = TMath::Pi();

TString BuildOutputNameDetector(Double_t ClusterRadius)
{
	TString outfileWithoutPath = "hypGeGeoSingle_radius";
	outfileWithoutPath += ClusterRadius;
	outfileWithoutPath += "cm.root";
	return outfileWithoutPath;
}

int hypGeGeoBuilderSingle(Double_t ClusterRadius_ext = 30)
{
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libHypGe");

	
	TString outfileWithoutPath = BuildOutputNameDetector(ClusterRadius_ext);

	TString outfile= "../../../../geometry/";
	outfile += outfileWithoutPath;

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

	Int_t CrystalNumber = 1;
	Double_t GlobalZOffset = 0;
	Double_t ClusterRadius = ClusterRadius_ext; //default is 30
	
	PndGeoHypGeModifiedBeamPipe *MBP = new PndGeoHypGeModifiedBeamPipe(Al, geom);
	//MBP -> PlaceBeamPipe(Realtop);

	PndGeoHypGeModifiedCTFrame *MCTF = new PndGeoHypGeModifiedCTFrame(Al, geom);
	//MCTF -> PlaceCTFrame(Realtop,new TGeoTranslation(0,0,GlobalZOffset));


	
	
	PndGeoHypGeSingleCluster *SingleCluster = new PndGeoHypGeSingleCluster(Ge,Al,1);

	SingleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius, 0,30,0,&CrystalNumber);
	SingleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius, 0,60,0,&CrystalNumber);

	top->AddNode(Realtop,0);	
	Realtop->PrintNodes();
	geom->CloseGeometry();	
	cout << "# of Nodes: " << Realtop->CountNodes(10,1) << endl;
	SingleCluster->PrintNodes(0);
	top->Write();
	fi->Close();
	cout << "Wrote geometry to file: " << outfileWithoutPath << endl;
	
	//Realtop->Browse(new TBrowser);
	//c1 = new TCanvas("c1","hypGe",800,600);Realtop->Draw("");//	Realtop->Raytrace();
	Realtop->Draw("ogl");
	

	//c1->x3d();
	//geom->CheckOverlaps(0.0000000001,option="d"); //1 Overlap von Strahlrohr + Kugel ist normal!!!!!!!!!!!!!!!!!
	//geom->PrintOverlaps();
	
	
  return 0;
}


