// root macro to analyze the simulation output



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
int GammaSpectraAnalysis_NoH(TString Filename = "Sim_Geo36_E0.500MeV_Evts10000000_FileEvts20000_Gen1_ST0__1.root", TString SubFolder ="Sim_Geo36_E0.500MeV_Evts10000000_FileEvts20000_Gen1_ST0")//, Double_t Energy)
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

	Int_t IndexPreEnergy = Filename.Index("E",1,4,0);
	Int_t IndexPostEnergy = Filename.Index("MeV",3,4,0);
	TString EnergyFromFileName = Filename(IndexPreEnergy+1,IndexPostEnergy-IndexPreEnergy-1);
	//cout << EnergyFromFileName << endl;
	Double_t Energy = EnergyFromFileName.Atof();				// now in MeV, correct???? (7.11.14)

	// the sim file you want to analyse
	//string Filename = "TripleBall40Offset10_1MeV_10000Evts"; //without File Type ending!!!
	//Double_t Energy = 0.001;


	if(Filename.EndsWith(".root",1))
	{
		Filename.ReplaceAll(".root","");
		cout << "Filename ending chopped!" << endl;
	}
	TString CompleteFilename = "$SIMDATADIR/Gamma/"+SubFolder+"/"+Filename+".root";
	TFile* g = new TFile(CompleteFilename);

