#include <map>
// Define interval in energy and theta, in which output histogram is provided
// Barrel (low energy, 0.03<e<1.0 GeV)
	
Double_t energyIntervalsBarrelLow[]= {0,0.03, 0.05, 0.07, 0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
	
Double_t thetaIntervalsBarrelLow[]= {22.,23.,24.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,105.,110.,115.,120.,125.,130.,135.,140.};

// Barrel (high energy, 1.0<e<8.0 GeV)
Double_t energyIntervalsBarrelHigh[]= {1.0,1.1,1.2,1.3,1.4,1.5,2.0,2.5,3.0,3.5,4.0,5.0,6.0,7.0,8.0,9.0,10.0};

Double_t thetaIntervalsBarrelHigh[]= {22.,23.,24.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,105.,110.,115.,120.,125.,130.,135.,140.};

// Fwd endcap (0.03<e<8.0 GeV)
Double_t energyIntervalsFwd[]= {0,0.03, 0.05, 0.07, 0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,2.0,2.5,3.0,3.5,4.0,5.0,6.0,7.0,8.0,9.0,10.0};

Double_t thetaIntervalsFwd[]= {5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,20.,21.,22.};

// Bwd endcap (0.03<e<2.0 GeV)
Double_t energyIntervalsBwd[]= {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,2.0};

Double_t thetaIntervalsBwd[]= {147.,148.,149.,150.,151.,152.,154.,156.,158.,160.,162.,164.,166.,168.,170.,172.};

// Shashlyk (0.01<e<16.0 GeV)
Double_t energyIntervalsShashlyk[]= {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,2.0,2.5,3.0,3.5,4.0,5.0,6.0,7.0,8.0,9.0,10.0, 11.0, 12.0, 13.0, 14.0, 15.0 ,16.0};

Int_t nrEnergyIntervalsBarrelLow() {return sizeof(energyIntervalsBarrelLow)/sizeof(Double_t);};

Int_t nrThetaIntervalsBarrelLow() {return sizeof(thetaIntervalsBarrelLow)/sizeof(Double_t);};

Int_t nrEnergyIntervalsBarrelHigh() {return sizeof(energyIntervalsBarrelHigh)/sizeof(Double_t);};

Int_t nrThetaIntervalsBarrelHigh() {return sizeof(thetaIntervalsBarrelHigh)/sizeof(Double_t);};

Int_t nrEnergyIntervalsFwd() {return sizeof(energyIntervalsFwd)/sizeof(Double_t);};

Int_t nrThetaIntervalsFwd() {return sizeof(thetaIntervalsFwd)/sizeof(Double_t);};

Int_t nrEnergyIntervalsBwd() {return sizeof(energyIntervalsBwd)/sizeof(Double_t);};

Int_t nrThetaIntervalsBwd() {return sizeof(thetaIntervalsBwd)/sizeof(Double_t);};

Int_t nrEnergyIntervalsShashlyk() {return sizeof(energyIntervalsShashlyk)/sizeof(Double_t);};

void emc_correction_parametrization(TString InputFile="emc_complete.root",
TString OutputFile="emc_cluster_correction_par_gamma_TGeant3_1.root")
{
	gROOT->SetStyle("Plain");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
	
	// energy: "E_reco / E_MC" for Target EMC & Shashlyk separatelly
	TH1F hBarrelLow[100][100];
	TH1F hBarrelHigh[100][100];
	TH1F hFwdEndcap[100][100];
	TH1F hBwdEndcap[100][100];
	TH1F hShashlyk[100];
	
	TH2F *hBarrelLowEnergyRatio=new TH2F("hBarrelLowEnergyRatio","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervalsBarrelLow()-1),energyIntervalsBarrelLow,(nrThetaIntervalsBarrelLow()-1),thetaIntervalsBarrelLow);
	hBarrelLowEnergyRatio->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hBarrelLowEnergyRatio->GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
	
	TH2F *hBarrelHighEnergyRatio=new TH2F("hBarrelHighEnergyRatio","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervalsBarrelHigh()-1),energyIntervalsBarrelHigh,(nrThetaIntervalsBarrelHigh()-1),thetaIntervalsBarrelHigh);
	hBarrelHighEnergyRatio->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hBarrelHighEnergyRatio->GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
	
	TH2F *hFwdEndcapEnergyRatio=new TH2F("hFwdEndcapEnergyRatio","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervalsFwd()-1),energyIntervalsFwd,(nrThetaIntervalsFwd()-1),thetaIntervalsFwd);
	hFwdEndcapEnergyRatio->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hFwdEndcapEnergyRatio->GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");
   
	TH2F *hBwdEndcapEnergyRatio=new TH2F("hBwdEndcapEnergyRatio","Ene_MC/Ene_reco: GetMean()",(nrEnergyIntervalsBwd()-1),energyIntervalsBwd,(nrThetaIntervalsBwd()-1),thetaIntervalsBwd);
	hBwdEndcapEnergyRatio->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	hBwdEndcapEnergyRatio->GetYaxis()->SetTitle("Cluster Reconstructed #theta Photon Angle (#circ)");

  //for Shashlyk
	TH1F *hShashlykEnergyRatio=new TH1F("hShashlykEnergyRatio","Ene_MC/Ene_reco _Shashlyk_:  GetMean()",(nrEnergyIntervalsShashlyk()-1),energyIntervalsShashlyk);
	hShashlykEnergyRatio->GetXaxis()->SetTitle("Cluster Reconstructed Photon Energy (GeV)");
	
	char cmd[64];
	
	for (Int_t i=0; i<(nrEnergyIntervalsBarrelLow()-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervalsBarrelLow()-1); j++)
		{
			sprintf(cmd,"hBarrelLow_%i_%i",i,j);
			hBarrelLow[i][j].SetName(cmd);
			hBarrelLow[i][j].SetBins(50,0.,2.);        
		}
	}
	
	for (Int_t i=0; i<(nrEnergyIntervalsBarrelHigh()-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervalsBarrelHigh()-1); j++)
		{
			sprintf(cmd,"hBarrelHigh_%i_%i",i,j);
			hBarrelHigh[i][j].SetName(cmd);
			hBarrelHigh[i][j].SetBins(50,0.,2.);        
		}
	}
	
	for (Int_t i=0; i<(nrEnergyIntervalsFwd()-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervalsFwd()-1); j++)
		{
			sprintf(cmd,"hFwdEndcap_%i_%i",i,j);
			hFwdEndcap[i][j].SetName(cmd);
			hFwdEndcap[i][j].SetBins(50,0.,2.);        
		}
	}
	
	for (Int_t i=0; i<(nrEnergyIntervalsBwd()-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervalsBwd()-1); j++)
		{
			sprintf(cmd,"hBwdEndcap_%i_%i",i,j);
			hBwdEndcap[i][j].SetName(cmd);
			hBwdEndcap[i][j].SetBins(50,0.,2.);        
		}
	}
	
	for (Int_t i=0; i<(nrEnergyIntervalsShashlyk()-1); i++)
	{
		sprintf(cmd,"hShashlyk_%i",i);
		hShashlyk[i].SetName(cmd);
		hShashlyk[i].SetBins(50,0.,5.);        
	}
	
	TFile *f=new TFile(InputFile);
	TChain *c=new TChain("cbmsim");
	c->Add(InputFile);
	
	TClonesArray* track_array=new TClonesArray("PndMCTrack");
	c->SetBranchAddress("MCTrack",&track_array);

	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
	c->SetBranchAddress("EmcCluster",&cluster_array);

	Double_t cluster_energy, cluster_theta, cluster_phi; //position of the cluster
	Double_t thMC, phiMC, enMC;
	
	for (Int_t j=0; j< c->GetEntries(); j++)
	{
		if (j%100==0)
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
			Int_t module = cluster->GetModule();
			TVector3 cluster_pos=cluster->where();
			cluster_theta=cluster_pos.Theta()*180./TMath::Pi();
			cluster_phi=cluster_pos.Phi()*180./TMath::Pi();
			cluster_energy=cluster->energy();
			
			Int_t range_set;
			if ((module==1)&&(cluster_energy<1.0))
				range_set=1;
			else if ((module==1)&&(cluster_energy>1.0))
				range_set=2;
			else
				range_set=module; 
			
			Int_t thetaBin=GetThetaBin(cluster_theta, range_set); // bin number for cluster_theta 
			Int_t energyBin=GetEnergyBin(cluster_energy, range_set);// bin number for cluster_energy 
			
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
	    
					if (fabs(phi_diff)<2.5 || fabs(phi_diff+360)<2.5 || fabs(phi_diff-360)<2.5){
	      
						if (range_set==1)
						{
							if (en_div > 0.4 && en_div <1.6){
								hBarrelLow[energyBin][thetaBin].Fill(en_div);
							}
							
						}
						else if (range_set==2)
						{
							if (en_div > 0.4 && en_div <1.6){
								hBarrelHigh[energyBin][thetaBin].Fill(en_div);
							}
							
						}
						else if (range_set==3)
						{
							if (cluster_theta <6.) continue; // Avoid the egges for FwEndCap...
 
							if (en_div > 0.4 && en_div <1.6){
								hFwdEndcap[energyBin][thetaBin].Fill(en_div);
							}
						}
						else if (range_set==4)
						{
							if (en_div > 0.4 && en_div <1.6){
								hBwdEndcap[energyBin][thetaBin].Fill(en_div);
							}
						}
						else if (range_set==5)
						{
							if (en_div > 0.1){
								hShashlyk[energyBin].Fill(en_div);
							}
						}
					} // Phi
				} // Theta
			} // ThetaBin & EnergyBin > 0
		}
	}
	
	std::cout<<"Array of histogram is filled"<<std::endl;
	// Loop over energy and theta bins 
	Double_t mean_en_ratio;

	for (Int_t i=0; i<(nrEnergyIntervalsBarrelLow()-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervalsBarrelLow()-1); j++)
		{
			if ( hBarrelLow[i][j].GetEntries() == 0) {
				mean_en_ratio = 1;
			}else{ 
				mean_en_ratio = hBarrelLow[i][j].GetMean();
			}
			hBarrelLowEnergyRatio->SetBinContent(i+1,j+1,mean_en_ratio);
		}
	}
	
	for (Int_t i=0; i<(nrEnergyIntervalsBarrelHigh()-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervalsBarrelHigh()-1); j++)
		{
			if ( hBarrelHigh[i][j].GetEntries() == 0) {
				mean_en_ratio = 1;
			}else{ 
				mean_en_ratio = hBarrelHigh[i][j].GetMean();
			}
			hBarrelHighEnergyRatio->SetBinContent(i+1,j+1,mean_en_ratio);
		}
	}
	
	for (Int_t i=0; i<(nrEnergyIntervalsFwd()-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervalsFwd()-1); j++)
		{
			if ( hFwdEndcap[i][j].GetEntries() == 0) {
				mean_en_ratio = 1;
			}else{ 
				mean_en_ratio = hFwdEndcap[i][j].GetMean();
			}
			hFwdEndcapEnergyRatio->SetBinContent(i+1,j+1,mean_en_ratio);
		}
	}

	for (Int_t i=0; i<(nrEnergyIntervalsBwd()-1); i++)
	{
		for (Int_t j=0; j<(nrThetaIntervalsBwd()-1); j++)
		{
			if ( hBwdEndcap[i][j].GetEntries() == 0) {
				mean_en_ratio = 1;
			}else{ 
				mean_en_ratio = hBwdEndcap[i][j].GetMean();
			}
			hBwdEndcapEnergyRatio->SetBinContent(i+1,j+1,mean_en_ratio);
		}
	}
	
	for (Int_t i=0; i<(nrEnergyIntervalsShashlyk()-1); i++)
	{
		if ( hShashlyk[i].GetEntries() == 0) {
			mean_en_ratio = 1;
		}else{ 
			mean_en_ratio = hShashlyk[i].GetMean();
		}
		hShashlykEnergyRatio->SetBinContent(i+1,mean_en_ratio);
	}
	
	std::cout<<"Histograms are filled with mean values"<<std::endl;
	
	TF2 * func1 = new TF2("func1",FitFunction1,0.03,1.0,22.,140., 10);
	double iniParams1[10] = { 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 };
	func1->SetParameters(iniParams1);
	TCanvas *c1=new TCanvas();
	hBarrelLowEnergyRatio->Fit(func1);
	
 	TF2 * func2 = new TF2("func2",FitFunction1,1.0,8.0,22.,140., 10);
 	double iniParams2[10] = { 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 };
 	func2->SetParameters(iniParams2);
	TCanvas *c2=new TCanvas();
 	hBarrelHighEnergyRatio->Fit(func2);
	
	TF2 * func3 = new TF2("func3",FitFunction1,0.03,8.0,5.,22., 10);
	double iniParams3[10] = { 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 };
	func3->SetParameters(iniParams3);
	TCanvas *c3=new TCanvas();
	hFwdEndcapEnergyRatio->Fit(func3);
	
	TF2 * func4 = new TF2("func4",FitFunction1,0.03,2.0,147.,172., 10);
	double iniParams4[10] = { 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 };
	func4->SetParameters(iniParams4);
	TCanvas *c4=new TCanvas();
	hBwdEndcapEnergyRatio->Fit(func4);
	
	TF1 * func5 = new TF1("func5",FitFunction2,0.0,16.0, 5);
	double iniParams5[10] = { 0.01, 0.01, 0.01, 0.01, 0.01};
	func5->SetParameters(iniParams5);
	TCanvas *c5=new TCanvas();
	hShashlykEnergyRatio->Fit(func5);
	
	TFile fout(OutputFile,"recreate"); // output file with corrections

	PndEmcClusterCalibrationParObject *parObject=new PndEmcClusterCalibrationParObject();
	enum {barrel_low=1, barrel_high=2, fwcap=3, bwcap=4, fsc=5};
	Double_t pars1[10], pars2[10], pars3[10], pars4[10], pars5[5];
	
	Double_t *p1=func1->GetParameters();
	for (int i=0;i<10;i++)
		pars1[i]=p1[i];
	Double_t *p2=func2->GetParameters();
	for (int i=0;i<10;i++)
		pars1[i]=p2[i];
	Double_t *p3=func3->GetParameters();
	for (int i=0;i<10;i++)
		pars1[i]=p3[i];
	Double_t *p4=func4->GetParameters();
	for (int i=0;i<10;i++)
		pars1[i]=p4[i];
	Double_t *p5=func5->GetParameters();
	for (int i=0;i<5;i++)
		pars1[i]=p5[i];
	
	parObject->SetCalibrationPar(barrel_low, pars1);
	parObject->SetCalibrationPar(barrel_high, pars2);
	parObject->SetCalibrationPar(fwcap, pars3);
	parObject->SetCalibrationPar(bwcap, pars4);
	parObject->SetCalibrationPar(fsc, pars5);
	
	parObject->Write();
	
	fout.Close();
	
}



