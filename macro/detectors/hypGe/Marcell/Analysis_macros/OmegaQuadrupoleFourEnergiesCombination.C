TString CompleteFilename(TString mu,
		TString Q,
		TString Psf
			)
{
	TString FileEnd = "mu"+mu+",Q"+Q+"_OQP_Psf"+Psf+".root";
	return FileEnd;

}

Double_t GetWidthkeV(TF1 *func, Double_t Whichwidth=0.5)
{
	Double_t Max = func->GetMaximum();
	//cout << "max"<<Max<<endl;
	//cout << "Xmax"<<func->GetMaximumX()<<endl;

	Double_t XMin = func->GetX(Max*Whichwidth,func->GetMaximumX()-0.20,func->GetMaximumX());
	Double_t XMax = func->GetX(Max*Whichwidth,func->GetMaximumX(),func->GetMaximumX()+0.20);

	return (XMax-XMin)*1000;
}




Double_t QuadGausPlusStep (Double_t *x,Double_t *par)
{

			Double_t Gausian1 = par[0] * TMath::Exp(- 0.5*pow((x[0] - par[1])/par[2],2)); // par[0]: amplitude, par[1]: x[0]0; par[2]: sigma
			Double_t Gausian2 = par[3] * TMath::Exp(- 0.5*pow((x[0] - par[4])/par[5],2)); // par[0]: amplitude, par[1]: x[0]0; par[2]: sigma
			Double_t Gausian3 = par[6] * TMath::Exp(- 0.5*pow((x[0] - par[7])/par[8],2)); // par[0]: amplitude, par[1]: x[0]0; par[2]: sigma
			Double_t Gausian4 = par[9] * TMath::Exp(- 0.5*pow((x[0] - par[10])/par[11],2)); // par[0]: amplitude, par[1]: x[0]0; par[2]: sigma
			Double_t SmoothedStep = par[12] * TMath::Erfc( (x[0] - par[1]) /(TMath::Sqrt(2)*par[2])); // par[3]: amplitude

	return Gausian1+Gausian2+Gausian3+Gausian4+SmoothedStep ;
}


