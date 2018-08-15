// visualization of field lines in the Panda field map

#include<TString.h>
#include<TStopwatch.h>
#include<TROOT.h>
#include<FairModule.h>
#include<PndCave.h>
#include<PndMagnet.h>
#include<PndPipe.h>
#include<PndMvdDetector.h>
#include<FairRunSim.h>
#include<PndLmdDetector.h>
#include<FairPrimaryGenerator.h>
#include<FairBoxGenerator.h>
#include<PndMultiField.h>
#include<PndTransMap.h>
#include<PndDipoleMap.h>
#include<PndSolenoidMap.h>
#include<FairParRootFileIo.h>
#include<PndMultiFieldPar.h>
#include<map>
#include<vector>
#include<fstream>
#include<iostream>
#include<iomanip>
#include<sstream>
#include<TH1.h>
#include<TTree.h>
#include<TFile.h>
#include<TCanvas.h>
#include<TVector3.h>
#include "TPolyLine3D.h"
#include "TView.h"
#include "TCanvas.h"
#include "TRandom.h"
#include<TApplication.h>
#include "TGLViewer.h"
#include <TGeoManager.h>
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"
#include "TString.h"
#include "TSystem.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"
#include "FairGeoBuilder.h"
#include "FairModule.h"

using namespace std;

vector<string> fieldmapnames;

