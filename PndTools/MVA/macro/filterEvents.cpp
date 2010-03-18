#include "filterEvents.h"

using namespace std;

std::vector<int> EvtIds;

int filterEvents(int pid)
{
  int counts = 0;
  TStopwatch timer;
  timer.Start();
  cout << "Count = " << counts << " pid = " << pid << endl;

  TFile sF("points_sttcombi.root");
  TTree* tsim = (TTree *) sF.Get("cbmsim");

  TClonesArray* trackList = new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack",&trackList);

  TClonesArray* pointList = new TClonesArray("PndEmcPoint");
  tsim->SetBranchAddress("EmcPoint",&pointList);

  TFile pF("params1_sttcombi.root");

  TFile digiF("digi_sttcombi.root");
  TTree* digiTr = (TTree *) digiF.Get("cbmsim");
  TClonesArray* clusters = new TClonesArray("PndEmcCluster");  
  digiTr->SetBranchAddress("EmcCluster",&clusters);

  TFile recoF("reco_sttcombi.root");
  TTree* RecoTr = (TTree *) recoF.Get("cbmsim");

  PndEmcMapper::Init(6);
  // Loop through the simulation data.
  for (int j = 0; j < tsim->GetEntriesFast(); j++){
    tsim->GetEntry(j);
    
    std::cout << "<INFO> proc eventNum " << j << std::endl;
    //Select the first interaction point.
    PndEmcPoint* pt   = (PndEmcPoint*)pointList->At(0);
    
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
    else{// No emc interaction
      std::cout << "No point on the EMC." << std::endl;
    }
  }
  
  for(size_t i = 0; i < EvtIds.size(); i++){
    int evid = EvtIds[i];
    cout << "EVT ID = " << evid << endl;
    RecoTr->GetEntry(evid);
    digiTr->GetEntry(evid);
    std::cout << "number of clusters for current evt = "
	      << clusters->GetEntriesFast() << endl;
    //Select the right cluster.
    
  }

  std::cout << "<-I-> Total number of events = " << tsim->GetEntriesFast()
            << ": No decay = " << counts
            << ": No decay array size = " << EvtIds.size() << std::endl;
  cout << "Number of events in reco file = " << RecoTr->GetEntriesFast() << endl;
  //=========== Clean-up
  sF.Close();
  pF.Close();
  recoF.Close();

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
