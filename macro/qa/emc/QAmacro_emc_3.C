void QAmacro_emc_3()
{
  // Macro loads a file after reconstruction and plots difference between initial direction of particle and angular position of cluster
	////////////////////////////////////////////////////////////////////////////////
	// The following part of macro access RunTimeDataBase and initialize PndEmcMapper from it 
	////////////////////////////////////////////////////////////////////////////////
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile("sim_emc.root");
	fRun->SetOutputFile("dummy_out.root");
	
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open("simparams.root");
	
  	TString emcAsciiPar = gSystem->Getenv("VMCWORKDIR");
	emcAsciiPar += "/macro/params/";
	emcAsciiPar += "emc.par";
	
	FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
	parInput2->open(emcAsciiPar.Data(),"in");
	
	rtdb->setFirstInput(parInput1);
	rtdb->setSecondInput(parInput2);
	
	PndEmcGeoPar *geoPar = (PndEmcGeoPar*) rtdb->getContainer("PndEmcGeoPar");
	fRun->Init();
	
	geoPar->InitEmcMapper();
	std::cout<<""<<std::endl;
	/////////////////////////////////////////////////////////////////////////////////
	
	TFile* f = new TFile("full_emc.root"); //file you want to analyse
	TTree *t=(TTree *) f->Get("cbmsim") ;
         
	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
	t->SetBranchAddress("EmcCluster",&cluster_array);
	TClonesArray* digi_array=new TClonesArray("PndEmcDigi");
	t->SetBranchAddress("EmcDigi",&digi_array);

	TFile* fsim = new TFile("sim_emc.root"); //file you want to analyse
	TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
	
	TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
	tsim->SetBranchAddress("MCTrack",&mctrack_array);
	
	TVector3 photon_momentum;

	double cluster_energy;
	double cluster_energy_calibrated;
	double cluster_theta, cluster_phi; //position of the cluster
	double theta, phi; // angular position of the initial particle
	double theta_diff, phi_diff;
	int ndigi, npoint;
	double max_energy=0;
	
    TH1F *ht=new TH1F("ht","Theta distribution",200,0.,180);
	TH1F *h1= new TH1F("h1","Theta difference",200,-5.,5.);
	TH1F *h2= new TH1F("h2","Phi difference",200,-5.,5.);
	TH1F *h3= new TH1F("h3","Cluster energy",100,0.85,1.05);
	TH2F *h2theta= new TH2F("h2theta","Theta difference",200,0.,180.,200,-5.,5.);
	TH2F *h2phi= new TH2F("h2phi","Phi difference",200,0.,180.,200,-5.,5.);
	TH1F *hE1= new TH1F("hE1","E1",200,0.,1.05);
	TH1F *hE1E9= new TH1F("hE1E9","E1 / E9",200,0.,1.05);
	TH1F *hE9E25= new TH1F("hE9E25","E9 / E25",200,0.,1.05);
 
	// Cluster angular position
	// Entrance point is determined by minimal time
	
	// Calibrartor version 3 corresponds to non-uniformity of light collection switched on
	PndEmcAbsClusterCalibrator * calibrator1= PndEmcClusterCalibrator::MakeEmcClusterCalibrator(1, 3);
		
	// Cluster energy
	for (Int_t j=0; j< t->GetEntriesFast(); j++)
	{
		t->GetEntry(j);
		for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
		{
			PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
			cluster_energy=cluster->energy();
			cluster_energy_calibrated=calibrator1->Energy(cluster);
			if ((cluster->NumberOfDigis()>1)&&(cluster_energy>0.02))
			h3->Fill(cluster_energy_calibrated);
			PndEmcClusterEnergySums esum(*cluster, digi_array);
			hE1->Fill(esum.E1());
			hE1E9->Fill(esum.E1E9());
			hE9E25->Fill(esum.E9E25());
			TVector3 cluster_pos=cluster->where();
			ht->Fill(cluster_pos.Theta()*180./3.1415);
		}
	}

	for (Int_t j=0; j< t->GetEntriesFast(); j++)//t->GetEntriesFast()
	{
		t->GetEntry(j);
		tsim->GetEntry(j);
		
		PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0);
		photon_momentum=mctrack->GetMomentum();
		theta=photon_momentum.Theta();
		phi=photon_momentum.Phi();
		
	
		// Loop over clusters
		// If we have 1 initial particle and several cluster
		// we can separate cluster from the first interaction by maximum energy

		max_energy=0;
		
		for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
		{
			PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
			cluster_energy=cluster->energy();
			if (cluster_energy>max_energy)
			{
				max_energy=cluster_energy;
				TVector3 cluster_pos=cluster->where();
				cluster_theta=cluster_pos.Theta();
				cluster_phi=cluster_pos.Phi();
			}
						
		}
		if (max_energy>0.6)
		{
		    //cluster_theta-
		    theta_diff=(cluster_theta-theta)*180./TMath::Pi();
		    h1->Fill(theta_diff);
		    h2theta->Fill(theta*TMath::RadToDeg(),theta_diff);
		    //cluster_phi-
		    phi_diff=(cluster_phi-phi)*180./TMath::Pi();
		    h2->Fill(phi_diff);
		    h2phi->Fill(theta*TMath::RadToDeg(),phi_diff);
		}   
	}
	
Bool_t fTest=kTRUE;

Double_t thetaCheckMean=h1->GetMean();
Double_t thetaCheckRMS=h1->GetRMS();

if (TMath::Abs(thetaCheckMean)<0.1 && thetaCheckRMS<0.7 && thetaCheckRMS>0.1)
{
    cout<<"\n Theta Diff - ACCEPTABLE "<<endl;
}
else
{
    cout<<" \n Theta Diff - SOMETHING WENT WRONG "<<endl;
    cout<<"    Theta mean = " << thetaCheckMean << endl;
    cout<<"    Theta RMS  = " << thetaCheckRMS << endl;
    fTest=kFALSE;
}

Double_t phiCheckMean=h2->GetMean();
Double_t phiCheckRMS=h2->GetRMS();

if (TMath::Abs(phiCheckMean)<0.1 && phiCheckRMS<1.0 && phiCheckRMS>0.2)
{
    cout<<"\n Phi Diff - ACCEPTABLE "<<endl;
}
else
{
    cout<<" \n Phi Diff - SOMETHING WENT WRONG "<<endl;
    cout<<"    Phi mean = " << phiCheckMean << endl;
    cout<<"    Phi RMS  = " << phiCheckRMS << endl;
    fTest=kFALSE;
}

Double_t energyCheckMean=h3->GetMean();
Double_t energyCheckRMS=h3->GetRMS();

if (energyCheckMean<1.02 && TMath::Abs(1.0-energyCheckMean)<0.06 && energyCheckRMS<0.04 && energyCheckRMS>0.01)
{
    cout<<"\n Cluster Energy - ACCEPTABLE "<<endl;
}
else
{
    cout<<" \n Cluster Energy - SOMETHING WENT WRONG "<<endl;
    cout<<"    Energy mean = " << energyCheckMean << endl;
    cout<<"    Energy RMS  = " << energyCheckRMS << endl;
    fTest=kFALSE;
}

 if (ht->GetBinContent(15)==0)
   {
     fTest = kFALSE;
     cout << "\n FW endcap absent - BAD" << endl;
   }

if (fTest){
    cout << " Test passed" << endl;
    cout << " All ok " << endl;  
}else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
}
exit(0);
}

