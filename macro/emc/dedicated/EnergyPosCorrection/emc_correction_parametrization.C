// Define interval in energy and theta, in which output histogram is provided
// Barrel (low energy, 0.03<e<1.0 GeV)
Double_t energyIntervalsBarrelLow[]= {0,0.03, 0.05, 0.07, 0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
Double_t thetaIntervalsBarrelLow[]= {22.,23.,24.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,105.,110.,115.,120.,125.,130.,135.,137., 139., 140., 141.};

// Barrel (high energy, 1.0<e<8.0 GeV)
Double_t energyIntervalsBarrelHigh[]= {1.0,1.1,1.2,1.5,2.0,2.5,3.0,3.5,4.0,5.0,6.0,7.0,8.0,9.0,10.0};
Double_t thetaIntervalsBarrelHigh[]= {22.,23.,24.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,105.,110.,115.,120.,125.,130.,135.,137., 139., 140., 141.};

// Fwd endcap (0.03<e<8.0 GeV)
Double_t energyIntervalsFwd[]= {0,0.03, 0.05, 0.07, 0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.5,2.0,2.5,3.0,3.5,4.0,5.0,6.0,7.0,8.0,9.0,10.0};
Double_t thetaIntervalsFwd[]= {5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,20.,21.,22.};

// Bwd endcap (0.03<e<2.0 GeV)
Double_t energyIntervalsBwd[]= {0,0.03,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.5,2.0};
Double_t thetaIntervalsBwd[]= {147.,148.,149.,150.,151.,152.,154.,156.,158.,160.,162.,164.,166.,168.,170.,172.};

// Shashlyk (0.01<e<16.0 GeV)
Double_t energyIntervalsShashlyk[]= {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.5,2.0,2.5,3.0,3.5,4.0,5.0,6.0,7.0,8.0,9.0,10.0};

Int_t nrEnergyIntervals[5];//BarrelLow, BarrelHigh, Fwd, Bwd, Shashlyk
Int_t nrThetaIntervals[4]; //BarrelLow, BarrelHigh, Fwd, Bwd

void emc_correction_parametrization(Int_t version, TString InputFile1, TString particle="gamma", Bool_t debug=false, Bool_t useStoredHistos=true)
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
TStopwatch timer;
timer.Start();
	
	// If exists intermidiate file with histograms and useStoredHistos==true
	// perform the fit only
	// If useStoredHistos==true but file "correction_histos.root" does not exist
	// then it is created
	Bool_t storeHistos=false;
	TFile *f_histos;
	if (useStoredHistos)
	{
		f_histos=new TFile("correction_histos_fit.root");
		if (f_histos->IsZombie())
		{
			storeHistos=true;
		}
		f_histos->Close();
	}

	TH2F *hisEnergyRatioBarrelLow, *hisEnergyRatioBarrelHigh;
	TH2F *hisEnergyRatioFwd, *hisEnergyRatioBwd;
	TH1F *hisEnergyRatioShashlyk;
	
	TString OutputFile="emc_correction_par_";
	OutputFile = OutputFile +particle+"_";
	OutputFile +=version;
	OutputFile += ".root";
	
	
	if (!useStoredHistos||storeHistos)
	{
		TChain *c=new TChain("cbmsim");
		if (InputFile1.Contains(".root"))
		{
			TFile *f1=new TFile(InputFile1);
			if (f1->IsZombie())
			{
				std::cout<<"Input file "<<InputFile1<<" does not exists"<<std::endl;
				abort();
			}
			c->Add(InputFile1);
		}
		else if (InputFile1.Contains(".txt"))
		{
			string file_name;
			ifstream infile(InputFile1.Data(), ios::in);
			
			while (getline(infile,file_name, '\n'))
			{
				if (std::string::npos != file_name.find(".root"))
				{
					TFile *f2=new TFile(file_name.c_str());
					if (f2->IsZombie())
					{
						std::cout<<"Input file "<<file_name<<" does not exists"<<std::endl;
						abort();
					}
					c->Add(file_name.c_str());
				}
			}
		}
		else
		{
			std::cout<<"Wrong input file: "<<InputFile1<<std::endl;
			abort();
		}
		
		nrEnergyIntervals[0]=sizeof(energyIntervalsBarrelLow)/sizeof(Double_t);
		nrEnergyIntervals[1]=sizeof(energyIntervalsBarrelHigh)/sizeof(Double_t);
		nrEnergyIntervals[2]=sizeof(energyIntervalsFwd)/sizeof(Double_t);
		nrEnergyIntervals[3]=sizeof(energyIntervalsBwd)/sizeof(Double_t);
		nrEnergyIntervals[4]=sizeof(energyIntervalsShashlyk)/sizeof(Double_t);

		nrThetaIntervals[0]=sizeof(thetaIntervalsBarrelLow)/sizeof(Double_t);
		nrThetaIntervals[1]=sizeof(thetaIntervalsBarrelHigh)/sizeof(Double_t);
		nrThetaIntervals[2]=sizeof(thetaIntervalsFwd)/sizeof(Double_t);
		nrThetaIntervals[3]=sizeof(thetaIntervalsBwd)/sizeof(Double_t);

		hisEnergyRatioBarrelLow=new TH2F("hisEnergyRatioBarrelLow","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervals[0]-1),energyIntervalsBarrelLow,(nrThetaIntervals[0]-1),thetaIntervalsBarrelLow);
		hisEnergyRatioBarrelLow->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
		hisEnergyRatioBarrelLow->GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
	
		hisEnergyRatioBarrelHigh=new TH2F("hisEnergyRatioBarrelHigh","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervals[1]-1),energyIntervalsBarrelHigh,(nrThetaIntervals[1]-1),thetaIntervalsBarrelHigh);
		hisEnergyRatioBarrelHigh->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
		hisEnergyRatioBarrelHigh->GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");

		hisEnergyRatioFwd=new TH2F("hisEnergyRatioFwd","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervals[2]-1),energyIntervalsFwd,(nrThetaIntervals[2]-1),thetaIntervalsFwd);
		hisEnergyRatioFwd->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
		hisEnergyRatioFwd->GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
	
		hisEnergyRatioBwd=new TH2F("hisEnergyRatioBwd","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervals[3]-1),energyIntervalsBwd,(nrThetaIntervals[3]-1),thetaIntervalsBwd);
		hisEnergyRatioBwd->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
		hisEnergyRatioBwd->GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
	
		hisEnergyRatioShashlyk=new TH1F("hisEnergyRatioShashlyk","Ene_MC/Ene_reco _Shashlyk_:  GetMean()",(nrEnergyIntervals[4]-1),energyIntervalsShashlyk);
		hisEnergyRatioShashlyk->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	
		// energy: "E_reco / E_MC" 
		TH1F hisEnergyRatio1[50][50]; // barrel low
		TH1F hisEnergyRatio2[50][50]; // barrel high
		TH1F hisEnergyRatio3[50][50]; // forward endcap
		TH1F hisEnergyRatio4[50][50]; // backward endcap
		TH1F hisEnergyRatio5[50]; // shashlyk
		
		for (Int_t i=0; i<(nrEnergyIntervals[0]-1); i++)
		{
			for (Int_t j=0; j<(nrThetaIntervals[0]-1); j++)
			{
				hisEnergyRatio1[i][j].SetBins(100,0.,2.);
				hisEnergyRatioBarrelLow->SetBinContent(i+1,j+1,1.);
			}
		}
		
		for (Int_t i=0; i<(nrEnergyIntervals[1]-1); i++)
		{
			for (Int_t j=0; j<(nrThetaIntervals[1]-1); j++)
			{
				hisEnergyRatio2[i][j].SetBins(100,0.,2.);
				hisEnergyRatioBarrelHigh->SetBinContent(i+1,j+1,1.);
			}
		}

		for (Int_t i=0; i<(nrEnergyIntervals[2]-1); i++)
		{
			for (Int_t j=0; j<(nrThetaIntervals[2]-1); j++)
			{
				hisEnergyRatio3[i][j].SetBins(100,0.,2.);
				hisEnergyRatioFwd->SetBinContent(i+1,j+1,1.);
			}
		}
		
		for (Int_t i=0; i<(nrEnergyIntervals[3]-1); i++)
		{
			for (Int_t j=0; j<(nrThetaIntervals[3]-1); j++)
			{
				hisEnergyRatio3[i][j].SetBins(100,0.,2.);
				hisEnergyRatioBwd->SetBinContent(i+1,j+1,1.);
			}
		}
		
		for (Int_t i=0; i<(nrEnergyIntervals[4]-1); i++)
		{
			hisEnergyRatio5[i].SetBins(100,0.,2.);
			hisEnergyRatioShashlyk->SetBinContent(i+1,j+1,1.);
		}
		
		TClonesArray* track_array=new TClonesArray("PndMCTrack");
		c->SetBranchAddress("MCTrack",&track_array);
		TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
		c->SetBranchAddress("EmcCluster",&cluster_array);

		Double_t cluster_energy, cluster_theta, cluster_phi; //position of the cluster
		Double_t thMC, phiMC, enMC;

timer.Stop();
Double_t rtime = timer.RealTime();
Double_t ctime = timer.CpuTime();
timer.Continue();
printf("Intitialization done RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
		
cout<<"c->GetEntries()="<<c->GetEntries()<<endl;
		for (Int_t j=0; j< c->GetEntries(); j++)
		{

			if (j%1000==0)
			{
				cout<<"Event "<<j<<endl;
			}
			c->GetEntry(j);

			// Select cluster of highest energy
			if (cluster_array->GetEntriesFast()>0) 
			{
				PndMCTrack *track=(PndMCTrack*)track_array->At(0);
				TLorentzVector p4mom=track->Get4Momentum();
				TVector3 mc_momentum=track->GetMomentum();
			
				thMC  = mc_momentum.Theta()*(180./TMath::Pi());
				phiMC = mc_momentum.Phi()*(180./TMath::Pi());
			
				enMC   = p4mom.E();

				Int_t    idWithHighestEnergy = 0;
				Double_t highestEnergy = -1.;
		
				for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
				{
					PndEmcCluster *cluster;
					cluster=(PndEmcCluster*)cluster_array->At(i);
					cluster_energy=cluster->energy();
	
					if (cluster_energy>highestEnergy)
					{
						idWithHighestEnergy = i;
						highestEnergy = cluster_energy;
					}
				}
		
				// Lets analyze that cluster! 
				PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(idWithHighestEnergy);
				TVector3 cluster_pos=cluster->where();
				cluster_theta=cluster_pos.Theta()*180./TMath::Pi();
				cluster_phi=cluster_pos.Phi()*180./TMath::Pi();
				cluster_energy=highestEnergy;
				Int_t module=cluster->GetModule();
				
				Int_t range_set;
				if (((module==1)||(module==2))&&(cluster_energy<=1.0))
				{
					range_set=1;
				}
				else if (((module==1)||(module==2))&&(cluster_energy>1.0))
				{
					range_set=2;
				}
				else
				{
					range_set=module; 
				}
				
				Int_t thetaBin=GetThetaBin(cluster_theta, range_set); // bin number for cluster_theta 
				Int_t energyBin=GetEnergyBin(cluster_energy, range_set);// bin number for cluster_energy 
				
				if (cluster_theta >= 141. && cluster_theta < 147.) continue; // Avoid the egges between barrel and bwendcap...
		
				Double_t en_div, phi_diff;
				if ((thetaBin<0&&range_set!=5) || energyBin<0)
				{
					cout<<"!!!!!!!!!!!!!!!!!!!!!!"<<endl;
					cout<<"Module="<<module<<" range_set="<<range_set<<endl;
					cout << "theta_reco =" << cluster_theta << "/ energy_reco =" << cluster_energy << endl;
					cout << "thetaBin= "<<thetaBin<<" & energyBin= "<<energyBin<<endl;
				}
				else
				{
					if (cluster_theta > (thMC-5) && cluster_theta <=(thMC+5) ){	   	   

						en_div = enMC/cluster_energy;
						phi_diff = phiMC-cluster_phi;
			
						if (fabs(phi_diff)<2.5 || fabs(phi_diff+360)<2.5 || fabs(phi_diff-360)<2.5)
						{
							switch (range_set)
							{
								case 1:
								{
									if ((fabs(phiMC) <5.)||fabs(phiMC-180.)<5.||fabs(phiMC-360.)<5.) break;
									if (en_div > 0.7 && en_div <1.3){
										hisEnergyRatio1[energyBin][thetaBin].Fill(en_div);
									}
									break;
								}
								case 2:
								{
									if ((fabs(phiMC) <5.)||fabs(phiMC-180.)<5.||fabs(phiMC-360.)<5.) break;
									if (en_div > 0.7 && en_div <1.3){
										hisEnergyRatio2[energyBin][thetaBin].Fill(en_div);
									}
									break;
								}
								case 3:
								{
									if (cluster_theta <11.) break; // Avoid the egges for FwEndCap...
									if (en_div > 0.7 && en_div <1.3){
										hisEnergyRatio3[energyBin][thetaBin].Fill(en_div);
									}
									break;
								}
								case 4:
								{
									if (en_div > 0.7 && en_div <1.3){
										hisEnergyRatio4[energyBin][thetaBin].Fill(en_div);
									}
									break;
								}
								case 5:
								{
									if ((en_div > 0.7 && en_div <1.3)&&(thMC>1.)){ // avoid edge around 0 degree
										hisEnergyRatio5[energyBin].Fill(en_div);
									}
									break;
								}
								default:
								{
									abort();
								}
							}
						} // Phi
					} // Theta
				} // ThetaBin & EnergyBin > 0
			}
		}
		
		std::cout<<"Array of histogram is filled"<<std::endl;
timer.Stop();
Double_t rtime = timer.RealTime();
Double_t ctime = timer.CpuTime();
timer.Continue();
printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
		// Loop over energy and theta bins 
		Double_t mean_en_ratio=1.;

		for (Int_t i=0; i<(nrEnergyIntervals[0]-1); i++)
		{
			for (Int_t j=0; j<(nrThetaIntervals[0]-1); j++)
			{
				if ( hisEnergyRatio1[i][j].GetEntries() == 0) {
					mean_en_ratio = 1;
				}else{ 
					mean_en_ratio = hisEnergyRatio1[i][j].GetMean();
				}
				hisEnergyRatioBarrelLow->SetBinContent(i+1,j+1,mean_en_ratio);
			}
		}
		
		for (Int_t i=0; i<(nrEnergyIntervals[1]-1); i++)
		{
			for (Int_t j=0; j<(nrThetaIntervals[1]-1); j++)
			{
				if ( hisEnergyRatio2[i][j].GetEntries() == 0) {
					mean_en_ratio = 1;
				}else{ 
					mean_en_ratio = hisEnergyRatio2[i][j].GetMean();
				}
				hisEnergyRatioBarrelHigh->SetBinContent(i+1,j+1,mean_en_ratio);
			}
		}

		for (Int_t i=0; i<(nrEnergyIntervals[2]-1); i++)
		{
			for (Int_t j=0; j<(nrThetaIntervals[2]-1); j++)
			{
				if ( hisEnergyRatio3[i][j].GetEntries() == 0) {
					mean_en_ratio = 1;
				}else{ 
					mean_en_ratio = hisEnergyRatio3[i][j].GetMean();
				}
				hisEnergyRatioFwd->SetBinContent(i+1,j+1,mean_en_ratio);
			}
		}

		for (Int_t i=0; i<(nrEnergyIntervals[3]-1); i++)
		{
			for (Int_t j=0; j<(nrThetaIntervals[3]-1); j++)
			{
				if ( hisEnergyRatio4[i][j].GetEntries() == 0) {
					mean_en_ratio = 1;
				}else{ 
					mean_en_ratio = hisEnergyRatio4[i][j].GetMean();
				}
				hisEnergyRatioBwd->SetBinContent(i+1,j+1,mean_en_ratio);
			}
		}

		for (Int_t i=0; i<(nrEnergyIntervals[4]-1); i++)
		{
			if ( hisEnergyRatio5[i].GetEntries() == 0) {
				mean_en_ratio = 1;
			}else{ 
				mean_en_ratio = hisEnergyRatio5[i].GetMean();
			}
			hisEnergyRatioShashlyk->SetBinContent(i+1,mean_en_ratio);
		}
		
		if (storeHistos)
		{
			f_histos=new TFile("correction_histos_fit.root","RECREATE");
			hisEnergyRatioBarrelLow->Write();
			hisEnergyRatioBarrelHigh->Write();
			hisEnergyRatioFwd->Write();
			hisEnergyRatioBwd->Write();
			hisEnergyRatioShashlyk->Write();
			f_histos->Close();
		}
		std::cout<<"Histograms are filled with mean values"<<std::endl;
	}
	
	if (useStoredHistos&&(!storeHistos))
	{
		f_histos=new TFile("correction_histos_fit.root");
		hisEnergyRatioBarrelLow=(TH2F*)f_histos->Get("hisEnergyRatioBarrelLow");
		hisEnergyRatioBarrelHigh=(TH2F*)f_histos->Get("hisEnergyRatioBarrelHigh");
		hisEnergyRatioFwd=(TH2F*)f_histos->Get("hisEnergyRatioFwd");
		hisEnergyRatioBwd=(TH2F*)f_histos->Get("hisEnergyRatioBwd");
		hisEnergyRatioShashlyk=(TH1F*)f_histos->Get("hisEnergyRatioShashlyk");
	}
	
	TF2 * func1 = new TF2("func1",FitFunction1,0.03,1.0,25.,135., 10);
	double iniParams1[10] = { 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 };
	func1->SetParameters(iniParams1);
	TCanvas *c1=new TCanvas("c1");
	hisEnergyRatioBarrelLow->Fit(func1,"RN"); //"SURF1"
	func1->Draw("SURF1");
	
 	TF2 * func2 = new TF2("func2",FitFunction1,1.0,8.0,25.,135., 10);
 	double iniParams2[10] = { 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 };
 	func2->SetParameters(iniParams2);
	TCanvas *c2=new TCanvas("c2");
 	hisEnergyRatioBarrelHigh->Fit(func2,"RN");
	func2->Draw("SURF1");//"cont1"
	
	TF2 * func3 = new TF2("func3",FitFunction1,0.03,8.0,11.,22., 10);
	double iniParams3[10] = { 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 };
	func3->SetParameters(iniParams3);
	TCanvas *c3=new TCanvas("c3");
	hisEnergyRatioFwd->Fit(func3,"RN");
	func3->Draw("SURF1");
	
	TF2 * func4 = new TF2("func4",FitFunction1,0.03,2.0,154.,161., 10);
	double iniParams4[10] = { 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 };
	func4->SetParameters(iniParams4);
	TCanvas *c4=new TCanvas("c4");
	hisEnergyRatioBwd->Fit(func4,"RN");
	func4->Draw("SURF1");
	
	TF1 * func5 = new TF1("func5",FitFunction2,0.0,10.0, 5);
	double iniParams5[10] = { 0.01, 0.01, 0.01, 0.01, 0.01};
	func5->SetParameters(iniParams5);
	TCanvas *c5=new TCanvas("c5");
	hisEnergyRatioShashlyk->Fit(func5,"R");
	
	
	if (debug)
	{
		c1->SaveAs("c1.root");
		c2->SaveAs("c2.root");
		c3->SaveAs("c3.root");
		c4->SaveAs("c4.root");
		c5->SaveAs("c5.root");
	}
	TFile fout(OutputFile,"recreate"); // output file with corrections

	PndEmcClusterCalibrationParObject *parObject=new PndEmcClusterCalibrationParObject();
	enum {barrel_low=1, barrel_high=2, fwcap=3, bwcap=4, fsc=5};
	Double_t pars1[10], pars2[10], pars3[10], pars4[10], pars5[5];
	
	Double_t *p1=func1->GetParameters();
	for (int i=0;i<10;i++)
		pars1[i]=p1[i];
	Double_t *p2=func2->GetParameters();
	for (int i=0;i<10;i++)
		pars2[i]=p2[i];
	Double_t *p3=func3->GetParameters();
	for (int i=0;i<10;i++)
		pars3[i]=p3[i];
	Double_t *p4=func4->GetParameters();
	for (int i=0;i<10;i++)
		pars4[i]=p4[i];
	Double_t *p5=func5->GetParameters();
	for (int i=0;i<5;i++)
		pars5[i]=p5[i];
	
	parObject->SetCalibrationPar(barrel_low, pars1);
	parObject->SetCalibrationPar(barrel_high, pars2);
	parObject->SetCalibrationPar(fwcap, pars3);
	parObject->SetCalibrationPar(bwcap, pars4);
	parObject->SetCalibrationPar(fsc, pars5);
	
	parObject->Write();
	
	fout.Close();
timer.Stop();
Double_t rtime = timer.RealTime();
Double_t ctime = timer.CpuTime();
printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
	
}



Double_t FitFunction1(Double_t *x, Double_t *par)
{
	double e=x[0];
	double theta=x[1]*TMath::DegToRad();

	double factor1= par[0]
			  +par[1]*log(e)
			  +par[2]*log(e)*log(e)
			  +par[3]*log(e)*log(e)*log(e)
			  +par[4]*cos(theta)
			  +par[5]*cos(theta)*cos(theta)
			  +par[6]*cos(theta)*cos(theta)*cos(theta)
			  +par[7]*cos(theta)*cos(theta)*cos(theta)*cos(theta)
			  +par[8]*cos(theta)*cos(theta)*cos(theta)*cos(theta)*cos(theta)
			  +par[9]*log(e)*cos(theta);

  double res=exp(factor1);
  
  return res;
}


Double_t FitFunction2(Double_t *x, Double_t *par)
{
	double e=x[0];
	double res=(par[0]-par[1]/sqrt(e)+par[2]/e+par[3]/(e*e)-par[4]/(e*sqrt(e)));
	return res;
}

Int_t GetThetaBin(Double_t val, Int_t range_set)
{
	Double_t *thetaIntervals=0;
	Int_t nrThetaInt=0;
	switch (range_set)
	{
		case 1:
			nrThetaInt=nrThetaIntervals[0];
			thetaIntervals=thetaIntervalsBarrelLow;
			break;
		case 2:
			nrThetaInt=nrThetaIntervals[1];
			thetaIntervals=thetaIntervalsBarrelHigh;
			break;
		case 3:
			nrThetaInt=nrThetaIntervals[2];
			thetaIntervals=thetaIntervalsFwd;
			break;
		case 4:
			nrThetaInt=nrThetaIntervals[3];
			thetaIntervals=thetaIntervalsBwd;
			break;
		case 5:
			return -1;
		default:
			std::cout<<"Wrong EMC module: "<<range_set<<std::endl;
			abort();
	}
	
	for (Int_t i=0; i<(nrThetaInt-1); i++)
	{
		if (val>=thetaIntervals[i] && val<thetaIntervals[i+1])
		{
			return (i);
		}
	}
	return -1;
}

Int_t GetEnergyBin(Double_t val, Int_t range_set)
{
	Double_t *energyIntervals=0;
	Int_t nrEnergyInt=0;
	switch (range_set)
	{
		case 1:
			nrEnergyInt=nrEnergyIntervals[0];
			energyIntervals=energyIntervalsBarrelLow;
			break;
		case 2:
			nrEnergyInt=nrEnergyIntervals[1];
			energyIntervals=energyIntervalsBarrelHigh;
			break;
		case 3:
			nrEnergyInt=nrEnergyIntervals[2];
			energyIntervals=energyIntervalsFwd;
			break;
		case 4:
			nrEnergyInt=nrEnergyIntervals[3];
			energyIntervals=energyIntervalsBwd;
			break;
		case 5:
			nrEnergyInt=nrEnergyIntervals[4];
			energyIntervals=energyIntervalsShashlyk;
			break;
		default:
			std::cout<<"Wrong EMC module: "<<range_set<<std::endl;
			abort();
	}
	
	for (Int_t i=0; i<(nrEnergyInt-1); i++)
	{
		if (val>=energyIntervals[i] && val<energyIntervals[i+1])
		{
			return (i);
		}
	}
	return -1;
	
}

