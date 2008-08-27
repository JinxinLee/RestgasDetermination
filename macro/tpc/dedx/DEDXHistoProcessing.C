void LoadClasses()	//use this global build function everywhere
{				//to avoid unnecessary trouble
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFit.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenterInitiator.C+");	
	gROOT->ProcessLine(".L macro/tpc/dedx/HistogramSet.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXHistoPainter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/CChainGetter.C+");		
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXFitCreator.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/FitServices.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGHistos.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/genHistos.C+");
}

///This macro plots histograms with dedx scatterplots, resolution, separation power
///Bethe Bloch Fits and some more stuff
///to use it the histograms have to be generated with 
///acallgenHistos.C with something else that can generate the histogram
///in the appropriate format. The format can be inspected by opening the histogram
///file in root
void DEDXHistoProcessing()
{	
	LoadClasses();

///some preprepared histograms -----------------------------------------------

	///these are ALICE MC data where hits for 0.5cm are merged
	//string Filename="/nfs/data/panda-mc/Hitm/Hitm/DEDX0-0.6PndTpcRawDEdx_Hitm_scaledHistos.root";
	
	///these are geant3 data to compare with the ALICE data above
	//string Filename="/nfs/data/panda-mc/geant3/geant3/DEDX0-0.6PndTpcRawDEdx_geant3_scaledHistos.root";
	
	///i tried to use step=0.125 und P=4x aber ich weiß gar nicht, ob das letztere einen Einfluss hat
	//string Filename="/nfs/data/panda-mc/AliceHP/AliceHP/DEDX0-0.6PndTpcRawDEdx_AliceHP_scaledHistos.root";
	
	///single hits
	//string Filename="/nfs/data/panda-mc/AliceHP/AliceHP/DEDX0-0.6PndTpcRawDEdx_NoMergeAlice_scaledHistos.root";
	
	///different cuts for single hits
	//string Filename="/nfs/data/panda-mc/AliceHP/AliceHP/DEDX0-0.5PndTpcRawDEdx_NoMergeAlice_scaledHistos.root";
	//string Filename="/nfs/data/panda-mc/AliceHP/AliceHP/DEDX0.1-0.7PndTpcRawDEdx_NoMergeAlice_scaledHistos.root";
	
	///jobs with the old MC with new Dx calculation, with Diffusion
	//string Filename="/nfs/data/panda-mc/oMCnDX/oMCnDX/oMCnDX/DEDX0.3-0.7PndTpcRawDEdxReco_oMCnDX_scaledHistos.root";
	
	//string Filename="/nfs/data/panda-mc/oMCnDX/oMCnDX/oMCnDX/DEDX0-0.6PndTpcRawDEdxReco_oMCnDX_scaledHistos.root";
	
	///jobs with the old MC with new Dx calculation, without Diffusion
	//string Filename="/nfs/data/panda-mc/oMCnDX/oMCnDX/noDiff/DEDX0-0.6PndTpcRawDEdxReco_noDiff_scaledHistos.root";
	
	///corresponding old MC
	//string Filename="/nfs/data/panda-mc/oMCnDX/oMCnDX/DEDX0-0.6PndTpcRawDEdx_oMCnDX_scaledHistos.root";
	
	///jobs with the new MC with new Dx calculation, without Diffusion
	//string Filename="/nfs/data/panda-mc/nMCnDX/nMCnDX/nMCnDX/DEDX0-0.6PndTpcRawDEdxReco_nMCnDX_scaledHistos.root";
	
	///jobs with the new MC with new Dx with diffusion
	//string Filename="/nfs/data/panda-mc/nMCnDX/nMCnDX/Diff/DEDX0-0.6PndTpcRawDEdxReco_Diff_scaledHistos.root";
    
 	///jobs with the new MC with new Dx with diffusion - settings aus der tpc.par
	//string Filename="/nfs/data/panda-mc/stdnMCnDX/stdnMCnDX/stdnMCnDX/DEDX0-0.6PndTpcRawDEdxReco_stdnMCnDX_scaledHistos.root";   

 	///jobs with the new MC with new Dx without diffusion - settings aus der tpc.par -> es sieht aber genau so
	/// aus wie das mit Diffusion
	//string Filename="/nfs/data/panda-mc/stdnMCnDX/stdnMCnDX/NoDiff/DEDX0-0.6PndTpcRawDEdxReco_NoDiff_scaledHistos.root";   
	///and the corresponding MC data
	//string Filename="/nfs/data/panda-mc/stdnMCnDX/stdnMCnDX/DEDX0-0.6PndTpcRawDEdx_stdnMCnDX_scaledHistos.root";   	

	 
	///Alice MC data merged to 0.5cm with continuous momentum reaching from 0.2 to 3 GeV/c
	/// 10*000 tracks per particle species
	//string Filename="/nfs/data/panda-mc/nMCfMom/nMCfMom/DEDX0-0.6PndTpcRawDEdx_nMCfMom_scaledHistos.root";

	///Alice MC data merged to 0.5cm with continuous momentum reaching from 0.2 to 3 GeV/c
	/// 30*000 tracks per particle species
	string Filename="/nfs/data/panda-mc/nMCfMom/nMCfMomFF/DEDX0-0.6PndTpcRawDEdx_nMCfMom_scaledHistos.root";

///--------------------------------------------------------------------------------
	cout << "Loading " <<  Filename << "..." << endl;
	TFile *f = new TFile(Filename.c_str());
	assert(f);
	PDGMap histoNames;

	///switch between scaled and unscaled histograms
	histoNames.FillPDGMapV3();
	//histoNames.FillScaledV3();
	///

	///set up the painter ------
	DEDXHistoPainter *leonardo=new DEDXHistoPainter;
	leonardo->SetPDGMap(&histoNames);
	leonardo->SetMomentaV2();
	leonardo->ClearMomenta();

	//
	//Be aware that the momenta do not have to coincide with those
	//used for fitting in the histo generation
	//
	//for the following momenta resolution, etc will be calculated
	//
	//only the corresponding momentum bins are used for the Bethe Bloch fits
	//...if the fitting needs a helpful hand ...
	//unfortunately it is not implemented that the momenta can be set independently for
	//the different particles
	//
	//leonardo->AddMomentum(0.2);
	//leonardo->AddMomentum(0.25);
	leonardo->AddMomentum(0.3);
	leonardo->AddMomentum(0.35);
	leonardo->AddMomentum(0.4);
	leonardo->AddMomentum(0.45);
	leonardo->AddMomentum(0.5);
	leonardo->AddMomentum(0.6);
	leonardo->AddMomentum(0.7);
	leonardo->AddMomentum(0.8);
	leonardo->AddMomentum(0.9);
	leonardo->AddMomentum(1.);
	leonardo->AddMomentum(1.0);
	leonardo->AddMomentum(1.2);
	leonardo->AddMomentum(1.4);
	leonardo->AddMomentum(1.6);
	leonardo->AddMomentum(1.8);
	leonardo->AddMomentum(2.0);
	leonardo->AddMomentum(2.2);
	leonardo->AddMomentum(2.4);
	leonardo->AddMomentum(2.6);
	leonardo->AddMomentum(2.8);
	leonardo->AddMomentum(2.9);

///------------------------------------------
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