#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "TF1.h"


int analysis_cluster_energyCorrection_Ntuple()
{
  TStopwatch timer;
  timer.Start();
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  //////////////////////////////////////////////
  TString inputfile("/home/moini/myPandaRoot/myCodes/ROOTfiles/FullEmc_isotropicPhotonGeneration/emc_complete_10to5000MeVphotons.root");
  TString outputfile("./energyCorrectionFactors_30MeVthreshold_cond2_Ntuple_10MillionEvt.root");
  //TString inputfile("/home/moini/myPandaRoot/myCodes/ROOTfiles/FwEndCapResolution_vacuum_noMagField/assuming150photonsperMeV/emc_complete_1GeVphotons.root");
  //TString outputfile("../seinsitivityToBinning/h_cAndEta_c/moduleDistribution_1GeVphotons_onlyFWendCap.root");
  //////////////////////////////////////////////
  TFile* fsim = new TFile(inputfile);

  TNtuple evtParams("EventParams", "Selected Event parameters","eMc:eCluster:ClustEnCorrFactor:clustTheta:clustPhi");

  TTree *tsim=(TTree *) fsim->Get("pndsim") ;
  TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack",&mctrack_array);

  TClonesArray* digi_array=new TClonesArray("PndEmcDigi");
  tsim->SetBranchAddress("EmcDigi",&digi_array);

  TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
  tsim->SetBranchAddress("EmcCluster",&cluster_array);

  //TClonesArray* bump_array=new TClonesArray("PndEmcBump");
  //tsim->SetBranchAddress("EmcBump",&bump_array);

  Double_t theta_mc, phi_mc, ene_mc;
  TVector3 photon_momentum;
  TLorentzVector p4mom;
  Double_t clusterThresholdEnergy=0.030;//in GeV

  Int_t ncounts = tsim->GetEntriesFast();
  cout << "Number of events = " << ncounts<< endl;
  //ncounts=200000; //to analyse specific number of events

  TH1I *HISTnumberOfClusters_thresholdPassed = new TH1I("HISTnumberOfClusters_thresholdPassed","Total number of clusters per event satisfying cluster threshold",140,0,70);
   TH1F *moduleDistribution_3MeVthreshold = new TH1F("moduleDistribution_3MeVthreshold","",10,0.25,5.25);
   TH1F *moduleDistribution_10MeVthreshold = new TH1F("moduleDistribution_10MeVthreshold","",10,0.25,5.25);
   TH1F *moduleDistribution_30MeVthreshold = new TH1F("moduleDistribution_30MeVthreshold","",10,0.25,5.25);

   Double_t clusterEnergyCorrectionFactor_max = 0;
   Double_t ene_mc_max = 0;

   for (int j = 0; j < ncounts; j++) {

     Double_t clusterEnergyCorrectionFactor = 0;
     float ClustEnergy = 0.0;
     float clustPhi, clustTheta;

     tsim->GetEntry(j);

	PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0);
	photon_momentum=mctrack->GetMomentum();
	theta_mc=photon_momentum.Theta()*(180./TMath::Pi());
	phi_mc=photon_momentum.Phi()*(180./TMath::Pi());
	p4mom=mctrack->Get4Momentum();
	ene_mc=p4mom.E();
////////////////////////////////////////////////////////////////////////// CLUSTER ANALYSIS
	Int_t NumberOfClusters, ClustNo, NumberOfClusters_thresholdPassed;
        NumberOfClusters=ClustNo=NumberOfClusters_thresholdPassed=0;   //set per event
	Int_t idClusterSatisfiedThresholdEnergy=-1;
	///Double_t highestenergy_sumOfclusters=0;     // (set per event)
	Double_t totalClusterEnergies = 0;

	std::vector <double> highestenergy;
	std::vector <double> originalClusterIndex;

	if (cluster_array->GetEntriesFast() > 0)
	  {
	      NumberOfClusters=cluster_array->GetEntriesFast();
	      std::vector <double> clusterEnergies;
	      std::vector <double> clusterEnergies_original;

	      for (Int_t i=0; i<NumberOfClusters; i++)                        //loop over all clusters of the present event under analysis (ndigi>=1)
		{
		  PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
		  Double_t cluster_energy=cluster->energy();
		  clusterEnergies.push_back(cluster_energy);                 //first push all the cluster energies into the vector; no sorting yet
		  clusterEnergies_original.push_back(cluster_energy);        //make another one, in order to keep track of indices
		  //////////////////////////////////////////////////////module distribution
		  if (cluster_energy >= 0.003)
		    moduleDistribution_3MeVthreshold->Fill(cluster->GetModule());
		  if (cluster_energy >= 0.010)
		    moduleDistribution_10MeVthreshold->Fill(cluster->GetModule());
		  if (cluster_energy >= 0.030)
		    moduleDistribution_30MeVthreshold->Fill(cluster->GetModule());
		  //////////////////////////////////////////////////////
		  if (cluster_energy >= clusterThresholdEnergy)
		  {
		    NumberOfClusters_thresholdPassed++;
		    totalClusterEnergies += cluster_energy;
		  }
		}

	      // Per-event analysis of all the clusters
      	      std::sort(clusterEnergies.begin(), clusterEnergies.end());   //now sort all the recognized clusters in the FwEndCap
              //std::reverse(clusterEnergies.begin(), clusterEnergies.end());

	      if (NumberOfClusters_thresholdPassed >= 1)    //finding at least one cluster satisfying threshold energy
	      {
	        for (int k=0; k<NumberOfClusters; k++)
	         {
		   highestenergy.push_back(clusterEnergies[(NumberOfClusters-1)-k]);
		   //cout << "decsendingly sorted energy of cluster # " << k+1 << " = " << highestenergy[k] << '\n';
		   ///highestenergy_sumOfclusters += highestenergy[k];
		   ///if (k==NumberOfClusters_thresholdPassed-1)                  //eventually write the summation of the cluster enegies
		     ///cout <<"highestenergy_sumOfclusters =  "<< highestenergy_sumOfclusters <<'\n'<<endl;

		   for (int i=0; i<NumberOfClusters; i++)//finding the original cluster index of the cluster
		     if (!(clusterEnergies_original[i]>highestenergy[k]) && !(clusterEnergies_original[i]<highestenergy[k]))//practically the equality sign
		       originalClusterIndex.push_back(i);


		   PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(originalClusterIndex[k]);
		   //(k==0 corresponds to the cluster with highest energy deposit)
		   if (k==0 && (totalClusterEnergies >= (ene_mc - 0.050)) && NumberOfClusters_thresholdPassed==1)//&& ((cluster->theta()*(180./TMath::Pi()))-theta_mc)<=5.
			//&& requiring the condition on the event energy (assuming 50 MeV tolerance on the energy) ----> cond1
			//&& requiring only one cluster per event satisfying the threshold ----> cond2
			//&& requiring a tolerance for the allowed difference between the theta angles of this cluster and the Monte Carlo angle ----> cond3
		   {
		    ClustEnergy = cluster->energy();
		    clusterEnergyCorrectionFactor = ene_mc/ClustEnergy;
		    if (clusterEnergyCorrectionFactor > clusterEnergyCorrectionFactor_max)
		     {
		      clusterEnergyCorrectionFactor_max = clusterEnergyCorrectionFactor;
		      ene_mc_max = ene_mc;
		     }
		    clustPhi = cluster->phi()*(180./TMath::Pi());
		    clustTheta = cluster->theta()*(180./TMath::Pi());

		    // Fill Ntuple
		    evtParams.Fill(ene_mc, ClustEnergy, clusterEnergyCorrectionFactor, clustTheta, clustPhi);

		   }
	         }
	      }
	  }

        if (NumberOfClusters_thresholdPassed>0)
	  HISTnumberOfClusters_thresholdPassed->Fill(NumberOfClusters_thresholdPassed);      //number of clusters per event, having more than or equal to one digi per cluster


	if (j%1000 ==0)
	  cout<<"Evt. No. "<< j<<endl;
   }
cout << "clusterEnergyCorrectionFactor_max= "<<clusterEnergyCorrectionFactor_max << " for the Monte Carlo energy of "<< ene_mc_max<< " GeV\n"<< endl;
  //++++++++++++++++++++++++++++++++++++++++

  TFile myoutPutFile(outputfile,"RECREATE","Woww", 9);
  HISTnumberOfClusters_thresholdPassed->Write();
   moduleDistribution_3MeVthreshold->Write();
   moduleDistribution_10MeVthreshold->Write();
   moduleDistribution_30MeVthreshold->Write();

  evtParams.Write();

  myoutPutFile.Close();


   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}

