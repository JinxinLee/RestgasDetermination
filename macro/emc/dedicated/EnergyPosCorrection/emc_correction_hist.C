// This macro produce histograms for energy-position correction of EMC clusters
// It reads input file with simulated-reconstructed data, create array of histograms with ratio of reconstructed and generated energy and difference of reconstructed and simulated polar angle for given energy-theta interval, then in each histogram mean value is calculated, which is stored in 2D histogram and used for correction 
// Output file name have pattern <particle>_en_theta_corr_<TransportModel>_<version>.root, e.g. "gamma_en_theta_corr_TGeant3_1.root", correspondence between <version> and EMC geometry version is hard-coded in PndEmcClusterHistCalibrator::PndEmcClusterHistCalibrator()

// Define interval in energy and theta, in which output histogram is provided
Double_t energyIntervals[]= {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,2.0,2.5,3.0,3.5,4.0,5.0,6.0,7.0,8.0,9.0,10.0};

Double_t thetaIntervals[]= {0.,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,20.,21.,22.,23.,24.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,105.,110.,115.,120.,125.,130.,135.,141.,142.,147.,148.,149.,150.,151.,152.,154.,156.,158.,160.,162.,164.,166.,168.,170.,172.};

Int_t numBinIntervals[]= {70,70,70,70,70,70,70,70,70,70,70,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};
	
Int_t nrEnergyIntervals;
Int_t nrThetaIntervals;