Double_t FitFunction1(Double_t *x, Double_t *par)
{
	double e=x[0];
	double theta=x[1];

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

  double res=e* exp(factor1);
  
  return res;
}


Double_t FitFunction2(Double_t *x, Double_t *par)
{
	double e=x[0];
	double res=(par[0]-par[1]/sqrt(e)+par[2]/e+par[3]/(e*e)-par[4]/(e*sqrt(e)))*e;
	return res;
}



Double_t GetTheta(Int_t i, Int_t range_set)
{
	switch (range_set){
		case 1:
			return thetaIntervalsBarrelLow[i];
			break;
		case 2:
			return thetaIntervalsBarrelHigh[i];
			break;
		case 3:
			return thetaIntervalsFwd[i];
			break;
		case 4:
			return thetaIntervalsBwd[i];
			break;
		default:
			std::cout<<"Wrong part in EnergyThetaIntervals::GetTheta()";
			abort();
	}
}

Double_t GetEnergy(Int_t i, Int_t range_set)
{
	switch (range_set){
		case 1:
			return ((energyIntervalsBarrelLow[i]+energyIntervalsBarrelLow[i+1])/2.);
			break;
		case 2:
			return ((energyIntervalsBarrelHigh[i]+energyIntervalsBarrelHigh[i+1])/2.);
			break;
		case 3:
			return ((energyIntervalsBarrelFwd[i]+energyIntervalsBarrelFwd[i+1])/2.);
			break;
		case 4:
			return ((energyIntervalsBwd[i]+energyIntervalsBwd[i+1])/2.);
			break;
		case 5:
			return ((energyIntervalsShashlyk[i]+energyIntervalsShashlyk[i+1])/2.);
			break;
		default:
			std::cout<<"Wrong part in EnergyThetaIntervals::GetEnergy()";
			abort();
	}

}

