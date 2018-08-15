#include<TString.h>
#include<TH1.h>
#include<TH2.h>
#include<sstream>
#include<iostream>
#include<vector>
#include<map>
#include<TVectorT.h>
#include<TCanvas.h>
#include<TApplication.h>
#include<TChain.h>
#include<TROOT.h>
#include<TSystem.h>
#include<TClonesArray.h>
#include<TRotation.h>
#include<TFile.h>
#include<TGaxis.h>
#include<PndMCTrack.h>
#include<PndSdsMCPoint.h>
#include<FairRunAna.h>
#include<FairGeane.h>
#include<FairRtdbRun.h>
#include<FairRuntimeDb.h>
#include<FairParRootFileIo.h>
#include<TGeoVolume.h>
#include<TGraph.h>
#include<TF1.h>
#include <algorithm>
#include<fstream>
#include<TStyle.h>
#include<TGaxis.h>
#include<TColor.h>
#include<TFile.h>
#include<FairGeoLoader.h>
#include<FairGeoInterface.h>
#include<FairGeoBuilder.h>
#include<FairGeoPcon.h>
#include<FairGeoMedia.h>
#include<TGeoManager.h>
#include <TImage.h>
#include <TLine.h>
#include <stdlib.h>
#include <TPolyLine.h>

using namespace std;

// some design constants
const int nplanes(120);

double last_percent(-1.);
// draw a progress bar only when the length changes significantly
void DrawProgressBar(int len, double percent);

// the path for the beam pipe as a function of the distance z to the interaction point
// par[0]: where bending starts (dipole region)
// par[1]: the bending radius
// par[2]: the bending angle
Double_t function_beampipe(Double_t *x, Double_t *par);

// a circle created out of a polyline
TPolyLine* Get_PolyLine_circle(float x,float y, float r);

// when giving a path to a beam pipe geometry, the beampipe is drawn to a panel
// when checking for acceptance along a beam pipe
string geo_beampipe = "/home/jasinski/bin/pandaroot/macro/lmd/Promme/beampipe_201209.root";

