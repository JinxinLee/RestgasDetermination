mAp1(TString Base)
{
	TString RawFile=Base+".raw.root";
	TFile *raw=new TFile(RawFile);
	raw->cd();

	TClonesArray *pPrimaryClusterArray=NULL;
	cbmsim->SetBranchAddress("PndTpcPrimaryCluster", &pPrimaryClusterArray);
	
	TClonesArray *pSignalArray=NULL;
	cbmsim->SetBranchAddress("PndTpcSignal", &pSignalArray);
		
	Long64_t nTreeEntries=cbmsim->GetEntries();
	printf("nTreeEntries: %lld\n", nTreeEntries);

	
	TH2F *pHist = new TH2F("SumPrimeSumSigAmp","SumPrimeSumSigAmp", 1000, 0,10000,5000,0,50000000);

	for(int i=0; i<nTreeEntries; i++)
	{
		cbmsim->GetEntry(i);
		Int_t nPrimaryCluster=pPrimaryClusterArray->GetEntriesFast();
		Int_t SumOfPrimaryElectrons=0;
		for(Int_t j=0; j < nPrimaryCluster; j++)	
		{
			PndTpcPrimaryCluster *pPrimaryCluster=(PndTpcPrimaryCluster*)pPrimaryClusterArray->At(j);
		     SumOfPrimaryElectrons+=pPrimaryCluster->q();
		}
		
		Int_t nSignals=pSignalArray->GetEntriesFast();
		Int_t SumOfSignalAmp=0;
		for(Int_t j=0; j < nSignals; j++)			
		{
			PndTpcSignal *pSignal=(PndTpcSignal*)pSignalArray->At(j);
			SumOfSignalAmp+=pSignal->amp();
		}
		//cout << SumOfPrimaryElectrons << "   " << SumOfSignalAmp << "   " << endl;
		pHist->Fill(SumOfPrimaryElectrons,SumOfSignalAmp);
	}
	TCanvas *p1=new TCanvas("Canvas1","prim e- vs. signal Amp");
	pHist->Draw();
}