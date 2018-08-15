#include <iostream>
#include <sstream>

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
#include<string>

#include<PndLmdDim.h>
#include<PndMCTrack.h>
#include<PndSdsMCPoint.h>
#include<TGraphErrors.h>

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TError.h"
#include "TLine.h"

#include <iomanip>

using namespace std;

// change some stuff in the ROOT appearance
void Root_Appearance();

double last_percent(-1.);
// draw a progress bar only when the length changes significantly
void DrawProgressBar(int len, double percent);

// the application
// Assuming to find an mc file called
// ./Lumi_MC_0.root
// if invert, then the solution for a transformation
// from lmd to ip is searched
int trafo_matrix_fit(bool invert = false);

// calling all vectors to minimize the parameters xx
// (Minimizer function)
double Chi2(const double *xx);

// load vectors from a given MC file
// vectors from hits at the first LMD plane
// with corresponding hits at the IP will be loaded
// if invert, then the solution for a transformation
// from lmd to ip is searched
void load_vectors(string filename, bool invert = false);

// test loaded vectors on a transformation matrix
// see code of Chi2() for the structure explanation of the matrix xx
// provide an output file name for the results
void trafo_matrix_test(const double *xx, string filename);

// definition of the vector of parameters
// containing the homogeneous space coordinates
// and the 3 momenta of particles
// x,y,z,1,px/pz,py/pz,1
class  TRepVect{
public:
	TRepVect(){
		for (int i = 0; i < 7; i++){
			entry[i] = 0;
		}
	}
	double entry[7];
	double Mag2(){
		double result = 0.;
		for (int i = 0; i < 7; i++){
			if (i < 4) result += entry[i]*entry[i];
			// put a penalty on the normalized momentum direction vectors
			// to have a good measure of the track direction
			else result += entry[i]*entry[i];
		}
		return result;
	}
	double& operator[] (int i) {return entry[i];}
	TRepVect operator- (const TRepVect vect){
		TRepVect result;
		for (int i = 0; i < 7; i++){
			result.entry[i] = this->entry[i]-vect.entry[i];
		}
		return result;
	}
};

class TRepVectPair{
public:
	TRepVect IP;
	TRepVect LMD;
	bool valid;
	void invert(){
		TRepVect _temp = IP;
		IP = LMD;
		LMD = _temp;
	}
};

// containing a 7 x 7 transformation matrix for TRepVect
/* Assuming the following structure of the matrix
 * which is used to transform homogeneous coordinates
 * as well as momenta directions
 * (x,y,z,1,px/pz,py/pz,1)^T = xi
 */
// The corresponding transformation would be
// xi' = Mij * xj
// where xi' are the vector parameters at the LMD
// and xj the vector parameters at the IP
// xi' will be written as xi
// Mij * xj - xi' should be ideally 0
// what will be used as an minimization criterion
class TRepMat{
public:
	double M[7][7];
	TRepMat(){
		for (int i = 0; i < 7; i++){
			for (int j = 0; j < 7; j++){
				M[i][j] = 0;
			}
		}
	}
	// constructor for 49 entries in a row
	// to be matched on the matrix
	TRepMat(const double* mat){
		for (int i = 0; i < 7*7; i++){
			(*this)[i] = mat[i];
		}
	}
	double& operator[](int const ij) {
		return M[ij/7][ij%7];
	}
	double& GetSet(int col, int row){
		return M[col][row];
	}
	double* Get(){
		double* result = new double[7*7];
		for (int i = 0; i < 7*7; i++){
			result[i] = (*this)[i];
		}
		return result;
	}
	TRepVect operator*(const TRepVect vect){
		TRepVect result;
		for (int i = 0; i < 7; i++){
			for (int j = 0; j < 7; j++){
				result.entry[i] += this->M[i][j] * vect.entry[j];
			}
		}
		return result;
	}
	TRepMat operator*(const TRepMat mat){
		TRepMat result;
		for (int i = 0; i < 7; i++){
			for (int j = 0; j < 7; j++){
				for (int k = 0; k < 7; k++){
					result.M[i][j] += this->M[i][k] * mat.M[k][j];
				}
			}
		}
		return result;
	}
	void Print(){
		cout << " The numbers are " << endl;
		for (int i = 0; i < 7; i++){
			for (int j = 0; j < 7; j++){
				cout << std::setprecision(4) << "\t" << M[i][j] << ", ";
			}
			cout << endl;
		}
	}
};