Int_t GetThetaBin(Double_t val, Int_t range_set)
{
	switch (range_set){
		case 1:
			for (Int_t i=0; i<(nrThetaIntervalsBarrelLow()-1); i++)
			{
				if (val>=thetaIntervalsBarrelLow[i] && val<thetaIntervalsBarrelLow[i+1])
					return (i);
			}
			return -1;
		case 2:
			for (Int_t i=0; i<(nrThetaIntervalsBarrelHigh()-1); i++)
			{
				if (val>=thetaIntervalsBarrelHigh[i] && val<thetaIntervalsBarrelHigh[i+1])
					return (i);
			}
			return -1;
		case 3:
			for (Int_t i=0; i<(nrThetaIntervalsFwd()-1); i++)
			{
				if (val>=thetaIntervalsFwd[i] && val<thetaIntervalsFwd[i+1])
					return (i);
			}
			return -1;
		case 4:
			for (Int_t i=0; i<(nrThetaIntervalsBwd()-1); i++)
			{
				if (val>=thetaIntervalsBwd[i] && val<thetaIntervalsBwd[i+1])
					return (i);
			}
			return -1;
		case 5:
			return -1;
		default:
			std::cout<<"Wrong part in EnergyThetaIntervals::GetThetaBin()";
			abort();
	}
	
}

