void testFitServices()
{
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFit.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenter.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/FitServices.C++");
	
	TH2F *histo=(TH2F*)gDirectory->Get("P211scaled");
	FitServices *test=new FitServices;
	test->SetParticleHistogram(histo);
	cout << test->GetBBMinimum() << endl;
	test->DrawBBFit();
}


void testGausFitCenterInitiation()
{
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFit.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenterInitiator.C+");
	
	GausFitCenterInitiator *test=new GausFitCenterInitiator;
	TH2F *histo=(TH2F*)gDirectory->Get("P211scaled");
	test->AddParticleHistogram(histo);
	test->InitGausFitCenter();
	GausFitCenter *gf=test->GetGausFitCenter();
	gf->CreateBBGraphs();
	TGraphErrors *graph=gf->GetBBGraph("P211scaled");
	if(graph)	{
		graph->Draw("ALP");
	}
	else	{
		cout << "bb graph is NULL." << endl;
	}
}

void testResolutionGraph()
{
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFit.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenterInitiator.C+");	

	GausFitCenterInitiator *test=new GausFitCenterInitiator;
	TH2F *histo=(TH2F*)gDirectory->Get("P211");
	TH2F *histo2=(TH2F*)gDirectory->Get("P2212");
	test->AddParticleHistogram(histo);
	test->AddParticleHistogram(histo2);
	test->InitGausFitCenter();
	GausFitCenter *gf=test->GetGausFitCenter();
	gf->CreateResolutionGraphs();
	TGraphErrors *graph=gf->GetResolutionGraph("P2212");
	if(graph)	{
		graph->Draw("ALP");
	}
	else	{
		cout << "resolution graph is NULL." << endl;
	}
}

void testSeparationPowerGraph()
{
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFit.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenterInitiator.C+");	

	GausFitCenterInitiator *test=new GausFitCenterInitiator;
	TH2F *histo=(TH2F*)gDirectory->Get("P211");
	TH2F *histo2=(TH2F*)gDirectory->Get("P2212");
	TH2F *histo3=(TH2F*)gDirectory->Get("P321");
	TH2F *histo4=(TH2F*)gDirectory->Get("11");
	
	test->AddParticleHistogram(histo);
	test->AddParticleHistogram(histo2);
	test->AddParticleHistogram(histo3);
	test->AddParticleHistogram(histo4);
	test->InitGausFitCenter();
	GausFitCenter *gf=test->GetGausFitCenter();
	gf->CreateSeparationPowerGraphs();
	
	TGraph *graph=gf->GetSeparationPowerGraph("P2212", "P211");
	TGraph *graph2=gf->GetSeparationPowerGraph("P2212", "P321");
	TGraph *graph3=gf->GetSeparationPowerGraph("P2212", "P11");
	TGraph *graph4=gf->GetSeparationPowerGraph("P321", "P211");
	
	TMultiGraph *mg=new TMultiGraph("Seppos", "Seppos");
	TLegend *legend=new TLegend(0.39,0.6,0.9,0.8);
	legend->SetTextFont(72);
	legend->SetTextSize(0.04);
	
	if(graph)	{
		mg->Add(graph);
		legend->AddEntry( graph, "P+ - Pi+");
	}
	if(graph2)	{
		mg->Add(graph2);
		legend->AddEntry( graph2, "P+ - K+");
	}
	if(graph3)	{
		mg->Add(graph3);
		legend->AddEntry( graph3, "P+ - e-");
	}
	if(graph4)	{
		mg->Add(graph4);
		legend->AddEntry( graph4, "K+ - Pi+");
	}
		
	
	mg->Draw("APC");
	mg->GetHistogram()->GetXaxis()->SetTitle("|P|");
	legend->Draw();
}

void testHistoPainter()
{
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFit.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenterInitiator.C+");	
	gROOT->ProcessLine(".L macro/tpc/dedx/HistogramSet.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXHistoPainter.C+");	

	PDGMap histoNames;
	histoNames.FillPDGMapV3();
	DEDXHistoPainter *test=new DEDXHistoPainter;
	test->SetPDGMap(&histoNames);
	test->LoadHistograms();
	TCanvas *pC1=new TCanvas("Seppo","Seppo");
	test->DrawSeparationPower();	
	TCanvas *pC2=new TCanvas("Reso", "Reso");
	test->DrawResolution();
	test->DrawBBFitting();
	test->DrawGausFitting();
	TCanvas *pC3=new TCanvas("dedx_summary", "dedx_summary");
	test->DrawBBSummary();
}