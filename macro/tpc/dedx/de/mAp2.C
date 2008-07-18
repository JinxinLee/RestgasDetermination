mAp2(TString Base)
{
	TString RawFile=Base+".raw.root";
	TFile *raw=new TFile(RawFile);
	raw->cd();
	
	TClonesArray *pDigiArray=NULL;
	cbmsim->SetBranchAddress("PndTpcDigi", &pDigiArray);
	
	TClonesArray *pSignalArray=NULL;
	cbmsim->SetBranchAddress("PndTpcSignal", &pSignalArray);
		
	Long64_t nTreeEntries=cbmsim->GetEntries();
	printf("nTreeEntries: %lld\n", nTreeEntries);

	
	TH2F *pHist = new TH2F("SumSigAmpSumDigiAmp","SumSigAmpSumDigiAmp", 5000, 0,60000000,500,0,30000000);

	for(int i=0; i<nTreeEntries; i++)
	{
		cbmsim->GetEntry(i);
		Int_t nDigi=pDigiArray->GetEntriesFast();
		Int_t SumOfDigiAmps=0;
		for(Int_t j=0; j < nDigi; j++)	
		{
			PndTpcDigi *pDigi=(PndTpcDigi*)pDigiArray->At(j);
		     SumOfDigiAmps+=pDigi->amp();
		}
		
		Int_t nSignals=pSignalArray->GetEntriesFast();
		Int_t SumOfSignalAmp=0;
		for(Int_t j=0; j < nSignals; j++)			
		{
			PndTpcSignal *pSignal=(PndTpcSignal*)pSignalArray->At(j);
			SumOfSignalAmp+=pSignal->amp();
		}
		cout << SumOfSignalAmp << "   " << SumOfDigiAmps << "   " << endl;
		pHist->Fill(SumOfSignalAmp, SumOfDigiAmps);
	}
	TCanvas *p2=new TCanvas("Canvas2", "signal Amp vs Digi Amp");
	pHist->Draw();
	TCanvas *pro2=new TCanvas("ProCanvas2", "signal Amp vs Digi Amp ProfileX");
	TProfile *pPro=pHist->ProfileX("_pfx", -1, -1, "b");
	assert(pPro);
	pPro->Draw();
	TF1 *f1 = new TF1("f1","pol1", 0, 20000000);
	f1->FixParameter(0,0);
     pPro->Fit("f1", "R");
}