int Check_particle_path() {
	cout << " analysis tool vers. 1.4 " << endl;

	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libSds");
	gSystem->Load("libSdsReco");
	gSystem->Load("libLmd");
	gSystem->Load("libLmdReco");
	gSystem->Load("libLmdTrk");

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
		gStyle->SetTitleOffset(0.8,"y");
		gStyle->SetTitleOffset(1.3,"z");
		gStyle->SetLabelFont(10*13+2,"xyz");
		gStyle->SetLabelSize(0.06,"xyz");
		gStyle->SetLabelOffset(0.009,"xyz");
		gStyle->SetPadBottomMargin(0.16);
		gStyle->SetPadTopMargin(0.16);
		gStyle->SetPadLeftMargin(0.10);
		gStyle->SetPadRightMargin(0.10);
		gStyle->SetOptTitle(1);
		gStyle->SetOptStat(1);
		gROOT->ForceStyle();
		gStyle->SetFrameFillColor(0);
	   	gStyle->SetFrameFillStyle(0);
	   	TGaxis::SetMaxDigits(3);
	}

	// ------------------------------------------------------------------------

	TString storePath = ".";
	TString startEvent = "";
	// ---- Input files --------------------------------------------------------
	TString simMC = storePath + "/Lumi_MC";
	simMC += startEvent;
	simMC += ".root";
	TChain tMC("pndsim");
	tMC.Add(simMC);

	TString DigiFile = storePath + "/Lumi_digi";
	DigiFile += startEvent;
	DigiFile += ".root";
	TChain tdigiHits("pndsim");
	tdigiHits.Add(DigiFile);

	// ---- Output file ----------------------------------------------------------------
	TString fakefile = storePath + "/fake";
	fakefile += startEvent;
	fakefile += ".root";

	// ---- Output file ----------------------------------------------------------------
	TString out = storePath + "/beampipe_results";
	out += startEvent;
	out += ".root";
	TFile *output_histfile = new TFile(out, "RECREATE");
	// ---------------------------------------------------------------------------------

	// Parameter file   << needed for geane back tracking
	TString parFile = storePath+"/Lumi_Params";
	parFile += startEvent;
	parFile += ".root";

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(simMC);
	fRun->AddFriend(DigiFile);
	//fRun->AddFriend(RecoFile);
	//fRun->AddFriend(CandFile);
	//fRun->AddFriend(TrkFile);
	fRun->SetOutputFile(fakefile);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data());
	rtdb->setFirstInput(parInput1);

	//--- MC info -----------------------------------------------------------------
	TClonesArray* true_tracks = new TClonesArray("PndMCTrack");
	tMC.SetBranchAddress("MCTrack", &true_tracks); //True Track to compare

	TClonesArray* true_points = new TClonesArray("PndSdsMCPoint");
	tMC.SetBranchAddress("LMDPoint", &true_points); //True Points to compare

	//----------------------------------------------------------------------------------

	// histograms per plane
	//TH2* hist_xy             [nplanes];
	//TH1* hist_theta_in       [nplanes];

	TH2* hist_xz = new TH2F("hist_xz", "hist_xz", 150, 0, 150, 1000, -40, 40);
	TH2* hist_yz = new TH2F("hist_yz", "hist_yz", 150, 0, 150, 1000, -40, 40);
	TH2* hist_dxdz_z = new TH2F("hist_dxdz_z", "hist_dxdz_z", 150, 0, 150,
			60000, -1e-3, 51.e-3);
	TH2* hist_dydz_z = new TH2F("hist_dydz_z", "hist_dydz_z", 150, 0, 150,
			1000, -5e-3, 5e-3);
	TH2* hist_mom_z = new TH2F("hist_mom_z", "hist_mom_z", 150, 0, 150,
				3000, -1.e-5, 1.e-5);
	TH1* hist_n_events = new TH1F("hist_n_events", "hist_n_events", 150, 0, 150);

	//TCanvas temp_canvas("temp_canvas", "canvas for initialization", 100, 100);
	//temp_canvas.cd();
	// loop over planes
	//cout << " constructing histograms for " << nplanes << " planes with " << nsensors_per_plane << " sensors per plane " << endl;

	/*
	 for (int iplane = 0; iplane < nplanes; iplane++){
	 stringstream hist_name;
	 stringstream hist_title;

	 hist_name  << "hist_xy_plane_" << iplane;
	 hist_title << "xy hit distribution plane " << iplane;
	 hist_xy[iplane] = new TH2F(hist_name.str().c_str(), hist_title.str().c_str(),
	 100, -40, 40, 100, -40, 40);
	 hist_xy[iplane]->Draw();
	 hist_xy[iplane]->GetXaxis()->SetTitle("X [cm]");
	 hist_xy[iplane]->GetYaxis()->SetTitle("Y [cm]");

	 hist_name .str("");
	 hist_title.str("");

	 hist_name  << "hist_theta_init_plane_" << iplane;
	 hist_title << "initial #Theta distribution plane " << iplane;
	 hist_theta_in[iplane] = new TH1F(hist_name.str().c_str(), hist_title.str().c_str(),
	 150, 0, 15e-3);
	 hist_theta_in[iplane]->Draw();
	 hist_theta_in[iplane]->GetXaxis()->SetTitle("#Theta [rad]");
	 hist_theta_in[iplane]->GetYaxis()->SetTitle("entries");

	 }*/

	int nEvents = tMC.GetEntries();
	cout << " reading " << nEvents << " Events " << endl;
	// store the x and z measurements for a later fit of a graph
	vector<double> pos_x;
	vector<double> pos_z;
	map<int,int> hits_z; // count the hits per plane
	map<int,TH2*> hists_ang_acceptance; // angular acceptance as a function of z
	map<int,TH2*>::iterator hists_ang_acceptance_it, hists_ang_acceptance_itnorm;
	// create the histogram to normalize to.
	hists_ang_acceptance[-1] = new TH2F("hists_ang_acceptance", "generated angular distribution",200, -10, 10, 200, -10, 10);//, 200, 2., 10., 150., -3.141, 3.141);
	hists_ang_acceptance_itnorm = hists_ang_acceptance.find(-1);
	// optionally read existing values in order to create a mean graph
	// for several scenarios. If you want to combine values, put the
	// x_z_values_out.dat into the next folder to analyze and name it
	// there as x_z_values_in.dat
	ifstream x_z_values_in("x_z_values_in.dat");
	if (nEvents <= 1000 && x_z_values_in.is_open()){
		cout << " ************ reading x_z_values_in.dat ************ " << endl;
		double x, z;
		int nlines(0);
		while(x_z_values_in.good()){
			x_z_values_in >> x >> z;
			if (x_z_values_in.good()){
				pos_x.push_back(x);
				pos_z.push_back(z);
				nlines++;
			} else break;
		}
		x_z_values_in.close();
		cout << " read " << nlines << " lines "<< endl;
	}

	/*
	TString vmcWorkdir = gSystem->Getenv("VMCWORKDIR");
	TFile* file_beampipe(NULL);
	TGeoVolume* beampipe(NULL);
	FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
	FairGeoInterface* geoFace = geoLoad->getGeoInterface();
	geoFace->setMediaFile(vmcWorkdir + "/geometry/media_pnd.geo");
	geoFace->readMedia();
	geoFace->print();
	FairGeoMedia* geoMedia = geoFace->getMedia();
	FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
	if (geo_beampipe.size() > 0){
		 file_beampipe = new TFile(geo_beampipe.c_str(), "OPEN");
		 beampipe = (TGeoVolume*) file_beampipe->Get("pipeassembly");
		 TCanvas* _canvas = new TCanvas("_canvas", "_canvas", 800, 200);
		 if (beampipe) {
			 gGeoManager->SetTopVolume(beampipe);
			 gGeoManager->CloseGeometry();
			 beampipe->Draw("ogl");
			 _canvas->Update();
			 _canvas->Print("test.gif");
		 }
	}
	return;*/

	for (Int_t j = 0; j < nEvents ; j++) {
		DrawProgressBar(50, (j + 1) / ((double) nEvents));
		tMC.GetEntry(j);
		tdigiHits.GetEntry(j);
		const int nParticles = true_tracks->GetEntriesFast();

		int npbar = 0;
		for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
			// if(nParticles!=4) continue;
			PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(iParticle);
			Int_t mcID = mctrk->GetPdgCode();
			if (mctrk->IsGeneratorCreated()) {//mcID == -2212) {
				npbar++;
				//  if(nParticles!=4) cout<<"For event #"<<j<<" mcID="<<mcID<<endl;
				TVector3 momMC = mctrk->GetMomentum();
				hists_ang_acceptance[-1]->Fill(momMC.X()/momMC.Z()*1e3, momMC.Y()/momMC.Z()*1e3);
				//hists_ang_acceptance[-1]->Fill(MomMC.Theta()*1e3, MomMC.Phi());
			}
		}
		if (0 && npbar != 1)
			cout << "found " << npbar << " anti-protons" << endl;

		int nHits = true_points->GetEntriesFast();
		int nSdsHits = 0;
		for (Int_t iHit = 0; iHit < nHits; iHit++) {
			PndSdsMCPoint* mcpoint = (PndSdsMCPoint*) true_points->At(iHit);
			if (mcpoint->GetTrackID() < 0) continue;
			PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(
					mcpoint->GetTrackID());
			if (mctrk->IsGeneratorCreated()) { // take only anti-protons from the generator
				if (mcpoint) {
					TVector3 _mcpoint((mcpoint->GetPosition())); // position at the sensor entrance
					TVector3 _mctrack(mcpoint->GetPx(), mcpoint->GetPy(),
							mcpoint->GetPz()); // momentum of the track at the entrance

					TVector3 momMC = mctrk->GetMomentum(); // momentum in the primary vertex
					TVector3 posMC = mctrk->GetStartVertex(); // position of the primary vertex

					//det_id = mcpoint->GetDetectorID(); // store the detector id
					//sens_id = mcpoint->GetSensorID(); // store the sensor id
					// calculate the plane and sensor on this plane
					int plane = mcpoint->GetSensorID();

					if (plane < 0 || plane >= nplanes) {
						cout
								<< " warning: calculated plane or sensor is wrong: plane "
								<< plane << endl;
					} else {
						// is it an unknown plane so far?
						if (hists_ang_acceptance.find(plane) == hists_ang_acceptance.end()){
							stringstream hist_name;
							stringstream hist_title;
							hist_name << "hists_ang_acceptance_plane_" << plane;
							hist_title << "angular acceptance at z = " << plane*10. << " cm " << endl;
							hists_ang_acceptance[plane] = new TH2F(hist_name.str().c_str(), hist_title.str().c_str(),
									200, -10, 10, 200, -10, 10);//200, 2., 10., 150., -3.141, 3.141);
						}
						hists_ang_acceptance[plane]->Fill(momMC.X()/momMC.Z()*1e3, momMC.Y()/momMC.Z()*1e3);//(momMC.Theta()*1e3, momMC.Phi());
						if (j < 1000){
							pos_x.push_back(_mcpoint.X());
							pos_z.push_back(_mcpoint.Z());
						}
						hist_xz->Fill(plane, _mcpoint.X());
						hist_yz->Fill(plane, _mcpoint.Y());
						hist_dxdz_z->Fill(plane, _mctrack.X() / _mctrack.Z());
						hist_dydz_z->Fill(plane, _mctrack.Y() / _mctrack.Z());
						hist_mom_z->Fill(plane, (_mctrack.Mag()-momMC.Mag())/momMC.Mag());
						//hist_xy [plane]        ->Fill(x_in,y_in);
						//hist_theta_in [plane]        ->Fill(ptheta_in);
						hits_z[plane]++;
					}
					nSdsHits++;
				}
			}
		}
		if (0 && nHits > 0)
			cout << "found " << nHits << " hit(s) with " << nSdsHits
					<< " sds hit(s)" << endl;
	}

	ofstream x_z_values_out("x_z_values_out.dat");
	if (nEvents <= 1000 && x_z_values_out.is_open()){
		cout << " ************ writing x_z_values_out.dat ************ " << endl;
		for (unsigned int iline = 0; iline < pos_x.size(); iline++){
			x_z_values_out << pos_x[iline] << " " << pos_z[iline] << "\n";
		}
		x_z_values_out.close();
		cout << " wrote " << pos_x.size() << " lines "<< endl;
	}

	//std::sort(pos_x.begin(), pos_x.end());
	//std::sort(pos_z.begin(), pos_z.end());
	TGraph* graph_x_z = new TGraph( pos_x.size(), (double*)&pos_z[0], (double*)&pos_x[0]);
	graph_x_z->SetNameTitle("graph_x_z","x over z hits");
	//graph_x_z->SetMarkerStyle(4);
	graph_x_z->SetLineWidth(0);
	TF1* func_beampipe = new TF1("func_beampipe", function_beampipe, 0., pos_z[pos_z.size()-1], 3);
	// par[0]: where bending starts (dipole region)
	// par[1]: the bending radius
	// par[2]: the bending angle
	func_beampipe->SetParameters(361., 6000., 40.e-3);
	func_beampipe->SetParNames(
			"bending start  [cm]  ",
			"bending radius [cm]  ",
			"bend by phi    [rad] ");
	func_beampipe->FixParameter(2,40.0e-3);
	func_beampipe->FixParameter(0,361.);
	func_beampipe->FixParameter(1,5720.);
	gStyle->SetOptFit(111);

	TGaxis::SetMaxDigits(3);
	TCanvas* canvas1 = new TCanvas("canvas1", "x over z hits", 1200, 800);
	//canvas1->Divide(2,2);
	canvas1->cd(0);
	//hist_xz->Draw("COLZ");
	graph_x_z->Draw("AP");
	graph_x_z->Fit(func_beampipe);
	graph_x_z->GetXaxis()->SetTitle("z [cm]");
	graph_x_z->GetYaxis()->SetTitle("x [cm]");
	func_beampipe->Draw("same");
	cout << " x displacement at 11 m is " << func_beampipe->Eval(1100.) << endl;
	gPad->Update();
	//func_beampipe->Draw("same");
	canvas1->Print("beampipe_results.png");
	// create residual distribution
	TCanvas* canvas6 = new TCanvas("canvas6", "residuals", 1200, 800);
	TH2F* hist_res_x_z = new TH2F("hist_res_x_z", "residuals of x over z fit", 150, 0, 1500, 100, -0.1, 0.1);
	for (unsigned int iz = 0; iz < pos_z.size(); iz++){
		hist_res_x_z->Fill(pos_z[iz], pos_x[iz]-func_beampipe->Eval(pos_z[iz]));
	}
	hist_res_x_z->Draw("COLZ");
	hist_res_x_z->GetXaxis()->SetTitle("x [cm]");
	hist_res_x_z->GetYaxis()->SetTitle("x - x_{fit} [cm]");
	canvas6->Print("beampipe_results.ps(");

	TCanvas* canvas2 = new TCanvas("canvas2", "canvas2", 1200, 800);
	//canvas2->Divide(2,2);
	canvas2->cd(0);
	hist_yz->Draw("COLZ");
	canvas2->Print("beampipe_results.ps(");
	TCanvas* canvas3 = new TCanvas("canvas3", "canvas3", 1200, 800);
	//canvas3->Divide(2,2);
	canvas3->cd(0);
	hist_dxdz_z->Draw("COLZ");
	canvas3->Print("beampipe_results.ps(");
	hist_dxdz_z->GetYaxis()->SetRangeUser(40.e-3, 40.05e-3);
	hist_dxdz_z->Draw("COLZ");
	canvas3->Print("beampipe_results.ps(");
	TCanvas* canvas4 = new TCanvas("canvas4", "canvas4", 1200, 800);
	//canvas4->Divide(2,2);
	canvas4->cd(0);
	hist_dydz_z->Draw("COLZ");
	canvas4->Print("beampipe_results.ps(");
	TCanvas* canvas5 = new TCanvas("canvas5", "canvas5", 1200, 800);
	//canvas5->Divide(2,2);
	canvas5->cd(0);
	hist_mom_z->Draw("COLZ");
	canvas5->Print("beampipe_results.ps(");
	TCanvas* canvas7 = new TCanvas("canvas6", "canvas6", 1200, 400);
	gPad->SetLeftMargin(0.1);
	gPad->SetRightMargin(0.05);
	canvas7->cd(0);
	// check the acceptance of the beam pipe as a function of z
	// normalize to the plane before
	map<int, int>::iterator hits_z_it, hits_z_it_next;
	for (hits_z_it = hits_z.begin(); hits_z_it != hits_z.end(); hits_z_it++){
		hits_z_it_next = hits_z_it;
		hits_z_it_next++;
		if (hits_z_it_next == hits_z.end()) break;
		if (hits_z_it->second == 0) continue;
		if (hits_z_it_next->first != hits_z_it->first+1){
			cout << " sorry, entries are not sorted! " << hits_z_it_next->first << " != " << hits_z_it->first << " +1 "<< endl;
			continue;
		}
		hist_n_events->Fill(hits_z_it_next->first, ((double)hits_z_it_next->second)/((double)hits_z_it->second));
	}
	hist_n_events->GetXaxis()->SetTitle("z [dm]");
	hist_n_events->GetYaxis()->SetTitle("relative acceptance");
	hist_n_events->Draw("hist text");
	canvas7->Print("beampipe_results.ps(");

	TCanvas* canvas8 = new TCanvas("canvas8", "canvas8", 800, 400);
	canvas8->Divide(2,1);
	canvas8->cd(1);
	gPad->SetLeftMargin(0.16);
	gPad->SetRightMargin(0.16);
	canvas8->cd(1);
	hists_ang_acceptance_itnorm->second->GetXaxis()->SetTitle("dX/dZ x 10^{3}");
	hists_ang_acceptance_itnorm->second->GetYaxis()->SetTitle("dY/dZ x 10^{3}");
	hists_ang_acceptance_itnorm->second->Draw("COLZ");
	canvas8->cd(2);
	gPad->SetLeftMargin(0.16);
	gPad->SetRightMargin(0.16);
	system("rm beampipe_acceptance*.gif");

	if (1){
		TImage *img = TImage::Open("/home/jasinski/bin/pandaroot/macro/lmd/Promme/beampipe.png");
		int width = 1000;
		double pic_width = img->GetWidth();
		int height = floor(img->GetHeight()*width/pic_width);
		TCanvas* canvas = new TCanvas("canvas_acceptance", "canvas_acceptance", width, height*2);
		gPad->Range(0, 0, width, height*2.);
		canvas->Divide(1,2);
		canvas->cd(1);
		gPad->Range(0, 0., 1., 1.);
		canvas->cd(2);
		gPad->Range(0, 0, width, height);
		double factor = 1./(-460.-1294.); // inserting ranges of the beampipe
		double ratio = height/(double)width;
		TPolyLine *arc = Get_PolyLine_circle(0,0,8);
		arc->SetLineColor(3);
		arc->SetLineWidth(2);
	// check the acceptance of the beam pipe as a function of z
	// normalize to the plane -1
	for (hists_ang_acceptance_it = hists_ang_acceptance.begin();
			hists_ang_acceptance_it != hists_ang_acceptance.end();
			hists_ang_acceptance_it++){\
		if (hists_ang_acceptance_it == hists_ang_acceptance_itnorm){
			continue;
		}
		canvas->cd(1);
		gPad->Clear();
		img->Draw("xxx");
		double x = factor*(hists_ang_acceptance_it->first*10. - 1294.);
		cout << "drawing x = " << hists_ang_acceptance_it->first*10. << " cm at " << x << endl;
		TLine line(x, 0., x, 1.);
		line.SetLineColor(2);
		line.SetLineWidth(2);
		line.Draw();
		gPad->Update();
		hists_ang_acceptance_it->second->Divide(hists_ang_acceptance_itnorm->second);
		hists_ang_acceptance_it->second->SetMaximum(1.);
		hists_ang_acceptance_it->second->GetXaxis()->SetTitle("dX/dZ x 10^{3}");
		hists_ang_acceptance_it->second->GetYaxis()->SetTitle("dY/dZ x 10^{3}");
		if (x-ratio/2. > 0){
			canvas->cd(2);
			TPad clonepad("insertpad","insertpad",x-ratio/2.,0.,x+ratio/2.,1., 0, 1, 1);
			clonepad.Draw();
			clonepad.cd();
			gPad->SetLeftMargin(0.16);
			gPad->SetRightMargin(0.16);
			hists_ang_acceptance_it->second->Draw("COLZ");
			gPad->Update();
			//sleep(1);
			//canvas->Print("beampipe_results.ps(");
			canvas->Print("beampipe_acceptance.gif+");
		}
		canvas8->cd(2);
		hists_ang_acceptance_it->second->Draw("COLZ");
		arc->Draw();
		canvas8->Update();
		//canvas->Print("beampipe_results.ps(");
		canvas8->Print("beampipe_acceptance_hists.gif+");
	}
	}
	canvas8->Clear();

	canvas8->Print("beampipe_results.ps)");


	// convert to pdf
	cout << "converting to pdf " << system("ps2pdf beampipe_results.ps")
			<< endl;
	cout << "deleting ps file  " << system("rm beampipe_results.ps") << endl;

	output_histfile->Write();

	output_histfile->Close();
  return 0;
}

