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
	//string Filename="/nfs/data/panda-mc/geant3/geant3/DEDX0-0.6PndTpcRawDEdx_geant3_scaledHistos.root";
	
	//i tried to use step=0.125 und P=4x aber ich weiﬂ gar nicht, ob das letztere einen Einfluss hat
	//string Filename="/nfs/data/panda-mc/AliceHP/AliceHP/DEDX0-0.6PndTpcRawDEdx_AliceHP_scaledHistos.root";
	
	//single hits
	//string Filename="/nfs/data/panda-mc/AliceHP/AliceHP/DEDX0-0.6PndTpcRawDEdx_NoMergeAlice_scaledHistos.root";
	
	//different cuts for single hits
	//string Filename="/nfs/data/panda-mc/AliceHP/AliceHP/DEDX0-0.5PndTpcRawDEdx_NoMergeAlice_scaledHistos.root";
	//string Filename="/nfs/data/panda-mc/AliceHP/AliceHP/DEDX0.1-0.7PndTpcRawDEdx_NoMergeAlice_scaledHistos.root";
	
	//jobs with the old MC with new Dx calculation, with Diffusion
	//string Filename="/nfs/data/panda-mc/oMCnDX/oMCnDX/oMCnDX/DEDX0.3-0.7PndTpcRawDEdxReco_oMCnDX_scaledHistos.root";
	
	//jobs with the old MC with new Dx calculation, without Diffusion
	//string Filename="/nfs/data/panda-mc/oMCnDX/oMCnDX/noDiff/DEDX0-0.6PndTpcRawDEdxReco_noDiff_scaledHistos.root";
	
	
	//jobs with the new MC with new Dx calculation, without Diffusion
	//string Filename="/nfs/data/panda-mc/nMCnDX/nMCnDX/nMCnDX/DEDX0-0.6PndTpcRawDEdxReco_nMCnDX_scaledHistos.root";
	
	//jobs with the new MC with new Dx with diffusion
	string Filename="/nfs/data/panda-mc/nMCnDX/nMCnDX/Diff/DEDX0-0.6PndTpcRawDEdxReco_Diff_scaledHistos.root";
	
	cout << "Loading " <<  Filename << "..." << endl;
	TFile *f = new TFile(Filename.c_str());
	assert(f);
	PDGMap histoNames;
	histoNames.FillPDGMapV3();
	//histoNames.FillScaledV3();
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