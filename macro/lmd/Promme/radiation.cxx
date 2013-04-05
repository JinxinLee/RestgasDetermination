/*
 * test_main.cxx
 *
 *  Created on: Feb 8, 2012
 *      Author: promme
 */

#include<iostream>
#include<sstream>
#include<string>

#include<TString.h>
#include<TChain.h>
#include<TClonesArray.h>
#include<TROOT.h>
#include<TVector3.h>
#include<TH1.h>
#include<TH2.h>
#include<TApplication.h>
#include<TCanvas.h>

#include<FairRadMapPoint.h>

#include<PndLmdDim.h>

/*
#include<TCanvas.h>

#include<TFile.h>
#include<TSystem.h>

#include<TRotation.h>

#include<TMath.h>
#include<TGaxis.h>

#include<TStopwatch.h>

#include<PndMCTrack.h>
#include<PndSdsMCPoint.h>

// needed for geane backtracking
#include<FairRunAna.h>
#include<FairRootManager.h>
#include<FairGeane.h>
#include<FairRtdbRun.h>
#include<FairRuntimeDb.h>
#include<FairParRootFileIo.h>
#include<PndLmdPerformanceTask.h>
#include<FairLogger.h>

*/

using namespace std;

double last_percent(0);

void DrawProgressBar(int len, double percent) {
	if ((int)(last_percent*100) == (int)(percent*100)) return;
	//cout << " drawing " << endl;
	cout << "\x1B[2K"; // Erase the entire current line.
	cout << "\x1B[0E"; // Move to the beginning of the current line.
	string progress;
	for (int i = 0; i < len; ++i) {
	if (i < static_cast<int>(len * percent)) {
	progress += "=";
	} else {
	progress += " ";
	}
	}
	cout << "[" << progress << "] " << (static_cast<int>(100 * percent)) << "%";
	flush(cout); // Required.
	last_percent = percent;
}

