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

// #include "PndSdsPixelCluster.h"

// -----   Default constructor   -------------------------------------------
PndLmdTrackFinderCATask::PndLmdTrackFinderCATask() :
  FairTask("LMD Track Finding Task (Cellular Automation)")
{
  missPlAlgo = false;
  fHitBranchStrip = "LMDHitsStrip";
  fClusterBranchStrip = "LMDStripClusterCand";
  fDigiBranchStrip = "LMDStripDigis";
  
  dXY = 0.5;
  d_max = 0.01;
  hdist = new TH1D("hdist","distance from common point",1e4,0,10.);
  htheta = new TH2D("htheta",";length;#theta angle",1e3,0,25,1e3,0,3.15);
  nSensPP = 8;
  nP = 4;
  flagStipSens = false;
  flagPixelSens = false;
  //   dXY = 0.01;//TEST
}


// -------------------------------------------------------------------------

// -----   Constructor   -------------------------------------------
PndLmdTrackFinderCATask::PndLmdTrackFinderCATask(const bool missPl, const double setdmax, int innSensPP, int innP, TString hitBranch, TString clusterBranch, TString digiBranch) :
  FairTask("LMD Track Finding Task (Cellular Automation) with/without <<missing planes>> algoritm")
{
  missPlAlgo = missPl;
  // fHitBranchStrip = "LMDHitsStrip";
  // fClusterBranchStrip = "LMDStripClusterCand";
  // fDigiBranchStrip = "LMDStripDigis";
  fHitBranchStrip = hitBranch;
  fClusterBranchStrip = clusterBranch;
  fDigiBranchStrip = digiBranch;
  
  dXY = 0.5;
  hdist = new TH1D("hdist","distance from common point",1e3,0,1.);
  htheta = new TH2D("htheta",";length;#theta angle",1e3,0,25,1e3,0,3.15);
  d_max = setdmax; 
  nSensPP = innSensPP;
  nP = innP;
  //   dXY = 0.01;//TEST
}


// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndLmdTrackFinderCATask::~PndLmdTrackFinderCATask()
{}
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

  /*
    FairRun* ana = FairRun::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    fGeoPar=(PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));

    return kSUCCESS;
  */
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
  //  cout<<"PndLmdTrackFinderCATask::SortHitsByDetSimple! nStripHits = "<<nStripHits<<endl;
  Int_t nPlanes=0;

