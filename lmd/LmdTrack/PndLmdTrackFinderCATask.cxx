 // -------------------------------------------------------------------------
// -----                      PndLmdTrackFinderCATask                  -----
// -----                  Created 18/05/11  by A. Karavdina            -----
// -------------------------------------------------------------------------

#include <math.h>
#include "TClonesArray.h"
#include "TArrayD.h"
#include "TVectorD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndLmdTrackFinderCATask.h"

#include "PndSdsDigiStrip.h"
#include "TStopwatch.h"
#include "PndLmdDim.h"

#include "PndSdsCell.h"
// #include "PndSdsPixelCluster.h"

// -----   Default constructor   -------------------------------------------
PndLmdTrackFinderCATask::PndLmdTrackFinderCATask() :
  FairTask("LMD Track Finding Task (Cellular Automation)")
{
  missPlAlgo = false;
  flagTrkCandCuts=true;
  fHitBranchStrip = "LMDHitsStrip";
  fClusterBranchStrip = "LMDStripClusterCand";
  fDigiBranchStrip = "LMDStripDigis";
  
  dXY = 0.5;
  rule_max = 1e-6;
  nSensPP = 8;
  nP = 4;
  flagStipSens = false;
  flagPixelSens = false;

  //TODO: flexiable matrix path&name!
  mtxpath = "../../input/";
  mtx_perfect =   mtxpath+"trafo_matrices_lmd.dat";

}


// -------------------------------------------------------------------------

// -----   Constructor   -------------------------------------------
PndLmdTrackFinderCATask::PndLmdTrackFinderCATask(const bool missPl, const double setdmax, int innSensPP, int innP, TString hitBranch, TString clusterBranch, TString digiBranch) :
  FairTask("LMD Track Finding Task (Cellular Automation) with/without <<missing planes>> algoritm")
{
  missPlAlgo = missPl;
  fHitBranchStrip = hitBranch;
  fClusterBranchStrip = clusterBranch;
  fDigiBranchStrip = digiBranch;
  
  dXY = 0.5;
  rule_max = setdmax; 
  nSensPP = innSensPP;
  nP = innP;
  flagStipSens = false;//default
  flagPixelSens = true;//default

//TODO: flexiable matrix path&name!
  mtxpath = "../../input/";
  mtx_perfect =   mtxpath+"trafo_matrices_lmd.dat";
}


// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndLmdTrackFinderCATask::~PndLmdTrackFinderCATask()
{

}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndLmdTrackFinderCATask::SetParContainers()
{
  // Get Base Container
  /*
    FairRun* ana = FairRun::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    fGeoPar = (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));
  */
}

InitStatus PndLmdTrackFinderCATask::ReInit()
{
  InitStatus stat=kERROR;
  return stat;
}


// -----   Private method SortHitsByDet   --------------------------------------------
bool PndLmdTrackFinderCATask::SortHitsByDet(std::vector< std::vector < std::pair <Int_t,bool> > > &hitsd, Int_t nStripHits)
{
  Int_t nPlanes=0;

//sort in plane's
  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));
  
    Int_t sensid = myHit->GetSensorID(); // Sensors: 1..32
    Int_t planeid = floor((sensid)/(double)nSensPP); //nSensPP sensors/plane => Planes: 0..3
    hitsd.at(planeid).push_back( make_pair (iHit,false) );
  }

  for(Int_t iPlane = 0; iPlane < nP; iPlane++){
     if(hitsd.at(iPlane).size()>0) nPlanes++;
  }

  //  cout << "Hits: " << nStripHits << endl;
  if(fVerbose>2) {
    cout << "Hits: " << nStripHits << " in " << nPlanes << " plane(s)." << endl;
    for(Int_t idet = 0; idet < nP; idet++)
      cout << "Plane: "<< idet <<" DiscHits: "<< hitsd.at(idet).size() <<endl;
  }

  if(nPlanes>2) return true;
  return false;
}
// -------------------------------------------------------------------------



