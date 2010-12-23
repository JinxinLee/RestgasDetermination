#include "selectEvents.h"

using namespace std;

void selectEvents(int pdg,
		  const std::string& partName,
		  const std::string& paramFile  ,
		  const std::string& simFile    ,
		  const std::string& digiFile   ,
		  const std::string& recoFile   ,
		  const std::string& outFileName
		  )
{
  int counts = 0;
  TStopwatch timer;
  timer.Start();

  cout << "\tCount = " << counts << "\n\tpdg = " << pdg << '\n';
  
  // First = event id. Second = MC_moment
  std::vector < std::pair<int, float> > EvtIds;
  
  // EMC cluster energy, num.clusters, numb. crystals
  float emc, emcCorr, mom, MCmom, lheMom;
  
  // Selected Zernike moments and LATeral energydeposition
  float z20, z53, latEdep;
  int numClus, numCrys, numBumps;
  
  MCmom = lheMom = mom = emc = emcCorr = z20 = z53 = 0.00;
  numClus = numCrys = numBumps = 0;
  
  // N-Tuple to store the variables.
  //TNtuple EmcNtp (partName.c_str(), partName.c_str(),
  //	  "id:MCp:p:emc:emcOld:lat:z20:z53:numClus:numCrys:numBumps");
  TNtuple EmcNtp (partName.c_str(), partName.c_str(),
		  "MCp:p:emc:emcOld:lat:z20:z53:numClus:numCrys:numBumps");

  // Open Simulation file
  TFile sF(simFile.c_str());
  
  // Get simulation tree
  TTree* tsim = (TTree *) sF.Get("cbmsim");
  
  // Tracks.
  TClonesArray* trackList = new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack", &trackList);
  
  // Points
  TClonesArray* pointList = new TClonesArray("PndEmcPoint");
  tsim->SetBranchAddress("EmcPoint", &pointList);
  
  // Maybe it can be done better (Cleaner).
  ////////////////////////////
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile.c_str());
  
  // Dummy output, containes nothing
  fRun->SetOutputFile("dummy_out.root");
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(paramFile.c_str());
  rtdb->setFirstInput(parInput1);
  
  PndEmcGeoPar *geoPar = (PndEmcGeoPar*) rtdb->getContainer("PndEmcGeoPar");
  fRun->Init();
  geoPar->InitEmcMapper();
  PndEmcMapper::Init(6);
  ////////////////////////////
  
  // Open Digi file
  TFile digiF(digiFile.c_str());
  
  // Digi tree
  TTree* digiTr = (TTree *) digiF.Get("cbmsim");
  
  // CLusters
  TClonesArray* clusters_arr = new TClonesArray("PndEmcCluster");  
  digiTr->SetBranchAddress("EmcCluster", &clusters_arr);
  
  // Open Reco file
  TFile recoF(recoFile.c_str());
  // Reco tree
  TTree* RecoTr = (TTree *) recoF.Get("cbmsim");
  
  // Track list
  TClonesArray* recTrakArr = new TClonesArray("PndTrack");
  RecoTr->SetBranchAddress("SttMvdGenTrack", &recTrakArr);
  //TClonesArray* recTrakArr = new TClonesArray("PndTrack");
  //RecoTr->SetBranchAddress("LheTrack", &recTrakArr);
  //RecoTr->SetBranchAddress("LheGenTrackPion", &recTrakArr);
  
  // Loop through the simulation data.
  std::cout << "<INFO> Selecting events." << '\n';

  for (int j = 0; j < tsim->GetEntriesFast(); j++)
  {
    tsim->GetEntry(j);

    // Select the first interaction point.
    PndEmcPoint* pt   = (PndEmcPoint*) pointList->At(0);
    
    if(pt)
    {
      int trID = pt->GetTrackID();
      PndMCTrack* track = (PndMCTrack*) trackList->At(trID);
      
      // Select events with the correct pdg code.
      if( track->GetPdgCode() == pdg )
      {
        counts++;
	// First = event Index, Second = MC Moment
	EvtIds.push_back( std::make_pair(j, (track->GetMomentum()).Mag()) );
      }
      else{//Decay ????
        std::cout << "<Wrong pdg> :"
		  << " Track ID = " << trID 
		  << " Track has pdg = " << track->GetPdgCode()
		  << '\n';
      }
    }
    else
    {// No emc interaction??
      std::cout << "No point on the EMC.\n";
    }
  }
  
  std::cout << "<INFO> Processing selected events.\n";
  
  // Loop through the selected events.
  for(size_t i = 0; i < EvtIds.size(); i++)
  {
    int evid = EvtIds[i].first;// Event index
    cout << "Selected Event no = " << evid << " ";

    tsim->GetEntry(evid);
    digiTr->GetEntry(evid);
    RecoTr->GetEntry(evid);
    
    PndTrack* tra = (PndTrack*) recTrakArr->At(0);
    
    if(tra)
    {
      // Charged or correct reconstructed.
      FairTrackParP par = tra->GetParamLast();
      
      std::cout << "Number of clusters = "
		<< clusters_arr->GetEntriesFast()
		<<" Number of tracks = "<< recTrakArr->GetEntriesFast()
		<< " With P = "<< par.GetMomentum().Mag()
		<< " MC_p = "  << EvtIds[i].second
		<< '\n';
      
      mom = par.GetMomentum().Mag();// Computed moment
      MCmom = EvtIds[i].second;// MC moment
    }// End if(tra)
    else
    {//Neutral or not correctly reconstructed.
      cout << "<Empty track>: number of clusters = "
	   << clusters_arr->GetEntriesFast() << '\n';
    }
    
    // Select the right cluster(highest E_dep).
    double maxEnergy = std::numeric_limits <double>::min();
    int clIndex      = -1;
    
    // Loop through the clusters.
    for(int cl = 0; cl < clusters_arr->GetEntriesFast(); cl++)
    {
      PndEmcCluster* clust = (PndEmcCluster*) clusters_arr->At(cl);
      
      if(clust->GetEnergyCorrected() > maxEnergy)
      {
	maxEnergy = clust->GetEnergyCorrected();
	clIndex = cl;
      }
    }
    
    // Found the cluster with highest E_dep.
    if( clIndex >= 0 )
    {
      PndEmcCluster* HE_cluster = (PndEmcCluster*) clusters_arr->At(clIndex);
      
      emc     = HE_cluster->energy();
      emcCorr = HE_cluster->GetEnergyCorrected();

      numClus  = clusters_arr->GetEntriesFast();
      numCrys  = HE_cluster->NumberOfDigis();
      numBumps = HE_cluster->NBumps();
      
      //z20 = clsZmom.AbsZernikeMoment(2, 0, 15);// Z_{n = 2}^{m = 0}
      z20 = HE_cluster->Z20();
      
      //z53 = clsZmom.AbsZernikeMoment(5, 3, 15);// Z_{n = 5}^{m = 3}
      z53 = HE_cluster->Z53();
      
      //latEdep = clsZmom.Lat();
      latEdep = HE_cluster->LatMom();
      
      // Fill tree (NTuple)
      if(tra->GetFlag() > 0)// If something wrong happened during the fitting
      {
	EmcNtp.Fill(MCmom, mom, emcCorr, emc, latEdep, z20, z53, numClus, numCrys, numBumps);

	std::cout << "Selected Cluster index = "<< clIndex 
		  << " emcOld = "  << emc
		  << " emcCorr = " << emcCorr
		  << " Track Flag = " << tra->GetFlag()
		  << '\n';
      }
    }
  }
  
  std::cout << "========================================================\n"
	    << "<INFO>\nTotal number of events = " << tsim->GetEntriesFast()
            << "\nNo decay = " << counts
            << "\nNo decay array size = " << EvtIds.size()
	    << "\nIn Ntuple " << EmcNtp.GetEntriesFast()
	    << "\nNumber of events in reco file = " << RecoTr->GetEntriesFast() 
	    << "\n========================================================\n";
  
  std::cout << "<INFO> Writing output to: " << outFileName << '\n';
  
  // Write to the output
  TFile out(outFileName.c_str(), "RECREATE","Selected evets File.", 9);

  //EmcNtp.Print();
  EmcNtp.Write();
  out.Close();
  
  //=========== Clean-up
  EvtIds.clear();
  sF.Close();
  digiF.Close();
  recoF.Close();
  
  delete clusters_arr;
  delete trackList;
  delete pointList;
  delete recTrakArr;
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout <<"=======================================\n"
       << "Macro finished succesfully.\n"
       << "Real time " << rtime << " s, CPU time " << ctime << " s.\n\n";
  // ------------------------------------------------------------------------
  exit(0);
}