vector<TRepVectPair> repvectpairs;

// see what is going on during minimization
TGraph convergence_graph;
TH1D Chi2_dist("Chi2_dist", "chi2 distribution", 100, 0, 0.002);
TLine meanline;
int ncalls(0);

int main() {
	trafo_matrix_fit(false);
	return 0;
}

double Chi2(const double *xx)
{
	Chi2_dist.Reset();
/*
	double Mij[7][7] = {
			{xx[0],xx[1],xx[2],xx[3],xx[4],xx[5],xx[6]},
			{xx[7],xx[8],xx[9],xx[10],xx[11],xx[12],xx[13]},
			{xx[14],xx[15],xx[16],xx[17],xx[18],xx[19],xx[20]},
			{xx[21],xx[22],xx[23],xx[24],xx[25],xx[26],xx[27]},
			{xx[28],xx[29],xx[30],xx[31],xx[32],xx[33],xx[34]},
			{xx[35],xx[36],xx[37],xx[38],xx[39],xx[40],xx[41]},
			{xx[42],xx[43],xx[44],xx[45],xx[46],xx[47],xx[48]}
	};
	*/
	TRepMat Mij(xx);
	double mean_diff = 0;
	int nvectors = repvectpairs.size();
	for (int ivector = 0; ivector < nvectors; ivector++){
		TRepVect x0 = Mij*repvectpairs[ivector].IP;
		x0 = x0 - repvectpairs[ivector].LMD;
		//cout << endl;
		//for (int i = 0; i < 7; i++){
		//	cout << "                                            " << repvectpairs[ivector].IP[i] << endl;
		//}
		//for (int i = 0; i < 7; i++){
		//	for (int j = 0; j < 7; j++){
		// 		cout << "\t" << Mij[i][j];
		//	}
		//	cout << endl;
		//}
		/*
		for (int i = 0; i < 7; i++){
			//cout << endl << i << " : " << repvectpairs[ivector].IP[i] << " becomes ";
			for (int j = 0; j < 7; j++){
				x0[i] += Mij[i][j] * repvectpairs[ivector].IP[j];
			}
			//cout << x0[i] << " and should be ";
			x0[i] -= repvectpairs[ivector].LMD[i];
			//cout << endl << repvectpairs[ivector].LMD[i] << endl;
		}*/
		//for (int i = 0; i < 7; i++){
		//	cout << " " << x0[i] << "\t" << repvectpairs[ivector].LMD[i] << endl;
		//}
		// calculate the length of the vector
		Chi2_dist.Fill(x0.Mag2());
		//cout << x0.Mag2() << endl;
		mean_diff += x0.Mag2()/(double) nvectors;

		//break;
	}
	// normalize
	//cout << " the difference before normalization is " << mean_diff << endl;
	//cout << mean_diff << " " << endl;
	//mean_diff /= (double) repvectpairs.size();
	//cout << " " << mean_diff << endl;
	/*cout << " The numbers are " << endl;
	for (int i = 0; i < 7; i++){
		for (int j = 0; j < 7; j++){
			cout << "\t" << Mij[i][j];
		}
		cout << endl;
	}*/
	//cout << " the difference is " << mean_diff << endl;

	ncalls++;
	if (ncalls > convergence_graph.GetN()){
		convergence_graph.Set(ncalls+100);
		for (int i = ncalls; i < ncalls + 100; i++)
			convergence_graph.SetPoint(i, 0, convergence_graph.GetY()[0]);
	}
	convergence_graph.SetPoint(ncalls-1, ncalls, mean_diff);
	if (ncalls%100 == 0){
		Chi2_dist.Draw();
		//convergence_graph.Draw("A*P");
		gPad->SetLogy();
		meanline.SetX1(mean_diff);
		meanline.SetX2(mean_diff);
		meanline.SetY1(1e-4);
		meanline.SetY2(1000);
		meanline.SetLineColor(2);
		meanline.SetLineWidth(2);
		meanline.Draw();
		gPad->Update();
		//sleep(1);
	}
	//mean_diff /= (double) repvectpairs.size();

	return Chi2_dist.GetMean();
	return mean_diff; // the mean diff seems not a good measure without a cut such as the histogram is performing
}

