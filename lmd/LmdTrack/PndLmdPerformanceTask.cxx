// -------------------------------------------------------------------------
// -----              PndLmdPerformance Task source file               -----
// -----                     Based on PndLmdGeaneTask                  -----
// -----                   Author: Prometeusz Jasinski                 -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <TMatrixDSym.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TStyle.h>
#include <sstream>

// framework includes
#include "FairRootManager.h"
#include "PndLmdPerformanceTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "PndMCTrack.h"
#include "FairBaseParSet.h"
#include "TGeant3.h"
#include "FairTrackParH.h"
#include "FairTrackParP.h"
#include "TDatabasePDG.h"

// PndSds includes
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"
//PndLmd includes
#include "PndLinTrack.h"
#include "TGaxis.h"
//#include "PndLmdDim.h"

// GenFit includes
#include "RKTrackRep.h"
#include "GFFieldManager.h"
#include "GFPandaField.h"

#include <vector>
#include <map>

// -----   Default constructor   -------------------------------------------
PndLmdPerformanceTask::PndLmdPerformanceTask() :
	FairTask("Geane Task for PANDA Lmd"), fEventNr(0), fUseMVDPoint(false) {
	fPro = NULL;
	fGeoH = NULL;
	hist_output_file = NULL;
	last_percent = -1;
	fnevents = -1;
	fievent = 0;
}
// -------------------------------------------------------------------------

PndLmdPerformanceTask::PndLmdPerformanceTask(Double_t pBeam, TVector3 IP) :
	FairTask("Geane Task for PANDA Lmd"), fEventNr(0), fUseMVDPoint(false) {
	fPbeam = pBeam;
	// if(fPDGid!=-2212){ //calculate momentum if particle is not antiproton
	//     }
	hist_output_file = NULL;
	cout << "Beam Momentum for particle with PDGid#" << fPDGid
			<< " this run is " << fPbeam << endl;
	vtx = IP;
	cout << "Interaction Point:" << endl;
	vtx.Print();
	last_percent = -1;
	fnevents = -1;
	fievent = 0;
}

