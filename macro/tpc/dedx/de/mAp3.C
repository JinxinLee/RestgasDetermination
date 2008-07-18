mAp3(TString Base)
{
	TString RawFile=Base+".raw.root";
	TFile *raw=new TFile(RawFile);
	raw->cd();
	
	TClonesArray *pDigiArray=NULL;
	cbmsim->SetBranchAddress("PndTpcDigi", &pDigiArray);
		
	Long64_t nTreeEntries=cbmsim->GetEntries();
	printf("nTreeEntries: %lld\n", nTreeEntries);

	
	TArrayL* pSDA=new TArrayL(nTreeEntries);
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
		
		//cout << SumOfDigiAmps << "   " << endl;
		pSDA->AddAt(SumOfDigiAmps, i);
		//pHist->Fill(SumOfSignalAmp, SumOfDigiAmps);
	}
	
	TString RecoFile=Base+".reco.root";
	TFile *reco=new TFile(RecoFile);
	reco->cd();
	
	TClonesArray *pClusterArray=NULL;
	cbmsim->SetBranchAddress("PndTpcCluster", &pClusterArray);
	
	nTreeEntries= cbmsim->GetEntries();
	printf("nTreeEntries: %lld\n", nTreeEntries);
	TArrayL* pSCA=new TArrayL(nTreeEntries);
	for(int i=0; i<nTreeEntries; i++)
	{
		cbmsim->GetEntry(i);
		Int_t nCluster=pClusterArray->GetEntriesFast();
		Int_t SumOfClusterAmps=0;
		for(Int_t j=0; j < nCluster; j++)	
		{
			PndTpcCluster *pCluster=(PndTpcCluster*)pClusterArray->At(j);
		     SumOfClusterAmps+=pCluster->amp();
		}
		//cout << SumOfClusterAmps << "   " << endl;
		pSCA->AddAt(SumOfClusterAmps, i);
		//pHist->Fill(SumOfSignalAmp, SumOfDigiAmps);
	}
	
	
	cout << "here" << endl;
	TH2F *pHist = new TH2F("SumDigiAmpSumClusterAmp","SumDigiAmpSumClusterAmp",500,0,20000000,500,0,20000000);	
	cout << "here2" << endl;
	for(int i=0; i<nTreeEntries; i++)
	{
		//pHist->Fill(0,0);
		pHist->Fill(pSDA->At(i), pSCA->At(i));
	}
	cout << "here3" << endl;
	
	TCanvas *p3=new TCanvas("Canvas3", "Digi Amp vs Cluster Amp");
	pHist->Draw();
}