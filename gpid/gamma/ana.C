/*
#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include "TNtuple.h"
#include "PndEmcMapper.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TCanvas.h"
#include <vector> 
*/

void ana( )
{
  // Macro loads a file after reconstruction and plots difference between initial direction of particle and angular position of cluster
  
        
        TFile* f = new TFile("reco_sttcombi.root"); //file you want to analyse
        TTree *t=(TTree *) f->Get("cbmsim") ;
        TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
        t->SetBranchAddress("EmcCluster",&cluster_array);

        TFile* fsim = new TFile("sim_emc.root"); //file you want to analyse
        TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
        PndEmcMapper *emcMap=PndEmcMapper::Instance(2);
        std::vector<PndEmcDigi*> list;

 TNtuple *tube = new TNtuple("pion","pion","mass:E1:E2:E3:E4:E5");


        for (Int_t j=0; j< t->GetEntriesFast(); j++)
        {
                t->GetEntry(j);

                std::vector<PndEmcDigi*>::iterator digiIt;
		std::vector<float> energyVec;
		std::vector<float>::iterator it;
		float val[10];
                for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
                {
                        PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
                        if ( cluster == NULL) continue;
                        if ( cluster->energy() < 2.0 )continue;
                        list = cluster->DigiList();
			val[0] = cluster->Mass();
			for ( int k = 0 ; k < list.size(); k++ )
			{
			energyVec.push_back(list[k]->GetEnergy()); 
			}
			sort(energyVec.begin(), energyVec.end());
			float Esum = energyVec.back();
			val[1] = Esum/cluster->energy();
                        energyVec.pop_back();
			Esum += energyVec.back();
			val[2] = Esum/cluster->energy();
                        energyVec.pop_back();
			Esum += energyVec.back();
			val[3] = Esum/cluster->energy();
                        energyVec.pop_back();
			Esum += energyVec.back();
			val[4] = Esum/cluster->energy();
                        tube->Fill(val);
                }
		list.clear();
        }
	TFile *out = new TFile("pid_tree.root","RECREATE");
	out->WriteObject(ntuple,"pidntuple");

}


