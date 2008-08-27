{
	gROOT->ProcessLine(".L macro/tpc/dedx/CChainGetter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C+");	
	gROOT->ProcessLine(".L macro/tpc/dedx/HistoGenerator.C+");	
	
	HistoGenerator genHistos;
	
	genHistos.SetRange("(600,0,30)");
	genHistos.SetFileExtension(".raw.root");
	genHistos.SetDrawCommand("fDE/fDx*1000*1000");
	genHistos.SetHistoNameAddOn("geant3", kRed);
	
	//this will produce plots for particles 211,2212,321,11
	genHistos.AddChainFile(211,"/nfs/data/panda-mc/newMC/geant3/geant3GesamtP211.gen");	
	genHistos.AddChainFile(2212,"/nfs/data/panda-mc/newMC/geant3/geant3GesamtP2212.gen");
	//genHistos.AddChainFile(321,"/nfs/data/panda-mc/newMC/geant3/geant3GesamtP321.gen");
	//genHistos.AddChainFile(11,"/nfs/data/panda-mc/newMC/geant3/geant3GesamtP11.gen");		
	//genHistos.AddChainFile(211,"/nfs/data/panda-mc/newMC/nmc1/nmc1GesamtP211.gen");	
	//genHistos.AddChainFile(2212,"/nfs/data/panda-mc/newMC/nmc1/nmc1GesamtP2212.gen");
	//genHistos.AddChainFile(321,"/nfs/data/panda-mc/newMC/nmc1/nmc1GesamtP321.gen");
	//genHistos.AddChainFile(11,"/nfs/data/panda-mc/newMC/nmc1/nmc1GesamtP11.gen");
	
	//genHistos.AddChainFile(211,"/nfs/data/panda-mc/Hitm/Hitm/HitmGesamtP211.gen");	
	//genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/Hitm4cmGesamtP2212.gen");
	//genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/Hitm10GesamtP2212.gen");
	//and momenta 0.2, 0.25
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.19 && PndTpcRawDEdx.fP<0.21", "P_0_2");
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.24 && PndTpcRawDEdx.fP<0.26", "P_0_25");	
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.29 && PndTpcRawDEdx.fP<0.31", "P_0_3");	
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.34 && PndTpcRawDEdx.fP<0.36", "P_0_35");	
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.39 && PndTpcRawDEdx.fP<0.41", "P_0_4");	
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.49 && PndTpcRawDEdx.fP<0.51", "P_0_5");	
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.59 && PndTpcRawDEdx.fP<0.61", "P_0_6");
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.69 && PndTpcRawDEdx.fP<0.71", "P_0_7");
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.79 && PndTpcRawDEdx.fP<0.81", "P_0_8");
	genHistos.AddSelection("PndTpcRawDEdx.fP>0.89 && PndTpcRawDEdx.fP<0.91", "P_0_9");

	genHistos.Generate();
	//genHistos.Draw();
	
	genHistos.ReleaseChainFiles();	//but use the same selections
	genHistos.SetHistoNameAddOn("ali", kBlue);
	genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/HitmGesamtP2212.gen");
	genHistos.AddChainFile(211,"/nfs/data/panda-mc/Hitm/Hitm/Hitm0_5cmNRGesamtP211.gen");
//	genHistos.AddChainFile(321,"/nfs/data/panda-mc/Hitm/Hitm/HitmGesamtP321.gen");
//	genHistos.AddChainFile(11,"/nfs/data/panda-mc/Hitm/Hitm/HitmGesamtP11.gen");
	//genHistos.AddChainFile(211,"/nfs/data/panda-mc/newMC/nmc1/nmc2GesamtP211.gen");	
	//genHistos.AddChainFile(2212,"/nfs/data/panda-mc/newMC/nmc1/nmc2GesamtP2212.gen");
	//genHistos.AddChainFile(321,"/nfs/data/panda-mc/newMC/nmc1/nmc2GesamtP321.gen");
	//genHistos.AddChainFile(11,"/nfs/data/panda-mc/newMC/nmc1/nmc2GesamtP11.gen");	
	genHistos.Generate();
	
	/*
	genHistos.ReleaseChainFiles();	
	genHistos.SetHistoNameAddOn("n30", kGreen);
	genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/Hitm30GesamtP2212.gen");	
	genHistos.Generate();
	
	genHistos.ReleaseChainFiles();	
	genHistos.SetHistoNameAddOn("n40", kCyan);
	genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/Hitm40GesamtP2212.gen");	
	genHistos.Generate();	
	
	genHistos.ReleaseChainFiles();	
	genHistos.SetHistoNameAddOn("n50", kYellow);
	genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/Hitm50GesamtP2212.gen");	
	genHistos.Generate();		
	
	genHistos.ReleaseChainFiles();	
	genHistos.SetHistoNameAddOn("n60", kMagenta);
	genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/Hitm60GesamtP2212.gen");	
	genHistos.Generate();		
	
	genHistos.ReleaseChainFiles();	
	genHistos.SetHistoNameAddOn("n100", 40);
	genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/Hitm100GesamtP2212.gen");	
	genHistos.Generate();		
	
	genHistos.ReleaseChainFiles();	
	genHistos.SetHistoNameAddOn("n200", 41);
	genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/Hit200GesamtP2212.gen");	
	genHistos.Generate();		
	
	genHistos.ReleaseChainFiles();	
	genHistos.SetHistoNameAddOn("n300", 42);
	genHistos.AddChainFile(2212,"/nfs/data/panda-mc/Hitm/Hitm/Hitm300GesamtP2212.gen");	
	genHistos.Generate();		
	*/
	
	genHistos.Draw();
	//...add new Chainfiles here
	
	//genHistos.DrawCurrentSetInOldCanvases();
	
}