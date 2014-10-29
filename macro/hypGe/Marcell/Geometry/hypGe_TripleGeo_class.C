#include "TGeoManager.h"
double Pi = TMath::Pi();

void ClusterPlacer(double x, double y, double z, double GlobalZOffset , int ObjectNo, TGeoVolume *top,TGeoVolume *Clusterr,double phi, double theta, double psi)
{
TGeoRotation *Clusterrot = new TGeoRotation("Clusterrot",phi, theta,psi-phi);
		Clusterrot->RegisterYourself();
top->AddNode(Clusterr,ObjectNo,new TGeoCombiTrans(x,y,GlobalZOffset+z,Clusterrot));
}

void hypGe_TripleGeo()
{

gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
	TString outfile= "../../../../geometry/hypGe_GeoMarcell_TripleClass.root";

  TFile* fi = new TFile(outfile,"RECREATE");

	

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
	//Strahlrohr
	TGeoVolume *Strahlrohr =geom->MakeTube("Strahlrohr",Al_sol,0,7.5,150);
	Strahlrohr->SetLineColor(kBlue);
	//top->AddNode(Strahlrohr,ObjektNr);
	ObjektNr++;
	//Außenhülle
	TGeoVolume *Außenhülle =geom->MakeTube("Außenhülle",Al_sol,45,45.5,100);
	Außenhülle->SetLineColor(kBlue);
	//top->AddNode(Außenhülle,ObjektNr,new TGeoTranslation(0,0,GlobalZOffset));
	ObjektNr++;

//Halteplatte
	TGeoVolume *HoldingStruc =geom->MakeBox("HoldingStruc",Al_sol,1,45,50);
	HoldingStruc->SetLineColor(kGreen);
	//top->AddNode(HoldingStruc,ObjektNr,new TGeoTranslation(0,0,GlobalZOffset));
	ObjektNr++;
//Hilfskugel
	TGeoSphere *Kugel = new TGeoSphere("Kugel",1,30,0,180,0,360);
	TGeoVolume *Kugell = new TGeoVolume("Kugell",Kugel,Al_sol);
	Kugell->SetLineColor(kBlue);
	Kugell->SetTransparency(50);
	//top->AddNode(Kugell,ObjektNr,new TGeoTranslation(0,0,GlobalZOffset));
	ObjektNr++;
	
//Nullpunktmarker
	TGeoSphere *Origin = new TGeoSphere("Origin",0,1,0,180,0,360);
	TGeoVolume *Originn = new TGeoVolume("Originn",Origin,Al_sol);
	Originn->SetLineColor(kGreen);
	//top->AddNode(Originn,ObjektNr);
	ObjektNr++;

ClusterPlacer(7.0801,12.5751, -26.3009, GlobalZOffset , 1000, strange,Cluster[0],180+150.62, 28.73,180-4.95);
cout << "test2" << endl;
ClusterPlacer(12.8422,-0, -27.1123, GlobalZOffset , 2000, strange,Cluster[1],180+90,25.34,180);
 ClusterPlacer(7.0801,-12.5751, -26.3009, GlobalZOffset , 3000, strange,Cluster[2],180+29.38,28.73,180+4.95);

 ClusterPlacer(-7.0801,-12.5751, -26.3009, GlobalZOffset , 4000, strange,Cluster[3],150.62,28.73,4.95);
 ClusterPlacer(-12.8422,0, -27.1123, GlobalZOffset , 5000, strange,Cluster[4],90,25.34,0);
 ClusterPlacer(-7.0801,12.5751, -26.3009, GlobalZOffset , 6000, strange,Cluster[5],29.38,28.73,4.95);

//Outer Ring
ClusterPlacer(8.421,20.8241, -19.8857, GlobalZOffset , 7000, strange,Cluster[6],180+157.98,48.5,0); //Patient
ClusterPlacer(17.9414,10.7963, -21.4836, GlobalZOffset , 8000, strange,Cluster[7],180+121.04,44.29,180-4.95);
ClusterPlacer(24.3173,0, -17.5689, GlobalZOffset , 9000, strange,Cluster[8],180+90,54.51,0);
ClusterPlacer(17.9414,-10.7963, -21.4836, GlobalZOffset , 10000, strange,Cluster[9],180+180-121.04,44.29,180+4.95);
ClusterPlacer(8.421,-20.8241, -19.8857, GlobalZOffset , 11000, strange,Cluster[10],180+180-157.98,48.5,0); //Patient

ClusterPlacer(-8.421,-20.8241, -19.8857, GlobalZOffset , 12000, strange,Cluster[11],+157.98,48.5,180); //Patient
ClusterPlacer(-17.9414,-10.7963, -21.4836, GlobalZOffset , 13000, strange,Cluster[12],+121.04,44.29,-4.95);
ClusterPlacer(-24.3173,0, -17.5689, GlobalZOffset , 14000, strange,Cluster[13],+90,54.51,180);
ClusterPlacer(-17.9414,10.7963, -21.4836, GlobalZOffset , 15000, strange,Cluster[14],+180-121.04,44.29,4.95);
ClusterPlacer(-8.421,20.8241, -19.8857, GlobalZOffset , 16000, strange,Cluster[15],+180-157.98,48.5,180); //Patient

	top->AddNode(strange,0);
	geom->CloseGeometry();
	top->Write();
	fi->Close();
	//strange->Draw("");	strange->Raytrace();
	strange->Draw("ogl");
	geom->CheckOverlaps(0.0000000001,option="d"); //1 Overlap von Strahlrohr + Kugel ist normal!!!!!!!!!!!!!!!!!
	geom->PrintOverlaps();
	cout <<"Anzahl der Objekte: "<< ObjektNr << endl;
	//Paket[1]->Print();
}


