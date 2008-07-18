void Process(char Spec, double AdditionalCoordinate)
{
	//Create Gaus Fits for every Particle(Momentum)
	DEDXDistribana distribution;
	distribution.FillDEdxDistributions();
	distribution.CreateSummaries();
	distribution.CreateDeDxDistributionGausFits();
	distribution.DrawDeDxDistribution();
	
	ResoHandler TheReso;	
	TheReso.Fill(&distribution);
	TheReso.Draw();
	TheReso.SetAdditionalCoordinate(Spec, AdditionalCoordinate);
	TheReso.Save("E2t40R.txt");
	
	//Create a BB-Fits for mean values
	DEDXFitCreator BB;
	BB.CreateFits();
	BB.traceDrawing();
	BB.DrawSummary();
	PndTpcDEDXFits * fits=BB.GetFits();
	fits->SetAdditionalCoordinateBB(Spec, AdditionalCoordinate);	
	fits->SaveFitParameters("E2t40B.txt");
}


void PID()
{	

	gROOT->ProcessLine(".L macro/tpc/dedx/CChainGetter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXFitCreator.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXDistribana.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/Coordinates.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/Resolution.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/ResoHandler.C+");
	
	
	CChainGetter chg;
	chg.SetFileExtension(".reco.root");
	

	///Create Histograms
	//Pions
	chg.SetFileListFile("/nfs/servers/godot/project/panda-mc/noStrag/t40/t40/t40GesamtP211.gen"); 
	chg.ReadFileNames();
	TChain* pC=chg.GetChain();

	TCanvas *p=new TCanvas("c1","c1");
	pC->Draw("PndTpcRawDEdxReco.TruncateAndMean():PndTpcRawDEdxReco.fP>>Pi+(60, 0.15, 0.45, 1300, -2000, 1700000)");
	chg.Reset();
	
	//Protons
	chg.SetFileListFile("/nfs/servers/godot/project/panda-mc/noStrag/t40/t40/t40GesamtP2212.gen"); 
	chg.ReadFileNames();
	pC=chg.GetChain();
	
	TCanvas *p=new TCanvas("c2","c2");
	pC->Draw("PndTpcRawDEdxReco.TruncateAndMean():PndTpcRawDEdxReco.fP>>p+(60, 0.15, 0.45, 1300, -2000, 1700000)");
	chg.Reset();
	
	//Kaons
	chg.SetFileListFile("/nfs/servers/godot/project/panda-mc/noStrag/t40/t40/t40GesamtP321.gen"); 
	chg.ReadFileNames();
	pC=chg.GetChain();
	
	TCanvas *p=new TCanvas("c3","c3");
	pC->Draw("PndTpcRawDEdxReco.TruncateAndMean():PndTpcRawDEdxReco.fP>>K+(60, 0.15, 0.45, 1300, -2000, 1700000)");
	chg.Reset();

		
	Process('T',40);

}

void PID2()
{

	gROOT->ProcessLine(".L macro/tpc/dedx/CChainGetter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXFitCreator.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXDistribana.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/Coordinates.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/Resolution.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/ResoHandler.C+");
	
	
	CChainGetter chg;
	chg.SetFileExtension(".reco.root");
	
	///Create Histograms
	//Pions
	chg.SetFileListFile("/nfs/servers/godot/project/panda-mc/noStrag/t60/t60/t60GesamtP211.gen"); 
	chg.ReadFileNames();
	TChain* pC=chg.GetChain();
	
	TCanvas *p=new TCanvas("c1","c1");
	pC->Draw("PndTpcRawDEdxReco.TruncateAndMean():PndTpcRawDEdxReco.fP>>Pi+(60, 0.15, 0.45, 1300, -2000, 1700000)");
	chg.Reset();
	
	//Protons
	chg.SetFileListFile("/nfs/servers/godot/project/panda-mc/noStrag/t60/t60/t60GesamtP2212.gen"); 
	chg.ReadFileNames();
	pC=chg.GetChain();
	
	TCanvas *p=new TCanvas("c2","c2");
	pC->Draw("PndTpcRawDEdxReco.TruncateAndMean():PndTpcRawDEdxReco.fP>>p+(60, 0.15, 0.45, 1300, -2000, 1700000)");
	chg.Reset();
	
	//Kaons
	chg.SetFileListFile("/nfs/servers/godot/project/panda-mc/noStrag/t60/t60/t60GesamtP321.gen"); 
	chg.ReadFileNames();
	pC=chg.GetChain();
	
	TCanvas *p=new TCanvas("c3","c3");
	pC->Draw("PndTpcRawDEdxReco.TruncateAndMean():PndTpcRawDEdxReco.fP>>K+(60, 0.15, 0.45, 1300, -2000, 1700000)");
	chg.Reset();	
	
	Process('T',60);
}
