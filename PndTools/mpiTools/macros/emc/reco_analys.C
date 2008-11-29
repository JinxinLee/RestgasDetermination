#include <iostream>
#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TGeoManager.h"
#include "CbmMCTrack.h"
#include "PndEmcHit.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcMapper.h"
#include "PndEmcWaveform.h"

void reco_analys(Char_t InputSimFile[]="sim_emc.root",
		 Char_t InputClusterFile[]="full_emc.root",
		 Char_t OutputFile[]="output.root")
{
        TFile *f=new TFile(InputSimFile);
	TGeoManager *fGeoManager = (TGeoManager *) f->Get("CBMGeom");

	TChain *c=new TChain("cbmsim");
	TChain *csim=new TChain("cbmsim");

	csim->Add(InputSimFile);
	c->Add(InputClusterFile);

	TClonesArray* track_array=new TClonesArray("CbmMCTrack");
	csim->SetBranchAddress("MCTrack",&track_array);

	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
	TClonesArray* wf_array=new TClonesArray("PndEmcWaveform");
	c->SetBranchAddress("EmcCluster",&cluster_array);
	c->SetBranchAddress("EmcWaveform",&wf_array);
	
	TFile *fs=new TFile(OutputFile,"recreate");
	TNtuple *n = new TNtuple("myntup","My Ntuple","et:tht:pht:ec:thc:phc:nc:sc");

	cout << "<I> Number of entries " << c->GetEntries() << endl;

	if (c->GetEntries()!=csim->GetEntries())
	  {
	    cout << "<E> Fatal error: the number of events in simulation and cluster root-files are not equal!" << endl;
	    exit(-1);
	  }

	double cluster_energy;
	double cluster_theta, cluster_phi; //position of the cluster
	int ndigi;
       
	PndEmcMapper *fEmcMap=PndEmcMapper::Instance(1);

	// Cluster energy
	for (Int_t j=0; j< c->GetEntries(); j++)
	{
	  if (0 == (j%1000)) 
	    {
	      printf(".%i.",j);fflush(stdout);
	    }
		c->GetEntry(j);
		csim->GetEntry(j);

		CbmMCTrack *track=(CbmMCTrack*)track_array->At(0);
		TLorentzVector p4mom=track->Get4Momentum();

		/*
		cout << endl << "<I> Event number is                                               " << j << endl;
		cout << "<I> The total energy of the incident particle in this event is    " << p4mom.E() << "\t" << "GeV" << endl;
		cout << "<I> The kinetic energy of the incident particle in this event is  " << p4mom.E()-p4mom.M() << "\t" << "GeV" << endl;
		cout << "<I> The mass of the incident particle in this event is            " << p4mom.M() << "\t" << "GeV/c2" << endl;
		cout << "<I> The momentum of the incident particle in this event is        " << p4mom.P() << "\t" << "GeV/c" << endl;
		cout << "<I> The polar angle of the incident particle in this event is     " << (180./TMath::Pi())*p4mom.Theta() << "\t" << "Degrees" << endl;
		cout << "<I> The azimuthal angle of the incident particle in this event is " << (180./TMath::Pi())*p4mom.Phi() << "\t" << "Degrees" << endl;
		cout << "<I> Number of clusters found is                                   " << cluster_array->GetEntriesFast() << endl;
		*/
		if (cluster_array->GetEntriesFast()>0)
		  {
		    Int_t    idWithHighestEnergy = 0;
		    Double_t highestEnergy = -1.;
		    
		    // First find the cluster with the highest energy
		    
		    for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
		      {
			PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
			cluster_energy=cluster->energy();
			if (cluster_energy>highestEnergy)
			  {
			    idWithHighestEnergy = i;
			    highestEnergy = cluster_energy;
			  }
			
		      }

		// Lets analyze that cluster! 
		    
		    PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(idWithHighestEnergy);
		    std::vector<PndEmcDigi*> digiList=cluster->DigiList();
		    ndigi=digiList.size();

		    TVector3 cluster_pos=cluster->where();
		    cluster_theta=cluster_pos.Theta();
		    cluster_phi=cluster_pos.Phi();
		    cluster_energy=cluster->energy();

		    /*		    
		    cout << "<I>         E              = " << cluster_energy << endl;
		    cout << "<I>         Theta          = " << cluster_theta*(180/TMath::Pi()) << endl;
		    cout << "<I>         Phi            = " << cluster_phi*(180/TMath::Pi()) << endl;
		    cout << "<I>         Nr of crystals = " << ndigi << endl;
		    cout << "<I>         Dets (Edep): ";
		    */

		    n->Fill((Float_t) p4mom.E()-p4mom.M(),
			    (Float_t) (180./TMath::Pi())*p4mom.Theta(),
			    (Float_t) (180./TMath::Pi())*p4mom.Phi(),
			    (Float_t) cluster_energy,
			    (Float_t) cluster_theta*(180/TMath::Pi()),
			    (Float_t) cluster_phi*(180/TMath::Pi()),
			    (Float_t) cluster_array->GetEntriesFast(),
			    (Float_t) ndigi);


		  }
	}
	fs->Write();
	return;
}