// -----   Private method SortHitsByDet   --------------------------------------------
bool PndLmdTrackFinderCATask::SortHitsByDetSimple(std::vector< std::vector< Int_t > > &hitsd, Int_t nStripHits)
{
  Int_t nPlanes=0;

//sort in plane's
  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));
    Int_t sensid = myHit->GetSensorID(); // Sensors: 1..32
    Int_t planeid = floor((sensid)/(double)nSensPP); //nSensPP sensors/plane => Planes: 0..3
    hitsd.at(planeid).push_back(iHit);
  }

  for(Int_t iPlane = 0; iPlane < nP; iPlane++){
     if(hitsd.at(iPlane).size()>0) nPlanes++;
  }

  if(fVerbose>2) {
    cout << "Hits: " << nStripHits << " in " << nPlanes << " plane(s)." << endl;
    for(Int_t idet = 0; idet < nP; idet++)
      cout << "Plane: "<< idet <<" DiscHits: "<< hitsd.at(idet).size() <<endl;
  }

  if(nPlanes>2) return true;
  return false;
}
// -------------------------------------------------------------------------


// -----   Private method SortHitsByDet   --------------------------------------------
bool PndLmdTrackFinderCATask::SortHitsByDetSimple2(std::vector< std::vector< Int_t > > &hitsd, Int_t nStripHits)
{
  Int_t nPlanes=0;

//sort in plane's
  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));
    Int_t sensid = myHit->GetSensorID(); // Sensors: 1..32
    int ihalf,iplane,imodule,iside,idie,isensor;
    lmddim->Get_sensor_by_id(sensid,ihalf,iplane,imodule,iside,idie,isensor);
    int virtplane = iplane;//merged hits
    if(nP>4) virtplane = 2*iplane+iside;// single hits
    hitsd.at(virtplane).push_back(iHit);
  }

  for(Int_t iPlane = 0; iPlane < nP; iPlane++){
     if(hitsd.at(iPlane).size()>0) nPlanes++;
  }

  if(fVerbose>2) {
    cout << "Hits: " << nStripHits << " in " << nPlanes << " plane(s)." << endl;
    for(Int_t idet = 0; idet < nP; idet++)
      cout << "Plane: "<< idet <<" DiscHits: "<< hitsd.at(idet).size() <<endl;
  }

  if(nPlanes>2) return true;
  return false;
}
// -------------------------------------------------------------------------

