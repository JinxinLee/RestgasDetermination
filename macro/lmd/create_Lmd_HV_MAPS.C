// compile by adding the lmd subdirectory to the CMakesLists of the macro folder

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

//#include<TH2.h>
//#include<TPad.h>

//using namespace LmdDim;
// to check the position and dimensions
// one may enable a beam pipe dummy to be displayed
const bool show_beam_pipe_dummy = true;

const bool include_box = false;

void Navigate_into(int depth = 0){
	//for (int idepth = 0; idepth < depth; idepth++){
	//	cout << "        ";
	//}
	cout << gGeoManager->GetPath() << " ";
	cout << "current node is " << gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetName() << endl;
	int nnodes = gGeoManager->GetCurrentNode()->GetNdaughters();
	for (int i = 0; i < nnodes; i++){
		cout << " navigating into node " << i << endl;
		gGeoManager->CdDown(i);
		Navigate_into(depth+1);
		gGeoManager->CdUp();
	}
}

#include <sstream>
#include <vector>

int create_HV_MAPS(bool misalign = true) {
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
	char* dirc = getenv("VMCWORKDIR");
	string dir;
	if(dirc == NULL){
		//env must be set, will crash otherwise
		cout << "warning! environment variable VMCWORKDIR not found! set this variable and try again!\n";
		exit(1);
	}
	else{
		dir = dirc;
	}

	//string moveto = dir + "/input/" + fileName + ".dat";
	//system(("mv "+fileName+".dat"+" "+moveto).c_str());

	TString outfile;
	if (misalign)
		outfile = dir+"/geometry/Luminosity-Detector_misaligned-0u.root";
	else
		outfile = dir+"/geometry/Luminosity-Detector.root";
	TFile* fi = new TFile(outfile,"RECREATE");

	FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
	FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	geoFace->setMediaFile((dir+"/geometry/media_pnd.geo").c_str());
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

	// for testing purposes ************************
	// only misaligned sensors, everything else should be perfectly aligned
	lmddim.side_offset_x = 0;
	lmddim.side_offset_y = 0;
	lmddim.side_offset_z = 0;
	lmddim.side_tilt_phi = 0;
	lmddim.side_tilt_theta = 0;
	lmddim.side_tilt_psi = 0;
	lmddim.cvd_offset_x = 0;
	lmddim.cvd_offset_y = 0;
	lmddim.cvd_offset_z = 0;
	lmddim.cvd_tilt_phi = 0;
	lmddim.cvd_tilt_theta = 0;
	lmddim.cvd_tilt_psi = 0;
	lmddim.plane_half_offset_x = 0;
	lmddim.plane_half_offset_y = 0;
	lmddim.plane_half_offset_z = 0;
	lmddim.plane_half_tilt_phi = 0;
	lmddim.plane_half_tilt_theta = 0;
	lmddim.plane_half_tilt_psi = 0;
	lmddim.half_offset_x = 0;
	lmddim.half_offset_y = 0;
	lmddim.half_offset_z = 0;
	lmddim.half_tilt_phi = 0;
	lmddim.half_tilt_theta = 0;
	lmddim.half_tilt_psi = 0;
	// **********************************************
	lmddim.Generate_rootgeom(*top, misalign);
	//lmddim.Write_transformation_matrices("matrices.txt", false);

	if (!misalign)
		lmddim.Write_transformation_matrices("",false); // create default matrices
	else
		//lmddim.Write_transformation_matrices(dir+"/geometry/trafo_matrices_lmd_misaligned.dat", false); // create a misaligned version
		lmddim.Write_transformation_matrices(dir+"/geometry/trafo_matrices_lmd_misaligned-0u.dat", false); // create a misaligned version
	/*
	  TH2* hist = lmddim.Get_histogram_Plane(0,0,false,true,false);
	  hist->Fill(-5,-5,177);
	  hist->Fill(5,5,2);
	 // hist->SetMinimum(5);
	  hist->Draw("text");
	  gPad->Print("hist.pdf");
	  gPad->Print("hist.C");*/

	//lmddim.Write_transformation_matrices("", false); // generate standard matrices in VMCWORKDIR/geometry

	//lmddim.Read_transformation_matrices(dir+"geometry/matrices.txt", true);
	//lmddim.Write_transformation_matrices(dir+"geometry/matrices_aligned.txt", true);

	gGeoMan->CloseGeometry();

	gGeoMan->CheckOverlaps(0.001); // [cm]
	//gGeoManager->CheckGeometryFull();
	gGeoMan->PrintOverlaps();
	top->Write();
	fi->Close();
	//gGeoManager->Export(outfile);
	gGeoMan->SetVisLevel(20);
	//lmddim.Retrieve_version_number();
	top->Draw("ogl");

	cout << " found " << gGeoManager->GetListOfPhysicalNodes()->GetEntries() << " entries " << endl;

	if (!misalign){
		cout << " trying to navigate through the geometry " << endl;
		vector<string> list_of_sensors;
		cout << lmddim.Get_List_of_Sensors(list_of_sensors) << endl;

		int offset;

		if (lmddim.Test_List_of_Sensors(list_of_sensors, offset)) cout << " testing of sensors was fine and gave an offset of " << offset << endl;

		//cout << " found " << list_of_sensors.size() << " sensors " << endl;

		//for (int isensor = 0; isensor < list_of_sensors.size() ; isensor++){
		//	  cout << list_of_sensors[isensor] << endl;
		//}

		if (1){ // consistency checks
			cout << " reading matrices from file into the aligned map " << endl;
			lmddim.Read_transformation_matrices(dir+"/geometry/trafo_matrices_lmd_misaligned.dat", true);
			cout << " loading transformation matrices from geometry into not aligned map " << endl;
			lmddim.Read_transformation_matrices_from_geometry(false);
			cout << " writing matrices from aligned map to geometry" << endl;
			lmddim.Write_transformation_matrices_to_geometry(true);
			cout << " reading transformation matrices from geometry once again " << endl;
			lmddim.Read_transformation_matrices_from_geometry(true);
			//cout << " testing matrices " << endl;
			//lmddim.Calc_matrix_offsets();
		}

		if (0){ // test setting individual matrices
			TGeoHMatrix* matrix = lmddim.Get_matrix("/lum_1/lmd_vol_vac_3/lmd_vol_ref_sys_0/lmd_vol_half_1", false, 1,-1,-1,-1,-1,-1);
			matrix->Print();
			matrix->RotateX(90.);
			matrix->Print();
			lmddim.Set_matrix("/lum_1/lmd_vol_vac_3/lmd_vol_ref_sys_0/lmd_vol_half_1", matrix, 1,-1,-1,-1,-1,-1);

			cout << " the original matrix is " << endl;
			matrix = lmddim.Get_matrix("/lum_1/lmd_vol_vac_3/lmd_vol_ref_sys_0/lmd_vol_half_1", false, 1,-1,-1,-1,-1,-1);
			matrix->Print();

			cout << " the aligned matrix is " << endl;
			matrix = lmddim.Get_matrix("/lum_1/lmd_vol_vac_3/lmd_vol_ref_sys_0/lmd_vol_half_1", true, 1,-1,-1,-1,-1,-1);
			matrix->Print();
		}
		cout << " testing matrices " << endl;
		lmddim.Calc_matrix_offsets();

		gGeoMan->RefreshPhysicalNodes(); // should be called but is not a must
		//gGeoMan->CloseGeometry();
		top->Draw("ogl"); // an already drawn geometry will be not updated according to changes in the matrices

	}
	//gGeoManager->CdTop();
	//Navigate_into();

	/*
	  gGeoManager->cd("/lum_1/lmd_vol_vac_3/lmd_vol_ref_sys_0/"
		"lmd_vol_half_1/lmd_vol_plane_3/lmd_vol_module_4/"
		"lmd_vol_side_1/lmd_vol_die_1/LumActivePixelRect_398");
	  gGeoManager->GetCurrentNode()->GetMatrix();

//gGeoManager->cd("/lum_1/lmd_vol_vac_3/lmd_vol_ref_sys_0");
	  //cout << gGeoManager->GetPath() << " " << gGeoManager->GetCurrentNode()->GetName() << endl;
	  //gGeoManager->GetCurrentNode()->GetMatrix()->Print();
	  //gGeoManager->GetCurrentMatrix()->Print();


	  gGeoManager->cd("/lum_1/lmd_vol_vac_3");
	  TGeoVolume* topvol = gGeoManager->GetCurrentNode()->GetVolume();
	  if (topvol){
	  gGeoManager->SetTopVolume(topvol);
	  gGeoManager->cd("/lmd_vol_ref_sys_0");
	  cout << gGeoManager->GetPath() << " " << gGeoManager->GetCurrentNode()->GetName() << endl;
	  gGeoManager->GetCurrentNode()->GetMatrix()->Print();
	  gGeoManager->GetCurrentMatrix()->Print();
	 */

	// according to http://personalpages.to.infn.it/~puccio/htmldoc/src/AliTRDalignment.cxx.html
	/*
	  TGeoPNEntry *pne;
	  TGeoMatrix* ideSm[400];
	  TGeoMatrix* misSm[400];
	  for (int isensor = 0; isensor < 399; isensor++){
		  stringstream path;
		  path << "/lum_1/lmd_vol_vac_3/lmd_vol_ref_sys_0/lmd_vol_half_1/lmd_vol_plane_3/lmd_vol_module_4/lmd_vol_side_1/lmd_vol_die_1/LumActivePixelRect_" << isensor;
		  gGeoManager->cd(path.str().c_str());
		  cout << gGeoManager->GetCurrentNode()->GetName() << endl;
		  pne = gGeoManager->GetAlignableEntry(path.str().c_str());
		  if (pne){
			  TGeoPhysicalNode* node = pne->GetPhysicalNode();
			  if (!node) {
				  cout << "physical node entry has no physical node " << path.str() << endl;
				  continue;
			  }
			  misSm[isensor] = new TGeoHMatrix(*node->GetNode(node->GetLevel())->GetMatrix());
			  ideSm[isensor] = new TGeoHMatrix(*node->GetOriginalMatrix());
		  } else {
			  cout << " sorry could not get alignable entry from sensor " << isensor << endl;
		  }
	  }*/

	//   gGeoManager->Export(outfile);
	//gGeoMan->SetVisLevel(20);
	//top->Draw("ogl");
	//top->RandomPoints(100000);
  return 0;
}

#include<TApplication.h>

int main() {
	TApplication myapp("myapp", 0, 0);
	cout << "before\n";
	create_HV_MAPS();
	cout << "after\n";
	myapp.Run();
	return 0;
}
