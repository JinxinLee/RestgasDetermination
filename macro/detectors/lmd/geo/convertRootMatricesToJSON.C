#include "json.hpp"
#include <boost/filesystem.hpp>

using std::cout;
using std::string;
using json = nlohmann::json;

std::map<std::string, TGeoHMatrix> *readRootMatrices(TString filename) {

	TFile *misalignmentMatrixRootfile = new TFile(filename, "READ");
	if (misalignmentMatrixRootfile->IsOpen()) {
		std::map<std::string, TGeoHMatrix> *matrices;
		gDirectory->GetObject("PndLmdMisalignMatrices", matrices);
		misalignmentMatrixRootfile->Close();
		cout << "read " << matrices->size() << " matrices from file.\n";
		return matrices;
	}
	else {
		cout << "file could not be read\n";
		return NULL;
	}
}

void saveMatricesToJson(std::map<std::string, TGeoHMatrix> matrices, std::string outfilename) {
	
    // matrices is a map with string->TGeoHMatrix
   	json j;
    double thisMatrixDoubles[16];
    TGeoHMatrix thisMatrix;

    for(auto &path : matrices){
        thisMatrix = path.second;
        thisMatrix.GetHomogenousMatrix(thisMatrixDoubles);
        
		j[path.first] = {
			thisMatrixDoubles[0], thisMatrixDoubles[1], thisMatrixDoubles[2], thisMatrixDoubles[12],
			thisMatrixDoubles[4], thisMatrixDoubles[5], thisMatrixDoubles[6], thisMatrixDoubles[13],
			thisMatrixDoubles[8], thisMatrixDoubles[9], thisMatrixDoubles[10], thisMatrixDoubles[14],
			thisMatrixDoubles[3], thisMatrixDoubles[7], thisMatrixDoubles[11], thisMatrixDoubles[15]
		};
    }

    // save!
	cout << "save " << j.size() << " matrices to json!\n";
	std::ofstream o(outfilename.c_str());
	o << std::setw(2) << j << std::endl;
}

void convertRootMatricesToJSON(TString filename){
    
    // read root file, this will be a map<TString, TGeoHMatrix>
    std::map<std::string, TGeoHMatrix> rootMatrices = *(readRootMatrices(filename));

    // extract filename from it without extension
    boost::filesystem::path p(filename.c_str());
    fileBasename = p.stem();
    
    // save file to json
    saveMatricesToJson(rootMatrices, "wrong.json");
}

void convertRootMatricesToJSON(){
    cout << "You must specify a file name!\n";
    exit(1);
}
