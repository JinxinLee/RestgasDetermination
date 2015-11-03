/*
 * Finds pixel hit pairs and stores them to pair File
 */
using namespace std;

// g++ $(root-config --cflags --glibs) -lGeom -I${PANDAROOT}/geobase -L${PANDAROOT}/../buildPanda/lib -lGeoBase runLumiPixel2fMatrixFinder.C -o /tmp/find_matrices -I${PANDAROOT}/lmd/LmdMC -I${PANDAROOT}/lmd -I${PANDAROOT}/parbase -I${PANDAROOT}/dbase/dbInterface -I${PANDAROOT}/dbase/dbValidation -I${PANDAROOT}/dbase/dbInput -I${PANDAROOT}/lmd/LmdSensorAlignment/icp -I${PANDAROOT}/lmd/LmdSensorAlignment -I${PANDAROOT}/base/event

#include <string>
//#include <algorithm>

std::vector<std::string> list_files(const char *dirname="", const char *ext=".root"){
	std::vector<std::string> list;
	TSystemDirectory dir(dirname, dirname);
	TList *files = dir.GetListOfFiles();
	if (files) {
		TSystemFile *file;
		TString fname;
		TIter next(files);
		while ((file=(TSystemFile*)next())) {
			fname = file->GetName();
			if (!file->IsDirectory() && fname.EndsWith(ext)) {
				list.push_back(fname.Data());
			}
		}
	}
	return list;
}

/*
 * reads hundreds of files in small batches and computes matrices, outputs to ascii files
 */
void evaluateBatch(string inpath, string filename){

	//using pixel coordinates only works with non simplified pairs!

	bool storeSimple=true;
	bool runMode=false;
	bool addMode=false;
	int filesPerBatch = 300;

	//this one does all the work
	PndLmdAlignManager manager;

	std::vector<std::string> files;
	files = list_files(inpath.c_str());
	int batchIteratior=0;
	int fileIterator=0;
	int batchNumber=0;

	cout << "found " << files.size() << " files in total." << endl;
	//std::sort(files.begin(), files.end());

	//loop over all files in directory
	while(fileIterator<files.size()){

		manager.setPretend(false);

		//check if pair file
		if(files[fileIterator].find("Lumi_Pairs") != std::string::npos ){
			//add mode as long as files available or batch not full
			addMode = ((batchIteratior<filesPerBatch) && (fileIterator < files.size()));
			if(addMode){
				//cout << "found " << inpath+"/"+files[fileIterator] << endl;
				manager.addFile(inpath+"/"+files[fileIterator]);
				batchIteratior++;
			}
		}
		fileIterator++;

		//cout << "file iterator is " << fileIterator << ", list size is " << files.size() << endl;
		//run mode every nth round or at end of file list
		runMode=((fileIterator == files.size()) || (batchIteratior >= filesPerBatch));
		if(runMode){

			cout << "->   making batch " << batchNumber++ << endl;
			cout << "added " << batchIteratior << " files." << endl;
			manager.setUseSimpleStorage(storeSimple);
			stringstream outfilename;
			outfilename << inpath << "/../" << filename << "data-";
			outfilename << batchNumber << ".txt";
			manager.setOutFilename(outfilename.str());

			stringstream outfilepath;
			outfilepath << inpath << "/../" << filename << "matrices/";
			manager.setMatrixOutDir(outfilepath.str());

			manager.setInCentimeters(false);
			manager.enableHelperMatrix(false);
			manager.readFiles();
			manager.alignAllSensors();

			//reset for next batch
			manager.reinitialize();
			batchIteratior=0;
			cout << "->  done." << endl;
		}
	}
}

void runLumiPixel2fMatrixFinder(const int nEvents=00, const int startEvent=0, TString storePath="test/fullrun-1.5", const int verboseLevel=0)
{
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();

	// -----   Initialize Shit   ----------------------------------------------
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	//gSystem->Load("libboost_filesystem");
	//gSystem->Load("libboost_regex");
	//gSystem->Load("libLmdPairFinder");

	//the load order is very important!
	//gSystem->Load("libLmdPairFinder");
	//gSystem->Load("libLmdSensorAlignerTools");
	//gSystem->Load("libLmdSensorAligner");

	if(false){
		PndLmdAlignManager manager;
		manager.addFile("/home/roman/pandaroot/macro/lmd/testPixel/pairFinderTest/Lumi_Pairs_0.root");
		manager.setPretend(false);
		manager.setMatrixOutDir("/home/roman/pandaroot/macro/lmd/testPixel/pairFinderTest/matrices/");
		manager.setInCentimeters(true);
		manager.enableHelperMatrix(false);
		manager.readFiles();
		manager.alignAllSensors();

		PndLmdAlignQA qaTask;
		qaTask.setOutputPath("/home/roman/pandaroot/macro/lmd/testPixel/pairFinderTest/pdf/");
		qaTask.setInCentimeters(true);
		qaTask.setCorrection(false);
		qaTask.compareMatrices();
		return;
	}

	if(true){
		//self contained matrix finder and qa task

		string pairFile = "/home/roman/pandaroot/macro/lmd/testPixel/countTest/FullRun/Lumi_Pairs_0.root";
		string matrixDir = "/home/roman/pandaroot/macro/lmd/testPixel/countTest/FullRun/matrices/";
		string pdfDir = "/home/roman/pandaroot/macro/lmd/testPixel/countTest/FullRun/pdf/";

		PndLmdAlignManager manager;
		manager.addFile(pairFile);
		manager.setPretend(false);
		manager.setMatrixOutDir(matrixDir);
		manager.setInCentimeters(true);
		manager.enableHelperMatrix(false);
		manager.readFiles();
		manager.alignAllSensors();

		manager.enableHelperMatrix(true);
		//manager.readFiles();
		manager.alignAllSensors();

		manager.setInCentimeters(false);
		manager.enableHelperMatrix(false);
		//manager.readFiles();
		manager.alignAllSensors();

		PndLmdAlignQA qaTask;
		qaTask.setOutputPath(pdfDir);
		qaTask.setMatrixDir(matrixDir);
		qaTask.setInCentimeters(true);
		qaTask.setCorrection(false);
		qaTask.compareMatrices();
		return;
	}

	//TODO: this is just a makeshift method, real matrix finder should work on all available files
	evaluateBatch("testPixel/countTest/1.5-misaligned", "data-misaligned/1.5/");
	//evaluateBatch("testPixel/countTest/1.5", "batched30/1.5/data-");
	//evaluateBatch("testPixel/countTest/15", "batched20/15/data-");

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
	cout << endl;
}