int main(int nargs, char** args) {
	// load first all dictionaries for ROOT in order
	// to read the files correctly
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	TApplication myapp("myapp",0,0);
	TString storePath = ".";
	// set the storepath to somewhere else, if provided
	if (nargs == 2) {
		storePath = args[1];
	} else {
		if (nargs == 1)
			cout << " Hint: you may change the standard path from "
					<< storePath.Data()
					<< " to any other folder by providing the path as an argument! "
					<< endl;
		if (nargs > 2)
			cout << " Error: too many arguments! " << endl;
	}
	TString startEvent = "";
	// Input file (MC events)
	TString MCFile = storePath + "/Lumi_MC";
	MCFile += startEvent;
	MCFile += ".root";
	TChain tMC("cbmsim");
	tMC.Add(MCFile);

	std::cout << "MCFile  : " << MCFile.Data() << std::endl;

	//--- MC info -----------------------------------------------------------------
	TClonesArray* true_tracks = new TClonesArray("PndMCTrack");
	tMC.SetBranchAddress("MCTrack", &true_tracks); //True Tracks to compare

	TClonesArray* true_points = new TClonesArray("PndSdsMCPoint");
	tMC.SetBranchAddress("LMDPoint", &true_points); //True Points to compare

	TClonesArray* radmap_points = new TClonesArray("FairRadMapPoint");
	tMC.SetBranchAddress("RadMap", & radmap_points); // Radiation map points

	TClonesArray* radlen_points = new TClonesArray("FairRadLenPoint");
	tMC.SetBranchAddress("RadLen", & radlen_points); // Radiation length points
	//----------------------------------------------------------------------------------

	TCanvas canvas_rad_map_x_y("canvas_rad_map_x_y", "radiation map x y", 600, 600);
	TH2D radiation_map_x_y("radiation_map_x_y", "radiation map x y", 500, 15, 45, 500, -15, 15);
	radiation_map_x_y.Draw("COLZ");
	radiation_map_x_y.SetXTitle("X [cm]");
	radiation_map_x_y.SetYTitle("Y [cm]");
	radiation_map_x_y.SetZTitle("dose");
	canvas_rad_map_x_y.SetLogz();

	TCanvas canvas_rad_map_x_z("canvas_rad_map_x_z", "radiation map x z", 1000, 600);
	TH2D radiation_map_x_z("radiation_map_x_z", "radiation map x z", 1000, 1100, 1200, 400, 10, 50);
	radiation_map_x_z.Draw("COLZ");
	radiation_map_x_z.SetXTitle("Z [cm]");
	radiation_map_x_z.SetYTitle("X [cm]");
	radiation_map_x_z.SetZTitle("dose");
	canvas_rad_map_x_z.SetLogz();

	TCanvas canvas_rad_map_x_y_plane_0("canvas_rad_map_x_y_plane_0", "radiation map x y of plane 0", 600, 600);
	TH2D radiation_map_x_y_plane_0("radiation_map_x_y_plane_0", "radiation map x y of plane 0", 150, 10, 40, 150, -15, 15);
	radiation_map_x_y_plane_0.Draw("COLZ");
	radiation_map_x_y_plane_0.SetXTitle("X [cm]");
	radiation_map_x_y_plane_0.SetYTitle("Y [cm]");
	radiation_map_x_y_plane_0.SetZTitle("dose");
	canvas_rad_map_x_y_plane_0.SetLogz();

	// initialize the lmd dimension class
	PndLmdDim& lmddim = PndLmdDim::Get_instance();
	lmddim.Read_transformation_matrices("matrices_perfect.txt", false);
	lmddim.Read_transformation_matrices("matrices.txt", true);

	int nEvents = tMC.GetEntries();
	cout << " reading " << nEvents << " Events " << endl;
	for (Int_t iEvent = 0; iEvent < nEvents; iEvent++) {
		DrawProgressBar(50, (iEvent+1)/((double)nEvents));
		tMC.GetEntry(iEvent);

		//PndSdsMCPoint* mcpoint = (PndSdsMCPoint*) true_points->At(iHit);
		int nradmappoints = radmap_points->GetEntriesFast();
		for (int iradmappoint = 0; iradmappoint < nradmappoints; iradmappoint++){
			FairRadMapPoint* radmap_point = (FairRadMapPoint*) radmap_points->At(iradmappoint);
			const TVector3& position = radmap_point->GetPosition();
			double dose = radmap_point->GetDose();

			int detID = radmap_point->GetDetectorID();

			//int ihalf, iplane, imodule, iside, idie, isensor;
			//lmddim.Get_sensor_by_id(detID, ihalf, iplane, imodule, iside, idie, isensor);

			//if (iplane == 0) does not work
			if (dose > 0){
				if (position.Z() > 1105 && position.Z() < 1130)
					radiation_map_x_y_plane_0.Fill(position.X(), position.Y(), dose);
				radiation_map_x_y.Fill(position.X(), position.Y(), dose);
				radiation_map_x_z.Fill(position.Z(), position.X(), dose);
			}
		}

		if ((iEvent%1000)==0){
			canvas_rad_map_x_y.cd();
			radiation_map_x_y.Draw("COLZ");
			canvas_rad_map_x_y.Update();
			canvas_rad_map_x_y_plane_0.cd();
			radiation_map_x_y_plane_0.Draw("COLZ");
			canvas_rad_map_x_y_plane_0.Update();
			canvas_rad_map_x_z.cd();
			radiation_map_x_z.Draw("COLZ");
			canvas_rad_map_x_z.Update();
		}
	}

	canvas_rad_map_x_y.Print("dose_x_y.pdf");
	canvas_rad_map_x_y_plane_0.Print("dose_x_y_plane_0.pdf");
	canvas_rad_map_x_z.Print("dose_x_z.pdf");



/*
	TString DigiFile = storePath + "/Lumi_digi";
	DigiFile += startEvent;
	DigiFile += ".root";
	// Digi file
	TString RecoFile = storePath + "/Lumi_reco";
	RecoFile += startEvent;
	RecoFile += ".root";
	// TCand file
	TString CandFile = storePath + "/Lumi_TCand";
	CandFile += startEvent;
	CandFile += ".root";
	// Parameter file   << this one
	TString parFile = storePath + "/Lumi_Params";
	parFile += startEvent;
	parFile += ".root";
	// Track file
	TString TrkFile = storePath + "/Lumi_Track";
	TrkFile += startEvent;
	TrkFile += ".root";

	// ----  Load libraries   -------------------------------------------------
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libSds");
	gSystem->Load("libSdsReco");
	gSystem->Load("libLmd");
	gSystem->Load("libLmdReco");
	gSystem->Load("libLmdTrk");
	// ------------------------------------------------------------------------
	// Output file
	TString outFile = storePath + "/Lumi_Testing_";
	outFile += startEvent;
	outFile += ".root";

	// Output histogram file
	TString outFilehist = storePath + "/Lumi_performance_histograms_";
	outFilehist += startEvent;
	outFilehist += ".root";

	std::cout << "DigiFile: " << DigiFile.Data() << std::endl;
	std::cout << "RecoFile: " << RecoFile.Data() << std::endl;
	std::cout << "TCandFile: " << CandFile.Data() << std::endl;
	std::cout << "TrackFile: " << TrkFile.Data() << std::endl;
	std::cout << "TestingFile: " << outFile.Data() << std::endl;
	// ---  Now choose concrete engines for the different tasks   -------------
	// ------------------------------------------------------------------------


	// In general, the following parts need not be touched
	// ========================================================================


	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// ------------------------------------------------------------------------


	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(MCFile);
	//fRun->AddFriend(DigiFile);
	//fRun->AddFriend(RecoFile);
	//fRun->AddFriend(CandFile);
	//fRun->AddFriend(TrkFile);
	fRun->SetOutputFile(outFile);
	// ------------------------------------------------------------------------

	FairGeane *Geane = new FairGeane();
	fRun->AddTask(Geane);

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data());
	rtdb->setFirstInput(parInput1);

	// =========================================================================
	// ======                  Geane Back-Propagating                     ======
	// =========================================================================

	// -----    SDS hit producer   --------------------------------------------

	// PndEmcMapper *emcMap = PndEmcMapper::Instance(6);
	Double_t fpBeam = 1.5;
	TVector3 IP(0, 0, 0);
	PndLmdPerformanceTask* lmdperformance = new PndLmdPerformanceTask();
	lmdperformance->SetVerbose(0);
	fRun->AddTask(lmdperformance);
	rtdb->setOutput(parInput1);
	//lmdperformance->SetHistFilename(outFilehist);
	rtdb->print();
	// =====                        End of Geane                           =====
	// =========================================================================

	// -----   Intialise and run   --------------------------------------------
	fRun->Init();
	//  // PndEmcMapper *emcMap = PndEmcMapper::Instance(6);
	//  PndEmcMapper *emcMap = PndEmcMapper::Instance();
	// //Geane->SetField(fRun->GetField());
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "Error: "
				<< "RootManager not instantiated!" << std::endl;
		return 1;
	}
	fRun->Run(0, ioman->CheckMaxEventNo()); // hmm does not work yet, seems to return 0 but then is runs over all what was intended.
	// ------------------------------------------------------------------------
	rtdb->saveOutput();
	rtdb->print();
	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	//myapp.Run();
*/
	return 0;
}

