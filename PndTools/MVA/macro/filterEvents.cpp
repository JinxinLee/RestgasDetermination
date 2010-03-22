#include "filterEvents.h"

using namespace std;

std::vector<int> EvtIds;

int filterEvents(int pid, const std::string outFileName)
{
  int counts = 0;
  TStopwatch timer;
  timer.Start();
  cout << "Count = " << counts << " pid = " << pid << endl;


  //EMC cluster energy, num.clusters, numb. crystals
  float emc, emcCorr, mom; 
  float z20, z53;//Selected Zernike moments
  int numClus, numCrys;
  mom = emc = emcCorr = z20 = z53 = 0.00;
  numClus = numCrys = 0;


  TNtuple EmcNtp ("pion","pion","p:emc:emcCorr:numClus:numCrys:z20:z53");

  TFile sF("points_sttcombi.root");
  TTree* tsim = (TTree *) sF.Get("cbmsim");

  TClonesArray* trackList = new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack", &trackList);

  TClonesArray* pointList = new TClonesArray("PndEmcPoint");
  tsim->SetBranchAddress("EmcPoint", &pointList);

  TFile pF("params1_sttcombi.root");

  TFile digiF("digi_sttcombi.root");
  TTree* digiTr = (TTree *) digiF.Get("cbmsim");
  TClonesArray* clusters_arr = new TClonesArray("PndEmcCluster");  
  digiTr->SetBranchAddress("EmcCluster", &clusters_arr);

  TFile recoF("reco_sttcombi.root");
  TTree* RecoTr = (TTree *) recoF.Get("cbmsim");
  
  TClonesArray* recTrakArr = new TClonesArray("PndTrack");  
  RecoTr->SetBranchAddress("LheGenTrackPion", &recTrakArr);

  PndEmcMapper::Init(6);

  // Loop through the simulation data.
  for (int j = 0; j < tsim->GetEntriesFast(); j++){
    tsim->GetEntry(j);
    
    std::cout << "<INFO> proc eventNum " << j << std::endl;
    //Select the first interaction point.
    PndEmcPoint* pt   = (PndEmcPoint*) pointList->At(0);
    
    if(pt){
      int trID = pt->GetTrackID();
      PndMCTrack* track = (PndMCTrack*) trackList->At(trID);
      // Select which event has the correct pdg code. When hitting the
      // first emc detector.
      if(track->GetPdgCode() == pid){
        counts++;
	EvtIds.push_back(j);
      }
      else{//Decay ????
        std::cout << "Track with wrong pdg. "<<std::endl;
        std::cout << "Track ID = " << trID << std::endl;
        std::cout << "Track with pdg = " << track->GetPdgCode() << std::endl;
      }
    }
    else{// No emc interaction??
      std::cout << "No point on the EMC." << std::endl;
    }
  }

  // Loop through the selected events.  
  for(size_t i = 0; i < EvtIds.size(); i++){
    int evid = EvtIds[i];
    cout << "EVT ID = " << evid << endl;
    RecoTr->GetEntry(evid);
    digiTr->GetEntry(evid);

    PndTrack* tra = (PndTrack*) recTrakArr->At(0);
    
    if(tra){  // Charged or correct reconstructed.
      FairTrackParP par = tra->GetParamLast();
      std::cout << "number of clusters for current evt = "
		<< clusters_arr->GetEntriesFast()
		<<" number of tracks = "<< recTrakArr->GetEntriesFast() 
		<< " with P = "<< par.GetMomentum().Mag() << endl;
      //Select the right cluster(highest E_dep).
      double maxEnergy = -1.0;
      int clIndex = 0;
      // Loop through the clusters.
      for(int cl = 0; cl < clusters_arr->GetEntriesFast(); cl++){
	PndEmcCluster* clust = (PndEmcCluster*) clusters_arr->At(cl);
	if(clust->GetEnergyCorrected() > maxEnergy){
	  maxEnergy = clust->GetEnergyCorrected();
	  clIndex = cl;
	}
      }
      // Found the cluster with highest E_dep.
      PndEmcCluster* HE_cluster = (PndEmcCluster*) clusters_arr->At(clIndex);
      PndEmcXClMoments clsZmom = HE_cluster->Xmoments();
      numClus = clusters_arr->GetEntriesFast();
      emc = HE_cluster->energy(); 
      emcCorr = HE_cluster->GetEnergyCorrected();  
      numCrys = HE_cluster->NumberOfDigis();
      //z20 = clsZmom.AbsZernikeMoment(2, 0, 15);// Z_{n = 2}^{m = 0}
      //z53 = clsZmom.AbsZernikeMoment(5, 3, 15);// Z_{n = 5}^{m = 3}
      mom = par.GetMomentum().Mag();
      //  TNtuple EmcNtp ("pion","pion","p:emc:emcCorr:numClus:numCrys:z20:z53");
      EmcNtp.Fill(mom, (emc/mom), (emcCorr/mom), numClus, numCrys, z20, z53);
    }// End if(tra)
    else{//Neutral or not correctly reconstructed.
      cout << "empty track" << endl;
      std::cout << "number of clusters for current evt = "
		<< clusters_arr->GetEntriesFast() << endl;
    }
  }
  std::cout << "<-I-> Total number of events = " << tsim->GetEntriesFast()
            << ": No decay = " << counts
            << ": No decay array size = " << EvtIds.size() << std::endl;
  cout << "Number of events in reco file = " << RecoTr->GetEntriesFast() << endl;

  cout << "<INFO> Writing output to: " << outFileName << endl;

  TFile out(outFileName.c_str(),"RECREATE");
  EmcNtp.Print();
  EmcNtp.Write();
  //out.Write();
  out.Close();
  //=========== Clean-up
  sF.Close();
  pF.Close();
  recoF.Close();
  EvtIds.clear();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout <<"=======================================" << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

  return 0;
}
