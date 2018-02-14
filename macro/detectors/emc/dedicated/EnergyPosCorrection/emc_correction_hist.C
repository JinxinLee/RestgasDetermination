// This macro produce histograms for energy-position correction of EMC clusters
// It reads input file with simulated-reconstructed data,
// create array of histograms with ratio of reconstructed
// and generated energy and difference of reconstructed and simulated polar angle
// for given energy-theta interval, then in each histogram mean value is calculated,
// which is stored in 2D histogram and used for correction 
// Output file name have pattern:
// emc_correction_<particle>-_<version>.root, e.g. "emc_correction_gamma_1.root"
// Input parametr InputFile1 can be root file or text file containing list of root files

// Define interval in energy and theta, in which output histogram is provided
Double_t energyIntervals[]= {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.5,2.0,2.5,3.0,3.5,4.0,5.0,6.0,7.0,8.0,9.0,10.0};
Double_t thetaIntervalsBarrel[]= {22.,23.,24.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,105.,110.,115.,120.,125.,130.,135.,137., 139., 140., 141.};
Double_t thetaIntervalsFwd[]= {5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,20.,21.,22.};
Double_t thetaIntervalsBwd[]= {147.,148.,149.,150.,151.,152.,154.,156.,158.,160.,161.,162.,163.,164.,165.,166.,168.,170.,172.};
Double_t thetaIntervalsShashlyk[]= {0.,0.5,1.,1.5,2.,2.5,3.,3.5,4.,4.5,5.,5.5,6.,6.5,7.,7.5,8.,8.5,9.,9.5,10.};

Int_t nrEnergyIntervals;
Int_t nrThetaIntervals[4]; //Barrel, Fwd, Bwd, Shashlyk