void trafo_matrix_test(const double *xx, string filename){
	TCanvas canvas("canvas", "canvas", 600, 600);
	TH1F hist_res_x("hist_res_x", "resolution x", 500, -0.005, 0.005);
	TH1F hist_res_y("hist_res_y", "resolution y", 500, -0.005, 0.005);
	TH1F hist_res_z("hist_res_z", "resolution z", 500, -0.005, 0.005);
	TH1F hist_res_w("hist_res_w", "resolution w", 500, -0.005, 0.005);
	TH1F hist_res_pxpz("hist_res_pxpz", "resolution px/pz", 500, -0.005, 0.005);
	TH1F hist_res_pypz("hist_res_pypz", "resolution py/pz", 500, -0.005, 0.005);
	TH1F hist_res_pw("hist_res_pw", "resolution pw", 500, -0.005, 0.005);
	//TH1F hist_res_theta("hist_res_theta", "resolution #theta", 1000, -0.1, 0.1);
	//TH1F hist_res_phi("hist_res_phi", "resolution #phi", 1000, -0.1, 0.1);
	TH1F* hists[7] = {&hist_res_x, &hist_res_y, &hist_res_z, &hist_res_w, &hist_res_pxpz, &hist_res_pypz, &hist_res_pw};
	TRepMat Mij(xx);
	/*
	double Mij[7][7] = {
			{xx[0],xx[1],xx[2],xx[3],xx[4],xx[5],xx[6]},
			{xx[7],xx[8],xx[9],xx[10],xx[11],xx[12],xx[13]},
			{xx[14],xx[15],xx[16],xx[17],xx[18],xx[19],xx[20]},
			{xx[21],xx[22],xx[23],xx[24],xx[25],xx[26],xx[27]},
			{xx[28],xx[29],xx[30],xx[31],xx[32],xx[33],xx[34]},
			{xx[35],xx[36],xx[37],xx[38],xx[39],xx[40],xx[41]},
			{xx[42],xx[43],xx[44],xx[45],xx[46],xx[47],xx[48]}
	};*/
	int nvectors = repvectpairs.size();
	for (int ivector = 0; ivector < nvectors; ivector++){
		//TRepVect x0;
		//TVector3 dir_lmd(repvectpairs[ivector].LMD[4], repvectpairs[ivector].LMD[5], repvectpairs[ivector].LMD[6]);
		//TVector3 dir_after;
		TRepVect x0 = Mij*repvectpairs[ivector].IP;
		x0 = x0 - repvectpairs[ivector].LMD;
		for (int i = 0; i < 7; i++){
			//for (int j = 0; j < 7; j++){
			//	x0[i] += Mij[i][j] * repvectpairs[ivector].IP[j];
			//}
			//if (i == 4) dir_after.SetX(x0[i]);
			//if (i == 5) dir_after.SetY(x0[i]);
			//if (i == 6) dir_after.SetZ(x0[i]);
			//x0[i] -= repvectpairs[ivector].LMD[i];
			hists[i]->Fill(x0[i]);
		}
		//hist_res_theta.Fill(dir_after.Theta()-dir_lmd.Theta());
		//hist_res_phi.Fill(dir_after.Phi()-dir_lmd.Phi());
	}
	//hist_res_theta.Draw("hist");
	//canvas.Print((filename+"(").c_str());
	//hist_res_phi.Draw("hist");
	//canvas.Print((filename+"(").c_str());
	for (int i = 0; i < 7; i++){
		hists[i]->Draw("hist");
		if (i < 6) canvas.Print((filename+"(").c_str());
		else canvas.Print((filename+")").c_str());
	}
//	canvas.Print((filename.str()+"]").c_str());
}

