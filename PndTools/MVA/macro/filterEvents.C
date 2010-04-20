/*
 * Filter Monte Carlo events for a given pdg code, if the particle has
 * been stable up to the first hit on the EMC detector.
*/
void filterEvents(int pid  = 211)
{
  int counts = 0;
  TStopwatch timer;
  timer.Start();
  gDebug = 0;

  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  rootlogon();
  basiclibs();

  PndEmcMapper::Init(6);

  TFile sF("points_sttcombi.root");
  TTree* tsim = (TTree *) sF.Get("cbmsim");

  TFile pF("params_sttcombi.root");

  TClonesArray* trackList = new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack",&trackList);

  TClonesArray* pointList = new TClonesArray("PndEmcPoint");
  tsim->SetBranchAddress("EmcPoint",&pointList);
  
  for (int j = 0; j < tsim->GetEntriesFast(); j++){
    tsim->GetEntry(j);
    
    //std::cout << "Number of tracks = " << trackList->GetEntriesFast() << std::endl;
    //std::cout << "Number of points = " << pointList->GetEntriesFast() << std::endl;
    PndEmcPoint* pt   = (PndEmcPoint*)pointList->At(0);
    
    if(pt){
      int trID = pt->GetTrackID();
      PndMCTrack* track = (PndMCTrack*) trackList->At(trID);

      if(track->GetPdgCode() == pid){
	counts++;
      }
      else{
	std::cout << "Track with wrong pdg. "<<std::endl;
	std::cout << "Track ID = " << trID << std::endl;
	std::cout << "Track with pdg = " << track->GetPdgCode() << std::endl;
      }
    }
    else{
      std::cout << "No point on the EMC." << std::endl;
    }
  }
  std::cout << " Total number of events = " << tsim->GetEntriesFast()
	    << " No decay = " << counts << std::endl;
  // Clean-up
  sF.Close();
  pF.Close();
  exit(1);
}
