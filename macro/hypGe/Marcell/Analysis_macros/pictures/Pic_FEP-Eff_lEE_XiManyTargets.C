void Pic_FEP_Eff_lEE_XiManyTargets()
{
	andi::setCustomStyle();
	TString HistoTitle = "Simulation of full-energy-peak-efficiency;#gamma energy [MeV]; Full-energy-peak-efficiency [%]";
	const int nTargets = 2;
	const int nPoints = 22;
	TString PicName = "FEPcomparison_IronLead_Omega";
	Double_t x[nTargets][nPoints],y[nTargets][nPoints],yE[nTargets][nPoints];			//without target
	Int_t TargetNumbers[nTargets] = {13,43};
	Int_t counter[nTargets] = {0,0};
	Color_t ColorArrray[nTargets] = {kBlack, kRed};
	TString TargetMaterialStrings[nTargets] = {"iron target","lead target"};
	Double_t Loss[nPoints];
	TGraphErrors *graphTr4020[nTargets];
	ifstream Inputfile("/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Fitted/FittedDataLowEnergy.txt");

	Int_t Geometry, Sektar, nEvents;
	Double_t Energy, FullEnergyPeakEff, FullEnergyPeakEffError, FWHM, FWHMError,FullEnergyPeakEffSEP, FullEnergyPeakEffSEPError,FullEnergyPeakEffSEPDEP, FullEnergyPeakEffSEPDEPError;
	char InputName[100];
	
	bool useNoTargetComparison = 1;
	while(Inputfile  >>  Geometry  >> Sektar >> Energy >> nEvents >>  FullEnergyPeakEff >>  FullEnergyPeakEffError >>FWHM >> FWHMError>> FullEnergyPeakEffSEP >> FullEnergyPeakEffSEPError>>FullEnergyPeakEffSEPDEP >> FullEnergyPeakEffSEPDEPError  )
	{
		for (int i = 0; i < nTargets; i++)
		{
			if (Sektar == TargetNumbers[i])
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
	for (int i = 0; i< nTargets; i++)
	{
		graphTr4020[i] = new TGraphErrors(nPoints,x[i],y[i],0,yE[i]);
		graphTr4020[i]->SetMarkerColor(i+1);
		graphTr4020[i]->SetMarkerSize(1);
		graphTr4020[i]->SetMarkerStyle(20);
		graphTr4020[i]->SetLineStyle(2);
		graphTr4020[i]->SetLineColor(i+1);
		leg->AddEntry(graphTr4020[i],TargetMaterialStrings[i],"p");
		
	}
	graphTr4020[0]->SetTitle(HistoTitle);
	graphTr4020[0]->GetXaxis()->SetTitleOffset(graphTr4020[0]->GetXaxis()->GetTitleOffset()*1.2);
	graphTr4020[0]->GetYaxis()->SetTitleOffset(graphTr4020[0]->GetYaxis()->GetTitleOffset()*0.8);
	graphTr4020[0]->GetYaxis()->SetRangeUser(0,12);
	//graphTr4020[0]->SetLineStyle(2);
	graphTr4020[0]->Draw("ALP");
	for (int i = 1; i< nTargets; i++)
	{
			graphTr4020[i]->Draw("LP");
	}
	leg->Draw();

	TString PicRootDir = gSystem->Getenv("PICTUREDIR");
	cout << PicRootDir.Data() << endl;
	andi::saveCanvas_allFileNames(c1,TString::Format("%s/XiAtoms/Gamma/%s",PicRootDir.Data(),PicName.Data()));

}
