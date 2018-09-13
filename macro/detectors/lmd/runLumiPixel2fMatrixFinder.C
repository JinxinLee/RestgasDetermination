/*
 *	@author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

// initialize a minimal, boiler plate FairRunAna so the gGeoManager loads the geometry
void initMiniAna(TString storePath) {

	//FIXME: don't hard code starting event!
	TString inFile = storePath + "/Lumi_MC_1000000.root";
	TString parFile = storePath + "/Lumi_Params_1000000.root";
	TString outFile = storePath + "/Lumi_digi_0-dummy.root";
	FairRunAna *fRun = new FairRunAna();
	FairFileSource *input_source = new FairFileSource(inFile);
	fRun->SetSource(input_source);
	fRun->SetOutputFile(outFile);
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data(), "in");
	rtdb->setFirstInput(parInput1);
	FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
	rtdb->setSecondInput(parInput2);
	fRun->Init();
}

void cleanup(){
	// -----   Finish   -------------------------------------------------------
	// temporary fix to avoid double frees at the destruction of te program for pandaroot/fairroot with root6
	gGeoManager->GetListOfVolumes()->Delete();
	gGeoManager->GetListOfShapes()->Delete();
	delete gGeoManager;

}

int runLumiPixel2fMatrixFinder(TString pairFilePath = "tmpOutput", TString binaryPairFilePath = "",
    TString LMDmatrixDir = "", bool incentimeters = true, const int verboseLevel = 2) {
	// -----   Timer   --------------------------------------------------------

	//load neccessary libraries
	//gSystem->Load("libLmd");
	//gSystem->Load("libLmdSensorAligner");

	//FIXME: don't hard code starting event!
	cout << "populating gGeoManager with geometry data...\n";

	initMiniAna(pairFilePath);

	// don't forget trailing slashes!!
	if (LMDmatrixDir == "") {
		LMDmatrixDir = pairFilePath + "/LMDmatrices/";
	}
	if (binaryPairFilePath == "") {
		binaryPairFilePath = pairFilePath + "/binaryPairs/";
	}

	int readNoOfFiles = 0;			//how many files should be processed? 0 for all

	PndLmdAlignManager manager;

	manager.setInCentimeters(incentimeters);
	manager.setBinaryPairFileDirectory(binaryPairFilePath.Data());
	manager.setMaxPairs(7e5);
	manager.verboseLevel(verboseLevel);
	manager.setMaxThreads(16);

	cout << "pairFilePath: " << pairFilePath << "\n";
	cout << "binaryPairFilePath: " << binaryPairFilePath << "\n";
	cout << "LMDmatrixDir: " << LMDmatrixDir << "\n";

	// ---------------------- check for binary files and sort/write, if necessary
	cout << "looking for binary pair files...\n";
	bool binaryPairsPresent = manager.checkForBinaryFiles();
	if (!binaryPairsPresent) {
		cout << "None found, reading pair root files...\n";

		manager.addFilesFromDirectory(pairFilePath.Data(), readNoOfFiles);
		manager.setMatrixOutDir(LMDmatrixDir.Data());
		manager.readFilesAndAlign();
	}
	else {
		cout << "binary pair files found.\n";
		//check for LMD Matrix Files
		bool LMDMatrixFilesPresent = manager.checkForLmdMatrixFiles();
		if (!LMDMatrixFilesPresent) {
			cout << "reading binary pair files.\n";
			manager.setMatrixOutDir(LMDmatrixDir.Data());
			manager.readPairsFromBinaryFilesAndAlign();
		}
	}

	//manager.computeCombinedMatrices();
	// ---------------------- compute absolute correction matrices from overlap matrices and store to PndLmdDim format, compare to pndLmdDim
	//TODO: well, this ^
	// steps:
	/*
	 * gather all modules
	 * gather all overlapping areas for each module
	 * compute all combined matrices for each module (this can be done multi-threadedly)
	 * store them in a good way (how?)
	 *
	 */


	//cleanup();
	return 0;
}