// -----   Private method SortHitsByZ   --------------------------------------------
bool PndLmdTrackFinderCATask::SortHitsByZ(std::vector< std::vector< std::pair<Int_t,bool> > > &hitsd, Int_t nStripHits)
{
  std::vector<Double_t> detZ;

//find plane positions
  for (Int_t iHit = 0; iHit < nStripHits; iHit++){
    Double_t tmp = ((PndSdsHit*) (fStripHitArray->At(iHit)))->GetZ();
    bool newZ = true;
    for(unsigned int idet = 0; idet < detZ.size(); idet++){
      if(fabs(tmp-detZ.at(idet))<9.){ //check if already found [for using with Dipole]
        newZ = false;
      }
    }
    if(newZ){
      detZ.push_back(tmp);

      //sort positions
      Int_t pos=-1;
      for(int idet = detZ.size()-1; idet >= 0; idet--){
        if(tmp < detZ.at(idet))
          pos=idet;
      }
      if(pos!=-1){
        Double_t swap = detZ.at(pos);
        detZ.at(pos) = tmp;
        tmp = swap;
        for(unsigned int i=pos+1; i<detZ.size(); i++){
          swap = detZ.at(i);
          detZ.at(i) = tmp;
          tmp = swap;
        }
      }
    }
  }

//sort in plane's
  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));

    Double_t z = myHit->GetZ();
    for(unsigned int idet = 0; idet < detZ.size(); idet++){ //planes
     
      if( fabs(z-detZ.at(idet))<9. ){ //[for using with Dipole]
	hitsd.at(idet).push_back( make_pair (iHit,false) );
      }
    }
  }


   if(fVerbose>2) {
     cout << "Hits: " << nStripHits << " in " << detZ.size() << " plane(s)." << endl;
     for(unsigned int idet = 0; idet < detZ.size(); idet++)
       cout << "Plane: "<< idet <<" DiscHits: "<< hitsd.at(idet).size() <<endl;
   }

  if(detZ.size()>3) return true;
  return false;
}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndLmdTrackFinderCATask::Init()
{

  lmddim = PndLmdDim::Instance();
  lmddim -> Read_transformation_matrices(mtx_perfect.Data(),false);


  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndLmdTrackFinderCATask::Init: "
		<< "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fStripHitArray = (TClonesArray*) ioman->GetObject(fHitBranchStrip);
  if ( !fStripHitArray){
    std::cout << "-W- PndLmdTrackFinderCATask::Init: " << "No fStripHitArray!" << std::endl;
    return kERROR;
  }

  fStripClusterArray = (TClonesArray*) ioman->GetObject(fClusterBranchStrip);
  if ( !fStripClusterArray){
    std::cout << "-W- PndLmdTrackFinderCATask::Init: " << "No StripclusterArray!" << std::endl;
    return kERROR;
  }

  fStripDigiArray = (TClonesArray*) ioman->GetObject(fDigiBranchStrip);
  if ( !fStripDigiArray){
    std::cout << "-W- PndLmdTrackFinderCATask::Init: " << "No StripdigiArray!" << std::endl;
    return kERROR;
  }

  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("LMDTrackCand", "PndLmd", fTrackCandArray, kTRUE);

  fCellArray = new TClonesArray("PndSdsCell");
  fCellArray_tmp = new TClonesArray("PndSdsCell");

  std::cout << "-I- PndLmdTrackFinderCATask: Initialisation successfull" << std::endl;
  if(missPlAlgo) std::cout << "-I- PndLmdTrackFinderCATask: missing plane(s) algorithm will be used" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndLmdTrackFinderCATask::Exec(Option_t* opt)
{
  TStopwatch *timer_array = new TStopwatch();
  if(fVerbose>0) 
    timer_array->Start();

  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Clear();
  Int_t nPixelHits = fStripHitArray->GetEntriesFast();
  if(nPixelHits<2){
    if(fVerbose>2) cout << "Evt finsihed: too less hits-----"<<endl<<endl;
    return;
  }
 
  std::vector< std::vector<Int_t> > hitsd(nP); //hit'ids splitted by detectorplane
  bool resSortHits;
  if(flagStipSens) resSortHits = SortHitsByDetSimple(hitsd, nPixelHits);//! strip sensors
  else{
    if(flagPixelSens) resSortHits = SortHitsByDetSimple2(hitsd, nPixelHits);//! pixel sensors
    else{
      std::cout<<"Algorithm depend on sensor type! Please, set it via SetSensStripFlag(bool fS) or SetSensPixelFlag(bool fS)"<<std::endl;
      return;
    }
  }
  if(!resSortHits){
    if(fVerbose>2) cout << "Evt finsihed: too less planes-----"<<endl<<endl;
    return;
  }

  
  //print hits (debug)
  TVector3 start, tmp, vec, dstart, dvec; //temp-vars
  if(fVerbose>2){
    for(Int_t iPlane = 0; iPlane < nP; iPlane++){
      if(hitsd.at(iPlane).size()>0){
	for (unsigned int i=0; i<hitsd.at(iPlane).size(); i++){
	  PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(iPlane).at(i));
	  if(fVerbose>2)	  
	    cout<<" Hit#"<<hitsd.at(iPlane).at(i)<<" Plane"<<iPlane<<" Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<") with err=("
		<<hit->GetDx()<<", "<<hit->GetDy()<<", "<<hit->GetDz()<<")"<<", MChitID = "<<hit->GetRefIndex()<<endl;
	}
      }
    }
  }
  if(fVerbose>0){ 
    cout<<"array preparation: "<<endl;
    timer_array->Stop();
    timer_array->Print();
  }
    //

    TStopwatch *timer_cook_cells = new TStopwatch();
    timer_cook_cells->Start();
    const unsigned int nplanes = nP;

  ///Build all cells  
  int ncells=0;
  if(fVerbose>2)
  cout<<"Start cell contruction from "<<nPixelHits<<" hits"<<endl;
  fCellArray->Clear();
  for(unsigned int pl0=0;pl0<(nplanes-1);pl0++){
    for (unsigned int i=0; i<hitsd.at(pl0).size(); i++){
      unsigned int pl1 = pl0+1;// no "missing plane"
      for (unsigned int j=0; j<hitsd.at(pl1).size(); j++){
	if(fVerbose>2)
	cout<<"new cell with hits #"<<hitsd.at(pl0).at(i)<<" and "<<hitsd.at(pl1).at(j)<<endl;
	PndSdsCell *cell = new PndSdsCell(hitsd.at(pl1).at(j), hitsd.at(pl0).at(i));
	new((*fCellArray)[ncells]) PndSdsCell(*(cell)); //save Track
	delete cell;//TEST
	ncells++;
      }
      if(missPlAlgo && pl0<nplanes-2){
	pl1 = pl0+2;// with "missing plane"
	for (unsigned int k=0; k<hitsd.at(pl1).size(); k++){
	  PndSdsCell *cell = new PndSdsCell(hitsd.at(pl1).at(k), hitsd.at(pl0).at(i));
	  if(fVerbose>2)
	  cout<<"new cell with hits #"<<hitsd.at(pl0).at(i)<<" and "<<hitsd.at(pl1).at(k)<<endl;
	  new((*fCellArray)[ncells]) PndSdsCell(*(cell)); //save Track
	  delete cell;//TEST
	  ncells++;
	}
      }
    }
  }

  if(fVerbose>0){
  cout<<"Cells cooking:"<<endl;
  timer_cook_cells->Stop();
  timer_cook_cells->Print();
  }
  //   Double_t rtime_cook_cells = timer_cook_cells->RealTime();
  //   Double_t ctime_cook_cells = timer_cook_cells->CpuTime();
  //   cout << "Real time for Cells cooking:" << rtime_cook_cells << " s, CPU time " << ctime_cook_cells << " s" << endl;
  // }

  TStopwatch *timer_neighbors_cells = new TStopwatch();
  if(fVerbose>0)
  timer_cook_cells->Start();

  //Find neighbors between cells
  /// Set counters values -----
  std::vector<int> pv_new;
  Int_t nCells = fCellArray->GetEntries();
  for(unsigned int icv=0;icv<nCells;icv++)
    pv_new.push_back(-1);
  bool stop_itter = true;
  for(int itt=0;itt<1000;itt++){ //should be infinite loop! due to small number of layers 1000 is close to infinity ;)
    fCellArray_tmp->Clear("C");
    int ncells_tmp = 0;
    stop_itter = true;
    TStopwatch *timer_neighbors_itter = new TStopwatch();
    if(fVerbose>0)
      timer_neighbors_itter->Start();
    nCells = fCellArray->GetEntries();
    if(fVerbose>2)
      cout<<" === ITTER === "<<itt<<" with "<<nCells<<" cells"<<endl;
    for(int ic=0; ic<nCells; ic++){
      PndSdsCell *cell0 =   (PndSdsCell*)fCellArray->At(ic);
      if(fVerbose>2)
      cout<<"cell0 with hitDw = "<<cell0->GetHitDw()<<" and hitUp = "<<cell0->GetHitUp()<<" and PV = "<<cell0->GetPV()<<endl;
      for(int jc=ic+1; jc<nCells; jc++){
	//	if (pv_new[ic]!=(cell0->GetHitUp()) && pv_new[ic]>1) continue;
	PndSdsCell *cell1 =   (PndSdsCell*)fCellArray->At(jc);
	if((cell0->GetHitUp())!=(cell1->GetHitDw())) continue;
	//	if((cell0->GetHitUp())==(cell1->GetHitDw())){ //cells have common point
       else { //cells have common point
	if(fVerbose>2)
	  cout<<"cell1 with hitDw = "<<cell1->GetHitDw()<<" and hitUp = "<<cell1->GetHitUp()<<" and PV = "<<cell1->GetPV()<<endl;

	  //check if cells make straight line
	  PndSdsHit *hit00=(PndSdsHit*)fStripHitArray->At(cell0->GetHitDw());
	  PndSdsHit *hit01=(PndSdsHit*)fStripHitArray->At(cell0->GetHitUp());
	  PndSdsHit *hit10 =(PndSdsHit*)fStripHitArray->At(cell1->GetHitDw());
	  PndSdsHit *hit11 =(PndSdsHit*)fStripHitArray->At(cell1->GetHitUp());
	  TVector3 A(hit01->GetX()-hit00->GetX(),hit01->GetY()-hit00->GetY(),hit01->GetZ()-hit00->GetZ());
	  TVector3 B(hit11->GetX()-hit10->GetX(),hit11->GetY()-hit10->GetY(),hit11->GetZ()-hit10->GetZ());
	  double ScalAB = A.Dot(B);
	  double cosPsi = ScalAB/(A.Mag()*B.Mag());
	  if((1-cosPsi)<rule_max){//rule for the straight line
	    if((cell0->GetPV())==(cell1->GetPV())){
	      if((cell1->GetPV())<0)
		pv_new[ic] = 0;
	      else
		pv_new[ic] = cell0->GetPV();
	      pv_new[jc] = cell1->GetPV()+1;
	      stop_itter = false;
	      if(fVerbose>2)
	      cout<<"pv 0 = "<< pv_new[ic]<<" pv 1 = "<< pv_new[jc]<<endl;
	    }
	  }
	}
      }
    }
    
    int fnCells = fCellArray->GetEntriesFast();
    for(int icv=0;icv<fnCells;icv++){
      if(pv_new[icv]<0){ //clean not used cells
	//TODO: celaning via RemoveAt doesn't work :( -> tmp solution: create new array with good cells
      }
      else{
	PndSdsCell *cell = (PndSdsCell*)fCellArray->At(icv);
	if(fVerbose>2)
	cout<<"cell with hitDw = "<<cell->GetHitDw()<<" and hitUp = "<<cell->GetHitUp()<<" gets new PV = "<<pv_new[icv]<<endl;
	cell->SetPV(pv_new[icv]);
	new((*fCellArray_tmp)[ncells_tmp]) PndSdsCell(*(cell)); //save Track
	delete cell;
	ncells_tmp++;
      }
    }
    
    if(fVerbose>0){
      cout<<"One itter with "<<nCells<<" cells: "<<endl;
      timer_neighbors_itter->Stop();
      timer_neighbors_itter->Print();
    }
    fCellArray = (TClonesArray*)fCellArray_tmp->Clone();
    if(fVerbose>0 && stop_itter) cout<<"-- CA made "<<itt<<" itterations --"<<endl;
    if(stop_itter) break;
  }
  ///--------------------------

  //  }
  if(fVerbose>0){
  timer_neighbors_cells->Stop();
  cout<<"neighbors search "<<endl;
  timer_neighbors_cells->Print();
  }
 
  nCells = fCellArray->GetEntriesFast();//final number of cells [could be different from the initial]

  //Build track from cells combination -------------------------------- 
  TStopwatch *timer_build_trk_combinations = new TStopwatch();
  if(fVerbose>0)
  timer_build_trk_combinations->Start();

  //find max number of cells in a track
  int pcmax = 0;
  for(unsigned int cid=1;cid<nCells;cid++){
    PndSdsCell *cell =   (PndSdsCell*)fCellArray->At(cid);
    int tag_cur = cell->GetPV();
    if(tag_cur>pcmax) pcmax = tag_cur;
  }
  if(fVerbose>4) cout<<"track can contain "<<pcmax<<"+1 cells"<<endl;
  const unsigned int trk_arr_size = pcmax+1;
  std::vector< std::vector<int> > trk_cells(trk_arr_size);
  int trk_count=-1;
  for(unsigned int newpcmax=pcmax;newpcmax>0;newpcmax--){// loop over possible number of cells in trk
    unsigned int cur_max_tag = newpcmax;
    if(fVerbose>4) cout<<"Now we are looking for trk with max "<<cur_max_tag+1<<" cells among "<<nCells<<" cells"<<endl;
    //    bool newtrk=true;
	for (int icell0 = 0; icell0 < nCells; icell0++){ 
	  PndSdsCell *cell0 =   (PndSdsCell*)fCellArray->At(icell0);
	  if(cell0->GetPV() == cur_max_tag){ //from "upstream" to "downstream"
	    for (int icell1 = 0; icell1 < nCells; icell1++){
	      PndSdsCell *cell1 =   (PndSdsCell*)fCellArray->At(icell1);
	      if( (cell0->GetPV()-cell1->GetPV())==1 && ((cell1->GetHitUp())==(cell0->GetHitDw()))){ //cells have common point
		//check if cells make straight line
		PndSdsHit *hit00=(PndSdsHit*)fStripHitArray->At(cell0->GetHitDw());
		PndSdsHit *hit01=(PndSdsHit*)fStripHitArray->At(cell0->GetHitUp());
		PndSdsHit *hit10 =(PndSdsHit*)fStripHitArray->At(cell1->GetHitDw());
		PndSdsHit *hit11 =(PndSdsHit*)fStripHitArray->At(cell1->GetHitUp());
		TVector3 A(hit01->GetX()-hit00->GetX(),hit01->GetY()-hit00->GetY(),hit01->GetZ()-hit00->GetZ());
		TVector3 B(hit11->GetX()-hit10->GetX(),hit11->GetY()-hit10->GetY(),hit11->GetZ()-hit10->GetZ());
		double ScalAB = A.Dot(B);
		double cosPsi = ScalAB/(A.Mag()*B.Mag());
		if((1-cosPsi)<rule_max){//rule for the straight line
		  if(fVerbose>2)
		    cout<<"trk #"<<trk_count<<" "<<cell0->GetPV()<<", "<<cell1->GetPV()<<"; ";
		  if(cell1->GetPV()>0){ //search for the last connected cell; TODO: this works only for number of planes = 4!!! extend it to infinite number of planes?
		    for (int icell2 = 0; icell2 < nCells; icell2++){
		      PndSdsCell *cell2 =   (PndSdsCell*)fCellArray->At(icell2);
		      if((cell1->GetPV()-cell2->GetPV())==1 && (cell2->GetHitUp())==(cell1->GetHitDw())){ //cells have common point
			//check if cells make straight line
			PndSdsHit *hit20=(PndSdsHit*)fStripHitArray->At(cell2->GetHitDw());
			PndSdsHit *hit21=(PndSdsHit*)fStripHitArray->At(cell2->GetHitUp());
			TVector3 A1(hit11->GetX()-hit10->GetX(),hit11->GetY()-hit10->GetY(),hit11->GetZ()-hit10->GetZ());
			TVector3 B1(hit21->GetX()-hit20->GetX(),hit21->GetY()-hit20->GetY(),hit21->GetZ()-hit20->GetZ());
			double ScalAB1 = A1.Dot(B1);
			double cosPsi1 = ScalAB1/(A1.Mag()*B1.Mag());
			if((1-cosPsi1)<rule_max){//rule for the straight line
			  trk_count++;
			  trk_cells.at(cell0->GetPV()).push_back(icell0);
			  trk_cells.at(cell1->GetPV()).push_back(icell1);
			  trk_cells.at(cell2->GetPV()).push_back(icell2);
			  if(fVerbose>2)
			  cout<<" "<<cell2->GetPV();
			}
		      }
		    }
		  }
		  else{
		    trk_count++;
		    trk_cells.at(cell0->GetPV()).push_back(icell0);
		    trk_cells.at(cell1->GetPV()).push_back(icell1);
		    if((cell0->GetPV()+1)==pcmax){
		      trk_cells.at(cell0->GetPV()+1).push_back(-1);
		      if(fVerbose>2)
		      cout<<" -1";
		    }
		  }
		  if(fVerbose>2)
		  cout<<""<<endl;
		}
	      }
	    }
	  }
	}
  }
  if(fVerbose>0){
  timer_build_trk_combinations->Stop();
  cout<<"Build trks combinations: "<<endl;
  timer_build_trk_combinations->Print();
  }
  if(fVerbose>2)
  cout<<"Before filtering number of trk-cands = "<<trk_cells.at(0).size()<<endl;
  // //TEST: no filter
  // vector<bool> trk_accept;
  // for(unsigned int itrk=0;itrk<trk_cells.at(0).size();itrk++){
  //   trk_accept.push_back(true);
  // }
  // //   TEST: no filter end---

  // filter ----------------------------------------------------
  TStopwatch *timer_filter_trk_combinations = new TStopwatch();
  if(fVerbose>0) 
  timer_filter_trk_combinations->Start();

  if(fVerbose>4) cout<<"--- filter trk-cand array: "<<endl;
  if(fVerbose>4) cout<<" Attention each trk candidate with repeated cells, but smaller cells number will be deleted!"<<endl;
  vector<unsigned int> cell_parts;
  vector<bool> trk_accept;
  for(unsigned int itrk=0;itrk<trk_cells.at(0).size();itrk++){
    int maxcellnum = trk_arr_size;
    int curr_arr=trk_arr_size;
    while(curr_arr>0){
      curr_arr--;
      if(fVerbose>4) cout<<" we have: trk_cells.at("<<curr_arr<<").at("<<itrk<<")="<<trk_cells.at(curr_arr).at(itrk)<<endl;
      if(trk_cells.at(curr_arr).at(itrk)<0) maxcellnum--;
    }
    cell_parts.push_back(maxcellnum);
    if(maxcellnum>0) trk_accept.push_back(true);
    else trk_accept.push_back(false);
    if(fVerbose>4) cout<<" with:"<<maxcellnum<<" cells"<<endl;
  }
  
  for(int itrc=(cell_parts.size()-1);itrc>=0;itrc--){
    if(!trk_accept[itrc]) continue;
    int cup=itrc-1;
    for(int itrc2=cup;itrc2>=0;itrc2--){
      if(!trk_accept[itrc]) break;
      if(!trk_accept[itrc2]) continue;
      //      cout<<"compare trk#"<<itrc<<" and #"<<itrc2<<endl;

      if(cell_parts[itrc]<cell_parts[itrc2]){
  	unsigned int count_re=0;
  	unsigned int count_hits=0;
  	//reject trk-cand with smaller number of cells and similar cells
  	int curr_arr=trk_arr_size;
  	while(curr_arr>0){
  	  curr_arr--;
 	  int cellnum1=trk_cells.at(curr_arr).at(itrc);
 	  int cellnum2=trk_cells.at(curr_arr).at(itrc2);
 	  //check by cells
  	  if(cellnum1<0 || cellnum2<0) continue;
  	  if(cellnum1==cellnum2)
  	    count_re++;
 	  //check by cells ends (hits)
	  PndSdsCell *cell0 =   (PndSdsCell*)fCellArray->At(cellnum1);
	  PndSdsCell *cell1 =   (PndSdsCell*)fCellArray->At(cellnum2);
 	  int hit0_0 = cell0->GetHitDw();
 	  int hit1_0 = cell0->GetHitUp();
 	  int hit0_1 = cell1->GetHitDw();
 	  int hit1_1 = cell1->GetHitUp();
	  if(hit0_0==hit0_1)
	    count_hits++;
	  if(hit1_0==hit1_1)
	    count_hits++;
	  if(hit0_0==hit1_1)
	    count_hits++;
	  if(hit1_0==hit0_1)
	    count_hits++;
  	}
 	if(fVerbose>3)
   	  cout<<"Cand.No"<<itrc<<": Number of repeating cells = "<<count_re<<" and cells in tot = "<<cell_parts[itrc]
 	      <<" Number of repeating hits = "<<count_hits<<endl;
 	if(count_re>0.55*cell_parts[itrc] || count_hits>2){
   	  trk_accept[itrc]=false;
  	  if(fVerbose>4){
	    cout<<"Delete: trk-cand#"<<itrc
 	      	<<" because it contains ("<<count_re<<") more then 55% cells from trk-cand#"<<itrc2<<endl;
 	  }
  	}
      }
    }
  }


  if(fVerbose>0){
  timer_filter_trk_combinations->Stop();
  cout<<"Filter: "<<endl;
  timer_filter_trk_combinations->Print();
  }




  //save trk-cands ----------------------------------------
  int NtrkRec = 0;
  for(unsigned int itrk=0;itrk<trk_accept.size();itrk++){
    if(!trk_accept[itrk]) continue; // filter
    bool firstHit=true;
    PndTrackCand *myTCand = new PndTrackCand();
    TVector3 dir;
    if(fVerbose>2)
      cout<<"Trk-cand contains hits:";
    for(unsigned int icell=0;icell<trk_arr_size;icell++){
      int cellNum = trk_cells.at(icell).at(itrk);
      if(cellNum<0) continue;
      //    cout<<"; from cell#"<<cellNum<<": ";
      PndSdsCell* cell =  (PndSdsCell*)fCellArray->At(cellNum);
      if(firstHit){//fill trk-cand seed
	PndSdsHit *myHit = (PndSdsHit*)fStripHitArray->At(cell->GetHitDw());
	if(fVerbose>2)
	cout<<" "<<cell->GetHitDw();
	PndSdsClusterStrip *myCluster =  (PndSdsClusterStrip*)fStripClusterArray->At(myHit->GetClusterIndex());
	PndSdsDigiStrip *astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
	myTCand->AddHit(astripdigi->GetDetID(),cell->GetHitDw(),myHit->GetPosition().Z()); 
	PndSdsHit *myHit1 = (PndSdsHit*)fStripHitArray->At(cell->GetHitUp());
	dir = myHit1->GetPosition() - myHit->GetPosition();
	dir*=1./dir.Mag();
	firstHit=false;
      }
      if(fVerbose>2)
      cout<<" "<<cell->GetHitUp();
      PndSdsHit *myHit1 = (PndSdsHit*)fStripHitArray->At(cell->GetHitUp());
      PndSdsClusterStrip *myCluster1 =  (PndSdsClusterStrip*)fStripClusterArray->At(myHit1->GetClusterIndex());
      PndSdsDigiStrip *astripdigi1 = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster1->GetDigiIndex(0));
      myTCand->AddHit(astripdigi1->GetDetID(),cell->GetHitUp(),myHit1->GetPosition().Z()); 
    }
    if(fVerbose>2)
    cout<<" "<<endl;
    const unsigned int numPts = myTCand->GetNHits(); //read how many points in this track
    if(numPts<3) cout<<"!!! Attention HERE is problem: number of hits in trk-cand = "<<numPts<<"!!!"<<endl;
    if(numPts<3) continue; //TEST
    new((*fTrackCandArray)[NtrkRec]) PndTrackCand(*(myTCand)); //save Track Candidate
    NtrkRec++;
    if(fVerbose>3) cout<<"Ntrk No. "<<NtrkRec<<endl;
    delete myTCand;
  }

  hitsd.erase(hitsd.begin(),hitsd.end());
  if(fVerbose>2){
    Int_t ntcandFin=fTrackCandArray->GetEntriesFast();
    cout<<"Number of Trk-Cands is "<<ntcandFin<<endl;
    cout<< "Evt finsihed--------------"<<endl<<endl;
    cout << endl;
  }
  fCellArray->Clear("C");
 fCellArray_tmp->Clear("C");
}

Double_t PndLmdTrackFinderCATask::GetTrackCurvature(PndMCTrack* myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (2/TMath::Sqrt(p.Px()*p.Px() + p.Py()*p.Py()));
}
// -------------------------------------------------------------------------

Double_t PndLmdTrackFinderCATask::GetTrackDip(PndMCTrack* myTrack)
{
  TVector3 p= myTrack->GetMomentum();
  return (p.Mag()/TMath::Sqrt(p.Px()*p.Px() + p.Py()*p.Py()));
}
void PndLmdTrackFinderCATask::FinishTask(){
}
// -------------------------------------------------------------------------
ClassImp(PndLmdTrackFinderCATask)
