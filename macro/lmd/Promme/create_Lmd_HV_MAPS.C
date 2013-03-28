// compile with
// g++ $(root-config --cflags --glibs) -lGeom -I${PANDAROOT}/geobase -L${PANDAROOT}/buildPanda/lib -lGeoBase create_Lmd_HV_MAPS.C -o /tmp/create_geometry -I${PANDAROOT}/lmd/LmdMC
// and run with
// /tmp/create_geometry

#include<TROOT.h>
#include<FairGeoLoader.h>
#include<FairGeoInterface.h>
#include<FairGeoBuilder.h>
#include<FairGeoPcon.h>
#include<FairGeoMedia.h>
#include<TGeoCompositeShape.h>
#include<TGeoMatrix.h>
#include<TGeoVolume.h>
#include<TGeoTube.h>
#include<TGeoArb8.h>
#include<TGeoPara.h>
#include<TSystem.h>
#include<TFile.h>
#include<TGeoPcon.h>
#include<TGeoManager.h>
#include<TGeoTorus.h>
#include<TGeoSphere.h>
#include<TMath.h>
#include<TRandom.h>
#include<PndLmdDim.h>
#include<TGeoPhysicalNode.h>
#include<TGeoCone.h>

#include<iostream>
#include<cmath>
#include<sstream>

//using namespace LmdDim;

// to check the position and dimensions
// one may enable a beam pipe dummy to be displayed
const bool show_beam_pipe_dummy = true;

const bool include_box = false;

void create_HV_MAPS(bool misalign = false) {
	/*
	// ****************************** Parameters of Detector ************************
	//--------------------------------------------------------------------
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	// Load this libraries
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libPndData");
	gSystem->Load("libPassive");


	TString outfile = "geo/HV_MAPS-Design.root";
	TFile* fi = new TFile(outfile, "RECREATE");

	FairGeoLoader* geoLoad = NULL;// FairGeoLoader::Instance();//
	if (!geoLoad){
		cout << " creating FairGeoLoader instance " << endl;
		geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
	}

	FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	geoFace->setMediaFile("../../geometry/media_pnd.geo");
	geoFace->readMedia();
	geoFace->print();

	FairGeoMedia *Media = geoFace->getMedia();
	FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

	// retrieve available media
	std::string str_ti = "Aluminum"; // "titanium" is in Panda software not yet implemented
	FairGeoMedium* FairMediumAir = Media->getMedium("air");
	FairGeoMedium* FairMediumSteel = Media->getMedium("steel");
	FairGeoMedium* FairMediumTi = Media->getMedium(str_ti.c_str()); //titanium"); not found in media_pnd.geo !
	FairGeoMedium *FairMediumSilicon = Media->getMedium("silicon");
	FairGeoMedium *FairMediumDiamond = Media->getMedium("HYPdiamond");
	FairGeoMedium *FairMediumVacuum = Media->getMedium("vacuum");
	FairGeoMedium *FairMediumKapton = Media->getMedium("kapton");

	if (!FairMediumAir || !FairMediumSteel || !FairMediumTi || !FairMediumKapton || !FairMediumSilicon || !FairMediumVacuum) {
		std::cout << " warning: not all media found " << std::endl;
	}

	int nmed;
	nmed = geobuild->createMedium(FairMediumAir);
	nmed = geobuild->createMedium(FairMediumSteel);
	nmed = geobuild->createMedium(FairMediumTi);
	nmed = geobuild->createMedium(FairMediumKapton);
	nmed = geobuild->createMedium(FairMediumSilicon);
	nmed = geobuild->createMedium(FairMediumDiamond);
	nmed = geobuild->createMedium(FairMediumVacuum);

	TGeoManager* gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
	if (!gGeoMan) {
		cout << " no FAIR Geometry manager found! " << endl;
		return;
	}

	TGeoVolume *top = new TGeoVolumeAssembly("lmd_HV_MAPS");

	gGeoMan->SetTopVolume(top);
	PndLmdDim& lmddim = PndLmdDim::Get_instance();
*/
	  //--------------------------------------------------------------------
	  //gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	  // Load this libraries
	  //gSystem->Load("libGeoBase");
	  //gSystem->Load("libParBase");
	  //gSystem->Load("libBase");
	  //gSystem->Load("libPndData");
	  //gSystem->Load("libPassive");

	  TString outfile= "geo/HV_MAPS-Design.root";
	  TFile* fi = new TFile(outfile,"RECREATE");

	  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
	  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	  geoFace->setMediaFile("../../geometry/media_pnd.geo");
	  geoFace->readMedia();
	  geoFace->print();

	  FairGeoMedia *Media =  geoFace->getMedia();
	  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

	  FairGeoMedium *FairMediumSilicon = Media->getMedium("silicon");

	  Int_t nmed=geobuild->createMedium(FairMediumSilicon);

	  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

	  TGeoVolume *top = new TGeoVolumeAssembly("lum");

	  gGeoMan->SetTopVolume(top);

	  PndLmdDim& lmddim = PndLmdDim::Get_instance();

	  // for testing purposes
	  lmddim.Generate_rootgeom(*top, misalign);
	  //lmddim.Write_transformation_matrices("matrices.txt", false);
	  if (!misalign)
		  lmddim.Write_transformation_matrices("matrices_perfect.txt", false);
	  else
		  lmddim.Write_transformation_matrices("matrices.txt", false);

	  //lmddim.Read_transformation_matrices("matrices.txt", true);
	  //lmddim.Write_transformation_matrices("matrices_aligned.txt", true);

	  gGeoMan->CloseGeometry();
	  top->Write();
	  fi->Close();
	//   gGeoManager->Export(outfile);
	  gGeoMan->SetVisLevel(20);
	  top->Draw("ogl");
	//   gGeoManager->Export(outfile);
	//gGeoMan->SetVisLevel(20);
	//top->Draw("ogl");
	//top->RandomPoints(100000);
}

#include<TApplication.h>

int main() {
	TApplication myapp("myapp", 0, 0);
	create_HV_MAPS();
	myapp.Run();
	return 0;
}