// -----   Destructor   ----------------------------------------------------
PndLmdPerformanceTask::~PndLmdPerformanceTask() {
	if (hist_output_file) hist_output_file->Close();
	delete hist_output_file;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdPerformanceTask::Init() {

	//if (hist_output_file)
	//		hist_output_file->cd();

	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndLmdPerformanceTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	} else {
		fnevents = ioman->CheckMaxEventNo();
	}

	/*
	fMCHits = (TClonesArray*) ioman->GetObject("LMDPoint");
	if (!fMCHits) {
		std::cout << "-W- PndLmdPerformanceTask::Init: " << "No LMDPoint"
				<< " array!" << std::endl;
		return kERROR;
	}*/

	/*fTracks = (TClonesArray*) ioman->GetObject("LMDTrack");
	if (!fTracks) {
		std::cout << "-W- PndLmdPerformanceTask::Init: " << "No Track"
				<< " array!" << std::endl;
		return kERROR;
	}

	fHits = (TClonesArray*) ioman->GetObject("LMDHitsStrip");
	if (!fHits) {
		std::cout << "-W- PndLmdPerformanceTask::Init: " << "No Hits"
				<< " array!" << std::endl;
		return kERROR;
	}*/

	//--- MC info -----------------------------------------------------------------
	true_tracks = (TClonesArray*) ioman->GetObject("MCTrack"); //True Track to compare
	/*	if (!true_tracks) {
	 std::cout << "-W- PndLmdPerformanceTask::Init: " << "No MCTrack"
	 << " array!" << std::endl;
	 return kFATAL;
	 }*/
	true_points = (TClonesArray*) ioman->GetObject("LMDPoint"); //True Points to compare
	//----------------------------------------------------------------------------------


	//--- Digitization info ------------------------------------------------------------
	//fStripClusterArray
	//		= (TClonesArray*) ioman->GetObject("LMDStripClusterCand");
	//fStripDigiArray = (TClonesArray*) ioman->GetObject("LMDStripDigis");
	//----------------------------------------------------------------------------------

	//--- Real Hits --------------------------------------------------------------------
	//rechit_array = (TClonesArray*) ioman->GetObject("LMDHitsStrip"); //Points for Tracks
	//----------------------------------------------------------------------------------


	//--- Track Candidate ---------------------------------------------------------------
	//trkcand_array = (TClonesArray*) ioman->GetObject("LMDTrackCand"); //Points for Track Canidates
	//-----------------------------------------------------------------------------------

	//--- Real tracks -------------------------------------------------------------------
	//rec_trk = (TClonesArray*) ioman->GetObject("LMDTrack"); //Tracks
	//----------------------------------------------------------------------------------

	fDetName = new TClonesArray("TObjString");
	ioman->Register("DetName", "Geane", fDetName, kTRUE);

	fPro = new FairGeanePro();
	//fPro->PropagateToPCA(1, -1);
	fGeoH = PndGeoHandling::Instance();
	//FairRun* fRun = FairRun::Instance();
	//FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	lmddim = PndLmdDim::Instance();
	lmddim -> Read_transformation_matrices("",true);
	lmddim -> Read_transformation_matrices("",false);
	//lmddim -> Read_transformation_matrices("matrices.txt", true);
	//lmddim -> Read_transformation_matrices("matrices_perfect.txt", false);
	// FairBaseParSet* par=(FairBaseParSet*)
	//   (rtdb->findContainer("FairBaseParSet"));
	// fPbeam = par->GetBeamMom();
	//  cout<<"Beam Momentum for this run is "<<fPbeam<<endl;

	GFFieldManager::getInstance()->init(new GFPandaField());

	std::cout << " Setting up histograms in PndLmdPerformanceTask ";
	flush(std::cout);

	TPad foo; // never remove this line :-)))
	if(1){
		gROOT->SetStyle("Plain");
		const Int_t NRGBs = 5;
		const Int_t NCont = 255;
		Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
		Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
		Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
		Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
		TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
		gStyle->SetNumberContours(NCont);
		gStyle->SetTitleFont(10*13+2,"xyz");
		gStyle->SetTitleSize(0.06, "xyz");
		gStyle->SetTitleOffset(1.3,"y");
		gStyle->SetTitleOffset(1.3,"z");
		gStyle->SetLabelFont(10*13+2,"xyz");
		gStyle->SetLabelSize(0.06,"xyz");
		gStyle->SetLabelOffset(0.009,"xyz");
		gStyle->SetPadBottomMargin(0.16);
		gStyle->SetPadTopMargin(0.16);
		gStyle->SetPadLeftMargin(0.16);
		gStyle->SetPadRightMargin(0.16);
		gStyle->SetOptTitle(1);
		gStyle->SetOptStat(1);
		gROOT->ForceStyle();
		gStyle->SetFrameFillColor(0);
	   	gStyle->SetFrameFillStyle(0);
	   	TGaxis::SetMaxDigits(3);
	}

	if (!hist_output_file) SetHistFilename("_hists.root");
	hist_output_file->cd();

	hist_angular_distr_gen = new TH2F("hist_angular_distr_gen",
			"hist_angular_distr_gen", 400, 2.e-3, 12.e-3, 400, -3.141, 3.141);
	hist_angular_distr_acc = new TH2F("hist_angular_distr_acc",
			"hist_angular_distr_acc", 400, 2.e-3, 12.e-3, 400, -3.141, 3.141);

	// spatial acceptance in x and y at the first lumi plane
	hist_spatial_distr_gen = new TH2F("hist_spatial_distr_gen",
			"hist_spatial_distr_gen", 1000, -100., 100., 1000, -100., 100.);
	hist_spatial_distr_acc = new TH2F("hist_spatial_distr_acc",
			"hist_spatial_distr_acc", 1000, -100., 100., 1000, -100., 100.);

	hist_theta_over_mom_gen = new TH2F("hist_theta_over_mom_gen",
			"hist_theta_over_mom_gen", 2000, 1., 20., 100, 2.e-3, 12.e-3);
	hist_theta_over_mom_acc = new TH2F("hist_theta_over_mom_acc",
			"hist_theta_over_mom_acc", 2000, 1., 20., 100, 2.e-3, 12.e-3);

	hist_phi_over_mom_gen = new TH2F("hist_phi_over_mom_gen",
			"hist_phi_over_mom_gen", 2000, 1., 20., 400, -3.141, 3.141);
	hist_phi_over_mom_acc = new TH2F("hist_phi_over_mom_acc",
			"hist_phi_over_mom_acc", 2000, 1., 20., 400, -3.141, 3.141);

	tree_results = new TTree("tree_results", "tree_results");
	// initial momenta of the antiprotons
	tree_results->Branch("px_init", &px_init);
	tree_results->Branch("py_init", &py_init);
	tree_results->Branch("pz_init", &pz_init);
	tree_results->Branch("ptheta_init", &ptheta_init);
	tree_results->Branch("pphi_init", &pphi_init);
	// momenta at the entry of a detector plane
	// in the reference system of the LUMI
	tree_results->Branch("px_in", &px_in);
	tree_results->Branch("py_in", &py_in);
	tree_results->Branch("pz_in", &pz_in);
	tree_results->Branch("ptheta_in", &ptheta_in);
	tree_results->Branch("pphi_in", &pphi_in);
	// position at the entry of a detector plane
	// in the reference system of the LUMI
	tree_results->Branch("x_in", &x_in);
	tree_results->Branch("y_in", &y_in);
	tree_results->Branch("z_in", &z_in);
	// detector and sensor id of the mc hits
	tree_results->Branch("det_id", &det_id);
	tree_results->Branch("sens_id", &sens_id);
	// calculated plane and sensor in plane id
	tree_results->Branch("plane", &plane);
	tree_results->Branch("sensor", &sensor);
	tree_results->Branch("missed", &missed);
	// new HVMAPS variables
	tree_results->Branch("ihalf", &ihalf);
	tree_results->Branch("iplane", &iplane);
	tree_results->Branch("imodule", &imodule);
	tree_results->Branch("iside", &iside);
	tree_results->Branch("idie", &idie);
	tree_results->Branch("isensor", &isensor);
	tree_results->Branch("x_in_mod", &x_in_mod); // reference frame on the module surface
	tree_results->Branch("y_in_mod", &y_in_mod);
	tree_results->Branch("z_in_mod", &z_in_mod);
	tree_results->Branch("x_in_sens", &x_in_sens); // reference frame on the sensor
	tree_results->Branch("y_in_sens", &y_in_sens);
	tree_results->Branch("z_in_sens", &z_in_sens);
	tree_results->Branch("x_in_sens_al", &x_in_sens_al); // reference frame on the sensor
	tree_results->Branch("y_in_sens_al", &y_in_sens_al);
	tree_results->Branch("z_in_sens_al", &z_in_sens_al);
	tree_results->Branch("x_in_aligned", &x_in_aligned); // reference frame on the module surface
	tree_results->Branch("y_in_aligned", &y_in_aligned); // aligned coordinates in the lumi frame
	tree_results->Branch("z_in_aligned", &z_in_aligned);
	tree_results->Branch("theta_prop", &theta_prop);
	tree_results->Branch("theta_prop_aligned", &theta_prop_aligned);

	// Drawing directly from a tree is elegant but slow as every Draw call
	// loops over the whole tree of events
	// Better it is to create the desired histograms in advance
	// and to fill those on event by event basis simultaneously

	// histograms per plane
	//hist_xy[nplanes];
	//hist_theta_init[nplanes];
	//hist_theta_in[nplanes];
	//hist_theta_diff[nplanes];
	//hist_theta_diff_rel[nplanes];

	// histograms per sensor
	//hists_xy[nplanes][nsensors_per_plane];
	//hists_theta_init[nplanes][nsensors_per_plane];
	//hists_theta_in[nplanes][nsensors_per_plane];
	//hists_theta_diff[nplanes][nsensors_per_plane];
	//hists_theta_diff_rel[nplanes][nsensors_per_plane];

	TCanvas temp_canvas("temp_canvas", "canvas for initialization", 100, 100);
	temp_canvas.cd();
	// loop over planes
	cout << " constructing histograms for " << nplanes << " planes with "
			<< nsensors_per_plane << " sensors per plane " << endl;

	stringstream hist_name;
	stringstream hist_title;
	for (unsigned int _iplane = 0; _iplane < nplanes; _iplane++) {

		hist_name .str("");
		hist_title.str("");

		hist_name << "hist_xy_plane_" << _iplane;
		hist_title << "xy hit distribution plane " << _iplane;
		hist_xy[_iplane] = new TH2F(hist_name.str().c_str(),
				hist_title.str().c_str(), 100, -10, 10, 100, -10, 10);
		hist_xy[_iplane]->Draw();
		hist_xy[_iplane]->GetXaxis()->SetTitle("X [cm]");
		hist_xy[_iplane]->GetYaxis()->SetTitle("Y [cm]");

		hist_name .str("");
		hist_title.str("");

		hist_name << "hist_theta_init_plane_" << _iplane;
		hist_title << "initial #Theta distribution plane " << _iplane;
		hist_theta_init[_iplane] = new TH2F(hist_name.str().c_str(),
				hist_title.str().c_str(), 150, 0, 15e-3, 360, -3.141, +3.141);
		hist_theta_init[_iplane]->Draw();
		hist_theta_init[_iplane]->GetXaxis()->SetTitle("#Theta [rad]");
		hist_theta_init[_iplane]->GetYaxis()->SetTitle("#Phi [rad]");

		hist_name .str("");
		hist_title.str("");

		hist_name << "hist_theta_in_plane_" << _iplane;
		hist_title << "#Theta distribution at plane " << _iplane;
		hist_theta_in[_iplane] = new TH2F(hist_name.str().c_str(),
				hist_title.str().c_str(), 200, 0, 20e-3, 360, -3.141, +3.141);
		hist_theta_in[_iplane]->Draw();
		hist_theta_in[_iplane]->GetXaxis()->SetTitle("#Theta [rad]");
		hist_theta_in[_iplane]->GetYaxis()->SetTitle("#Phi [rad]");

		hist_name .str("");
		hist_title.str("");

		hist_name << "hist_rec_theta_with_plane_" << _iplane;
		hist_title << "#Theta reconstructed distribution with plane " << _iplane;
		hist_theta_rec[_iplane] = new TH2F(hist_name.str().c_str(),
				hist_title.str().c_str(), 200, 0, 20e-3, 360, -3.141, +3.141);
		hist_theta_rec[_iplane]->Draw();
		hist_theta_rec[_iplane]->GetXaxis()->SetTitle("#Theta [rad]");
		hist_theta_rec[_iplane]->GetYaxis()->SetTitle("#Phi [rad]");

		hist_name .str("");
		hist_title.str("");

		hist_name << "hist_theta_diff_in_plane_" << _iplane;
		hist_title << "#Delta#Theta distribution at plane " << _iplane;
		hist_theta_diff[_iplane] = new TH2F(hist_name.str().c_str(),
				hist_title.str().c_str(), 100, -20e-5, 20e-5, 360, -3.141, +3.141);
		hist_theta_diff[_iplane]->Draw();
		hist_theta_diff[_iplane]->GetXaxis()->SetTitle("#Delta#Theta [rad]");
		hist_theta_diff[_iplane]->GetYaxis()->SetTitle("#Phi [rad]");

		hist_name .str("");
		hist_title.str("");

		hist_name << "hist_theta_rec_diff_in_plane_" << _iplane;
		hist_title << "#Delta#Theta reco distribution at plane " << _iplane;
		hist_theta_rec_diff[_iplane] = new TH2F(hist_name.str().c_str(),
				hist_title.str().c_str(), 100, -20e-5, 20e-5, 360, -3.141, +3.141);
		hist_theta_rec_diff[_iplane]->Draw();
		hist_theta_rec_diff[_iplane]->GetXaxis()->SetTitle("#Delta#Theta [rad]");
		hist_theta_rec_diff[_iplane]->GetYaxis()->SetTitle("#Phi [rad]");

		hist_name .str("");
		hist_title.str("");

		hist_name << "hist_theta_diff_rel_in_plane_" << _iplane;
		hist_title << "#Delta#Theta/#Theta distribution at plane " << _iplane;
		hist_theta_diff_rel[_iplane] = new TH2F(hist_name.str().c_str(),
				hist_title.str().c_str(), 100, -1, 1, 360, -3.141, +3.141);
		hist_theta_diff_rel[_iplane]->Draw();
		hist_theta_diff_rel[_iplane]->GetXaxis()->SetTitle("#Delta#Theta/#Theta");
		hist_theta_diff_rel[_iplane]->GetYaxis()->SetTitle("#Phi [rad]");

		hist_name .str("");
		hist_title.str("");

		hist_name << "hist_theta_rec_diff_rel_in_plane_" << _iplane;
		hist_title << "#Delta#Theta/#Theta reco distribution at plane " << _iplane;
		hist_theta_rec_diff_rel[_iplane] = new TH2F(hist_name.str().c_str(),
				hist_title.str().c_str(), 100, -1, 1, 360, -3.141, +3.141);
		hist_theta_rec_diff_rel[_iplane]->Draw();
		hist_theta_rec_diff_rel[_iplane]->GetXaxis()->SetTitle("#Delta#Theta/#Theta");
		hist_theta_rec_diff_rel[_iplane]->GetYaxis()->SetTitle("#Phi [rad]");

		// loop over sensors per plane which are enumerated linearly
		for (unsigned int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
			hist_name .str("");
			hist_title.str("");

			hist_name << "hist_xy_plane_" << _iplane << "_sensor_" << _isensor;
			hist_title << "xy hit distribution plane " << _iplane << " sensor "
					<< _isensor;
			hists_xy[_iplane][_isensor] = new TH2F(hist_name.str().c_str(),
					hist_title.str().c_str(), 100, -10, 10, 100, -10, 10);
			hists_xy[_iplane][_isensor]->Draw();
			hists_xy[_iplane][_isensor]->GetXaxis()->SetTitle("X [cm]");
			hists_xy[_iplane][_isensor]->GetYaxis()->SetTitle("Y [cm]");

			hist_name .str("");
			hist_title.str("");

			hist_name << "hist_xy_local_plane_" << _iplane << "_sensor_" << _isensor;
			hist_title << "xy local hit distribution plane " << _iplane << " sensor "
					<< _isensor;
			hists_xy_local[_iplane][_isensor] = new TH2F(hist_name.str().c_str(),
					hist_title.str().c_str(), 100, -2, 2, 100, -2, 2);
			hists_xy_local[_iplane][_isensor]->Draw();
			hists_xy_local[_iplane][_isensor]->GetXaxis()->SetTitle("X [cm]");
			hists_xy_local[_iplane][_isensor]->GetYaxis()->SetTitle("Y [cm]");

			hist_name << "hist_theta_init_plane_" << _iplane << "_sensor_"
					<< _isensor;
			hist_title << "initial #Theta distribution plane " << _iplane
					<< " sensor " << _isensor;
			hists_theta_init[_iplane][_isensor] = new TH1F(
					hist_name.str().c_str(), hist_title.str().c_str(), 150, 0,
					15e-3);
			hists_theta_init[_iplane][_isensor]->Draw();
			hists_theta_init[_iplane][_isensor]->GetXaxis()->SetTitle(
					"#Theta [rad]");
			hists_theta_init[_iplane][_isensor]->GetYaxis()->SetTitle("entries");

			hist_name .str("");
			hist_title.str("");

			hist_name << "hist_theta_in_plane_" << _iplane << "_sensor_"
					<< _isensor;
			hist_title << "#Theta distribution at plane " << _iplane
					<< " sensor " << _isensor;
			hists_theta_in[_iplane][_isensor] = new TH1F(hist_name.str().c_str(),
					hist_title.str().c_str(), 200, 0, 20e-3);
			hists_theta_in[_iplane][_isensor]->Draw();
			hists_theta_in[_iplane][_isensor]->GetXaxis()->SetTitle(
					"#Theta [rad]");
			hists_theta_in[_iplane][_isensor]->GetYaxis()->SetTitle("entries");

			hist_name .str("");
			hist_title.str("");

			hist_name << "hist_theta_diff_in_plane_" << _iplane << "_sensor_"
					<< _isensor;
			hist_title << "#Delta#Theta distribution at plane " << _iplane
					<< " sensor " << _isensor;
			hists_theta_diff[_iplane][_isensor] = new TH1F(
					hist_name.str().c_str(), hist_title.str().c_str(), 100,
					-20e-5, 20e-5);
			hists_theta_diff[_iplane][_isensor]->Draw();
			hists_theta_diff[_iplane][_isensor]->GetXaxis()->SetTitle(
					"#Delta#Theta [rad]");
			hists_theta_diff[_iplane][_isensor]->GetYaxis()->SetTitle("entries");

			hist_name .str("");
			hist_title.str("");

			hist_name << "hist_theta_diff_rel_in_plane_" << _iplane
					<< "_sensor_" << _isensor;
			hist_title << "#Delta#Theta/#Theta distribution at plane "
					<< _iplane << " sensor " << _isensor;
			hists_theta_diff_rel[_iplane][_isensor] = new TH1F(
					hist_name.str().c_str(), hist_title.str().c_str(), 100, -1,
					1);
			hists_theta_diff_rel[_iplane][_isensor]->Draw();
			hists_theta_diff_rel[_iplane][_isensor]->GetXaxis()->SetTitle(
					"#Delta#Theta/#Theta");
			hists_theta_diff_rel[_iplane][_isensor]->GetYaxis()->SetTitle(
					"entries");
		}
	}

	hist_name .str("");
	hist_title.str("");

	hist_name << "hist_theta_diff_prop_true";
	hist_title << "#Delta#Theta true distribution (Error by Geane back propagation) ";
	hist_theta_diff_prop_true = new TH2F(hist_name.str().c_str(),
			hist_title.str().c_str(), 300, -10e-5, 10e-5, 360, -3.141, +3.141);
	hist_theta_diff_prop_true->Draw();
	hist_theta_diff_prop_true->GetXaxis()->SetTitle("#Delta#Theta [rad]");
	hist_theta_diff_prop_true->GetYaxis()->SetTitle("#Phi [rad]");

	hist_name .str("");
	hist_title.str("");

	hist_name << "hist_theta_diff_prop_true_o_theta";
	hist_title << "#Delta#Theta true distribution (Error by Geane back propagation) ";
	hist_theta_diff_prop_true_o_theta = new TH2F(hist_name.str().c_str(),
			hist_title.str().c_str(), 300, -10e-5, 10e-5, 200, 2.e-3, 10.e-3);
	hist_theta_diff_prop_true_o_theta->Draw();
	hist_theta_diff_prop_true_o_theta->GetXaxis()->SetTitle("#Delta#Theta [rad]");
	hist_theta_diff_prop_true_o_theta->GetYaxis()->SetTitle("#Theta [rad]");


	std::cout << " done " << std::endl;

	// rotation into the lumi position according to the beamline file
	// matrix tri6 - tr=1  rot=1  refl=0  scl=0
	//  0.999192    0.000000    0.040200    Tx =  22.893063
	//  0.000000    1.000000   -0.000000    Ty =   0.000000
	// -0.040200    0.000000    0.999192    Tz = 1049.770390
	// The matrix is a rotation around y of around 0.04025 radian
	// TRotation inv_lmdrotation;
	inv_lmdrotation.RotateY(-0.04025);
	inv_lmdrotation.RotateY(-40.068e-3);
	inv_lmdtranslation.SetXYZ(-26.2461, 0.000000, -1130.);

	fgGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
	if (!fgGeoMan) cout << "Error: could not find the geometry manager!" << endl;

	return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndLmdPerformanceTask::SetParContainers() {
	// Get Base Container
	/// FairRun* ana = FairRun::Instance();
	//s FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}

void PndLmdPerformanceTask::SetHistFilename(TString filename){
	if (hist_output_file){
		cout << " Warning in PndLmdPerformanceTask::SetHistFilename(): output file was set to " << hist_output_file->GetName() << endl;
		cout << "          Setting it to: " << filename << endl;
		hist_output_file->Close();
	}
	hist_output_file = new TFile(filename, "RECREATE");
}

/*
void PndLmdPerformanceTask::Propagate(TVector3 pos, TVector3 mom, int pdgcode,
		TVector3 ip, TVector3& initpos, TVector3& initdir) {
	//TVector3 initpos, TVector3 initdir, TVector3 IP, TVector3& respos, TVector3& resdir, ", &);mom){ // PDGCode
	if (!fPro) {
		cout << " no propagator found, creating one and initilizing the rest "
				<< endl;
		Init();
		//fPro = new FairGeanePro();
		//fGeoH = PndGeoHandling::Instance();
		//FairRun* fRun = FairRun::Instance();
		//FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	}
	fPbeam = initdir.Mag();
	// if(fPDGid!=-2212){ //calculate momentum if particle is not antiproton
	//     }
	//cout<<"Beam Momentum for particle with PDGid#"<<fPDGid<<" this run is "<<fPbeam<<endl;
	vtx = ip;
	//cout<<"Interaction Point:"<<endl;
	//vtx.Print();
	TVector3 StartPosErr, StartMomErr, ResPosErr, ResMomErr;
	int charge = pdgcode > 0 ? 1 : pdgcode < 0 ? -1 : 0;
	FairTrackParH *fStart = new FairTrackParH(pos, mom, StartPosErr,
			StartMomErr, charge); // exchange fCharge by charge of PDGCode
	//TClonesArray& clref = *fTrackParGeane;
	//Int_t size = clref.GetEntriesFast();
	FairTrackParH *fRes = new FairTrackParH(initpos, initdir, ResPosErr,
			ResMomErr, charge);
	fPro->SetPoint(ip);
	//  fPro->BackTrackToVertex();
	Bool_t isProp = fPro->Propagate(fStart, fRes, pdgcode);
	delete fStart;
	delete fRes;

	if (!isProp)
		cout
				<< "PndLmdPerformanceTask::Propagate(): back propagation not possible! "
				<< endl;

	//TVector3 gPos(fRes->GetX(),fRes->GetY(),fRes->GetZ());
	//TVector3 gMom(fRes->GetPx(),fRes->GetPy(),fRes->GetPz());
	//TVector3 gErrPos(fRes->GetDX(),fRes->GetDY(),fRes->GetDZ());
	//TVector3 gErrMom(fRes->GetDPx(),fRes->GetDPy(),fRes->GetDPz());

	//initpos = fRes->GetPosition();
	//initdir = fRes->GetMomentum();
}*/

// -----   Public method Exec   --------------------------------------------
void PndLmdPerformanceTask::Exec(Option_t* opt) {

	bool verbose(false);

	fTrackPlaneHitMap.clear();
	fTrackMCVectMapIn.clear();
	fTrackMCVectMapInit.clear();

	fievent++;
	if (fnevents == -1) fnevents = 1000000;

	DrawProgressBar(50, (fievent) / ((double) fnevents));

	const int nParticles = true_tracks->GetEntriesFast();

	int npbar = 0;
	for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
		// if(nParticles!=4) continue;
		PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(iParticle);
		//Int_t mcID = mctrk->GetPdgCode();
		if (mctrk->IsGeneratorCreated()) {//mcID == -2212) {
			npbar++;
			//  if(nParticles!=4) cout<<"For event #"<<j<<" mcID="<<mcID<<endl;
			TVector3 MomMC = mctrk->GetMomentum();
			hist_angular_distr_gen->Fill(MomMC.Theta(), MomMC.Phi());
			hist_theta_over_mom_gen->Fill(MomMC.Mag(), MomMC.Theta());
			hist_phi_over_mom_gen->Fill(MomMC.Mag(), MomMC.Phi());
			//hallTheta->Fill(MomMC.Theta());
			//hallPhi->Fill(MomMC.Phi());
			fTrackMCVectMapInit.push_back(MomMC);
		}
	}
	if (verbose && npbar != 1)
		cout << "found " << npbar << " anti-protons" << endl;

	int nHits = true_points->GetEntriesFast();
	int nSdsHits = 0;
	//cout << " hits " << nHits << endl;
	for (Int_t iHit = 0; iHit < nHits; iHit++) {
		PndSdsMCPoint* mcpoint = (PndSdsMCPoint*) true_points->At(iHit);
		PndMCTrack *mctrk =
				(PndMCTrack*) true_tracks->At(mcpoint->GetTrackID());
		if (mctrk->IsGeneratorCreated()) { // take only anti-protons from the generator
			if (mcpoint) {
				TVector3 _mcpoint((mcpoint->GetPosition())); // position at the sensor entrance
				//cout << " momentum " << mctrk->GetMomentum().Mag();
				//cout << " momentum difference " << (mctrk->GetMomentum().Mag() - fTrackMCVectMapInit[0].Mag())/fTrackMCVectMapInit[0].Mag();
				//cout << " Energy loss " << mcpoint->GetEnergyLoss() << endl;
				TVector3 _mctrack(mcpoint->GetPx(), mcpoint->GetPy(),
						mcpoint->GetPz()); // momentum of the track at the entrance
				// variables used for conversion via root geometries
				//double glob_pt[3] = {mcpoint->GetX(), mcpoint->GetY(), mcpoint->GetZ() + 1.e-4};
				//double loc_pt[3] = {0,0,0};
				if (0) { // perform some geane tests
					if (mcpoint->GetSensorID() > 7) return;
					TVector3 StartPos, StartPosErr, StartMom, StartMomErr, StartO, StartU, StartV;
					//TVector3 DirVec =  _momrec.Unit();
					//cout << " here " << ihere++ << endl;
					StartMom = _mctrack;//.Unit()*1.5*(1.-1.e-3);//_momrec.Unit()*itmominit.Mag();
					TVector3 move_upstream_vect(0,0,-0.1);
					StartPos = _mcpoint + move_upstream_vect ;
					StartPosErr = TVector3(50.e-4, 50.e-4, 50.e-4);// TVector3(50.e-4, 50.e-4, 50.e-4); // 50 mu like pitch size
					StartMomErr = _mctrack.Mag()*1e-3;//_momrec.Unit()*fPbeam*1e-3; //_momrec.Unit()*fPbeam*1e-3; // some estimate
					//cout << " start mom     " << StartMom.X() << '\t' << StartMom.Y() << '\t' << StartMom.Z() << '\t' << endl;
					//cout << " start mom err " << StartMomErr.X() << '\t' << StartMomErr.Y() << '\t' << StartMomErr.Z() << '\t' << endl;
					//cout << " start pos     " << StartPos.X() << '\t' << StartPos.Y() << '\t' << StartPos.Z() << '\t' << endl;
					//cout << " start pos err " << StartPosErr.X() << '\t' << StartPosErr.Y() << '\t' << StartPosErr.Z() << '\t' << endl;
					int PDGCode = -2212;
					TVector3 _vtx(0.,0.,0.); // point to find the closest approach to
					TVector3 gPos(0.,0.,0.); // point of closest approach
					TVector3 gMom(0.,0.,0.); // momentum at that point
					if (0) {// geane methods implemented by turany
						//if (fievent > 100000) return; // debugging
						//if (_mctrack.Phi() < 0.09 || _mctrack.Phi() > 0.1) return;
						//cout << " here " << ihere++ << endl;
						FairTrackParH *fStart = new FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, -1);
						FairTrackParH *fRes = new FairTrackParH();
						//cout << " here " << ihere++ << endl;
						fPro->SetPoint(_vtx);
						fPro->PropagateToPCA(1,-1);
						//  fPro->BackTrackToVertex();
						Bool_t isProp = fPro->Propagate(fStart, fRes, PDGCode);
						if (!isProp){
							cout << " Warning in PndLmdPerformanceTask::Exec(): propagation failed! " << endl;
						}
						gPos.SetXYZ(fRes->GetX(),fRes->GetY(),fRes->GetZ());
						gMom.SetXYZ(fRes->GetPx(),fRes->GetPy(),fRes->GetPz());
						//_momrec = gMom;
						delete fStart;
						delete fRes;
					} else { // genfit Runge Kutta Nystroem algorithm
						RKTrackRep* trackrep = new RKTrackRep(StartPos, StartMom, StartPosErr, StartMomErr, PDGCode);
						trackrep->extrapolateToPoint(_vtx, gPos, gMom);
					}
					if (fTrackMCVectMapInit.size() > 0){
						double theta_lmd_true = gMom.Theta();
						double theta_true = fTrackMCVectMapInit[0].Theta(); // reference momentum direction
						TVector3 _mcpoint_rot = inv_lmdtranslation + _mcpoint;
						_mcpoint_rot = inv_lmdrotation * _mcpoint_rot;
						//if (-2.4 < _mcpoint_rot.Phi() && _mcpoint_rot.Phi() < -2.3){
							hist_theta_diff_prop_true->Fill(theta_lmd_true - theta_true, fTrackMCVectMapInit[0].Phi());// _mcpoint_rot.Phi());// fTrackMCVectMapInit[0].Phi());
							hist_theta_diff_prop_true_o_theta->Fill(theta_lmd_true - theta_true, theta_true);
						//}
						if (fTrackMCVectMapInit.size() > 1 && verbose){
							cout << " Warning in PndLmdPerformanceTask::Exec(): more than 1 IP MC anti proton found -> using first found" << endl;
						}
					}
					return;
				}
				if (1) {
					//const double* current_point = fgGeoMan->GetCurrentPoint();
					//cout << current_point[0] << " " << current_point[1] << " " << current_point[2] << endl;
					TGeoNode* node =
							fgGeoMan->FindNode(mcpoint->GetX(), mcpoint->GetY(), mcpoint->GetZ() + 1.e-4);
					if (node){
						missed = false;
						//std::cout << node->GetName() << std::endl;
						//std::cout << node->GetIndex() << std::endl;
						//node->GetMatrix()->Print();
						//std::cout <<  << std::endl;
						//std::cout << node->GetMotherVolume()->GetName() << std::endl;
						//std::cout << node->GetNumber() << "\n" << std::endl;
						TGeoVolume* local_top = fgGeoMan->GetVolume("lmd_vol_ref_sys");
						if (! local_top ){
							std::cout << " Error: Could not set top volume to lmd_vol_ref_sys" << std::endl;
						} else {
							//fgGeoMan->SetTopVolume(local_top);
							//cout << fgGeoMan->GetPath() << endl;
							//fgGeoMan->MasterToLocal(glob_pt, loc_pt);

						}
						//std::cout << glob_pt[0] << " " << glob_pt[1] << " " << glob_pt[2] << " " << std::endl;
						//std::cout << loc_pt[0] << " " << loc_pt[1] << " " << loc_pt[2] << " " << std::endl;
					} else {
						missed = true;
						std::cout << " Error: no node found for the current MC point " << std::endl;
						//mcpoint->Print();
					}
					/*
					double new_point[3] = {8.7, 1., 29.7375};
					fgGeoMan->SetCurrentPoint(new_point);
					const double* new_current_point = fgGeoMan->GetCurrentPoint();
					cout << new_current_point[0] << " " << new_current_point[1] << " " << new_current_point[2] << endl;
					cout << fgGeoMan->GetCurrentNode()->GetName() << endl;
					gGeoMan->FindNode();
					cout << fgGeoMan->GetCurrentNode()->GetName() << endl;

					string path = "/lmd_HV_MAPS_1/vol_lmd_vac_0/Lumi_HV-MAPS_0/vol_LumPassive_cvd_disc_plane_1_disc_1_0";
					fgGeoMan->cd(path.c_str());///LumActivePixelRect_plane_0_disc_0_side_0_col_0_row_0_0");
					cout << fgGeoMan->GetPath()<< endl;
					TGeoPhysicalNode* node =
							fgGeoMan->MakePhysicalNode((path + "/LumActivePixelRect_plane_0_disc_0_side_0_col_0_row_0_0").c_str());///Lumi_HV-MAPS/LumPassive_cvd_disc_plane_2_disc_3");
					if (node){
						//TGeoCombiTrans* align_trans = new TGeoCombiTrans(1.,0.,0.,georot_no);
						//node->Align(align_trans);
						//gGeoMan->CloseGeometry();
						TGeoHMatrix* matrix = node -> GetMatrix();
						matrix -> Print();
					}*/
				}

				if (0) { // translate it into the reference system of the lumi monitor
					//_mcpoint = inv_lmdtranslation + _mcpoint;
					//_mcpoint = inv_lmdrotation * _mcpoint;
				}
				if (1) {
					_mcpoint = lmddim->Transform_global_to_lmd_local(_mcpoint, false, false);
				}
				TVector3 momMC = mctrk->GetMomentum(); // momentum in the primary vertex
				TVector3 posMC = mctrk->GetStartVertex(); // position of the primary vertex
				if (1) { // use back propagation for the track
					//TVector3 initpos, initmom(0, 0, momMC.Mag()), StartPosErr,
					//		StartMomErr, ResPosErr, ResMomErr;
					//int charge = mctrk->GetPdgCode() > 0 ? 1
					//		: mctrk->GetPdgCode() < 0 ? -1 : 0;
					//FairTrackParH *fStart = new FairTrackParH(posMC, momMC,
					//		StartPosErr, StartMomErr, charge); // exchange fCharge by charge of PDGCode
					//FairTrackParH *fRes = new FairTrackParH(initpos, initmom,
					//		ResPosErr, ResMomErr, charge);
					//FairTrackParH *fRes = new FairTrackParH();
					//fPro->SetPoint(vtx);
					//fPro->PropagateToPCA(1, -1);
					//Bool_t isProp = fPro->Propagate(fStart, fRes,
					//		mctrk->GetPdgCode());
					//if (!isProp){
						//cout << "Propagate(): back propagation not possible! "
						//		<< endl;
					//}
					//else {
						//cout << "Propagation worked! " << endl;
					//}
					//_mctrack = fRes->GetMomentum();//
					//_mctrack = inv_lmdrotation*_mctrack;
					//lmddim->Transform_global_to_lmd_local(_mctrack, true);
					//delete fStart;
					//delete fRes;
				}
				det_id = mcpoint->GetDetectorID(); // store the detector id
				sens_id = mcpoint->GetSensorID(); // store the sensor id
				// calculate the plane and sensor on this plane
				lmddim->Get_sensor_by_id(sens_id, ihalf, iplane, imodule, iside, idie, isensor);
				sensor = isensor;
				plane = iplane;
				if (idie == 1) sensor += 2;
				sensor += lmddim->n_sensors * iside;
				sensor += lmddim->n_sensors * 2 * imodule;
				sensor += lmddim->n_sensors * 2 * lmddim->nmodules * ihalf; // I know, I know these lines are not optimized ;)
				//plane = sens_id / nsensors_per_plane;
				//sensor = sens_id - plane * nsensors_per_plane;
				TVector3 _mctrack_aligned = lmddim->Transform_global_to_sensor(_mctrack, ihalf, iplane, imodule, iside, idie, isensor, true, true);
				_mctrack = lmddim->Transform_global_to_sensor(_mctrack, ihalf, iplane, imodule, iside, idie, isensor, true, false);

				// write those values out
				px_init = momMC.X();
				py_init = momMC.Y();
				pz_init = momMC.Z();
				ptheta_init = momMC.Theta();
				pphi_init = momMC.Phi();
				x_in = _mcpoint.X();
				y_in = _mcpoint.Y();
				z_in = _mcpoint.Z();
				px_in = _mctrack.X();
				py_in = _mctrack.Y();
				pz_in = _mctrack.Z();
				ptheta_in = _mctrack.Theta();
				pphi_in = _mctrack.Phi();

				const TVector3& _mcpoint_mod =
						lmddim->Transform_lmd_local_to_module_side(_mcpoint,
								ihalf, iplane, imodule, iside, false, false);
				x_in_mod = _mcpoint_mod.X();
				y_in_mod = _mcpoint_mod.Y();
				z_in_mod = _mcpoint_mod.Z();
				const TVector3& _mcpoint_sens =
						lmddim->Transform_global_to_sensor(mcpoint->GetPosition(),
								ihalf, iplane, imodule, iside, idie, isensor, false, false);
				x_in_sens = _mcpoint_sens.X(); // reference frame on the sensor
				y_in_sens = _mcpoint_sens.Y();
				z_in_sens = _mcpoint_sens.Z();
				const TVector3& _mcpoint_sens_al =
						lmddim->Transform_global_to_sensor(mcpoint->GetPosition(),
								ihalf, iplane, imodule, iside, idie, isensor, false, true);
				x_in_sens_al = _mcpoint_sens_al.X(); // reference frame on the sensor
				y_in_sens_al = _mcpoint_sens_al.Y();
				z_in_sens_al = _mcpoint_sens_al.Z();
				const TVector3& _mcpoint_aligned =
						lmddim->Transform_global_to_lmd_local(mcpoint->GetPosition(), false, true);
				x_in_aligned = _mcpoint_aligned.X(); // reference frame on the module surface
				y_in_aligned = _mcpoint_aligned.Y(); // aligned coordinates in the lumi frame
				z_in_aligned = _mcpoint_aligned.Z();

				theta_prop = _mctrack.Theta();
				theta_prop_aligned = _mctrack_aligned.Theta();

				if (plane < 0 || plane >= nplanes || sensor < 0 || sensor
						>= nsensors_per_plane) {
					cout
							<< " warning: calculated plane or sensor is wrong: plane "
							<< plane << " sensor " << sensor << " sensorID " << sens_id << endl;
				} else {
					// we need the unrotated vectors, too
					TVector3 mcpoint_org((mcpoint->GetPosition())); // position at the sensor entrance
					TVector3 mctrack_org(mcpoint->GetPx(), mcpoint->GetPy(),
											mcpoint->GetPz()); // momentum of the track at the entrance
					fTrackPlaneHitMap[plane].push_back(mcpoint_org);
					fTrackMCVectMapIn[plane].push_back(mctrack_org);

					hist_xy[plane] ->Fill(x_in, y_in);
					hists_xy[plane][sensor]->Fill(x_in, y_in);
					hists_xy_local[plane][sensor]->Fill(_mcpoint_sens.X(), _mcpoint_sens.Y());

					hist_theta_init[plane] ->Fill(ptheta_init, pphi_init);
					hists_theta_init[plane][sensor]->Fill(ptheta_init);

					hist_theta_in[plane] ->Fill(ptheta_in, pphi_in);
					hists_theta_in[plane][sensor]->Fill(ptheta_in);

					hist_theta_diff[plane] ->Fill(ptheta_in - ptheta_init, pphi_init);
					hists_theta_diff[plane][sensor]->Fill(
							ptheta_in - ptheta_init, pphi_init);

					if (ptheta_init > 0) {
						hist_theta_diff_rel[plane] ->Fill(
								(ptheta_in - ptheta_init) / ptheta_init, pphi_init);
						hists_theta_diff_rel[plane][sensor]->Fill(
								(ptheta_in - ptheta_init) / ptheta_init, pphi_init);
					}

					tree_results->Fill();
				}

				//cout << _mcpoint.X() << " " << _mcpoint.Y() << " "<< _mcpoint.Z() << endl;
				hist_spatial_distr_acc->Fill(_mcpoint.X(), _mcpoint.Y());
				//hist_angular_distr_acc->Fill(momMC.Theta(), momMC.Phi()); // moved to reconstruction point
				//hist_theta_over_mom_acc->Fill(momMC.Mag(), momMC.Theta());
				//hist_phi_over_mom_acc->Fill(momMC.Mag(), momMC.Phi());
				nSdsHits++;
			}
		}
	}
	if (verbose && nHits > 0)
		cout << "found " << nHits << " hit(s) with " << nSdsHits
				<< " sds hit(s)" << endl;

	// perform a simple tracking using hits between 2 planes without digitization
	// to see only effects of smearing
	std::map<int, std::vector<TVector3> >::iterator itplane1, itplane2, itmomin;
	// find a hit in the first plane (simplified tracking here needs the first plane)
	itplane1 = fTrackPlaneHitMap.find(0);
	// as well as the momentum in the first plane
	itmomin = fTrackMCVectMapIn.find(0);
	bool accepted(false);
	if (itplane1 != fTrackPlaneHitMap.end() && itmomin != fTrackMCVectMapIn.end()){
		if (fTrackMCVectMapInit.size() > 0){
			TVector3 itmominit = fTrackMCVectMapInit[0]; // reference momentum direction
			if (fTrackMCVectMapInit.size() > 1 && verbose){
				cout << " Warning in PndLmdPerformanceTask::Exec(): more than 1 IP MC anti proton found -> using first found" << endl;
			}

			if ((*itmomin).second.size() > 0){
				TVector3 _itmomin = (*itmomin).second[0]; // reference momentum direction at plane 0
				if ((*itmomin).second.size() > 1 && verbose){
					cout << " Warning in PndLmdPerformanceTask::Exec(): more than 1 IP MC anti proton momentum in the first plane found -> using first found" << endl;
				}
				// check the first found point
				if ((*itplane1).second.size() > 0){
					TVector3 point1 = (*itplane1).second[0];
					if ((*itplane1).second.size() > 1 && verbose){
						cout << " Warning in PndLmdPerformanceTask::Exec(): more than 1 IP MC anti proton hit in the plane 0 found -> using first found" << endl;
					}
					// find partners in other planes
					for (int _iplane = 1; _iplane < nplanes; _iplane++){
						itplane2 = fTrackPlaneHitMap.find(_iplane);
						if (itplane2 != fTrackPlaneHitMap.end()){
							// reconstruct the vector between the first plane and that one
							if ((*itplane2).second.size() > 0){
								TVector3 point2 = (*itplane2).second[0];
								if ((*itplane2).second.size() > 1 && verbose){
									cout << " Warning in PndLmdPerformanceTask::Exec(): more than 1 IP MC anti proton hit in the plane "<< _iplane <<" found -> using first found" << endl;
								}
								TVector3 _momrec = point2 - point1;
								//int ihere(0);
								//cout << " here " << ihere++ << endl;
								// back propagation
								if (1){
									//cout << " here " << ihere++ << endl;
									TVector3 StartPos, StartPosErr, StartMom, StartMomErr, StartO, StartU, StartV;
									//TVector3 DirVec =  _momrec.Unit();
									//cout << " here " << ihere++ << endl;
									StartMom = _momrec.Unit()*itmominit.Mag();
									StartPos = point1;
									StartPosErr = TVector3(50.e-4, 50.e-4, 50.e-4);// TVector3(50.e-4, 50.e-4, 50.e-4); // 50 mu like pitch size
									StartMomErr = _momrec.Unit()*fPbeam*1e-3; //_momrec.Unit()*fPbeam*1e-3; // some estimate
									TVector3 _vtx(0.,0.,0.); //
									int PDGCode = -2212;
									//cout << " start mom     " << StartMom.X() << '\t' << StartMom.Y() << '\t' << StartMom.Z() << '\t' << endl;
									//cout << " start mom err " << StartMomErr.X() << '\t' << StartMomErr.Y() << '\t' << StartMomErr.Z() << '\t' << endl;
									//cout << " start pos     " << StartPos.X() << '\t' << StartPos.Y() << '\t' << StartPos.Z() << '\t' << endl;
									//cout << " start pos err " << StartPosErr.X() << '\t' << StartPosErr.Y() << '\t' << StartPosErr.Z() << '\t' << endl;
									FairTrackParH *fStart = new FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, -1);
									FairTrackParH *fRes = new FairTrackParH();
									//cout << " here " << ihere++ << endl;
									TVector3 gPos(0.,0.,0.);
									TVector3 gMom(0.,0.,0.);
									if (0){ // use geane back propagation else ...
										fPro->SetPoint(_vtx);
										fPro->PropagateToPCA(1,-1);
										//  fPro->BackTrackToVertex();
										Bool_t isProp = fPro->Propagate(fStart, fRes, PDGCode);
										if (!isProp){
											cout << " Warning in PndLmdPerformanceTask::Exec(): propagation failed! " << endl;
										}
										gPos.SetXYZ(fRes->GetX(),fRes->GetY(),fRes->GetZ());
										gMom.SetXYZ(fRes->GetPx(),fRes->GetPy(),fRes->GetPz());

										delete fStart;
										delete fRes;
									} else { // ... use Runge-Kutta algorithm

										RKTrackRep* trackrep = new RKTrackRep(StartPos, StartMom, StartPosErr, StartMomErr, PDGCode);
										trackrep->extrapolateToPoint(_vtx, gPos, gMom);

									}

									_momrec = gMom;
								}
								hist_theta_rec[_iplane]->Fill(_momrec.Theta(), _momrec.Phi());
								hist_theta_rec_diff[_iplane]->Fill(_momrec.Theta() - itmominit.Theta(), itmominit.Phi());
								hist_theta_rec_diff_rel[_iplane]->Fill((_momrec.Theta() - itmominit.Theta())/itmominit.Theta(), itmominit.Phi());
								if (!accepted){ // fill the following histograms only once per event
									hist_angular_distr_acc->Fill(itmominit.Theta(), itmominit.Phi());
									hist_theta_over_mom_acc->Fill(itmominit.Mag(), itmominit.Theta());
									hist_phi_over_mom_acc->Fill(itmominit.Mag(), itmominit.Phi());
									accepted = true;
								}
							}
						}
					}
				}
			}
		}
	}

	/*

	 // cout<<"PndLmdPerformanceTask::Exec starts!"<<endl;
	 std::map<int, std::vector<int> > mcHitMap;//Track ->  MCHits
	 fTrackParGeane->Delete();
	 fTrackParIni->Delete();
	 fTrackParFinal->Delete();
	 fDetName->Delete();

	 mcHitMap = AssignHitsToTracks();
	 fEventNr++;

	 //Charge & mass of particle
	 Int_t PDGCode = -2212; //antiproton
	 //  Int_t PDGCode = fPDGid;
	 TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
	 TParticlePDG *fParticle = fdbPDG->GetParticle(PDGCode);
	 Double_t fCharge = fParticle->Charge();

	 //go through all tracks
	 int glI = fTracks->GetEntriesFast();
	 // cout<<"glI = "<<glI<<endl;
	 Int_t counterGeaneTrk = 0;
	 for (Int_t i = 0; i < glI; i++) {
	 //  cout<<"PndLmdPerformanceTask::Exec for track#"<<i<<endl;
	 TVector3 StartPos, StartPosErr, StartMom, StartMomErr, StartO, StartU,
	 StartV;
	 int p = 0;

	 ///Get parameters of real track
	 PndLinTrack* recTrack = (PndLinTrack*) (fTracks->At(i));
	 //   cout<<"recTrack="<<recTrack<<endl;
	 TString DecName = recTrack->GetDetName();
	 if (DecName != "Lumi")
	 continue;
	 // cout<<"DecName: "<<DecName<<endl;
	 //Vector of particle momentum and starting point
	 TVector3 DirVec = recTrack->GetDirectionVec();
	 StartMom = TVector3(DirVec.X() * fPbeam, DirVec.Y() * fPbeam,
	 DirVec.Z() * fPbeam);
	 StartPos = recTrack->GetStartVec();
	 StartPosErr = recTrack->GetStartErrVec();
	 StartMomErr = (recTrack->GetDirectionErrVec()) * fPbeam;
	 if (fVerbose > 2) {
	 cout << "------------------------------------------" << endl;
	 cout << "StartPos:" << endl;
	 StartPos.Print();
	 cout << "StartPosErr:" << endl;
	 StartPosErr.Print();
	 cout << "" << endl;
	 cout << "StartMom: " << StartMom.Mag() << endl;
	 StartMom.Print();
	 cout << "StartMomErr: " << StartMomErr.Mag() << endl;
	 StartMomErr.Print();
	 }
	 TClonesArray& clref1 = *fTrackParIni;
	 Int_t size1 = clref1.GetEntriesFast();

	 ///Propagate to the PCA to a space point---------------------------------
	 FairTrackParH *fStart = new (clref1[size1]) FairTrackParH(StartPos,
	 StartMom, StartPosErr, StartMomErr, fCharge);
	 TClonesArray& clref = *fTrackParGeane;
	 Int_t size = clref.GetEntriesFast();
	 FairTrackParH *fRes = new (clref[size]) FairTrackParH();
	 fPro->SetPoint(vtx);
	 fPro->PropagateToPCA(1, -1);
	 //  fPro->BackTrackToVertex();
	 Bool_t isProp = fPro->Propagate(fStart, fRes, PDGCode);
	 ///----------------------------------------------------------------------

	 // ///Forwars propagate to the 1st plane to a space point---------------------------------
	 // PndMCTrack* mctrk = (PndMCTrack*)(fMCTracks->At(i));
	 // TVector3 MomMC = mctrk->GetMomentum();
	 // TVector3 PosMC = mctrk->GetStartVertex();
	 // FairTrackParH *fStart =
	 // 	new (clref1[size1]) FairTrackParH(PosMC, MomMC, StartPosErr, StartMomErr, fCharge);
	 // TClonesArray& clref = *fTrackParGeane;
	 // Int_t size = clref.GetEntriesFast();
	 // FairTrackParH *fRes = new(clref[size]) FairTrackParH();
	 // fPro->SetPoint(StartPos);
	 // //fPro->PropagateToPCA(1,-1);
	 // fPro->PropagateToPCA(1,1);
	 // Bool_t isProp = fPro->Propagate(fStart, fRes, PDGCode);
	 // ///----------------------------------------------------------------------


	 // ///Propagate to virtual plane at PCA ------------------------------------
	 // TVector3 oc = (0,0,0);
	 // TVector3 dj(0,1,0);
	 // TVector3 dk(-1,0,0);
	 // FairTrackParP *fStart =
	 // 	new (clref1[size1]) FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, oc, dj, dk);
	 // TClonesArray& clref = *fTrackParGeane;
	 // Int_t size = clref.GetEntriesFast();
	 // FairTrackParP *fRes = new(clref[size]) FairTrackParP();
	 // fPro->SetPoint(vtx);
	 // fPro->BackTrackToVirtualPlaneAtPCA(1);
	 // Bool_t isProp =	fPro->Propagate(fStart, fRes, PDGCode);
	 // cout<<"================= %%%% ===================="<<endl;
	 // ///----------------------------------------------------------------------


	 //------- TEST of calculation errors -------
	 TVector3 gPos(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	 TVector3 gMom(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());
	 TVector3 gErrPos(fRes->GetDX(), fRes->GetDY(), fRes->GetDZ());
	 TVector3 gErrMom(fRes->GetDPx(), fRes->GetDPy(), fRes->GetDPz());
	 // cout<<" "<<endl;
	 if (fVerbose > 2) {
	 //      cout<<"================= %%%% ===================="<<endl;

	 cout << "gPos:" << endl;
	 gPos.Print();
	 // cout<<"difference between final and initial point = "<<(-StartPos.Mag()+gPos.Mag())<<endl;
	 cout << "gErrPos:" << endl;
	 gErrPos.Print();
	 //  cout<<"difference between final and initial point error = "<<(-StartPosErr.Mag()+gErrPos.Mag())<<endl;

	 cout << "gMom: " << gMom.Mag() << endl;
	 gMom.Print();
	 // cout<<"difference between initial and final momentum = "<<(StartMom.Mag()-gMom.Mag())<<endl;
	 cout << "gErrMom: " << gErrMom.Mag() << endl;
	 // cout<<"difference between initial and final momentum error= "<<(StartMomErr.Mag()-gErrMom.Mag())<<endl;
	 gErrMom.Print();
	 cout << "================= %%%% ====================" << endl;
	 }
	 //------------------------------------------

	 if (isProp == kTRUE) {
	 new ((*fTrackParFinal)[counterGeaneTrk]) FairTrackParH(*(fRes)); //save Track
	 //	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParP(*(fRes)); //save Track
	 counterGeaneTrk++;
	 cout << "***** isProp TRUE *****" << endl;
	 } else {
	 cout << "!!! Back-propagation with GEANE didn't return result !!!"
	 << endl;
	 cout << "StartPos:" << endl;
	 StartPos.Print();
	 cout << "StartPosErr:" << endl;
	 StartPosErr.Print();

	 cout << "StartMom: " << StartMom.Mag() << endl;
	 StartMom.Print();
	 cout << "StartMomErr: " << StartMomErr.Mag() << endl;
	 StartMomErr.Print();
	 new ((*fTrackParFinal)[counterGeaneTrk]) FairTrackParH(); //save NULL
	 //	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParP(); //save NULL
	 counterGeaneTrk++;
	 }
	 }
	 // fMCTracks->Delete();
	 fMCHits->Delete();
	 cout << "PndLmdPerformanceTask::Exec END!" << endl;*/
}