// version is used for output file name
int emc_correction_hist(Int_t version, TString InputFile1, Bool_t debug=false)
{
	TStopwatch timer;
	timer.Start();

	TChain *c=new TChain("cbmsim");
	TString particle="";
	if (InputFile1.Contains(".root"))
	{
		TObjArray* tmp=InputFile1.Tokenize("_");
		TObjString *os=(TObjString *)tmp->At(2);
		particle=os->GetString();
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
				if (particle=="")
				{
					TObjArray* tmp=TString(file_name).Tokenize("_");
					TObjString *os=(TObjString *)tmp->At(2);
					particle=os->GetString();
				}
			}
		}
	}
	else
	{
		std::cout<<"Wrong input file: "<<InputFile1<<std::endl;
		abort();
	}
	
	TString OutputFile="emc_correction_hist_";
	OutputFile = OutputFile +particle+"_";
	OutputFile +=version;
	OutputFile += ".root";
	
	nrEnergyIntervals=sizeof(energyIntervals)/sizeof(Double_t);
	nrThetaIntervals[0]=sizeof(thetaIntervalsBarrel)/sizeof(Double_t);
	nrThetaIntervals[1]=sizeof(thetaIntervalsFwd)/sizeof(Double_t);
	nrThetaIntervals[2]=sizeof(thetaIntervalsBwd)/sizeof(Double_t);
	nrThetaIntervals[3]=sizeof(thetaIntervalsShashlyk)/sizeof(Double_t);

	TH2F hisEnergyRatioBarrel, hisEnergyRatioFwd, hisEnergyRatioBwd, hisEnergyRatioShashlyk;
	TH2F hisThetaDiffBarrel, hisThetaDiffFwd, hisThetaDiffBwd, hisThetaDiffShashlyk;

	TH2F hisEnergyRatioBarrel("hisEnergyRatioBarrel","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals[0]-1),thetaIntervalsBarrel);
	hisEnergyRatioBarrel.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisEnergyRatioBarrel.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
  
	TH2F hisThetaDiffBarrel("hisThetaDiffBarrel","Theta_MC - Theta_reco:  GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals[0]-1),thetaIntervalsBarrel);
	hisThetaDiffBarrel.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisThetaDiffBarrel.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");

	TH2F hisEnergyRatioFwd("hisEnergyRatioFwd","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals[1]-1),thetaIntervalsFwd);
	hisEnergyRatioFwd.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisEnergyRatioFwd.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
  
	TH2F hisThetaDiffFwd("hisThetaDiffFwd","Theta_MC - Theta_reco:  GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals[1]-1),thetaIntervalsFwd);
	hisThetaDiffFwd.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisThetaDiffFwd.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");

	TH2F hisEnergyRatioBwd("hisEnergyRatioBwd","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals[2]-1),thetaIntervalsBwd);
	hisEnergyRatioBwd.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisEnergyRatioBwd.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
  
	TH2F hisThetaDiffBwd("hisThetaDiffBwd","Theta_MC - Theta_reco:  GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals[2]-1),thetaIntervalsBwd);
	hisThetaDiffBwd.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisThetaDiffBwd.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");

	TH2F hisEnergyRatioShashlyk("hisEnergyRatioShashlyk","Ene_MC/Ene_reco _Shashlyk_:  GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals[3]-1),thetaIntervalsShashlyk);
	hisEnergyRatioShashlyk.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisEnergyRatioShashlyk.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
  
	TH2F hisThetaDiffShashlyk("hisThetaDiffShashlyk","Theta_MC-Theta_reco _Shashlyk_:  GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals[3]-1),thetaIntervalsShashlyk);
	hisThetaDiffShashlyk.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisThetaDiffShashlyk.GetYaxis()->SetTitle("Cluster Reconstructed  #theta Photon Angle (#circ)");
	

	// energy: "E_reco / E_MC" 
	TH1F hisEnergyRatio1[100][100]; // barrel
	TH1F hisEnergyRatio2[100][100]; // forward endcap
	TH1F hisEnergyRatio3[100][100]; // backward endcap
	TH1F hisEnergyRatio4[100][100]; // shashlyk
	
	// Theta: "Theta_MC - Theta_reco"
	TH1F hisThetaDiff1[100][100]; // barrel
	TH1F hisThetaDiff2[100][100]; // forward endcap
	TH1F hisThetaDiff3[100][100]; // backward endcap
	TH1F hisThetaDiff4[100][100]; // shashlyk
	
	char cmd[64];
	
	for (Int_t i=0; i<(nrEnergyIntervals-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervals[0]-1); j++)
		{
			sprintf(cmd,"hisEnergyRatio1_%i_%i",i,j);
			hisEnergyRatio1[i][j].SetName(cmd);
			hisEnergyRatio1[i][j].SetTitle(cmd);
			hisEnergyRatio1[i][j].SetBins(100,0.,2.);
			sprintf(cmd,"hisThetaDiff1_%i_%i",i,j);
			hisThetaDiff1[i][j].SetName(cmd);
			hisThetaDiff1[i][j].SetTitle(cmd);
			hisThetaDiff1[i][j].SetBins(100,-5.,5);
			hisEnergyRatioBarrel.SetBinContent(i+1,j+1,1.);
			hisThetaDiffBarrel.SetBinContent(i+1,j+1,0.);
		}
		for (Int_t j=0; j<(nrThetaIntervals[1]-1); j++)
		{
			sprintf(cmd,"hisEnergyRatio2_%i_%i",i,j);
			hisEnergyRatio2[i][j].SetName(cmd);
			hisEnergyRatio2[i][j].SetTitle(cmd);
			hisEnergyRatio2[i][j].SetBins(100,0.,2.);
			sprintf(cmd,"hisThetaDiff2_%i_%i",i,j);
			hisThetaDiff2[i][j].SetName(cmd);
			hisThetaDiff2[i][j].SetTitle(cmd);
			hisThetaDiff2[i][j].SetBins(100,-5.,5);
			hisEnergyRatioFwd.SetBinContent(i+1,j+1,1.);
			hisThetaDiffFwd.SetBinContent(i+1,j+1,0.);
		}
		for (Int_t j=0; j<(nrThetaIntervals[2]-1); j++)
		{
			sprintf(cmd,"hisEnergyRatio3_%i_%i",i,j);
			hisEnergyRatio3[i][j].SetName(cmd);
			hisEnergyRatio3[i][j].SetTitle(cmd);
			hisEnergyRatio3[i][j].SetBins(100,0.,2.);
			sprintf(cmd,"hisThetaDiff3_%i_%i",i,j);
			hisThetaDiff3[i][j].SetName(cmd);
			hisThetaDiff3[i][j].SetTitle(cmd);
			hisThetaDiff3[i][j].SetBins(100,-5.,5);
			hisEnergyRatioBwd.SetBinContent(i+1,j+1,1.);
			hisThetaDiffBwd.SetBinContent(i+1,j+1,0.);
		}
		for (Int_t j=0; j<(nrThetaIntervals[3]-1); j++)
		{
			sprintf(cmd,"hisEnergyRatio4_%i_%i",i,j);
			hisEnergyRatio4[i][j].SetName(cmd);
			hisEnergyRatio4[i][j].SetTitle(cmd);
			hisEnergyRatio4[i][j].SetBins(100,0.,2.);
			sprintf(cmd,"hisThetaDiff4_%i_%i",i,j);
			hisThetaDiff4[i][j].SetName(cmd);
			hisThetaDiff4[i][j].SetTitle(cmd);
			hisThetaDiff4[i][j].SetBins(100,-5.,5);
			hisEnergyRatioShashlyk.SetBinContent(i+1,j+1,1.);
			hisThetaDiffShashlyk.SetBinContent(i+1,j+1,0.);
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	TClonesArray* track_array=new TClonesArray("PndMCTrack");
	c->SetBranchAddress("MCTrack",&track_array);
	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
	c->SetBranchAddress("EmcCluster",&cluster_array);


	Double_t cluster_energy, cluster_theta, cluster_phi; //position of the cluster
	Double_t thMC, phiMC, enMC;
	for (Int_t j=0; j< c->GetEntries(); j++)
	{
		if (j%1000==0)
			cout<<"Event "<<j<<endl;

		c->GetEntry(j);
	
		PndMCTrack *track=(PndMCTrack*)track_array->At(0);
		TLorentzVector p4mom=track->Get4Momentum();
		TVector3 mc_momentum=track->GetMomentum();
	
		thMC  = mc_momentum.Theta()*(180./TMath::Pi());
		phiMC = mc_momentum.Phi()*(180./TMath::Pi());
	
		enMC   = p4mom.E();
	
		// Select cluster of highest energy
		if (cluster_array->GetEntriesFast()>0) 
		{
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
			cluster_energy=cluster->energy();
			Int_t module=cluster->GetModule();
			
			Int_t thetaBin=GetThetaBin(cluster_theta, module); // bin number for cluster_theta 
			Int_t energyBin=GetEnergyBin(cluster_energy, module);// bin number for cluster_energy 
			
			if (cluster_theta >= 141. && cluster_theta < 147.) continue; // Avoid the egges between barrel and bwendcap...
      
			Double_t en_div, phi_diff, theta_diff;
			if (thetaBin<0 || energyBin<0)
			{
				cout<<"!!!!!!!!!!!!!!!!!!!!!!"<<endl;
				cout<<"Module="<<module<<endl;
				cout << "theta_reco =" << cluster_theta << "/ energy_reco =" << cluster_energy << endl;
				cout << "thetaBin= "<<thetaBin<<" & energyBin= "<<energyBin<<endl;
			}
			else
			{	
				if (cluster_theta > (thMC-5) && cluster_theta <=(thMC+5) ){	   	   

					en_div = cluster_energy/enMC;
					phi_diff = phiMC-cluster_phi;
					theta_diff= thMC-cluster_theta;
						    
					if (fabs(phi_diff)<2.5 || fabs(phi_diff+360)<2.5 || fabs(phi_diff-360)<2.5){
						switch (module)
						{
							case 1:
							case 2:
							{
								// avoid beam pipe region
								if ((fabs(phiMC) <5.)||fabs(phiMC-180.)<5.||fabs(phiMC-360.)<5.) break;
								if (en_div > 0.7 && en_div <1.3){
									hisEnergyRatio1[energyBin][thetaBin].Fill(en_div);
									hisThetaDiff1[energyBin][thetaBin].Fill(theta_diff);	 
								}
								break;
							}
							case 3:
							{
								if (cluster_theta <6.) break; // Avoid the egges for FwEndCap...
								if (en_div > 0.7 && en_div <1.3){
									hisEnergyRatio2[energyBin][thetaBin].Fill(en_div);
									hisThetaDiff2[energyBin][thetaBin].Fill(theta_diff);	 
								}
								break;
							}
							case 4:
							{
								if (en_div > 0.7 && en_div <1.3){
									hisEnergyRatio3[energyBin][thetaBin].Fill(en_div);
									hisThetaDiff3[energyBin][thetaBin].Fill(theta_diff);	 
								}
								break;
							}
							case 5:
							{
								if (en_div > 0.7 && en_div <1.3){
									hisEnergyRatio4[energyBin][thetaBin].Fill(en_div);
									hisThetaDiff4[energyBin][thetaBin].Fill(theta_diff);	 
								}
								break;
							}
							default:
							{
								abort();
							}
						}
			
					}
				}
			} 
		}
	}


	// Loop over energy and theta bins to GetMean() values from energy and theta histograms
	// and write GetMean() value to the 2D histograms

	TFile fout(OutputFile,"recreate"); // output file with corrections
	Double_t mean_th_dif, mean_en_ratio;

	for (Int_t i=0; i<(nrEnergyIntervals-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervals[0]-1); j++)
		{
			//
			// Energy & Theta for Barrel
			//
			if (hisThetaDiff1[i][j].GetEntries() == 0) {
				mean_th_dif = 0;
			}else{
				mean_th_dif = hisThetaDiff1[i][j].GetMean(); // (Theta_MC - Theta_reco)
			}

			hisThetaDiffBarrel.SetBinContent(i+1,j+1,mean_th_dif); // Theta_MC - Theta_Reco: GetMean()

			if ( hisEnergyRatio1[i][j].GetEntries() == 0) {
				mean_en_ratio = 1;
			}else{ 
				mean_en_ratio = hisEnergyRatio1[i][j].GetMean(); // (E_MC / E_reco)
			}
	  
			hisEnergyRatioBarrel.SetBinContent(i+1,j+1,mean_en_ratio);  // Mean       
		}
		for (Int_t j=0; j<(nrThetaIntervals[1]-1); j++)
		{
			//
			// Energy & Theta for Forward Edncap
			//
			if (hisThetaDiff2[i][j].GetEntries() == 0) {
				mean_th_dif = 0;
			}else{
				mean_th_dif = hisThetaDiff2[i][j].GetMean(); // (Theta_MC - Theta_reco)
			}

			hisThetaDiffFwd.SetBinContent(i+1,j+1,mean_th_dif); // Theta_MC - Theta_Reco: GetMean()

			if ( hisEnergyRatio2[i][j].GetEntries() == 0) {
				mean_en_ratio = 1;
			}else{ 
				mean_en_ratio = hisEnergyRatio2[i][j].GetMean(); // (E_MC / E_reco)
			}
	  
			hisEnergyRatioFwd.SetBinContent(i+1,j+1,mean_en_ratio);  // Mean       
		}
		for (Int_t j=0; j<(nrThetaIntervals[2]-1); j++)
		{
			//
			// Energy & Theta for Backward Endcap
			//
			if (hisThetaDiff3[i][j].GetEntries() == 0) {
				mean_th_dif = 0;
			}else{
				mean_th_dif = hisThetaDiff3[i][j].GetMean(); // (Theta_MC - Theta_reco)
			}

			hisThetaDiffBwd.SetBinContent(i+1,j+1,mean_th_dif); // Theta_MC - Theta_Reco: GetMean()

			if ( hisEnergyRatio3[i][j].GetEntries() == 0) {
				mean_en_ratio = 1;
			}else{ 
				mean_en_ratio = hisEnergyRatio3[i][j].GetMean(); // (E_MC / E_reco)
			}
	  
			hisEnergyRatioBwd.SetBinContent(i+1,j+1,mean_en_ratio);  // Mean       
		}
		for (Int_t j=0; j<(nrThetaIntervals[3]-1); j++)
		{
			//
			// Energy & theta for Shashlyk
			//
			if (hisThetaDiff4[i][j].GetEntries() == 0){
				mean_th_dif = 0;
			}else{
				mean_th_dif = hisThetaDiff4[i][j].GetMean();
			}

			hisThetaDiffShashlyk.SetBinContent(i+1,j+1,mean_th_dif); // Theta_MC - Theta_Reco: GetMean()

	  
			if (hisEnergyRatio4[i][j].GetEntries() == 0) {
				mean_en_ratio = 1;
			}else{
				mean_en_ratio = hisEnergyRatio4[i][j].GetMean();
			}
	  
			hisEnergyRatioShashlyk.SetBinContent(i+1,j+1,mean_en_ratio);
		}
	}
	
	hisEnergyRatioBarrel.Write();
	hisThetaDiffBarrel.Write();
	hisEnergyRatioFwd.Write();
	hisThetaDiffFwd.Write();
	hisEnergyRatioBwd.Write();
	hisThetaDiffBwd.Write();
	hisEnergyRatioShashlyk.Write();
	hisThetaDiffShashlyk.Write();
	
	if (debug)
	{
		for (Int_t i=0; i<(nrEnergyIntervals-1); i++)
		{
			for (Int_t j=0; j<(nrThetaIntervals[0]-1); j++)
			{
				TH1F *h1=(TH1F *)hisEnergyRatio1[i][j].Clone();
				h1->Write();
				TH1F *h1=(TH1F *)hisThetaDiff1[i][j].Clone();
				h1->Write();
			}
			for (Int_t j=0; j<(nrThetaIntervals[1]-1); j++)
			{
				TH1F *h1=(TH1F *)hisEnergyRatio2[i][j].Clone();
				h1->Write();
				TH1F *h1=(TH1F *)hisThetaDiff2[i][j].Clone();
				h1->Write();
			}
			for (Int_t j=0; j<(nrThetaIntervals[2]-1); j++)
			{
				TH1F *h1=(TH1F *)hisEnergyRatio3[i][j].Clone();
				h1->Write();
				TH1F *h1=(TH1F *)hisThetaDiff3[i][j].Clone();
				h1->Write();
			}
			for (Int_t j=0; j<(nrThetaIntervals[3]-1); j++)
			{
				TH1F *h1=(TH1F *)hisEnergyRatio4[i][j].Clone();
				h1->Write();
				TH1F *h1=(TH1F *)hisThetaDiff4[i][j].Clone();
				h1->Write();
			}
		}

	}
	fout.Close();

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

  return 0;
}

Int_t GetThetaBin(Double_t val, Int_t module)
{
	Double_t *thetaIntervals;
	Int_t nrThetaInt;
	switch (module)
	{
		case 1:
		case 2:
			nrThetaInt=nrThetaIntervals[0];
			thetaIntervals=thetaIntervalsBarrel;
			break;
		case 3:
			nrThetaInt=nrThetaIntervals[1];
			thetaIntervals=thetaIntervalsFwd;
			break;
		case 4:
			nrThetaInt=nrThetaIntervals[2];
			thetaIntervals=thetaIntervalsBwd;
			break;
		case 5:
			nrThetaInt=nrThetaIntervals[3];
			thetaIntervals=thetaIntervalsShashlyk;
			break;
		default:
			std::cout<<"Wrong EMC module: "<<module<<std::endl;
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

Int_t GetEnergyBin(Double_t val, Int_t module)
{
	for (Int_t i=0; i<(nrEnergyIntervals-1); i++)
	{
		if (val>=energyIntervals[i] && val<energyIntervals[i+1])
			return (i);
	}
	return -1;
}
