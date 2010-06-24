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
  cout << "Count = " << counts << " pdg = " << pdg << endl;
  
  std::vector<int> EvtIds;
  
  //EMC cluster energy, num.clusters, numb. crystals
  float emc, emcCorr, mom;
  
  //Selected Zernike moments and LATeral energydeposition
  float z20, z53, latEdep;
  int numClus, numCrys;
  mom = emc = emcCorr = z20 = z53 = 0.00;
  numClus = numCrys = 0;
  
  // N-Tuple to store the variables.
  TNtuple EmcNtp (partName.c_str(), partName.c_str(),
		  "id:p:emc:emcCorr:numClus:numCrys:lat:z20:z53");
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
  
  // Reco file
  TFile recoF(recoFile.c_str());
  // Reco tree
  TTree* RecoTr = (TTree *) recoF.Get("cbmsim");
  
  // Track list
  TClonesArray* recTrakArr = new TClonesArray("PndTrack");  
  RecoTr->SetBranchAddress("LheTrack", &recTrakArr);
  //RecoTr->SetBranchAddress("LheGenTrackPion", &recTrakArr);
  
  // Loop through the simulation data.
  std::cout << "<INFO> Selecting events." << std::endl;
  for (int j = 0; j < tsim->GetEntriesFast(); j++){
    tsim->GetEntry(j);
    
    cout << "<Event>: " << j << endl;
    
    // Select the first interaction point.
    PndEmcPoint* pt   = (PndEmcPoint*) pointList->At(0);
    
    if(pt){
      int trID = pt->GetTrackID();
      PndMCTrack* track = (PndMCTrack*) trackList->At(trID);
      
      // Select event with the correct pdg code.
      if(track->GetPdgCode() == pdg){
        counts++;
	EvtIds.push_back(j);
      }
      else{//Decay ????
        std::cout << "<Wrong pdg> :"
		  << " Track ID = " << trID 
		  << " Track with pdg = " << track->GetPdgCode()
		  << std::endl;
      }
    }
    else{// No emc interaction??
      std::cout << "No point on the EMC." << std::endl;
    }
  }
  
  std::cout << "<INFO> Processing selected events." << std::endl;

  // Loop through the selected events.
  for(size_t i = 0; i < EvtIds.size(); i++){
    int evid = EvtIds[i];
    cout << "<Event number> = " << evid << " ";
    RecoTr->GetEntry(evid);
    digiTr->GetEntry(evid);
    
    PndTrack* tra = (PndTrack*) recTrakArr->At(0);
    
    if(tra){
      // Charged or correct reconstructed.
      FairTrackParP par = tra->GetParamLast();
      
      std::cout << "number of clusters = "
		<< clusters_arr->GetEntriesFast()
		<<" number of tracks = "<< recTrakArr->GetEntriesFast() 
		<< " with P = "<< par.GetMomentum().Mag() 
		<< endl;
      
      // Select the right cluster(highest E_dep).
      double maxEnergy = -1.0;
      int clIndex = -1;
      
      // Loop through the clusters.
      for(int cl = 0; cl < clusters_arr->GetEntriesFast(); cl++){
	PndEmcCluster* clust = (PndEmcCluster*) clusters_arr->At(cl);
	if(clust->GetEnergyCorrected() > maxEnergy){
	  maxEnergy = clust->GetEnergyCorrected();
	  clIndex = cl;
	}
      }
      
      // Found the cluster with highest E_dep.
      if( clIndex >= 0 ){
	PndEmcCluster* HE_cluster = (PndEmcCluster*) clusters_arr->At(clIndex);
	
	numClus = clusters_arr->GetEntriesFast();
	numCrys = HE_cluster->NumberOfDigis();
	mom = par.GetMomentum().Mag();
	emc = HE_cluster->energy(); 
	emcCorr = HE_cluster->GetEnergyCorrected();  
	
	//z20 = clsZmom.AbsZernikeMoment(2, 0, 15);// Z_{n = 2}^{m = 0}
	z20 = HE_cluster->Z20();
	
	//z53 = clsZmom.AbsZernikeMoment(5, 3, 15);// Z_{n = 5}^{m = 3}
	z53 = HE_cluster->Z53();
	
	//latEdep = clsZmom.Lat();
	latEdep = HE_cluster->LatMom();
	
	// Fill tree
	if( (mom > 0) && (mom <= 15) && ((emc/mom) <= 2.0) ){
	  EmcNtp.Fill(evid, mom, emc, emcCorr, numClus, numCrys, latEdep, z20, z53);
	  std::cout << "emc = " << emc << " emcCorr = " << emcCorr << std::endl;
	}
      }
    }// End if(tra)
    else{//Neutral or not correctly reconstructed.
      cout << "<Empty track>: number of clusters = "
	   << clusters_arr->GetEntriesFast() << endl;
    }
  }
  
  std::cout << "========================================================"
	    << std::endl;
  std::cout << "<INFO>" << std::endl 
	    << "Total number of events = " << tsim->GetEntriesFast() << std::endl
            << "No decay = " << counts << std::endl
            << "No decay array size = " << EvtIds.size() << std::endl;
  std::cout << "In Ntuple " << EmcNtp.GetEntriesFast() << std::endl;
  std::cout << "Number of events in reco file = " << RecoTr->GetEntriesFast() 
	    << std::endl;
  std::cout << "========================================================"
	    << std::endl;
  
  cout << "<INFO> Writing output to: " << outFileName << endl;
  
  // Write to the output
  TFile out(outFileName.c_str(), "RECREATE");
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
  cout <<"=======================================" << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  exit(0);
}
