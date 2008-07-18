void SavePIDInfo()
{
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXFitCreator.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXDistribana.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/Coordinates.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/Resolution.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/ResoHandler.C++");
	
	cout << "Generating PIDInfo..." << endl;
	TFile *f = new TFile("/nfs/data/panda-mc/E2Dir/t40/t40/DEDX0-0.6PndTpcRawDEdxReco_t40_Histos.root");	
	
	DEDXDistribana distribution;
	distribution.FillDEdxDistributions();
	distribution.CreateDeDxDistributionGausFits();
	distribution.DrawDeDxDistribution();
	
	ResoHandler TheReso;	
	TheReso.Fill(&distribution);
	
	DEDXFitCreator BB;
	BB.CreateFits();
	BB.traceDrawing();
	BB.DrawSummary();
	PndTpcDEDXFits * fits=BB.GetFits();	
	
	cout << "Saving... " << endl;
	fits->SaveFitParameters("E2t40B.txt");
	TheReso.Save("E2t40R.txt");	
	
	//f->Delete();
}