//Output Files
	Filename.ReplaceAll("Sim","Ana");
	SubFolder.ReplaceAll("Sim","Ana");
	TString Path = getenv("SIMDATADIR");
	TString outfile= Path+"/Gamma/Ana/"+SubFolder;
	char CommandBuffer[400];
	sprintf(CommandBuffer,".!mkdir -p %s",outfile.Data());
		cout << "Processing " << CommandBuffer<< endl;
		gROOT->ProcessLine(CommandBuffer);		// create subfolder for the output
	outfile += "/";
	outfile += Filename;
	TString txtfileName =  outfile;
	outfile +=".root";
	txtfileName += ".txt";
	TFile* fi = new TFile(outfile,"RECREATE");

	//cout << "Path" << Path << endl;
	//TString txtfileName = Path +"/Data_Marcell/"+Filename+"_Spectrum.txt";
	//cout << txtfileName << endl;
	ofstream txtfile;
	txtfile.open(txtfileName);
	txtfile << "File read:" << CompleteFilename << endl;


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
	TH1D* gamTde = new TH1D("gamTde",Name.c_str(),ChannelResolution*HistoUpperThreshold,0.0000,HistoUpperThreshold);


	bool verbose = false;
	Int_t MotherId,Motherpdg;

	TH1D *hNoHits = new TH1D("Number of Hits", "Number of Hits", 10,0,10);

	TVector3 vecs,pos;
	int mcpdg = -1,ev;
	Double_t mult,En,Eng,Enth;

	//vector<int> event;
	int count;
	std::set<int> SetOfCrystalHit;					//seems to be some error with set's -> taken out for now
	std::set<int>::iterator it;

	Int_t nEvents = b->GetEntriesFast();
	cout<< "Number of Simulated Events: "<<nEvents<<endl;
	txtfile<< "Number of Simulated Events: "<<nEvents<<endl;

	Double_t Resolution = 2.;	//keV
	PndHypGePoint *hitgam;// = new PndHypGePoint();
	PndMCTrack *mcgam;
	for (Int_t k=0; k<nEvents; k++)
	{
			//cout << k << endl;
		Eng=0.;
		b->GetEntry(k);

		if (!((k*10)% nEvents))
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
			//std::cout << "SetOfCrystalHit contains Crystal No";
			//for (it=SetOfCrystalHit.begin(); it!=SetOfCrystalHit.end(); ++it)
			//{
			//	std::cout << ' ' << *it;
			//}
			//std::cout << " and is " << SetOfCrystalHit.size() << " long\n";
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



	TCanvas* can3 = new TCanvas("can3","germanium detector",0,0,1000,1000);

  //gamTde->Draw();

  //Analysis of spectrum

	Double_t lowAngle = invCompton(0.001332,0.001040);
	Double_t highAngle = invCompton(0.001332,0.001096);
	Int_t npeaks = 1;
	Int_t PeakToLook = 1;

	TSpectrum *s = new TSpectrum(npeaks);
	s->Search(gamTde,npeaks,"new",0.01);

	Float_t *xpeaks = s->GetPositionX();
	Float_t *ypeaks = s->GetPositionY();

		//Print Peaks
	for (Int_t i = 0; i < npeaks; ++i)
	{
		cout <<"("<< xpeaks[i]<<"," << ypeaks[i] << ") ";
	}
	cout << endl;

	Double_t PeakX = xpeaks[PeakToLook-1]*ChannelResolution;
	Double_t PeakY = ypeaks[PeakToLook-1];
	Double_t DPeakY = sqrt(PeakY);
	Double_t SumPeak = 0;

	for(Int_t i = PeakX-Resolution*10*10/2.3548; i <= PeakX+Resolution*10*10/2.3548; i++)		//Peak = 10 * Resolution of 1 crystal --> takes multiple hits into account (faster than fitting and than doing it again, error is small)
		SumPeak += gamTde->GetBinContent(i);
	Double_t DSumPeak = sqrt(SumPeak);
	cout <<"MaxX: "<< PeakX << "; MaxY: " << PeakY << " +- " << DPeakY<< endl;
	cout << "SumPeak: " << SumPeak << " +- " << DSumPeak << endl;
	txtfile <<"MaxX: "<< PeakX << "; MaxY: " << PeakY << " +- " << DPeakY<< endl;
	txtfile << "SumPeak: " << SumPeak << " +- " << DSumPeak << endl;

	Double_t ComptonEdge = Int_t(PeakX*(1-1/(1+2*PeakX/5110)))+1;
	cout <<"ComptonEdge @ Bin: " << ComptonEdge << endl;
	Double_t SumCompton=0, DSumCompton = 0;


  for (Int_t i =Compton(Energy,lowAngle)*ChannelResolution; i <= Compton(Energy,highAngle)*ChannelResolution; i++)
  {
		SumCompton += gamTde->GetBinContent(i);
	}
	DSumCompton = sqrt(SumCompton);

	cout << "lowEnChan: " << Compton(Energy,lowAngle)*ChannelResolution << endl << "highEnChan: "<< Compton(Energy,highAngle)*ChannelResolution << endl;
	txtfile << "lowEnChan: " << Compton(Energy,lowAngle)*ChannelResolution << endl << "highEnChan: "<< Compton(Energy,highAngle)*ChannelResolution << endl;
	cout <<"N. of Compton Events: "<< SumCompton<< " +- " << DSumCompton << endl;
	txtfile <<"N. of Compton Events: "<< SumCompton<< " +- " << DSumCompton << endl;
	Double_t PeakToCompton = 0;
	Double_t DPeakToCompton = 0;
	if (SumCompton != 0)
	{
		PeakToCompton = PeakY/SumCompton*(Compton(Energy,highAngle)-Compton(Energy,lowAngle))*ChannelResolution;
		DPeakToCompton = sqrt(pow(DPeakY/SumCompton,2)+ pow(PeakY*DSumCompton/SumCompton/SumCompton,2))*(Compton(Energy,highAngle)-Compton(Energy,lowAngle))*ChannelResolution;
		cout << "Peak/Compton (res): "<< PeakToCompton << " +- "<< DPeakToCompton << endl;
		txtfile << "Peak/Compton (res): "<< PeakToCompton << " +- "<< DPeakToCompton << endl;
	}
	else
	{

		cout << "SumCompton = 0 -> no PeakToCompton possible!"<<endl;
		txtfile << "SumCompton = 0 -> no PeakToCompton possible!"<<endl;
	}
	Double_t AllEntries = gamTde->GetEntries();
	cout << "Entries: " << AllEntries << endl;
	txtfile << "Entries: " << AllEntries << endl;


	//fitting the histograms

	TF1 *GausBG = new TF1("GausBG","gausn"/*(0)+pol1(3)" */,(PeakX-100)/ChannelResolution,(PeakX+100)/ChannelResolution);
		GausBG->SetParName(0,"Ampl");
		GausBG->SetParName(1,"x0");
		GausBG->SetParName(2,"sigma");
		GausBG->SetParameter(0,PeakY/10000);
		GausBG->SetParameter(1,PeakX/Double_t(ChannelResolution));
		GausBG->SetParameter(2,100./Double_t(ChannelResolution));
		GausBG->SetParLimits(0,0.0000001, 1000000);
		GausBG->SetParLimits(1,(PeakX-10)/Double_t(ChannelResolution),(PeakX+10)/Double_t(ChannelResolution));
		GausBG->SetParLimits(2,1./Double_t(ChannelResolution),100./Double_t(ChannelResolution));
		/*GausBG->SetParameter(0,1);
		GausBG->SetParameter(1,1);
		GausBG->SetParameter(2,0);
		GausBG->FixParameter(2,0);
		GausBG->FixParameter(1,0);
		GausBG->FixParameter(0,0);
		*/
		GausBG->SetNpx(100000);
		GausBG->SetLineColor(kRed);
	gamTde->Fit(GausBG,"R");

	txtfile << "Fitparameter Spectrum (model: gausn):" << endl;
	for (Int_t i = 0; i < 3; i++)
	{
		txtfile << GausBG->GetParName(i) << ":\t" << GausBG->GetParameter(i) << endl;
	}
	Double_t FWHM = GausBG->GetParameter(2)*2.3548200;
	txtfile << "FWHM[keV]:\t" <<  FWHM*1000000 << endl;
	cout << "FWHM[keV]:\t" <<  FWHM*1000000 << endl;

	TF1 *Poisson = new TF1("Poisson","gausn",0,15);//,2);			//maybe gausn? maybe no fit? //gausn --> no ,2);
		//Poisson->SetParameter(0,4);
		//Poisson->SetParameter(1,SumPeak);
		//Poisson->SetParLimits(0,0,12);
		Poisson->SetParName(0,"Ampl");//Poisson->SetParName(0,"lambda");
		Poisson->SetParName(1,"x0");//Poisson->SetParName(1,"Ampl");
		Poisson->SetParName(2,"sigma");
		//Poisson->SetParameter(1,1000);
		Poisson->SetNpx(100000);
		Poisson->SetLineColor(kRed);
	//hNoHits->Fit(Poisson);

	//txtfile << "Fitparameter Number of Hits (model: Ampl.*Poisson):" << endl;
	//for (Int_t i = 0; i < 3; i++)			// if gausn fit --> < 2 -> < 3 , if no fit, comment it out!!!
	//{
	//	txtfile << Poisson->GetParName(i) << ":\t" << Poisson->GetParameter(i) << endl;
	//}

	//some make-up for the histograms
	gamTde->SetXTitle("Energy [GeV]");
	gamTde->SetYTitle("Counts");
	gamTde->GetYaxis()->SetTitleOffset(1.35);
	hNoHits->SetXTitle("Number of Hits");
	hNoHits->SetYTitle("Counts");
	hNoHits->GetYaxis()->SetTitleOffset(1.1);

	cout << "Full-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParameter(0)*ChannelResolution/(nEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated
	txtfile << "Full-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParameter(0)*ChannelResolution/(nEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated
	// writing to files and closing
	cout << "Error ofFull-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParError(0)*ChannelResolution/(nEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated
	txtfile << "Error of Full-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParError(0)*ChannelResolution/(nEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated
	// writing to files and closing
	gamTde->Write();
	hNoHits->Write();
	//GausBG->Write();
	//Poisson->Write();
	fi->Close();
	txtfile.close();



	//if(g)
		//delete g;
	//if (gamTde)
		//delete gamTde;
	//if(hNoHits)
		//delete hNoHits;
	//if(GausBG)
		//delete GausBG;
	//if (Poisson)
		//delete Poisson;
	//if (fi)
		//delete fi;
	//if (can3)
		//delete can3;
	//if(hit_bar)
		//delete hit_bar;
	//if(mc_bar)
		//delete mc_bar;
	//if(s)
		//delete s;


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
