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
//#include<FairGeane.h>
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
//#include<FairGeoLoader.h>
//#include<FairGeoInterface.h>
//#include<FairGeoBuilder.h>
//#include<FairGeoPcon.h>
//#include<FairGeoMedia.h>
//#include<TGeoManager.h>
#include <TImage.h>
#include <TLine.h>
#include <stdlib.h>
#include <TPolyLine.h>
#include <PndLmdDim.h>

using namespace std;

double last_percent(-1.);
// draw a progress bar only when the length changes significantly
void DrawProgressBar(int len, double percent);

void scattered_particles() {
	cout << " analysis tool vers. 1.0 " << endl;

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
	TString out = storePath + "/analysis_results";
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


	PndLmdDim* lmddim = PndLmdDim::Instance();
	lmddim->Read_transformation_matrices();
	const unsigned int nplanes = lmddim->n_planes;
	TH2D* hist_all_hits[nplanes];
	TH2D* hist_sec_hits[nplanes];
	TH2D* hist_prim_hits[nplanes];
	TH2D* hist_gen_events = new TH2D("hist_gen_events", "generated events", 100, 0., 10, 100, -3.1416, 3.1416);
	TH2D* hist_acc_events = new TH2D("hist_acc_events", "accepted events", 100, 0., 10, 100, -3.1416, 3.1416);

	TH2D* hist_sec_hit_theta[nplanes];
	TH2D* hist_sec_hit_z[nplanes];

	TH2D* hist_sec_hit_edep[nplanes];
	TH2D* hist_prim_hit_edep[nplanes];

	TH2D* hist_angle_vs_origin[nplanes];
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		stringstream name;
		stringstream title;
		name << "hist_all_hits_plane_"<< iplane;
		title << "Hit distribution of all hits at plane " << iplane;
		hist_all_hits[iplane] = new TH2D(name.str().c_str(), title.str().c_str(), 100, -10, 10, 100, -10, 10);
		name.str("");
		title.str("");
		name << "hist_sec_hits_plane_"<< iplane;
		title << "Hit distribution of secondary hits at plane " << iplane;
		hist_sec_hits[iplane] = new TH2D(name.str().c_str(), title.str().c_str(), 100, -10, 10, 100, -10, 10);
		name.str("");
		title.str("");
		name << "hist_prim_hits_plane_"<< iplane;
		title << "Hit distribution of primary hits at plane " << iplane;
		hist_prim_hits[iplane] = new TH2D(name.str().c_str(), title.str().c_str(), 100, -10, 10, 100, -10, 10);
		name.str("");
		title.str("");
		name << "hist_sec_hit_theta_plane_"<< iplane;
		title << "track angle distribution of secondary hits at plane " << iplane;
		hist_sec_hit_theta[iplane] = new TH2D(name.str().c_str(), title.str().c_str(), 100, 0, 1, 50, 0, 10);
		name.str("");
		title.str("");
		name << "hist_sec_hit_origin_plane_"<< iplane;
		title << "track origin z distribution of secondary hits at plane " << iplane;
		hist_sec_hit_z[iplane] = new TH2D(name.str().c_str(), title.str().c_str(), 50, -50, 50, 50, 0, 10);

		name.str("");
		title.str("");
		name << "hist_sec_hit_edep_plane_"<< iplane;
		title << "secondary edep at plane " << iplane;
		hist_sec_hit_edep[iplane] = new TH2D(name.str().c_str(), title.str().c_str(), 100, 0, 30, 50, 0, 10);

		name.str("");
		title.str("");
		name << "hist_prim_hit_edep_plane_"<< iplane;
		title << "primary edep at plane " << iplane;
		hist_prim_hit_edep[iplane] = new TH2D(name.str().c_str(), title.str().c_str(), 100, 0, 30, 50, 0, 10);

		name.str("");
		title.str("");
		name << "hist_angle_vs_origin_plane_"<< iplane;
		title << "hit angle vs track origin z " << iplane;
		hist_angle_vs_origin[iplane] = new TH2D(name.str().c_str(), title.str().c_str(), 200, -20, 50, 100, 0, 1);
	}


	int nEvents = tMC.GetEntries();
	cout << " reading " << nEvents << " Events " << endl;

	for (Int_t j = 0; j < nEvents ; j++) {
		DrawProgressBar(50, (j + 1) / ((double) nEvents));
		tMC.GetEntry(j);
		tdigiHits.GetEntry(j);
		const int nParticles = true_tracks->GetEntriesFast();

		PndMCTrack* primary_track = NULL;

		int npbar = 0;
		for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
			// if(nParticles!=4) continue;
			PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(iParticle);
			Int_t mcID = mctrk->GetPdgCode();
			if (mctrk->IsGeneratorCreated()) {//mcID == -2212) {
				npbar++;
				primary_track = mctrk;
				hist_gen_events->Fill(mctrk->GetMomentum().Theta()*1e3, mctrk->GetMomentum().Phi());
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
			if (mcpoint) {
				TVector3 _mcpoint((mcpoint->GetPosition())); // position at the sensor entrance
				TVector3 _mctrack(mcpoint->GetPx(), mcpoint->GetPy(),
						mcpoint->GetPz()); // momentum of the track at the entrance

				TVector3 momMC = mctrk->GetMomentum(); // momentum in the primary vertex
				TVector3 posMC = mctrk->GetStartVertex(); // position of the primary vertex

				const TVector3& mcpoint_lmd = lmddim->Transform_global_to_lmd_local(_mcpoint);
				const TVector3& mctrack_lmd = lmddim->Transform_global_to_lmd_local(_mctrack, true);

				int sensID = mcpoint->GetSensorID();
				int ihalf, iplane, imodule, iside, idie, isensor;
				lmddim->Get_sensor_by_id(sensID, ihalf, iplane, imodule, iside, idie, isensor);
				nSdsHits++;

				double x = mcpoint_lmd.X();
				double y = mcpoint_lmd.Y();
				double r = sqrt(x*x+y*y);
				double Eloss = mcpoint->GetEnergyLoss()*1e6;
				const TVector3& posMC_lmd = lmddim->Transform_global_to_lmd_local(posMC);
				double startpos = posMC_lmd.Z();
				double startr = sqrt(posMC_lmd.X()*posMC_lmd.X()+posMC_lmd.Y()*posMC_lmd.Y());
				double trackangle = mctrack_lmd.Theta();

				if (Eloss > 5.5){
					if (startr < 3.6) hist_all_hits[iplane]->Fill(x, y);
					if (startr < 3.6) hist_angle_vs_origin[iplane]->Fill(startpos, trackangle);
				}
				if (mctrk->IsGeneratorCreated()) { // take only anti-protons from the generator
					hist_prim_hit_edep[iplane]->Fill(Eloss,r);
					if (Eloss > 5.5){ // minimal E deposit for protons to be detected in HV-MAPS (by T.Weber)
						if (startr < 3.6) hist_prim_hits[iplane]->Fill(x, y);
						if (primary_track)
							hist_acc_events->Fill(primary_track->GetMomentum().Theta()*1e3, primary_track->GetMomentum().Phi());
					}
				} else { // it is a secondary
					hist_sec_hit_edep[iplane]->Fill(Eloss,r);
					if (Eloss > 5.5){
						if (startr < 3.6) hist_sec_hits[iplane]->Fill(x, y);
						if (startr < 3.6) hist_sec_hit_theta[iplane]->Fill(trackangle, r);
						if (startr < 3.6) hist_sec_hit_z[iplane]->Fill(startpos, r);
					}
				}
			}
		}
		if (0 && nHits > 0)
			cout << "found " << nHits << " hit(s) with " << nSdsHits
					<< " sds hit(s)" << endl;
	}

	output_histfile->cd();

	TCanvas canvas("canvas", "Results", 800, 800);
	canvas.SetMargin(0.15, 0.15, 0.15, 0.15);
	canvas.cd();
	hist_gen_events->Draw("COLZ");
	gPad->Update();
	hist_gen_events->GetXaxis()->SetTitle("#theta / mrad");
	hist_gen_events->GetYaxis()->SetTitle("#phi / phi");
	canvas.Print("Secondary_results.ps(");
	hist_acc_events->Divide(hist_gen_events);
	hist_acc_events->Draw("COLZ");
	gPad->Update();
	hist_acc_events->GetXaxis()->SetTitle("#theta / mrad");
	hist_acc_events->GetYaxis()->SetTitle("#phi / rad");
	canvas.Print("Secondary_results.ps(");

	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		hist_all_hits[iplane]->Draw("COLZ");
		gPad->Update();
		hist_all_hits[iplane]->GetXaxis()->SetTitle("x[cm]");
		hist_all_hits[iplane]->GetYaxis()->SetTitle("y[cm]");
		canvas.Print("Secondary_results.ps(");
		hist_all_hits[iplane]->Write();
	}
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		hist_sec_hits[iplane]->Draw("COLZ");
		gPad->Update();
		hist_sec_hits[iplane]->GetXaxis()->SetTitle("x[cm]");
		hist_sec_hits[iplane]->GetYaxis()->SetTitle("y[cm]");
		hist_sec_hits[iplane]->Write();
		canvas.Print("Secondary_results.ps(");
	}
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		hist_prim_hits[iplane]->Draw("COLZ");
		gPad->Update();
		hist_prim_hits[iplane]->GetXaxis()->SetTitle("x[cm]");
		hist_prim_hits[iplane]->GetYaxis()->SetTitle("y[cm]");
		hist_prim_hits[iplane]->Write();
		canvas.Print("Secondary_results.ps(");
	}
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		hist_sec_hit_theta[iplane]->Draw("COLZ");
		gPad->Update();
		hist_sec_hit_theta[iplane]->GetXaxis()->SetTitle("#theta[rad]");
		hist_sec_hit_theta[iplane]->GetYaxis()->SetTitle("r[cm]");
		hist_sec_hit_theta[iplane]->Write();
		canvas.Print("Secondary_results.ps(");
	}
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		hist_sec_hit_z[iplane]->Draw("COLZ");
		gPad->Update();
		hist_sec_hit_z[iplane]->GetXaxis()->SetTitle("z[cm]");
		hist_sec_hit_z[iplane]->GetYaxis()->SetTitle("r[cm]");
		hist_sec_hit_z[iplane]->Write();
		canvas.Print("Secondary_results.ps(");
	}
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		hist_sec_hit_edep[iplane]->Draw("COLZ");
		gPad->Update();
		hist_sec_hit_edep[iplane]->GetXaxis()->SetTitle("Eloss[keV]");
		hist_sec_hit_edep[iplane]->GetYaxis()->SetTitle("r[cm]");
		hist_sec_hit_edep[iplane]->Write();
		canvas.Print("Secondary_results.ps(");
	}
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		hist_prim_hit_edep[iplane]->Draw("COLZ");
		gPad->Update();
		hist_prim_hit_edep[iplane]->GetXaxis()->SetTitle("Eloss[keV]");
		hist_prim_hit_edep[iplane]->GetYaxis()->SetTitle("r[cm]");
		hist_prim_hit_edep[iplane]->Write();
		canvas.Print("Secondary_results.ps(");
	}
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		stringstream name_prim;
		name_prim << "hist_prim_hit_edep_projX_plane_" << iplane;
		TH1D* hist_prim = hist_prim_hit_edep[iplane]->ProjectionX(name_prim.str().c_str());
		stringstream name_sec;
		name_sec << "hist_sec_hit_edep_projX_plane_" << iplane;
		TH1D* hist_sec = hist_sec_hit_edep[iplane]->ProjectionX(name_sec.str().c_str());
		hist_prim->DrawNormalized();
		hist_sec->DrawNormalized("same");
		gPad->Update();
		hist_prim->Write();
		hist_sec->Write();
		canvas.Print("Secondary_results.ps(");
	}
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		hist_angle_vs_origin[iplane]->Draw("COLZ");
		gPad->Update();
		hist_angle_vs_origin[iplane]->GetXaxis()->SetTitle("z[cm]");
		hist_angle_vs_origin[iplane]->GetYaxis()->SetTitle("#theta[rad]");
		hist_angle_vs_origin[iplane]->Write();
		canvas.Print("Secondary_results.ps(");
	}
	// normalize
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		stringstream name;
		stringstream title;
		name << "hist_sec_hist_norm_prim_plane_" << iplane;
		title << "Secondary hit distribution normalized to primaries at plane " << iplane;
		TH2D* _hist = new TH2D(*hist_sec_hits[iplane]);
		_hist->SetNameTitle(name.str().c_str(), title.str().c_str());
		_hist->Divide(hist_prim_hits[iplane]);
		_hist->Draw("COLZ");
		_hist->SetMaximum(.2);
		gPad->Update();
		_hist->Write();
		canvas.Print("Secondary_results.ps(");
	}
	for (unsigned int iplane = 0; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		stringstream name;
		stringstream title;
		name << "hist_sec_hist_norm_all_plane_" << iplane;
		title << "Secondary hit distribution normalized to all hits at plane " << iplane;
		TH2D* _hist = new TH2D(*hist_sec_hits[iplane]);
		_hist->SetNameTitle(name.str().c_str(), title.str().c_str());
		_hist->Divide(hist_all_hits[iplane]);
		_hist->Draw("COLZ");
		_hist->SetMaximum(.2);
		gPad->Update();
		_hist->Write();
		canvas.Print("Secondary_results.ps(");
	}
	// normalize
	for (unsigned int iplane = 1; iplane < nplanes; iplane++){
		canvas.cd();
		gPad->Clear();
		stringstream name;
		stringstream title;
		name << "hist_sec_hist_norm_plane0_plane_" << iplane;
		title << "Secondary hit distribution normalized to first plane at plane " << iplane;
		TH2D* _hist = new TH2D(*hist_sec_hits[iplane]);
		_hist->SetNameTitle(name.str().c_str(), title.str().c_str());
		_hist->Divide(hist_sec_hits[0]);
		_hist->Draw("COLZ");
		//_hist->SetMaximum(.2);
		gPad->Update();
		_hist->Write();
		canvas.Print("Secondary_results.ps(");
	}
	gPad->Clear();
	canvas.Print("Secondary_results.ps)");

	cout << " converting ps to pdf " << endl;
	system("ps2pdf Secondary_results.ps");
	cout << " done " << endl;

	//output_histfile->Write();

	output_histfile->Close();
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

int main() {
	//TApplication myapp("myapp", 0, 0);
	scattered_particles();
	//myapp.Run();
	return 0;
}