//sort in plane's
  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));
    Int_t sensid = myHit->GetSensorID(); // Sensors: 1..32
    //    cout<<"sensid = "<<sensid<<endl;
    Int_t planeid = floor((sensid)/(double)nSensPP); //nSensPP sensors/plane => Planes: 0..3
    //    cout<<" planeid = "<< planeid<<endl;
    hitsd.at(planeid).push_back(iHit);
  }

  for(Int_t iPlane = 0; iPlane < nP; iPlane++){
     if(hitsd.at(iPlane).size()>0) nPlanes++;
     //  cout<<"nPlanes = "<<nPlanes<<endl;
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
bool PndLmdTrackFinderCATask::SortHitsByDetSimple2(std::vector< std::vector< Int_t > > &hitsd, Int_t nStripHits)
{
  //  cout<<"PndLmdTrackFinderCATask::SortHitsByDetSimple! nStripHits = "<<nStripHits<<endl;
  Int_t nPlanes=0;

//sort in plane's
  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));
    Int_t sensid = myHit->GetSensorID(); // Sensors: 1..32
    //    cout<<"sensid = "<<sensid<<endl;
    // Int_t planeid = floor((sensid)/(double)nSensPP); //nSensPP sensors/plane => Planes: 0..3
    // cout<<" planeid = "<< planeid<<endl;
    int ihalf,iplane,imodule,iside,idie,isensor;
    lmddim->Get_sensor_by_id(sensid,ihalf,iplane,imodule,iside,idie,isensor);
    //    hitsd.at(iplane).push_back(iHit);
    int virtplane = iplane;//merged hits
    if(nP>4) virtplane = 2*iplane+iside;// single hits
    hitsd.at(virtplane).push_back(iHit);
  }

  for(Int_t iPlane = 0; iPlane < nP; iPlane++){
     if(hitsd.at(iPlane).size()>0) nPlanes++;
     //  cout<<"nPlanes = "<<nPlanes<<endl;
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

// -----   Private method SortHitsByZ   --------------------------------------------
bool PndLmdTrackFinderCATask::SortHitsByZ(std::vector< std::vector< std::pair<Int_t,bool> > > &hitsd, Int_t nStripHits)
{
  std::vector<Double_t> detZ;

//find plane positions
  for (Int_t iHit = 0; iHit < nStripHits; iHit++){
    Double_t tmp = ((PndSdsHit*) (fStripHitArray->At(iHit)))->GetZ();
    bool newZ = true;
    for(Int_t idet = 0; idet < detZ.size(); idet++){
      //  if(tmp == detZ.at(idet)){ //check if already found
      //   cout<<"tmp = "<<tmp<<" detZ.at(idet) = "<<detZ.at(idet)
      //	  <<" fabs(tmp-detZ.at(idet))="<<fabs(tmp-detZ.at(idet))<<endl;
      if(fabs(tmp-detZ.at(idet))<9.){ //check if already found [for using with Dipole]
      // if(fabs(tmp-detZ.at(idet))<0.015){ //check if already found [for using with Dipole] !!! for pixel !!!
        newZ = false;
      }
    }
    if(newZ){
      detZ.push_back(tmp);

      //sort positions
      Int_t pos=-1;
      for(Int_t idet = detZ.size()-1; idet >= 0; idet--){
        if(tmp < detZ.at(idet))
          pos=idet;
      }
      if(pos!=-1){
        Double_t swap = detZ.at(pos);
        detZ.at(pos) = tmp;
        tmp = swap;
        for(Int_t i=pos+1; i<detZ.size(); i++){
          swap = detZ.at(i);
          detZ.at(i) = tmp;
          tmp = swap;
        }
      }
    }
  }
  //  cout<<"Attention! detZ.size()="<<detZ.size()<<endl;

//sort in plane's
  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));

    Double_t z = myHit->GetZ();
    for(Int_t idet = 0; idet < detZ.size(); idet++){ //planes
     
      // if( z == detZ.at(idet) ){
      if( fabs(z-detZ.at(idet))<9. ){ //[for using with Dipole]
      //  cout<<" fabs(z-detZ.at(idet) = "<< fabs(z-detZ.at(idet))<<endl;
      //      if( fabs(z-detZ.at(idet))<0.015 ){ //[for using with Dipole] !!! for pixel !!!
	hitsd.at(idet).push_back( make_pair (iHit,false) );
	//	cout<<"detZ.at("<<idet<<")="<<detZ.at(idet)<<" z="<<z<<endl;
      }
    }
  }

  //   cout << "Hits: " << nStripHits << endl;
   if(fVerbose>2) {
     cout << "Hits: " << nStripHits << " in " << detZ.size() << " plane(s)." << endl;
     for(Int_t idet = 0; idet < detZ.size(); idet++)
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
  // lmddim -> Read_transformation_matrices("matrices.txt", true);
  lmddim -> Read_transformation_matrices("matrices_perfect.txt", false);


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

  // fTrackCandArrayTemp = new TClonesArray("PndTrackCand"); //delete it!
  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("LMDTrackCand", "PndLmd", fTrackCandArray, kTRUE);


  std::cout << "-I- PndLmdTrackFinderCATask: Initialisation successfull" << std::endl;
  if(missPlAlgo) std::cout << "-I- PndLmdTrackFinderCATask: missing plane(s) algorithm will be used" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndLmdTrackFinderCATask::Exec(Option_t* opt)
{
  TStopwatch *timer_exec = new TStopwatch();
  if(fVerbose>2){
    timer_exec->Start();
    cout << "Evt started--------------"<<endl<<endl;
  }
  
  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Clear();
  Int_t nStripHits = fStripHitArray->GetEntriesFast();
  if(nStripHits<2){
    if(fVerbose>2) cout << "Evt finsihed: too less hits-----"<<endl<<endl;
    return;
  }
  std::vector< std::vector<Int_t> > hitsd(nP); //hit'ids splitted by detectorplane
  // std::vector< std::vector< std::pair<Int_t,bool> > > hitsd(4);
  bool resSortHits;
  if(flagStipSens) resSortHits = SortHitsByDetSimple(hitsd, nStripHits);//! strip sensors
  else{
    if(flagPixelSens) resSortHits = SortHitsByDetSimple2(hitsd, nStripHits);//! pixel sensors
    else{
      std::cout<<"Algorithm depend on sensor type! Please, set it via SetSensStripFlag(bool fS) or SetSensPixelFlag(bool fS)"<<std::endl;
      return;
    }
  }
  if(!resSortHits){
    if(fVerbose>2) cout << "Evt finsihed: too less planes-----"<<endl<<endl;
    return;
  }

  // for(Int_t iHit = 0; iHit < nStripHits; iHit++){
  //   PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));
  //   Int_t sensid = myHit->GetSensorID(); // Sensors: 1..32
  //   Int_t planeid = floor((sensid)/8.); //8 sensors/plane => Planes: 0..3
  //   hitsd.at(planeid).push_back(iHit);
  // }

  // Int_t nPlanes=0;
  // for(Int_t iPlane = 0; iPlane < 4; iPlane++){
  //   if(hitsd.at(iPlane).size()>0) nPlanes++;
  // }
  // if(nPlanes<3){
  //   if(fVerbose>2) cout << "Evt finsihed: too less planes-----"<<endl<<endl;
  //   return;
  // }
  
  //print hits (debug)
  TVector3 start, tmp, vec, dstart, dvec; //temp-vars
  if(fVerbose>2){
    for(Int_t iPlane = 0; iPlane < nP; iPlane++){
      if(hitsd.at(iPlane).size()>0){
	for (Int_t i=0; i<hitsd.at(iPlane).size(); i++){
	  PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(iPlane).at(i));
	  cout<<"Plane"<<iPlane<<" Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<") with err=("
	      <<hit->GetDx()<<", "<<hit->GetDy()<<", "<<hit->GetDz()<<")"<<endl;
	}
      }
    }
  }
  
  // TStopwatch *timer_cook_cells = new TStopwatch();
  // timer_cook_cells->Start();
  int Npoints=0;
  const int nplanes = nP;
  int NpointsI[nplanes];
  int cell_arr_size=0;
  for(int i=0;i<nplanes;i++){
    NpointsI[i]=hitsd.at(i).size();
    if(i>0) cell_arr_size+= NpointsI[i]* NpointsI[i-1];
    if(i>1) cell_arr_size+= NpointsI[i]* NpointsI[i-2];
    if(i>2) cell_arr_size+= NpointsI[i]* NpointsI[i-3];
    //  if(i>3) cell_arr_size+= NpointsI[i]* NpointsI[i-4];
    if(hitsd.at(i).size()>Npoints)
      Npoints=hitsd.at(i).size();
  }
  //  cell_arr_size++;
  if(fVerbose>4) cout<<"Number of possible cells = "<<cell_arr_size<<endl;
  // if(NpointsI[nplanes-1]>NpointsI[0] || NpointsI[nplanes-2]>NpointsI[0]) cell_arr_size*=10000;
  //  if(cell_arr_size>1100) return;
  if(cell_arr_size>100000) cell_arr_size*=0.5;
  ///Build all cells  
  std::vector< std::vector<Double_t> > cells(11,vector<double>(cell_arr_size));
 
  int count = 0;
  for(int j=0;j<(nplanes-1);j++){
    int maxI = NpointsI[j];
    int maxK1 = NpointsI[j+1];
    int maxK2 = 0;
    int maxK3 = 0;
    if(j<(nplanes-2)) maxK2 = NpointsI[j+2];
    if(j<(nplanes-3)) maxK3 = NpointsI[j+3];
    int maxK = maxK1; 
    if(maxK2>maxK) maxK=maxK2;
    if(maxK3>maxK) maxK=maxK3;
    // if(fVerbose>4) cout<<" maxI[j] = "<<maxI<<" maxK1[j+1] = "<<maxK1<<" maxK2[j+2] = "<<maxK2<<endl;
    // cout<<"j="<<j<<endl;
    for(int i=0; i<maxI;i++){ //Assume number of hits is not equal on each plane!
      //  cout<<"i="<<i<<endl;
      for(int k=0; k<maxK;k++){
	//	cout<<"k="<<k<<endl;
	//	if(hitsd.at(j-1).size()==0 || hitsd.at(j).size()==0) continue;
	PndSdsHit *hit0=(PndSdsHit*)fStripHitArray->At(hitsd.at(j).at(i));
	double x0 = hit0->GetX(); double y0 = hit0->GetY(); double z0 = hit0->GetZ();
	if(k<maxK1){
	  PndSdsHit *hit1=(PndSdsHit*)fStripHitArray->At(hitsd.at(j+1).at(k));
	  // if(fVerbose>4)
	  //   cout<<"check hits between "<<j<<" and "<<j+1<<" planes"<<endl;
	  double x1 = hit1->GetX(); double y1 = hit1->GetY(); double z1 = hit1->GetZ();
	  TVector3 dirc(x1-x0,y1-y0,z1-z0);
	  // dirc *= 1./dirc.Mag();
	  ///dTheta of cells to reduce wrong combination
	  htheta->Fill(dirc.Mag(),dirc.Theta());
	  //  if((dirc.Theta()>0.01 && dirc.Mag()>1.) || (dirc.Mag()<0.1 && dirc.Theta()>1.5)){ //in LUMI frame //for point between diff.planes or for point between diff. layes
	  bool goodDir = true;
	  if(dirc.Mag()>1.){
	    if((dirc.Theta()<0.03 && dirc.Theta()>0.05) || fabs(dirc.Phi())>0.25){
	      goodDir = false;
	      //  if(fVerbose>6) cout<<" dirc.Mag()>1. && (dirc.Theta()<0.03 && dirc.Theta()>0.05) || fabs(dirc.Phi())>0.25)"<<endl;
	    }
	  }
	  else{
	    //  if(dirc.Theta()>0.5){
	    if(dirc.Theta()>2){
	      goodDir = false;
	      //   if(fVerbose>6) cout<<" dirc.Mag()<1.&& dirc.Theta()>0.5"<<endl;
	    }
	  }
	 
	if(!goodDir){ //in LUMI frame //for point between diff.planes or for point between diff. layes
	  // if(0>1){ //TEST
	    if(fVerbose>4){
	      cout<<"For cell between #"<<(j)<<"."<<i<<" and #"<<(j+1)<<"."<<k;
	      cout<<" dirc.Theta() = "<<dirc.Theta()<<" dirc.Phi() = "<<dirc.Phi()<<endl;
	    }
	  } 
	  else{
	    if(fVerbose>4){
	      cout<<"GOOD CELL #"<<count<<" [between #"<<(j)<<"."<<i<<" and #"<<j+1<<"."<<k<<"]:";
	      cout<<" dirc.Theta() = "<<dirc.Theta()<<" dirc.Phi() = "<<dirc.Phi()<<endl;
	    }
	    cells.at(0).at(count) = x0;
	    cells.at(1).at(count) = y0;
	    cells.at(2).at(count) = z0;
	    cells.at(3).at(count) = x1;
	    cells.at(4).at(count) = y1;
	    cells.at(5).at(count) = z1;
	    cells.at(6).at(count) = i;
	    cells.at(7).at(count) = k;
	    cells.at(8).at(count) = j;
	    cells.at(9).at(count) = j+1;
	    cells.at(10).at(count) = 0;
	    count++;
	  }
	}

	if(missPlAlgo){
	  //  for(int jp=2;jp<4;jp++){//skip one or two layres 
	  for(int jp=2;jp<3;jp++){//skip one layres 
	    if(j>=(nplanes-jp)) continue;
	    if(k<hitsd.at(j+jp).size()){
	      PndSdsHit *hit2=(PndSdsHit*)fStripHitArray->At(hitsd.at(j+jp).at(k));
	      // if(fVerbose>4)
	      //   cout<<"check hits between "<<j<<" and "<<j+jp<<" planes"<<endl;
	      double x2 = hit2->GetX(); double y2 = hit2->GetY(); double z2 = hit2->GetZ();
	      TVector3 dirc2(x2-x0,y2-y0,z2-z0);
	      //     dirc2 *= 1./dirc2.Mag();
	      ///dTheta of cells to reduce wrong combination
	      //    if(dirc2.Theta()>0.01){ //in LUMI frame
	      htheta->Fill(dirc2.Mag(),dirc2.Theta());
	      //	      if((dirc2.Theta()>0.01 && dirc2.Mag()>1.) || (dirc2.Mag()<0.1 && dirc2.Theta()>1.5)){ //in LUMI frame //for point between diff.planes or for point between diff. layes
	      //  if((dirc2.Theta()<0.03 && dirc2.Theta()>0.05 && fabs(dirc2.Phi())>0.25 && dirc2.Mag()>1.) || (dirc2.Mag()<0.1 && dirc2.Theta()>0.5)){ //in LUMI frame //for point between diff.planes or for point between diff. layes
	      
	      bool goodDir = true;
	      if(dirc2.Mag()>1.){
		if((dirc2.Theta()<0.03 && dirc2.Theta()>0.05) || fabs(dirc2.Phi())>0.25){
		  goodDir = false;
		  // if(fVerbose>6) cout<<" dirc2.Mag()>1. && (dirc2.Theta()<0.03 && dirc2.Theta()>0.05) || fabs(dirc2.Phi())>0.25)"<<endl;
		}
	      }
	      else{
		//	if(dirc2.Theta()>0.5){
		if(dirc2.Theta()>2){
		  //  if(fVerbose>6) cout<<" dirc2.Mag()<1.&& dirc2.Theta()>0.5"<<endl;
		  goodDir = false;
		}
	      }


	    if(!goodDir){ //
	      //   if(0>1){ //TEST
		if(fVerbose>4){
		  cout<<"For cell between #"<<(j)<<"."<<i<<" and #"<<(j+jp)<<"."<<k;
		  cout<<" dirc2.Theta() = "<<dirc2.Theta()<<" dirc2.Phi() = "<<dirc2.Phi()<<endl;
		}
	      } 
	      else{
		if(fVerbose>4){
		  cout<<"GOOD CELL #"<<count<<" [between #"<<(j)<<"."<<i<<" and #"<<j+jp<<"."<<k<<"]:";
		  cout<<" dirc2.Theta() = "<<dirc2.Theta()<<" dirc2.Phi() = "<<dirc2.Phi()<<endl;
		}
		cells.at(0).at(count) = x0;
		cells.at(1).at(count) = y0;
		cells.at(2).at(count) = z0;
		cells.at(3).at(count) = x2;
		cells.at(4).at(count) = y2;
		cells.at(5).at(count) = z2;
		cells.at(6).at(count) = i;
		cells.at(7).at(count) = k;
		cells.at(8).at(count) = j;
		cells.at(9).at(count) = j+jp;
		cells.at(10).at(count) = 0;
		count++;
		// cout<<"CELL saved!"<<endl;
	      }
	    }
	  }
	}
      }
 
    }
  }
 // if(fVerbose>4)
 //   cout<<"Number of good cells:"<<count<<endl;
  //  cout<<"finished cells building!"<<endl;
  for(int icell=0;icell<11;icell++)
    cells.at(icell).erase(cells.at(icell).begin()+count,cells.at(icell).end());
  // if(fVerbose>4)
  //   cout<<"Number of cells after cleaning:"<<cells.at(0).size()<<endl;

  // timer_cook_cells->Stop();
  // Double_t rtime_cook_cells = timer_cook_cells->RealTime();
  // Double_t ctime_cook_cells = timer_cook_cells->CpuTime();
  // cout << "Real time for Cells cooking:" << rtime_cook_cells << " s, CPU time " << ctime_cook_cells << " s" << endl;

  // TStopwatch *timer_neighbors_cells = new TStopwatch();
  // timer_cook_cells->Start();
  //Find neighbors between cells
  vector<int> connect;//save position connected cells
  // for(int itter=0; itter<1; itter++){
  for(int ic=0; ic<cells.at(0).size(); ic++){
    for(int jc=0; jc<cells.at(0).size(); jc++){
      // if(fVerbose>4)
      // 	cout<<"Cells #"<<ic<<" and #"<<jc<<" under check now"<<endl;
      if(ic==jc) continue; 
    // for(int jc=(cells.at(0).size()-1); jc >= 0; jc--){
      if(fabs(cells.at(9).at(ic)-cells.at(8).at(jc))>1e-1) continue;
      if(fabs(cells.at(7).at(ic)-cells.at(6).at(jc))>1e-1) continue;
      // if(fVerbose>4)
      // 	cout<<" still Cells #"<<ic<<" and #"<<jc<<" under check now"<<endl;
      //   cout<<"we are checking now: "<<ic<<" and "<<jc<<" cells"<<endl;
	    /// v - vector of track direction
	    /// w - vector between middle point and another end of cell
	    /// d= [v,w]/|v| - distance
	    /// [v,w]=(v_y*w_z-v_z*w_y, v_z*w_x-v_x*w_z,v_x*w_y-v_y*w_x)
	    /// |v|=sqrt(v_x^2+v_y^2+v_z^2)
	 
	    double x0 = cells.at(0).at(ic); double y0 = cells.at(1).at(ic); double z0 = cells.at(2).at(ic);
	    double x1 = cells.at(3).at(jc); double y1 = cells.at(4).at(jc); double z1 = cells.at(5).at(jc);
	    double xt = cells.at(3).at(ic); double yt = cells.at(4).at(ic); double zt = cells.at(5).at(ic);
	    ///v(x1-x0,y1-y0,z1-z0)
	    ///w(x0-xt,y0-yt,z0-zt)
	    
	    double d_x =(y1-y0)*(z0-zt)-(z1-z0)*(y0-yt);
	    double d_y =(z1-z0)*(x0-xt)-(x1-x0)*(z0-zt);
	    double d_z =(x1-x0)*(y0-yt)-(y1-y0)*(x0-xt);
	    
	    double norm_v = sqrt(pow((x1-x0),2)+pow((y1-y0),2)+pow((z1-z0),2));
	    double d = sqrt(d_x*d_x+d_y*d_y+d_z*d_z)/norm_v;
	    hdist->Fill(d);
	    if(d<d_max){
	      // if(fVerbose>4)
	      // 	cout<<"BINGO! Cells #"<<ic<<" and #"<<jc<<endl;
	      //  if(int(cells.at(10).at(ic))==int(cells.at(10).at(jc))){
		cells.at(10).at(jc)++;
		connect.push_back(ic);
		connect.push_back(jc);
		//  }
	      // else{
	      // 	if(fVerbose>4)
	      // 	  cout<<"Ooops, smth wrong with cells #"<<ic<<" and #"<<jc<<" "<<cells.at(10).at(ic)<<" "<<cells.at(10).at(jc)<<endl;
	      // }
	    }
	    else{
	      if(fVerbose>4)
		cout<<"Cells #"<<ic<<" and #"<<jc<<" aren't connected, because d = "<<d<<" >= dmax!"<<endl;
	    }
    }
  }

  /// Set counters values -----
  const int cellsSize = cells.at(0).size();
  int pv_new[cellsSize];
  for(int icv=0;icv<cellsSize;icv++)
    pv_new[icv]=0;

  for(int itter=0;itter<2*nplanes;itter++){
    for(int con=0;con<connect.size();){
      int con1 = connect[con];
      int con2 = connect[con+1];
      if(cells.at(10).at(con1)==cells.at(10).at(con2) && pv_new[con2]==cells.at(10).at(con2)){
	pv_new[con1] = cells.at(10).at(con1);
	pv_new[con2] = cells.at(10).at(con2)+1;
      }
      con++;
      con++;
    }
    for(int icv=0;icv<cellsSize;icv++)
      cells.at(10).at(icv) = pv_new[icv];
  }
  ///--------------------------

  //  }
  // timer_neighbors_cells->Stop();
  // Double_t rtime_neighbors_cells = timer_neighbors_cells->RealTime();
  // Double_t ctime_neighbors_cells = timer_neighbors_cells->CpuTime();
  // cout << "Real time for neighbors between cells search :" << rtime_neighbors_cells << " s, CPU time " << ctime_neighbors_cells << " s" << endl;
 // if(fVerbose>4)
 //   cout<<"Number of connected cells "<<connect.size()<<endl;
  if(fVerbose>4){
    cout<<"connect:"<<endl;
    for(int con=0;con<connect.size();){
      int con1 = connect[con];
      int con2 = connect[con+1];
      cout<<" "<<connect[con]<<"("<<cells.at(10).at(con1)<<") "<<connect[con+1]<<"("<<cells.at(10).at(con2)<<") "<<endl;
      con++;
      con++;
    }
  }

  // TStopwatch *timer_build_trk_combinations = new TStopwatch();
  // timer_build_trk_combinations->Start();

  //Build track from cells combination --------------------------------

  //find max number of cells in a track
  int pcmax = 0;
  for(int cid=1;cid<cells.at(10).size();cid++){
    int tag_cur = cells.at(10).at(cid);
    if(tag_cur>pcmax) pcmax = tag_cur;
  }
  if(fVerbose>4) cout<<"track can contain "<<pcmax<<"+1 cells"<<endl;
  const int trk_arr_size = pcmax+1;
  //  const int trk_arr_size = pcmax+2;
  std::vector< std::vector<int> > trk_cells(trk_arr_size);
  int trk_count=-1;
  for(int newpcmax=pcmax;newpcmax>0;newpcmax--){// loop over possible number of cells in trk
    int cur_max_tag = newpcmax;
    if(fVerbose>4) cout<<"Now we are looking for trk with max "<<cur_max_tag+1<<" cells"<<endl;
    bool newtrk=true;
    for(int con=connect.size()-1;con>=0;){// loop over all connections between cells
      bool nextloop=false;
      int con1,con2;
      int curr_arr;
      if(newtrk){// here we check both cells
	//check last array in trk_cells was filled by data
	bool add_new=false;
	if(trk_count<0) add_new=true;
	else{
	  curr_arr=trk_arr_size;
	  while(curr_arr>0){
	    //	    cout<<"curr_arr = "<<curr_arr<<endl;
	    curr_arr--;
	    if(trk_cells.at(curr_arr).at(trk_count)>0) add_new=true;
	  }
	}
	if(add_new){
	  if(fVerbose>4)	cout<<"**************** Hey, here comes new trk-cand! *****************"<<endl;
	  trk_count++;//increase counter for trk-candidates
	  //sizes of cells arrays in trk should be the same
	  curr_arr=trk_arr_size;
	  while(curr_arr>0){
	    curr_arr--;
	    trk_cells.at(curr_arr).push_back(-1);
	  }
	}

	cur_max_tag = newpcmax;
	con2 = connect[con];
	con--;
	con1 = connect[con];
	con--;
	if((cells.at(10).at(con2))==cur_max_tag) nextloop=true;
      }
      else{// here we check only one cell, another one is already known
	con2 = con1; //trk should be connected to cell from previous part
	for(int coni=connect.size()-1;coni>=0;){
	  if(con2==connect[coni]){
	    coni--;
	    con1 = connect[coni];
	    nextloop=true;
	  }
	  coni -=2;
	}
	if(con2==con1) break;
	//	cout<<"	con2 = "<<con2<<" con1 = "<<con1<<endl;
      }
      if(!nextloop) continue;
      while(cur_max_tag>0){// here we know cell_con2 and cell_con1
	// cout<<"cells.at(10).at("<<con2<<") = "<<cells.at(10).at(con2)<<endl;
	// cout<<"cells.at(10).at("<<con1<<") = "<<cells.at(10).at(con1)<<endl;
	if((cells.at(10).at(con2))==cur_max_tag){
	  //  cout<<"cur_max_tag = "<<cur_max_tag<<endl;
	  cur_max_tag -=1;
	  if((cells.at(10).at(con1))==cur_max_tag){// difference in tags = 1
	    //  cout<<"cur_max_tag = "<<cur_max_tag<<endl;
	    // cur_max_tag -=1;
	    // trk_cells.at(cur_max_tag+2).at(trk_count)=con2;
	    // trk_cells.at(cur_max_tag+1).at(trk_count)=con1;
	    trk_cells.at(cur_max_tag+1).at(trk_count)=con2;
	    trk_cells.at(cur_max_tag).at(trk_count)=con1;
	    // con--;
	    // con--;
	    //	    cout<<"for new loop: cur_max_tag = "<<cur_max_tag<<" con = "<<con<<endl;
	    if(cur_max_tag>0) newtrk=false; // we are looking for others cells in trk
	    if(cur_max_tag>0) break;
	    else{
	      //  cout<<"New trk search will be started!"<<endl;
	      newtrk=true; 
	    }
	    //   if(cur_max_tag>0) continue;
	    // else{
	    //   for(int conN=0;conN<connect.size();){
	    // 	int con3 = connect[conN];
	    // 	conN++;
	    // 	int con4 = connect[conN];
	    // 	conN++;
	    // 	if(con4==con1){
	    // 	  trk_cells.at(cur_max_tag).at(trk_count)=con3;
	    // 	  if(fVerbose>4)    cout<<" con3:"<<con3<<endl;
	    // 	}
	    //   }
	    // }
	  }
	  // else{
	  //   cur_max_tag--;
	  //   if((cells.at(10).at(con1))==cur_max_tag){// difference in tags = 2
	  //     if(fVerbose>4) cout<<" con2:"<<con2<<" con1:"<<con1<<endl;
	  //     trk_cells.at(cur_max_tag+2).at(trk_count)=con2;
	  //     trk_cells.at(cur_max_tag+1).at(trk_count)=con1;
	  //     if(cur_max_tag>0) newtrk=false;
	  //     if(cur_max_tag>0) continue;
	  //     else{
	  // 	for(int conN=0;conN<connect.size();){
	  // 	  int con3 = connect[conN];
	  // 	  conN++;
	  // 	  int con4 = connect[conN];
	  // 	  conN++;
	  // 	  if(con4==con1){
	  // 	    trk_cells.at(cur_max_tag).at(trk_count)=con3;
	  // 	    if(fVerbose>4)    cout<<" con3:"<<con3<<endl;
	  // 	  }
	  // 	}
	  //     }
	  //   }
	  // }
	}
	else
	  break;
      }
      //      cout<<"con = "<<con<<endl;
    }
    //    cout<<"newpcmax = "<<newpcmax<<endl;
  }
  
  //fillter -----------------------------------------------
  if(fVerbose>4) cout<<"--- fillter trk-cand array: "<<endl;
  if(fVerbose>4) cout<<" Attention each trk candidate contain repeated cell, but smaller cells number will be deleted!"<<endl;
  vector<int> cell_parts;
  vector<bool> trk_accept;
  for(int itrk=0;itrk<trk_cells.at(0).size();itrk++){
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
    //    trk_accept.push_back(true);
    if(fVerbose>4) cout<<" with:"<<maxcellnum<<" cells"<<endl;
  }
  
  for(int itrc=0;itrc<cell_parts.size();itrc++){
    if(!trk_accept[itrc]) continue;
    for(int itrc2=0;itrc2<cell_parts.size();itrc2++){
      if(!trk_accept[itrc2]) continue;
      if(cell_parts[itrc]<cell_parts[itrc2]){
	int count_re=0;
	//reject trk-cand with smaller number of cells and similar cells
	int curr_arr=trk_arr_size;
	while(curr_arr>0){
	  curr_arr--;
	  if(trk_cells.at(curr_arr).at(itrc)<0 || trk_cells.at(curr_arr).at(itrc2)<0) continue;
	  if(trk_cells.at(curr_arr).at(itrc)==trk_cells.at(curr_arr).at(itrc2))
	    count_re++;
	}
	if(count_re>0.3*cell_parts[itrc]){
	  trk_accept[itrc]=false;
	  if(fVerbose>4){
	    // cout<<"Delete: trk-cand#"<<itrc<<" because of trk_cells.at("<<curr_arr<<").at("<<itrc<<"):"
	    // 	<<trk_cells.at(curr_arr).at(itrc)<<endl;
	    cout<<"Delete: trk-cand#"<<itrc
		<<" because it contains ("<<count_re<<") more then 30% cells from trk-cand#"<<itrc2<<endl;
	  }
	}
      }
    }
  }

  for(int itrc=0;itrc<(cell_parts.size()/2);itrc++){
    for(int itrc2=(cell_parts.size()/2);itrc2<cell_parts.size();itrc2++){
      if(cell_parts[itrc]==cell_parts[itrc2]){
	//reject trk-cand with the same number of cells and similar cells
	int count_re=0;
	int curr_arr=trk_arr_size;
	while(curr_arr>0){
	  curr_arr--;
	  if(trk_cells.at(curr_arr).at(itrc)<0 || trk_cells.at(curr_arr).at(itrc2)<0) continue;
	  if(trk_cells.at(curr_arr).at(itrc)==trk_cells.at(curr_arr).at(itrc2)){
	    count_re++;
	  }
	}
	if(count_re>0.7*cell_parts[itrc]){
	  trk_accept[itrc]=false;
	  if(fVerbose>4){
	    cout<<"Delete: trk-cand#"<<itrc<<" because it contains("
		<<count_re<<") more then 70% of cells from trk-cand#"<<itrc2<<endl;
	  }
	}
      }
    }
  }

  // timer_build_trk_combinations->Stop();
  // Double_t rtime_build_trk_combinations = timer_build_trk_combinations->RealTime();
  // Double_t ctime_build_trk_combinations = timer_build_trk_combinations->CpuTime();
  // cout << "Real time for build trk combinations :" << rtime_build_trk_combinations << " s, CPU time " << ctime_build_trk_combinations << " s" << endl;

  // TStopwatch *timer_save_trks = new TStopwatch();
  // timer_save_trks->Start();
  //save trk-cands ----------------------------------------
  int NtrkRec = 0;
  //  for(int itrk=0;itrk<trk_cells.at(pcmax).size();itrk++){
  for(int itrk=0;itrk<trk_accept.size();itrk++){
    if(!trk_accept[itrk]) continue;
    PndTrackCand *myTCand = new PndTrackCand();
    bool firstHit=true;
    TVector3 dir;
    for(int icell=0;icell<trk_arr_size;icell++){
      int cellNum = trk_cells.at(icell).at(itrk);
      if(cellNum<0) continue;
      if(firstHit){//fill trk-cand seed
	int id = cells.at(6).at(cellNum);
	int pl = cells.at(8).at(cellNum);
	if(fVerbose>3) cout<<"Trk uses hit#"<<id<<" from pl#"<<pl<<endl;
	PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(hitsd.at(pl).at(id)));
	PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
	myTCand->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Z()); 
	//	myTCand->AddHit(0,hitsd.at(pl).at(id),myHit->GetPosition().Z()); 

	double startX = cells.at(0).at(cellNum);
	double startY = cells.at(1).at(cellNum);
	double startZ = cells.at(2).at(cellNum);
	TVector3 posSeed(startX,startY,startZ);
	int cellNumNext=trk_cells.at(icell+1).at(itrk);
	if(cellNumNext<0){//next plane is missing
	  cellNumNext=trk_cells.at(icell+2).at(itrk);
	}
	double dirX = cells.at(3).at(cellNumNext)-cells.at(0).at(cellNum);
	double dirY = cells.at(4).at(cellNumNext)-cells.at(1).at(cellNum);
	double dirZ = cells.at(5).at(cellNumNext)-cells.at(2).at(cellNum);
	dir.SetXYZ(dirX,dirY,dirZ);
	if(fVerbose>3)
	  cout<<"Trk-cand direction is taking based on "<<dir.Mag()<<" cm"<<endl;

	dir*=1./dir.Mag();
	//shift trk out of plane [needed for correct treatment in Kalman Fillter and GEANE]
	double sh_z = -0.02; //200 mkm
	double sh_x = dirX*sh_z;
	double sh_y = dirY*sh_y;
	TVector3 sh_point(sh_x,sh_y,sh_z);
	posSeed +=sh_point;
	// if(fVerbose>2){
	//   cout<<"posSeed:"<<endl;
	//   posSeed.Print();
	//   cout<<"dirSeed:"<<endl;
	//   dir.Print();
	// }
	
	myTCand->setTrackSeed(posSeed,dir,-1);
	firstHit=false;
      }
      int id = cells.at(7).at(cellNum);
      int pl = cells.at(9).at(cellNum);
      if(fVerbose>3) cout<<"Trk uses hit#"<<id<<" from pl#"<<pl<<endl;
      PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(hitsd.at(pl).at(id)));
      PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
      PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
      myTCand->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Z()); 
      // myTCand->AddHit(0,hitsd.at(pl).at(id),myHit->GetPosition().Z()); 
    }
    if(fVerbose>3){
      if((dir.Theta()<0.03 && dir.Theta()>0.05) || fabs(dir.Phi())>0.25)
	cout<<"Ooops, trk-cand has: theta="<<dir.Theta()<<" and phi="<<dir.Phi()<<endl;
      }
    if((dir.Theta()<0.03 && dir.Theta()>0.05) || fabs(dir.Phi())>0.25) continue; //TEST
    const int numPts = myTCand->GetNHits(); //read how many points in this track
    if(numPts<1) cout<<"!!! Attention HERE is problem: number of hits in trk-cand = "<<numPts<<"!!!"<<endl;
    new((*fTrackCandArray)[NtrkRec]) PndTrackCand(*(myTCand)); //save Track Candidate
    //    new((*fTrackCandArrayTemp)[NtrkRec]) PndTrackCand(*(myTCand)); //save Track Candidate
    NtrkRec++;
    if(fVerbose>3) cout<<"Ntrk No. "<<NtrkRec<<endl;
    delete myTCand;
  }
  // timer_save_trks->Stop();
  // Double_t rtime_save_trks = timer_save_trks->RealTime();
  // Double_t ctime_save_trks = timer_save_trks->CpuTime();
  // cout << "Real time for save trks :" << rtime_save_trks << " s, CPU time " << ctime_save_trks << " s" << endl;
  
  if(fVerbose>2){
    Int_t ntcandFin=fTrackCandArray->GetEntriesFast();
    cout<<"Number of Trk-Cands is "<<ntcandFin<<endl;
    timer_exec->Stop();
    Double_t rtime_exec = timer_exec->RealTime();
    Double_t ctime_exec = timer_exec->CpuTime();
    cout << "Real time for Exec:" << rtime_exec << " s, CPU time " << ctime_exec << " s" << endl;
    cout << endl;
    cout<< "Evt finsihed--------------"<<endl<<endl;
  }

  
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


// -------------------------------------------------------------------------
ClassImp(PndLmdTrackFinderCATask);