int OmegaQuadrupoleFourEnergiesCombination(
			TString mu = "-2.5",
			TString Q = "-2.8",
			TString Psf = "1",
			TString Filename1 ="/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Combined_Ana_Geo43_E0.5189MeV_Evts2860000_FileEvts11440_Gen1_ST12_mu-2.5,Q-2.8_OQP_Psf1.root",
			TString Filename2 ="/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Combined_Ana_Geo43_E0.5198MeV_Evts2580000_FileEvts10320_Gen1_ST12_mu-2.5,Q-2.8_OQP_Psf1.root",
			TString Filename3 ="/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Combined_Ana_Geo43_E0.5208MeV_Evts2330000_FileEvts9320_Gen1_ST12_mu-2.5,Q-2.8_OQP_Psf1.root",
			TString Filename4 ="/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Combined_Ana_Geo43_E0.5215MeV_Evts2110000_FileEvts8440_Gen1_ST12_mu-2.5,Q-2.8_OQP_Psf1.root"
	)
{
//	TString Filename1 ="Combined_Ana_Geo43_E0.5*MeV_Evts2860000_FileEvts11440_Gen1_ST12_"+CompleteFilename(mu,Q,Psf);
//	TString Filename2 ="Combined_Ana_Geo43_E0.5*MeV_Evts2580000_FileEvts10320_Gen1_ST12_"+CompleteFilename(mu,Q,Psf);
//	TString Filename3 ="Combined_Ana_Geo43_E0.5*MeV_Evts2330000_FileEvts9320_Gen1_ST12_"+CompleteFilename(mu,Q,Psf);
//	TString Filename4 ="Combined_Ana_Geo43_E0.5*MeV_Evts2110000_FileEvts8440_Gen1_ST12_"+CompleteFilename(mu,Q,Psf);


	andi::setCustomStyle();

	TString Path = "/data/work/kpha1/steinen/Gamma/Ana/CombinedData/";
	TString Filenames[4];

		Filenames[0]=Filename1;
		Filenames[1]=Filename2;
		Filenames[2]=Filename3;
		Filenames[3]=Filename4;

		cout<< Filenames[0].Data()<<endl;
	TFile *InputFiles[4];
	TFile *OutputFile;
	TH1D *hAddUp;// = new TH1D("hGammaSpectrumCombined","Combined Gamma spectrum; Energy [MeV];Counts [a.u.]",7000,1,0.7)
	TH1D *hTemporary[4];
	TF1 *fGausFit[4];
	//TF1 *fGausFitCombined = new TF1("fGausFitCombined","gaus(0)+gaus(3)+gaus(6)+gaus(9)+pol2(12)",0.515,0.525);
	TF1 *fGausFitCombined = new TF1("fGausFitCombined",QuadGausPlusStep,0.515,0.525,13);
	TString Fitname;
	TCanvas *canCombo= new TCanvas("cCombo","cCombo",800,600);
	TCanvas *canAdd= new TCanvas("cAdd","cAdd",800,600);
	TCanvas *can[4];
	can[0]=new TCanvas("c0","c0",800,600);
	can[1]=new TCanvas("c1","c1",800,600);
	can[2]=new TCanvas("c2","c2",800,600);
	can[3]=new TCanvas("c3","c3",800,600);
	for (int i = 0; i<4; i++)
	{
		InputFiles[i]= new TFile(Filenames[i].Data());
		InputFiles[i]->GetObject("GamEnergy;1",hTemporary[i]);
		hTemporary[i]->SetDirectory(0);
		InputFiles[i]->Close();
		canCombo->cd();

		hTemporary[i]->Draw();
		can[i]->cd();
		Fitname.Form("fGausFit%d",i);
		fGausFit[i]= new TF1(Fitname.Data(),"gaus",0.515,0.525);
		hTemporary[i]->Fit(Fitname.Data(),"r");
		for (int j=0;j<3;j++)
			fGausFitCombined->SetParameter(3*i+j,fGausFit[i]->GetParameter(j));
		fGausFitCombined->FixParameter(1+3*i,fGausFitCombined->GetParameter(1+3*i));
		hTemporary[i]->Draw("");
	}
	hAddUp = new TH1D("hAddUp","4 lines added up;Energy [MeV];Counts [a.u.]",6000,0,0.6);
	for(int i = 0; i<4;i++)
	{
		hAddUp->Add(hTemporary[i],1);
	}
	canAdd->cd();
	fGausFitCombined->SetLineColor(kRed);
	hAddUp->Fit("fGausFitCombined","r");
	//hAddUp->Draw();

	//fGausFitCombined->Draw("same");

	for(int i = 0; i<4;i++)
		{
		fGausFit[i]->Draw("same");
		cout <<"gaus " << i << endl;
		//cout << fGausFit[i]->GetParameter(0) << "\t" << fGausFitCombined->GetParameter(0+i*3) << endl;
		//cout << fGausFit[i]->GetParameter(1) << "\t" << fGausFitCombined->GetParameter(1+i*3) << endl;
		//cout << fGausFit[i]->GetParameter(2) << "\t" << fGausFitCombined->GetParameter(2+i*3) << endl;
		}

	Double_t Mean =fGausFitCombined->Mean(0.516,0.524);
	Double_t StandardDev =sqrt(fGausFitCombined->Variance(0.516,0.524));
	//Double_t Dip = fGausFitCombined->GetMinimumX(fGausFit[1]->GetParameter(1),fGausFit[2]->GetParameter(1));			// the dip does not work for mu = -2.0, there is simply no dip!!!
	Double_t Skewness = fGausFitCombined->CentralMoment(3,0.516,0.524);
	Double_t Kurtosis = fGausFitCombined->CentralMoment(4,0.516,0.524);
	Double_t MaxX = fGausFitCombined->GetMaximumX(0.516,0.524);
	cout <<GetWidthkeV(fGausFitCombined,0.5)<<endl;
	cout <<GetWidthkeV(fGausFitCombined,0.3)<<endl;
	cout <<GetWidthkeV(fGausFitCombined,0.1)<<endl;
	cout <<"Variance"<<StandardDev<<endl;
	cout <<"Mean"<<Mean<<endl;
	//cout << "Dip"<< Dip<<endl;
	cout <<"Skewness"<<Skewness<<endl;
	cout <<"Kurtosis"<<Kurtosis<<endl;

	//cout << "mu" << "\t"<<"Q"<<"\t"<<"Psf"<< "\t"<< "Mean" << "\t" << "StandardDev" << "\t" << "Dip" << endl;
	//cout << mu.Data() << "\t"<<Q.Data()<<"\t"<<Psf.Data()<< "\t"<< Mean << "\t" << StandardDev << "\t" << Dip << endl;

	TString Outfile = Path + "OmegaResults/OmegaResults.txt";
	ifstream outfile (Outfile.Data());			// check if file exist, if not, header line will be written later on
	bool FileExists =outfile.good();
	outfile.close();
	ofstream outfile2 (Outfile.Data(),std::ofstream::app|std::ofstream::out);
	if (!FileExists)
			outfile2 << "mu" << "\t"<<"Q"<<"\t"<<"Psf"<< "\t"<< "MeanPos" << "\t" << "StandardDev" << "\t" << "Skewness"<< "\t" << "Kurtosis" << "\t" << "MaximumX" << endl;
	outfile2<< mu.Data() << "\t"<<Q.Data()<<"\t"<<Psf.Data()<< "\t"<< Mean << "\t" << StandardDev << "\t" << Skewness <<"\t" << Kurtosis<< "\t" << MaxX  << endl;
	outfile2.close();

	TString RootOutName = Path + "/OmegaResults/mu"+mu+"_Q"+Q +"__Psf"+Psf+".root"	;
	TFile *RootOutfile = new TFile(RootOutName.Data(),"RECREATE");
		hAddUp->Write();
		fGausFitCombined->Write();
		canAdd->Write();
		for(int i = 0;i<4; i++)
		{
			fGausFit[i]->Write();
			hTemporary[i]->Write();
			can[i]->Write();
		}
		RootOutfile->Close();
  return 0;
}
