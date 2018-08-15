  #include "TH1.h"
  #include "TMath.h"
  #include "TF1.h"
  #include "TLegend.h"
  #include "TCanvas.h
  #include "TList.h"
  #include <iomanip>


int analysis_hit_digi_cluster_fwendcap()
{
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  TFile* fsim = new TFile("/home/moini/myPandaRoot/myCodes/ROOTfiles/FwEndCapResolution_vacuum_noMagField/assuming150photonsperMeV/emc_complete_5GeVphotons.root");

  TTree *tsim=(TTree *) fsim->Get("pndsim") ;
  TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack",&mctrack_array);

  TClonesArray* hit_array=new TClonesArray("PndEmcHit");
  tsim->SetBranchAddress("EmcHit",&hit_array);

  TClonesArray* digi_array=new TClonesArray("PndEmcDigi");
  tsim->SetBranchAddress("EmcDigi",&digi_array);

  TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
  tsim->SetBranchAddress("EmcCluster",&cluster_array);

  TClonesArray* bump_array=new TClonesArray("PndEmcBump");
  tsim->SetBranchAddress("EmcBump",&bump_array);

  Double_t theta_mc, phi_mc, ene_mc,
	   hit_ene, hit_theta, hit_phi, hit_id, hit_x, hit_y,
	   digi_ene, digi_theta, digi_phi;
  TVector3 photon_momentum;
  TLorentzVector p4mom;
  Double_t clusterThresholdEnergy=0;

  TH1F *hene_mc= new TH1F("hene_mc","MC energy (GeV)",100,0.0,1.05);
  TH1F *h10= new TH1F("h10"," Theta (MC)",180,0.,180.);
  TH1F *h20= new TH1F("h20"," Phi (MC)",360,0.,360.);
  TH1F *h10d= new TH1F("h10d"," Theta (data)",180,0.,180.);
  TH1F *h20d= new TH1F("h20d"," Phi (data)",360,0.,360.);
  TH1F *h1= new TH1F("h1"," Theta difference",100,-2.,2.);
  TH1F *h2= new TH1F("h2"," Phi difference",100,-2.,2.);

  Int_t ncounts = tsim->GetEntriesFast();
  cout << "Number of events = " << ncounts<< endl;
  //ncounts=100; //in case you want to analyse specific number of events


   Int_t module_h, id_h, crystal_hid, row_hid;
   TH1F *HISTene_h= new TH1F("HISTene_h","energy (hit)",1000,0.0,1.01);  // deposited energy per hit in the whole FwEndCap
   TH1F *HISThit_ene= new TH1F("HISThit_ene","FwEndCap energy (hit)",2010,0.0,2.01); // deposited energy per event in the whole FwEndCap (1 MeV/bin precision)
   TH1F *HISThit_ene_10by6crystalPack= new TH1F("HISThit_ene_10by6crystalPack","",2010,0.0,2.01);//(1 MeV/bin precision)
   TH1F *HISThit_ene_3by3crystalPack= new TH1F("HISThit_ene_3by3crystalPack","",2010,0.0,2.01);//(1 MeV/bin precision)
   TH1F *HISTz_h = new TH1F("HISTz_h","z (hit)",100,200,300);
   TH2F *HISTxpad_x_h = new TH2F("HISTxpad_x_h","xpad vs x (hit)",211,-105,105,2*38.5,-38.5,38.5);
   TH2F *HISTypad_y_h = new TH2F("HISTypad_y_h","ypad vs y (hit)",211,-105,105,2*38.5,-38.5,38.5);
   TH2F *HISTrow_x_h = new TH2F("HISTrow_x_h","row vs x (hit)",200,-100,100,2*38.5,-38.5,38.5);
   TH2F *HISTrow_crystal_h = new TH2F("HISTrow_crystal_h","row vs column (hit)",2*38.5,-38.5,38.5,2*38.5,-38.5,38.5);
   TH2F *HISTy_x_h = new TH2F("HISTy_x_h","y vs x (hit)",211,-105,105,211,-105,105);  // y- versus x-position of the center of the hit crystal
   TH1F *HISTtheta_h = new TH1F("HISTtheta_h","theta (hit)",180,0,180);  // theta of the
   TH1I *HISTmultiplicity_5MeVtrigThresh_h = new TH1I("HISTmultiplicity_5MeVtrigThresh_h","Crystal hit multiplicity (3 MeV detection threshold and considering 5 MeV trigger threshold)",40,0.5,40.5);
   TH1I *HISTmultiplicity_10MeVtrigThresh_h = new TH1I("HISTmultiplicity_10MeVtrigThresh_h","Crystal hit multiplicity (3 MeV detection threshold and 10 MeV trigger threshold)",40,0.5,40.5);
   TH1I *HISTmultiplicity_10MeVtrigThresh_5MeVdetectionThresh_h = new TH1I("HISTmultiplicity_10MeVtrigThresh_5MeVdetectionThresh_h","FwEndCap crystal hit multiplicity (5 MeV detection threshold and 10 MeV trigger threshold)",40,0.5,40.5);
   TH1I *HISTmultiplicity_10MeVtrigThresh_10MeVdetectionThresh_h = new TH1I("HISTmultiplicity_10MeVtrigThresh_10MeVdetectionThresh_h","FwEndCap crystal hit multiplicity (10 MeV trigger threshold) -- sensitivity to detection threshold",40,0.5,40.5);
   TH1I *HISTmultiplicity_10MeVtrigThresh_NOdetectionThresh_h = new TH1I("HISTmultiplicity_10MeVtrigThresh_NOdetectionThresh_h","FwEndCap crystal hit multiplicity (no detection threshold and 10 MeV trigger threshold)",40,0.5,40.5);
   TH1I *HISTmultiplicity_1MeVtrigThresh_100keV_h = new TH1I("HISTmultiplicity_1MeVtrigThresh_100keV_h","",40,0.5,40.5);
   TH1I *HISTmultiplicity_1MeVtrigThresh_300keV_h = new TH1I("HISTmultiplicity_1MeVtrigThresh_300keV_h","",40,0.5,40.5);

   //TH1F *HISTid_h = new TH1F("HISTid_h","id (HIT)",140000000,300000000,340000000);
  TH1F *HISTOhit_ene19_1= new TH1F("HISTOhit_ene19_1","crystal [19,1] dep. energy (GeV)",2010,0.0,2.01);
  TH1F *HISTOhit_ene19_2= new TH1F("HISTOhit_ene19_2","crystal [19,2] dep. energy (GeV)",2010,0.0,2.01);
  TH1F *HISTOhit_ene19_3= new TH1F("HISTOhit_ene19_3","crystal [19,3] dep. energy (GeV)",2010,0.0,2.01);
  TH1F *HISTOhit_ene19_4= new TH1F("HISTOhit_ene19_4","crystal [19,4] dep. energy (GeV)",2010,0.0,2.01);
  TH1F *HISTOhit_ene19_5= new TH1F("HISTOhit_ene19_5","crystal [19,5] dep. energy (GeV)",2010,0.0,2.01);

   Int_t module_d, id_d, crystal_did, row_did;
   TH1F *HISTdigi_ene= new TH1F("HISTdigi_ene","FwEndCap energy (digi)",2010,0.0,2.01); // deposited energy per event in the whole FwEndCap (1 MeV/bin precision)
   TH1F *HISTene_d= new TH1F("HISTene_d","energy (digi)",1000,0.0001,1.01);  // deposited energy per digi in the whole FwEndCap
   TH2F *HISTxpad_x_d = new TH2F("HISTxpad_x_d","xpad vs x (digi)",200,200,300,2*38.5,-38.5,38.5);   // here x means: map of the x-position of the hit crystal
   TH2F *HISTypad_y_d = new TH2F("HISTypad_y_d","ypad vs y (digi)",200,200,300,2*38.5,-38.5,38.5);   // here y means: map of the y-position of the hit crystal
   TH2F *HISTrow_x_d = new TH2F("HISTrow_x_d","row vs x (digi)",200,200,300,2*38.5,-38.5,38.5);      // here x means: map of the x-position of the hit crystal
   TH2F *HISTrow_crystal_d = new TH2F("HISTrow_crystal_d","row vs column (digi)",2*38.5,-38.5,38.5,2*38.5,-38.5,38.5);
   TH2F *HISTy_x_d = new TH2F("HISTy_x_d","y vs x (digi map)",200,200,300,200,200,300);  // here x and y mean: map of the y- and x-position of the hit crystal
   TH1F *HISTtotdigis_fromClusterAnalysis = new TH1F("HISTtotdigis_fromClusterAnalysis","total number of digis per event",100,0,25);
   TH1I *HISTmultiplicity_5MeVtrigThresh_d = new TH1I("HISTmultiplicity_5MeVtrigThresh_d","Crystal digi multiplicity per event (3 MeV detection threshold and considering 5 MeV trigger threshold)",40,0.5,40.5);
   TH1I *HISTmultiplicity_10MeVtrigThresh_d = new TH1I("HISTmultiplicity_10MeVtrigThresh_d","Crystal digi multiplicity per event (3 MeV detection threshold and considering 10 MeV trigger threshold)",40,0.5,40.5);
  TH1F *HISTOdigi_ene19_1= new TH1F("HISTOdigi_ene19_1","crystal [19,1] dep. energy (GeV)",2010,0.0,2.01);
  TH1F *HISTOdigi_ene19_2= new TH1F("HISTOdigi_ene19_2","crystal [19,2] dep. energy (GeV)",2010,0.0,2.01);
  TH1F *HISTOdigi_ene19_3= new TH1F("HISTOdigi_ene19_3","crystal [19,3] dep. energy (GeV)",2010,0.0,2.01);
  TH1F *HISTOdigi_ene19_4= new TH1F("HISTOdigi_ene19_4","crystal [19,4] dep. energy (GeV)",2010,0.0,2.01);
  TH1F *HISTOdigi_ene19_5= new TH1F("HISTOdigi_ene19_5","crystal [19,5] dep. energy (GeV)",2010,0.0,2.01);
  TH1F *HISTdigi_ene_10by6crystalPack= new TH1F("HISTdigi_ene_10by6crystalPack","",2010,0.0,2.01);//(1 MeV/bin precision)
  TH1F *HISTdigi_ene_3by3crystalPack= new TH1F("HISTdigi_ene_3by3crystalPack","",2010,0.0,2.01);//(1 MeV/bin precision)

   TH1I *HISTnumberOfClusters = new TH1I("HISTnumberOfClusters","Total number of clusters per event, having more than or equal to one digi per cluster",100,0,20);
   TH1I *HISTclustNo = new TH1I("HISTclustNo","Total number of clusters per event, having more than one digi per cluster and satisfying the cluster threshold energy",1000,0,20);
   TH1I *HISTndigisOfHighestEnergyCluster = new TH1I("HISTndigisOfHighestEnergyCluster","Number of digis per event for clusters with highest energy deposition",1000,0,30);
   TH1F *HISTmax_cluster_energy= new TH1F("HISTOmax_cluster_energy","Maximum cluster energy deposition per event",500,0.,1.05);
   TH1F *HISTmax_cluster_theta = new TH1F("HISTmax_cluster_theta","#theta per event of the cluster with maximum deposition energy",100,0,30);
   TH1F *HISTmax_cluster_phi = new TH1F("HISTmax_cluster_phi","#phi per event of the cluster with maximum deposition energy",370,-185,185);
   TH1F *HISTmax_cluster_x = new TH1F("HISTmax_cluster_x","x per event of the cluster with maximum deposition energy",220,-110,110);
   TH1F *HISTmax_cluster_y = new TH1F("HISTmax_cluster_y","y per event of the cluster with maximum deposition energy",220,-110,110);
   TH1F *HISTmax_cluster_z = new TH1F("HISTmax_cluster_z","z per event of the cluster with maximum deposition energy",80,208,216);
   TH2F *HISTmax_cluster_y_x = new TH2F("HISTmax_cluster_y_x","y vs x per event of the cluster with maximum deposition energy",220,-110,110,220,-110,110);
   TH1F *HISTtheta_MonteCarlo = new TH1F("HISTtheta_MonteCarlo","Monte Carlo #theta per event of the cluster with maximum deposition energy",100,0,30);
   TH1F *HISTphi_MonteCarlo = new TH1F("HISTphi_MonteCarlo","Monte Carlo #phi per event of the cluster with maximum deposition energy",370,-185,185);
   TH1F *HISTmax_cluster_theta_diff = new TH1F("HISTmax_cluster_theta_diff","Difference between Monte Carlo and reconstructed #theta per event of the cluster with maximum deposition energy",100,-5,5);
   TH1F *HISTmax_cluster_phi_diff = new TH1F("HISTmax_cluster_phi_diff","Difference between Monte Carlo and reconstructed #phi per event of the cluster with maximum deposition energy",360,-5,5);
   TH2F *HISTtheta_MonteCarlo_cluster = new TH2F("HISTtheta_MonteCarlo_cluster","Monte Carlo versus cluster #theta of the cluster with maximum deposition energy",100,0,30,100,0,30);
   TH2F *HISTphi_MonteCarlo_cluster = new TH2F("HISTphi_MonteCarlo_cluster","Monte Carlo versus cluster #phi of the cluster with maximum deposition energy",370,-185,185,370,-185,185);
   TH1F *HISTmax_cluster_x_diff = new TH1F("HISTmax_cluster_x_diff","Difference between Monte Carlo and reconstructed x per event of the cluster with maximum deposition energy",100,-5,5);
   TH1F *HISTmax_cluster_y_diff = new TH1F("HISTmax_cluster_y_diff","Difference between Monte Carlo and reconstructed y per event of the cluster with maximum deposition energy",100,-5,5);
   TH2F *HISTclusterSize_clusterPhi = new TH2F("HISTclusterSize_clusterPhi","number of digis vs cluster #phi for clusters with highest energy deposition",370,-185,185,100,0,25);
   TH2F *HISTclusterEnergy_clusterPhi = new TH2F("HISTclusterEnergy_clusterPhi","cluster energy deposition vs cluster #phi for clusters with highest energy deposition",370,-185,185,500,0.,1.05);
   TH2F *HISTnumberOfclusters_clusterPhi = new TH2F("HISTnumberOfclusters_clusterPhi","number of clusters vs cluster #phi for clusters with highest energy deposition",370,-185,185,100,0,20);
   TH2F *HISTtotdigis_clusterPhi = new TH2F("HISTtotdigis_clusterPhi","total number of digis vs cluster #phi (#phi of clusters with highest energy deposition)",370,-185,185,100,0,25);


   Int_t totalhits=0;         //counter on the total number of hits (fired detectors) in all events
   Int_t efficiencyCounter_h=0;
   Int_t atLeastOneHitPerEventCounter_h=0;

   Int_t totaldigis=0;        //counter on the total number of digis (fired detectors) in all events
   Int_t efficiencyCounter_d=0;
   Int_t atLeastOneHitPerEventCounter_d=0;

   for (int j = 0; j < ncounts; j++) {
     Int_t efficiencyFlag10MeV_h=0;
     Int_t efficiencyFlag5MeV_h=0;
     Int_t efficiencyFlag1MeV_h=0;
     Int_t atLeastOneHitPerEventFlag_h=0;
     Double_t sum_hit_ene=0;
     Double_t sum_hit_ene_10by6crystalPack=0;
     Double_t sum_hit_ene_3by3crystalPack=0;

     Int_t efficiencyFlag10MeV_d=0;
     Int_t efficiencyFlag5MeV_d=0;
     Int_t atLeastOneHitPerEventFlag_d=0;
     Double_t sum_digi_ene=0;
     Double_t sum_digi_ene_10by6crystalPack=0;
     Double_t sum_digi_ene_3by3crystalPack=0;

     tsim->GetEntry(j);

	PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0);
	photon_momentum=mctrack->GetMomentum();
	theta_mc=photon_momentum.Theta()*(180./TMath::Pi());
	phi_mc=photon_momentum.Phi()*(180./TMath::Pi());
	//p4mom=mctrack->Get4Momentum();

////////////////////////////////////////////////////////////////////////// HIT ANALYSIS
	Int_t nhits = hit_array->GetEntries();
	totalhits+=nhits;
	if (nhits!=0)
	  atLeastOneHitPerEventFlag_h = 1;
	Int_t firedCrystals_h=0;
	Int_t firedCrystals5MeV_h=0;
	Int_t firedCrystals10MeV_h=0;
	Int_t firedCrystals100keV_h=0;
	Int_t firedCrystals300keV_h=0;

	for (Int_t i=0; i<nhits; i++)
	  {
	    PndEmcHit *hit=(PndEmcHit*)hit_array->At(i); // get the i-th hit (detector) of the event
	    module_h = hit->GetModule();

	    if (module_h==3){
	      //hene_mc->Fill(p4mom.E());
	      hit_ene=hit->GetEnergy();
	      HISTene_h->Fill(hit_ene);
	      if (hit_ene >= 0.010) // 10 MeV trigger threshold used (here only) to calculate the efficiency of the FwEndCap geometry
		efficiencyFlag10MeV_h = 1;
	      if (hit_ene >= 0.005)
		efficiencyFlag5MeV_h = 1;
	      if (hit_ene >= 0.001)
		efficiencyFlag1MeV_h = 1;

	      if (hit_ene >= 0.003)
		firedCrystals_h++; //for 3 MeV detection threshold
	      if (hit_ene >= 0.005)
		firedCrystals5MeV_h++; //for 5 MeV detection threshold
	      if (hit_ene >= 0.010)
		firedCrystals10MeV_h++; //for 10 MeV detection threshold
	      if (hit_ene >= 0.0001)
		firedCrystals100keV_h++; //for 100 keV detection threshold
	      if (hit_ene >= 0.0003)
		firedCrystals300keV_h++; //for 300 keV detection threshold


	      //filling some histograms for a few crystals
	      if (hit_ene >= 0.003 && hit->GetXPad()==19 && hit->GetYPad()==1)
		HISTOhit_ene19_1->Fill(hit_ene);
	      else if (hit_ene >= 0.003 && hit->GetXPad()==19 && hit->GetYPad()==2)
		HISTOhit_ene19_2->Fill(hit_ene);
	      else if (hit_ene >= 0.003 && hit->GetXPad()==19 && hit->GetYPad()==3)
		HISTOhit_ene19_3->Fill(hit_ene);
	      else if (hit_ene >= 0.003 && hit->GetXPad()==19 && hit->GetYPad()==4)
		HISTOhit_ene19_4->Fill(hit_ene);
	      else if (hit_ene >= 0.003 && hit->GetXPad()==19 && hit->GetYPad()==5)
		HISTOhit_ene19_5->Fill(hit_ene);

	      if (hit_ene >= 0.003 && hit->GetXPad()>=15 && hit->GetXPad()<=24 && hit->GetYPad()>=-1 && hit->GetYPad()<=5)//pack of 6x10 crystals
	        sum_hit_ene_10by6crystalPack+=hit_ene;
	      if (hit_ene >= 0.003 && hit->GetXPad()>=18 && hit->GetXPad()<=20 && hit->GetYPad()>=1 && hit->GetYPad()<=3)//pack of 3x3 crystals around crystal of col=19 and row=2
		sum_hit_ene_3by3crystalPack+=hit_ene;

	      if (hit_ene >= 0.003)
	        sum_hit_ene+=hit_ene;

	      TVector3 hit_pos(hit->GetX(),hit->GetY(),hit->GetZ()); // position of the center of the hit crystal
	      hit_theta=hit_pos.Theta()*(180./TMath::Pi());
	      hit_phi=hit_pos.Phi()*(180./TMath::Pi());
	      h10->Fill(theta_mc);
	      h20->Fill(phi_mc);
	      h10d->Fill(hit_theta);
	      h20d->Fill(hit_phi);
	      h1->Fill(hit_theta-theta_mc);
	      h2->Fill(hit_phi-phi_mc);

	      id_h = hit->GetDetectorID();
	      crystal_hid = -(id_h%10000 -36);     //crystal column number (the same as GetXPad())
	      row_hid = (id_h/1000000)%100 - 37;   //crystal row number (the same as GetYPad())

	      HISTz_h->Fill(hit->GetZ());
	      HISTxpad_x_h->Fill(hit->GetX(),hit->GetXPad());
	      HISTypad_y_h->Fill(hit->GetY(),hit->GetYPad());
	      HISTrow_x_h->Fill(hit->GetX(),hit->GetYPad());

	      //HISTid_h->Fill(id_h);
	      HISTrow_crystal_h->Fill(hit->GetXPad(),hit->GetYPad());
	      HISTy_x_h->Fill(hit->GetX(),hit->GetY());

	    }
	  }

	if (efficiencyFlag10MeV_h==1)         //if the trigger threshold is satisfied
	{
	  HISThit_ene->Fill(sum_hit_ene);                                  // deposited energy per event in all hits (whole FwEndCap)
	  HISThit_ene_10by6crystalPack->Fill(sum_hit_ene_10by6crystalPack);// deposited energy per event in the pack of 6x10 crystals
	  HISThit_ene_3by3crystalPack->Fill(sum_hit_ene_3by3crystalPack);// deposited energy per event in the pack of 3x3 crystals
	}

	if (efficiencyFlag10MeV_h==1)
	{
          efficiencyCounter_h+=1;
	  HISTmultiplicity_10MeVtrigThresh_h->Fill(firedCrystals_h); //10 MEV trigger threshold and 3 MeV detection threshold (in the level of hit)
	  HISTmultiplicity_10MeVtrigThresh_5MeVdetectionThresh_h->Fill(firedCrystals5MeV_h); //10 MEV trigger threshold and 5 MeV detection threshold (in the level of hit)
	  HISTmultiplicity_10MeVtrigThresh_10MeVdetectionThresh_h->Fill(firedCrystals10MeV_h); //10 MEV trigger threshold and 10 MeV detection threshold (in the level of hit)
	  HISTmultiplicity_10MeVtrigThresh_NOdetectionThresh_h->Fill(nhits); //10 MEV trigger threshold and no detection threshold (in the level of hit)
	}
	if (efficiencyFlag5MeV_h==1)
	  HISTmultiplicity_5MeVtrigThresh_h->Fill(firedCrystals_h); //5 MEV trigger threshold and 3 MeV detection threshold (in the level of hit)

	if (efficiencyFlag1MeV_h==1)
	{
	  HISTmultiplicity_1MeVtrigThresh_100keV_h->Fill(firedCrystals100keV_h); //1 MEV trigger threshold and 100 keV detection threshold (in the level of hit)
	  HISTmultiplicity_1MeVtrigThresh_300keV_h->Fill(firedCrystals300keV_h); //1 MEV trigger threshold and 300 keV detection threshold (in the level of hit)
	}

	if (atLeastOneHitPerEventFlag_h==1)
	  atLeastOneHitPerEventCounter_h+=1;

////////////////////////////////////////////////////////////////////////// DIGI ANALYSIS
	Int_t ndigis = digi_array->GetEntries();
	totaldigis+=ndigis;
	if (ndigis!=0)
	  atLeastOneHitPerEventFlag_d = 1;

	for (Int_t i=0; i<ndigis; i++)
	  {
	    PndEmcDigi *digi=(PndEmcDigi*)digi_array->At(i);
	    TVector3 digi_pos=digi->where();
	    module_d = digi->GetModule();
	    if (module_d==3){
	      //dene_mc->Fill(p4mom.E());

	      digi_ene=digi->GetEnergy();
	      HISTene_d->Fill(digi_ene);
	      if (digi_ene >= 0.010) // 10 MeV trigger threshold used (here only) to calculate the efficiency of the FwEndCap geometry
		efficiencyFlag10MeV_d = 1;
	      if (digi_ene >= 0.005) // 5 MeV trigger threshold used (here only) to calculate the efficiency of the FwEndCap geometry
		efficiencyFlag5MeV_d = 1;

	      //filling some histograms for a few crystals
	      if (digi_ene >= 0.003 && digi->GetXPad()==19 && digi->GetYPad()==1)
		HISTOdigi_ene19_1->Fill(digi_ene);
	      else if (digi_ene >= 0.003 && digi->GetXPad()==19 && digi->GetYPad()==2)
		HISTOdigi_ene19_2->Fill(digi_ene);
	      else if (digi_ene >= 0.003 && digi->GetXPad()==19 && digi->GetYPad()==3)
		HISTOdigi_ene19_3->Fill(digi_ene);
	      else if (digi_ene >= 0.003 && digi->GetXPad()==19 && digi->GetYPad()==4)
		HISTOdigi_ene19_4->Fill(digi_ene);
	      else if (digi_ene >= 0.003 && digi->GetXPad()==19 && digi->GetYPad()==5)
		HISTOdigi_ene19_5->Fill(digi_ene);

	      if (digi_ene >= 0.003 && digi->GetXPad()>=15 && digi->GetXPad()<=24 && digi->GetYPad()>=-1 && digi->GetYPad()<=5)//pack of 6x10 crystals
	        sum_digi_ene_10by6crystalPack+=digi_ene;
	      if (digi_ene >= 0.003 && digi->GetXPad()>=18 && digi->GetXPad()<=20 && digi->GetYPad()>=1 && digi->GetYPad()<=3)//pack of 3x3 crystals around crystal of col=19 and row=2
		sum_digi_ene_3by3crystalPack+=digi_ene;

	      if (digi_ene >= 0.003)
	        sum_digi_ene+=digi_ene;

	      //Double_t digi_z=digi_pos.Z();
	      //hz_d->Fill(digi_z);

	      digi_theta=(digi->GetTheta())*(180./TMath::Pi());
	      digi_phi=(digi->GetPhi())*(180./TMath::Pi());
	      //h10digi->Fill(theta_mc);
	      //h20digi->Fill(phi_mc);
	      //h10ddigi->Fill(digi_theta);
	      //h20ddigi->Fill(digi_phi);
	      //h1d->Fill(digi_theta-theta_mc);
	      //h2d->Fill(digi_phi-phi_mc);

	      id_d = digi->GetDetectorId();
	      crystal_did = -(id_d%10000 - 36);     //crystal column number (the same as GetXPad())
	      row_did = (id_d/1000000)%100 - 37;    //crystal row number (the same as GetYPad())

	      HISTxpad_x_d->Fill(digi->GetThetaInt(),digi->GetXPad());
	      HISTypad_y_d->Fill(digi->GetPhiInt(),digi->GetYPad());
	      HISTrow_x_d->Fill(digi->GetThetaInt(),digi->GetYPad());

	      //hid_d->Fill(id_d);
	      //hmodule_d->Fill(module_d);
	      HISTrow_crystal_d->Fill(digi->GetXPad(),digi->GetYPad());
	      HISTy_x_d->Fill(digi->GetThetaInt(),digi->GetPhiInt());
	    }
	  }

	if (efficiencyFlag10MeV_d==1)             //if the trigger threshold is satisfied
	{
	  HISTdigi_ene->Fill(sum_digi_ene);       // deposited energy per event in all digis (whole FwEndCap)
	  HISTdigi_ene_10by6crystalPack->Fill(sum_digi_ene_10by6crystalPack);// deposited energy per event in the pack of 6x10 crystals
	  HISTdigi_ene_3by3crystalPack->Fill(sum_digi_ene_3by3crystalPack);// deposited energy per event in the pack of 3x3 crystals
	}

	if (efficiencyFlag10MeV_d==1)
	{
          efficiencyCounter_d+=1;
	  HISTmultiplicity_10MeVtrigThresh_d->Fill(ndigis); //10 MEV trigger threshold and 3 MeV detection threshold (in the level of digi this 3 MeV is already implemented)
	}
	if (efficiencyFlag5MeV_d==1)
	  HISTmultiplicity_5MeVtrigThresh_d->Fill(ndigis); //5 MEV trigger threshold and 3 MeV detection threshold (in the level of digi this 3 MeV is already implemented)

	if (atLeastOneHitPerEventFlag_d==1)
	  atLeastOneHitPerEventCounter_d+=1;

	/*
////////////////////////////////////////////////////////////////////////// CLUSTER ANALYSIS
	Int_t NumberOfClusters, ClustNo, idClusterSatisfiedThresholdEnergy, ndigisOfHighestEnergyCluster, totdigis_fromClusterAnalysis;
	idClusterSatisfiedThresholdEnergy = -1;                              //set per event
        NumberOfClusters=ClustNo=ndigisOfHighestEnergyCluster=totdigis_fromClusterAnalysis=0;    //set per event
        Double_t highestenergy = 0;                                          //set per event

	if (cluster_array->GetEntriesFast() > 0 && module_d==3)
	  {
	    // find the cluster with the highest energy

	      NumberOfClusters=cluster_array->GetEntriesFast();
	      for (Int_t i=0; i<NumberOfClusters; i++)                       //loop over all clusters of the present event under analysis
		{
		  PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
		  Double_t cluster_energy=cluster->energy();

		  Int_t ndigi=cluster->NumberOfDigis();                      // number of fired detectors inside the cluster
		  totdigis_fromClusterAnalysis += ndigi;
		  if (ndigi > 1 && cluster_energy >= clusterThresholdEnergy) //by definition for a cluster ndigi should be more than 1
		      ClustNo++;

		  if (cluster_energy > highestenergy)                        //finding the cluster with highest deposited energy
		    {
		      idClusterSatisfiedThresholdEnergy = i;
		      highestenergy = cluster_energy;
		      ndigisOfHighestEnergyCluster = ndigi;
		    }
		}

	      // Per-event analysis of this cluster with highest deposited energy
	      if (idClusterSatisfiedThresholdEnergy != -1) {
	        PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(idClusterSatisfiedThresholdEnergy);
	        TVector3 cluster_pos=cluster->where();
	        Double_t cluster_theta=cluster->theta()*(180./TMath::Pi()); //cluster_theta=cluster_pos.Theta()*(180./TMath::Pi());
	        Double_t cluster_phi=cluster->phi()*(180./TMath::Pi());     //cluster_phi=cluster_pos.Phi()*(180./TMath::Pi());
		Double_t cluster_x=cluster->x();
		Double_t cluster_y=cluster->y();
		Double_t cluster_z=cluster->z();
	        cluster_energy=cluster->energy();
	        //sum_clus_ene+=cluster_energy;                          //sum up the highest deposited cluster energy of all events
		HISTmax_cluster_energy->Fill(cluster_energy);            //fill the highest deposited cluster energy per event
		HISTmax_cluster_theta->Fill(cluster_theta);              // reconstructed
		HISTmax_cluster_theta_diff->Fill(cluster_theta-theta_mc);
		//if (cluster_theta>10)
		// {
		  HISTmax_cluster_phi->Fill(cluster_phi);                  // reconstructed
                  HISTmax_cluster_phi_diff->Fill(cluster_phi-phi_mc);
		  HISTmax_cluster_x->Fill(cluster_x);                      // reconstructed
		  HISTmax_cluster_y->Fill(cluster_y);                      // reconstructed
		  HISTmax_cluster_y_x->Fill(cluster_x,cluster_y);
		  HISTmax_cluster_z->Fill(cluster_z);                      // reconstructed
                  Double_t x_mc = cluster_z*TMath::Tan(theta_mc/180.*TMath::Pi())*TMath::Cos(phi_mc/180.*TMath::Pi());
		  Double_t y_mc = cluster_z*TMath::Tan(theta_mc/180.*TMath::Pi())*TMath::Sin(phi_mc/180.*TMath::Pi());
		  HISTmax_cluster_x_diff->Fill(cluster_x-x_mc);
		  HISTmax_cluster_y_diff->Fill(cluster_y-y_mc);
		 //}
		HISTtheta_MonteCarlo->Fill(theta_mc);
		HISTphi_MonteCarlo->Fill(phi_mc);
		HISTtheta_MonteCarlo_cluster->Fill(cluster_theta,theta_mc);
		HISTphi_MonteCarlo_cluster->Fill(cluster_phi,phi_mc);
		HISTclusterSize_clusterPhi->Fill(cluster_phi,ndigisOfHighestEnergyCluster);
		HISTclusterEnergy_clusterPhi->Fill(cluster_phi,cluster_energy);
		HISTnumberOfclusters_clusterPhi->Fill(cluster_phi,NumberOfClusters);
		HISTtotdigis_clusterPhi->Fill(cluster_phi,totdigis_fromClusterAnalysis);
	      }
	  }

       //hz_clust->Fill(cluster->z());

        if (NumberOfClusters>0)
	  HISTnumberOfClusters->Fill(NumberOfClusters);                         //number of clusters per event, having more than or equal to one digi per cluster
	if (ClustNo>0)
	  HISTclustNo->Fill(ClustNo);                                           //number of clusters per event, having more than one digi per cluster and satisfying the cluster threshold energy
	if (ndigisOfHighestEnergyCluster>0)
	  HISTndigisOfHighestEnergyCluster->Fill(ndigisOfHighestEnergyCluster); //number of digis per event for clusters with highest energy deposition

  	HISTtotdigis_fromClusterAnalysis->Fill(totdigis_fromClusterAnalysis);

*/
	if (j%1000 ==0)
	  cout<<"Evt. No. "<< j<<endl;
   }
   //cout<<"totalhits = "<<totalhits<<"\n";
   //cout<<"efficiencyCounter_h = "<<efficiencyCounter_h<<" , atLeastOneHitPerEventCounter_h = "<<atLeastOneHitPerEventCounter_h<<"\n";
   //cout<<"FwEndCap efficiency (more than 10 MeV deposition in at least one crystal) = "<< (double)efficiencyCounter_h/atLeastOneHitPerEventCounter_h<<"\n";

   //cout<<"totaldigis = "<<totaldigis<<"\n";
   //cout<<"efficiencyCounter_d = "<<efficiencyCounter_d<<" , atLeastOneHitPerEventCounter_d = "<<atLeastOneHitPerEventCounter_d<<"\n";
   //cout<<"FwEndCap efficiency (more than 10 MeV deposition in at least one crystal) = "<< (double)efficiencyCounter_d/atLeastOneHitPerEventCounter_d<<"\n";


   ////////////////////////////////////////////////////////////////////////// HIT && DIGI ANALYSIS
  /* TH2I *HISTnhitsVSndigis = new TH2I("HISTnhitsVSndigis","HISTnhitsVSndigis",90,-1,44,90,-1,44);
   TH2F *HISThitENE_digiENE = new TH2F("HISThitENE_digiENE","HISThitENE_digiENE",1000,0,1.,1000,0,1.);
   TH2F *HISThitVSdigi_phi_3MeVhitEnergyThreshold = new TH2F("HISThitVSdigi_phi_3MeVhitEnergyThreshold","hit vs digi #phi (3 MeV hit threshold energy)",370,-185,185,370,-185,185);
   TH2F *HISThitVSdigi_phi_3MeVhitEnergyThreshold_zoomed = new TH2F("HISThitVSdigi_phi_3MeVhitEnergyThreshold_zoomed","hit vs digi #phi (3 MeV hit threshold energy)",30,100,130,30,100,130);
   TH1F *HISThit_phi = new TH1F("HISThit_phi","#phi of hit",370,-185,185);
   TH1F *HISTdigi_phi = new TH1F("HISTdigi_phi","#phi of digi",370,-185,185);
   TH1F *HISThit_x = new TH1F("HISThit_x","x of hit",220,-110,110);
   TH1F *HISThit_y = new TH1F("HISThit_y","y of hit",220,-110,110);
   TH1F *HISTdigi_x = new TH1F("HISTdigi_x","x of digi",220,-110,110);
   TH1F *HISTdigi_y = new TH1F("HISTdigi_y","y of digi",220,-110,110);
   TH2F *HISTdigi_phi_x = new TH2F("HISTdigi_phi_x","#phi vs x of digi",220,-110,110,370,-185,185);
   TH2F *HISTdigi_phi_y = new TH2F("HISTdigi_phi_y","#phi vs y of digi",220,-110,110,370,-185,185);

    for (int j = 0; j < ncounts; j++)
      {
        tsim->GetEntry(j);
        Int_t ndigis = digi_array->GetEntries();
        Int_t nhits = hit_array->GetEntries();

	for (Int_t ii=0; ii<nhits; ii++)
	{
          PndEmcHit *hit=(PndEmcHit*)hit_array->At(ii);
          Double_t hitENE = hit->GetEnergy();
          TVector3 hit_pos(hit->GetX(),hit->GetY(),hit->GetZ()); // position of the center of the hit crystal
	  Double_t hitPHI=hit_pos.Phi()*(180./TMath::Pi()); //wrong answer if using-----> hit->GetPhi()*180./TMath::Pi();
	  Int_t hitXPad = hit->GetXPad();
	  Int_t hitYPad = hit->GetYPad();
	  Double_t hitX = hit->GetX();
	  Double_t hitY = hit->GetY();

	  for (Int_t jj=0; jj<ndigis; jj++)
	  {
	    PndEmcDigi *digi=(PndEmcDigi*)digi_array->At(jj);

	    Double_t digiENE = digi->GetEnergy();
	    Double_t digiPHI = digi->GetPhi()*180./TMath::Pi();
	    Int_t digiXPad = digi->GetXPad();
	    Int_t digiYPad = digi->GetYPad();
	    Double_t digiX = (digi->where()).X();
	    Double_t digiY = (digi->where()).Y();

	    if (hitXPad==digiXPad && hitYPad==digiYPad && hitENE >= 0.003)//the detection threshold condition of 3 MeV doesn't show much effect on the pattern of the 2D histogram
              {
                HISThitENE_digiENE->Fill(digiENE,hitENE);
	        HISThitVSdigi_phi_3MeVhitEnergyThreshold->Fill(digiPHI,hitPHI);
		HISThitVSdigi_phi_3MeVhitEnergyThreshold_zoomed->Fill(digiPHI,hitPHI);
	      }
	    if (hitXPad==digiXPad && hitYPad==digiYPad)
	    {
	      HISThit_phi->Fill(hitPHI);
	      HISTdigi_phi->Fill(digiPHI);

	      HISThit_x->Fill(hitX);
	      HISThit_y->Fill(hitY);
	      HISTdigi_x->Fill(digiX);
	      HISTdigi_y->Fill(digiY);
	      HISTdigi_phi_x->Fill(digiX,digiPHI);
	      HISTdigi_phi_y->Fill(digiY,digiPHI);
	    }
	  }
	}

	Int_t flag_h=0;
	for (Int_t ii=0; ii<nhits; ii++)
	{
	  PndEmcHit *hit=(PndEmcHit*)hit_array->At(ii);
	  if (hit->GetEnergy() > 0)
	    flag_h=1;
	}
	if (flag_h==1)
          HISTnhitsVSndigis->Fill(ndigis,nhits);//fill the histogram if there a hit registers (hitEne > 0)
      }
*/
////////////////////////////////////////////////////////////////////////// DRAWING
  gStyle->SetPalette(1);

   TString outfile= "./hitdigiEne.root";
   TFile* file_out = new TFile(outfile,"RECREATE");
   HISThit_ene->Write();
   HISThit_ene_10by6crystalPack->Write();
   HISThit_ene_3by3crystalPack->Write();
   HISTdigi_ene->Write();
   HISTdigi_ene_10by6crystalPack->Write();
   HISTdigi_ene_3by3crystalPack->Write();
   file_out->Close();

