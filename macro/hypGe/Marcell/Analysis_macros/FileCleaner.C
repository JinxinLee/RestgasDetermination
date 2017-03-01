int FileCleaner(TString InputFile = "/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Combined_Ana_Geo43_E0.5189MeV_Evts2860000_FileEvts11440_Gen1_ST12_mu-2.5,Q-2.8_OQP_Psf1.5.root")
{
	TString FullFilename = InputFile;
	//TString FullFilename = "/data/work/kpha1/steinen/Gamma/Ana/CombinedData/"+InputFile;
	int ComboNumber = 250;
	char buffer[100];
	TFile *File = new TFile(FullFilename.Data(),"update");

	for (int i = 1; i<=ComboNumber;i++)
	{
		sprintf(buffer,"cbmout;%d",i);
		gDirectory->Delete(buffer);
		sprintf(buffer,"FileHeader;%d",i);
		gDirectory->Delete(buffer);
	}

	gDirectory->Delete("BranchList;1");
	gDirectory->Delete("cbmsim;1");
	File->Close();
  return 0;
}
