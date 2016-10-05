void Pic_FEP_DEGASComparison()
{
	andi::setCustomStyle();
	//TString HistoTitle = "Simulation of full-energy-peak-efficiency;#gamma energy [MeV]; Full-energy-peak-efficiency [%]";
	TString HistoTitle = ";#gamma energy [MeV]; Full-energy-peak-efficiency [%]";
	const int nGeometries = 3;
	const int nPoints = 38;
	TString PicName = "FEPcomparison_NewGeometries";
	TString PicName2 = "FEPRatios_NewGeometries";
	Double_t x[nGeometries][nPoints],y[nGeometries][nPoints],yE[nGeometries][nPoints];			//without target
	Int_t TargetNumbers[nGeometries] = {36,41,42};
	Int_t counter[nGeometries] = {0,0};
	Color_t ColorArrray[nGeometries] = {kBlack, kRed,kBlue};
	TString TargetMaterialStrings[nGeometries] = {"old Geometry (16)","DEGAS straight (20)","DEGAS Ball (18)"};
	TString ComparisonStrings[nGeometries] = {"straight/old","ball/old","straight/ball"};
	Double_t Loss[nPoints];
	TGraphErrors *graphTr4020[nGeometries];
	ifstream Inputfile("/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Fitted/FittedDataNewGeometry.txt");

	Int_t Geometry, Sektar, nEvents;
	Double_t Energy, FullEnergyPeakEff, FullEnergyPeakEffError, FWHM, FWHMError,FullEnergyPeakEffSEP, FullEnergyPeakEffSEPError,FullEnergyPeakEffSEPDEP, FullEnergyPeakEffSEPDEPError;
	char InputName[100];
	
	bool useNoTargetComparison = 1;
	while(Inputfile  >>  Geometry  >> Sektar >> Energy >> nEvents >>  FullEnergyPeakEff >>  FullEnergyPeakEffError >>FWHM >> FWHMError>> FullEnergyPeakEffSEP >> FullEnergyPeakEffSEPError>>FullEnergyPeakEffSEPDEP >> FullEnergyPeakEffSEPDEPError  )
	{
		for (int i = 0; i < nGeometries; i++)
		{
			if (Geometry == TargetNumbers[i])
			{
				x[i][counter[i]] = Energy;
				y[i][counter[i]] = FullEnergyPeakEff;
				yE[i] [counter[i]]= FullEnergyPeakEffError;
				cout <<counter[i] << " "<< Sektar << " " << x[i][counter[i] ] <<" " <<y[i][counter[i] ] << " " <<yE[i][counter[i] ]<<endl;
				counter[i]++;
			}
		}
	}


	Inputfile.close();
	TCanvas *c1 = new TCanvas("c1","c1",800,600);
	c1->SetGridx(1);
	c1->SetGridy(1);
	TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
	//TGraph *graph = new TGraph(10,x,y);
	for (int i = 0; i< nGeometries; i++)
	{
		graphTr4020[i] = new TGraphErrors(nPoints,x[i],y[i],0,yE[i]);
		graphTr4020[i]->SetMarkerColor(i+1);
		graphTr4020[i]->SetMarkerSize(1);
		graphTr4020[i]->SetMarkerStyle(20);
		graphTr4020[i]->SetLineStyle(2);
		graphTr4020[i]->SetLineColor(i+1);
		graphTr4020[i]->SetTitle(HistoTitle);
		graphTr4020[i]->GetXaxis()->SetTitleOffset(graphTr4020[i]->GetXaxis()->GetTitleOffset()*1.2);
		graphTr4020[i]->GetYaxis()->SetTitleOffset(graphTr4020[i]->GetYaxis()->GetTitleOffset()*0.8);
		graphTr4020[i]->GetYaxis()->SetRangeUser(0,12);

		leg->AddEntry(graphTr4020[i],TargetMaterialStrings[i],"p");
		
	}


	//graphTr4020[0]->SetLineStyle(2);
	//graphTr4020[0]->Draw("ALP");
	for (int i = 1; i< nGeometries-1; i++)
	{
			graphTr4020[i]->Sort();
			if (i ==0)
				graphTr4020[0]->Draw("ALP");
			else
				graphTr4020[i]->Draw("ALP");
	}
	//leg->Draw();

	TString PicRootDir = gSystem->Getenv("PICTUREDIR");
	cout << PicRootDir.Data() << endl;
	andi::saveCanvas_allFileNames(c1,TString::Format("%s/XiAtoms/Gamma/%s",PicRootDir.Data(),PicName.Data()));

	// now plots to compare the values of the geometries

	Double_t CompArray[nGeometries][nPoints]; // 0 = 41/36 ; 1 = 42/36 ; 2 = 41/42

	for (int j = 0; j < nPoints; j++)
	{
		CompArray[0][j]= y[1][j]/y[0][j]*100;
		CompArray[1][j]= y[2][j]/y[0][j]*100;
		CompArray[2][j]= y[1][j]/y[2][j]*100;
		cout << CompArray[1][j] << endl;
	}
	TGraph *graphComp[nGeometries];

	TCanvas *c2 = new TCanvas("c2","c2",800,600);
	c2->SetGridx(1);
	c2->SetGridy(1);
	TLegend* leg2 = new TLegend(0.7,0.7,0.9,0.9);
	for (int i = 0; i< nGeometries; i++)
	{
		graphComp[i] =new TGraph(nPoints,x[i],CompArray[i]);
		graphComp[i]->SetMarkerColor(i+2);
		graphComp[i]->SetMarkerSize(1);
		graphComp[i]->SetMarkerStyle(20);
		graphComp[i]->SetLineStyle(2);
		graphComp[i]->SetLineColor(i+1);
		if (i ==0)
			leg2->AddEntry(graphComp[i],ComparisonStrings[i],"p");
	}
	TString CompTitle = "Comparison of full-energy-peak-efficiency;#gamma energy [MeV]; Ratio of fep [%]";
	graphComp[0]->SetTitle(CompTitle);
	graphComp[0]->GetXaxis()->SetTitleOffset(graphComp[0]->GetXaxis()->GetTitleOffset()*1.2);
	graphComp[0]->GetYaxis()->SetTitleOffset(graphComp[0]->GetYaxis()->GetTitleOffset()*0.8);
	graphComp[0]->GetYaxis()->SetRangeUser(100,130);

	for (int i = 0; i< nGeometries; i++)
	{
		graphComp[i]->Sort();
		if (i ==0)
			graphComp[0]->Draw("ALP");
		//else
			//graphComp[i]->Draw("LP");
	}
	leg2->Draw();

	andi::saveCanvas_allFileNames(c2,TString::Format("%s/XiAtoms/Gamma/%s",PicRootDir.Data(),PicName2.Data()));
}