/*
 // some design constants of the LUMI detector
 const int nplanes(4);
 const int nsensors_per_plane(8);

 double last_percent(-1.);
 // draw a progress bar only when the length changes significantly
 void DrawProgressBar(int len, double percent);

 void Resolution_and_acceptance(){
 cout << " analysis tool vers. 1.1 " << endl;


 gSystem->Load("libSds");
 gSystem->Load("libSdsReco");
 gSystem->Load("libLmd");
 gSystem->Load("libLmdReco");
 gSystem->Load("libLmdTrk");

 TStopwatch timer;
 timer.Start();
 // ------------------------------------------------------------------------

 TString storePath = "./tmpOutput";
 TString startEvent = "0";
 // ---- Input files --------------------------------------------------------
 TString simMC = storePath + "/Lumi_MC_";
 simMC += startEvent;
 simMC += ".root";
 TChain tMC("cbmsim");
 tMC.Add(simMC);

 TString DigiFile = storePath + "/Lumi_digi_";
 DigiFile += startEvent;
 DigiFile += ".root";
 TChain tdigiHits("cbmsim");
 tdigiHits.Add(DigiFile);

 TString recHit = storePath + "/Lumi_reco_";
 recHit += startEvent;
 recHit += ".root";
 TChain tHits("cbmsim");
 tHits.Add(recHit);

 TString trkCand = storePath + "/Lumi_TCand_";
 trkCand += startEvent;
 trkCand += ".root";
 TChain tTrkCand("cbmsim");
 tTrkCand.Add(trkCand);

 TString recTrack = storePath + "/Lumi_Track_";
 recTrack += startEvent;
 recTrack += ".root";
 TChain tTrkRec("cbmsim");
 tTrkRec.Add(recTrack);

 TString geaneFile = storePath + "/Lumi_Geane_";
 geaneFile += startEvent;
 geaneFile += ".root";
 TChain tgeane("cbmsim");
 tgeane.Add(geaneFile);

 // Parameter file   << needed for geane back tracking
 TString parFile = storePath+"/Lumi_Params_";
 parFile += startEvent;
 parFile += ".root";

 // ---- Output file ----------------------------------------------------------------
 TString fakefile = storePath + "/fake";
 fakefile += startEvent;
 fakefile += ".root";tmpOutput
 // ---------------------------------------------------------------------------------

 // ---------------------------------------------------------------------------------

 // ---- needed to use the back propagation ----------------------------------------

 FairRunAna *fRun= new FairRunAna();
 fRun->SetInputFile(simMC);
 //fRun->AddFriend(DigiFile);
 //fRun->AddFriend(recHit);
 //fRun->AddFriend(trkCand);
 //fRun->AddFriend(recTrack);
 fRun->SetOutputFile(fakefile);
 // ------------------------------------------------------------------------

 FairGeane *Geane = new FairGeane();
 //fRun->AddTask(Geane);
 FairGeanePro* fPro = Geane->Exec();
 //	new FairGeanePro();

 cout << " ok " << endl;

 // -----  Parameter database   --------------------------------------------
 FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
 FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
 parInput1->open(parFile.Data());
 rtdb->setFirstInput(parInput1);
 fRun->Init();

 // ---- Output file ----------------------------------------------------------------
 TString out = storePath + "/Resolution_and_acceptance";
 out += startEvent;
 out += ".root";
 TFile *output_histfile = new TFile(out, "RECREATE");
 // ---------------------------------------------------------------------------------

 //--- MC info -----------------------------------------------------------------
 TClonesArray* true_tracks = new TClonesArray("PndMCTrack");
 tMC.SetBranchAddress("MCTrack", &true_tracks); //True Track to compare

 TClonesArray* true_points = new TClonesArray("PndSdsMCPoint");
 tMC.SetBranchAddress("LMDPoint", &true_points); //True Points to compare
 //----------------------------------------------------------------------------------


 //--- Digitization info ------------------------------------------------------------
 TClonesArray* fStripClusterArray = new TClonesArray("PndSdsClusterStrip");
 tHits.SetBranchAddress("LMDStripClusterCand", &fStripClusterArray);

 TClonesArray* fStripDigiArray = new TClonesArray("PndSdsDigiStrip");
 tdigiHits.SetBranchAddress("LMDStripDigis", &fStripDigiArray);
 //----------------------------------------------------------------------------------

 //--- Real Hits --------------------------------------------------------------------
 TClonesArray* rechit_array = new TClonesArray("PndSdsHit");
 tHits.SetBranchAddress("LMDHitsStrip", &rechit_array); //Points for Tracks
 //----------------------------------------------------------------------------------


 //--- Track Candidate ---------------------------------------------------------------
 TClonesArray* trkcand_array = new TClonesArray("PndTrackCand");
 tTrkCand.SetBranchAddress("LMDTrackCand", &trkcand_array); //Points for Track Canidates
 //-----------------------------------------------------------------------------------

 //--- Real tracks -------------------------------------------------------------------
 TClonesArray* rec_trk = new TClonesArray("PndLinTrack");
 tTrkRec.SetBranchAddress("LMDTrack", &rec_trk); //Tracks
 //----------------------------------------------------------------------------------

 //--- Geane info ------------------------------------------------------------------
 // TClonesArray* geaneArray =new TClonesArray("FairTrackParP");
 //TClonesArray* geaneArray = new TClonesArray("FairTrackParH");
 //tgeane.SetBranchAddress("GeaneTrackFinal", &geaneArray); //Tracks with parabolic parameterization
 // angular acceptance in theta and phi

 TH2* hist_angular_distr_gen = new TH2F("hist_angular_distr_gen", "hist_angular_distr_gen", 100, 0., 20.e-3, 100, 0., 3.141);
 TH2* hist_angular_distr_acc = new TH2F("hist_angular_distr_acc", "hist_angular_distr_acc", 100, 0., 20.e-3, 100, 0., 3.141);

 // spatial acceptance in x and y at the first lumi plane
 TH2* hist_spatial_distr_gen = new TH2F("hist_spatial_distr_gen", "hist_spatial_distr_gen", 1000, -100., 100., 1000, -100., 100.);
 TH2* hist_spatial_distr_acc = new TH2F("hist_spatial_distr_acc", "hist_spatial_distr_acc", 1000, -100., 100., 1000, -100., 100.);

 TTree* tree_results = new TTree("tree_results", "tree_results");
 // initial momenta of the antiprotons
 double px_init;
 double py_init;
 double pz_init;
 double ptheta_init;
 double pphi_init;
 tree_results->Branch("px_init", &px_init);
 tree_results->Branch("py_init", &py_init);
 tree_results->Branch("pz_init", &pz_init);
 tree_results->Branch("ptheta_init", &ptheta_init);
 tree_results->Branch("pphi_init", &pphi_init);
 // momenta at the entry of a detector plane
 // in the reference system of the LUMI
 double px_in;
 double py_in;
 double pz_in;
 double ptheta_in;
 double pphi_in;
 tree_results->Branch("px_in", &px_in);
 tree_results->Branch("py_in", &py_in);
 tree_results->Branch("pz_in", &pz_in);
 tree_results->Branch("ptheta_in", &ptheta_in);
 tree_results->Branch("pphi_in", &pphi_in);
 // position at the entry of a detector plane
 // in the reference system of the LUMI
 double x_in;
 double y_in;
 double z_in;
 tree_results->Branch("x_in", &x_in);
 tree_results->Branch("y_in", &y_in);
 tree_results->Branch("z_in", &z_in);
 // detector and sensor id of the mc hits
 int det_id;
 int sens_id;
 tree_results->Branch("det_id", &det_id);
 tree_results->Branch("sens_id", &sens_id);
 // calculated plane and sensor in plane id
 int plane;
 int sensor;
 tree_results->Branch("plane", &plane);
 tree_results->Branch("sensor", &sensor);

 // Drawing directly from a tree is elegant but slow as every Draw call
 // loops over the whole tree of events
 // Better it is to create the desired histograms in advance
 // and to fill those on event by event basis simultaneously

 // histograms per plane
 TH2* hist_xy             [nplanes];
 TH1* hist_theta_init     [nplanes];
 TH1* hist_theta_in       [nplanes];
 TH1* hist_theta_diff     [nplanes];
 TH1* hist_theta_diff_rel [nplanes];

 // histograms per sensor
 TH2* hists_xy             [nplanes][nsensors_per_plane];
 TH1* hists_theta_init     [nplanes][nsensors_per_plane];
 TH1* hists_theta_in       [nplanes][nsensors_per_plane];
 TH1* hists_theta_diff     [nplanes][nsensors_per_plane];
 TH1* hists_theta_diff_rel [nplanes][nsensors_per_plane];

 TCanvas temp_canvas("temp_canvas", "canvas for initialization", 100, 100);
 temp_canvas.cd();
 // loop over planes
 cout << " constructing histograms for " << nplanes << " planes with " << nsensors_per_plane << " sensors per plane " << endl;

 for (int iplane = 0; iplane < nplanes; iplane++){
 stringstream hist_name;
 stringstream hist_title;

 hist_name  << "hist_xy_plane_" << iplane;
 hist_title << "xy hit distribution plane " << iplane;
 hist_xy[iplane] = new TH2F(hist_name.str().c_str(), hist_title.str().c_str(),
 100, -10, 10, 100, -10, 10);
 hist_xy[iplane]->Draw();
 hist_xy[iplane]->GetXaxis()->SetTitle("X [cm]");
 hist_xy[iplane]->GetYaxis()->SetTitle("Y [cm]");

 hist_name .str("");
 hist_title.str("");

 hist_name  << "hist_theta_init_plane_" << iplane;
 hist_title << "initial #Theta distribution plane " << iplane;
 hist_theta_init[iplane] = new TH1F(hist_name.str().c_str(), hist_title.str().c_str(),
 150, 0, 15e-3);
 hist_theta_init[iplane]->Draw();
 hist_theta_init[iplane]->GetXaxis()->SetTitle("#Theta [rad]");
 hist_theta_init[iplane]->GetYaxis()->SetTitle("entries");

 hist_name .str("");
 hist_title.str("");

 hist_name  << "hist_theta_in_plane_" << iplane;
 hist_title << "#Theta distribution at plane " << iplane;
 hist_theta_in[iplane] = new TH1F(hist_name.str().c_str(), hist_title.str().c_str(),
 200, 0, 20e-3);
 hist_theta_in[iplane]->Draw();
 hist_theta_in[iplane]->GetXaxis()->SetTitle("#Theta [rad]");
 hist_theta_in[iplane]->GetYaxis()->SetTitle("entries");

 hist_name .str("");
 hist_title.str("");

 hist_name  << "hist_theta_diff_in_plane_" << iplane;
 hist_title << "#Delta#Theta distribution at plane " << iplane;
 hist_theta_diff[iplane] = new TH1F(hist_name.str().c_str(), hist_title.str().c_str(),
 100, -20e-5, 20e-5);
 hist_theta_diff[iplane]->Draw();
 hist_theta_diff[iplane]->GetXaxis()->SetTitle("#Delta#Theta [rad]");
 hist_theta_diff[iplane]->GetYaxis()->SetTitle("entries");

 hist_name .str("");
 hist_title.str("");

 hist_name  << "hist_theta_diff_rel_in_plane_" << iplane;
 hist_title << "#Delta#Theta/#Theta distribution at plane " << iplane;
 hist_theta_diff_rel[iplane] = new TH1F(hist_name.str().c_str(), hist_title.str().c_str(),
 100, -1, 1);
 hist_theta_diff_rel[iplane]->Draw();
 hist_theta_diff_rel[iplane]->GetXaxis()->SetTitle("#Delta#Theta/#Theta");
 hist_theta_diff_rel[iplane]->GetYaxis()->SetTitle("entries");

 // loop over sensors per plane which are enumerated linearly
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 hist_name .str("");
 hist_title.str("");

 hist_name  << "hist_xy_plane_" << iplane << "_sensor_" << isensor;
 hist_title << "xy hit distribution plane " << iplane << " sensor " << isensor;
 hists_xy[iplane][isensor] = new TH2F(hist_name.str().c_str(), hist_title.str().c_str(),
 100, -10, 10, 100, -10, 10);
 hists_xy[iplane][isensor]->Draw();
 hists_xy[iplane][isensor]->GetXaxis()->SetTitle("X [cm]");
 hists_xy[iplane][isensor]->GetYaxis()->SetTitle("Y [cm]");

 hist_name .str("");
 hist_title.str("");

 hist_name  << "hist_theta_init_plane_" << iplane << "_sensor_" << isensor;
 hist_title << "initial #Theta distribution plane " << iplane << " sensor " << isensor;
 hists_theta_init[iplane][isensor] = new TH1F(hist_name.str().c_str(), hist_title.str().c_str(),
 150, 0, 15e-3);
 hists_theta_init[iplane][isensor]->Draw();
 hists_theta_init[iplane][isensor]->GetXaxis()->SetTitle("#Theta [rad]");
 hists_theta_init[iplane][isensor]->GetYaxis()->SetTitle("entries");

 hist_name .str("");
 hist_title.str("");

 hist_name  << "hist_theta_in_plane_" << iplane << "_sensor_" << isensor;
 hist_title << "#Theta distribution at plane " << iplane << " sensor " << isensor;
 hists_theta_in[iplane][isensor] = new TH1F(hist_name.str().c_str(), hist_title.str().c_str(),
 200, 0, 20e-3);
 hists_theta_in[iplane][isensor]->Draw();
 hists_theta_in[iplane][isensor]->GetXaxis()->SetTitle("#Theta [rad]");
 hists_theta_in[iplane][isensor]->GetYaxis()->SetTitle("entries");

 hist_name .str("");
 hist_title.str("");

 hist_name  << "hist_theta_diff_in_plane_" << iplane << "_sensor_" << isensor;
 hist_title << "#Delta#Theta distribution at plane " << iplane << " sensor " << isensor;
 hists_theta_diff[iplane][isensor] = new TH1F(hist_name.str().c_str(), hist_title.str().c_str(),
 100, -20e-5, 20e-5);
 hists_theta_diff[iplane][isensor]->Draw();
 hists_theta_diff[iplane][isensor]->GetXaxis()->SetTitle("#Delta#Theta [rad]");
 hists_theta_diff[iplane][isensor]->GetYaxis()->SetTitle("entries");

 hist_name .str("");
 hist_title.str("");

 hist_name  << "hist_theta_diff_rel_in_plane_" << iplane << "_sensor_" << isensor;
 hist_title << "#Delta#Theta/#Theta distribution at plane " << iplane << " sensor " << isensor;
 hists_theta_diff_rel[iplane][isensor] = new TH1F(hist_name.str().c_str(), hist_title.str().c_str(),
 100, -1, 1);
 hists_theta_diff_rel[iplane][isensor]->Draw();
 hists_theta_diff_rel[iplane][isensor]->GetXaxis()->SetTitle("#Delta#Theta/#Theta");
 hists_theta_diff_rel[iplane][isensor]->GetYaxis()->SetTitle("entries");
 }
 }


 // rotation into the lumi position according to the beamline file
 // matrix tri6 - tr=1  rot=1  refl=0  scl=0
 //  0.999192    0.000000    0.040200    Tx =  22.893063
 //  0.000000    1.000000   -0.000000    Ty =   0.000000
 // -0.040200    0.000000    0.999192    Tz = 1049.770390
 // The matrix is a rotation around y of around 0.04025 radian
 TRotation inv_lmdrotation;
 //inv_lmdrotation.RotateY(-0.04025);
 inv_lmdrotation.RotateY(-2.326/180.*TMath::Pi()); // as derived by mathias michel
 TVector3 inv_lmdtranslation(
 -22.893063, 0.000000, -1049.770390);

 int nEvents = tMC.GetEntries();
 if (tTrkCand.GetEntries() != nEvents) {cout << "nMC <> nTrkCand! Exiting." << endl; return;}
 if (tTrkRec.GetEntries() != nEvents) {cout << "nMC <> nTrkRec! Exiting." << endl; return;}
 if (tHits.GetEntries() != nEvents) {cout << "nMC <> nHits! Exiting." << endl; return;}
 if (tdigiHits.GetEntries() != nEvents) {cout << "nMC <> nTrkdigiHits! Exiting." << endl; return;}
 //if (tgeane.GetEntries() != nEvents) {cout << "nMC <> TrkCand! Exiting." << endl; return;}
 //TVector3 ip(0,0,0);
 //PndLmdGeaneTask* lmdgeane = new PndLmdGeaneTask();
 cout << " reading " << nEvents << " Events " << endl;
 for (Int_t j = 0; j < nEvents; j++) {
 DrawProgressBar(50, (j+1)/((double)nEvents));
 tMC.GetEntry(j);
 tTrkCand.GetEntry(j);
 tTrkRec.GetEntry(j);
 tHits.GetEntry(j);
 tdigiHits.GetEntry(j);
 const int nParticles = true_tracks->GetEntriesFast();

 int npbar = 0;
 for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
 // if(nParticles!=4) continue;
 PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(iParticle);
 Int_t mcID = mctrk->GetPdgCode();
 if (mctrk->IsGeneratorCreated()){//mcID == -2212) {
 npbar++;
 //  if(nParticles!=4) cout<<"For event #"<<j<<" mcID="<<mcID<<endl;
 TVector3 MomMC = mctrk->GetMomentum();
 hist_angular_distr_gen->Fill(MomMC.Theta(), MomMC.Phi());
 //hallTheta->Fill(MomMC.Theta());
 //hallPhi->Fill(MomMC.Phi());
 }
 }
 if (0&&npbar != 1)
 cout << "found " << npbar << " anti-protons" << endl;

 int nHits = true_points->GetEntriesFast();
 int nSdsHits = 0;
 for (Int_t iHit = 0; iHit < nHits; iHit++) {
 PndSdsMCPoint* mcpoint = (PndSdsMCPoint*) true_points->At(iHit);
 PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(mcpoint->GetTrackID());
 if (mctrk->IsGeneratorCreated()){ // take only anti-protons from the generator
 if (mcpoint){
 TVector3 _mcpoint((mcpoint->GetPosition())); // position at the sensor entrance
 TVector3 _mctrack(mcpoint->GetPx(), mcpoint->GetPy(), mcpoint->GetPz()); // momentum of the track at the entrance
 if (1) { // translate it into the reference system of the lumi monitor
 _mcpoint = inv_lmdtranslation+_mcpoint;
 _mcpoint = inv_lmdrotation*_mcpoint;
 }
 TVector3 momMC = mctrk->GetMomentum(); // momentum in the primary vertex
 TVector3 posMC = mctrk->GetStartVertex(); // position of the primary vertex
 if (1) { // use back propagation for the track
 TVector3 initpos, initmom(0,0,momMC.Mag()),
 StartPosErr, StartMomErr, \
								ResPosErr, ResMomErr;
 int charge = mctrk->GetPdgCode() > 0 ? 1 : mctrk->GetPdgCode() < 0 ? -1 : 0;
 FairTrackParH *fStart =
 new FairTrackParH(posMC, momMC, StartPosErr, StartMomErr, charge); // exchange fCharge by charge of PDGCode
 FairTrackParH *fRes =
 new FairTrackParH(initpos, initmom, ResPosErr, ResMomErr, charge);
 Bool_t isProp = fPro->Propagate(fStart, fRes, mctrk->GetPdgCode());
 if (!isProp) cout << "Propagate(): back propagation not possible! " << endl;
 _mctrack = initmom;// inv_lmdrotation*_mctrack;

 }
 det_id = mcpoint->GetDetectorID(); // store the detector id
 sens_id = mcpoint->GetSensorID(); // store the sensor id
 // calculate the plane and sensor on this plane
 plane = sens_id/nsensors_per_plane;
 sensor = sens_id-plane*nsensors_per_plane;

 // write those values out
 px_init = momMC.X();
 py_init = momMC.Y();
 pz_init = momMC.Z();
 ptheta_init = momMC.Theta();
 pphi_init   = momMC.Phi();
 x_in    = _mcpoint.X();
 y_in    = _mcpoint.Y();
 z_in    = _mcpoint.Z();
 px_in   = _mctrack.X();
 py_in   = _mctrack.Y();
 pz_in   = _mctrack.Z();
 ptheta_in = _mctrack.Theta();
 pphi_in   = _mctrack.Phi();

 if (plane < 0 || plane >= nplanes || sensor < 0 || sensor >= nsensors_per_plane){
 cout << " warning: calculated plane or sensor is wrong: plane " << plane << " sensor " << sensor << endl;
 } else {
 hist_xy [plane]        ->Fill(x_in,y_in);
 hists_xy[plane][sensor]->Fill(x_in,y_in);

 hist_theta_init  [plane]        ->Fill(ptheta_init);
 hists_theta_init [plane][sensor]->Fill(ptheta_init);

 hist_theta_in [plane]        ->Fill(ptheta_in);
 hists_theta_in[plane][sensor]->Fill(ptheta_in);

 hist_theta_diff[plane]         ->Fill(ptheta_in-ptheta_init);
 hists_theta_diff[plane][sensor]->Fill(ptheta_in-ptheta_init);

 if (ptheta_init > 0){
 hist_theta_diff_rel[plane]         ->Fill((ptheta_in-ptheta_init)/ptheta_init);
 hists_theta_diff_rel[plane][sensor]->Fill((ptheta_in-ptheta_init)/ptheta_init);
 }

 tree_results->Fill();
 }

 //cout << _mcpoint.X() << " " << _mcpoint.Y() << " "<< _mcpoint.Z() << endl;
 hist_spatial_distr_acc->Fill(_mcpoint.X(), _mcpoint.Y());
 hist_angular_distr_acc->Fill(momMC.Theta(), momMC.Phi());
 nSdsHits++;
 }
 }
 }
 if (0 && nHits > 0)
 cout << "found " << nHits << " hit(s) with " << nSdsHits << " sds hit(s)" << endl;
 }

 hist_angular_distr_acc->Divide(hist_angular_distr_gen);

 TGaxis::SetMaxDigits(3);
 TCanvas* canvas1 = new TCanvas("canvas1", "canvas1", 800, 800);
 canvas1->Divide(2,2);
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
 hist_spatial_distr_gen->GetXaxis()->SetRangeUser(-10,10);
 hist_spatial_distr_gen->GetYaxis()->SetRangeUser(-10,10);
 gPad->Update();
 canvas1->cd(4);
 hist_spatial_distr_acc->Draw("COLZ");
 hist_spatial_distr_acc->SetXTitle("X [cm]");
 hist_spatial_distr_acc->SetYTitle("Y [cm]");
 hist_spatial_distr_acc->GetXaxis()->SetRangeUser(-10,10);
 hist_spatial_distr_acc->GetYaxis()->SetRangeUser(-10,10);
 gPad->Update();

 // draw individual plane and sensor properties
 //gStyle->SetPaperSize(25,25);
 TCanvas canvas_properties_per_plane("canvas_properties_per_plane", "properties per plane", 800, 800);
 canvas_properties_per_plane.cd();
 // assuming to have 4 planes
 if (nplanes != 4) cout << " warning: attempting to draw histograms for a number of planes not 4! " << endl;
 canvas_properties_per_plane.Divide(2,2);

 TCanvas canvas_properties_per_sensor("canvas_properties_per_sensor", "properties per sensor", 800, 800);
 canvas_properties_per_sensor.cd();
 // assuming to have 8 sensors per plane
 if (nsensors_per_plane != 8) cout << " warning: attempting to draw histograms for a number of sensors per plane not 8! " << endl;
 canvas_properties_per_sensor.Divide(3,3);

 for (int iplane = 0; iplane < nplanes; iplane++){
 canvas_properties_per_plane.cd(iplane+1);
 hist_xy[iplane]->Draw("COLZ");
 }
 canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
 for (int iplane = 0; iplane < nplanes; iplane++){
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 canvas_properties_per_sensor.cd(isensor+1);
 hists_xy[iplane][isensor]->Draw("COLZ");
 }
 canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
 }

 for (int iplane = 0; iplane < nplanes; iplane++){
 canvas_properties_per_plane.cd(iplane+1);
 hist_theta_init[iplane]->Draw();
 }
 canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
 for (int iplane = 0; iplane < nplanes; iplane++){
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 canvas_properties_per_sensor.cd(isensor+1);
 hists_theta_init[iplane][isensor]->Draw();
 canvas_properties_per_sensor.cd(9);
 if (isensor > 0)
 hists_theta_init[iplane][isensor]->Draw("same E");
 else
 hists_theta_init[iplane][isensor]->Draw("E");
 hists_theta_init[iplane][isensor]->SetLineColor(kAzure-9+isensor);
 }
 canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
 }
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 canvas_properties_per_sensor.cd(isensor+1);
 for (int iplane = 0; iplane < nplanes; iplane++){
 if (iplane > 0)
 hists_theta_init[iplane][isensor]->Draw("same");
 else
 hists_theta_init[iplane][isensor]->Draw();
 hists_theta_init[iplane][isensor]->SetLineColor(kGray+iplane);
 }
 }
 canvas_properties_per_sensor.cd(9);
 gPad->Clear();
 canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");

 for (int iplane = 0; iplane < nplanes; iplane++){
 canvas_properties_per_plane.cd(iplane+1);
 hist_theta_in[iplane]->Draw();
 }
 canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
 for (int iplane = 0; iplane < nplanes; iplane++){
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 canvas_properties_per_sensor.cd(isensor+1);
 hists_theta_in[iplane][isensor]->Draw();
 canvas_properties_per_sensor.cd(9);
 if (isensor > 0)
 hists_theta_in[iplane][isensor]->Draw("same E");
 else
 hists_theta_in[iplane][isensor]->Draw("E");
 hists_theta_in[iplane][isensor]->SetLineColor(kAzure-9+isensor);
 }
 canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
 }
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 canvas_properties_per_sensor.cd(isensor+1);
 for (int iplane = 0; iplane < nplanes; iplane++){
 if (iplane > 0)
 hists_theta_in[iplane][isensor]->Draw("same");
 else
 hists_theta_in[iplane][isensor]->Draw();
 hists_theta_in[iplane][isensor]->SetLineColor(kGray+iplane);
 }
 }
 canvas_properties_per_sensor.cd(9);
 gPad->Clear();
 canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");

 for (int iplane = 0; iplane < nplanes; iplane++){
 canvas_properties_per_plane.cd(iplane+1);
 hist_theta_diff[iplane]->Draw();
 }
 canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
 for (int iplane = 0; iplane < nplanes; iplane++){
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 canvas_properties_per_sensor.cd(isensor+1);
 hists_theta_diff[iplane][isensor]->Draw();
 canvas_properties_per_sensor.cd(9);
 if (isensor > 0)
 hists_theta_diff[iplane][isensor]->Draw("same E");
 else
 hists_theta_diff[iplane][isensor]->Draw("E");
 hists_theta_diff[iplane][isensor]->SetLineColor(kAzure-9+isensor);
 }
 canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
 }
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 canvas_properties_per_sensor.cd(isensor+1);
 for (int iplane = 0; iplane < nplanes; iplane++){
 if (iplane > 0)
 hists_theta_diff[iplane][isensor]->Draw("same");
 else
 hists_theta_diff[iplane][isensor]->Draw();
 hists_theta_diff[iplane][isensor]->SetLineColor(kGray+iplane);
 }
 }
 canvas_properties_per_sensor.cd(9);
 gPad->Clear();
 canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");

 for (int iplane = 0; iplane < nplanes; iplane++){
 canvas_properties_per_plane.cd(iplane+1);
 hist_theta_diff_rel[iplane]->Draw();
 }
 canvas_properties_per_plane.Print("Resolution_acceptance_results.ps(");
 for (int iplane = 0; iplane < nplanes; iplane++){
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 canvas_properties_per_sensor.cd(isensor+1);
 hists_theta_diff_rel[iplane][isensor]->Draw();
 canvas_properties_per_sensor.cd(9);
 if (isensor > 0)
 hists_theta_diff_rel[iplane][isensor]->Draw("same E");
 else
 hists_theta_diff_rel[iplane][isensor]->Draw("E");
 hists_theta_diff_rel[iplane][isensor]->SetLineColor(kAzure-9+isensor);
 }
 canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");
 }
 for (int isensor = 0; isensor < nsensors_per_plane; isensor++){
 canvas_properties_per_sensor.cd(isensor+1);
 for (int iplane = 0; iplane < nplanes; iplane++){
 if (iplane > 0)
 hists_theta_diff_rel[iplane][isensor]->Draw("same");
 else
 hists_theta_diff_rel[iplane][isensor]->Draw();
 hists_theta_diff_rel[iplane][isensor]->SetLineColor(kGray+iplane);
 }
 }
 canvas_properties_per_sensor.cd(9);
 gPad->Clear();
 canvas_properties_per_sensor.Print("Resolution_acceptance_results.ps(");

 canvas_properties_per_plane.Clear();
 canvas_properties_per_plane.Print("Resolution_acceptance_results.ps)");
 // convert to pdf
 cout << "converting to pdf " << system("ps2pdf Resolution_acceptance_results.ps")<< endl;
 cout << "deleting ps file  " << system("rm Resolution_acceptance_results.ps")<< endl;

 output_histfile->Write();

 hist_angular_distr_gen->SetDirectory(0);
 hist_angular_distr_acc->SetDirectory(0);
 hist_spatial_distr_gen->SetDirectory(0);
 hist_spatial_distr_acc->SetDirectory(0);
 output_histfile->Close();
 }

 void DrawProgressBar(int len, double percent) {
 if ((int)(last_percent*100) == (int)(percent*100)) return;
 //cout << " drawing " << endl;
 cout << "\x1B[2K"; // Erase the entire current line.
 cout << "\x1B[0E"; // Move to the beginning of the current line.
 string progress;
 for (int i = 0; i < len; ++i) {
 if (i < static_cast<int>(len * percent)) {
 progress += "=";
 } else {
 progress += " ";
 }
 }
 cout << "[" << progress << "] " << (static_cast<int>(100 * percent)) << "%";
 flush(cout); // Required.
 last_percent = percent;
 }*/

