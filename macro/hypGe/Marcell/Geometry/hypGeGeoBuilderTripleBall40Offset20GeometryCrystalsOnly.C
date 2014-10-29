#include "TGeoManager.h"
double Pi = TMath::Pi();


void hypGeGeoBuilderTripleBall40Offset20GeometryCrystalsOnly()
{
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libHypGe");

	TString outfile= "../../../../geometry/hypGeGeoTripleCluster_Ball40_Offset20CrystalsOnly.root";

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
	Double_t SourceToBallCenterOffset = 20;	
	Double_t GlobalZOffset = -55+SourceToBallCenterOffset;
	
	//Place Support
	PndGeoHypGeModifiedBeamPipe *MBP = new PndGeoHypGeModifiedBeamPipe(Al, geom);
	//MBP -> PlaceBeamPipe(Realtop);

	PndGeoHypGeModifiedCTFrame *MCTF = new PndGeoHypGeModifiedCTFrame(Al, geom);
	//MCTF -> PlaceCTFrame(Realtop,new TGeoTranslation(0,0,GlobalZOffset));

	//Place a sphere to check the right positioning of the clusters
	//TGeoSphere *Ball = new TGeoSphere("Ball",0,40,0,180,0,360);
	//TGeoVolume *BallVol = new TGeoVolume("BallVol",Ball,Al);
	//Realtop->AddNode(BallVol,0,new TGeoTranslation(0,0,-45));

	PndGeoHypGeTripleClusterCrystalsOnly *TripleCluster = new PndGeoHypGeTripleClusterCrystalsOnly(Ge,Al,1);
	
	//Read coordinates and angles from file

	const Int_t NumberOfCluster=16;
	Double_t CoordinatesAndAngles[NumberOfCluster*6];
	string Filename = "TripleBall40GeometryCoordinatesAndAngles.txt";


	TripleCluster->ReadCoordinatesAndAngles(Filename,NumberOfCluster,CoordinatesAndAngles);

	//for (Int_t i = 0; i < 16*6; i++)
	//	cout << CoordinatesAndAngles[i] << endl;
	//	Double_t ClusterRadius = 30;

	for (Int_t iCluster = 0; iCluster < 16; iCluster++)
	{

			
		TripleCluster ->PlaceCluster(Realtop,CoordinatesAndAngles[6*(iCluster)],CoordinatesAndAngles[6*(iCluster)+1],CoordinatesAndAngles[6*(iCluster)+2],  GlobalZOffset,CoordinatesAndAngles[6*(iCluster)+3],CoordinatesAndAngles[6*(iCluster)+4],CoordinatesAndAngles[6*(iCluster)+5],&CrystalNumber);
	
		
	}

	
	top->AddNode(Realtop,0);	
	Realtop->PrintNodes();
	geom->CloseGeometry();	
	cout << "# of Nodes: " << Realtop->CountNodes(10,1) << endl;
	TripleCluster->PrintNodes(0);
	top->Write();
	fi->Close();
	
	//Realtop->Browse(new TBrowser);
	//c1 = new TCanvas("c1","hypGe",800,600);Realtop->Draw("");//	Realtop->Raytrace();
	Realtop->Draw("ogl");
	

	//c1->x3d();
	//geom->CheckOverlaps(0.0000000001,option="d"); //1 Overlap von Strahlrohr + Kugel ist normal!!!!!!!!!!!!!!!!!
	//geom->PrintOverlaps();
	
	
}


