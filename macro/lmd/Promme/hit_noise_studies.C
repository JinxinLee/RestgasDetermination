#include <iostream>

#include<TCanvas.h>
#include<TApplication.h>
#include<TROOT.h>
#include<TChain.h>
#include<TStyle.h>
#include<TGaxis.h>
#include<TColor.h>
#include<TClonesArray.h>
#include<TH1.h>
#include<TH2.h>

#include<PndLmdDim.h>
//#include<PndMCTrack.h>
#include<PndTrack.h>
#include<PndSdsMCPoint.h>
#include<PndSdsDigiPixel.h>
#include<PndSdsHit.h>
#include<PndSdsMergedHit.h>
#include<FairTrackParH.h>

using namespace std;

// change some stuff in the ROOT appearance
void Root_Appearance();

double last_percent(-1.);
// draw a progress bar only when the length changes significantly
void DrawProgressBar(int len, double percent);

// the application
// Assuming to find an mc file called
// ./Lumi_MC_0.root
void hit_noise_studies();

int main() {
	Root_Appearance();
	TApplication myapp("myapp", 0, 0);
	hit_noise_studies();
	myapp.Run();
	return 0;
}

void hit_noise_studies(){
	cout << " lmd hit noise studies " << endl;
	TCanvas* canvas = new TCanvas("canvas", "canvas", 900, 600);
	canvas->Divide(3,2);

	// load the MC File
	TChain tMC("cbmsim");
	tMC.Add("./Lumi_digi_0.root");

	TChain thits("cbmsim");
	thits.Add("./Lumi_recoMerged_0.root");

	TChain ttrk("cbmsim");
	ttrk.Add("./Lumi_TrackNotFiltered_0.root");

	TChain ttrk_filt("cbmsim");
	ttrk_filt.Add("./Lumi_Track_0.root");

	bool usefiltered(true);
	TChain ttrk_prop("cbmsim");
	if (usefiltered)
		ttrk_prop.Add("./Lumi_GeaneFiltered_0.root");
	else
		ttrk_prop.Add("./Lumi_Geane_0.root");


	//--- assign MC info -----------------------------------------------------
	//TClonesArray* true_tracks = new TClonesArray("PndSdsDigiPixel");
	//tMC.SetBranchAddress("PndSdsDigiPixel", &true_tracks); //True Track to compare

	TClonesArray* digis = new TClonesArray("PndSdsDigiPixel");
	tMC.SetBranchAddress("LMDPixelDigis", &digis); //hits

	TClonesArray* hits = new TClonesArray("PndSdsMergedHit");
	thits.SetBranchAddress("LMDHitsMerged", &hits); //True Track to compare

	TClonesArray* tracks = new TClonesArray("PndTrack");
	ttrk.SetBranchAddress("LMDPndTrack", &tracks);

	TClonesArray* tracks_filt = new TClonesArray("PndTrack");
	ttrk_filt.SetBranchAddress("LMDPndTrack", &tracks_filt);

	TClonesArray* tracks_prop = new TClonesArray("FairTrackParH");
	if (usefiltered){
		ttrk_prop.SetBranchAddress("LMDCleanTrack", &tracks_prop);
		cout << " using geanie filtered tracks " << endl;
	} else
		ttrk_prop.SetBranchAddress("GeaneTrackFinal", &tracks_prop);


	//TClonesArray* tracks = new TClonesArray("LMDPndTrack");
	//tMC.SetBranchAddress("LMDPndTrack", &tracks);

	// get the lmd dim help class
	PndLmdDim& lmddim = PndLmdDim::Get_instance();

	// setup histograms
	/*
	TH1F* hist_eloss_total = new TH1F("hist_eloss_total", "energy deposit in 50#mum x 2cm x2cm sensors", 200, 0, 60);
	TH1F* hist_eloss = new TH1F("hist_eloss", "energy deposit scaled to 1#mum", 200, 0, 1200);
	hist_eloss_total->SetXTitle("E_{dep} [keV]");
	hist_eloss->SetXTitle("E_{dep} [eV]");

	TH2* plane3_hits = new TH2F("plane3_hits", "hits on plane 3", 200, -10, 10, 200, -10, 10); //lmddim.Get_histogram_Plane(0, 0);
	plane3_hits->SetXTitle("X [cm]");
	plane3_hits->SetYTitle("Y [cm]");
	plane3_hits->SetZTitle("# hits");
	TH2Poly* plane3_Edep = lmddim.Get_histogram_Plane(3, 0);
	plane3_Edep->SetNameTitle("plane3_Edep", "E dep on plane 3");
	plane3_Edep->SetZTitle("E_{dep} [J]");
	TH2Poly* plane3_rate = lmddim.Get_histogram_Plane(3, 0);
	plane3_rate->SetNameTitle("plane3_rate", "rate on plane 3");
	plane3_rate->SetZTitle("rate [kHz]");
	TH2Poly* plane3_rad = lmddim.Get_histogram_Plane(3, 0);
	plane3_rad->SetNameTitle("plane3_rad", "IEL dose on plane 3");
	plane3_rad->SetZTitle("IEL dose [Gy/a]");
*/
	TH2D* hist_track_dir = new TH2D("hist_track_dir", "track dir", 500, -200, 200, 500, -200, 200);
	TH2D* hist_track_dir_filt = new TH2D("hist_track_dir_filt", "track dir filtered", 500, -200, 200, 500, -200, 200);
	TH2D* hist_track_dir_prop = new TH2D("hist_track_dir_prop", "track dir propagated", 50, -20, 20, 50, -20, 20);
	TH1D* hist_track_theta = new TH1D("hist_track_theta", "#theta distr", 50, 0, 15);
	//	TH2D* hist_hit_mult = new TH2D("hist_hit_mult", "hit multiplicity", 100, 0, 100, 4, 0, 4);
	TH2D* hist_hit_mult = new TH2D("hist_hit_mult", "hit multiplicity", 1000, 0, 1000, 4, 0, 4);
	TH1D* hist_trk_mult = new TH1D("hist_trk_mult", "track multiplicity", 100, 0, 100);
	TH2D* hist_hit_distr = new TH2D("hist_hit_distr", "hit distribution", 100, -10, 10, 100, -10, 10);
	// read events and analyze
	int nEvents = tMC.GetEntries();
	int nEvents_counted(0);
	int nhitstotal(0);
	int ntrackstotal(0);
	int ntrackstotal_filt(0);
	int ntrackstotal_prop(0);
	int ntrackstotal_rel(0);

	cout << " reading " << nEvents << " Events " << endl;

	int real_hits(0);
	int noise_hits(0);
	for (Int_t j = 0; j < nEvents ; j++) {
		nEvents_counted++;
		DrawProgressBar(50, (j + 1) / ((double) nEvents));
		tMC.GetEntry(j);
		ttrk.GetEntry(j);
		ttrk_filt.GetEntry(j);
		ttrk_prop.GetEntry(j);
		thits.GetEntry(j);
		ttrk_prop.GetEntry(j);

		int nTracks = tracks->GetEntriesFast();
		hist_trk_mult->Fill(nTracks);
		//cout << " reading " << nTracks << " tracks" << endl;
		for (auto iTrack = 0; iTrack < nTracks; iTrack++){
			ntrackstotal++;
			PndTrack* track = (PndTrack*) tracks->At(iTrack);
			if (track){
				double pxpz = track->GetParamFirst().GetPx()/track->GetParamFirst().GetPz()*1e3;
				pxpz -= 40;
				double pypz = track->GetParamFirst().GetPy()/track->GetParamFirst().GetPz()*1e3;
				hist_track_dir->Fill(pxpz, pypz);
				double theta2 = pxpz*pxpz + pypz*pypz;
				if ((2*2 < theta2 && theta2 < 12*12)){
					ntrackstotal_rel++;
					//hist_track_dir->Fill(pxpz, pypz);
				}
			}
		}

		nTracks = tracks_filt->GetEntriesFast();
		//cout << " reading " << nTracks << " tracks" << endl;
		for (auto iTrack = 0; iTrack < nTracks; iTrack++){
			ntrackstotal_filt++;
			PndTrack* track = (PndTrack*) tracks->At(iTrack);
			if (track){
				double pxpz = track->GetParamFirst().GetPx()/track->GetParamFirst().GetPz()*1e3;
				pxpz -= 40;
				double pypz = track->GetParamFirst().GetPy()/track->GetParamFirst().GetPz()*1e3;
				hist_track_dir_filt->Fill(pxpz, pypz);
			}
		}

		nTracks = tracks_prop->GetEntriesFast();
		//cout << " reading " << nTracks << " tracks" << endl;
		for (auto iTrack = 0; iTrack < nTracks; iTrack++){
			FairTrackParH* track = (FairTrackParH*) tracks_prop->At(iTrack);
			if (track){
			  if(track->GetLambda()!=0){ //ignore not propagated tracks
			    ntrackstotal_prop++;
			    double pxpz = track->GetPx()/track->GetPz()*1e3;
			    double pypz = track->GetPy()/track->GetPz()*1e3;
			    hist_track_dir_prop->Fill(pxpz, pypz);
			    double theta = sqrt(pxpz*pxpz+pypz*pypz);
			    hist_track_theta->Fill(theta);
			  }
			}
		}

		int nHits = digis->GetEntriesFast();
		int nSdsHits = 0;
		//cout << " reading " << nHits << " digis " << endl;
		int ihits[4] = {0,0,0,0};
		for (Int_t iHit = 0; iHit < nHits; iHit++) {
			nhitstotal++;
			PndSdsDigiPixel* mcpoint = (PndSdsDigiPixel*) digis->At(iHit);
			//hist_hit_distr->Fill(mcpoint->Get);
			//if (mcpoint-> < 0)
				noise_hits++;
			//else {
			//	real_hits++;
			//}
				int ihalf, iplane, imodule, iside, idie, isens;
				int sensid = mcpoint->GetSensorID();
				lmddim.Get_sensor_by_id(sensid, ihalf, iplane, imodule, iside, idie, isens);

				ihits[iplane]++;
			// cout << mcpoint->GetEnergyLoss() << endl;
			//hist_eloss_total->Fill(mcpoint->GetEnergyLoss()*1.e6); // in keV
			//hist_eloss->Fill(mcpoint->GetEnergyLoss()*1.e9/50.); // in eV
/*
			int sensID = mcpoint->GetSensorID();
			int ihalf, iplane, imodule, iside, idie, isensor;
			lmddim.Get_sensor_by_id(sensID, ihalf, iplane, imodule, iside, idie, isensor);
			TVector3 hit_pos = mcpoint->GetPosition();
			TVector3 hit_pos_lmd = lmddim.Transform_global_to_lmd_local(hit_pos, false);

			cout << mcpoint->GetTime() << endl;
			//cout << mcpoint->Get << endl << endl;

			if (iplane == 3 && iside == 0){
				//plane3_hits->Fill(hit_pos_lmd.X(), hit_pos_lmd.Y());
				//plane3_rate->Fill(hit_pos_lmd.X(), hit_pos_lmd.Y());
				//plane3_Edep->Fill(hit_pos_lmd.X(), hit_pos_lmd.Y(), mcpoint->GetEnergyLoss()*1.e9*1.602e-19); // in J
				//plane3_rad->Fill(hit_pos_lmd.X(), hit_pos_lmd.Y(), mcpoint->GetEnergyLoss()*1.e9*1.602e-19); // in J
			}

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
				}
			} */
		}
		hist_hit_mult->Fill(ihits[0], 0);
		hist_hit_mult->Fill(ihits[1], 1);
		hist_hit_mult->Fill(ihits[2], 2);
		hist_hit_mult->Fill(ihits[3], 3);

		nHits = hits->GetEntriesFast();
		for (auto iHit = 0; iHit < nHits; iHit++){
			PndSdsMergedHit* hit = (PndSdsMergedHit*) hits->At(iHit);
			int sensID = hit->GetSensorID();
			int ihalf, iplane, imodule, iside, idie, isensor;
			lmddim.Get_sensor_by_id(sensID, ihalf, iplane, imodule, iside, idie, isensor);
			TVector3 hit_pos = hit->GetPosition();
			TVector3 hit_pos_lmd = lmddim.Transform_global_to_lmd_local(hit_pos, false);
			hist_hit_distr->Fill(hit_pos_lmd.X(), hit_pos_lmd.Y());
		}
	}
	// cross section for N generated events at 1.5 GeV/c momentum was
	// sigm_coul = 36.683 mbarn
	// sigm_had_el = 25.3021 mbarn
	// sigm_inel = 62.71 mbarn according to Anastasias note
	// -> sig_total = 124.63 mbarn or 124.63 x 10-27 cm^-2
	//double sig_total = 124.63e-27;//62e-27;//124.63e-27; // cm^2
	// the mean instantanious luminosity at high luminosity mode is 1.6 x 10^32
	// but we use here for reference 1 x 10^32 which can be scaled easily to any other number
	//double lumi_ref = 1e32; // cm^-2 s^-1
	// therefore we simulated an interaction rate of
	//double rate_total = sig_total * lumi_ref; // 1/s or Hz
	//cout << "\n The total rate at 1.5 GeV/c and " << lumi_ref << " mean luminosity to normalize to is " << rate_total << " Hz" << endl;
	// The simulated time scale is then
	//double tsim = nEvents_counted / rate_total; // s
	cout << nEvents_counted << " events " << endl;// correspond to a real time of " << tsim << " s " << endl;
	cout << " in " << nhitstotal << " hits found " << real_hits << " real hits and " << noise_hits << " noise hits " << endl;
	cout << " the ratio over nevents is " << (double) noise_hits / (double) nEvents_counted << endl;
	cout << " found " << ntrackstotal << " tracks " << (double) ntrackstotal/(double) nEvents_counted << endl;
	cout << " found " << ntrackstotal_rel << " tracks relevant for analysis " << (double) ntrackstotal_rel/(double) nEvents_counted << endl;
	cout << " found " << ntrackstotal_filt << " filtered tracks " << (double) ntrackstotal_filt/(double) nEvents_counted << endl;
	cout << " found " << ntrackstotal_prop << " propagated tracks " << (double) ntrackstotal_prop/(double) nEvents_counted << endl;

	canvas->cd(1);
	hist_track_dir->Draw("COLZ");
	hist_track_dir->SetXTitle("p_{x}/p_{z}x10^{-3}");
	hist_track_dir->SetYTitle("p_{y}/p_{z}x10^{-3}");

	canvas->cd(2);
	hist_hit_distr->Draw("COLZ");
	hist_hit_distr->SetXTitle("X [cm]");
	hist_hit_distr->SetYTitle("Y [cm]");

	//hist_track_dir_filt->Draw("COLZ");
	//hist_track_dir_filt->SetXTitle("p_{x}/p_{z}x10^{-3}");
	//hist_track_dir_filt->SetYTitle("p_{y}/p_{z}x10^{-3}");

	canvas->cd(3);
	hist_trk_mult->Draw();
	hist_trk_mult->SetXTitle("# tracks / event");

	canvas->cd(4);
	hist_hit_mult->Draw("colz");
	hist_hit_mult->SetXTitle("# hits / event");
	hist_hit_mult->SetYTitle("plane");

	canvas->cd(5);
	hist_track_dir_prop->Draw("COLZ");
	hist_track_dir_prop->SetXTitle("p_{x}/p_{z}x10^{-3}");
	hist_track_dir_prop->SetYTitle("p_{y}/p_{z}x10^{-3}");

	canvas->cd(6);
	hist_track_theta->Draw();
	hist_track_theta->SetXTitle("#Theta [mrad]");
	/*
	plane3_rate->Scale(1./tsim*1e-3); // in kHz
	//plane3_rate->SetMaximum(150);
	cout << " total rate of " << nhitstotal << " hits is " << nhitstotal/tsim*1e-6 << "MHz" << endl;
	plane3_rad->Scale(1./tsim*60.*60.*24.*365.*0.5/(2.33e-3*(2*2*50.e-4))); // 50% duty cycle over a year and normalized to the mass in kg of one sensor
	//canvas->cd(1);
	plane3_Edep->Draw("colz");
	//hist_eloss_total->Draw();
	//canvas->cd(2);
	plane3_hits->Draw("colz");
	canvas->Print("MC_hit_distr_1_5_plane_3_side_0.pdf");
	canvas->Print("MC_hit_distr_1_5_plane_3_side_0.root");
	hist_eloss->Draw();
	canvas->Print("MC_IEL_distrib_1_5.pdf");
	canvas->Print("MC_IEL_distrib_1_5.root");
	//canvas->cd(3);
	plane3_rate->Draw("colz");
	cout << " max rate is " << plane3_rate->GetBinContent(plane3_rate->GetMaximumBin()) << " kHz" << endl;
	canvas->Print("MC_hit_rate_1_5_plane_3_side_0.pdf");
	canvas->Print("MC_hit_rate_1_5_plane_3_side_0.root");
	//canvas->cd(4);
	plane3_rad->Draw("colz");
	cout << " max dose is " << plane3_rad->GetBinContent(plane3_rad->GetMaximumBin()) << " Gy" << endl;
	canvas->Print("MC_IEL_dose_1_5_plane_3_side_0.pdf");
	canvas->Print("MC_IEL_does_1_5_plane_3_side_0.root");
	*/
}

void Root_Appearance(){
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
		gStyle->SetTitleOffset(1,"xy");
		gStyle->SetTitleOffset(1.3,"z");
		gStyle->SetLabelFont(10*13+2,"xyz");
		gStyle->SetLabelSize(0.06,"xyz");
		gStyle->SetLabelOffset(0.009,"xyz");
		gStyle->SetPadBottomMargin(0.2);
		gStyle->SetPadTopMargin(0.15);
		gStyle->SetPadLeftMargin(0.15);
		gStyle->SetPadRightMargin(0.20);
		gStyle->SetOptTitle(0);
		gStyle->SetOptStat(0);
		gROOT->ForceStyle();
		gStyle->SetFrameFillColor(0);
	   	gStyle->SetFrameFillStyle(0);
	   	TGaxis::SetMaxDigits(3);
	}
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
