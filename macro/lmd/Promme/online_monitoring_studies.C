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
#include<vector>
#include<TTree.h>
#include<sstream>

#include<PndLmdDim.h>
#include<PndMCTrack.h>
#include<PndSdsMCPoint.h>
#include<TGraphErrors.h>

using namespace std;

// change some stuff in the ROOT appearance
void Root_Appearance();

double last_percent(-1.);
// draw a progress bar only when the length changes significantly
void DrawProgressBar(int len, double percent);

// the application
// Assuming to find an mc file called
// ./Lumi_MC_0.root
void online_monitoring_studies();

int main() {
	online_monitoring_studies();
	return 0;
}

void online_monitoring_studies(){
	Root_Appearance();
	TApplication myapp("myapp", 0, 0);
	cout << " lmd online monitoring studies " << endl;

	// get the lmd dim help class
	PndLmdDim& lmddim = PndLmdDim::Get_instance();


	const int nmomenta = 1;//3;
	double momenta[nmomenta] = {1.5};//{1.5, 4.06, 15.00};
	double pixelsize = 80; // um
	double ip_size = 1.; // mm
	double ip_div = 1.; // mrad
	double ip_width_z = 5.; // mm
	// z positions of planes in m
	vector < double > z; z.push_back(11.2); z.push_back(11.4); z.push_back(11.5); z.push_back(11.6);
	// acceptance in the plane (already the power of 2)
	const double Rmin = 0.035*0.035;
	const double Rmax = (0.035+0.05)*(0.035+0.05);
	// cut for a cone starting from ip
	// angles for a cone cut
	const double theta_cut_low = 4.; // mrad
	const double theta_cut_high = 8.; // mrad
	vector < double > Rmin_cuts;
	vector < double > Rmax_cuts;
	for (int iplane = 0; iplane < 4; iplane++ ){
		Rmin_cuts.push_back(theta_cut_low*1e-3*z[iplane]*theta_cut_low*1e-3*z[iplane]);
		Rmax_cuts.push_back(theta_cut_high*1e-3*z[iplane]*theta_cut_high*1e-3*z[iplane]);
	}

	TCanvas canvas("canvas", "canvas", 600, 1000);
	canvas.Divide(2,1);
	canvas.cd(1);
	TCanvas canvas_distributions("canvas_distributions", "distributions", 800, 800);
	canvas_distributions.Divide(2,2);

	//TFile treefile("asymmetries_results.root", "RECREATE");
	TTree results("asymmetry_results", "Asymmetry results");
	double offsetx, offsety, tiltx, tilty, momentum,
		offsetx_lmd, offsety_lmd, tiltx_lmd, tilty_lmd,
		offsetx_fit, offsety_fit, tiltx_fit, tilty_fit,
		meanx_0, meany_0, rmsx_0, rmsy_0,
		meanx_1, meany_1, rmsx_1, rmsy_1,
		meanx_2, meany_2, rmsx_2, rmsy_2,
		meanx_3, meany_3, rmsx_3, rmsy_3;
	int	count_0[10] = {0,0,0,0,0,0,0,0,0,0};
	int count_1[10] = {0,0,0,0,0,0,0,0,0,0};
	int count_2[10] = {0,0,0,0,0,0,0,0,0,0};
	int count_3[10] = {0,0,0,0,0,0,0,0,0,0};
	TLine line;

	results.Branch("offsetx", &offsetx);
	results.Branch("offsety", &offsety);
	results.Branch("tiltx", &tiltx);
	results.Branch("tilty", &tilty);
	results.Branch("offsetx_lmd", &offsetx_lmd);
	results.Branch("offsety_lmd", &offsety_lmd);
	results.Branch("tiltx_lmd", &tiltx_lmd);
	results.Branch("tilty_lmd", &tilty_lmd);
	results.Branch("momentum", &momentum);
	results.Branch("offsetx_fit", &offsetx_fit);
	results.Branch("offsety_fit", &offsety_fit);
	results.Branch("tiltx_fit", &tiltx_fit);
	results.Branch("tilty_fit", &tilty_fit);
	results.Branch("meanx_0", &meanx_0);
	results.Branch("meany_0", &meany_0);
	results.Branch("rmsx_0", &rmsx_0);
	results.Branch("rmsy_0", &rmsy_0);
	results.Branch("meanx_1", &meanx_1);
	results.Branch("meany_1", &meany_1);
	results.Branch("rmsx_1", &rmsx_1);
	results.Branch("rmsy_1", &rmsy_1);
	results.Branch("meanx_2", &meanx_2);
	results.Branch("meany_2", &meany_2);
	results.Branch("rmsx_2", &rmsx_2);
	results.Branch("rmsy_2", &rmsy_2);
	results.Branch("meanx_3", &meanx_3);
	results.Branch("meany_3", &meany_3);
	results.Branch("rmsx_3", &rmsx_3);
	results.Branch("rmsy_3", &rmsy_3);
	results.Branch("count_0", count_0, "count_0[10]/I");
	results.Branch("count_1", count_1, "count_1[10]/I");
	results.Branch("count_2", count_2, "count_2[10]/I");
	results.Branch("count_3", count_3, "count_3[10]/I");


	vector< string > files;

	// 15 GeV/c
	double pbeam = 1.5;
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.5_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.001_0.001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0002_-0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0002_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_-0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_-0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0001_0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0002_-0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0001_-0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0001_0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0001_-0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0002_0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.001_0.001/200000/mc_data/asymmetries_results.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.001_0.001/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0001_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0001_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0002_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.5_0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/asymmetries_results.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.1_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.1_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.5_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.5_0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.1_0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.5_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.5_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root");
/*

	// 1.5 GeV/c
	double pbeam = 1.5;
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.5_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.001_0.001_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.001_0.001/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.5_0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.1_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.1_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.5_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.5_0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.1_0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.5_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
	files.push_back("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.5_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data");
*/
	int nfiles = files.size();
	for (int ifile = 0; ifile < nfiles; ifile++){
		cout << endl << " file " << ifile << " of " << nfiles << "  = " <<  files[ifile] << endl;
		// load the MC File
		TChain tMC("cbmsim");
		tMC.Add((files[ifile]+"/Lumi_MC*.root").c_str());

		//--- assign MC info -----------------------------------------------------
		TClonesArray* true_tracks = new TClonesArray("PndMCTrack");
		tMC.SetBranchAddress("MCTrack", &true_tracks); //True Track to compare

		TClonesArray* true_points = new TClonesArray("PndSdsMCPoint");
		tMC.SetBranchAddress("LMDPoint", &true_points); //True Points to compare
		stringstream filename;
		filename << "asymmetry_studies_"<< momentum << "_GeVc_x" << offsetx << "_mm_y_" << offsety << "_mm_dxdz_" << tiltx << "_mrad_dydz_" << tilty << "_mrad";
		//TFile file((filename.str()+".root").c_str(), "RECREATE");

		TH2F hist_gen_theta_phi ("hist_gen_theta_phi", "theta phi generated distribution", 100, 2e-3, 9e-3, 100, -3.141, 3.141);
		TH2F hist_gen_x_y ("hist_gen_x_y", "x y generated distribution", 100, -5e-3, 5e-3, 100, -5e-3, 5e-3);
		TH2F hist_gen_dx_dy ("hist_gen_dx_dy", "dx/dz dy/dz generated distribution", 100, -15e-3, 15e-3, 100, -15e-3, 15e-3);
		TH2F hist_XY_plane_0 ("hist_XY_plane_0", "xy distribution at plane 0", 100, -0.100, 0.100, 100, -0.100, 0.100);
		TH2F hist_XY_plane_1 ("hist_XY_plane_1", "xy distribution at plane 1", 100, -0.100, 0.100, 100, -0.100, 0.100);
		TH2F hist_XY_plane_2 ("hist_XY_plane_2", "xy distribution at plane 2", 100, -0.100, 0.100, 100, -0.100, 0.100);
		TH2F hist_XY_plane_3 ("hist_XY_plane_3", "xy distribution at plane 3", 100, -0.100, 0.100, 100, -0.100, 0.100);
		TH2F* hist_XY_plane[4] = {&hist_XY_plane_0,&hist_XY_plane_1,&hist_XY_plane_2,&hist_XY_plane_3};
		//TH2F hist_phi_count("hist_phi_count", "counts over plane number", 4, -0.5, 3.5, 10, -3.141, 3.141);
		//TH2F hist_center_of_gravity("hist_center_of_gravity", "center of gravity over plane number", 4, -0.5, 3.5, 10, -3.141, 3.141);

		// calculate some multiple scattering constants

//		double beta = 1/sqrt(1+pow(0.938/momentum,2));
//		cout << "the beta factor for " << momentum << " GeV/c antiprotons is " << beta << endl;
//		double x_over_x0 = 2.329/21.82*350/1000/10;
//		cout << "x/X0 is" << x_over_x0 << endl;
//		double theta_0_slicon = 13.6/beta/(momentum*1e3)*sqrt(x_over_x0)*(1+0.038*log(x_over_x0));
//		cout << "scattering angle of 350 mum silicon is " << theta_0_slicon << endl;
		// assuming a multiple scattering effect in the transition region of a silicon plane of half thickness
//		double theta_0_kapton = 13.6/beta/(momentum*1e3)*sqrt(x_over_x0/2.)*(1+0.038*log(x_over_x0/2.));

		int nEvents = tMC.GetEntries();
		int nEvents_counted(0);
		int nhitstotal(0);

		tiltx = 0.;
		tilty = 0.;
		offsetx = 0.;
		offsety = 0.;
		momentum = 0.;

		int npbar = 0; // to normalize the mean for the above values to

		cout << " reading " << nEvents << " Events " << endl;
		for (Int_t j = 0; j < nEvents ; j++) {
			nEvents_counted++;
			DrawProgressBar(50, (j + 1) / ((double) nEvents));
			tMC.GetEntry(j);

			int nHits = true_points->GetEntriesFast();
			int nSdsHits = 0;

			const int nParticles = true_tracks->GetEntriesFast();

			// get the beam properties of the IP

			for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
				PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(iParticle);
				Int_t mcID = mctrk->GetPdgCode();
				if (mctrk->IsGeneratorCreated() && mcID == -2212) {
					npbar++;
					TVector3 momMC = mctrk->GetMomentum();
					tiltx += momMC.X()/momMC.Z();
					tilty += momMC.Y()/momMC.Z();
					momentum += momMC.Mag();
					TVector3 posMC = mctrk->GetStartVertex();
					offsetx += posMC.X();
					offsety += posMC.Y();
					hist_gen_theta_phi.Fill(momMC.Theta()*1e3, momMC.Phi());
					hist_gen_x_y.Fill(posMC.X()*10., posMC.X()*10.);
					hist_gen_dx_dy.Fill(momMC.X()/momMC.Z()*1e3, momMC.Y()/momMC.Z()*1e3);
				}
			}

			for (Int_t iHit = 0; iHit < nHits; iHit++) {
				PndSdsMCPoint* mcpoint = (PndSdsMCPoint*) true_points->At(iHit);
				double theta_ip = 0.;
				//if (mcpoint->GetTrackID() >= 0) {
				//	(true_tracks->At(mcpoint->GetTrackID))->;
				//}
				// cout << mcpoint->GetEnergyLoss() << endl;
				//hist_eloss_total->Fill(mcpoint->GetEnergyLoss()*1.e6); // in keV
				//hist_eloss->Fill(mcpoint->GetEnergyLoss()*1.e9/50.); // in eV

				int sensID = mcpoint->GetSensorID();
				int ihalf, iplane, imodule, iside, idie, isensor;
				lmddim.Get_sensor_by_id(sensID, ihalf, iplane, imodule, iside, idie, isensor);
				TVector3 hit_pos = mcpoint->GetPosition();
				TVector3 hit_pos_lmd = lmddim.Transform_global_to_lmd_local(hit_pos, false);


				nhitstotal++;

				double X = hit_pos_lmd.X()*0.01;
				double Y = hit_pos_lmd.Y()*0.01;
				//cout << hit_pos.Z() << endl;
				double RR = X*X+Y*Y;

				/*
				if (Rmin_cuts[iplane] < RR && RR < Rmax_cuts[iplane]){//(Rmin < RR && RR < Rmax){
					hist_XY_plane[iplane]->Fill(X, Y);
				}*/
			}
		}

		offsetx /= (double) npbar;
		offsety /= (double) npbar;
		TVector3 offset_ip(offsetx, offsety, 0);
		offsetx *= 10.; // mm
		offsety *= 10.; // mm
		tiltx /= (double) npbar;
		tilty /= (double) npbar;
		TVector3 tilt_ip(tiltx, tilty, 1.);
		tiltx *= 1000.; // mrad
		tilty *= 1000.; // mrad
		momentum /= (double) npbar;

		// propagate properties to lmd
		TVector3 tilt_lmd = tilt_ip;
		TVector3 offset_lmd = offset_ip;
		cout << " before propagation " << endl;
		offset_lmd.Print();
		tilt_lmd.Print();
		cout << endl;

		lmddim.Propagate_fast_ip_to_lmd(offset_lmd, tilt_lmd, pbeam);
		cout << " after propagation " << endl;
		offset_lmd.Print();
		tilt_lmd.Print();
		cout << endl;

		offset_lmd = lmddim.Transform_global_to_lmd_local(offset_lmd, false, true);
		tilt_lmd = lmddim.Transform_global_to_lmd_local(tilt_lmd, true, true);
		cout << " in lmd frame " << endl;
		offset_lmd.Print();
		tilt_lmd.Print();
		cout << endl;

		offsetx_lmd = offset_lmd.X()/10.;
		offsety_lmd = offset_lmd.Y()/10.;
		tiltx_lmd = tilt_lmd.X()/tilt_lmd.Z() * 1000.;
		tilty_lmd = tilt_lmd.Y()/tilt_lmd.Z() * 1000.;


		cout << " analyzing histograms for momentum " << momentum << endl;
		cout << " mean pos of ip was (generated)" << endl;
		cout << " x = " << hist_gen_x_y.GetMean(1) << " mm ("<< offsetx <<" mm)" << endl;
		cout << " y = " << hist_gen_x_y.GetMean(2) << " mm ("<< offsety <<" mm)" << endl;

		cout << " mean dir of beam was (generated)" << endl;
		cout << " theta = " << hist_gen_theta_phi.GetMean(1) << " mrad ("<< tiltx <<" mrad)" << endl;
		cout << " phi = " << hist_gen_theta_phi.GetMean(2) << " rad ("<< tilty <<" mrad)" << endl;
		cout << " dx/dz = " << hist_gen_dx_dy.GetMean(1) << " mrad ("<< tiltx <<" mrad)" << endl;
		cout << " dy/dz = " << hist_gen_dx_dy.GetMean(2) << " mrad ("<< tilty <<" mrad)" << endl;

		cout << " mean dir of beam was (propagated to lmd)" << endl;
		//cout << " theta = " << " mrad ("<< theta_lmd <<" mrad)" << endl;
		//cout << " phi = " << " rad ("<< phi_lmd <<" rad)" << endl;
		cout << " dx/dz = " << " mrad ("<< tiltx_lmd <<" mrad)" << endl;
		cout << " dy/dz = "  << " mrad ("<< tilty_lmd <<" mrad)" << endl;

		cout << " mean in plane 0,1,2,3 is " << endl;
		vector < double > means_x;
		vector < double > widths_x;
		vector < double > means_y;
		vector < double > widths_y;
		vector < double > z_err; z_err.push_back(0); z_err.push_back(0); z_err.push_back(0); z_err.push_back(0);
		means_x.push_back(hist_XY_plane_0.GetMean(1)); meanx_0 = means_x[0];
		means_x.push_back(hist_XY_plane_1.GetMean(1)); meanx_1 = means_x[1];
		means_x.push_back(hist_XY_plane_2.GetMean(1)); meanx_2 = means_x[2];
		means_x.push_back(hist_XY_plane_3.GetMean(1)); meanx_3 = means_x[3];
		means_y.push_back(hist_XY_plane_0.GetMean(2)); meany_0 = means_x[0];
		means_y.push_back(hist_XY_plane_1.GetMean(2)); meany_1 = means_x[1];
		means_y.push_back(hist_XY_plane_2.GetMean(2)); meany_2 = means_x[2];
		means_y.push_back(hist_XY_plane_3.GetMean(2)); meany_3 = means_x[3];

		widths_x.push_back(hist_XY_plane_0.GetRMS(1)/sqrt(hist_XY_plane_0.GetEntries())); rmsx_0 = hist_XY_plane_0.GetRMS(1);
		widths_x.push_back(hist_XY_plane_1.GetRMS(1)/sqrt(hist_XY_plane_1.GetEntries())); rmsx_1 = hist_XY_plane_1.GetRMS(1);
		widths_x.push_back(hist_XY_plane_2.GetRMS(1)/sqrt(hist_XY_plane_2.GetEntries())); rmsx_2 = hist_XY_plane_2.GetRMS(1);
		widths_x.push_back(hist_XY_plane_3.GetRMS(1)/sqrt(hist_XY_plane_3.GetEntries())); rmsx_3 = hist_XY_plane_3.GetRMS(1);
		widths_y.push_back(hist_XY_plane_0.GetRMS(2)/sqrt(hist_XY_plane_0.GetEntries())); rmsy_0 = hist_XY_plane_0.GetRMS(2);
		widths_y.push_back(hist_XY_plane_1.GetRMS(2)/sqrt(hist_XY_plane_1.GetEntries())); rmsy_1 = hist_XY_plane_1.GetRMS(2);
		widths_y.push_back(hist_XY_plane_2.GetRMS(2)/sqrt(hist_XY_plane_2.GetEntries())); rmsy_2 = hist_XY_plane_2.GetRMS(2);
		widths_y.push_back(hist_XY_plane_3.GetRMS(2)/sqrt(hist_XY_plane_3.GetEntries())); rmsy_3 = hist_XY_plane_3.GetRMS(2);

		//TMarker markeroffset(offsetx, offsety, 4);
		//markeroffset.SetMarkerColor(2);
		canvas_distributions.cd(1);
		hist_XY_plane_0.Draw("COLZ");
		//markeroffset.Draw();
		gPad->Update();
		canvas_distributions.cd(2);
		hist_XY_plane_1.Draw("COLZ");
		//markeroffset.Draw();
		gPad->Update();
		canvas_distributions.cd(3);
		hist_XY_plane_2.Draw("COLZ");
		//markeroffset.Draw();
		gPad->Update();
		canvas_distributions.cd(4);
		hist_XY_plane_3.Draw("COLZ");
		//markeroffset.Draw();
		gPad->Update();

		// save values to tree


		cout << " x = " << meanx_0 << ", "<< meanx_1 << ", "<< meanx_2 << ", "<< meanx_3 << endl;
		cout << " y = " << meany_0 << ", "<< meany_1 << ", "<< meany_2 << ", "<< meany_3 << endl;
		cout << " rms x = " << rmsx_0 << ", "<< rmsx_1 << ", "<< rmsx_2 << ", "<< rmsx_3 << endl;
		cout << " rms y = " << rmsy_0 << ", "<< rmsy_1 << ", "<< rmsy_2 << ", "<< rmsy_3 << endl;

		TGraphErrors graph_average_x(means_x.size(), &z[0], &means_x[0], NULL, &widths_x[0]);
		graph_average_x.SetNameTitle("graph_average_x", "average x over z");
		canvas.cd(1);
		gPad->Clear();
		graph_average_x.Draw("ALP");
		graph_average_x.GetYaxis()->SetRangeUser(-0.02, 0.02);
		TF1 funcx("funcx", "pol1", -0.1, 0.5);
		graph_average_x.Fit(&funcx);
		//line.DrawLine(z[0], offsetx*1e-3, z[3], offsetx*1e-3);
		//line.DrawLine(z[0], tiltx*1e-3*z[0]+offsetx*1e-3, z[3], tiltx*1e-3*z[3]+offsetx*1e-3);
		//line.DrawLine(z[0], offsetx_lmd/100., z[3], offsetx_lmd/100.);
		line.DrawLine(z[0], offsetx_lmd/100., z[3], offsetx_lmd/100.+tiltx_lmd/1000.*(z[3]-z[0]));
		gPad->Update();
		//sleep(2);
		//graph_average_x.Write();
		canvas.cd(2);
		TGraphErrors graph_average_y(means_y.size(), &z[0], &means_y[0], NULL, &widths_y[0]);
		graph_average_y.SetNameTitle("graph_average_y", "average y over z");
		gPad->Clear();
		graph_average_y.Draw("ALP");
		graph_average_y.GetYaxis()->SetRangeUser(-0.02, 0.02);
		TF1 funcy("funcy", "pol1", -0.1, 0.5);
		graph_average_y.Fit(&funcy);
		//line.DrawLine(z[0], offsety*1e-3, z[3], offsety*1e-3);
		//line.DrawLine(z[0], tilty*1e-3*z[0]+offsety*1e-3, z[3], tilty*1e-3*z[3]+offsety*1e-3);
		//line.DrawLine(z[0], offsety_lmd/100., z[3], offsety_lmd/100.);
		line.DrawLine(z[0], offsety_lmd/100., z[3], offsety_lmd/100.+tilty_lmd/1000.*(z[3]-z[0]));
		//graph_average_y.Write();
		gPad->Update();

		cout << " mean pos of ip fit (generated and propagated)" << endl;
		offsetx_fit = funcx.Eval(z[0])*1e3;// GetParameter(0)*1e3;
		offsety_fit = funcy.Eval(z[0])*1e3;// GetParameter(0)*1e3;
		cout << " x = " << offsetx_fit << " mm ("<< offsetx_lmd <<" mm)" << endl;
		cout << " y = " << offsety_fit << " mm ("<< offsety_lmd <<" mm)" << endl;

		tiltx_fit = funcx.GetParameter(1)*1e3;
		tilty_fit = funcy.GetParameter(1)*1e3;
		cout << " mean dir of beam fit (generated and propagated)" << endl;
		cout << " dx/dz = " << tiltx_fit << " mrad ("<< tiltx_lmd <<" mrad)" << endl;
		cout << " dy/dz = " << tilty_fit << " mrad ("<< tilty_lmd <<" mrad)" << endl;

		cout << endl;
		results.Fill();
		//file.Write();
		//file.Close();
	}

	TFile treefile("asymmetries_results.root", "RECREATE");
	results.SetDirectory(&treefile);
	results.Write();
	treefile.Close();
	cout << " done " << endl;
	myapp.Run();
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
		gStyle->SetOptTitle(1);
		gStyle->SetOptStat(1);
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
