void NeutronCombinedAnalysis(TString Filename = "Combined_AnaGeo36urqmd_500000Evts.root")
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/hypGe/Marcell/SharedMacros/SharedMacroFunctions.C");
	TString Path = getenv("SIMDATADIR");
	TString FullPath= Path+"/Neutron/Ana/CombinedData/";
	TString FilenameWithPath= FullPath+Filename;

	int GeoNumber =-1;
	int TotalEvents=-1;

	sscanf(Filename.Data(),"Combined_AnaGeo%d%*5s_%dEvts.root",&GeoNumber,&TotalEvents);

	TFile *file = new TFile(FilenameWithPath);
		TH1D* hCrystalHit;
		file ->GetObject("hCrystalHit",hCrystalHit);

	cout << hCrystalHit->GetEntries() << endl;

	TString TxTOutFilename =FilenameWithPath + ".txt";
	ofstream TxTOutfile(TxTOutFilename.Data());
	Int_t CrystalNumber = 1;
	cout << "Bin\tCrystal\tCluster\tNeutron hits"<<endl;
	TxTOutfile << "Bin\tCrystal\tCluster\tNeutron hits"<<endl;
	int MaxCrystalHits = -1;
	for (Int_t iBin=0;iBin<2100;iBin++)
	{

		if(hCrystalHit->GetBinContent(iBin))
		{
			cout << iBin << "\t"<< CrystalNumber <<"\t"<< iBin/100 <<"\t"<< hCrystalHit->GetBinContent(iBin)<< endl;

			TxTOutfile << iBin << "\t"<< CrystalNumber <<"\t"<< iBin/100 <<"\t"<< hCrystalHit->GetBinContent(iBin)<< endl;
			CrystalNumber++;
			if (MaxCrystalHits <hCrystalHit->GetBinContent(iBin))
				MaxCrystalHits =hCrystalHit->GetBinContent(iBin);
		}
	}

	Double_t AverageCrystalHits = hCrystalHit->GetEntries()/CrystalNumber;
	cout << "Average Hits:\n";
	TxTOutfile << "Average Hits:\n";
	cout << AverageCrystalHits<<endl;
	TxTOutfile << AverageCrystalHits<<endl;
	cout << "Max Hits:\n";
	TxTOutfile << "Max Hits:\n";
	cout << MaxCrystalHits<<endl;
	TxTOutfile << MaxCrystalHits<<endl;
	cout << "Total Events:\n";
	TxTOutfile << "Total Events:\n";
	cout << TotalEvents<<endl;
	TxTOutfile << TotalEvents<<endl;

	TxTOutfile.close();
}
