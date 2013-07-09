// Panda fieldmaps created with tosca by jost luehning are read
// and transformed to panda compatible fieldmaps

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

using namespace std;

// for testing purposes the default
// field map name is modified
// Moreover in case of setting a string of
// length > 0
// files are stored in the $VMCWORKDIR/input/ folder
const string name_modification = "";

vector<string> fieldmapnames;

TTree* gtree_values(NULL);

double gBx, gBy, gBz, gDBx, gDBy, gDBz;

// read fem calculated fieldmap and transform it
// 	fieldtype 1: no symetry
//  fieldtype 2: Solenoidfieled
//  fieldtype 3: Dipolefield
//  fieldtype 4: Transition region
// filename_out is the pandaroot filename without the extenstion
// in contrast to filename_in having the extension
void transform_jost_fieldmap(string filename_in, string filename_out, int fieldtype){
	cout << " reading file " << filename_in << endl;
	ifstream jostmapFile(filename_in.c_str());
	if (!jostmapFile.is_open()) {
		cout << "ReadAsciiFile: Could not open file! " << endl;
		return;
	}
	// coordinate min max ranges
	double x_min = -1.e9;
	double x_max = -1.e9;
	double y_min = -1.e9;
	double y_max = -1.e9;
	double z_min = -1.e9;
	double z_max = -1.e9;
	// number of entries
	int n_x   = 0;
	int n_y   = 0;
	int n_z   = 0;
	// spacing (maximum allowed precision is 1 mm)
	int d_x   = 0;
	int d_y   = 0;
	int d_z   = 0;
	// store the grid axes
	map <double, int> x_counts;
	map <double, int> y_counts;
	map <double, int> z_counts;
	map <double, int>::iterator it_counts, it_counts_prev;
	map <string, TVector3> field_in;
	map <string, TVector3>::iterator it_field_in;
	// first pass: analyzing parameters
	while (1)
    {
		string line;
		getline (jostmapFile , line);
		if (!jostmapFile.good()) break;
		// go through the line
		bool isvalidline = true;
		// I'm expecting 6 entries x,y,z and Bx, By, Bz;
		char value[100];
		value[0] = '\0';
		unsigned int ivalchar = 0;
		double values[6];
		unsigned int ival = 0;
		stringstream key;
		for (unsigned int ichar = 0; ichar < line.size(); ichar++){
			if (line[ichar] == '#'){
				isvalidline = false;
				break;
			}
			//if (line[ichar] == '\n' || line[ichar] == '\0'){
				//ival = 0;
			//	break;
			//}
			if (line[ichar] == ' ' || line[ichar] == '\n' || line[ichar] == '\0' || ichar == line.size()-1){
				if (value[0] != '\0' && ivalchar != 0){
					if (ival > 5) {
						cout << " hmm, more than 6 values per line? -> Fatal " << endl;
						break;
					}
					value[ivalchar] = '\n';
					values[ival] = atof(value);
					value[0] = '\0';
					ivalchar = 0;
					ival++;
				}
				if (line[ichar] == '\n' || line[ichar] == '\0'){
					ival = 0;
					break;
				} else continue;
			}
			value[ivalchar] = line[ichar];
			ivalchar++;
		}
		if (ival != 6 && ival > 0 && isvalidline) {
			cout << " not enough values in this line!!! -> Fatal " << ival << endl;
			cout << line << endl;
			continue;
		} else {
			if (isvalidline){
				// analyze the values
				if (x_min == -1.e9 || values[0] < x_min) x_min = values[0];
				if (x_max == -1.e9 || values[0] > x_max) x_max = values[0];
				if (y_min == -1.e9 || values[1] < y_min) y_min = values[1];
				if (y_max == -1.e9 || values[1] > y_max) y_max = values[1];
				if (z_min == -1.e9 || values[2] < z_min) z_min = values[2];
				if (z_max == -1.e9 || values[2] > z_max) z_max = values[2];
				x_counts[values[0]]++;
				y_counts[values[1]]++;
				z_counts[values[2]]++;
				// maximum resolution in the grid spacing will be 1 mm
				key << ((int)floor(values[0])) << "_" << ((int)floor(values[1])) << "_" << ((int)floor(values[2]));
				TVector3 fieldval(values[3], values[4], values[5]);
				field_in[key.str()] = fieldval;
			}
		}
    }
	jostmapFile.close();

	cout << " finished reading " << endl;

	n_x = x_counts.size();
	n_y = y_counts.size();
	n_z = z_counts.size();
	d_x = (int)floor((x_max - x_min)/(n_x-1));
	d_y = (int)floor((y_max - y_min)/(n_y-1));
	d_z = (int)floor((z_max - z_min)/(n_z-1));

	cout << "grid size is " << n_x << " X " << n_y << " X " << n_z << endl;
	cout << "grid spacing is [mm] " << d_x << " " << d_y << " " << d_z << endl;
	cout << x_min << " < x [mm] < " << x_max << endl;
	cout << y_min << " < y [mm] < " << y_max << endl;
	cout << z_min << " < z [mm] < " << z_max << endl;

	cout << "checking consistency " << endl;

	// get spacing and check grid in the file
	for (it_counts = x_counts.begin(); it_counts != x_counts.end(); it_counts++){
		if (it_counts == x_counts.begin()) {
			it_counts_prev = it_counts;
			continue;
		}
		double spacing = it_counts->first - it_counts_prev->first;
		if (fabs(spacing - d_x) > 1e-5)
			cout << " warning: grid spacing does not match for dx: " << spacing << " vs " << d_x << endl;
		if (it_counts->second != it_counts_prev->second)
			cout << " warning: grid is not fully determined in x! " << endl;
		it_counts_prev = it_counts;
	}
	for (it_counts = y_counts.begin(); it_counts != y_counts.end(); it_counts++){
		if (it_counts == y_counts.begin()) {
			it_counts_prev = it_counts;
			continue;
		}
		double spacing = it_counts->first - it_counts_prev->first;
		if (fabs(spacing - d_y) > 1e-5)
			cout << " warning: grid spacing does not match for dy: " << spacing << " vs " << d_y << endl;
		if (it_counts->second != it_counts_prev->second)
			cout << " warning: grid is not fully determined in y! " << endl;
		it_counts_prev = it_counts;
	}
	for (it_counts = z_counts.begin(); it_counts != z_counts.end(); it_counts++){
		if (it_counts == z_counts.begin()) {
			it_counts_prev = it_counts;
			continue;
		}
		double spacing = it_counts->first - it_counts_prev->first;
		if (fabs(spacing - d_z) > 1e-5)
			cout << " warning: grid spacing does not match for dz: " << spacing << " vs " << d_z << endl;
		if (it_counts->second != it_counts_prev->second)
			cout << " warning: grid is not fully determined in z! " << endl;
		it_counts_prev = it_counts;
	}
	cout << " done " << endl;
	// now we can try to transform the maps
	// units are assumed to be gaus and mm
	// pandaroot needs kGaus and cm

	cout << "writing " << filename_out << " files" << endl;
	ofstream mapFile((filename_out+".dat").c_str());
		if (!mapFile.is_open()) {
			cout << "WriteAsciiFile: Could not open file! " << endl;
			return;
		}

		// Write field map grid parameters
		mapFile.precision(6);
		mapFile << showpoint;
		if (fieldtype == 1)
			mapFile << "nosym" << endl;
		if (fieldtype == 2)
			mapFile << "Solenoid" << endl;
		if (fieldtype == 3)
			mapFile << "Dipole" << endl;
		if (fieldtype == 4)
			mapFile << "Trans" << endl;
		//if (map_to_corr->GetUnit() == 10.0)
		//	mapFile << "T" << endl;
		//if (map_to_corr->GetUnit() == 0.001)
		double unit = 0.001;
			mapFile << "G" << endl; // units are assumed in Gauss
		//if (map_to_corr->GetUnit() == 1.0)
		//	mapFile << "kG" << endl;

		mapFile << x_min/10. << " " << x_max/10. << " "
				<< n_x << endl;
		mapFile << y_min/10. << " " << y_max/10. << " "
				<< n_y << endl;
		mapFile << z_min/10. << " " << z_max/10. << " "
				<< n_z << endl;

		// Write field values
		double scale = 1.;
		Double_t factor = unit * scale;
		cout << right;
		Int_t nTot = n_x * n_y * n_z;
		cout << "-I- PndFieldMap: " << n_x * n_y * n_z << " entries to write... "
				<< setw(3) << 0 << " % ";
		Int_t index = 0;
		div_t modul;
		Int_t iDiv = TMath::Nint(nTot / 100.);

		for (int x = 0; x < n_x; x++) {
			for (int y = 0; y < n_y; y++) {
				for (int z = 0; z < n_z; z++) {
					index = x * n_y * n_z + y
							* n_z + z;
					//double Bx1 = (*(map_to_corr->GetBx()))[index];
					//double Bx2 = (*(map_to_interpol->GetBx()))[index];
					//double By1 = (*(map_to_corr->GetBy()))[index];
					//double By2 = (*(map_to_interpol->GetBy()))[index];
					//double Bz1 = (*(map_to_corr->GetBz()))[index];
					//double Bz2 = (*(map_to_interpol->GetBz()))[index];
					stringstream key;
					key << (int)floor(x_min + x * d_x) << "_";
					key << (int)floor(y_min + y * d_y) << "_";
					key << (int)floor(z_min + z * d_z);
					it_field_in = field_in.find(key.str());
					if (it_field_in != field_in.end()){
						double new_Bx = it_field_in->second.X();
						double new_By = it_field_in->second.Y();
						double new_Bz = it_field_in->second.Z();

					//cout << " changing \t" << Bx1 << '\t' << By1 << '\t' << Bz1 << '\n' ;
					//cout << " to       \t" << new_Bx << '\t' << new_By << '\t' << new_Bz << '\n' << endl;
						modul = div(index, iDiv);
						if (modul.rem == 0) {
							Double_t perc = TMath::Nint(100. * index / nTot);
							cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
						}
						//mapFile << new_Bx / factor << " " << new_By
						//		/ factor << " " << new_Bz / factor << endl;
						mapFile << new_Bx << " " << new_By << " " << new_Bz << endl;
					} else {
						cout << " fatal: no value to key " << key << " found!" << endl;
					}
				}
			}
		}

	//	for (Int_t ix = 0; ix < fNx; ix++) {
	//		for (Int_t iy = 0; iy < fNy; iy++) {
	//			for (Int_t iz = 0; iz < fNz; iz++) {
	//				index = ix * fNy * fNz + iy * fNz + iz;
	//			} // z-Loop
	//		} // y-Loop
	//	} // x-Loop
		cout << "   " << index + 1 << " written" << endl;
		mapFile.close();

		// test
		//map_to_corr->WriteAsciiFile((fileName+".dat").c_str());

		// It is indeed annoying that I did not find any accessors to the field values to
		// modify. So I have to write an ascii file, read it and save it as a root file.

		cout << " moving ascii file " << filename_out << " to the PANDAROOT input folder " << endl;
		// move it to the input directory of PANDAROOT
	    string dir = getenv("VMCWORKDIR");
	    string moveto = dir + "/input/" + filename_out + ".dat";
	    system(("mv "+filename_out+".dat"+" "+moveto).c_str());
		cout << " converting ASCII files to ROOT files " << endl;
		PndFieldMap* fieldmap_converted = NULL;
		//gfield = 0;
		if (fieldtype == 2) {
			fieldmap_converted = new PndSolenoidMap(filename_out.c_str(), "A");
			//gfield = 3;
		}
		if (fieldtype == 3) {
			fieldmap_converted = new PndDipoleMap(filename_out.c_str(), "A");
			//gfield = 3;
		}
		if (fieldtype == 4) {
			fieldmap_converted = new PndTransMap (filename_out.c_str(), "A");
			//gfield = 4;
		}
		if (fieldmap_converted){
			fieldmap_converted->Init();
			stringstream _filename;
			_filename << filename_out;// << "_" <<
			fieldmap_converted->WriteRootFile((_filename.str()+".root").c_str(),_filename.str().c_str());
			cout << " converted " << n_x << " x " << n_y << " x " << n_z << endl;
			cout << " to        " << fieldmap_converted->GetNx() << " x " << fieldmap_converted->GetNy() << " x " << fieldmap_converted->GetNz() << endl;
			cout << " values " << endl;
		}
}

void read_jost_fieldmaps() {
	//Load basic libraries
	//gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	transform_jost_fieldmap("s1301_z-172_hsc.dat", "SolenoidMap1", 2);
	transform_jost_fieldmap("s1301_z-40_hsc.dat", "SolenoidMap2", 2);
	transform_jost_fieldmap("s1301_z180_hsc.dat", "SolenoidMap3", 2);
	transform_jost_fieldmap("s1301_z248_hsc.dat", "SolenoidMap4", 2);
	transform_jost_fieldmap("p1301_z283_hsc_0150.dat", "TransMap.0150", 4);
}

#include<TApplication.h>

int main(int argc, char **argv) {
	read_jost_fieldmaps();
	return 0;
}

