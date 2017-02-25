int combineGridDC02Data(Int_t runNr)	//give only the initial number (1,2,3)
{
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	//gSystem->Load("libEve");
	//gROOT->LoadMacro("DrawEveHisto.C");


	PndMvdAnaRadDam radAna;

	for (int i = 0; i < 8; i++){
		std::stringstream fileName;
		fileName << "./radDamageData/run";
		fileName << runNr << "0" << i;
		fileName << "/Mvd_GridUrqmdAna_AnalyzedData.root";
		radAna.AddFile(fileName.str().c_str());
	}

	radAna.AnalyzeFiles();
	std::stringstream outputName;
	outputName << "./radDamageData/" << "Mvd_GridUrqmdAna_" << runNr << ".root";
	radAna.SaveHistos(outputName.str().c_str());
  return 0;
}
