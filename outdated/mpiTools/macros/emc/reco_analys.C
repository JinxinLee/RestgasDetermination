#include <iostream>
#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TGeoManager.h"
#include "PndMCTrack.h"
#include "PndEmcHit.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcTwoCoordIndex.h"
//#include "PndEmcMapper.h"
#include "PndEmcWaveform.h"

void reco_analys(Char_t InputSimFile[]="sim_emc.root",
		 Char_t OutputFile[]="output.root")
{
        TFile *f=new TFile(InputSimFile);
	TGeoManager *fGeoManager = (TGeoManager *) f->Get("FAIRGeom");

	TChain *c=new TChain("pndsim");

	c->Add(InputSimFile);

	TClonesArray* track_array=new TClonesArray("PndMCTrack");
	c->SetBranchAddress("MCTrack",&track_array);

	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
	TClonesArray* wf_array=new TClonesArray("PndEmcWaveform");
	c->SetBranchAddress("EmcCluster",&cluster_array);
	c->SetBranchAddress("EmcWaveform",&wf_array);

	TClonesArray* digi_array=new TClonesArray("PndEmcDigi");
        c->SetBranchAddress("EmcDigi",&digi_array);

	TClonesArray* hit_array=new TClonesArray("PndEmcHit");
        c->SetBranchAddress("EmcHit",&hit_array);

	TFile *fs=new TFile(OutputFile,"recreate");
	TNtuple *n = new TNtuple("myntup","My Ntuple","et:tht:pht:ec:thc:phc:nc:sc:edbar:edfw:edbw:eh:ecc:thd:phd");

	cout << "<I> Number of entries " << c->GetEntries() << endl;

	double cluster_energy,cluster_energy_check,digi_low,digi_high;
	double cluster_theta, cluster_phi; //position of the cluster
	Double_t tot_digi_energy_barrel,tot_digi_energy_fwendcap,tot_digi_energy_bwendcap;
	Double_t tot_hit_energy;
	Double_t theta_digi,phi_digi;
	int ndigi;

//	PndEmcMapper *fEmcMap=PndEmcMapper::Instance(1);

	// Cluster energy
	for (Int_t j=0; j< c->GetEntries(); j++)
	{
	  if (0 == (j%1000))
	    {
	      printf(".%i.",j);fflush(stdout);
	    }
	  c->GetEntry(j);

		PndMCTrack *track=(PndMCTrack*)track_array->At(0);
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

	        tot_digi_energy_barrel=0;
	        tot_digi_energy_fwendcap=0;
	        tot_digi_energy_bwendcap=0;
	        tot_hit_energy=0;
	        for (Int_t i=0; i<digi_array->GetEntriesFast(); i++)
	         {
	          PndEmcDigi *digi=(PndEmcDigi*)digi_array->At(i);
	          Double_t digi_energy=digi->GetEnergy();
		  Int_t detid=digi->GetDetectorId()/100000000;
		  //cout << digi->GetDetectorId() << "/" << detid << endl;
	          if (detid==3)
		      {
			  tot_digi_energy_fwendcap+=digi_energy;
		      }
		  else if (detid==4)
		      {
			  tot_digi_energy_bwendcap+=digi_energy;
		      }
	          else
		      {
			  tot_digi_energy_barrel+=digi_energy;
		      }

	          }
	        for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
	         {
	          PndEmcHit *hit=(PndEmcHit*)hit_array->At(i);
	          Double_t hit_energy=hit->GetEnergy();
	          tot_hit_energy+=hit_energy;
	          }

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
/*
		    std::vector<PndEmcDigi*> digiList=cluster->DigiList();
		    ndigi=digiList.size();
		    ndigi=0;

		    cluster_energy_check=0;
		    digi_low=digiList[0]->GetEnergy();
		    digi_high=digiList[0]->GetEnergy();
		    Int_t digi_high_id=0;
		    for (Int_t k=0; k<ndigi; k++)
		    {
			cluster_energy_check+=digiList[k]->GetEnergy();

			if (digiList[k]->GetEnergy()<digi_low)
			{
			    digi_low=digiList[k]->GetEnergy();
			}
			if (digiList[k]->GetEnergy()>digi_high)
			{
			    digi_high=digiList[k]->GetEnergy();
			    digi_high_id=k;
			}
		    }

		    theta_digi=digiList[digi_high_id]->GetTheta();
		    phi_digi=digiList[digi_high_id]->GetPhi();
*/
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
			    (Float_t) ndigi,
			    (Float_t) tot_digi_energy_barrel,
			    (Float_t) tot_digi_energy_fwendcap,
			    (Float_t) tot_digi_energy_bwendcap,
			    (Float_t) tot_hit_energy,
			    (Float_t) cluster_energy_check,
			    (Float_t) theta_digi*(180/TMath::Pi()),
			    (Float_t) phi_digi*(180/TMath::Pi()));
		  }
	}
	fs->Write();
	return;
}

