#include "TGeoManager.h"
double Pi = TMath::Pi();


TString BuildOutputNameSiPm()
{
	TString outfileWithoutPath = "hypGeGeoCOSYSiPm.root";
	return outfileWithoutPath;
}

TString hypGeGeoCOSYSiPm( Double_t Angle = 0)
{
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libHypGe");

	TString outfileWithoutPath =BuildOutputNameSiPm();
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

  FairGeoMedium *medSilicon  = Media->getMedium("HYPsilicon");
  Int_t nmedcap=geobuild->createMedium(medSilicon);

	TGeoMedium *Silicon = gGeoManager->GetMedium("HYPsilicon");

  TGeoManager *geom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TGeoVolume *top = new TGeoVolumeAssembly("hpGe");
	TGeoVolume *Realtop = new TGeoVolumeAssembly("Realtop");
	              
  geom->SetTopVolume(top);

	TGeoVolume* SiPm = geom->MakeBox("CrystalSiPm_1",Silicon,0.15,0.15,0.005);
	SiPm->SetLineColor(kYellow);
	SiPm->SetFillColor(kYellow);
	TGeoRotation *SiPmRot = new TGeoRotation("SiPmRot",0,0,0);
	Realtop->AddNode(SiPm,20000,new TGeoCombiTrans(60,0,10,SiPmRot));
	
	TGeoVolume* SiPm2 = geom->MakeBox("CrystalSiPm_2",Silicon,0.15,0.15,0.005);
	SiPm2->SetLineColor(kYellow);
	SiPm2->SetFillColor(kYellow);
	Realtop->AddNode(SiPm2,20001,new TGeoCombiTrans(-25,0,20,SiPmRot));
	

	top->AddNode(Realtop,0);
	geom->CloseGeometry();	
	cout << "# of Nodes: " << Realtop->CountNodes(10,1) << endl;

	top->Write();
	fi->Close();
	
	Realtop->Draw("ogl");
	


	
	
	
	return outfileWithoutPath; // returns the name of the created geometry file
}