void load_vectors(string filename, bool invert){
	PndLmdDim& lmddim = PndLmdDim::Get_instance();
	TChain tMC("pndsim");
	tMC.Add(filename.c_str());

	//--- assign MC info -----------------------------------------------------
	TClonesArray* true_tracks = new TClonesArray("PndMCTrack");
	tMC.SetBranchAddress("MCTrack", &true_tracks); //True Track to compare

	TClonesArray* true_points = new TClonesArray("PndSdsMCPoint");
	tMC.SetBranchAddress("LMDPoint", &true_points); //True Points to compare

	// ******** testing
	TCanvas canvas("canvas_test", "canvas_test", 600, 600);
	TH1F hist_res_x("hist_res_x", "resolution x", 500, -0.1, 0.1);
	TH1F hist_res_y("hist_res_y", "resolution y", 500, -0.1, 0.1);
	TH1F hist_res_z("hist_res_z", "resolution z", 500, -0.5, 0.5);
	TH1F hist_res_px("hist_res_px", "resolution px", 500, -0.001, 0.001);
	TH1F hist_res_py("hist_res_py", "resolution py", 500, -0.001, 0.001);
	TH1F hist_res_pz("hist_res_pz", "resolution pz", 500, -0.001, 0.001);
	TH1F hist_res_theta("hist_res_theta", "resolution theta", 500, -0.001, 0.001);
	TH1F hist_res_phi("hist_res_phi", "resolution phi", 500, -0.1, 0.1);
	// ******** end testing

	int nEvents = tMC.GetEntries();
	cout << " reading " << nEvents << " Events " << endl;
	for (Int_t j = 0; j < nEvents ; j++) {
		DrawProgressBar(50, (j + 1) / ((double) nEvents));
		tMC.GetEntry(j);

		// For testing purposes only
		TVector3 pos_ip_test; // for testing only
		TVector3 mom_ip_test; // for testing only
		TVector3 pos_lmd_test; // for testing only
		TVector3 mom_lmd_test; // for testing only
		// end of "testing purposes only"

		int nHits = true_points->GetEntriesFast();
		int nSdsHits = 0;

		const int nParticles = true_tracks->GetEntriesFast();

		TRepVectPair repvectpair;
		repvectpair.valid = false;

		// get the beam properties of the IP
		for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
			PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(iParticle);
			Int_t mcID = mctrk->GetPdgCode();
			if (mctrk->IsGeneratorCreated() && mcID == -2212) {
				//npbar++;
				TVector3 momMC = mctrk->GetMomentum();
				mom_ip_test = momMC;
				//tiltx += momMC.X()/momMC.Z();
				//tilty += momMC.Y()/momMC.Z();
				//momentum += momMC.Mag();
				TVector3 posMC = mctrk->GetStartVertex();
				pos_ip_test = posMC;
				//offsetx += posMC.X();
				//offsety += posMC.Y();
				repvectpair.IP[0] = posMC.X()/100.;
				repvectpair.IP[1] = posMC.Y()/100.;
				repvectpair.IP[2] = posMC.Z()/100.;
				repvectpair.IP[3] = 1.;
				repvectpair.IP[4] = momMC.X()/momMC.Z()*10.; // scale by 10 in order to weight its importance
				repvectpair.IP[5] = momMC.Y()/momMC.Z()*10.;
				repvectpair.IP[6] = 1.;//momMC.Z()/momMC.Mag();
				break;
			}
		}

		for (Int_t iHit = 0; iHit < nHits; iHit++) {
			PndSdsMCPoint* mcpoint = (PndSdsMCPoint*) true_points->At(iHit);
			if (mcpoint->GetTrackID() < 0) continue;
			PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(mcpoint->GetTrackID());
			if (mctrk->GetPdgCode() == -2212){

			}
			// cout << mcpoint->GetEnergyLoss() << endl;
			//hist_eloss_total->Fill(mcpoint->GetEnergyLoss()*1.e6); // in keV
			//hist_eloss->Fill(mcpoint->GetEnergyLoss()*1.e9/50.); // in eV

			int sensID = mcpoint->GetSensorID();
			int ihalf, iplane, imodule, iside, idie, isensor;
			lmddim.Get_sensor_by_id(sensID, ihalf, iplane, imodule, iside, idie, isensor);
			if (iplane > 0) continue; // take only hits on the very first plane
			TVector3 posMC = mcpoint->GetPosition();
			TVector3 momMC(mcpoint->GetPx(), mcpoint->GetPy(), mcpoint->GetPz());// = mctrk->GetMomentum();
			pos_lmd_test = posMC;
			mom_lmd_test = momMC;
			repvectpair.LMD[0] = posMC.X()/100.;
			repvectpair.LMD[1] = posMC.Y()/100.;
			repvectpair.LMD[2] = posMC.Z()/100.;
			repvectpair.LMD[3] = 1.;
			repvectpair.LMD[4] = momMC.X()/momMC.Z()*10.;
			repvectpair.LMD[5] = momMC.Y()/momMC.Z()*10.;
			repvectpair.LMD[6] = 1.;//momMC.Z()/momMC.Mag();
			repvectpair.valid = true;
			break;
			//TVector3 hit_pos_lmd = lmddim.Transform_global_to_lmd_local(hit_pos, false);
			//nhitstotal++;
		}
		if (repvectpair.valid){
			if (invert) repvectpair.invert();
			repvectpairs.push_back(repvectpair);
			/*
			cout << endl;
			for (int i = 0; i < 7; i++){
				cout << i << " " << repvectpair.IP[i] << " ";
				cout << i << " " << repvectpair.LMD[i] << " ";
			}
			cout << endl;*/
			// testing fast tracking of lmd dim which is based on the results from this fit
			//cout << endl;
			//pos_ip_test.Print();
			//mom_ip_test.Print();
			lmddim.Propagate_fast_ip_to_lmd(pos_ip_test, mom_ip_test, 1.5); // modfy last number according to beam momentum
			//cout << endl;
			//pos_ip_test.Print();
			//pos_lmd_test.Print();
			//mom_ip_test.Print();
			//mom_lmd_test.Print();

			double theta_test = mom_ip_test.Theta() - mom_lmd_test.Theta();
			double phi_test = mom_ip_test.Phi() - mom_lmd_test.Phi();
			pos_lmd_test = pos_ip_test - pos_lmd_test;
			mom_lmd_test = mom_ip_test - mom_lmd_test;
			hist_res_x.Fill(pos_lmd_test.X());
			hist_res_y.Fill(pos_lmd_test.Y());
			hist_res_z.Fill(pos_lmd_test.Z());
			hist_res_px.Fill(mom_lmd_test.X());
			hist_res_py.Fill(mom_lmd_test.Y());
			hist_res_pz.Fill(mom_lmd_test.Z());
			hist_res_theta.Fill(theta_test);
			hist_res_phi.Fill(phi_test);
		}
	}
	cout << endl << " found " << repvectpairs.size() << " vector pairs for analysis " << endl;

	// testing
	static int datacounter (0);
	datacounter++;
	stringstream outfile;
	outfile << "test_lmd_" << datacounter << ".pdf";

	hist_res_x.Draw("hist");
	canvas.Print((outfile.str()+"(").c_str());
	hist_res_y.Draw("hist");
	canvas.Print((outfile.str()+"(").c_str());
	hist_res_z.Draw("hist");
	canvas.Print((outfile.str()+"(").c_str());
	hist_res_px.Draw("hist");
	canvas.Print((outfile.str()+"(").c_str());
	hist_res_py.Draw("hist");
	canvas.Print((outfile.str()+"(").c_str());
	hist_res_pz.Draw("hist");
	canvas.Print((outfile.str()+"(").c_str());
	hist_res_theta.Draw("hist");
	canvas.Print((outfile.str()+"(").c_str());
	hist_res_phi.Draw("hist");
	canvas.Print((outfile.str()+")").c_str());
}

