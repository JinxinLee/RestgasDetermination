#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "TF1.h"

int analysis_digi_cluster_allGammasAnalysis_FullEmc()
{
  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  TFile* fsim = new TFile("/home/moini/myPandaRoot/myCodes/ROOTfiles/h_c_channel/emc_complete_fullEMC.root");

  TTree *tsim=(TTree *) fsim->Get("pndsim") ;
  TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack",&mctrack_array);

  TClonesArray* digi_array=new TClonesArray("PndEmcDigi");
  tsim->SetBranchAddress("EmcDigi",&digi_array);

  TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
  tsim->SetBranchAddress("EmcCluster",&cluster_array);

  TClonesArray* bump_array=new TClonesArray("PndEmcBump");
  tsim->SetBranchAddress("EmcBump",&bump_array);

  Double_t theta_mc, phi_mc, ene_mc,
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
  //ncounts=10000; //in case you want to analyse specific number of events

   Int_t module_d, id_d, crystal_did, row_did;
   TH1F *HISTdigi_ene= new TH1F("HISTdigi_ene","FwEndCap energy (digi)",2000,0.0,2.05); // deposited energy per event in the whole FwEndCap
   TH1F *HISTene_d= new TH1F("HISTene_d","energy (digi)",1000,0.0001,1.01);  // deposited energy per digi in the whole FwEndCap
   TH2F *HISTxpad_x_d = new TH2F("HISTxpad_x_d","xpad vs x (digi)",200,200,300,2*38.5,-38.5,38.5);   // here x means: map of the x-position of the hit crystal
   TH2F *HISTypad_y_d = new TH2F("HISTypad_y_d","ypad vs y (digi)",200,200,300,2*38.5,-38.5,38.5);   // here y means: map of the y-position of the hit crystal
   TH2F *HISTrow_x_d = new TH2F("HISTrow_x_d","row vs x (digi)",200,200,300,2*38.5,-38.5,38.5);      // here x means: map of the x-position of the hit crystal
   TH2F *HISTrow_crystal_d = new TH2F("HISTrow_crystal_d","row vs column (digi)",2*38.5,-38.5,38.5,2*38.5,-38.5,38.5);
   TH2F *HISTy_x_d = new TH2F("HISTy_x_d","y vs x (digi map)",200,200,300,200,200,300);  // here x and y mean: map of the y- and x-position of the hit crystal
   TH1F *HISTOdigi_ene11_12= new TH1F("HISTOdigi_ene11_12","crystal [11,12] dep. energy (GeV)",100,0.0001,1.01);
   TH1F *HISTOdigi_ene12_12= new TH1F("HISTOdigi_ene12_12","crystal [12,12] dep. energy (GeV)",100,0.0001,1.01);
   TH1F *HISTOdigi_ene13_12= new TH1F("HISTOdigi_ene13_12","crystal [13,12] dep. energy (GeV)",100,0.0001,1.01);
   TH1F *HISTOdigi_ene14_12= new TH1F("HISTOdigi_ene14_12","crystal [14,12] dep. energy (GeV)",100,0.0001,1.01);
   TH1F *HISTtotdigis_fromClusterAnalysis = new TH1F("HISTtotdigis_fromClusterAnalysis","total number of digis per event",100,0,25);
   TH1I *HISTmultiplicity_5MeVtrigThresh_d = new TH1I("HISTmultiplicity_5MeVtrigThresh_d","Crystal digi multiplicity per event (3 MeV detection threshold and considering 5 MeV trigger threshold)",40,0.5,40.5);
   TH1I *HISTmultiplicity_10MeVtrigThresh_d = new TH1I("HISTmultiplicity_10MeVtrigThresh_d","Crystal digi multiplicity per event (3 MeV detection threshold and considering 10 MeV trigger threshold)",40,0.5,40.5);

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

   TH1F *HISTinvariantMass = new TH1F("HISTinvariantMass","invariant mass of every two clusters",3200,0.,3.2);
   TH2F *HISTinvariantMass_openingAngle = new TH2F("HISTinvariantMass_openingAngle","opening angle of the two clusters versus the invariant mass of the assumed two gammas that make the two clusters",3200,0,3.2,500,0,180);
   TH1F *HIST_Egamma1Egamma2 = new TH1F("HIST_Egamma1Egamma2","E_{#gamma1} #times E_{#gamma2}",8000,0,8.);
   TH2F *HIST_Egamma1vsEgamma2 = new TH2F("HIST_Egamma1vsEgamma2","E_{#gamma2} versus E_{#gamma1}",6400,0,6.4,3200,0,3.2);
   TH2F *HIST_Egamma1Egamma2_openingAngle = new TH2F("HIST_Egamma1Egamma2_openingAngle","opening angle of the two clusters versus E_{#gamma1} #times E_{#gamma2}",8000,0,8.,500,0,180);
   TH2F *HISTinvariantMass_Egamma1Egamma2 = new TH2F("HISTinvariantMass_Egamma1Egamma2","E_{#gamma1} #times E_{#gamma2} versus the invariant mass of the two gammas",3200,0,3.2,8000,0,8.);
   TH2F *HIST_Egamma1_Egamma1Egamma2 = new TH2F("HIST_Egamma1_Egamma1Egamma2","E_{#gamma1} #times E_{#gamma2} versus E_{#gamma1}",6400,0,6.4,8000,0,8.);
   TH2F *HIST_Egamma1_openingAngle = new TH2F("HIST_Egamma1_openingAngle","opening angle of the two clusters versus E_{#gamma1}",6400,0,6.4,500,0,180);
   TH3F *HISTinvariantMass_openingAngle_Egamma1Egamma2 = new TH3F("HISTinvariantMass_openingAngle_Egamma1Egamma2","",150,0,1.5,140,0,50,400,0,4.);

   TH1F *moduleDistribution = new TH1F("moduleDistribution","Module distribution of clusters for the decay channel h_{c}-> #pi^{0} #pi^{0} #eta #gamma",12,0,6);

   Int_t totaldigis=0;        //counter on the total number of digis (fired detectors) in all events
   Int_t efficiencyCounter_d=0;
   Int_t atLeastOneHitPerEventCounter_d=0;
   Int_t totClusters=0;

   for (int j = 0; j < ncounts; j++) {

     Int_t efficiencyFlag10MeV_d=0;
     Int_t efficiencyFlag5MeV_d=0;
     Int_t atLeastOneHitPerEventFlag_d=0;
     Double_t sum_digi_ene=0;

     tsim->GetEntry(j);

	PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0);
	photon_momentum=mctrack->GetMomentum();
	theta_mc=photon_momentum.Theta()*(180./TMath::Pi());
	phi_mc=photon_momentum.Phi()*(180./TMath::Pi());
	//p4mom=mctrack->Get4Momentum();
/*
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
	      if (digi->GetXPad()==11 && digi->GetYPad()==12)
		HISTOdigi_ene11_12->Fill(digi_ene);
	      else if (digi->GetXPad()==12 && digi->GetYPad()==12)
		HISTOdigi_ene12_12->Fill(digi_ene);
	      else if (digi->GetXPad()==13 && digi->GetYPad()==12)
		HISTOdigi_ene13_12->Fill(digi_ene);
	      else if (digi->GetXPad()==14 && digi->GetYPad()==12)
		HISTOdigi_ene14_12->Fill(digi_ene);

	      if (digi_ene >= 0.010)
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

	if (efficiencyFlag10MeV_d==1)  //if the trigger threshold is satisfied
	  HISTdigi_ene->Fill(sum_digi_ene);           // deposited energy per event in all digis (whole FwEndCap)

	if (efficiencyFlag10MeV_d==1)
	{
          efficiencyCounter_d+=1;
	  HISTmultiplicity_10MeVtrigThresh_d->Fill(ndigis); //10 MEV trigger threshold and 3 MeV detection threshold (in the level of digi this 3 MeV is already implemented)
	}
	if (efficiencyFlag5MeV_d==1)
	  HISTmultiplicity_5MeVtrigThresh_d->Fill(ndigis); //5 MEV trigger threshold and 3 MeV detection threshold (in the level of digi this 3 MeV is already implemented)

	if (atLeastOneHitPerEventFlag_d==1)
	  atLeastOneHitPerEventCounter_d+=1;
*/

////////////////////////////////////////////////////////////////////////// CLUSTER ANALYSIS
	Int_t NumberOfClusters, ClustNo, totdigis_fromClusterAnalysis, ndigisOfHighestEnergyCluster, NumberOfClusters_noModule5;
        NumberOfClusters=ClustNo=totdigis_fromClusterAnalysis=ndigisOfHighestEnergyCluster=NumberOfClusters_noModule5=0;   //set per event
	Int_t idClusterSatisfiedThresholdEnergy=-1;
	Double_t highestEnergy=0;                   // (set per event)
	Double_t highestenergy_sumOfclusters=0;     // (set per event)

	std::vector <double> highestenergy;
	std::vector <double> cluster_x;
	std::vector <double> cluster_y;
	std::vector <double> cluster_z;
	std::vector <double> cluster_dist;
	std::vector <double> cluster_module;
	std::vector <double> originalClusterIndex;

	if (cluster_array->GetEntriesFast() > 0)
	  {
	      NumberOfClusters=cluster_array->GetEntriesFast(); //in all 5 modules
	      totClusters+=NumberOfClusters; //in all 5 modules
	      std::vector <double> clusterEnergies;
	      std::vector <double> clusterEnergies_original;

	      for (Int_t i=0; i<NumberOfClusters; i++)                        //loop over all clusters of the present event under analysis (ndigi>=1)
		{
		  PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
		  if (cluster->GetModule()!=5)
		    {
		      NumberOfClusters_noModule5++; //in all modules except for module 5
		      if (cluster->GetModule()==3 || cluster->GetModule()==4 || cluster->GetModule()==1 || cluster->GetModule()==2)//Full Emc except for shashlyk
		        {
		         moduleDistribution->Fill(cluster->GetModule());

		         Double_t cluster_energy=cluster->GetEnergyCorrected();//energy();
		         clusterEnergies.push_back(cluster_energy);                 //first, push all the cluster energies into the vector; no sorting yet
		         clusterEnergies_original.push_back(cluster_energy);        //make another one, in order to keep track of indices

		         Int_t ndigi=cluster->NumberOfDigis();                      // number of fired detectors inside the cluster
		         totdigis_fromClusterAnalysis += ndigi;
		         if (ndigi >= 1 && cluster_energy >= clusterThresholdEnergy) //by this definition for a cluster, ndigi is considered to be >= 1
		           ClustNo++;
		        }
		    }
		}

	      // Per-event analysis of all the clusters
      	      std::sort(clusterEnergies.begin(), clusterEnergies.end());   //now sort all the recognized clusters in the 4 modules

	      if (NumberOfClusters_noModule5>=7)  //finding at least 7 clusters on modules 1,2,3,4 in total
	      {
	        for (int k=0; k<NumberOfClusters_noModule5; k++)           //(k=0 corresponds to the cluster with biggest energy)
	         {
		   highestenergy.push_back(clusterEnergies[(NumberOfClusters_noModule5-1)-k]);
		   //cout << "decsendingly sorted energy of cluster # " << k+1 << " = " << highestenergy[k] << '\n';
		   ///highestenergy_sumOfclusters += highestenergy[k];
		   ///if (k==NumberOfClusters_noModule5-1)              //eventually write the summation of the cluster enegies
		     ///cout <<"highestenergy_sumOfclusters =  "<< highestenergy_sumOfclusters <<'\n'<<endl;

		   for (int i=0; i<NumberOfClusters_noModule5; i++) //finding the original cluster indices of the clusters
		     if (!(clusterEnergies_original[i]>highestenergy[k]) && !(clusterEnergies_original[i]<highestenergy[k]))//practically the equality sign
		     {
		       originalClusterIndex.push_back(i);
		       //cout<<"originalClusterIndex["<<k<<"] = "<<i<<endl;
		     }

		   PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(originalClusterIndex[k]);
		   TVector3 cluster_pos=cluster->where();
		   cluster_x.push_back(cluster->x());
		   cluster_y.push_back(cluster->y());
		   cluster_z.push_back(cluster->z());
		   cluster_dist.push_back(cluster_pos.Mag());//distance of the center of the cluster to the target point
		   cluster_module.push_back(cluster->GetModule());
	         }

	      for (int k0=1; k0<NumberOfClusters_noModule5; k0++)
	        {
		  //if (cluster_module[0] != cluster_module[k0])
		  {
		  Double_t cosTheta = (1./cluster_dist[0]/cluster_dist[k0])*(cluster_x[0]*cluster_x[k0]+cluster_y[0]*cluster_y[k0]+cluster_z[0]*cluster_z[k0]);
		  Double_t invMass = sqrt(2*highestenergy[0]*highestenergy[k0]*(1-cosTheta));
		  HISTinvariantMass->Fill(invMass);
		  HISTinvariantMass_openingAngle->Fill(invMass,acos(cosTheta)*180/3.1415926535);
		  HIST_Egamma1Egamma2->Fill(highestenergy[0]*highestenergy[k0]);// (E_gamma1)(E_gamma2)
		  HIST_Egamma1vsEgamma2->Fill(highestenergy[0],highestenergy[k0]);// E_gamma1 versus E_gamma2
		  HIST_Egamma1Egamma2_openingAngle->Fill(highestenergy[0]*highestenergy[k0],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_Egamma1Egamma2->Fill(invMass,highestenergy[0]*highestenergy[k0]);
		  HIST_Egamma1_Egamma1Egamma2->Fill(highestenergy[0],highestenergy[0]*highestenergy[k0]);
		  HIST_Egamma1_openingAngle->Fill(highestenergy[0],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_openingAngle_Egamma1Egamma2->Fill(invMass,acos(cosTheta)*180/3.1415926535,highestenergy[0]*highestenergy[k0]);
		  }
	        }
	      for (int k1=2; k1<NumberOfClusters_noModule5; k1++)
	        {
		  //if (cluster_module[1] != cluster_module[k1])
		  {
		  Double_t cosTheta = (1./cluster_dist[1]/cluster_dist[k1])*(cluster_x[1]*cluster_x[k1]+cluster_y[1]*cluster_y[k1]+cluster_z[1]*cluster_z[k1]);
		  Double_t invMass = sqrt(2*highestenergy[1]*highestenergy[k1]*(1-cosTheta));
		  HISTinvariantMass->Fill(invMass);
		  HISTinvariantMass_openingAngle->Fill(invMass,acos(cosTheta)*180/3.1415926535);
		  HIST_Egamma1Egamma2->Fill(highestenergy[1]*highestenergy[k1]);// (E_gamma1)(E_gamma2)
		  HIST_Egamma1vsEgamma2->Fill(highestenergy[1],highestenergy[k1]);// E_gamma1 versus E_gamma2
		  HIST_Egamma1Egamma2_openingAngle->Fill(highestenergy[1]*highestenergy[k1],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_Egamma1Egamma2->Fill(invMass,highestenergy[1]*highestenergy[k1]);
		  HIST_Egamma1_Egamma1Egamma2->Fill(highestenergy[1],highestenergy[1]*highestenergy[k1]);
		  HIST_Egamma1_openingAngle->Fill(highestenergy[1],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_openingAngle_Egamma1Egamma2->Fill(invMass,acos(cosTheta)*180/3.1415926535,highestenergy[1]*highestenergy[k1]);
		  }
	        }
	      for (int k2=3; k2<NumberOfClusters_noModule5; k2++)
	        {
		  //if (cluster_module[2] != cluster_module[k2])
		  {
		  Double_t cosTheta = (1./cluster_dist[2]/cluster_dist[k2])*(cluster_x[2]*cluster_x[k2]+cluster_y[2]*cluster_y[k2]+cluster_z[2]*cluster_z[k2]);
		  Double_t invMass = sqrt(2*highestenergy[2]*highestenergy[k2]*(1-cosTheta));
		  HISTinvariantMass->Fill(invMass);
		  HISTinvariantMass_openingAngle->Fill(invMass,acos(cosTheta)*180/3.1415926535);
		  HIST_Egamma1Egamma2->Fill(highestenergy[2]*highestenergy[k2]);// (E_gamma1)(E_gamma2)
		  HIST_Egamma1vsEgamma2->Fill(highestenergy[2],highestenergy[k2]);// E_gamma1 versus E_gamma2
		  HIST_Egamma1Egamma2_openingAngle->Fill(highestenergy[2]*highestenergy[k2],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_Egamma1Egamma2->Fill(invMass,highestenergy[2]*highestenergy[k2]);
		  HIST_Egamma1_Egamma1Egamma2->Fill(highestenergy[2],highestenergy[2]*highestenergy[k2]);
		  HIST_Egamma1_openingAngle->Fill(highestenergy[2],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_openingAngle_Egamma1Egamma2->Fill(invMass,acos(cosTheta)*180/3.1415926535,highestenergy[2]*highestenergy[k2]);
		  }
		}
	      for (int k3=4; k3<NumberOfClusters_noModule5; k3++)
	        {
		  //if (cluster_module[3] != cluster_module[k3])
		  {
		  Double_t cosTheta = (1./cluster_dist[3]/cluster_dist[k3])*(cluster_x[3]*cluster_x[k3]+cluster_y[3]*cluster_y[k3]+cluster_z[3]*cluster_z[k3]);
		  Double_t invMass = sqrt(2*highestenergy[3]*highestenergy[k3]*(1-cosTheta));
		  HISTinvariantMass->Fill(invMass);
		  HISTinvariantMass_openingAngle->Fill(invMass,acos(cosTheta)*180/3.1415926535);
		  HIST_Egamma1Egamma2->Fill(highestenergy[3]*highestenergy[k3]);// (E_gamma1)(E_gamma2)
		  HIST_Egamma1vsEgamma2->Fill(highestenergy[3],highestenergy[k3]);// E_gamma1 versus E_gamma2
		  HIST_Egamma1Egamma2_openingAngle->Fill(highestenergy[3]*highestenergy[k3],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_Egamma1Egamma2->Fill(invMass,highestenergy[3]*highestenergy[k3]);
		  HIST_Egamma1_Egamma1Egamma2->Fill(highestenergy[3],highestenergy[3]*highestenergy[k3]);
		  HIST_Egamma1_openingAngle->Fill(highestenergy[3],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_openingAngle_Egamma1Egamma2->Fill(invMass,acos(cosTheta)*180/3.1415926535,highestenergy[3]*highestenergy[k3]);
		  }
		}
	      for (int k4=5; k4<NumberOfClusters_noModule5; k4++)
	        {
		  //if (cluster_module[4] != cluster_module[k4])
		  {
		  Double_t cosTheta = (1./cluster_dist[4]/cluster_dist[k4])*(cluster_x[4]*cluster_x[k4]+cluster_y[4]*cluster_y[k4]+cluster_z[4]*cluster_z[k4]);
		  Double_t invMass = sqrt(2*highestenergy[4]*highestenergy[k4]*(1-cosTheta));
		  HISTinvariantMass->Fill(invMass);
		  HISTinvariantMass_openingAngle->Fill(invMass,acos(cosTheta)*180/3.1415926535);
		  HIST_Egamma1Egamma2->Fill(highestenergy[4]*highestenergy[k4]);// (E_gamma1)(E_gamma2)
		  HIST_Egamma1vsEgamma2->Fill(highestenergy[4],highestenergy[k4]);// E_gamma1 versus E_gamma2
		  HIST_Egamma1Egamma2_openingAngle->Fill(highestenergy[4]*highestenergy[k4],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_Egamma1Egamma2->Fill(invMass,highestenergy[4]*highestenergy[k4]);
		  HIST_Egamma1_Egamma1Egamma2->Fill(highestenergy[4],highestenergy[4]*highestenergy[k4]);
		  HIST_Egamma1_openingAngle->Fill(highestenergy[4],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_openingAngle_Egamma1Egamma2->Fill(invMass,acos(cosTheta)*180/3.1415926535,highestenergy[4]*highestenergy[k4]);
		  }
		}
	      for (int k5=6; k5<NumberOfClusters_noModule5; k5++)
	        {
		  //if (cluster_module[5] != cluster_module[k5])
		  {
		  Double_t cosTheta = (1./cluster_dist[5]/cluster_dist[k5])*(cluster_x[5]*cluster_x[k5]+cluster_y[5]*cluster_y[k5]+cluster_z[5]*cluster_z[k5]);
		  Double_t invMass = sqrt(2*highestenergy[5]*highestenergy[k5]*(1-cosTheta));
		  HISTinvariantMass->Fill(invMass);
		  HISTinvariantMass_openingAngle->Fill(invMass,acos(cosTheta)*180/3.1415926535);
		  HIST_Egamma1Egamma2->Fill(highestenergy[5]*highestenergy[k5]);// (E_gamma1)(E_gamma2)
		  HIST_Egamma1vsEgamma2->Fill(highestenergy[5],highestenergy[k5]);// E_gamma1 versus E_gamma2
		  HIST_Egamma1Egamma2_openingAngle->Fill(highestenergy[5]*highestenergy[k5],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_Egamma1Egamma2->Fill(invMass,highestenergy[5]*highestenergy[k5]);
		  HIST_Egamma1_Egamma1Egamma2->Fill(highestenergy[5],highestenergy[5]*highestenergy[k5]);
		  HIST_Egamma1_openingAngle->Fill(highestenergy[5],acos(cosTheta)*180/3.1415926535);
		  HISTinvariantMass_openingAngle_Egamma1Egamma2->Fill(invMass,acos(cosTheta)*180/3.1415926535,highestenergy[5]*highestenergy[k5]);
		  }
		}
	      }
	  }

       //hz_clust->Fill(cluster->z());

        if (NumberOfClusters>0)
	  HISTnumberOfClusters->Fill(NumberOfClusters);                         //number of clusters per event, having more than or equal to one digi per cluster
	if (ClustNo>0)
	  HISTclustNo->Fill(ClustNo);                                           //number of clusters per event, having more than or equal to one digi per cluster and satisfying the cluster threshold energy
	if (ndigisOfHighestEnergyCluster>0)
	  HISTndigisOfHighestEnergyCluster->Fill(ndigisOfHighestEnergyCluster); //number of digis per event for clusters with highest energy deposition

  	HISTtotdigis_fromClusterAnalysis->Fill(totdigis_fromClusterAnalysis);

	if (j%1000 ==0)
	  cout<<"Evt. No. "<< j<<endl;
   }
   //cout<<"totaldigis = "<<totaldigis<<"\n";
   //cout<<"efficiencyCounter_d = "<<efficiencyCounter_d<<" , atLeastOneHitPerEventCounter_d = "<<atLeastOneHitPerEventCounter_d<<"\n";
   //cout<<"FwEndCap efficiency (more than 10 MeV deposition in at least one crystal) = "<< (double)efficiencyCounter_d/atLeastOneHitPerEventCounter_d<<"\n";
   cout<<"totClusters = "<<totClusters<<endl;
////////////////////////////////////////////////////////////////////////// DRAWING
  gStyle->SetPalette(1);



/*
  TCanvas* c3 = new TCanvas("c3","", 100, 100, 800, 800);
  c3->Divide(3,2);
  c3->cd(1);
  HISTOdigi_ene11_12->Draw();
  c3->cd(2);
  HISTOdigi_ene12_12->Draw();
  c3->cd(3);
  HISTOdigi_ene13_12->Draw();
  c3->cd(4);
  HISTOdigi_ene14_12->Draw();
  c3->cd(5);
  HISTene_d->Draw();
  HISTene_d->GetXaxis()->SetTitle("FwEndCap Deposited Energy per digi (digi analysis)");
  c3->cd(6);
  HISTdigi_ene->SetLineColor(1);
  HISTdigi_ene->Draw();
  HISTdigi_ene->GetXaxis()->SetTitle("FwEndCap Deposited Energy per event (digi analysis)");
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
  TCanvas* c8 = new TCanvas("c8","", 100, 100, 1200, 800);
  c8->Divide(2,2);
  c8->cd(1);
   HISTmultiplicity_5MeVtrigThresh_d->Draw();
  c8->cd(2);
   HISTmultiplicity_10MeVtrigThresh_d->Draw();
*/

   TCanvas* c10 = new TCanvas("c10","", 100, 100, 1200, 800);
   c10->Divide(2,4);
   c10->cd(1)->SetLogy();
   HISTinvariantMass->Draw();
   c10->cd(2)->SetLogy();
   HIST_Egamma1Egamma2->Draw();
   c10->cd(3)->SetLogz();
   HIST_Egamma1vsEgamma2->Draw("colz");
   c10->cd(4)->SetLogz();
   HISTinvariantMass_openingAngle->Draw("colz");
   c10->cd(5)->SetLogz();
   HIST_Egamma1Egamma2_openingAngle->Draw("colz");
   c10->cd(6)->SetLogz();
   HIST_Egamma1_openingAngle->Draw("colz");
   c10->cd(7)->SetLogz();
   HIST_Egamma1_Egamma1Egamma2->Draw("colz");
   c10->cd(8)->SetLogz();
   HISTinvariantMass_Egamma1Egamma2->Draw("colz");

   TCanvas* c100 = new TCanvas("c100","", 100, 100, 1200, 800);
   HISTinvariantMass_openingAngle_Egamma1Egamma2->Draw("Box");
   HISTinvariantMass_openingAngle_Egamma1Egamma2->GetXaxis()->SetTitle("m_{#gamma#gamma}");
   HISTinvariantMass_openingAngle_Egamma1Egamma2->GetXaxis()->SetTitleOffset(1.5);
   HISTinvariantMass_openingAngle_Egamma1Egamma2->GetYaxis()->SetTitle("opening angle");
   HISTinvariantMass_openingAngle_Egamma1Egamma2->GetYaxis()->SetTitleOffset(1.5);
   HISTinvariantMass_openingAngle_Egamma1Egamma2->GetZaxis()->SetTitle("E_{#gamma1} #times E_{#gamma2}");
   HISTinvariantMass_openingAngle_Egamma1Egamma2->GetZaxis()->SetTitleOffset(1.1);


   TCanvas* c11 = new TCanvas("c11","", 100, 100, 1200, 800);
   c11->SetLogy();
   moduleDistribution->Draw();

//////////////////////////////////////////////////////////////////////////writing into a root file:
   TString outfile= "./invMass_histo_allGammasAnalysis_FullEmcIncluded_GetEnergyCorrected.root";
   TFile* file_out = new TFile(outfile,"RECREATE");

   HISTinvariantMass->Write();
   HISTinvariantMass_openingAngle->Write();
   HIST_Egamma1Egamma2->Write();
   HIST_Egamma1vsEgamma2->Write();
   HIST_Egamma1Egamma2_openingAngle->Write();
   HISTinvariantMass_Egamma1Egamma2->Write();
   HIST_Egamma1_Egamma1Egamma2->Write();
   HIST_Egamma1_openingAngle->Write();
   HISTinvariantMass_openingAngle_Egamma1Egamma2->Write();

   file_out->Close();

   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}
