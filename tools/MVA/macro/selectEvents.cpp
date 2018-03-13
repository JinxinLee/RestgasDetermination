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

  cout << "\tCount = " << counts
       << "\n\tpdg = " << pdg << '\n';

  // First = event id. Second = MC_moment
  std::vector < std::pair<int, float> > EvtIds;

  // EMC cluster energy, num.clusters, numb. crystals
  float emc, emcOld, mom, MCmom;

  // Selected Zernike moments and LATeral energydeposition
  float z20, z53, latEdep;

  int numClus, numCrys, numBumps;

  // Selected cluster properties.
  float E1, E9, E25, E1E9, E9E25;

  MCmom = mom = emc = emcOld = z20 = z53 = 0.0;
  E1 = E9 = E25 = E1E9 = E9E25 = 0.0;
  numClus = numCrys = numBumps = 0;

  // N-Tuple to store the variables.
  TNtuple EmcNtp (partName.c_str(), partName.c_str(),
		  "MCp:p:emc:emcOld:lat:z20:z53:numClus:numCrys:numBumps:E1:E9:E25:E1E9:E9E25");

  // Open Simulation file
  TFile sF(simFile.c_str());

  // Get simulation tree
  TTree* tsim = (TTree *) sF.Get("pndsim");

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

  PndEmcMapper::Init(1);

  ////////////////////////////

  // Open Digi file
  TFile digiF(digiFile.c_str());

  // Digi tree
  TTree* digiTr = (TTree *) digiF.Get("pndsim");

  // Digi Array
  TClonesArray* digi_arr = new TClonesArray("PndEmcDigi");
  digiTr->SetBranchAddress("EmcDigi", &digi_arr);

  // CLusters Array
  TClonesArray* clusters_arr = new TClonesArray("PndEmcCluster");
  digiTr->SetBranchAddress("EmcCluster", &clusters_arr);

  // Bumps Array
  // TClonesArray* bumps_arr = new TClonesArray("PndEmcBump");
  // digiTr->SetBranchAddress("EmcBump", &bumps_arr);

  // Open Reco file
  TFile recoF(recoFile.c_str());

  // Reco tree
  TTree* RecoTr = (TTree *) recoF.Get("pndsim");

  // Track list
  TClonesArray* recTrakArr = new TClonesArray("PndTrack");
  RecoTr->SetBranchAddress("SttMvdGemGenTrack", &recTrakArr);

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
      else{ // Decay ????
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
    mom = MCmom = -1.00;
    // Event index
    int evid = EvtIds[i].first;

    cout << "Selected Event no = " << evid << " ";

    tsim->GetEntry(evid);
    digiTr->GetEntry(evid);
    RecoTr->GetEntry(evid);

    // Track for the primary particle. Other indices correspond to
    // secondary ....
    PndTrack* tra = (PndTrack*) recTrakArr->At(0);

    if(tra)
    {
      // Charged or correct reconstructed.
      FairTrackParP par = tra->GetParamLast();

      mom = par.GetMomentum().Mag();// Computed (Reco.) moment
      MCmom = EvtIds[i].second;// MC moment

      std::cout << "Number of clusters = "
		<< clusters_arr->GetEntriesFast()
		<<" Number of tracks = "<< recTrakArr->GetEntriesFast()
		<< " With p = "<< mom
		<< " MC_p = "  << MCmom
		<< '\n';

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

	std::cout << " Number of involved digis = " << digi_arr->GetEntries()
		  << '\n';

	PndEmcClusterEnergySums Som ( (*HE_cluster), digi_arr);

	emcOld   = HE_cluster->energy();
	emc      = HE_cluster->GetEnergyCorrected();

	numClus  = clusters_arr->GetEntriesFast();
	numCrys  = HE_cluster->NumberOfDigis();
	numBumps = HE_cluster->NBumps();

	// Z_{n = 2}^{m = 0}
	z20 = HE_cluster->Z20();

	// Z_{n = 5}^{m = 3}
	z53 = HE_cluster->Z53();

	//lat Edep
	latEdep = HE_cluster->LatMom();

	E1    = Som.E1();
	E9    = Som.E9();
	E25   = Som.E25();
	E1E9  = Som.E1E9();
	E9E25 = Som.E9E25();

	// Fill tree (NTuple)
	// If something WENT wrong, during the fitting
	if( tra->GetFlag() > 0 )
	{
	  EmcNtp.Fill(MCmom, mom, emc, emcOld,
		      latEdep, z20, z53,
		      numClus, numCrys, numBumps,
		      E1, E9, E25, E1E9, E9E25);

	  std::cout << " Cluster index = "<< clIndex
		    << " emcOld = " << emcOld
		    << " emc = "    << emc
		    << " Track Flag = " << tra->GetFlag()
		    << " E1  = "    << Som.E1()
		    << " E9  = "    << Som.E9()
		    << " E25 = "    << Som.E25()
		    << " E1E9 = "   << Som.E1E9()
		    << " E9E25 = "  << Som.E9E25()
		    << '\n';
	}// IF(tra->GetFlag() > 0)
      }// IF(clIndex >= 0)
    }// End if(tra)
    else
    {//Neutral or not correctly reconstructed.
      cout << "<Empty track>: number of clusters = "
	   << clusters_arr->GetEntriesFast() << '\n';
    }
  }//Selected Event loop

  std::cout << "========================================================\n"
	    << "<INFO>\nTotal number of events = " << tsim->GetEntriesFast()
	    << "\nPdg = " << pdg << " Particle = " << partName
            << "\nNo decay = " << counts
            << "\nNo decay array size = " << EvtIds.size()
	    << "\nIn Ntuple " << EmcNtp.GetEntriesFast()
	    << "\nNumber of events in reco file = " << RecoTr->GetEntriesFast()
	    << "\n========================================================\n";

  std::cout << "<INFO> Writing output to: " << outFileName << '\n';

  // Write to the output
  TFile out(outFileName.c_str(), "RECREATE","Selected evets File.", 9);

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
