int Pic_OmegaQuadrupoleComparison()
{
	const int nFiles = 3;
	TString RootInputFiles[nFiles];
		RootInputFiles[0] = "/data/work/kpha1/steinen/Gamma/Ana/CombinedData/OmegaResults/mu-2.0_Q0.0__Psf1.root";
		RootInputFiles[1] = "/data/work/kpha1/steinen/Gamma/Ana/CombinedData/OmegaResults/mu-2.0_Q-2.0__Psf1.root";
		RootInputFiles[2] = "/data/work/kpha1/steinen/Gamma/Ana/CombinedData/OmegaResults/mu-2.0_Q-4.0__Psf1.root";
	TF1 *fTemporary[nFiles];


	TString PicRootDir = gSystem->Getenv("PICTUREDIR");
	andi::setCustomStyle();
	TString HistoTitleMean = "Mean;#Omega Quadrupole moment [10^{-2}fm^{2}e]; Mean [MeV]";
	TString HistoTitleVariance = "Variance;#Omega Quadrupole moment [10^{-2}fm^{2}e]; Variance [MeV^{2}]";
	TString HistoTitleSkewness = "Skewness;#Omega Quadrupole moment [10^{-2}fm^{2}e]; Skewness [MeV^{3}]";
	TString HistoTitleKurtosis = "Kurtosis;#Omega Quadrupole moment [10^{-2}fm^{2}e]; Kurtosis [MeV^{4}]";
	TString HistoTitleMaxX = "Position of Maximum;#Omega Quadrupole moment [10^{-2}fm^{2}e]; Position of Maximum [MeV]";
	const int nPsf = 2;
	const int nPsfPlot = 1;
	const int nPoints = 22;
	TString PicName = "FEPcomparison_IronLead_Omega";
	Double_t aQ[nPsf][nPoints],
		aMean[nPsf][nPoints],
		aVariance[nPsf][nPoints],
		aSkewness[nPsf][nPoints],
		aKurtosis[nPsf][nPoints],
		aMaxX[nPsf][nPoints];
	Int_t TargetNumbers[nPsf] = {13,43};
	Int_t counter[nPsf] = {0,0};
	Color_t ColorArrray[nPsf] = {kBlack, kRed};
	Color_t ColorArrrayFunction[nFiles] = {kBlack, kRed, kGreen};
	TString TargetMaterialStrings[nPsf] = {"Psf 1","Psf 1.5"};
	Double_t Loss[nPoints];
	TGraphErrors *graphMean[nPsf];
	TGraphErrors *graphVariance[nPsf];
	TGraphErrors *graphSkewness[nPsf];
	TGraphErrors *graphKurtosis[nPsf];
	TGraphErrors *graphMaxX[nPsf];
	ifstream Inputfile("/data/work/kpha1/steinen/Gamma/Ana/CombinedData/OmegaResults/OmegaResults.txt");

	Double_t mu, Q, Psf;
	Double_t MeanPos,StandardDev, Skewness,Kurtosis,MaxX;
	char InputName[100];
	
	char buffer[100];
	Inputfile.getline(buffer,100);
	while(Inputfile  >>  mu  >> Q  >> Psf >>
			MeanPos >>  StandardDev >> Skewness>>Kurtosis >>MaxX)
	{
		if (mu == -2)
		{
			if(Psf == 1)
			{
				aQ[0][counter[0]] = Q;
				aMean[0][counter[0]] = MeanPos;
				aVariance[0][counter[0]] = StandardDev*StandardDev;
				aSkewness[0][counter[0]] = Skewness;
				aKurtosis[0][counter[0]] = Kurtosis;
				aMaxX[0][counter[0]] = MaxX;
				cout  <<  mu  << Q  << Psf << 	MeanPos <<  StandardDev << Skewness << Kurtosis << MaxX<<  endl;
				counter[0]++;
			}
			else if (Psf == 1.5)
			{
				aQ[1][counter[1]] = Q;
				aMean[1][counter[1]] = MeanPos;
				aVariance[1][counter[1]] = StandardDev;
				aSkewness[1][counter[1]] = Skewness;
				aKurtosis[1][counter[1]] = Kurtosis;
				aMaxX[1][counter[1]] = MaxX;
				counter[1]++;
				cout  << "bla" << mu  << Q  << Psf << 	MeanPos <<  StandardDev << Skewness << Kurtosis<< MaxX<< endl;
			}
		}
	}


	Inputfile.close();

	TCanvas *c1[5];
	for (int i = 0; i<5; i++)
	{
		sprintf(buffer,"c%i",i);
		c1[i] = new TCanvas(buffer,buffer,1200,800);
		c1[i]->SetGridx(1);
		c1[i]->SetGridy(1);
		c1[i]->SetTicks();
	}
	TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
	//TGraph *graph = new TGraph(10,x,y);
	for (int i = 0; i< nPsf; i++)
	{
		graphMean[i] = new TGraphErrors(counter[i],aQ[i],aMean[i],0,0);
		graphMean[i]->Sort();
		graphMean[i]->SetMarkerColor(i+1);
		graphMean[i]->SetMarkerSize(1);
		graphMean[i]->SetMarkerStyle(20);
		graphMean[i]->SetLineStyle(2);
		graphMean[i]->SetLineColor(i+1);
		

		graphVariance[i] = new TGraphErrors(counter[i],aQ[i],aVariance[i],0,0);
		graphVariance[i]->Sort();
		graphVariance[i]->SetMarkerColor(i+1);
		graphVariance[i]->SetMarkerSize(1);
		graphVariance[i]->SetMarkerStyle(20);
		graphVariance[i]->SetLineStyle(2);
		graphVariance[i]->SetLineColor(i+1);

		graphSkewness[i] = new TGraphErrors(counter[i],aQ[i],aSkewness[i],0,0);
		graphSkewness[i]->Sort();
		graphSkewness[i]->SetMarkerColor(i+1);
		graphSkewness[i]->SetMarkerSize(1);
		graphSkewness[i]->SetMarkerStyle(20);
		graphSkewness[i]->SetLineStyle(2);
		graphSkewness[i]->SetLineColor(i+1);

		graphKurtosis[i] = new TGraphErrors(counter[i],aQ[i],aKurtosis[i],0,0);
		graphKurtosis[i]->Sort();
		graphKurtosis[i]->SetMarkerColor(i+1);
		graphKurtosis[i]->SetMarkerSize(1);
		graphKurtosis[i]->SetMarkerStyle(20);
		graphKurtosis[i]->SetLineStyle(2);
		graphKurtosis[i]->SetLineColor(i+1);

		graphMaxX[i] = new TGraphErrors(counter[i],aQ[i],aMaxX[i],0,0);
		graphMaxX[i]->Sort();
		graphMaxX[i]->SetMarkerColor(i+1);
		graphMaxX[i]->SetMarkerSize(1);
		graphMaxX[i]->SetMarkerStyle(20);
		graphMaxX[i]->SetLineStyle(2);
		graphMaxX[i]->SetLineColor(i+1);

	}
	for (int i = 0; i< nPsfPlot; i++)
	{
		leg->AddEntry(graphMean[i],TargetMaterialStrings[i],"p");
	}


	graphMean[0]->SetTitle(HistoTitleMean);
	graphMean[0]->GetXaxis()->SetTitleOffset(graphMean[0]->GetXaxis()->GetTitleOffset()*1.2);
	graphMean[0]->GetYaxis()->SetTitleOffset(graphMean[0]->GetYaxis()->GetTitleOffset()*0.8);
	graphMean[0]->GetYaxis()->SetRangeUser(0.52,0.5202);

	graphVariance[0]->SetTitle(HistoTitleVariance);
	graphVariance[0]->GetXaxis()->SetTitleOffset(graphVariance[0]->GetXaxis()->GetTitleOffset()*1.2);
	graphVariance[0]->GetYaxis()->SetTitleOffset(graphVariance[0]->GetYaxis()->GetTitleOffset()*0.8);
	//graphVariance[0]->GetYaxis()->SetRangeUser(0.52,0.5202);

	graphSkewness[0]->SetTitle(HistoTitleSkewness);
	graphSkewness[0]->GetXaxis()->SetTitleOffset(graphSkewness[0]->GetXaxis()->GetTitleOffset()*1.2);
	graphSkewness[0]->GetYaxis()->SetTitleOffset(graphSkewness[0]->GetYaxis()->GetTitleOffset()*0.8);
	//graphSkewness[0]->GetYaxis()->SetRangeUser(0.52,0.5202);

	graphKurtosis[0]->SetTitle(HistoTitleKurtosis);
	graphKurtosis[0]->GetXaxis()->SetTitleOffset(graphKurtosis[0]->GetXaxis()->GetTitleOffset()*1.2);
	graphKurtosis[0]->GetYaxis()->SetTitleOffset(graphKurtosis[0]->GetYaxis()->GetTitleOffset()*0.8);
	//graphKurtosis[0]->GetYaxis()->SetRangeUser(0.52,0.5202);

	graphMaxX[0]->SetTitle(HistoTitleMaxX);
	graphMaxX[0]->GetXaxis()->SetTitleOffset(graphMaxX[0]->GetXaxis()->GetTitleOffset()*1.2);
	graphMaxX[0]->GetYaxis()->SetTitleOffset(graphMaxX[0]->GetYaxis()->GetTitleOffset()*0.8);
	//graphMaxX[0]->GetYaxis()->SetRangeUser(0.52,0.5202);

	//graphMean[0]->SetLineStyle(2);
	c1[0]->cd();
	graphMean[0]->Draw("ALP");
	for (int i = 1; i< nPsfPlot; i++)
	{
		graphMean[i]->Draw("LP");
	}
	//leg->Draw();
	c1[1]->cd();
	graphVariance[0]->Draw("ALP");
	for (int i = 1; i< nPsfPlot; i++)
	{
		graphVariance[i]->Draw("LP");
	}
	//leg->Draw();
	c1[2]->cd();
	graphSkewness[0]->Draw("ALP");
	for (int i = 1; i< nPsfPlot; i++)
	{
		graphSkewness[i]->Draw("LP");
	}
	//leg->Draw();
	c1[3]->cd();
	graphKurtosis[0]->Draw("ALP");
	for (int i = 1; i< nPsfPlot; i++)
	{
		graphKurtosis[i]->Draw("LP");
	}
	//leg->Draw();

	c1[4]->cd();
	graphMaxX[0]->Draw("ALP");
	for (int i = 1; i< nPsfPlot; i++)
	{
		graphMaxX[i]->Draw("LP");
	}
	//leg->Draw();

	//cout << PicRootDir.Data() << endl;


	TFile *RootInfile[nFiles];
	for (int i = 0;i<nFiles;i++)
	{
		RootInfile[i]= new TFile(RootInputFiles[i].Data());
		RootInfile[i]->GetObject("fGausFitCombined;1",fTemporary[i]);
		//fTemporary[i]->SetDirectory(0);
		fTemporary[i]->SetLineColor(ColorArrrayFunction[i]);
		RootInfile[i]->Close();
	}
	TCanvas *cComparison = new TCanvas("cComparison","direct Comparison",1200,800);
	cComparison->SetGrid();
	cComparison->SetTicks();
	cComparison->SetLeftMargin(0.12);
	cComparison->SetBottomMargin(0.10);
	TLegend* leg2 = new TLegend(0.7,0.7,0.9,0.9);
		leg2->AddEntry(fTemporary[0],"Q = 0.00 fm^{2}e","l");
		leg2->AddEntry(fTemporary[1],"Q = -0.02 fm^{2}e","l");
		leg2->AddEntry(fTemporary[2],"Q = -0.04 fm^{2}e","l");
	fTemporary[0]->SetTitle("");
	fTemporary[0]->GetXaxis()->SetTitle("Energy [MeV]");
	fTemporary[0]->GetXaxis()->SetTitleOffset(1.05);
	fTemporary[0]->GetYaxis()->SetTitle("Counts [a.u.]");
	fTemporary[0]->GetYaxis()->SetTitleOffset(1.3);
	fTemporary[0]->Draw();
	for (int i = 1;i<nFiles;i++)
	{
		fTemporary[i]->Draw("same");
	}
	leg2->Draw();

	andi::saveCanvas_allFileNames(c1[0],TString::Format("%s/OmegaQuadrupole/%s",PicRootDir.Data(),"Mean"));
	andi::saveCanvas_allFileNames(c1[1],TString::Format("%s/OmegaQuadrupole/%s",PicRootDir.Data(),"Variance"));
	andi::saveCanvas_allFileNames(c1[2],TString::Format("%s/OmegaQuadrupole/%s",PicRootDir.Data(),"Skewness"));
	andi::saveCanvas_allFileNames(c1[3],TString::Format("%s/OmegaQuadrupole/%s",PicRootDir.Data(),"Kurtosis"));
	andi::saveCanvas_allFileNames(c1[4],TString::Format("%s/OmegaQuadrupole/%s",PicRootDir.Data(),"MaxX"));
	andi::saveCanvas_allFileNames(cComparison,TString::Format("%s/OmegaQuadrupole/%s",PicRootDir.Data(),"ShapeComparison"));
	return 0;
}