////////////////////////////////end of resolution calculation...............................................

  TCanvas* c1 = new TCanvas("c1","", 100, 100, 800, 800);
  c1->Divide(3,3);
  c1->cd(1);
  HISTOhit_ene19_1->Draw();
  c1->cd(2);
  HISTOhit_ene19_2->Draw();
  HISTOhit_ene19_2->GetXaxis()->SetTitle("central crystal Deposited Energy per event (hit analysis)");
  c1->cd(3);
  HISTOhit_ene19_3->Draw();
  c1->cd(4);
  HISTOhit_ene19_4->Draw();
  c1->cd(5);
  HISTOhit_ene19_5->Draw();
  c1->cd(6);
  HISThit_ene_3by3crystalPack->Draw();
  HISThit_ene_3by3crystalPack->GetXaxis()->SetTitle("3x3 pack of crystals in FwEndCap Deposited Energy per event (hit analysis)");
  c1->cd(7);
  HISThit_ene_10by6crystalPack->Draw();
  HISThit_ene_10by6crystalPack->GetXaxis()->SetTitle("10x6 pack of crystals in FwEndCap Deposited Energy per event (hit analysis)");
  c1->cd(8);
  HISThit_ene->Draw();
  HISThit_ene->GetXaxis()->SetTitle("FwEndCap Deposited Energy per event (hit analysis)");

  TCanvas* c2 = new TCanvas("c2","", 100, 100, 800, 800);
  c2->Divide(3,3);
  c2->cd(1);
  HISTOdigi_ene19_1->Draw();
  c2->cd(2);
  HISTOdigi_ene19_2->Draw();
  HISTOdigi_ene19_2->GetXaxis()->SetTitle("central crystal Deposited Energy per event (digi analysis)");
  c2->cd(3);
  HISTOdigi_ene19_3->Draw();
  c2->cd(4);
  HISTOdigi_ene19_4->Draw();
  c2->cd(5);
  HISTOdigi_ene19_5->Draw();
  c2->cd(6);
  HISTdigi_ene_3by3crystalPack->Draw();
  HISTdigi_ene_3by3crystalPack->GetXaxis()->SetTitle("3x3 pack of crystals in FwEndCap Deposited Energy per event (digi analysis)");
  c2->cd(7);
  HISTdigi_ene_10by6crystalPack->Draw();
  HISTdigi_ene_10by6crystalPack->GetXaxis()->SetTitle("10x6 pack of crystals in FwEndCap Deposited Energy per event (digi analysis)");
  c2->cd(8);
  HISTdigi_ene->Draw();
  HISTdigi_ene->GetXaxis()->SetTitle("FwEndCap Deposited Energy per event (digi analysis)");

 /*
  TCanvas* c3 = new TCanvas("c3","", 100, 100, 800, 800);
  c3->Divide(3,2);
  c3->cd(1);
   HISTy_x_h->Draw("colz");
  c3->cd(2);
   HISTrow_crystal_h->Draw("colz");
  c3->cd(3);
   HISTxpad_x_h->Draw("colz");
  c3->cd(4);
   HISTypad_y_h->Draw("colz");
  c3->cd(5);
   HISTrow_x_h->Draw("colz");
  */

 /*
  TCanvas* c4 = new TCanvas("c4","", 100, 100, 800, 800);
  c4->Divide(3,2);
  c4->cd(1);
   HISTy_x_d->Draw("colz");
  c4->cd(2);
   HISTrow_crystal_d->Draw("colz");
  c4->cd(3);
   HISTxpad_x_d->Draw("colz");
  c4->cd(4);
   HISTypad_y_d->Draw("colz");
  c4->cd(5);
   HISTrow_x_d->Draw("colz");
 */

