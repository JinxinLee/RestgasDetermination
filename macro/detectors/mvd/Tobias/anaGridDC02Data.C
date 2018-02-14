int anaGridDC02Data(Int_t runNr, Int_t start = -1)
{
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	//gSystem->Load("libEve");
	//gROOT->LoadMacro("DrawEveHisto.C");


	PndMvdAnaRadDam radAna;

	Int_t startIndex, stopIndex;
	if (start < 0){
		startIndex = 1;
		stopIndex = 501;
	}
	else {
		startIndex = start * 100 + 1;
		stopIndex = startIndex + 100;
	}

	for (int i = startIndex; i < stopIndex; i++){
		std::stringstream fileName;
		fileName << "./radDamageData/run";
		fileName << runNr;
		fileName << "/Mvd_GridUrqmdAna_";
		fileName << i;
		fileName << ".root";
		radAna.AddFile(fileName.str().c_str());
	}


	radAna.AnalyzeFiles();
	std::stringstream outputName;
	outputName << "./radDamageData/run" << runNr;
	if (start < 0) outputName << "/Mvd_GridUrqmdAna_AnalyzedData.root";
	else outputName << "/Mvd_GridUrqmdAna_AnalyzedData_" << startIndex << "-" << stopIndex-1 << ".root";
	radAna.SaveHistos(outputName.str().c_str());
  return 0;
}