#include<TColor.h>
#include<TStyle.h>

void PndLmdPerformanceTask::Finish() {
	// is there a file to store the histograms to?
	if (hist_output_file)
		hist_output_file->cd();

	//gStyle->SetPaperSize(50,50);


	hist_angular_distr_acc->Divide(hist_angular_distr_gen);

	TGaxis::SetMaxDigits(3);
	TCanvas* canvas1 = new TCanvas("canvas1", "canvas1", 800, 800);
	canvas1->Divide(2, 2);
	canvas1->cd(1);
	hist_angular_distr_gen->Draw("COLZ");
	hist_angular_distr_gen->SetXTitle("#Theta [rad]");
	hist_angular_distr_gen->SetYTitle("#Phi [rad]");
	gPad->Update();
	canvas1->cd(2);
	hist_angular_distr_acc->Draw("COLZ");
	hist_angular_distr_acc->SetXTitle("#Theta [rad]");
	hist_angular_distr_acc->SetYTitle("#Phi [rad]");
	gPad->Update();
	canvas1->cd(3);
	hist_spatial_distr_gen->Draw("COLZ");
	hist_spatial_distr_gen->SetXTitle("X [cm]");
	hist_spatial_distr_gen->SetYTitle("Y [cm]");
	hist_spatial_distr_gen->GetXaxis()->SetRangeUser(-10, 10);
	hist_spatial_distr_gen->GetYaxis()->SetRangeUser(-10, 10);
	gPad->Update();
	canvas1->cd(4);
	hist_spatial_distr_acc->Draw("COLZ");
	hist_spatial_distr_acc->SetXTitle("X [cm]");
	hist_spatial_distr_acc->SetYTitle("Y [cm]");
	hist_spatial_distr_acc->GetXaxis()->SetRangeUser(-10, 10);
	hist_spatial_distr_acc->GetYaxis()->SetRangeUser(-10, 10);
	gPad->Update();
	canvas1->Print("Resolution_acceptance_results.ps(");

	hist_theta_over_mom_acc->Divide(hist_theta_over_mom_gen);
	hist_phi_over_mom_acc->Divide(hist_phi_over_mom_gen);

	// find the interesting momentum range
	int xbin_low(-1), xbin_high(-1);
	for (int xbin = 1; xbin <=  hist_theta_over_mom_gen->GetNbinsX(); xbin++){
		int nentries(0);
		for (int ybin = 1; ybin <=  hist_theta_over_mom_gen->GetNbinsY(); ybin++){
			nentries+=hist_angular_distr_gen->GetBinContent(xbin, ybin);
		}
		if (nentries > 0){
			if (xbin_low == -1){
				xbin_low = xbin;
			}
		} else {
			if (xbin_low > -1){
				xbin_high = xbin;
				break;
			}
		}
	}
	if (0 && xbin_low > -1 && xbin_high > -1){
		hist_theta_over_mom_gen->GetXaxis()->SetRange(xbin_low, xbin_high);
		hist_theta_over_mom_acc->GetXaxis()->SetRange(xbin_low, xbin_high);
		hist_phi_over_mom_gen->GetXaxis()->SetRange(xbin_low, xbin_high);
		hist_phi_over_mom_acc->GetXaxis()->SetRange(xbin_low, xbin_high);
	}

	canvas1->cd(1);
	hist_theta_over_mom_gen->Draw("COLZ");
	hist_theta_over_mom_gen->SetXTitle("p [GeV]");
	hist_theta_over_mom_gen->SetYTitle("#Theta [rad]");
	gPad->Update();
	canvas1->cd(2);
	hist_theta_over_mom_acc->Draw("COLZ");
	hist_theta_over_mom_acc->SetXTitle("p [GeV]");
	hist_theta_over_mom_acc->SetYTitle("#Theta [rad]");
	gPad->Update();
	canvas1->cd(3);
	hist_phi_over_mom_gen->Draw("COLZ");
	hist_phi_over_mom_gen->SetXTitle("p [GeV]");
	hist_phi_over_mom_gen->SetYTitle("#Phi [rad]");
	gPad->Update();
	canvas1->cd(4);
	hist_phi_over_mom_acc->Draw("COLZ");
	hist_phi_over_mom_acc->SetXTitle("p [GeV]");
	hist_phi_over_mom_acc->SetYTitle("#Phi [rad]");
	gPad->Update();

	canvas1->Print("Resolution_acceptance_results.ps(");

	// draw individual plane and sensor properties
	//gStyle->SetPaperSize(25,25);
	TCanvas canvas_properties_per_plane("canvas_properties_per_plane",
			"properties per plane", 800, 800);
	canvas_properties_per_plane.cd();
	// assuming to have 4 planes
	if (nplanes != 4)
		cout
				<< " warning: attempting to draw histograms for a number of planes not 4! "
				<< endl;
	canvas_properties_per_plane.Divide(2, 2);

	TCanvas canvas_properties_per_sensor("canvas_properties_per_sensor",
			"properties per sensor", 800, 800);
	canvas_properties_per_sensor.cd();
	// assuming to have 100 sensors per plane
	if (nsensors_per_plane != 100)
		cout
				<< " warning: attempting to draw histograms for a number of sensors per plane not 100! "
				<< endl;
	canvas_properties_per_sensor.Divide(10, 10);

	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		canvas_properties_per_plane.cd(_iplane + 1);
		hist_xy[_iplane]->Draw("COLZ");
	}
	canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
			canvas_properties_per_sensor.cd(_isensor + 1);
			hists_xy[_iplane][_isensor]->Draw("COLZ");
		}
		canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
	}
	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
			canvas_properties_per_sensor.cd(_isensor + 1);
			hists_xy_local[_iplane][_isensor]->Draw("COLZ");
		}
		canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
	}

	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		canvas_properties_per_plane.cd(_iplane + 1);
		hist_theta_init[_iplane]->Draw("COLZ");
	}
	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		canvas_properties_per_plane.cd(_iplane + 1);
		hist_theta_rec[_iplane]->Draw("COLZ");
	}
	canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
			canvas_properties_per_sensor.cd(_isensor + 1);
			hists_theta_init[_iplane][_isensor]->Draw();
			canvas_properties_per_sensor.cd(nsensors_per_plane+1);
			if (_isensor > 0)
				hists_theta_init[_iplane][_isensor]->Draw("same E");
			else
				hists_theta_init[_iplane][_isensor]->Draw("E");
			//hists_theta_init[_iplane][_isensor]->SetLineColor(
			//		kAzure - 9 + _isensor);
		}
		canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
	}
	for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
		canvas_properties_per_sensor.cd(_isensor + 1);
		for (int _iplane = 0; _iplane < nplanes; _iplane++) {
			if (_iplane > 0)
				hists_theta_init[_iplane][_isensor]->Draw("same");
			else
				hists_theta_init[_iplane][_isensor]->Draw();
			hists_theta_init[_iplane][_isensor]->SetLineColor(kGray + _iplane);
		}
	}
	canvas_properties_per_sensor.cd(nsensors_per_plane+1);
	gPad->Clear();
	canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");

	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		canvas_properties_per_plane.cd(_iplane + 1);
		hist_theta_in[_iplane]->Draw("COLZ");
	}
	canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
			canvas_properties_per_sensor.cd(_isensor + 1);
			hists_theta_in[_iplane][_isensor]->Draw();
			canvas_properties_per_sensor.cd(nsensors_per_plane+1);
			if (_isensor > 0)
				hists_theta_in[_iplane][_isensor]->Draw("same E");
			else
				hists_theta_in[_iplane][_isensor]->Draw("E");
			//hists_theta_in[_iplane][_isensor]->SetLineColor(kAzure - 9 + _isensor);
		}
		canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
	}
	for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
		canvas_properties_per_sensor.cd(_isensor + 1);
		for (int _iplane = 0; _iplane < nplanes; _iplane++) {
			if (_iplane > 0)
				hists_theta_in[_iplane][_isensor]->Draw("same");
			else
				hists_theta_in[_iplane][_isensor]->Draw();
			hists_theta_in[_iplane][_isensor]->SetLineColor(kGray + _iplane);
		}
	}
	canvas_properties_per_sensor.cd(nsensors_per_plane+1);
	gPad->Clear();
	canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");

	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		canvas_properties_per_plane.cd(_iplane + 1);
		hist_theta_diff[_iplane]->Draw("COLZ");
	}
	canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		canvas_properties_per_plane.cd(_iplane + 1);
		hist_theta_rec_diff[_iplane]->Draw("COLZ");
	}
	canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
			canvas_properties_per_sensor.cd(_isensor + 1);
			hists_theta_diff[_iplane][_isensor]->Draw();
			canvas_properties_per_sensor.cd(nsensors_per_plane+1);
			if (_isensor > 0)
				hists_theta_diff[_iplane][_isensor]->Draw("same E");
			else
				hists_theta_diff[_iplane][_isensor]->Draw("E");
			//hists_theta_diff[_iplane][_isensor]->SetLineColor(
			//		kAzure - 9 + _isensor);
		}
		canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
	}
	for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
		canvas_properties_per_sensor.cd(_isensor + 1);
		for (int _iplane = 0; _iplane < nplanes; _iplane++) {
			if (_iplane > 0)
				hists_theta_diff[_iplane][_isensor]->Draw("same");
			else
				hists_theta_diff[_iplane][_isensor]->Draw();
			hists_theta_diff[_iplane][_isensor]->SetLineColor(kGray + _iplane);
		}
	}
	canvas_properties_per_sensor.cd(nsensors_per_plane+1);
	gPad->Clear();
	canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");

	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		canvas_properties_per_plane.cd(_iplane + 1);
		hist_theta_diff_rel[_iplane]->Draw("COLZ");
	}
	canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		canvas_properties_per_plane.cd(_iplane + 1);
		hist_theta_rec_diff_rel[_iplane]->Draw("COLZ");
	}
	canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
	for (int _iplane = 0; _iplane < nplanes; _iplane++) {
		for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
			canvas_properties_per_sensor.cd(_isensor + 1);
			hists_theta_diff_rel[_iplane][_isensor]->Draw();
			canvas_properties_per_sensor.cd(nsensors_per_plane+1);
			if (_isensor > 0)
				hists_theta_diff_rel[_iplane][_isensor]->Draw("same E");
			else
				hists_theta_diff_rel[_iplane][_isensor]->Draw("E");
			//hists_theta_diff_rel[_iplane][_isensor]->SetLineColor(
			//		kAzure - 9 + _isensor);
		}
		canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
	}
	for (int _isensor = 0; _isensor < nsensors_per_plane; _isensor++) {
		canvas_properties_per_sensor.cd(_isensor + 1);
		for (int _iplane = 0; _iplane < nplanes; _iplane++) {
			if (_iplane > 0)
				hists_theta_diff_rel[_iplane][_isensor]->Draw("same");
			else
				hists_theta_diff_rel[_iplane][_isensor]->Draw();
			hists_theta_diff_rel[_iplane][_isensor]->SetLineColor(kGray + _iplane);
		}
	}
	canvas_properties_per_sensor.cd(nsensors_per_plane+1);
	gPad->Clear();
	canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");

	canvas_properties_per_plane.Clear();
	canvas_properties_per_plane.Print("Resolution_acceptance_results.ps)");
	// convert to pdf
	cout << "converting to pdf " << system(
			"ps2pdf Resolution_acceptance_results.ps") << endl;
	cout << "deleting ps file  " << system(
			"rm Resolution_acceptance_results.ps") << endl;

	if (hist_output_file)
			hist_output_file->Write();
	//output_histfile->Write();

}

