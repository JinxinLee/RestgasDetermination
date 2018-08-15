// created on 23.09.13 by Marcell Steinen steinen@kph.uni-mainz.de
// root macro to create an energy spectrum of parts of simulations with a high number of events (memory leak somewhere :/); to be used with "script_Gamma_Ana_List_split.sh" on himster
// further analysis is done  via bash script "script_CombineAndAnalyzeGamma.sh" which combines the split up root output via "hadd" and calls root macro "AnalyseCombinedGammaFiles.C" to analyse the spectrum



Double_t Compton(Double_t Energy, Double_t Th)
{
return Energy-Energy/(1+Energy/0.000511*(1-TMath::Cos(Th)));
}
Double_t invCompton(Double_t Energy, Double_t Eprime)
{
return TMath::ACos((Energy*Energy-Energy*Eprime-Eprime*0.000511)/(Energy*(Energy-Eprime)));
}

Double_t PeakFunc (Double_t *x, Double_t *par)
{
	return par[0]+x[0] * par[1]+x[0] * x[0] * par[2]+ par[3] * 1/(TMath::Sqrt(2 * TMath::Pi()) * par[5]) * TMath::Exp(- pow((x[0] -par[4]),2)/2/par[5]/par[5]);
}

Double_t PoissonFunc(Double_t *x, Double_t *par)
{
	return par[1]*TMath::Poisson(x[0],par[0]);
}
int GammaSpectraAnalysis_NoH_Split(TString Filename = "TripleBall40Offset10STT_8MeV_1000EvtswithSecTar.root",Int_t NoOfJobs = 10, Int_t JobNumber = 2)//, Double_t Energy)
{


  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
   //gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


   gSystem->Load("libHypGe");
	gSystem->Load("set");						// needed to use a std::set
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

	//Get energy from filename

	Int_t IndexPreEnergy = Filename.Index("_",1,0,0);
	Int_t IndexPostEnergy = Filename.Index("MeV",3,0,0);
	TString EnergyFromFileName = Filename(IndexPreEnergy+1,IndexPostEnergy-IndexPreEnergy-1);
	//cout << EnergyFromFileName << endl;
	Double_t Energy = EnergyFromFileName.Atof()/1000;

	// the sim file you want to analyse
	//string Filename = "TripleBall40Offset10_1MeV_10000Evts"; //without File Type ending!!!
	//Double_t Energy = 0.001;
	if(Filename.EndsWith(".root",1))
	{
		Filename.ReplaceAll(".root","");
		cout << "Filename ending chopped!" << endl;
	}


	TString CompleteFilename = "$SIMDATADIR/Gamma/"+Filename+".root";
	TFile* g = new TFile(CompleteFilename);

//Output Files
	TString SIMDATADIR = getenv("SIMDATADIR");
	TString OutPath = SIMDATADIR+"/Gamma/Ana/" + Filename;
	TString MkDir = ".!mkdir -p " + OutPath;
	gROOT->ProcessLine(MkDir);
	cout << MkDir << endl;
	TString outfile= OutPath + "/Ana";
	outfile += Filename;
	outfile += "_";
	outfile += JobNumber;
	TString txtfileName =  outfile;
	outfile +=".root";
	txtfileName += ".txt";
	TFile* fi = new TFile(outfile,"RECREATE");


   //photons from hyp electromag. decay
  TTree *b=(TTree *) g->Get("pndsim") ;
  TClonesArray* hit_bar=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&hit_bar);//Branch names
  TClonesArray* mc_bar=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&mc_bar);//Branch names




  //****photons from hyp elect. decay
	Double_t HistoUpperThreshold = Energy*1.2;
	string Name = "total gam energy deposit, " + Filename;
	Int_t ChannelResolution = 10000000;		// GeV/ChannelResolution = energy per channel , with 10000000 100 eV/channel
	Double_t Resolution = 2.;	//keV
	TH1D* gamTde = new TH1D("gamTde",Name.c_str(),ChannelResolution*HistoUpperThreshold,0.0000,HistoUpperThreshold);
	TH1D* ParameterBuffer = new TH1D("ParameterBuffer", "ParameterBuffer",4,0,3);
	ParameterBuffer->SetBinContent(1,Energy);
	ParameterBuffer->SetBinContent(2,b->GetEntriesFast());
	ParameterBuffer->SetBinContent(3,ChannelResolution);
	ParameterBuffer->SetBinContent(4,Resolution);


  ParameterBuffer->Write();

	bool verbose = false;
	Int_t MotherId,Motherpdg;

	TH1D *hNoHits = new TH1D("Number of Hits", "Number of Hits", 10,0,10);

	TVector3 vecs,pos;
	int mcpdg = -1,ev;
	Double_t mult,En,Eng,Enth;

	int count;
	std::set<int> SetOfCrystalHit;					//seems to be some error with set's -> taken out for now
	std::set<int>::iterator it;

	Int_t nEvents = b->GetEntriesFast();
	cout<< "Number of Simulated Events: "<<nEvents<<endl;



	PndHypGePoint *hitgam;// = new PndHypGePoint();
	PndMCTrack *mcgam;
	Int_t StartEvent = nEvents/NoOfJobs*(JobNumber-1);
	Int_t EndEvent = nEvents/NoOfJobs*(JobNumber);
	cout << "Analysing job number " << JobNumber <<"; from Event " << StartEvent << " to " << EndEvent-1 << endl;

	//for (Int_t k=0; k<nEvents; k++)
	for (Int_t k=StartEvent; k<EndEvent; k++)
	{
			//cout << k << endl;
		Eng=0.;
		b->GetEntry(k);
		//cout << k<< endl;
		if (!((k*10)% (EndEvent-StartEvent)))
		{
			cout << k << endl;
		}
	    //if(verbose) cout<<"Event No "<<j<<endl;
		for (Int_t i=0; i<hit_bar->GetEntriesFast(); i++)
		{
			//cout << hit_bar->GetEntriesFast()<<endl;
			hitgam = new PndHypGePoint();
			hitgam=(PndHypGePoint*)hit_bar->At(i);
					//cout << hitgam << endl;
			//mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());
			Eng += gRandom->Gaus(0,0.000001*Resolution/2.3548);
			Eng += hitgam->GetEnergyLoss();
			SetOfCrystalHit.insert(hitgam->GetDetectorID());
			//cout <<"DetID" << hitgam->GetDetectorID()<< endl;
			//delete hitgam;
			//delete mcgam;

		}//end for i (points in event)
	    //count =0;
			//cout <<Eng<<endl;
			//cout << TMath::Abs(Eng-Energy) << "\t" << 3*0.000001*Resolution/2.3548 << endl;
		if (TMath::Abs(Eng-Energy) < 10*0.000001*Resolution/2.3548) 			//Peak = 10 * Resolution of 1 crystal --> takes multiple hits into account (faster than fitting and than doing it again, error is small)
		{
			std::cout << "SetOfCrystalHit contains Crystal No";
			for (it=SetOfCrystalHit.begin(); it!=SetOfCrystalHit.end(); ++it)
			{
				std::cout << ' ' << *it;
			}
			std::cout << " and is " << SetOfCrystalHit.size() << " long\n";
			hNoHits->Fill(SetOfCrystalHit.size());
			//hNoHits->Fill(hit_bar->GetEntriesFast());		//Fill # of Hits -diagramm with events inside 3 sigma of the peak
			//cout << hit_bar->GetEntriesFast()<<endl;
		}
		if(Eng>0)
		{
			gamTde->Fill(Eng);		//Fill spectrum
		}
	  //cout << "Size" << SetOfCrystalHit.size()<< endl;
		SetOfCrystalHit.clear();
		//cout << "Size" << SetOfCrystalHit.size()<< endl;
	}// end for j (events)





	//some make-up for the histograms
	gamTde->SetXTitle("Energy [GeV]");
	gamTde->SetYTitle("Counts");
	gamTde->GetYaxis()->SetTitleOffset(1.35);
	hNoHits->SetXTitle("Number of Hits");
	hNoHits->SetYTitle("Counts");
	hNoHits->GetYaxis()->SetTitleOffset(1.1);


	gamTde->Write();
	hNoHits->Write();

	fi->Close();

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << outfile << endl;
	cout << "Parameter file is " << txtfileName << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

  return 0;
}
