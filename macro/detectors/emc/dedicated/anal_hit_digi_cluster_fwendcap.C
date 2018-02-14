{
  // Macro loads a file after reconstruction and plots difference between initial direction of particle and angular position of cluster
  //gROOT->Macro("style.C");
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  //TFile* fsim = new TFile("data/fixed/sim_emc_g3_m3_1000eve_fixed.root"); // (GEANT 3) 1.07.09
  TFile* fsim = new TFile("data/fixed/sim_emc_g4_m3_1000eve_fixed.root"); // (GEANT 4) 1.07.09

  TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
  TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack",&mctrack_array);
  PndEmcMapper *emcMap=PndEmcMapper::Instance(1);//,tsim);


  //TFile* fful = new TFile("data/fixed/full_emc_g3_m3_1000eve_fixed.root"); // (GEANT 3) 1.07.09
  TFile* fful = new TFile("data/fixed/full_emc_g4_m3_1000eve_fixed.root"); // (GEANT 4) 1.07.09
  
  TTree *tful =(TTree *) fful->Get("cbmsim") ;

  TClonesArray* hit_array=new TClonesArray("PndEmcHit");		
  tful->SetBranchAddress("EmcHit",&hit_array);

  TClonesArray* digi_array=new TClonesArray("PndEmcDigi");		
  tful->SetBranchAddress("EmcDigi",&digi_array);

  TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");		
  tful->SetBranchAddress("EmcCluster",&cluster_array);


  Double_t theta_mc, phi_mc, ene_mc, cemc, 
    cluster_energy, cluster_theta, cluster_phi;
  Double_t hit_ene, hit_theta, hit_phi,
    x_h, y_h, z_h, module_h, crystal_h, row_h, copy_h,
    crystal_hid,row_hid,
    hit_id, hit_x, hit_y,
    crystal_cid,row_cid,crystal_c, row_c, copy_c;
  
  Double_t digi_ene, digi_theta, digi_phi,
    module_d, crystal_d, row_d, copy_d,
    crystal_did,row_did,d_id;

  Int_t id_h,id_d;

  TVector3 photon_momentum;
  TLorentzVector p4mom;

  int ndigi, nhit;
  double max_energy1=0;
  
  //----- x, y, z, id
  TH2F *hrow_crystal_h = new TH2F("row_crystal","row vs crystal (HIT)",72,-36,36,72,-36,36); 
  TH2F *hrow_crystal_d = new TH2F("row_crystal_digi","row vs crystal (DIGI)",72,-36,36,72,-36,36); 
  TH2F *hrow_crystal_c = new TH2F("row_crystal_c","row vs crystal (CLUSTER)",72,-36,36,72,-36,36); 

  TH2F *hx_y_h = new TH2F("x_y_h","x vs y (HIT)",200,-100,100,200,-100,100); 
  TH2F *hx_y_d = new TH2F("x_y_d","GetThetaInt() vs GetPhiInt() (DIGI)",200,200,300,200,200,300);

  TH1F *hid_h = new TH1F("id_h","id (HIT)",140000000,300000000,340000000);
  TH1F *hz_h = new TH1F("z_h","z (HIT)",300,1,300);
  TH1F *hmodule_h = new TH1F("module_h","Module (HIT)",5,1,6);

  TH2F *hxpad_x_h = new TH2F("xpad_x_h","xpad vs x (HIT)",200,-100,100,200,-100,100); 
  TH2F *hypad_y_h = new TH2F("ypad_y_h","ypad vs y (HIT)",200,-100,100,200,-100,100); 

  TH2F *hxpad_x_d = new TH2F("xpad_x_d","xpad vs GetThetaInt() (DIGI)",200,-100,100,200,200,300); 
  TH2F *hypad_y_d = new TH2F("ypad_y_d","ypad vs GetPhiInt() (DIGI)",200,-100,100,200,200,300); 

  TH2F *hx_row_h = new TH2F("x_row_h","x vs row (HIT)",200,-100,100,72,-36,36); 
  TH2F *hx_row_d = new TH2F("x_row_d","GetThetaInt() vs row (DIGI)",200,200,300,72,-36,36); 

  TH2F *hcp_z_h = new TH2F("cp_z_h","Copy vs.Z pos (HITS)",4,1,5,80,200,240);

  TH1F *hz_cp1_h = new TH1F("z_cp1_h","z (HIT)",300,1,300);
  TH1F *hz_cp2_h = new TH1F("z_cp2_h","z (HIT)",300,1,300);
  TH1F *hz_cp3_h = new TH1F("z_cp3_h","z (HIT)",300,1,300);
  TH1F *hz_cp4_h = new TH1F("z_cp4_h","z (HIT)",300,1,300);
 
  TH1F *hz_d = new TH1F("z_d","z (DIGI)",300,1,300);
  TH1F *hz_cp1_d = new TH1F("z_cp1_d","z (DIGI)",300,1,300);
  TH1F *hz_cp2_d = new TH1F("z_cp2_d","z (DIGI)",300,1,300);
  TH1F *hz_cp3_d = new TH1F("z_cp3_d","z (DIGI)",300,1,300);
  TH1F *hz_cp4_d = new TH1F("z_cp4_d","z (DIGI)",300,1,300);

  TH1F *hz_clust = new TH1F("z_cluster","z (CLUSTER)",300,1,300);
  TH1F *hz_cp1_c = new TH1F("z_cp1_c","z (CLUSTER)",300,1,300);
  TH1F *hz_cp2_c = new TH1F("z_cp2_c","z (CLUSTER)",300,1,300);
  TH1F *hz_cp3_c = new TH1F("z_cp3_c","z (CLUSTER)",300,1,300);
  TH1F *hz_cp4_c = new TH1F("z_cp4_c","z (CLUSTER)",300,1,300);

  //--------- Energy: MC, HITs, DIGITs, CLUSTERs---------------------------------------------------
  TH1F *hene_mc= new TH1F("hene_mc","MC energy HIT (GeV)",100,0.05,1.05); 
  TH1F *dene_mc= new TH1F("dene_mc","MC energy DIGI (GeV)",100,0.05,1.05); 
  TH1F *hclus_ene_sum_mc = new TH1F("hclus_ene_sum_mc","SUM of MC Cluster energy (GeV)",100,0.05,1.05);

  TH1F *hene_h= new TH1F("hene_h","DATA energy HIT (GeV)",100,0.05,1.05); 
  TH1F *hene_d= new TH1F("hene_d","DATA energy DIGI (GeV)",100,0.05,1.05); 

  TH1F *hhit_ene= new TH1F("hhit_ene","SUM of Hits energy (GeV)",100,0.05,1.05);  
  TH1F *hdigi_ene= new TH1F("hdigi_ene","SUM of Digi energy (GeV)",100,0.05,1.05); 
  TH1F *hclus_ene= new TH1F("hclus_ene","SUM of Cluster energy (GeV)",100,0.05,1.05); 

  TH1F *hclus_ene_mc= new TH1F("hclus_ene_mc","SUM of MC Cluster energy (GeV)",100,0.05,1.05);
  TH1F *hclus_ene_sum = new TH1F("hclus_ene_sum","SUM of Cluster energy (GeV)",100,0.05,1.05); 

  TH1F *hhit_ene_cp1= new TH1F("hhit_ene_cp1","SUM of Hits energy (GeV) _cp1",100,0.05,1.05);  
  TH1F *hhit_ene_cp2= new TH1F("hhit_ene_cp2","SUM of Hits energy (GeV) _cp2",100,0.05,1.05);  
  TH1F *hhit_ene_cp3= new TH1F("hhit_ene_cp3","SUM of Hits energy (GeV) _cp3",100,0.05,1.05);  
  TH1F *hhit_ene_cp4= new TH1F("hhit_ene_cp4","SUM of Hits energy (GeV) _cp4",100,0.05,1.05);  

  TH1F *hclus_ene_cp1= new TH1F("hclus_ene_cp1","Cluster energy (GeV) _cp1",100,0.05,1.05); 
  TH1F *hclus_ene_cp2= new TH1F("hclus_ene_cp2","Cluster energy (GeV) _cp2",100,0.05,1.05); 
  TH1F *hclus_ene_cp3= new TH1F("hclus_ene_cp3","Cluster energy (GeV) _cp3",100,0.05,1.05); 
  TH1F *hclus_ene_cp4= new TH1F("hclus_ene_cp4","Cluster energy (GeV) _cp4",100,0.05,1.05); 

  //--------- Theta & Phi (reconstructed & truth): HITs, DIGIs, CLUSTERs -------------------------
  TH1F *h10= new TH1F("h10","HIT Theta (MC)",180,0.,180.);
  TH1F *h20= new TH1F("h20","HIT Phi (MC)",360,-180.,180.);
  TH1F *h10d= new TH1F("h10d","HIT Theta (data)",180,0.,180.);
  TH1F *h20d= new TH1F("h20d","HIT Phi (data)",360,-180.,180.);
  TH1F *h1= new TH1F("h1","HIT Theta difference",100,-5.,5.);
  TH1F *h2= new TH1F("h2","HIT Phi difference",100,-5.,5.);

  TH1F *h10digi= new TH1F("h10digi","DIGI Theta (MC)",180,0.,180.);
  TH1F *h20digi= new TH1F("h20digi","DIGI Phi (MC)",360,-180.,180.);
  TH1F *h10ddigi= new TH1F("h10ddigi","DIGI Theta (data)",180,0.,180.);
  TH1F *h20ddigi= new TH1F("h20ddigi","DIGI Phi (data)",360,-180.,180.);
  TH1F *h1d= new TH1F("h1d","DIGI Theta difference",100,-5.,5.);
  TH1F *h2d= new TH1F("h2d","DIGI Phi difference",100,-5.,5.);

  TH1F *hdigi_ene_cp1= new TH1F("hdigi_ene_cp1","SUM of Digi energy (GeV) _cp1",100,0.05,1.05);  
  TH1F *hdigi_ene_cp2= new TH1F("hdigi_ene_cp2","SUM of Digi energy (GeV) _cp2",100,0.05,1.05);  
  TH1F *hdigi_ene_cp3= new TH1F("hdigi_ene_cp3","SUM of Digi energy (GeV) _cp3",100,0.05,1.05);  
  TH1F *hdigi_ene_cp4= new TH1F("hdigi_ene_cp4","SUM of Digi energy (GeV) _cp4",100,0.05,1.05);  

  TH1F *hthc_mc= new TH1F("hthcmc","CLUSTER Theta (MC)",180,0.,180.);
  TH1F *hphc_mc= new TH1F("hphcmc","CLUSTER Phi (MC)",360,-180.,180.);
  TH1F *hthc   = new TH1F("hthc","CLUSTER Theta (data)",180,0.,180.);
  TH1F *hphc   = new TH1F("hphc","CLUSTER Phi (data)",360,-180.,180.);
  TH1F *hthc_diff= new TH1F("hthc_diff","CLUSTER Theta difference",100,-5.,5.);
  TH1F *hphc_diff= new TH1F("hphc_diff","CLUSTER Phi difference",100,-5.,5.);

  TH1F *hid_d = new TH1F("id_d","id (DIGI)",140000000,300000000,340000000);
  TH1F *hmodule_d = new TH1F("module_d","Module (DIGI)",5,1,6);

  TH1F *hClustNoOrig  = new TH1F("hClustNoOrig","hClustNoOrig",10,0,9);
  TH1F *hClustNo      = new TH1F("hClustNo","hClustNo",10,0,9);

  TH1F *hClNo_cp1     = new TH1F("hClNo_cp1","hClNo_cp1",10,0,9);
  TH1F *hClNo_cp2     = new TH1F("hClNo_cp2","hClNo_cp2",10,0,9);
  TH1F *hClNo_cp3     = new TH1F("hClNo_cp3","hClNo_cp3",10,0,9);
  TH1F *hClNo_cp4     = new TH1F("hClNo_cp4","hClNo_cp4",10,0,9);


   Int_t ncounts = tsim->GetEntries();
   for(int j = 0; j < ncounts; j++){
   //for (Int_t j=1;j< 3; j++){ 
     Float_t sum_hit_ene,hit_ene_cp1,hit_ene_cp2,hit_ene_cp3,hit_ene_cp4;
     Float_t sum_digi_ene,digi_ene_cp1,digi_ene_cp2,digi_ene_cp3,digi_ene_cp4;
     Float_t sum_clus_ene=0;
     Float_t cemc_sum=0;

     sum_hit_ene = hit_ene_cp1 = hit_ene_cp2 = hit_ene_cp3 = hit_ene_cp4 = 0;
     sum_digi_ene = digi_ene_cp1 = digi_ene_cp2 = digi_ene_cp3 = digi_ene_cp4 =0;

     tsim->GetEntry(j);
     tful->GetEntry(j);

	PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0); 
	photon_momentum=mctrack->GetMomentum();
	theta_mc=photon_momentum.Theta()*(180./TMath::Pi());	     
	phi_mc=photon_momentum.Phi()*(180./TMath::Pi());

	p4mom=mctrack->Get4Momentum();

	Int_t nhits = hit_array->GetEntries();
	for (Int_t i=0; i<nhits; i++)
	  {
	    PndEmcHit *hit=(PndEmcHit*)hit_array->At(i);	   

	    module_h = hit->GetModule();

	    if (module_h==3){

	      hene_mc->Fill(p4mom.E());

	      hit_ene=hit->GetEnergy();

	      hene_h->Fill(hit_ene); 

	      sum_hit_ene+=hit_ene;

	      crystal_h = hit->GetCrystal();
	      row_h = hit->GetRow();
	      copy_h = hit->GetCopy();

	      if (copy_h==1) hit_ene_cp1+=hit->GetEnergy();
	      if (copy_h==2) hit_ene_cp2+=hit->GetEnergy();
	      if (copy_h==3) hit_ene_cp3+=hit->GetEnergy();
	      if (copy_h==4) hit_ene_cp4+=hit->GetEnergy();

	      TVector3 hit_pos(hit->GetX(),hit->GetY(),hit->GetZ());
	      hit_theta=hit_pos.Theta()*(180./TMath::Pi());
	      hit_phi=hit_pos.Phi()*(180./TMath::Pi());
	    
	      //cout << "MC: theta= "<<theta_mc<<", phi= "<<phi_mc<<endl;
	      h10->Fill(theta_mc);
	      h20->Fill(phi_mc);
	    
	      h10d->Fill(hit_theta);
	      h20d->Fill(hit_phi);
	    
	      h1->Fill(hit_theta-theta_mc);
	      h2->Fill(hit_phi-phi_mc);	    
	     
	      if (copy_h==1) hz_cp1_h->Fill(hit->GetZ());
	      if (copy_h==2) hz_cp2_h->Fill(hit->GetZ());
	      if (copy_h==3) hz_cp3_h->Fill(hit->GetZ());
	      if (copy_h==4) hz_cp4_h->Fill(hit->GetZ());

	      hz_h->Fill(hit->GetZ());
	      hcp_z_h->Fill(copy_h,hit->GetZ());

	      hxpad_x_h->Fill(hit->GetXPad(),hit->GetX());
	      hypad_y_h->Fill(hit->GetYPad(),hit->GetY());
	      
	      hx_row_h->Fill(hit->GetRow(),hit->GetX());
	      
	      id_h = hit->GetDetectorID();
	    
	      crystal_hid = (id_h%10000);
	      row_hid = ((id_h/1000000)%100);

	      hmodule_h->Fill(module_h);
	      hrow_crystal_h->Fill(crystal_hid,row_hid);
	      hx_y_h->Fill(hit->GetX(),hit->GetY());
	     
	      hid_h->Fill(id_h);
	    }
	  }
	hhit_ene->Fill(sum_hit_ene);	
	hhit_ene_cp1->Fill(hit_ene_cp1);
	hhit_ene_cp2->Fill(hit_ene_cp2);
	hhit_ene_cp3->Fill(hit_ene_cp3);
	hhit_ene_cp4->Fill(hit_ene_cp4);

	// digits
	Int_t ndigits = digi_array->GetEntries();
	//cout << "DIGI array ==  " << ndigits<< endl;
	for (Int_t i=0; i<ndigits; i++)
	  {
	    PndEmcDigi *digi=(PndEmcDigi*)digi_array->At(i);	   

	    TVector3 digi_pos=digi->where();
	    
	    module_d = digi->GetModule();

	    if (module_d==3){

	      dene_mc->Fill(p4mom.E());	      

	      digi_ene=digi->GetEnergy();
	      hene_d->Fill(digi_ene); 

	      sum_digi_ene+=digi_ene;
	      
	      crystal_d = digi->GetCrystal();
	      row_d = digi->GetRow();
	      copy_d = digi->GetCopy();	 

	      Double_t digi_z=digi_pos.Z();
	      hz_d->Fill(digi_z);

	      if (copy_d==1){
		digi_ene_cp1+=digi->GetEnergy();
		hz_cp1_d->Fill(digi_z);
	      }
	      if (copy_d==2){
		digi_ene_cp2+=digi->GetEnergy();
		hz_cp2_d->Fill(digi_z);
	      }
	      if (copy_d==3){
		digi_ene_cp3+=digi->GetEnergy();
		hz_cp3_d->Fill(digi_z);
	      }
	      if (copy_d==4){
		digi_ene_cp4+=digi->GetEnergy();
		hz_cp4_d->Fill(digi_z);
	      }

	      digi_theta=(digi->GetTheta())*(180./TMath::Pi());
	      digi_phi=(digi->GetPhi())*(180./TMath::Pi());
	    
	      h10digi->Fill(theta_mc);
	      h20digi->Fill(phi_mc);
	    
	      h10ddigi->Fill(digi_theta);
	      h20ddigi->Fill(digi_phi);
	    
	      h1d->Fill(digi_theta-theta_mc);
	      h2d->Fill(digi_phi-phi_mc);
	    
	      hxpad_x_d->Fill(digi->GetXPad(),digi->GetThetaInt());
	      hypad_y_d->Fill(digi->GetYPad(),digi->GetPhiInt());
	       
	      hx_row_d->Fill(digi->GetRow(),digi->GetThetaInt());
	      
	      id_d = digi->GetDetectorId();
	    
	      crystal_did = (id_d%10000);
	      row_did = ((id_d/1000000)%100);

	      hmodule_d->Fill(module_d);
	      hrow_crystal_d->Fill(crystal_did,row_did);
	      hx_y_d->Fill(digi->GetThetaInt(),digi->GetPhiInt());
	     
	      hid_d->Fill(id_d);
	    }
	  }
	hdigi_ene->Fill(sum_digi_ene);	
	hdigi_ene_cp1->Fill(digi_ene_cp1);
        hdigi_ene_cp2->Fill(digi_ene_cp2);
	hdigi_ene_cp3->Fill(digi_ene_cp3);
	hdigi_ene_cp4->Fill(digi_ene_cp4);

	// reconstructed clusters
	Int_t ClustNoOrig,ClustNo,ClustNo_cp1,ClustNo_cp2,ClustNo_cp3,ClustNo_cp4;
	Int_t detectorID, module, mod3;
	if (cluster_array->GetEntriesFast()>0)
	  {
	    Int_t    idWithHighestEnergy = 0;
	    Double_t highestEnergy = -1.;
	    
	    // First find the cluster with the highest energy
	    
	    if (module_h==3){
	      
	      ClustNoOrig=ClustNo=
		ClustNo_cp1=ClustNo_cp2=ClustNo_cp3=ClustNo_cp4=0;

	      ClustNoOrig=cluster_array->GetEntriesFast();	    
	      for (Int_t i=0; i<ClustNoOrig; i++)
		{
		  PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
		  cluster_energy=cluster->energy();
		  
		  std::vector<PndEmcDigi*> digiList=cluster->DigiList();
		  ndigi=digiList.size();
		  
		  if(ndigi != 0)
		    {
		      for(int k = 0; k < digiList.size(); k++)
			{
			  detectorID = digiList[k]->GetDetectorId();
			  module = digiList[k]->GetModule();
			  row_c = digiList[k]->GetRow();
			  crystal_c = digiList[k]->GetCrystal();
			  copy_c = digiList[k]->GetCopy();
			  
			  crystal_cid = (detectorID%10000);
			  row_cid = ((detectorID/1000000)%100);
			  
			  hrow_crystal_c->Fill(crystal_cid,row_cid);
			  //cluster_energy=cluster->energy();		 
			}	      
		    }
		  mod3 = module;
		  
		  if (cluster_energy>highestEnergy)
		    {		   
		      idWithHighestEnergy = i;
		      highestEnergy = cluster_energy;
		      ClustNo++;
		    }
		} // number of cluster
	      
	      // Lets analyze that cluster!
	      if (mod3 !=3 )cout << "mod3 = "<< mod3 << endl;	    
	      PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(idWithHighestEnergy);

	      TVector3 cluster_pos=cluster->where();
	      cluster_theta=cluster_pos.Theta()*(180./TMath::Pi());
	      cluster_phi=cluster_pos.Phi()*(180./TMath::Pi());//+360;
	      
	      cluster_energy=cluster->energy();
	      //ClustNo++;

	      if (copy_c==1){
		hclus_ene_cp1->Fill(cluster->energy());
		hz_cp1_c->Fill(cluster->z());
		ClustNo_cp1++;
	      }
	      if (copy_c==2){
		hclus_ene_cp2->Fill(cluster->energy());
		hz_cp2_c->Fill(cluster->z());
		ClustNo_cp2++;
	      }
	      if (copy_c==3){
		hclus_ene_cp3->Fill(cluster->energy());
		hz_cp3_c->Fill(cluster->z());
		ClustNo_cp3++;
	      }
	      if (copy_c==4){
		hclus_ene_cp4->Fill(cluster->energy());
		hz_cp4_c->Fill(cluster->z());
		ClustNo_cp4++;
	      }

	      hz_clust->Fill(cluster->z());
	      
	      //cout<<"CLUSTER: th="<<cluster_theta<<", ph="<<cluster_phi<<", energy="<<cluster_energy<<endl;
	      
	      sum_clus_ene+=cluster_energy;
	      
	      cemc=p4mom.E();
	      cemc_sum+=cemc;
	      
	      hclus_ene_mc->Fill(cemc);
	      hclus_ene->Fill(cluster_energy);
	      
	      hthc_mc->Fill(theta_mc); // Monte Carlo 
	      hphc_mc->Fill(phi_mc);   // Monte Carlo 
	      
	      hthc->Fill(cluster_theta); // reconstructed 
	      hphc->Fill(cluster_phi);   // reconstructed 
	      
	      hthc_diff->Fill(cluster_theta-theta_mc);
	      hphc_diff->Fill(cluster_phi-phi_mc);
	    }
	  }
	hclus_ene_sum_mc->Fill(cemc_sum);
	
	hclus_ene_sum->Fill(sum_clus_ene);

	hClustNoOrig->Fill(ClustNoOrig);
	hClustNo->Fill(ClustNo);
	
	hClNo_cp1->Fill(ClustNo_cp1);
	hClNo_cp2->Fill(ClustNo_cp2);
	hClNo_cp3->Fill(ClustNo_cp3);
	hClNo_cp4->Fill(ClustNo_cp4);
   }
 
   TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800); 	
   c1->Divide(2,3);

   gStyle->SetPalette(1);
   
   c1->cd(1); // Z position in 4 quarters of fwendcap (HITS)
   
   TLegend *copies = new TLegend(0.65,0.4,0.85,0.6);
   copies->SetTextSize(0.05);
   copies->SetFillColor(0);
   copies->SetBorderSize(0);
   //copies->SetMarkerSize(2.0);
   copies->AddEntry(hz_h, "All copies (G3)","l");
   copies->AddEntry(hz_cp1_h, "copy_1","l");
   copies->AddEntry(hz_cp2_h, "copy_2","l");
   copies->AddEntry(hz_cp3_h, "copy_3","l");
   copies->AddEntry(hz_cp4_h, "copy_4","l");
   
   hz_h->Draw(); // all quarters (copies) together

   hz_cp1_h->SetLineColor(2);
   hz_cp1_h->Draw("same");   

   hz_cp2_h->SetLineColor(3);
   hz_cp2_h->Draw("same");   

   hz_cp3_h->SetLineColor(4);
   hz_cp3_h->Draw("same");   

   hz_cp4_h->SetLineColor(6);
   hz_cp4_h->Draw("same");   
   
   hz_h->GetXaxis()->SetTitle("Z position");
   hz_h->GetYaxis()->SetTitle("Counts");

   copies->Draw();
      
   c1->Modified();


   c1->cd(2);// Sum of energy 4 quarters of fwendcap (HITs)
   TLegend *copies4 = new TLegend(0.20,0.40,0.5,0.6);
   copies4->SetTextSize(0.05);
   copies4->SetFillColor(0);
   copies4->SetBorderSize(0);
   copies4->AddEntry(hhit_ene, "All copies (G3)","l");
   copies4->AddEntry(hhit_ene_cp1, "copy_1","l");
   copies4->AddEntry(hhit_ene_cp2, "copy_2","l");
   copies4->AddEntry(hhit_ene_cp3, "copy_3","l");
   copies4->AddEntry(hhit_ene_cp4, "copy_4","l");
   
   hhit_ene->Draw(); // all quarters (copies) together

   hhit_ene_cp1->SetLineColor(2);
   hhit_ene_cp1->Draw("same");   

   hhit_ene_cp2->SetLineColor(3);
   hhit_ene_cp2->Draw("same");   

   hhit_ene_cp3->SetLineColor(4);
   hhit_ene_cp3->Draw("same");   

   hhit_ene_cp4->SetLineColor(6);
   hhit_ene_cp4->Draw("same");   
   
   hhit_ene->GetXaxis()->SetTitle("Hit energy (GeV)");
   hhit_ene->GetYaxis()->SetTitle("Counts");

   copies4->Draw();
      
   c1->Modified();   


   c1->cd(3);// Z position in 4 quarters of fwendcap (DIGITs)
   TLegend *copies6 = new TLegend(0.65,0.4,0.85,0.6);
   copies6->SetTextSize(0.05);
   copies6->SetFillColor(0);
   copies6->SetBorderSize(0);
   copies6->AddEntry(hz_d, "All copies (G3)","l");
   copies6->AddEntry(hz_cp1_d, "copy_1","l");
   copies6->AddEntry(hz_cp2_d, "copy_2","l");
   copies6->AddEntry(hz_cp3_d, "copy_3","l");
   copies6->AddEntry(hz_cp4_d, "copy_4","l");
   
   hz_d->Draw(); // all quarters (copies) together

   hz_cp1_d->SetLineColor(2);
   hz_cp1_d->Draw("same");   

   hz_cp2_d->SetLineColor(3);
   hz_cp2_d->Draw("same");   

   hz_cp3_d->SetLineColor(4);
   hz_cp3_d->Draw("same");   

   hz_cp4_d->SetLineColor(6);
   hz_cp4_d->Draw("same");   
   
   hz_d->GetXaxis()->SetTitle("Z position");
   hz_d->GetYaxis()->SetTitle("Counts");

   copies6->Draw();
      
   c1->Modified();


   c1->cd(4);// Sum of energy 4 quarters of fwendcap (DIGITs)
   TLegend *copies5 = new TLegend(0.20,0.40,0.5,0.6);
   copies5->SetTextSize(0.05);
   copies5->SetFillColor(0);
   copies5->SetBorderSize(0);
   copies5->AddEntry(hdigi_ene, "All copies (G3)","l");
   copies5->AddEntry(hdigi_ene_cp1, "copy_1","l");
   copies5->AddEntry(hdigi_ene_cp2, "copy_2","l");
   copies5->AddEntry(hdigi_ene_cp3, "copy_3","l");
   copies5->AddEntry(hdigi_ene_cp4, "copy_4","l");
   
   hdigi_ene->Draw(); // all quarters (copies) together

   hdigi_ene_cp1->SetLineColor(2);
   hdigi_ene_cp1->Draw("same");   

   hdigi_ene_cp2->SetLineColor(3);
   hdigi_ene_cp2->Draw("same");   

   hdigi_ene_cp3->SetLineColor(4);
   hdigi_ene_cp3->Draw("same");   

   hdigi_ene_cp4->SetLineColor(6);
   hdigi_ene_cp4->Draw("same");   
   
   hdigi_ene->GetXaxis()->SetTitle("Digi energy (GeV)");
   hdigi_ene->GetYaxis()->SetTitle("Counts");

   copies5->Draw();
      
   c1->Modified();   

   c1->cd(5);// Z position in 4 quarters of fwendcap (CLUSTERs)
   TLegend *copies3 = new TLegend(0.20,0.30,0.5,0.5);
   copies3->SetTextSize(0.05);
   copies3->SetFillColor(0);
   copies3->SetBorderSize(0);
   copies3->AddEntry(hz_clust, "All copies (G3)","l");
   copies3->AddEntry(hz_cp1_c, "copy_1","l");
   copies3->AddEntry(hz_cp2_c, "copy_2","l");
   copies3->AddEntry(hz_cp3_c, "copy_3","l");
   copies3->AddEntry(hz_cp4_c, "copy_4","l");
   
   hz_clust->Draw(); // all quarters (copies) together

   hz_cp1_c->SetLineColor(2);
   hz_cp1_c->Draw("same");   

   hz_cp2_c->SetLineColor(3);
   hz_cp2_c->Draw("same");   

   hz_cp3_c->SetLineColor(4);
   hz_cp3_c->Draw("same");   

   hz_cp4_c->SetLineColor(6);
   hz_cp4_c->Draw("same");   
   
   hz_clust->GetXaxis()->SetTitle("Z position (cm)");
   hz_clust->GetYaxis()->SetTitle("Counts");

   copies3->Draw();
      
   c1->Modified();


   c1->cd(6); // Energy 4 quarters of fwendcap (CLUSTERs)
   TLegend *copies2a = new TLegend(0.20,0.4,0.5,0.6);
   copies2a->SetTextSize(0.05);
   copies2a->SetFillColor(0);
   copies2a->SetBorderSize(0);
   copies2a->AddEntry(hclus_ene, "All copies (G3)","l");
   copies2a->AddEntry(hclus_ene_cp1, "copy_1","l");
   copies2a->AddEntry(hclus_ene_cp2, "copy_2","l");
   copies2a->AddEntry(hclus_ene_cp3, "copy_3","l");
   copies2a->AddEntry(hclus_ene_cp4, "copy_4","l");
   
   hclus_ene->Draw(); // all quarters (copies) together

   hclus_ene_cp1->SetLineColor(2);
   hclus_ene_cp1->Draw("same");   

   hclus_ene_cp2->SetLineColor(3);
   hclus_ene_cp2->Draw("same");   

   hclus_ene_cp3->SetLineColor(4);
   hclus_ene_cp3->Draw("same");   

   hclus_ene_cp4->SetLineColor(6);
   hclus_ene_cp4->Draw("same");   
   
   hclus_ene->GetXaxis()->SetTitle("Cluster energy (GeV)");
   hclus_ene->GetYaxis()->SetTitle("Counts");

   copies2a->Draw();
      
   c1->Modified();


   TCanvas* c9 = new TCanvas("c9", "", 100, 100, 800, 800); 	
   c9->Divide(2,2);

   c9->cd(1);
   hthc_diff->Draw();
   hthc_diff->GetXaxis()->SetTitle("#theta_{Reconstructed}-#theta_{MC} (#circ)");
   hthc_diff->GetYaxis()->SetTitle("Counts");

   c9->cd(2);
   hphc_diff->Draw();
   hphc_diff->GetXaxis()->SetTitle("#phi_{Reconstructed}-#phi_{MC} (#circ)");
   hphc_diff->GetYaxis()->SetTitle("Counts");

   c9->cd(3);
   hthc->Draw();
   hthc_mc->SetLineColor(4);
   hthc_mc->Draw("same");
   hthc->GetXaxis()->SetTitle("#theta_{Reconstructed} (black) #theta_{MC} (blue) (#circ)");
   hthc->GetYaxis()->SetTitle("Counts");

   c9->cd(4);
   hphc->Draw();
   hphc_mc->SetLineColor(4);
   hphc_mc->Draw("same");
   hphc->GetXaxis()->SetTitle("#phi_{Reconstructed} (black) #phi_{MC} (blue) (#circ)");
   hphc->GetYaxis()->SetTitle("Counts");
   
   /*TLegend *cops = new TLegend(0.40,0.50,0.6,0.7);
   cops->SetTextSize(0.05);
   cops->SetFillColor(0);
   cops->SetBorderSize(0);
   cops->AddEntry(hClustNoOrig, "All clusters","l");
   cops->AddEntry(hClustNo, "Clusters with highest energy","l");

   hClustNo->Draw(); // all quarters (copies) together

   hClustNoOrig->SetLineStyle(2);
   hClustNoOrig->Draw("same");   

   cops->Draw();
   */
   c9->Modified();
}