std::map<int, std::vector<int> > PndLmdPerformanceTask::AssignHitsToTracks() {
	/*
	std::map<int, std::vector<int> > result;
	for (int i = 0; i < fMCHits->GetEntriesFast(); i++) { //get all MC Hits
		PndSdsMCPoint* myPoint = (PndSdsMCPoint*) (fMCHits->At(i)); //sort MCHits with Tracks
		//PndMCTrack* myTrack = (PndMCTrack*)(fMCTracks->At(myPoint->GetTrackID()));
		result[myPoint->GetTrackID()].push_back(i);

	}
	return result;*/
}

void PndLmdPerformanceTask::DrawProgressBar(int len, double percent) {
	if ((int) (last_percent * 100) == (int) (percent * 100))
		return;
	//cout << " drawing " << endl;
	cout << "\x1B[2K"; // Erase the entire current line.
	cout << "\x1B[0E"; // Move to the beginning of the current line.
	string progress;
	for (int i = 0; i < len; ++i) {
		if (i < static_cast<int> (len * percent)) {
			progress += "=";
		} else {
			progress += " ";
		}
	}
	cout << "[" << progress << "] " << (static_cast<int> (100 * percent))
			<< "%";
	flush( cout); // Required.
	last_percent = percent;
}

ClassImp(PndLmdPerformanceTask)
;