void DrawProgressBar(int len, double percent) {
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

Double_t function_beampipe(Double_t *x, Double_t *par)
{
	double pos_z = x[0];
	double result = 0.;
	double end_seg_upstream = par[0]; // where bending starts with
	double r_bend = par[1]; // a radius
	double phi_bend = par[2]; // and the angle of the circle path
	// the rest is fully determined
	double end_seg_bend = end_seg_upstream+sin(phi_bend)*r_bend;
	// the first straight part
	if (pos_z < end_seg_upstream){
		result = 0.;
	}
	// bending part is a part of a circle
	if (end_seg_upstream <= pos_z && pos_z < end_seg_bend){
		result = r_bend - cos(atan((pos_z-end_seg_upstream)/r_bend))*r_bend;
	}
	// straight part behind the dipole is a tangent to the circle
	if (end_seg_bend <= pos_z){
		result = (pos_z - end_seg_upstream - tan(phi_bend/2.)*r_bend)*tan(phi_bend);
	}
	return result;
}

TPolyLine* Get_PolyLine_circle(float x,float y, float r){
    const int nsegments = 360;
	Double_t _x[nsegments+1] = {0.,}; // +1 : Endpoint is Startpoint
    Double_t _y[nsegments+1] = {0.,};
    for (int i = 0; i <= nsegments; i++){
    	double angle = 2*TMath::Pi()*((double) i)/((double) nsegments);
    	_x[i] = r*sin(angle)+x;
    	_y[i] = r*cos(angle)+y;
    }
    TPolyLine *pline_circle = new TPolyLine(nsegments+1,_x,_y);
    return pline_circle;
}

int main() {
	//TApplication myapp("myapp", 0, 0);
	Check_particle_path();
	//myapp.Run();
	return 0;
}
