// Macro loads 4 files after simulation-reconstruction for 4 emc components
// barrel, forward endcap, backward endcap, shashlyk
// Errors are estimated for (energy,phi,z) for barrel and (energy, x, y) for other components
// Dependence of parameters error (standard deviation) as a function of energy is fitted with 2 functions (one for energy and another for position parameters).
// Parameters of this fitting for all four EMC components are stored in PndEmcErrorMatrixParObject in output file  

int error_matrix_fit(Int_t emcGeometry=17, TString fileVersion="1")
{
	gROOT->SetStyle("Plain");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	rootlogon();
	basiclibs();
	
	TH2F *h2phi= new TH2F("h2phi","Phi difference",70,0.,7.,100,-2.,2.);
	TH2F *h2x= new TH2F("h2x","X difference",70,0.,7.,100,-2.,2.);
	TH2F *h2y= new TH2F("h2y","Y difference",70,0.,7.,100,-2.,2.);
	TH2F *h2z= new TH2F("h2z","Z difference",70,0.,7.,100,-2.,2.);
	TH2F *h2energy= new TH2F("h2energy","Energy difference",70,0.,7.,100,-0.2,0.2);	

	TH1F *h1energy_1= new TH1F("h1energy_1","Cluster energy",70,0.,7.);
	TH1F *h1energy_2= new TH1F("h1energy_2","Cluster energy",70,0.,7.);
	TH1F *h1energy_3= new TH1F("h1energy_3","Cluster energy",70,0.,7.);
	TH1F *h1energy_4= new TH1F("h1energy_4","Cluster energy",70,0.,7.);
	
	// Barrel
	TH1F *h1phi= new TH1F("hphi","Phi difference",70,0.,7.);
	TH1F *h1z= new TH1F("hz","Z difference",70,0.,7.);
	
	// Fwd endcap, bwd endcap, shashlyk
	TH1F *h1x_2= new TH1F("hx_2","X difference",70,0.,7.);
	TH1F *h1y_2= new TH1F("hy_2","Y difference",70,0.,7.);
	TH1F *h1x_3= new TH1F("hx_3","X difference",70,0.,7.);
	TH1F *h1y_3= new TH1F("hy_3","Y difference",70,0.,7.);
	TH1F *h1x_4= new TH1F("hx_4","X difference",70,0.,7.);
	TH1F *h1y_4= new TH1F("hy_4","Y difference",70,0.,7.);

	
	TVector3 photon_momentum;
	double cluster_energy, energy;
	double cluster_theta, cluster_phi, cluster_x, cluster_y, cluster_z; //position of the cluster
	double theta, phi, x, y, z; // position of the initial particle
	double x_diff, y_diff, z_diff, phi_diff, e_diff;
	double max_energy=0;
	Int_t emc_component;
	double sigma_e, sigma_x, sigma_y, sigma_z, sigma_phi, sigma_e_e;
	double sigma_e_error, sigma_x_error, sigma_y_error, sigma_z_error, sigma_phi_error, sigma_e_e_error;
	TH1D *h_phi, *h_x, *h_y, *h_z, *h_energy; 

	// Flags set if data files for EMC component is present
	Bool_t barrel_flag=false, fwcap_flag=false, bwcap_flag=false, fsc_flag=false;
	// Fit parametrs
	Double_t pars1[10]={0.}, pars2[10]={0.}, pars3[10]={0.}, pars4[10]={0.};
	
	/////////////////////////////////////////////////////////////////////////
	//////////////////////// Barrel /////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	emc_component=1;
	TString outFile="emc_data_";
	outFile+=emcGeometry;
	outFile+="_";
	outFile+=emc_component;
	outFile+=".root";

	TFile* file1 = new TFile(outFile); //file you want to analyse
	if (!file1->IsZombie())
	{
		barrel_flag=true;
		h2phi->Reset();h2x->Reset();h2y->Reset();h2z->Reset();h2energy->Reset();
		TTree *t=(TTree *) file1->Get("cbmsim") ;
		TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
		t->SetBranchAddress("EmcCluster",&cluster_array);
		TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
		t->SetBranchAddress("MCTrack",&mctrack_array);
		
			// Cluster energy
		for (Int_t j=0; j< t->GetEntriesFast(); j++)
		{
			t->GetEntry(j);
			
			PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0);
			photon_momentum=mctrack->GetMomentum();
			energy=photon_momentum.Mag();
			theta=photon_momentum.Theta();
			phi=photon_momentum.Phi();
			
			if (emc_component==1)
			{
				// Z is calculated at R=100 cm
				z=100*cos(theta);
			} else
			{
				// X and Y are calculated at Z=100 cm
				x=100*sin(theta)*cos(phi);
				y=100*sin(theta)*sin(phi);
			}
			
			max_energy=0;
			
			for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
			{
				PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
				cluster_energy=cluster->energy();
				if (cluster_energy>max_energy)
				{
					max_energy=cluster_energy;
					TVector3 cluster_pos=cluster->where();
					cluster_phi=cluster_pos.Phi();
					cluster_theta=cluster_pos.Theta();
					if (emc_component==1)
					{
						cluster_z=100*cos(cluster_theta);
					} else {
						cluster_x=100*sin(cluster_theta)*cos(cluster_phi);
						cluster_y=100*sin(cluster_theta)*sin(cluster_phi);
					}
				}
			}
			
			e_diff=energy-max_energy;
			h2energy->Fill(energy,e_diff);
			
			if (emc_component==1)
			{
				phi_diff=(cluster_phi-phi)*180./TMath::Pi();
				h2phi->Fill(energy,phi_diff);
				z_diff=cluster_z-z;
				h2z->Fill(energy,z_diff);
			} else {
				x_diff=cluster_x-x;
				h2x->Fill(energy,x_diff);
				y_diff=cluster_y-y;
				h2y->Fill(energy,y_diff);
			} 
		}
		
		for (int i=0; i<h2energy->GetNbinsX();i++)
		{
			h_energy = h2energy->ProjectionY("hp_energy_1",i,i);
			sigma_e=h_energy->GetRMS();
			sigma_e_error=h_energy->GetRMSError();
			energy=h2energy->GetXaxis()->GetBinCenter(i);
			if (energy!=0) {
				sigma_e_e=sigma_e/energy;
				sigma_e_e_error=sigma_e_error/energy;
			}
			else {
				sigma_e_e=0;
				sigma_e_e_error=0;
			}
			h1energy_1->SetBinContent(i,sigma_e_e);
			h1energy_1->SetBinError(i,sigma_e_e_error);
		}
		
		for (int i=0; i<h2phi->GetNbinsX();i++)
		{
			h_phi = h2phi->ProjectionY("hp_phi",i,i);
			sigma_phi=h_phi->GetRMS()*TMath::Pi()/180.;
			sigma_phi_error=h_phi->GetRMSError()*TMath::Pi()/180.;
			h1phi->SetBinContent(i,sigma_phi);
			h1phi->SetBinError(i,sigma_phi_error);
		}
		for (int i=0; i<h2z->GetNbinsX();i++)
		{
			h_z = h2z->ProjectionY("hp_z",i,i);
			sigma_z=h_z->GetRMS();
			sigma_z_error=h_z->GetRMSError();
			h1z->SetBinContent(i,sigma_z);
			h1z->SetBinError(i,sigma_z_error);
		}
		std::cout<<"emc_component "<<emc_component<<" done."<<std::endl;
	} else {
		std::cout<<"EMC data file for component "<<emc_component<<" for gemetry "<<emcGeometry<<" does not exist"<<std::endl;
	}	
	
	/////////////////////////////////////////////////////////////////////////
	//////////////////////// Fwd endcap /////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	emc_component=2;
	TString outFile="emc_data_";
	outFile+=emcGeometry;
	outFile+="_";
	outFile+=emc_component;
	outFile+=".root";

	TFile* file2 = new TFile(outFile); //file you want to analyse
	if (!file2->IsZombie())
	{
		fwcap_flag=true;
		h2phi->Reset();h2x->Reset();h2y->Reset();h2z->Reset();h2energy->Reset();
		TTree *t=(TTree *) file2->Get("cbmsim") ;
		TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
		t->SetBranchAddress("EmcCluster",&cluster_array);
		TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
		t->SetBranchAddress("MCTrack",&mctrack_array);
		
			// Cluster energy
		for (Int_t j=0; j< t->GetEntriesFast(); j++)
		{
			t->GetEntry(j);
			
			PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0);
			photon_momentum=mctrack->GetMomentum();
			energy=photon_momentum.Mag();
			theta=photon_momentum.Theta();
			phi=photon_momentum.Phi();
			
			if (emc_component==1)
			{
				// Z is calculated at R=100 cm
				z=100*cos(theta);
			} else
			{
				// X and Y are calculated at Z=100 cm
				x=100*sin(theta)*cos(phi);
				y=100*sin(theta)*sin(phi);
			}
			
			max_energy=0;
			
			for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
			{
				PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
				cluster_energy=cluster->energy();
				if (cluster_energy>max_energy)
				{
					max_energy=cluster_energy;
					TVector3 cluster_pos=cluster->where();
					cluster_phi=cluster_pos.Phi();
					cluster_theta=cluster_pos.Theta();
					if (emc_component==1)
					{
						cluster_z=100*cos(cluster_theta);
					} else {
						cluster_x=100*sin(cluster_theta)*cos(cluster_phi);
						cluster_y=100*sin(cluster_theta)*sin(cluster_phi);
					}
				}
			}
			
			e_diff=energy-max_energy;
			h2energy->Fill(energy,e_diff);
			
			if (emc_component==1)
			{
				phi_diff=(cluster_phi-phi)*180./TMath::Pi();
				h2phi->Fill(energy,phi_diff);
				z_diff=cluster_z-z;
				h2z->Fill(energy,z_diff);
			} else {
				x_diff=cluster_x-x;
				h2x->Fill(energy,x_diff);
				y_diff=cluster_y-y;
				h2y->Fill(energy,y_diff);
			} 
		}
		
	
		for (int i=0; i<h2energy->GetNbinsX();i++)
		{
			h_energy = h2energy->ProjectionY("hp_energy_2",i,i);
			sigma_e=h_energy->GetRMS();
			sigma_e_error=h_energy->GetRMSError();
			energy=h2energy->GetXaxis()->GetBinCenter(i);
			if (energy!=0) {
				sigma_e_e=sigma_e/energy;
				sigma_e_e_error=sigma_e_error/energy;
			}
			else {
				sigma_e_e=0;
				sigma_e_e_error=0;
			}
			h1energy_2->SetBinContent(i,sigma_e_e);
			h1energy_2->SetBinError(i,sigma_e_e_error);
		}
		
		for (int i=0; i<h2x->GetNbinsX();i++)
		{
			h_x = h2x->ProjectionY("hp_x_2",i,i);
			sigma_x=h_x->GetRMS();
			sigma_x_error=h_x->GetRMSError();
			h1x_2->SetBinContent(i,sigma_x);
			h1x_2->SetBinError(i,sigma_x_error);
		}
		for (int i=0; i<h2y->GetNbinsX();i++)
		{
			h_y = h2y->ProjectionY("hp_y_2",i,i);
			sigma_y=h_y->GetRMS();
			sigma_y_error=h_y->GetRMSError();
			h1y_2->SetBinContent(i,sigma_y);
			h1y_2->SetBinError(i,sigma_y_error);
		}
		
		std::cout<<"emc_component "<<emc_component<<" done."<<std::endl;
	} else {
		std::cout<<"EMC data file for component "<<emc_component<<" for gemetry "<<emcGeometry<<" does not exist"<<std::endl;
	}	
	
	/////////////////////////////////////////////////////////////////////////
	//////////////////////// Bwd endcap /////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	emc_component=3;
	TString outFile="emc_data_";
	outFile+=emcGeometry;
	outFile+="_";
	outFile+=emc_component;
	outFile+=".root";

	TFile* file3 = new TFile(outFile); //file you want to analyse
	if (!file3->IsZombie())
	{
		bwcap_flag=true;
		h2phi->Reset();h2x->Reset();h2y->Reset();h2z->Reset();h2energy->Reset();
		TTree *t=(TTree *) file3->Get("cbmsim") ;
		TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
		t->SetBranchAddress("EmcCluster",&cluster_array);
		TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
		t->SetBranchAddress("MCTrack",&mctrack_array);
		
			// Cluster energy
		for (Int_t j=0; j< t->GetEntriesFast(); j++)
		{
			t->GetEntry(j);
			
			PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0);
			photon_momentum=mctrack->GetMomentum();
			energy=photon_momentum.Mag();
			theta=photon_momentum.Theta();
			phi=photon_momentum.Phi();
			
			if (emc_component==1)
			{
				// Z is calculated at R=100 cm
				z=100*cos(theta);
			} else
			{
				// X and Y are calculated at Z=100 cm
				x=100*sin(theta)*cos(phi);
				y=100*sin(theta)*sin(phi);
			}
			
			max_energy=0;
			
			for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
			{
				PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
				cluster_energy=cluster->energy();
				if (cluster_energy>max_energy)
				{
					max_energy=cluster_energy;
					TVector3 cluster_pos=cluster->where();
					cluster_phi=cluster_pos.Phi();
					cluster_theta=cluster_pos.Theta();
					if (emc_component==1)
					{
						cluster_z=100*cos(cluster_theta);
					} else {
						cluster_x=100*sin(cluster_theta)*cos(cluster_phi);
						cluster_y=100*sin(cluster_theta)*sin(cluster_phi);
					}
				}
			}
			
			e_diff=energy-max_energy;
			h2energy->Fill(energy,e_diff);
			
			if (emc_component==1)
			{
				phi_diff=(cluster_phi-phi)*180./TMath::Pi();
				h2phi->Fill(energy,phi_diff);
				z_diff=cluster_z-z;
				h2z->Fill(energy,z_diff);
			} else {
				x_diff=cluster_x-x;
				h2x->Fill(energy,x_diff);
				y_diff=cluster_y-y;
				h2y->Fill(energy,y_diff);
			} 
		}
		
	
		for (int i=0; i<h2energy->GetNbinsX();i++)
		{
			h_energy = h2energy->ProjectionY("hp_energy_3",i,i);
			sigma_e=h_energy->GetRMS();
			sigma_e_error=h_energy->GetRMSError();
			energy=h2energy->GetXaxis()->GetBinCenter(i);
			if (energy!=0) {
				sigma_e_e=sigma_e/energy;
				sigma_e_e_error=sigma_e_error/energy;
			}
			else {
				sigma_e_e=0;
				sigma_e_e_error=0;
			}
			h1energy_3->SetBinContent(i,sigma_e_e);
			h1energy_3->SetBinError(i,sigma_e_e_error);
		}
		
		for (int i=0; i<h2x->GetNbinsX();i++)
		{
			h_x = h2x->ProjectionY("hp_x_3",i,i);
			sigma_x=h_x->GetRMS();
			sigma_x_error=h_x->GetRMSError();
			h1x_3->SetBinContent(i,sigma_x);
			h1x_3->SetBinError(i,sigma_x_error);
		}
		for (int i=0; i<h2y->GetNbinsX();i++)
		{
			h_y = h2y->ProjectionY("hp_y_3",i,i);
			sigma_y=h_y->GetRMS();
			sigma_y_error=h_y->GetRMSError();
			h1y_3->SetBinContent(i,sigma_y);
			h1y_3->SetBinError(i,sigma_y_error);
		}
		std::cout<<"emc_component "<<emc_component<<" done."<<std::endl;
	} else {
		std::cout<<"EMC data file for component "<<emc_component<<" for gemetry "<<emcGeometry<<" does not exist"<<std::endl;
	}	
	
	/////////////////////////////////////////////////////////////////////////
	//////////////////////// Shashlyk ///////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	emc_component=4;
	TString outFile="emc_data_";
	outFile+=emcGeometry;
	outFile+="_";
	outFile+=emc_component;
	outFile+=".root";

	TFile* file4 = new TFile(outFile); //file you want to analyse
	if (!file4->IsZombie())
	{
		fsc_flag=true;
		h2phi->Reset();h2x->Reset();h2y->Reset();h2z->Reset();h2energy->Reset();
		TTree *t=(TTree *) file4->Get("cbmsim") ;
		TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
		t->SetBranchAddress("EmcCluster",&cluster_array);
		TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
		t->SetBranchAddress("MCTrack",&mctrack_array);
		
			// Cluster energy
		for (Int_t j=0; j< t->GetEntriesFast(); j++)
		{
			t->GetEntry(j);
			
			PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0);
			photon_momentum=mctrack->GetMomentum();
			energy=photon_momentum.Mag();
			theta=photon_momentum.Theta();
			phi=photon_momentum.Phi();
			
			if (emc_component==1)
			{
				// Z is calculated at R=100 cm
				z=100*cos(theta);
			} else
			{
				// X and Y are calculated at Z=100 cm
				x=100*sin(theta)*cos(phi);
				y=100*sin(theta)*sin(phi);
			}
			
			max_energy=0;
			
			for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
			{
				PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
				cluster_energy=cluster->energy();
				if (cluster_energy>max_energy)
				{
					max_energy=cluster_energy;
					TVector3 cluster_pos=cluster->where();
					cluster_phi=cluster_pos.Phi();
					cluster_theta=cluster_pos.Theta();
					if (emc_component==1)
					{
						cluster_z=100*cos(cluster_theta);
					} else {
						cluster_x=100*sin(cluster_theta)*cos(cluster_phi);
						cluster_y=100*sin(cluster_theta)*sin(cluster_phi);
					}
				}
			}
			
			e_diff=energy-max_energy;
			h2energy->Fill(energy,e_diff);
			
			if (emc_component==1)
			{
				phi_diff=(cluster_phi-phi)*180./TMath::Pi();
				h2phi->Fill(energy,phi_diff);
				z_diff=cluster_z-z;
				h2z->Fill(energy,z_diff);
			} else {
				x_diff=cluster_x-x;
				h2x->Fill(energy,x_diff);
				y_diff=cluster_y-y;
				h2y->Fill(energy,y_diff);
			} 
		}
		
	
		for (int i=0; i<h2energy->GetNbinsX();i++)
		{
			h_energy = h2energy->ProjectionY("hp_energy_4",i,i);
			sigma_e=h_energy->GetRMS();
			sigma_e_error=h_energy->GetRMSError();
			energy=h2energy->GetXaxis()->GetBinCenter(i);
			if (energy!=0) {
				sigma_e_e=sigma_e/energy;
				sigma_e_e_error=sigma_e_error/energy;
			}
			else {
				sigma_e_e=0;
				sigma_e_e_error=0;
			}
			h1energy_4->SetBinContent(i,sigma_e_e);
			h1energy_4->SetBinError(i,sigma_e_e_error);
		}
		
		for (int i=0; i<h2x->GetNbinsX();i++)
		{
			h_x = h2x->ProjectionY("hp_x_4",i,i);
			sigma_x=h_x->GetRMS();
			sigma_x_error=h_x->GetRMSError();
			h1x_4->SetBinContent(i,sigma_x);
			h1x_4->SetBinError(i,sigma_x_error);
		}
		for (int i=0; i<h2y->GetNbinsX();i++)
		{
			h_y = h2y->ProjectionY("hp_y_4",i,i);
			sigma_y=h_y->GetRMS();
			sigma_y_error=h_y->GetRMSError();
			h1y_4->SetBinContent(i,sigma_y);
			h1y_4->SetBinError(i,sigma_y_error);
		}
		std::cout<<"emc_component "<<emc_component<<" done."<<std::endl;
	} else {
		pars4[0]=0.202292;
		pars4[1]=0.717711;
		pars4[2]=0.083642;
		pars4[3]=0.000000;
		pars4[4]=0.333771;
		pars4[5]=0.715637;
		pars4[6]=0.341344;
		pars4[7]=0.393149;
		pars4[8]=0.397055;
		pars4[9]=0.321306;
		std::cout<<"EMC data file for component "<<emc_component<<" for gemetry "<<emcGeometry<<" does not exist"<<std::endl;
	}	
	
	/////////////////////////////////////////////////////////////////
	/////// Plot results and fit ////////////////////////////////////
	/////////////////////////////////////////////////////////////////

	
	//////////////////////////// Barrel ////////////////////////////////////////
	if (barrel_flag){
	
		TCanvas* c1 = new TCanvas("c1", "Energy-position resolution vs Energy", 100, 100, 800, 800);
		c1->Divide(2,2);
		c1->cd(1);
		h1energy_1->Draw("E1");
		
		TF1 *f1 = new TF1("f1", fit1, 0., 7.,4);
		f1->SetLineColor(2);
		h1energy_1->Fit("f1");
		f1->GetParameters(pars1);
		
		c1->cd(2);
		h1z->Draw("E1");
		TF1 *f2 = new TF1("f2", fit2, 0., 7.,3);
		f2->SetLineColor(2);
		h1z->Fit("f2");
		f2->GetParameters(pars1+4);
		
		c1->cd(3);
		h1phi->Draw("E1");
		TF1 *f3 = new TF1("f3", fit2, 0., 7.,3);
		f3->SetLineColor(2);
		h1phi->Fit("f3");
		f3->GetParameters(pars1+7);
	}
		
	//////////////////////////// Fwd endcap ////////////////////////////////////////
	if (fwcap_flag)
	{
		TCanvas* c2 = new TCanvas("c2", "Energy-position resolution vs Energy", 100, 100, 800, 800);
		c2->Divide(2,2);
		c2->cd(1);
		h1energy_2->Draw("E1");
		
		TF1 *f4 = new TF1("f4", fit1, 0., 7.,4);
		f4->SetLineColor(2);
		h1energy_2->Fit("f4");
		f4->GetParameters(pars2);
		
		c2->cd(2);
		h1x_2->Draw("E1");
		TF1 *f5 = new TF1("f5", fit2, 0., 7.,3);
		f5->SetLineColor(2);
		h1x_2->Fit("f5");
		f5->GetParameters(pars2+4);
		
		c2->cd(3);
		h1y_2->Draw("E1");
		TF1 *f6 = new TF1("f6", fit2, 0., 7.,3);
		f6->SetLineColor(2);
		h1y_2->Fit("f6");
		f6->GetParameters(pars2+7);
	}	
	
	//////////////////////////// Bwd endcap ////////////////////////////////////////
	if (bwcap_flag)
	{
		TCanvas* c3 = new TCanvas("c3", "Energy-position resolution vs Energy", 100, 100, 800, 800);
		c3->Divide(2,2);
		c3->cd(1);
		h1energy_3->Draw("E1");
		
		TF1 *f7 = new TF1("f7", fit1, 0., 7.,4);
		f7->SetLineColor(2);
		h1energy_3->Fit("f7");
		f7->GetParameters(pars3);
		
		c3->cd(2);
		h1x_3->Draw("E1");
		TF1 *f8 = new TF1("f8", fit2, 0., 7.,3);
		f8->SetLineColor(2);
		h1x_3->Fit("f8");
		f8->GetParameters(pars3+4);
		
		c3->cd(3);
		h1y_3->Draw("E1");
		TF1 *f9 = new TF1("f9", fit2, 0., 7.,3);
		f9->SetLineColor(2);
		h1y_3->Fit("f9");
		f9->GetParameters(pars3+7);
	}	
	
	//////////////////////////// Shashlyk ////////////////////////////////////////
	if (fsc_flag)
	{
		TCanvas* c4 = new TCanvas("c4", "Energy-position resolution vs Energy", 100, 100, 800, 800);
		c4->Divide(2,2);
		c4->cd(1);
		h1energy_4->Draw("E1");
		
		TF1 *f10 = new TF1("f10", fit1, 0., 7.,4);
		f10->SetLineColor(2);
		h1energy_4->Fit("f10");
		f10->GetParameters(pars4);
		
		c4->cd(2);
		h1x_4->Draw("E1");
		TF1 *f11 = new TF1("f11", fit2, 0., 7.,3);
		f11->SetLineColor(2);
		h1x_4->Fit("f11");
		f11->GetParameters(pars4+4);
		
		c4->cd(3);
		h1y_4->Draw("E1");
		TF1 *f12 = new TF1("f12", fit2, 0., 7.,3);
		f12->SetLineColor(2);
		h1y_4->Fit("f12");
		f12->GetParameters(pars4+7);
	}	
	
	////////////// Store parametrs to file ////////////////////////	
	TString fileName="emc_error_matrix_";
	fileName+=fileVersion;
	fileName+=".root";
	TFile *errorfile = new TFile(fileName, "RECREATE");
	
	PndEmcErrorMatrixParObject *parObject=new PndEmcErrorMatrixParObject();
	enum {barrel, fwcap, bwcap, fsc};

	parObject->SetErrorMatrix(barrel, pars1);
	parObject->SetErrorMatrix(fwcap, pars2);
	parObject->SetErrorMatrix(bwcap, pars3);
	parObject->SetErrorMatrix(fsc, pars4);
	
	parObject->Write();
	errorfile->Close();

  return 0;
}

Double_t fit1(Double_t *x, Double_t *par)
{
	double xx=x[0];
	
	double a=par[0];
	double power=par[1];
	double c=par[2];
	double q=par[3];
				
	double res;
	
	res=a*a/pow(xx,power)+c*c+q*q/(xx*xx);
	
	return res;
}

Double_t fit2(Double_t *x, Double_t *par)
{
	double xx=x[0];
	
	double a=par[0];
	double power=par[1];
	double c=par[2];
				
	double res;
	
	res=a*a/pow(xx,power)+c*c;
	
	return res;
}
