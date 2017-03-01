int Pic_FEP_Eff_lEE()
{
	const int nPoints = 20;
Double_t x[nPoints],y[nPoints],yE[nPoints];			//without target
Double_t x3[nPoints],y3[nPoints],yE3[nPoints];		//with target 3
Double_t x2[nPoints],y2[nPoints],yE2[nPoints];		//with target 3
Double_t Loss[nPoints];
ifstream Inputfile("/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Fitted/FittedDataLowEnergy.txt");

	Int_t Geometry, Sektar, nEvents;
	Double_t Energy, FullEnergyPeakEff, FullEnergyPeakEffError, FWHM, FWHMError,FullEnergyPeakEffSEP, FullEnergyPeakEffSEPError,FullEnergyPeakEffSEPDEP, FullEnergyPeakEffSEPDEPError;
	char InputName[100];
	int counter = 0;
	int counter2 = 0;
	int counter3 = 0;
	int iSekTarFile = 7;
	int iSekTarFile2 = 6; //tantallum
	bool useNoTargetComparison = 1;
	while(Inputfile  >>  Geometry  >> Sektar >> Energy >> nEvents >>  FullEnergyPeakEff >>  FullEnergyPeakEffError >>FWHM >> FWHMError>> FullEnergyPeakEffSEP >> FullEnergyPeakEffSEPError>>FullEnergyPeakEffSEPDEP >> FullEnergyPeakEffSEPDEPError  )
	{
	if (Sektar == 4)
	{	
		x[counter] = Energy;
		y[counter] = FullEnergyPeakEff;
		yE[counter] = FullEnergyPeakEffError;
		cout <<counter << " "<< Sektar << " " << x[counter] <<" " <<y[counter] << " " <<yE[counter]<<endl;
		counter++;
	}
	if (Sektar == iSekTarFile)
	{	
		x3[counter3] = Energy;
		y3[counter3] = FullEnergyPeakEff;
		yE3[counter3] = FullEnergyPeakEffError;
		cout <<counter3 << " " <<Sektar  << " " << x3[counter3] <<" " <<y3[counter3] << " " <<yE3[counter3]<<endl;
		counter3++;
		
	}
	if (Sektar == iSekTarFile2)
		{
			x2[counter2] = Energy;
			y2[counter2] = FullEnergyPeakEff;
			yE2[counter2] = FullEnergyPeakEffError;
			cout <<counter2 << " " <<Sektar  << " " << x2[counter2] <<" " <<y2[counter2] << " " <<yE2[counter2]<<endl;
			counter2++;

		}
	}
	Inputfile.close();
	TCanvas *c1 = new TCanvas("c1","c1",800,600);
	c1->SetGridx(1);
	c1->SetGridy(1);
	TGraph *graph = new TGraph(10,x,y);
	TGraphErrors *graphTr4020 = new TGraphErrors(nPoints,x,y,0,yE);
	TGraphErrors *graphTr4020SekTar3 = new TGraphErrors(nPoints,x3,y3,0,yE3);
	TGraphErrors *graphTr4020SekTar2 = new TGraphErrors(nPoints,x2,y2,0,yE2);
	TString HistoTitle;
	switch (iSekTarFile)
	{
		case 4: HistoTitle = "Simulation of full-energy-peak-efficiency, iron target";
			break;
		//case 6: HistoTitle = "Simulation of full-energy-peak-efficiency, tantalum target";
		case 6: HistoTitle = "Simulation of full-energy-peak-efficiency";
			break;
		default: HistoTitle = "Simulation of full-energy-peak-efficiency";
	}
	graphTr4020->SetTitle(HistoTitle.Data());
	graphTr4020->GetHistogram()->SetXTitle("#gamma energy [MeV]");
	graphTr4020->GetHistogram()->GetXaxis()->CenterTitle();
	graphTr4020->GetHistogram()->GetXaxis()->SetTitleSize(0.06);
	graphTr4020->GetHistogram()->GetXaxis()->SetTitleOffset(0.75);
	graphTr4020->GetHistogram()->GetXaxis()->SetLabelSize(0.045);
	graphTr4020->GetHistogram()->SetYTitle("Full-energy-peak-efficiency [%]");
	graphTr4020->GetHistogram()->GetYaxis()->CenterTitle();
	graphTr4020->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
	graphTr4020->GetHistogram()->GetYaxis()->SetTitleOffset(0.6);
	graphTr4020->GetHistogram()->GetYaxis()->SetLabelSize(0.045);   
	graphTr4020->GetHistogram()->SetMinimum(0);
	//graphTr4020->GetHistogram()->SetMaximum(12);
	Size_t dMarkerSize = 2;
	graphTr4020->SetMarkerColor(1);
	graphTr4020->SetMarkerSize(dMarkerSize/2);
	graphTr4020->SetMarkerStyle(20);
	graphTr4020->SetLineColor(1);
	graphTr4020->SetLineStyle(2);
	
	graphTr4020SekTar3->SetTitle(HistoTitle.Data());
	graphTr4020SekTar3->GetHistogram()->SetXTitle("#gamma energy [MeV]");
	graphTr4020SekTar3->GetHistogram()->GetXaxis()->CenterTitle();
	graphTr4020SekTar3->GetHistogram()->GetXaxis()->SetTitleSize(0.06);
	graphTr4020SekTar3->GetHistogram()->GetXaxis()->SetTitleOffset(0.75);
	graphTr4020SekTar3->GetHistogram()->GetXaxis()->SetLabelSize(0.045);
	graphTr4020SekTar3->GetHistogram()->SetYTitle("Full-energy-peak-efficiency [%]");
	graphTr4020SekTar3->GetHistogram()->GetYaxis()->CenterTitle();
	graphTr4020SekTar3->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
	graphTr4020SekTar3->GetHistogram()->GetYaxis()->SetTitleOffset(0.6);
	graphTr4020SekTar3->GetHistogram()->GetYaxis()->SetLabelSize(0.045);
	graphTr4020SekTar3->GetHistogram()->SetMinimum(0);


	graphTr4020SekTar3->SetMarkerColor(2);
	graphTr4020SekTar3->SetMarkerSize(dMarkerSize/2);
	graphTr4020SekTar3->SetMarkerStyle(20);
	graphTr4020SekTar3->SetLineColor(2);
	graphTr4020SekTar3->SetLineStyle(2);

	graphTr4020SekTar2->SetTitle(HistoTitle.Data());
		graphTr4020SekTar2->GetHistogram()->SetXTitle("#gamma energy [MeV]");
		graphTr4020SekTar2->GetHistogram()->GetXaxis()->CenterTitle();
		graphTr4020SekTar2->GetHistogram()->GetXaxis()->SetTitleSize(0.06);
		graphTr4020SekTar2->GetHistogram()->GetXaxis()->SetTitleOffset(0.75);
		graphTr4020SekTar2->GetHistogram()->GetXaxis()->SetLabelSize(0.045);
		graphTr4020SekTar2->GetHistogram()->SetYTitle("Full-energy-peak-efficiency [%]");
		graphTr4020SekTar2->GetHistogram()->GetYaxis()->CenterTitle();
		graphTr4020SekTar2->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
		graphTr4020SekTar2->GetHistogram()->GetYaxis()->SetTitleOffset(0.6);
		graphTr4020SekTar2->GetHistogram()->GetYaxis()->SetLabelSize(0.045);
		graphTr4020SekTar2->GetHistogram()->SetMinimum(0);


		graphTr4020SekTar2->SetMarkerColor(3);
		graphTr4020SekTar2->SetMarkerSize(dMarkerSize/2);
		graphTr4020SekTar2->SetMarkerStyle(20);
		graphTr4020SekTar2->SetLineColor(3);
		graphTr4020SekTar2->SetLineStyle(2);

	if (useNoTargetComparison)
	{
		TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
		leg->AddEntry(graphTr4020,"iron target","p");
		//leg->AddEntry(graphTr4020SekTar3,"tantalum target","p");
		leg->AddEntry(graphTr4020SekTar3,"iron target, with silicons","p");
		leg->AddEntry(graphTr4020SekTar2,"tantalum target","p");
		graphTr4020->Draw("APL");
		graphTr4020SekTar3->Draw("PL");
		graphTr4020SekTar2->Draw("PL");
		leg->Draw("");
	}
	else
		graphTr4020SekTar3->Draw("APL");
	//graph->Draw("APL");
	
	if (useNoTargetComparison)
	{
		for (int i = 0; i<counter;i++)
		{
			Loss[i]=100-y3[i]/y[i]*100;
			cout << i << " " << Loss[i] << endl;
		}

		TCanvas *c2 = new TCanvas("c2","c2",800,600);
		c2->SetGridx(1);
		c2->SetGridy(1);
		TGraph *graphLoss = new TGraph(10,x,Loss);
		graphLoss->SetMarkerColor(1);
		graphLoss->SetMarkerSize(dMarkerSize/2);
		graphLoss->SetMarkerStyle(20);
		graphLoss->SetLineColor(1);
		graphLoss->SetLineStyle(2);
		graphLoss->SetTitle("Efficiency loss due to the silicon detectors");
		graphLoss->GetHistogram()->SetXTitle("#gamma energy [keV]");
		graphLoss->GetHistogram()->GetXaxis()->CenterTitle();
		graphLoss->GetHistogram()->GetXaxis()->SetTitleSize(0.06);
		graphLoss->GetHistogram()->GetXaxis()->SetTitleOffset(0.75);
		graphLoss->GetHistogram()->GetXaxis()->SetLabelSize(0.045);
		graphLoss->GetHistogram()->SetYTitle("Efficiency loss [%]");
		graphLoss->GetHistogram()->GetYaxis()->CenterTitle();
		graphLoss->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
		graphLoss->GetHistogram()->GetYaxis()->SetTitleOffset(0.6);
		graphLoss->GetHistogram()->GetYaxis()->SetLabelSize(0.045);
		graphLoss->GetHistogram()->SetMinimum(0);
		graphLoss->Draw("APL");
	}
	TString PicFEPString ="~/pictures/FEP-Eff_Geo";
			PicFEPString += 	iSekTarFile;
			PicFEPString += "_LE.png";
	c1->SaveAs(PicFEPString.Data());
	if (useNoTargetComparison)
	{
		TString PicFEPCompString ="~/pictures/Target-NoTarget-Comparion_Geo";
			PicFEPCompString += 	iSekTarFile;
			PicFEPCompString += "_LE.png";

		c2->SaveAs(PicFEPCompString.Data());
	}
  return 0;
}
