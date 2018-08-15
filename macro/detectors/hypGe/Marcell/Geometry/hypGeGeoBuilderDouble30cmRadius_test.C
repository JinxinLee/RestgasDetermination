#include "TGeoManager.h"
double Pi = TMath::Pi();



int hypGeGeoBuilderDouble30cmRadius_test()
{
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libHypGe");

	TString outfile= "../../../../geometry/hypGeGeoDoubleCluster_V3.root";

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
	Double_t GlobalZOffset = -55;
	Double_t ClusterRadius = 30;
	
	PndGeoHypGeModifiedBeamPipe *MBP = new PndGeoHypGeModifiedBeamPipe(Al, geom);
	//MBP -> PlaceBeamPipe(Realtop);

	PndGeoHypGeModifiedCTFrame *MCTF = new PndGeoHypGeModifiedCTFrame(Al, geom);
	//MCTF -> PlaceCTFrame(Realtop,new TGeoTranslation(0,0,GlobalZOffset));


	
	
	PndGeoHypGeDoubleCluster *DoubleCluster = new PndGeoHypGeDoubleCluster(Ge,Al,1);


//geometry is build in 4 quarters:
//		3	|	2
//	----o----		looking in +z (beam forward) direction
//		4	|	1
	for (Int_t iHalves = 0; iHalves < 1; iHalves++)
	{
		
		//iHalves = 0 -> right half (first and second quarter), iHalves = 1 -> left half (third and fourth quarter) (looking in +z (beam forward) direction
		//first quarter

		DoubleCluster ->PlaceCluster(Realtop,  0,0,  0, 0, 0,&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius,  180*iHalves + 180-55.6699, 29.9031, 180-(-50.21),&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius,  180*iHalves + 180-13.4046, 52.2607, 180-(-8.3),&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius,  180*iHalves +180-31.5985, 50.2457, 180-(-21.67) ,&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius,  180*iHalves + 180-50.5126, 51.6831, 180-(-35.43),&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius,  180*iHalves + 180-75.1143, 46.348, 180-(9.44),&CrystalNumber);
		
		//second quarter
		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius, 180*iHalves + 23.1678, 27.7747, -20.74,&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius, 180*iHalves + 55.6699, 29.9031, -50.21,&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius, 180*iHalves + 13.4046, 52.2607, -8.3,&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius, 180*iHalves + 31.5985, 50.2457, -21.67,&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius, 180*iHalves + 50.5126, 51.6831, -35.43,&CrystalNumber);

		//DoubleCluster ->PlaceCluster(Realtop,  GlobalZOffset,ClusterRadius, 180*iHalves + 75.1143, 46.348, 9.44,&CrystalNumber);
	}

	
	top->AddNode(Realtop,0);	
	Realtop->PrintNodes();
	geom->CloseGeometry();	
	cout << "# of Nodes: " << Realtop->CountNodes(10,1) << endl;
	DoubleCluster->PrintNodes(0);
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