// read fem calculated fieldmap and transform it
// 	fieldtype 1: no symetry
//  fieldtype 2: Solenoidfieled
//  fieldtype 3: Dipolefield
//  fieldtype 4: Transition region
// filename_out is the pandaroot filename without the extenstion
// in contrast to filename_in having the extension
void visualize_fieldmaps() {
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	cout << " setup of visualization " << endl;
	TApplication* myapp = NULL;
	TCanvas *canvas_fieldlines = NULL;
	myapp = new TApplication("myapp",0,0);


	TString vmcWorkdir = gSystem->Getenv("VMCWORKDIR");
	// materials and media
	FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
	FairGeoInterface* geoFace = geoLoad->getGeoInterface();
	geoFace->setMediaFile(vmcWorkdir + "/geometry/media_pnd.geo");
	geoFace->readMedia();
	geoFace->print();
	FairGeoMedia* geoMedia = geoFace->getMedia();
	FairGeoMedium *FairMediumAir = geoMedia->getMedium("air");
	FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
	Int_t nmed=geoBuild->createMedium(FairMediumAir);
	TGeoVolume *top_vol = gGeoManager->MakeBox("cave", gGeoManager->GetMedium("air"), 400, 400, 1000);
	top_vol->SetVisibility(0);

	TFile* solenoid_file = new TFile("$VMCWORKDIR/geometry/FullSolenoid.root" ,"OPEN");
	TGeoVolume * solenoid = (TGeoVolume*) solenoid_file->Get("topNode");
	if (!solenoid){
		cout << " 'Error: no solenoid file found to display " << endl;
	} else {
		solenoid->SetLineColor(12);
		solenoid->SetTransparency(50);
		//top_vol->AddNode(solenoid, 1);
	}
	TFile* beampipe_file = new TFile("$VMCWORKDIR/geometry/beampipe_201309.root" ,"OPEN");
	TGeoVolume * beampipe = (TGeoVolume*) beampipe_file->Get("pipeassembly");
	if (!beampipe){
		cout << " 'Error: no beam pipe file found to display " << endl;
	} else {
		beampipe->SetTransparency(50);
		top_vol->AddNode(beampipe, 1);
	}
	TFile* lmd_setup_file = new TFile("$VMCWORKDIR/geometry/Dipole.root" ,"OPEN");
	TGeoVolume * lmd_setup = (TGeoVolume*) lmd_setup_file->Get("cave");
	if (!lmd_setup){
		cout << " 'Error: no lmd setup found to display " << endl;
	} else {
		//lmd_setup->SetLineColor(12);
		lmd_setup->SetTransparency(50);
		//top_vol->AddNode(lmd_setup, 1);
	}
	gGeoManager->SetTopVolume(top_vol);
	top_vol->Draw("ogl");

	cout << " loading panda field map for 1.5 GeV/c beam momentum" << endl;

	PndMultiField *fField= new PndMultiField();

	PndTransMap *map_t = new PndTransMap("TransMap", "R", 1.5);
	PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R", 1.5);
	PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R", 1.5);
	PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
	PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
	PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
	PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");

	fField->AddField(map_t);
	fField->AddField(map_d1);
	fField->AddField(map_d2);
	fField->AddField(map_s1);
	fField->AddField(map_s2);
	fField->AddField(map_s3);
	fField->AddField(map_s4);

	fField->Init();

	// define the valid area of the field map
	// currently the dipole area is the smallest one
	const double x_min = -30.;
	const double x_max =  30.;
	const double y_min = -30.;
	const double y_max =  30.;
	const double z_min = -172.;
	const double z_max =  660.;

	if (0){
		cout << " drawing the field bounding box" << endl;
		double x[2];
		double y[2];
		double z[2];
		TPolyLine3D* line;

		for (int ix1 = 0; ix1 < 2; ix1++){
			if (ix1) x[0] = x_min; else x[0] = x_max;
			for (int ix2 = 0; ix2 < 2; ix2++){
				if (ix2) x[1] = x_min; else x[1] = x_max;
				for (int iy1 = 0; iy1 < 2; iy1++){
					if (iy1) y[0] = y_min; else y[0] = y_max;
					for (int iy2 = 0; iy2 < 2; iy2++){
						if (iy2) y[1] = y_min; else y[1] = y_max;
						for (int iz1 = 0; iz1 < 2; iz1++){
							if (iz1) z[0] = z_min; else z[0] = z_max;
							for (int iz2 = 0; iz2 < 2; iz2++){
								if (iz2) z[1] = z_min; else z[1] = z_max;
								line = new TPolyLine3D(2, x, y, z);
								line->SetLineColor(3);
								line->Draw("same");
							}
						}
					}
				}
			}
		}
	}

	// store the starting points
	vector<double>xstart_prev;
	vector<double>ystart_prev;
	vector<double>zstart_prev;
	vector<int> line_color;
	// Initialize with a starting grid
	// field lines will be drawn into both directions of the grid
	if (xstart_prev.size()==0){
		//const Int_t n = 100;
		//create a raster for one plane in the middle of the solenoid
		//to start the field lines from
		// Check here the field strength in order to have same distances
		// for same field strength
		double pval[3] = {0.,0.,0.};
		double Bval[3] = {0.,0.,0.};
		// get the field strength perpendicular to the
		// plane in the following loop
		fField->GetFieldValue(pval, Bval);
		const double B_strength_init = fabs(Bval[2]);
		cout << " the solenoid field strength is " << B_strength_init;
		// that strength has to to correspond to a step size of 0.2 cm
		const double norm_step_size = 0.2;
		// span a x-y-plane starting from the point with
		// the strongest field
		// a rotational symmetry of the field map is assumed in all 4 quarters
		cout << " initializing solenoid grid for field lines " << endl;
		for (double iy = -2.; iy < 2.; iy += norm_step_size){
			for (double ix = -2.; ix < 2.; ix += norm_step_size){
				xstart_prev.push_back(ix);
				ystart_prev.push_back(iy);
				zstart_prev.push_back(0.);
				int color = 4;//12; // + floor(7./4.*r2);
				line_color.push_back(color);
				/*pval[0] = ix;
				pval[1] = 0.;
				pval[2] = 0.;
				fField->GetFieldValue(pval, Bval);
				double B_strength = fabs(Bval[2]);
				int mod = B_strength/B_strength_init;
				*/
			}
		}
		// take the middle of the dipole
		const double dipole_middle_z = 475.;
		const double iy = 0.;
		// create starting points for the dipole
		// start in the middle of the dipole and follow x
		// assume in the first loop a y-z mirror plane symmetry
		cout << " creating variables " << endl;
		bool print_info = true;
		bool** found_mod;
		const int max_mods = 250;
		found_mod = new bool*[max_mods];
		for (int i = 0; i < max_mods; i++)
			found_mod[i] = new bool[max_mods];
		cout << " initializing dipole grid for field lines " << endl;
		for (int dir_x = -1; dir_x < 2; dir_x += 2){
			for (int dir_z = -1; dir_z < 2; dir_z += 2){
				for (int i = 0; i < max_mods; i++){
					for (int j = 0; j < max_mods; j++){
						found_mod[i][j] = false;
					}
				}
				for (double _ix = 0.; _ix < 200.; _ix += .1){
					for (double _iz = 0.; _iz < 500.; _iz += .1){
						// skip double entries for the lines on the symmetry axis
						if ((_ix == 0 && dir_x == -1) || (_iz == 0 && dir_z == -1)) continue;
						double ix = _ix*dir_x;
						double iz = _iz*dir_z + dipole_middle_z;
						pval[0] = ix;
						pval[1] = iy;
						pval[2] = iz;
						// check the validity of the field map range
						//cout << x_min << " " << pval[0] << " " << x_max << endl;
						//cout << y_min << " " << pval[1] << " " << y_max << endl;
						//cout << z_min << " " << pval[2] << " " << z_max << endl << endl;
						if (!(x_min < pval[0] && pval[0] < x_max)||
							!(y_min < pval[1] && pval[1] < y_max)||
							!(z_min < pval[2] && pval[2] < z_max)){
							continue;
						}
						// get the field strength perpendicular to the
						// plane in the following loop
						fField->GetFieldValue(pval, Bval);
						if (print_info){
							print_info = false;
							cout << " dipole field strength to normalize to is " << Bval[1] << endl;
						}
						int mod_x = (int) floor(fabs(_ix/norm_step_size*Bval[1]/B_strength_init));
						int mod_z = (int) floor(fabs(_iz/norm_step_size*Bval[1]/B_strength_init));
						if (mod_x >= max_mods || mod_z >= max_mods || mod_x < 0 || mod_z < 0){
							cout << " Grid is out of bounds for mod x " << mod_x << " mod z " << mod_z << endl;
							continue;
						}
						if (!found_mod[mod_x][mod_z]){
							found_mod[mod_x][mod_z] = true;
							xstart_prev.push_back(ix);
							ystart_prev.push_back(iy);
							zstart_prev.push_back(iz);
							int color = 4;
							line_color.push_back(color);
						}
					}// loop over iz
				}// loop over ix
			}// loop over dir_z
		}// loop over dir_x
		cout << " deleting " << endl;
		for (int i = 0; i < max_mods; i++)
			delete[] found_mod[i];
		delete[] found_mod;
	}
	// define here the smallest grid spacing
	double d_x = 1.;
	double d_y = 1.;
	double d_z = 1.;
	cout << " drawing field lines " << endl;
	// draw field lines following the path of the field vectors
	// follow the field lines in both directions
	for (int direction = 1; direction > -2; direction -= 2){
		for (unsigned int istartpoint = 0; istartpoint < xstart_prev.size(); istartpoint++){
			int npoints = 10000;
			double stepsize = d_x;
			if (d_y < stepsize) stepsize = d_y;
			if (d_z < stepsize) stepsize = d_z;
			vector<double> xs;
			vector<double> ys;
			vector<double> zs;
			xs.push_back(xstart_prev[istartpoint]);
			ys.push_back(ystart_prev[istartpoint]);
			zs.push_back(zstart_prev[istartpoint]);
			//cout << endl;
			for (int ipoint = 1; ipoint < npoints; ipoint++){
				// get the previous position
				double pval[3] = {xs[ipoint-1],ys[ipoint-1],zs[ipoint-1]};
				// check the validity of the field map range
				//cout << x_min << " " << pval[0] << " " << x_max << endl;
				//cout << y_min << " " << pval[1] << " " << y_max << endl;
				//cout << z_min << " " << pval[2] << " " << z_max << endl << endl;
				if (!(x_min < pval[0] && pval[0] < x_max)||
					!(y_min < pval[1] && pval[1] < y_max)||
					!(z_min < pval[2] && pval[2] < z_max)){
					//if (xs.size()>2)
					//cout << " break due to out of range after " << ipoint << " points " << endl;
					break;
				}
				// get the corresponding field value
				// what is in fact the direction of the field line
				double Bval[3];
				fField->GetFieldValue(pval, Bval);
				// scale to the stepsize and add to the position
				double fieldstrength = sqrt(Bval[0]*Bval[0]+Bval[1]*Bval[1]+Bval[2]*Bval[2]);
				pval[0] = pval[0] + stepsize/fieldstrength*Bval[0]*direction;
				pval[1] = pval[1] + stepsize/fieldstrength*Bval[1]*direction;
				pval[2] = pval[2] + stepsize/fieldstrength*Bval[2]*direction;
				// store the next point
				xs.push_back(pval[0]);
				ys.push_back(pval[1]);
				zs.push_back(pval[2]);
			}
			// create a line out of the points
			if (xs.size() > 2){
				//cout << " creating polyline " << endl;
				TPolyLine3D* fieldline = new TPolyLine3D(xs.size(), &xs[0], &ys[0], &zs[0]);
				fieldline->SetLineColor(line_color[istartpoint]);
				fieldline->Draw("same");
				// mirror it
				for (unsigned int ipoint = 0; ipoint < xs.size(); ipoint++){
					xs[ipoint] = -xs[ipoint];
				}
			}
		}
	}
	myapp->Run();

}

int main(int argc, char **argv) {
	visualize_fieldmaps();
	return 0;
}