void emc_correction_hist(TString InputClusterFile="emc_complete.root",
								  TString OutputFile="gamma_en_theta_corr_TGeant3_1.root")
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
	
	
	nrEnergyIntervals=sizeof(energyIntervals)/sizeof(Double_t);
	nrThetaIntervals=sizeof(thetaIntervals)/sizeof(Double_t);

	TH2F hisEnergyDelta("hisEnergyDelta","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals-1),thetaIntervals);
	hisEnergyDelta.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisEnergyDelta.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
	hisEnergyDelta.GetYaxis()->SetTitleOffset(1.5);
	hisEnergyDelta.GetYaxis()->SetTitleOffset(1.05);
  
	TH2F hisThetaDiff("hisThetaDiff","Theta_MC - Theta_reco:  GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals-1),thetaIntervals);
	hisThetaDiff.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisThetaDiff.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
	hisThetaDiff.GetYaxis()->SetTitleOffset(1.5);
	hisThetaDiff.GetYaxis()->SetTitleOffset(1.05);

  //for Shashlyk
	TH2F hisEnergy5Delta("hisEnergy5Delta","Ene_MC/Ene_reco _Shashlyk_:  GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals-1),thetaIntervals);
	hisEnergy5Delta.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisEnergy5Delta.GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
	hisEnergy5Delta.GetYaxis()->SetTitleOffset(1.5);
	hisEnergy5Delta.GetYaxis()->SetTitleOffset(1.05);
  
	TH2F hisTheta5Diff("hisTheta5Diff","Theta_MC-Theta_reco _Shashlyk_:  GetMean()",(nrEnergyIntervals-1),energyIntervals,(nrThetaIntervals-1),thetaIntervals);
	hisTheta5Diff.GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hisTheta5Diff.GetYaxis()->SetTitle("Cluster Reconstructed  #theta Photon Angle (#circ)");
	hisTheta5Diff.GetYaxis()->SetTitleOffset(1.5);
	hisTheta5Diff.GetYaxis()->SetTitleOffset(1.05);
	
	// energy: "E_reco / E_MC" for Target EMC & Shashlyk separatelly
	TH1F hisA0[100][100];
	TH1F hisA5[100][100];
	// Theta: "Theta_MC - Theta_reco" for Target EMC & Shashlyk separatelly
	TH1F hisE0[100][100];
	TH1F hisE5[100][100];
	
	char cmd[64];
	
	for (Int_t i=0; i<(nrEnergyIntervals-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervals-1); j++)
		{
			sprintf(cmd,"hisA0_%i_%i",i,j);
			hisA0[i][j].SetName(cmd);
			sprintf(cmd,"energy=%.2f theta=%.2f",GetEnergy(i),GetTheta(j));
			hisA0[i][j].SetTitle(cmd);
			hisA0[i][j].SetBins(numBinIntervals[i],0.,2.);        

			sprintf(cmd,"hisA5_%i_%i",i,j);
			hisA5[i][j].SetName(cmd);
			sprintf(cmd,"energy=%.2f theta=%.2f",GetEnergy(i),GetTheta(j));
			hisA5[i][j].SetTitle(cmd);
			hisA5[i][j].SetBins(numBinIntervals[i],0.,5.);        
			
			sprintf(cmd,"hisE0_%i_%i",i,j);
			hisE0[i][j].SetName(cmd);
			sprintf(cmd,"energy=%.2f theta=%.2f",GetEnergy(i),GetTheta(j));
			hisE0[i][j].SetTitle(cmd);
			hisE0[i][j].SetBins(100,-5.,5);

			sprintf(cmd,"hisE5_%i_%i",i,j);
			hisE5[i][j].SetName(cmd);
			sprintf(cmd,"energy=%.2f theta=%.2f",GetEnergy(i),GetTheta(j));
			hisE5[i][j].SetTitle(cmd);
			hisE5[i][j].SetBins(100,-5.,5);
		}
	}
	
	//
	// Histograms are set to 0 or 1 before filling by GetMean() values
	//
	for (Int_t i=0; i<(nrEnergyIntervals-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervals-1); j++)
		{	  
			hisEnergyDelta.SetBinContent(i+1,j+1,1.);
			hisThetaDiff.SetBinContent(i+1,j+1,0.);
			hisEnergy5Delta.SetBinContent(i+1,j+1,1.);
			hisTheta5Diff.SetBinContent(i+1,j+1,0.);
		}
	}

	TFile *f=new TFile(InputClusterFile);
	TChain *c=new TChain("cbmsim");
	c->Add(InputClusterFile);
	
	TClonesArray* track_array=new TClonesArray("PndMCTrack");
	c->SetBranchAddress("MCTrack",&track_array);

	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
	c->SetBranchAddress("EmcCluster",&cluster_array);

	Double_t cluster_energy, cluster_theta, cluster_phi; //position of the cluster
	Double_t thMC, phiMC, enMC;
	
	for (Int_t j=0; j< c->GetEntries(); j++)
	{
		cout<<" "<<endl;
		cout<<"Event "<<j<<endl;
		c->GetEntry(j);
	
		PndMCTrack *track=(PndMCTrack*)track_array->At(0);
		TLorentzVector p4mom=track->Get4Momentum();
		TVector3 mc_momentum=track->GetMomentum();
	
		thMC  = mc_momentum.Theta()*(180./TMath::Pi());
		phiMC = mc_momentum.Phi()*(180./TMath::Pi());
	
		enMC   = p4mom.E();
	
		cout<<"energy_MC= "<<enMC<<", theta_MC == "<<thMC<< ", phi_MC == "<<phiMC<< endl;
		
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
			
			Int_t thetaBin=GetThetaBin(cluster_theta); // bin number for cluster_theta 
			Int_t energyBin=GetEnergyBin(cluster_energy);// bin number for cluster_energy 
			
			if (cluster_theta >= 141. && cluster_theta < 150.) continue; // Avoid the egges between barrel and bwendcap...
      
			Double_t en_div, phi_diff, theta_diff;
			if (thetaBin<0 || energyBin<0)
			{
				cout << "theta_reco =" << cluster_theta << "/ energy_reco =" << cluster_energy << endl;
				cout << "thetaBin= "<<thetaBin<<" & energyBin= "<<energyBin<<endl;
			}
			else
			{	
				if (cluster_theta > (thMC-5) && cluster_theta <=(thMC+5) ){	   	   

					en_div = cluster_energy/enMC;
					phi_diff = phiMC-cluster_phi;
					theta_diff= thMC-cluster_theta;
					Int_t module=cluster->GetModule();
	    
					if (fabs(phi_diff)<2.5 || fabs(phi_diff+360)<2.5 || fabs(phi_diff-360)<2.5){
	      
						if (module==5) // Above 12 degrees there is no events for Shashlyk
						{
							if (en_div > 0.1){

								hisA5[energyBin][thetaBin].Fill(en_div);
								hisE5[energyBin][thetaBin].Fill(theta_diff);	 
							}
						}
						else
						{
							if (cluster_theta <6.) continue; // Avoid the egges for FwEndCap...
		    
							if (en_div > 0.4 && en_div <1.6){

								hisA0[energyBin][thetaBin].Fill(en_div);
								hisE0[energyBin][thetaBin].Fill(theta_diff);	 
							}
						}
					} // Phi
				} // Theta
			} // ThetaBin & EnergyBin > 0


		}
		
	}
	
	// Loop over energy and theta bins to GetMean() values from energy and theta histograms
	// and write GetMean() value to the 2D histograms

	TFile fout(OutputFile,"recreate"); // output file with corrections
	Double_t mean_th_dif, mean_en_dif;
	Double_t mean_th5_dif, mean_en5_dif; //Shashlyk

	for (Int_t i=0; i<(nrEnergyIntervals-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervals-1); j++)
		{
			//
			// Energy & theta for Shashlyk
			//
			if (hisE5[i][j].GetEntries() == 0){
				mean_th5_dif = 0;
			}else{
				mean_th5_dif = hisE5[i][j].GetMean();
			}

			hisTheta5Diff.SetBinContent(i+1,j+1,mean_th5_dif); // Theta_MC - Theta_Reco: GetMean()

	  
			if (hisA5[i][j].GetEntries() == 0) {
				mean_en5_dif = 1;
			}else{
				mean_en5_dif = hisA5[i][j].GetMean();
			}
	  
			hisEnergy5Delta.SetBinContent(i+1,j+1,mean_en5_dif);
  
			//
			// Energy & Theta for Target EMC
			//
			if (hisE0[i][j].GetEntries() == 0) {
				mean_th_dif = 0;
			}else{
				mean_th_dif = hisE0[i][j].GetMean(); // hisE0 = (Theta_MC - Theta_reco)
			}

			hisThetaDiff.SetBinContent(i+1,j+1,mean_th_dif); // Theta_MC - Theta_Reco: GetMean()

			if ( hisA0[i][j].GetEntries() == 0) {
				mean_en_dif = 1;
			}else{ 
				mean_en_dif = hisA0[i][j].GetMean(); // hisA0 = (E_MC / E_reco)
			}
	  
			hisEnergyDelta.SetBinContent(i+1,j+1,mean_en_dif);  // Mean       
	 
		}
	}
	
	hisThetaDiff.Write();
	hisEnergyDelta.Write();

	hisTheta5Diff.Write();
	hisEnergy5Delta.Write();

	fout.Close();
}


Double_t GetTheta(Int_t i)
{
	return thetaIntervals[i];
}

Double_t GetEnergy(Int_t i)
{
	return ((energyIntervals[i]+energyIntervals[i+1])/2.);
}

Int_t GetThetaBin(Double_t val)
{
	for (Int_t i=0; i<(nrThetaIntervals-1); i++)
	{
		if (val>=thetaIntervals[i] && val<thetaIntervals[i+1])
			return (i);
	}
	return -1;
}

Int_t GetEnergyBin(Double_t val)
{
	for (Int_t i=0; i<(nrEnergyIntervals-1); i++)
	{
		if (val>=energyIntervals[i] && val<energyIntervals[i+1])
			return (i);
	}
	return -1;
}
