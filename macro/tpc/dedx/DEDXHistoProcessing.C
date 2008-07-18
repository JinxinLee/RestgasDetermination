void LoadClasses()
{
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFit.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenter.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenterInitiator.C++");	
	gROOT->ProcessLine(".L macro/tpc/dedx/HistogramSet.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXHistoPainter.C++");
}

void DEDXHistoProcessing()
{	
	LoadClasses();

	//these are ALICE MC data where hits for 0.5cm are merged
	//string Filename="/nfs/data/panda-mc/Hitm/Hitm/DEDX0-0.6PndTpcRawDEdx_Hitm_scaledHistos.root";
	//these are geant3 data to compare with the ALICE data above
	string Filename="/nfs/data/panda-mc/geant3/geant3/DEDX0-0.6PndTpcRawDEdx_geant3_scaledHistos.root";
	
	cout << "Loading " <<  Filename << "..." << endl;
	TFile *f = new TFile(Filename.c_str());
	assert(f);
	PDGMap histoNames;
	//histoNames.FillPDGMapV3();
	histoNames.FillScaledV3();
	DEDXHistoPainter *leonardo=new DEDXHistoPainter;
	leonardo->SetPDGMap(&histoNames);
	leonardo->SetMomentaV2();
	leonardo->LoadHistograms();
	TCanvas *pC1=new TCanvas("Seppo","Seppo");
	leonardo->DrawSeparationPower();	
	TCanvas *pC2=new TCanvas("Reso", "Reso");
	leonardo->DrawResolution();
	leonardo->DrawBBFitting();
	leonardo->DrawGausFitting();
	TCanvas *pC3=new TCanvas("dedx_summary", "dedx_summary");
	leonardo->DrawBBSummary();
}