Int_t GetEnergyBin(Double_t val, Int_t range_set)
{
	switch (range_set){
		case 1:
			for (Int_t i=0; i<(nrEnergyIntervalsBarrelLow()-1); i++)
			{
				if (val>=energyIntervalsBarrelLow[i] && val<energyIntervalsBarrelLow[i+1])
					return (i);
			}
			return -1;
		case 2:
			for (Int_t i=0; i<(nrEnergyIntervalsBarrelHigh()-1); i++)
			{
				if (val>=energyIntervalsBarrelHigh[i] && val<energyIntervalsBarrelHigh[i+1])
					return (i);
			}
			return -1;
		case 3:
			for (Int_t i=0; i<(nrEnergyIntervalsFwd()-1); i++)
			{
				if (val>=energyIntervalsFwd[i] && val<energyIntervalsFwd[i+1])
					return (i);
			}
			return -1;
		case 4:
			for (Int_t i=0; i<(nrEnergyIntervalsBwd()-1); i++)
			{
				if (val>=energyIntervalsBwd[i] && val<energyIntervalsBwd[i+1])
					return (i);
			}
			return -1;
		case 5:
			for (Int_t i=0; i<(nrEnergyIntervalsShashlyk()-1); i++)
			{
				if (val>=energyIntervalsShashlyk[i] && val<energyIntervalsShashlyk[i+1])
					return (i);
			}
			return -1;
		default:
			std::cout<<"Wrong part in EnergyThetaIntervals::GetEnergyBin()";
			abort();
	}
}