/*
  TCanvas* c5 = new TCanvas("c5","", 100, 100, 800, 800);
  c5->Divide(4,4);
  c5->cd(1);
   HISTnumberOfClusters->Draw("");
  c5->cd(2);
   HISTclustNo->Draw("");
  c5->cd(3);
   HISTndigisOfHighestEnergyCluster->Draw("");
  c5->cd(4);
   HISTmax_cluster_energy->Draw("");
  c5->cd(5);
   HISTtheta_MonteCarlo->Draw("");
  c5->cd(6);
   HISTphi_MonteCarlo->Draw("");
  c5->cd(7);
   HISTtheta_MonteCarlo_cluster->Draw("colz");
  c5->cd(8);
   HISTphi_MonteCarlo_cluster->Draw("colz");
  c5->cd(9);
   HISTmax_cluster_theta->Draw("");
  c5->cd(10);
   HISTmax_cluster_phi->Draw("");
  c5->cd(11);
   HISTmax_cluster_theta_diff->Draw("");
  c5->cd(12);
   HISTmax_cluster_phi_diff->Draw("");
  c5->cd(13);
   HISTmax_cluster_x->Draw("");
  c5->cd(14);
   HISTmax_cluster_y->Draw("");
  c5->cd(15);
   //HISTmax_cluster_z->Draw("");
   HISTmax_cluster_x_diff->Draw("");
  c5->cd(16);
   HISTmax_cluster_y_diff->Draw("");

   TCanvas* c55 = new TCanvas("c55","", 100, 100, 800, 800);
   HISTmax_cluster_y_x->Draw("colz");
  */

 /* TCanvas* c6 = new TCanvas("c6","", 100, 100, 800, 800);
  c6->Divide(2,2);
  c6->cd(1);
   HISTclusterSize_clusterPhi->Draw("colz");
  c6->cd(2);
   HISTclusterEnergy_clusterPhi->Draw("colz");
  c6->cd(3);
   HISTnumberOfclusters_clusterPhi->Draw("colz");
  c6->cd(4);
   HISTtotdigis_clusterPhi->Draw("colz");
   //HISTtotdigis_fromClusterAnalysis->Draw();
 */

   /*
  TCanvas* c7 = new TCanvas("c7","", 100, 100, 800, 800);
  c7->Divide(2,2);
  c7->cd(1);
   HISTnhitsVSndigis->Draw("colz");
  c7->cd(3);
   HISThitENE_digiENE->Draw("colz");
  c7->cd(2)->Divide(2,1);
  c7->cd(2)->cd(1);
   HISThit_phi->Draw();
  c7->cd(2)->cd(2);
   HISTdigi_phi->Draw();
  c7->cd(4)->Divide(2,1);
  c7->cd(4)->cd(1);
   HISThitVSdigi_phi_3MeVhitEnergyThreshold->Draw("colz");
  c7->cd(4)->cd(2);
   HISThitVSdigi_phi_3MeVhitEnergyThreshold_zoomed->Draw("colz");

  TCanvas* c77 = new TCanvas("c77","", 100, 100, 800, 1000);
  c77->Divide(2,3);
  c77->cd(1);
   HISThit_x->Draw();
  c77->cd(2);
   HISThit_y->Draw();
  c77->cd(3);
   HISTdigi_x->Draw();
  c77->cd(4);
   HISTdigi_y->Draw();
  c77->cd(5);
   HISTdigi_phi_x->Draw("colz");
  c77->cd(6);
   HISTdigi_phi_y->Draw("colz");
*/

 /*
  TCanvas* c8 = new TCanvas("c8","", 100, 100, 1200, 800);
  c8->Divide(2,2);
  c8->cd(1);
   HISTmultiplicity_5MeVtrigThresh_d->Draw();
  c8->cd(2);
   HISTmultiplicity_10MeVtrigThresh_d->Draw();
  c8->cd(3);
   HISTmultiplicity_5MeVtrigThresh_h->Draw();//3 MeV detection threshold
  c8->cd(4);
   HISTmultiplicity_10MeVtrigThresh_h->Draw();//3 MeV detection threshold


  TCanvas* c9 = new TCanvas("c9","", 100, 100, 1000, 800);
  HISTmultiplicity_10MeVtrigThresh_10MeVdetectionThresh_h->Draw();
  HISTmultiplicity_10MeVtrigThresh_10MeVdetectionThresh_h->SetLineWidth(3);

  HISTmultiplicity_10MeVtrigThresh_NOdetectionThresh_h->Draw("same");
  HISTmultiplicity_10MeVtrigThresh_NOdetectionThresh_h->SetLineStyle(2);

  HISTmultiplicity_10MeVtrigThresh_h->Draw("same");//3 MeV detection threshold

  HISTmultiplicity_10MeVtrigThresh_5MeVdetectionThresh_h->Draw("same");
  HISTmultiplicity_10MeVtrigThresh_5MeVdetectionThresh_h->SetLineWidth(2);


  TCanvas* c99 = new TCanvas("c99","", 100, 100, 1000, 800);
  HISTmultiplicity_1MeVtrigThresh_300keV_h->Draw();
  HISTmultiplicity_1MeVtrigThresh_300keV_h->SetLineWidth(1);

  HISTmultiplicity_1MeVtrigThresh_100keV_h->Draw("same");
  HISTmultiplicity_1MeVtrigThresh_100keV_h->SetLineStyle(2);
*/

  return 0;
}
