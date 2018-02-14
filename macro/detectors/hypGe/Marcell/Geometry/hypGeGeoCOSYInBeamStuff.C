#include "TGeoManager.h"
double Pi = TMath::Pi();


TString BuildOutputNameInBeamStuff()
{
	TString outfileWithoutPath = "hypGeGeoCOSYInBeamStuff.root";
	return outfileWithoutPath;
}

TString hypGeGeoCOSYInBeamStuff( Double_t Angle = 0)
{
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libHypGe");
	
	TString outfileWithoutPath =BuildOutputNameInBeamStuff();
	TString outfile= "../../../../geometry/";
	outfile += outfileWithoutPath;

	cout << outfile << endl;

  TFile* fi = new TFile(outfile,"RECREATE");

 FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../../../geometry/media_pnd.geo");
  geoFace->readMedia();
  //geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *medScint  = Media->getMedium("MDTPlastic");
  Int_t nmedcap=geobuild->createMedium(medScint);
	TGeoMedium *Scint = gGeoManager->GetMedium("MDTPlastic");

  FairGeoMedium *medPlexi  = Media->getMedium("acrylicGlass");
  nmedcap=geobuild->createMedium(medPlexi);
	TGeoMedium *Plexi = gGeoManager->GetMedium("acrylicGlass");

  FairGeoMedium *medTargetH  = Media->getMedium("TargetH");
  nmedcap=geobuild->createMedium(medTargetH);
	TGeoMedium *TargetH = gGeoManager->GetMedium("TargetH");

  FairGeoMedium *medPVC  = Media->getMedium("PVC");
  nmedcap=geobuild->createMedium(medPVC);
	TGeoMedium *PVC = gGeoManager->GetMedium("PVC");
	
  TGeoManager *geom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TGeoVolume *top = new TGeoVolumeAssembly("hpGe");
	TGeoVolume *Realtop = new TGeoVolumeAssembly("Realtop");
	   
//cout<<" geom "<<geom<<endl;           
  geom->SetTopVolume(top);
	TGeoRotation *SiPmRot = new TGeoRotation("SiPmRot",0,0,0);
	
	TGeoVolume* Dirc1 = geom->MakeBox("Dirc1",Plexi,100,50,2);			// close dirc
	Realtop->AddNode(Dirc1,1,new TGeoCombiTrans(0,-40,-52,SiPmRot));
	
	TGeoVolume* Dirc2 = geom->MakeBox("Dirc2",Plexi,100,50,2.5);		// "far" dirc
	Realtop->AddNode(Dirc2,2,new TGeoCombiTrans(0,+40,-102.5,SiPmRot));
	
	TGeoVolume* Scint1 = geom->MakeBox("Scint1",Scint,10,10,0.4);			// Scint1
	Realtop->AddNode(Scint1,3,new TGeoCombiTrans(0,0,-120.4,SiPmRot));
	
	TGeoVolume* Scint2 = geom->MakeBox("Scint2",Scint,10,10,0.25);		// Scint2
	Realtop->AddNode(Scint2,4,new TGeoCombiTrans(0,0,-127.5,SiPmRot));
	
	TGeoVolume* Hodo1 = geom->MakeTube("Hodo1",Scint,0,20,0.3);			// Hodo1
	Realtop->AddNode(Hodo1,5,new TGeoCombiTrans(0,0,-132.8,SiPmRot));
	
	TGeoVolume* Hodo2 = geom->MakeBox("Hodo2",Scint,20,20,0.2);		// Hodo2
	Realtop->AddNode(Hodo2,6,new TGeoCombiTrans(0,0,-223.2,SiPmRot));
	
	TGeoVolume* Scint3 = geom->MakeBox("Scint3",Scint,25,15,0.5);		// Scint3
	Realtop->AddNode(Scint3,7,new TGeoCombiTrans(0,0,-253.5,SiPmRot));
	
	TGeoVolume* BPEndCap = geom->MakeTube("BPEndCap",PVC,0,5,0.05);		// BPEndCap
	Realtop->AddNode(BPEndCap,8,new TGeoCombiTrans(0,0,-293.05,SiPmRot));
	
	TGeoVolume* TOFTarget = geom->MakeBox("TOFTarget",TargetH,5,5,0.3);		// BPEndCap
	Realtop->AddNode(TOFTarget,9,new TGeoCombiTrans(0,0,-943.3,SiPmRot));
	
	top->AddNode(Realtop,0);
	geom->CloseGeometry();	
	cout << "# of Nodes: " << Realtop->CountNodes(10,1) << endl;

	top->Write();
	fi->Close();
	
	Realtop->Draw("ogl");
	


	
	
	
	return outfileWithoutPath; // returns the name of the created geometry file
}