int trafo_matrix_fit(bool invert){
	TApplication myapp("myapp", 0, 0);
	// all 1.5 GeV/c files by stefan

load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.5_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.001_0.001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0002_-0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0002_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_-0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_-0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0001_0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0002_-0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0001_-0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0001_0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0001_-0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0002_0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.001_0.001/200000/mc_data/asymmetries_results.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.001_0.001/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_-0.0001_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0001_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0001_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0002_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0002_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.5_0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/asymmetries_results.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.1_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.1_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.1_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.5_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.5_0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.1_0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_-0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.1_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_-0.5_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.5_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
//load_vectors("./box_th_0.0-15.0mrad_recoil_corrected/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
//load_vectors("./box_th_0.0-15.0mrad_recoil_corrected/ip_offset_XYZDXDYDZ_0.5_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);
//load_vectors("./box_th_0.0-15.0mrad_recoil_corrected/ip_offset_XYZDXDYDZ_-0.5_-0.5_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC_200000.root", invert);

//	load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC*.root", invert);

	// 15 GeV/c files by stefan
	//load_vectors("./dpm_elastic_thmin_0.06deg/ip_offset_XYZDXDYDZ_0.0_0.0_0.0_0.08_0.08_0.35/beam_grad_XYDXDY_0.0_0.0_0.0_0.0/200000/mc_data/Lumi_MC*.root", invert);

	ROOT::Math::Minimizer* min =
			ROOT::Math::Factory::CreateMinimizer("Minuit2", "");
	min->SetMaxFunctionCalls(100000);
	min->SetMaxIterations(100000);
	min->SetTolerance(0.00001);
	min->SetPrintLevel(1);

	ROOT::Math::Functor f(&Chi2, 49);
	/*
	double Mtranssol[49] = {
			1		,0	,0		,0			,0  	,0		,0		, // angles are scaled by 10
			0		,1	,0		,0			,0		,0  	,0		, // 2 equals 2m
			0	    ,0	,1		,6.5		,0		,0		,0		,
			0		,0	,0		,1			,0		,0		,0		,
			0		,0	,0		,0			,1		,0		,0		, // 40 mrad times 10
			0		,0	,0		,0			,0		,1		,0		,
			0		,0	,0		,0			,0		,0		,1
	};
	double Mrotsol[49] = {
			cos(.21)	,-sin(.21)	,0		,0			,0		,0		,0		, // angles are scaled by 10
			sin(.21)	,cos(.21)	,0		,0			,0		,0		,0		, // 1.124 equals 11.24m
			0			,0			,1		,0			,0		,0		,0		,
			0			,0			,0		,1			,0		,0		,0		,
			0			,0			,0		,0			,1		,0		,0		, // 40 mrad times 10
			0			,0			,0		,0			,0		,1		,0		,
			0			,0			,0		,0			,0		,0		,1
	};
	double Mrotdip[49] = {
			1		,0	,40e-3	,0			,0		,0		,0		, // angles are scaled by 10
			0		,1	,0		,0			,0		,0		,0		, // 1.124 equals 11.24m
			-40e-3	,0	,1		,0			,0		,0		,0		,
			0		,0	,0		,1			,0		,0		,0		,
			0		,0	,0		,0			,1		,0		,400e-3	, // 40 mrad times 10
			0		,0	,0		,0			,0		,1		,0		,
			0		,0	,0		,0			,0		,0		,1
	};
	double Mtransdiplmd[49] = {
			1		,0	,0		,0			,1.124  ,0		,0		, // angles are scaled by 10
			0		,1	,0		,0			,0		,1.124  ,0		, // 2 equals 2m
			0	    ,0	,1		,4.74		,0		,0		,0		,
			0		,0	,0		,1			,0		,0		,0		,
			0		,0	,0		,0			,1		,0		,0		, // 40 mrad times 10
			0		,0	,0		,0			,0		,1		,0		,
			0		,0	,0		,0			,0		,0		,1
	};*/

	//TRepMat M1(Mtranssol), M2(Mrotsol), M3(Mrotdip), M4(Mtransdiplmd);
	//TRepMat MSum = M4* M3 * M2 * M1;
	//M4.Print();
	//M3.Print();
	//M2.Print();
	//M1.Print();
	// hacking
	//MSum.M[0][6] = 0;
	//MSum.M[1][3] = 0;
	//MSum.Print();
	//double* Mij = MSum.Get();
	//double phi = 0.21;
	/*
	double Mij[49] = {
			1		,0		,0.04	,6.5*0.04	,1.0992	,-0.234	,0		, // angles are scaled by 10
			0		,1		,0		,0			,+0.234	,1.0992	,0		, // 1.124 equals 11.24m
			-0.04	,0		,1		,11.24		,0		,0		,0		,
			0		,0		,0		,1			,0		,0		,0		,
			0		,0		,0		,0			,0.978	,-0.208	,0.4	, // 40 mrad times 10
			0		,0		,0		,0			,0.208	,0.978	,0.0  ,
			0		,0		,0		,0			,0		,0		,1
	};*/
	double Mij[49] = {
			1		,0	,40e-3	,6.5*40e-3	,1.124	,0.			,0		, // angles are scaled by 10
			0		,1	,0		,0			,0		,1.124		,0		, // 1.124 equals 11.24m
			-40e-3	,0	,1		,11.24		,0		,0			,0		,
			0		,0	,0		,1			,0		,0			,0		,
			0		,0	,0		,0			,1		,0			,400e-3	, // 40 mrad times 10
			0		,0	,0		,0			,0		,1			,0		,
			0		,0	,0		,0			,0		,0			,1
	};
	if (invert){
		double Mij_inverted[49] = {
		/*		1		,0	,-0.04	,4.75*0.04	,-1.124	,0			,0.4*1.124	, // angles are scaled by 10
				0		,1	,0		,0			,0		,-1.124		,0			, // 1.124 equals 11.24m
				0.04	,0	,1		,-11.24		,0		,0			,0			,
				0		,0	,0		,1			,0		,0			,0			,
				0		,0	,0		,0			,1		,0			,-400e-3	, // 40 mrad times 10
				0		,0	,0		,0			,0		,1			,0			,
				0		,0	,0		,0			,0		,0			,1 */
				0.979707, 0.00855083, -0.957085, 10.5033, -1.14336, -0.00568183,
				0.45782, 0.0379627, 0.955083, 0.999504, -11.2443, 0.00606571,
				-1.07678, -0.00239097, 9.70718, -4.38641, 244.35, -2749.01,
				-0.0322177, 4.76301, 0.0134341, 0.0000190301, 0.000359596,
				0.000503436, 0.994336, 0.0000179324, -0.00038629, -6.77702e-6,
				0.147446, -0.229086, 3.19516, -35.9521, 0.974497, 0.28175,
				-0.389989, 0.105476, 0.0543932, 0.267799, -3.03747, -0.167528,
				0.943272, 0.0670344, 0.0000190301, 0.000359596, 0.000503436,
				-0.00566317, 0.0000179324, -0.00038629, 0.999993
		};
		for (int i = 0; i < 49; i++)
			Mij[i] = Mij_inverted[i];
	}
	TRepMat MSum(Mij);
	cout << " the starting condition is " << endl;
	MSum.Print();
/*
	double Mij[49] = {
			1		,0	,0		,0		,0		,0	,0		,
			0		,1	,0		,0		,0		,0	,0		,
			0		,0	,1		,0		,0		,0	,0		,
			0		,0	,0		,1		,0		,0	,0		,
			0		,0	,0		,0		,1		,0	,0		,
			0		,0	,0		,0		,0		,1	,0		,
			0		,0	,0		,0		,0	 	,0	,1
	};*/
	//Chi2(Mij);
	//Chi2(Mij);
	min->SetFunction(f);
	double stepsize = 0.001;
	for (int i = 0; i < 49; i++){
		stringstream varname;
		varname << "M" << i/7 << i%7;
		if (i > 3) stepsize = 0.001;
		else stepsize = 0.001;
		//if (Mij[i] == 0) Mij[i] = 0.0001;
		min->SetVariable(i, varname.str().c_str(), Mij[i], stepsize);
	}
	//trafo_matrix_test(0, "residuals_test.pdf");

	trafo_matrix_test(Mij, "residuals_before.pdf");
	min->Minimize();
	TRepMat Mij_min(min->X());
	TRepMat Mij_err(min->Errors());
	cout << " The minimized matrix is " << endl;
	Mij_min.Print();
	cout << " with the corresponding errors " << endl;
	Mij_err.Print();

	trafo_matrix_test(Mij_min.Get(), "residuals_after.pdf");
	//min->PrintResults();
	myapp.Run();
  return 0;
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
