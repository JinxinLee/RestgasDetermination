// Panda FullSim macro

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

using namespace std;

void interpolate_fieldmap(PndFieldMap* map_to_corr,
		PndFieldMap* map_to_interpol, double weight, string fileName) {
	// Open file
	//fLogger->Info(MESSAGE_ORIGIN, "PndFieldMap: Writing field map to ASCII file %s ",fileName);
	ofstream mapFile((fileName+".dat_").c_str());
	if (!mapFile.is_open()) {
		cout << "WriteAsciiFile: Could not open file! " << endl;
		return;
	}

	// Write field map grid parameters
	mapFile.precision(4);
	mapFile << showpoint;
	if (map_to_corr->GetType() == 1)
		mapFile << "nosym" << endl;
	if (map_to_corr->GetType() == 2)
		mapFile << "Solenoid" << endl;
	if (map_to_corr->GetType() == 3)
		mapFile << "Dipole" << endl;
	if (map_to_corr->GetType() == 4)
		mapFile << "Trans" << endl;
	if (map_to_corr->GetUnit() == 10.0)
		mapFile << "T" << endl;
	if (map_to_corr->GetUnit() == 0.001)
		mapFile << "G" << endl;
	if (map_to_corr->GetUnit() == 1.0)
		mapFile << "kG" << endl;

	mapFile << map_to_corr->GetXmin() << " " << map_to_corr->GetXmax() << " "
			<< map_to_corr->GetNx() << endl;
	mapFile << map_to_corr->GetYmin() << " " << map_to_corr->GetYmax() << " "
			<< map_to_corr->GetNy() << endl;
	mapFile << map_to_corr->GetZmin() << " " << map_to_corr->GetZmax() << " "
			<< map_to_corr->GetNz() << endl;

	// Write field values
	Double_t factor = map_to_corr->GetUnit() * map_to_corr->GetScale(); // Takes out scaling
	cout << right;
	Int_t nTot = map_to_corr->GetNx() * map_to_corr->GetNy() * map_to_corr->GetNz();
	cout << "-I- PndFieldMap: " << map_to_corr->GetNx() * map_to_corr->GetNy() * map_to_corr->GetNz() << " entries to write... "
			<< setw(3) << 0 << " % ";
	Int_t index = 0;
	div_t modul;
	Int_t iDiv = TMath::Nint(nTot / 100.);

	// using linear interpolation between two values from two energies E1 and E2
	// x_E = [0,1]; f(0) = val(E1); f(1) = val(E2)
	// f(x_E) = (f(1)-f(0)) * x_E + f(0) =
	for (int x = 0; x < map_to_corr->GetNx(); x++) {
		for (int y = 0; y < map_to_corr->GetNy(); y++) {
			for (int z = 0; z < map_to_corr->GetNz(); z++) {
				index = x * map_to_corr->GetNy() * map_to_corr->GetNz() + y
						* map_to_corr->GetNz() + z;
				double Bx1 = (*(map_to_corr->GetBx()))[index];
				double Bx2 = (*(map_to_interpol->GetBx()))[index];
				double By1 = (*(map_to_corr->GetBy()))[index];
				double By2 = (*(map_to_interpol->GetBy()))[index];
				double Bz1 = (*(map_to_corr->GetBz()))[index];
				double Bz2 = (*(map_to_interpol->GetBz()))[index];
				double new_Bx = (Bx2 - Bx1) * weight + Bx1;
				double new_By = (By2 - By1) * weight + By1;
				double new_Bz = (Bz2 - Bz1) * weight + Bz1;
				//cout << " changing \t" << Bx1 << '\t' << By1 << '\t' << Bz1 << '\n' ;
				//cout << " to       \t" << new_Bx << '\t' << new_By << '\t' << new_Bz << '\n' << endl;
				modul = div(index, iDiv);
				if (modul.rem == 0) {
					Double_t perc = TMath::Nint(100. * index / nTot);
					cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
				}
				mapFile << new_Bx / factor << " " << new_By
						/ factor << " " << new_Bz / factor << endl;
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

	cout << " moving ascii file " << fileName << " to the PANDAROOT input folder " << endl;
	// move it to the input directory of PANDAROOT
    string dir = getenv("VMCWORKDIR");
    string moveto = dir + "/input/" + fileName + ".dat";
    system(("mv "+fileName+".dat"+" "+moveto).c_str());
	cout << " converting ASCII files to ROOT files " << endl;
	PndFieldMap* fieldmap_converted = NULL;
	if (map_to_corr->GetType() == 3) fieldmap_converted = new PndDipoleMap(fileName.c_str(), "A");
	if (map_to_corr->GetType() == 4) fieldmap_converted = new PndTransMap (fileName.c_str(), "A");
	if (fieldmap_converted){
		fieldmap_converted->Init();
		fieldmap_converted->WriteRootFile((fileName+".root").c_str(),fileName.c_str());
		cout << " converted " << map_to_corr->GetNx() << " x " << map_to_corr->GetNy() << " x " << map_to_corr->GetNz() << endl;
		cout << " to        " << fieldmap_converted->GetNx() << " x " << fieldmap_converted->GetNy() << " x " << fieldmap_converted->GetNz() << endl;
		cout << " values " << endl;
		delete fieldmap_converted;
	}
}

void interpolate_fieldmaps() {
	//Load basic libraries
	//gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	//PndFieldMap* fieldmap = new PndFieldMap("DipoleMap1.0150", "R");
	// pairs of values for interpolation are provided
	// the key is always the value to be corrected by interpolation
	map < string, string > momenta;
	// The weight is the position for the interpolation between 0 and 1
	map<string, double> weight;
	momenta["0150"] = "0406";
	weight["0150"] = -0.01; // extrapolate
	momenta["0406"] = "0150";
	weight["0406"] = 0.01; // interpolate
	momenta["0890"] = "0406";
	weight["0890"] = 0.01; // interpolate
	momenta["1191"] = "0890";
	weight["1191"] = 0.01; // interpolate
	momenta["1500"] = "1191";
	weight["1500"] = 0.01; // interpolate

	map<string, PndDipoleMap*> dipolefieldmaps1;
	map<string, PndDipoleMap*> dipolefieldmaps2;
	map<string, PndTransMap*> transfieldmaps;

	for (map<string, string>::iterator it = momenta.begin(); it
			!= momenta.end(); it++) {
		string momentum = it->first;
		dipolefieldmaps1[momentum] = new PndDipoleMap(
				("DipoleMap1." + (momentum)).c_str(), "R");
		dipolefieldmaps1[momentum]->Init();
		dipolefieldmaps2[momentum] = new PndDipoleMap(
				("DipoleMap2." + (momentum)).c_str(), "R");
		dipolefieldmaps2[momentum]->Init();
		transfieldmaps[momentum] = new PndTransMap(
				("TransMap." + (momentum)).c_str(), "R");
		transfieldmaps[momentum]->Init();
		//std::cout << "interpolating " << transfieldmaps[*it]->GetNx();
		//std::cout << " x " << transfieldmaps[*it]->GetNy();
		//std::cout << " x " << transfieldmaps[*it]->GetNz() << " values " << std::endl;
		//dipolefieldmaps1[momentum]->WriteAsciiFile(
		//		("Promme_test_file" + momentum + ".dat").c_str());
	}

	for (map<string, string>::iterator it = momenta.begin(); it
			!= momenta.end(); it++) {
		string momentum = it->first;
		string interpol_mom = it->second;
		cout << " interpolating maps for " << momentum << endl;
		//PndFieldMap* new_dipolefieldmap1 = new PndDipoleMap(("DipoleMap1_new."+(momentum)).c_str(), "R");
		//new_dipolefiledmap1
		interpolate_fieldmap(dipolefieldmaps1[momentum],
				dipolefieldmaps1[interpol_mom], weight[momentum], "DipoleMap1."+momentum);
	}

}

#include<TApplication.h>

int main(int argc, char **argv) {
	interpolate_fieldmaps();
	return 0;
}

