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
  flagTrkCandCuts=true;
  fHitBranchStrip = "LMDHitsStrip";
  fClusterBranchStrip = "LMDStripClusterCand";
  fDigiBranchStrip = "LMDStripDigis";
  
  dXY = 0.5;
  rule_max = 1e-6;
  hdist = new TH1D("hdist","distance from common point",1e4,0,10.);
  hcosPSI = new TH1D("hcosPSI","",1e4,0,1e-4);
  //  htthetatphi = new TNtuple("htthetatphi","ntthetatphi","tg_theta:tg_phi");
  htthetatphiTrk = new TNtuple("htthetatphiTrk","ntthetatphiTrk","tg_theta:tg_phi:nHits");
  htthetatphiCells = new TNtuple("htthetatphiCells","ntthetatphiCells","tg_theta:tg_phi:x0:y0:z0:x1:y1:z1");
  //  hthetaphi = new TH2D("hthetaphi",";#theta;#phi",1e3,0,1.,1e3,-3.15,3.15);
  /// hcosPSI = new TH1D("hcosPSI","breaking angle",1e3,-1.5,1.5);
  //  htheta = new TH2D("htheta",";length;#theta angle",1e3,0,25,1e3,0,3.15);
  //  htime = new TH2D("htime",";time distance;time angle",1e2,0,10,1e2,0,10);
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
  //  htthetatphi = new TH2D("htthetatphi",";tg#theta;tg#phi",1e3,0,10,1e3,-10,10);
  htthetatphiTrk = new TNtuple("htthetatphiTrk","ntthetatphiTrk","tg_theta:tg_phi:nHits");
  htthetatphiCells = new TNtuple("htthetatphiCells","ntthetatphiCells","tg_theta:tg_phi:x0:y0:z0:x1:y1:z1");
  //  hthetaphi = new TH2D("hthetaphi",";#theta;#phi",1e3,0,1.,1e3,-3.15,3.15);
  hcosPSI= new TH1D("hcosPSI","",1e4,0,1e-4);
  //  hcosPSI = new TH1D("hcosPSI","breaking angle",1e3,-1.5,1.5);
  //  htheta = new TH2D("htheta",";length;#theta angle",1e3,0,25,1e3,0,3.15);
  //  htime = new TH2D("htime",";time distance, ns;time angle,ns",5e3,0,5e3,5e3,0,5e3);
  rule_max = setdmax; 
  nSensPP = innSensPP;
  nP = innP;
  flagStipSens = false;//default
  flagPixelSens = true;//default
  //   dXY = 0.01;//TEST
}


// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndLmdTrackFinderCATask::~PndLmdTrackFinderCATask()
{

  // if(fVerbose<2) {
  //   delete hdist;
  //   delete htthetatphi;
  //   delete hthetaphi;
  // }
  // else{
  // }
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
    for(unsigned int idet = 0; idet < detZ.size(); idet++){
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
  //  cout<<"Attention! detZ.size()="<<detZ.size()<<endl;

//sort in plane's
  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));

    Double_t z = myHit->GetZ();
    for(unsigned int idet = 0; idet < detZ.size(); idet++){ //planes
     
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
  // TStopwatch *timer_exec = new TStopwatch();
  // if(fVerbose>0){
  //   timer_exec->Start();
  //   cout << "Evt started--------------"<<endl<<endl;
  // }
  
  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Clear();
  Int_t nStripHits = fStripHitArray->GetEntriesFast();
  if(nStripHits<2){
    if(fVerbose>2) cout << "Evt finsihed: too less hits-----"<<endl<<endl;
    return;
  }
  //  if(fVerbose>9){ 
    if(nStripHits!=4){
      cout << "!!! TEST cuts on CA cells Evt finsihed: too many hits-----"<<endl<<endl;
      return;
    }
    //  }
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

  
  //print hits (debug)
  TVector3 start, tmp, vec, dstart, dvec; //temp-vars
  if(fVerbose>2){
    for(Int_t iPlane = 0; iPlane < nP; iPlane++){
      if(hitsd.at(iPlane).size()>0){
	for (unsigned int i=0; i<hitsd.at(iPlane).size(); i++){
	  PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(iPlane).at(i));
	  cout<<"Plane"<<iPlane<<" Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<") with err=("
	      <<hit->GetDx()<<", "<<hit->GetDy()<<", "<<hit->GetDz()<<")"<<", MChitID = "<<hit->GetRefIndex()<<endl;
	}
      }
    }
  }

    // cout<<"array preparation: "<<endl;
    // timer_array->Print();
    // timer_array->Stop();

  // TStopwatch *timer_cook_cells = new TStopwatch();
  // if(fVerbose>0){
  //   timer_cook_cells->Start();
  // }
  //  int Npoints=0;
  const unsigned int nplanes = nP;
  // unsigned int NpointsI[nplanes];
  std::vector<unsigned int> NpointsI;
  //  int cell_arr_size=0;
  for(unsigned int i=0;i<nplanes;i++){
    NpointsI.push_back(hitsd.at(i).size());
  }
  ///Build all cells  
  //  std::vector< std::vector<Double_t> > cells(11,std::vector<double>(cell_arr_size));
  //  std::vector< std::vector<double> > cells(11,std::vector<double>());//= new std::vector< std::vector<Double_t> > cells(11,std::vector<double>());
  std::vector<double> cells0;//x0
  std::vector<double> cells1;//y0
  std::vector<double> cells2;//z0
  std::vector<double> cells3;//x1
  std::vector<double> cells4;//y1
  std::vector<double> cells5;//z1
  std::vector<unsigned int> cells6;//hit0 id
  std::vector<unsigned int> cells7;//hit1 id
  std::vector<unsigned int> cells8;//hit0 plane
  std::vector<unsigned int> cells9;//hit1 plane
  std::vector<unsigned int> cells10;//pos.value
 
  int count = 0;
  for(unsigned int j=0;j<(nplanes-1);j++){
    unsigned int maxI = NpointsI[j];
    unsigned int maxK1 = NpointsI[j+1];
    unsigned int maxK2 = 0;
    unsigned int maxK3 = 0;
    if(j<(nplanes-2)){
      maxK2 = NpointsI[j+2];
    }
    else{
      if(j<(nplanes-3)) maxK3 = NpointsI[j+3];
    }
    unsigned int maxK = maxK1; 
    if(maxK2>maxK) maxK=maxK2;
    if(maxK3>maxK) maxK=maxK3;
    // if(fVerbose>4) cout<<" maxI[j] = "<<maxI<<" maxK1[j+1] = "<<maxK1<<" maxK2[j+2] = "<<maxK2<<endl;
    // cout<<"j="<<j<<endl;
    for(unsigned int i=0; i<maxI;i++){ //Assume number of hits is not equal on each plane!
      PndSdsHit *hit0=(PndSdsHit*)fStripHitArray->At(hitsd.at(j).at(i));
      double x0 = hit0->GetX(); double y0 = hit0->GetY(); double z0 = hit0->GetZ();
      for(unsigned int k=0; k<maxK;k++){
	if(k<maxK1){
	  PndSdsHit *hit1=(PndSdsHit*)fStripHitArray->At(hitsd.at(j+1).at(k));
	  double x1 = hit1->GetX(); double y1 = hit1->GetY(); double z1 = hit1->GetZ();
	  ////// TEST tgTh vs. tgPhi cells -------------------
	  if(fVerbose>4){	 
	    double xvec = x1-x0; 	  double yvec = y1-y0; 	  double zvec = z1-z0;
	    double tgPhi = yvec/xvec;
	    double tgTheta = sqrt(xvec*xvec+yvec*yvec)/zvec;
	    htthetatphiCells->Fill(tgTheta,tgPhi,x0,y0,z0,x1,y1,z1);
	  }
	  //////-----------------------------------------------------



	  // bool goodDir = true;
	  // if(flagTrkCandCuts){
	  //   double xvec = x1-x0; 	  double yvec = y1-y0; 	  double zvec = z1-z0;
	  //   double tgPhi = yvec/xvec;
	  //   if(tgPhi<0) tgPhi*=-1;
	  //   if(tgPhi>0.25){
	  //     goodDir = false;
	  //   }
	  //   else{
	  //     double tgTheta = sqrt(xvec*xvec+yvec*yvec)/zvec;
	  //     if(tgTheta<0.03 || tgTheta>0.05){
	  // 	goodDir = false;
	  //     }
	  //   }
	  // }
	  // if(fVerbose>4){	 
	  //   double xvec = x1-x0; 	  double yvec = y1-y0; 	  double zvec = z1-z0;
	  //   double tgPhi = yvec/xvec;
	  //   double tgTheta = sqrt(xvec*xvec+yvec*yvec)/zvec;
	  //   htthetatphi->Fill(tgTheta,tgPhi);
	  //   if(!goodDir)
	  //     cout<<"BAD cell between #"<<(j)<<"."<<i<<" and #"<<(j+1)<<"."<<k
	  // 	  <<" (tgTheta,tgPhi)=("<<tgTheta<<","<<tgPhi<<")"<<endl;
       	  //   else
	  //     cout<<"GOOD CELL #"<<count<<" between #"<<(j)<<"."<<i<<" and #"<<j+1<<"."<<k<<endl;
	  // }
	  // if(goodDir){
	    cells0.push_back(x0);
	    cells1.push_back(y0);
	    cells2.push_back(z0);
	    cells3.push_back(x1);
	    cells4.push_back(y1);
	    cells5.push_back(z1);
	    cells6.push_back(i);
	    cells7.push_back(k);
	    cells8.push_back(j);
	    cells9.push_back(j+1);
	    cells10.push_back(0);
	    count++;
	    //	  }
	}
      
	if(missPlAlgo){
	  //  for(int jp=2;jp<4;jp++){//skip one or two layres 
	  for(unsigned int jp=2;jp<3;jp++){//skip one layres 
	    if(j>=(nplanes-jp)) continue;
	    if(k<hitsd.at(j+jp).size()){
	      PndSdsHit *hit2=(PndSdsHit*)fStripHitArray->At(hitsd.at(j+jp).at(k));
	      double x2 = hit2->GetX(); double y2 = hit2->GetY(); double z2 = hit2->GetZ();
	      if(fVerbose>4){	 
		double xvec = x2-x0; 	  double yvec = y2-y0; 	  double zvec = z2-z0;
		double tgPhi = yvec/xvec;
		double tgTheta = sqrt(xvec*xvec+yvec*yvec)/zvec;
		htthetatphiCells->Fill(tgTheta,tgPhi,x0,y0,z0,x2,y2,z2);
	      }
	      // bool goodDir = true;
	      // if(flagTrkCandCuts){
	      // 	double xvec = x2-x0; 	  double yvec = y2-y0; 	  double zvec = z2-z0;
	      // 	double tgPhi = yvec/xvec;
	      // 	if(tgPhi<0) tgPhi*=-1;
	      // 	if(tgPhi>0.25){
	      // 	  goodDir = false;
	      // 	}
	      // 	else{
	      // 	  double tgTheta = sqrt(xvec*xvec+yvec*yvec)/zvec;
	      // 	  if(tgTheta<0.03 || tgTheta>0.05){
	      // 	    goodDir = false;
	      // 	  }
	      // 	}
	      // }
	      // if(fVerbose>4){
	      // 	double xvec = x2-x0; 	  double yvec = y2-y0; 	  double zvec = z2-z0;
	      // 	double tgPhi = yvec/xvec;
	      // 	double tgTheta = sqrt(xvec*xvec+yvec*yvec)/zvec;
	      // 	htthetatphi->Fill(tgTheta,tgPhi);
	      // 	if(!goodDir)
	      // 	  cout<<"BAD cell between #"<<(j)<<"."<<i<<" and #"<<(j+jp)<<"."<<k
	      // 	      <<" (tgTheta,tgPhi)=("<<tgTheta<<","<<tgPhi<<")"<<endl;
	      // 	else
	      // 	  cout<<"GOOD CELL #"<<count<<" [between #"<<(j)<<"."<<i<<" and #"<<j+jp<<"."<<k<<endl;
	      // }
	      // if(goodDir){
	      cells0.push_back(x0);
	      cells1.push_back(y0);
	      cells2.push_back(z0);
	      cells3.push_back(x2);
	      cells4.push_back(y2);
	      cells5.push_back(z2);
	      cells6.push_back(i);
	      cells7.push_back(k);
	      cells8.push_back(j);
	      cells9.push_back(j+jp);
	      cells10.push_back(0);

	      count++;
	      //    }
	      // cout<<"CELL saved!"<<endl;
	    }
	  }
	}
      }
    }
  }
 // if(fVerbose>4)
 //   cout<<"Number of good cells:"<<count<<endl;
  //  cout<<"finished cells building!"<<endl
  // for(int icell=0;icell<11;icell++)
  //   cells.at(icell).erase(cells.at(icell).begin()+count,cells.at(icell).end());
  // if(fVerbose>4)
  //   cout<<"Number of cells after cleaning:"<<cells.at(0).size()<<endl;
  // if(fVerbose>0){
  //   timer_cook_cells->Stop();
  //   Double_t rtime_cook_cells = timer_cook_cells->RealTime();
  //   Double_t ctime_cook_cells = timer_cook_cells->CpuTime();
  //   cout << "Real time for Cells cooking:" << rtime_cook_cells << " s, CPU time " << ctime_cook_cells << " s" << endl;
  // }

  // TStopwatch *timer_neighbors_cells = new TStopwatch();
  // if(fVerbose>0)
  //   timer_cook_cells->Start();

  //Find neighbors between cells
  vector<unsigned int> connect;//save position connected cells
  // for(int itter=0; itter<1; itter++){
  for(int ic=0; ic<cells0.size(); ic++){
    //    for(int jc=0; jc<cells0.size(); jc++){
    for(int jc=ic+1; jc<cells0.size(); jc++){
      // if(fabs(cells9.at(ic)-cells8.at(jc))>1e-1) continue;
      // if(fabs(cells7.at(ic)-cells6.at(jc))>1e-1) continue;
      if((cells9.at(ic))!=(cells8.at(jc))) continue;
      if((cells7.at(ic))!=(cells6.at(jc))) continue;

	    // /// v - vector of track direction
	    // /// w - vector between middle point and another end of cell
	    // /// d= [v,w]/|v| - distance
	    // /// [v,w]=(v_y*w_z-v_z*w_y, v_z*w_x-v_x*w_z,v_x*w_y-v_y*w_x)
	    // /// |v|=sqrt(v_x^2+v_y^2+v_z^2)
	 
	    // double x0 = cells0.at(ic); double y0 = cells1.at(ic); double z0 = cells2.at(ic);
	    // double x1 = cells3.at(jc); double y1 = cells4.at(jc); double z1 = cells5.at(jc);
	    // double xt = cells3.at(ic); double yt = cells4.at(ic); double zt = cells5.at(ic);

	    // double dx10 = (x1-x0); double dy10 = (y1-y0); 	double dz10 = (z1-z0);
	    // double dx0t = (x0-xt);  double dy0t = (y0-yt);    double dz0t = (z0-zt);

	    // double d_x2 = pow((dy10*dz0t-dz10*dy0t),2);
	    // double d_y2 = pow((dz10*dx0t-dx10*dz0t),2);
	    // double d_z2 = pow((dx10*dy0t-dy10*dx0t),2);
	    
	    // //	    double norm_v = sqrt(pow((x1-x0),2)+pow((y1-y0),2)+pow((z1-z0),2));
	    // double norm_v = dz10;
	    // if(10*dx10 > dz10){
	    // double dx10_2 = (x1-x0)*(x1-x0); double dy10_2 = (y1-y0)*(y1-y0); double dz10_2 = (z1-z0)*(z1-z0);
	    // norm_v = sqrt(dx10_2+dy10_2+dz10_2);
	    // }
	    // double d = sqrt(d_x2+d_y2+d_z2)/norm_v;
	    // // timerD.Stop();

	    // // Double_t rtimeD = 1e9*(timerD.RealTime());
	    // // Double_t ctimeD = 1e9*(timerD.CpuTime());
	    // // cout<<"Timer for distance calculation: real "<<rtimeD<<" s, CPU "<<ctimeD<<endl;
	    // // cout<<"distance = "<<d<<endl;
	    // //	    timerD.Reset();
	    // if(fVerbose>1) hdist->Fill(d);

	    // TStopwatch timerA;
	    // timerA.Start();
	 
	    // // double x0 = cells.at(0).at(ic); double y0 = cells.at(1).at(ic); double z0 = cells.at(2).at(ic);
	    // // double x1 = cells.at(3).at(jc); double y1 = cells.at(4).at(jc); double z1 = cells.at(5).at(jc);
	    // // double xt = cells.at(3).at(ic); double yt = cells.at(4).at(ic); double zt = cells.at(5).at(ic);
	    double Ax = cells3.at(ic) - cells0.at(ic);  
	    double Ay = cells4.at(ic) - cells1.at(ic); 
	    double Az = cells5.at(ic) - cells2.at(ic); 
	    double Bx = cells3.at(jc) - cells3.at(ic);  
	    double By = cells4.at(jc) - cells4.at(ic); 
	    double Bz = cells5.at(jc) - cells5.at(ic); 
	    double AB = Ax*Bx+Ay*By+Az*Bz;
	    double Amag = sqrt(Ax*Ax+Ay*Ay+Az*Az);
	    double Bmag = sqrt(Bx*Bx+By*By+Bz*Bz);
	    double cosPsi = AB/(Amag*Bmag);
	    //	    double cosPsi = AB/double(Az*Bz);
	    //  timerA.Stop();
	    if(fVerbose>1)  hcosPSI->Fill(1-cosPsi);

	    // Double_t rtimeA = 1e9*(timerA.RealTime());
	    // Double_t ctimeA = 1e9*(timerA.CpuTime());
	    // htime->Fill(rtimeD,rtimeA);
	    //	    timerA.Reset();
	    //	     cout<<"Timer for angle calculation: real "<<rtimeA<<" s, CPU "<<ctimeA<<endl;
	    // cout<<"cosPsi = "<<cosPsi<<endl;
	    //	    if(d<rule_max){
	    //	    if((1-cosPsi)<1e-5){// 1e-5 OK for 1.5 GeV;
	    if((1-cosPsi)<rule_max){// 1e-5 OK for 15 GeV;
	      // if(fVerbose>4)
	      // 	cout<<"BINGO! Cells #"<<ic<<" and #"<<jc<<endl;
	      //  if(int(cells.at(10).at(ic))==int(cells.at(10).at(jc))){
		cells10.at(jc)++;
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
		//	cout<<"Cells #"<<ic<<" and #"<<jc<<" aren't connected, because d = "<<d<<" >="<<rule_max<<endl;
		cout<<"Cells #"<<ic<<" and #"<<jc<<" aren't connected, because  (1-cosPsi) = "<<1-cosPsi<<endl;
	    }
    }
  }

  /// Set counters values -----
  const unsigned int cellsSize = cells0.size();
  //  int pv_new[cellsSize];
  std::vector<unsigned int> pv_new;
  for(unsigned int icv=0;icv<cellsSize;icv++)
    pv_new.push_back(0);

  for(unsigned int itter=0;itter<2*nplanes;itter++){
  //  for(unsigned int itter=0;itter<nplanes;itter++){
    for(unsigned int con=0;con<connect.size();){
      unsigned int con1 = connect[con];
      unsigned int con2 = connect[con+1];
      if(cells10.at(con1)==cells10.at(con2) && pv_new[con2]==cells10.at(con2)){
	pv_new[con1] = cells10.at(con1);
	pv_new[con2] = cells10.at(con2)+1;
      }
      // con++;
      // con++;
      con+=2;
    }
    for(unsigned int icv=0;icv<cellsSize;icv++)
      cells10.at(icv) = pv_new[icv];
  }
  ///--------------------------

  //  }
  // if(fVerbose>0){
  //   timer_neighbors_cells->Stop();
  //   Double_t rtime_neighbors_cells = timer_neighbors_cells->RealTime();
  //   Double_t ctime_neighbors_cells = timer_neighbors_cells->CpuTime();
  //   cout << "Real time for neighbors between cells search :" << rtime_neighbors_cells << " s, CPU time " << ctime_neighbors_cells << " s" << endl;
  // }
 // if(fVerbose>4)
 //   cout<<"Number of connected cells "<<connect.size()<<endl;
  if(fVerbose>4){
    cout<<"connect:"<<endl;
    for(unsigned int con=0;con<connect.size();){
      unsigned int con1 = connect[con];
      unsigned int con2 = connect[con+1];
      cout<<" "<<connect[con]<<"("<<cells10.at(con1)<<") "<<connect[con+1]<<"("<<cells10.at(con2)<<") "<<endl;
      con++;
      con++;
    }
  }

  // TStopwatch *timer_build_trk_combinations = new TStopwatch();
  // if(fVerbose>0)
  //   timer_build_trk_combinations->Start();

  // TStopwatch *timer_build_all_trk_combinations = new TStopwatch();
  // if(fVerbose>0)
  //   timer_build_all_trk_combinations->Start();

  //Build track from cells combination --------------------------------

  //find max number of cells in a track
  unsigned int pcmax = 0;
  for(unsigned int cid=1;cid<cells10.size();cid++){
    unsigned int tag_cur = cells10.at(cid);
    if(tag_cur>pcmax) pcmax = tag_cur;
  }
  if(fVerbose>4) cout<<"track can contain "<<pcmax<<"+1 cells"<<endl;
  const unsigned int trk_arr_size = pcmax+1;
  //  const int trk_arr_size = pcmax+2;
  std::vector< std::vector<int> > trk_cells(trk_arr_size);
  int trk_count=-1;
  for(unsigned int newpcmax=pcmax;newpcmax>0;newpcmax--){// loop over possible number of cells in trk
    unsigned int cur_max_tag = newpcmax;
    if(fVerbose>4) cout<<"Now we are looking for trk with max "<<cur_max_tag+1<<" cells"<<endl;
    bool newtrk=true;
    for(int con=connect.size()-1;con>=0;){// loop over all connections between cells
      bool nextloop=false;
      unsigned int con1,con2;
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
	if((cells10.at(con2))==cur_max_tag) nextloop=true;
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
	if((cells10.at(con2))==cur_max_tag){
	  //  cout<<"cur_max_tag = "<<cur_max_tag<<endl;
	  cur_max_tag -=1;
	  if((cells10.at(con1))==cur_max_tag){// difference in tags = 1
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
  
  // if(fVerbose>0){
  //   timer_build_all_trk_combinations->Stop();
  //   Double_t rtime_build_all_trk_combinations = timer_build_all_trk_combinations->RealTime();
  //   Double_t ctime_build_all_trk_combinations = timer_build_all_trk_combinations->CpuTime();
  //   cout << "Real time for build all trk combinations :" << rtime_build_all_trk_combinations << " s, CPU time " << ctime_build_all_trk_combinations << " s" << endl;
  // }

  // //TEST: no filter
  // vector<bool> trk_accept;
  // for(unsigned int itrk=0;itrk<trk_cells.at(0).size();itrk++){
  //   trk_accept.push_back(true);
  // }
  // //   TEST: no filter end---

  //filter -----------------------------------------------
  // TStopwatch *timer_filter_trk_combinations = new TStopwatch();
  // if(fVerbose>0)
  //   timer_filter_trk_combinations->Start();

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
  	//reject trk-cand with smaller number of cells and similar cells
  	int curr_arr=trk_arr_size;
  	while(curr_arr>0){
  	  curr_arr--;
  	  if(trk_cells.at(curr_arr).at(itrc)<0 || trk_cells.at(curr_arr).at(itrc2)<0) continue;
  	  if(trk_cells.at(curr_arr).at(itrc)==trk_cells.at(curr_arr).at(itrc2))
  	    count_re++;
  	}
  	if(count_re>0.55*cell_parts[itrc]){
  	  trk_accept[itrc]=false;
  	  if(fVerbose>4){
  	    // cout<<"Delete: trk-cand#"<<itrc<<" because of trk_cells.at("<<curr_arr<<").at("<<itrc<<"):"
  	    // 	<<trk_cells.at(curr_arr).at(itrc)<<endl;
  	    cout<<"Delete: trk-cand#"<<itrc
  		<<" because it contains ("<<count_re<<") more then 55% cells from trk-cand#"<<itrc2<<endl;
  	  }
  	}
      }
    }
  }

  for(int itrc=(cell_parts.size()-1);itrc>=0;itrc--){
      for(int itrc2=itrc-1;itrc2>=0;itrc2--){
  	//	if(itc==itrc2) continue;
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
  	if(count_re>0.9*cell_parts[itrc]){
  	  trk_accept[itrc]=false;
  	  if(fVerbose>4){
  	    cout<<"Delete: trk-cand#"<<itrc<<" because it contains("
  		<<count_re<<") more then 90% of cells from trk-cand#"<<itrc2<<endl;
  	  }
  	}
      }
    }
  }
  //filter(end)------------------------------------------------

  // if(fVerbose>0){
  // timer_filter_trk_combinations->Stop();
  // timer_build_trk_combinations->Stop();
  // Double_t rtime_filter_trk_combinations = timer_filter_trk_combinations->RealTime();
  // Double_t ctime_filter_trk_combinations = timer_filter_trk_combinations->CpuTime();
  // cout << "Real time for filtering trk combinations :" << rtime_filter_trk_combinations << " s, CPU time " << ctime_filter_trk_combinations << " s" << endl;
  // }
  // if(fVerbose>0){
  //   //  timer_build_trk_combinations->Stop();
  // Double_t rtime_build_trk_combinations = timer_build_trk_combinations->RealTime();
  // Double_t ctime_build_trk_combinations = timer_build_trk_combinations->CpuTime();
  // cout << "Real time for build&filter trk combinations :" << rtime_build_trk_combinations << " s, CPU time " << ctime_build_trk_combinations << " s" << endl;
  // }
  // TStopwatch *timer_save_trks = new TStopwatch();
  // if(fVerbose>0)
  //   timer_save_trks->Start();
  //save trk-cands ----------------------------------------
  int NtrkRec = 0;
  //  for(int itrk=0;itrk<trk_cells.at(pcmax).size();itrk++){
  for(unsigned int itrk=0;itrk<trk_accept.size();itrk++){
    if(!trk_accept[itrk]) continue; // filter
    PndTrackCand *myTCand = new PndTrackCand();
    bool firstHit=true;
    TVector3 dir;
    for(unsigned int icell=0;icell<trk_arr_size;icell++){
      int cellNum = trk_cells.at(icell).at(itrk);
      if(cellNum<0) continue;
      if(firstHit){//fill trk-cand seed
	int id = cells6.at(cellNum);
	int pl = cells8.at(cellNum);
	if(fVerbose>3) cout<<"Trk uses hit#"<<id<<" from pl#"<<pl<<endl;
	PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(hitsd.at(pl).at(id)));
	PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
	myTCand->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Z()); 
	//	myTCand->AddHit(0,hitsd.at(pl).at(id),myHit->GetPosition().Z()); 

	double startX = cells0.at(cellNum);
	double startY = cells1.at(cellNum);
	double startZ = cells2.at(cellNum);
	TVector3 posSeed(startX,startY,startZ);
	int cellNumNext=trk_cells.at(icell+1).at(itrk);
	if(cellNumNext<0){//next plane is missing
	  cellNumNext=trk_cells.at(icell+2).at(itrk);
	}
	double dirX = cells3.at(cellNumNext)-cells0.at(cellNum);
	double dirY = cells4.at(cellNumNext)-cells1.at(cellNum);
	double dirZ = cells5.at(cellNumNext)-cells2.at(cellNum);
	dir.SetXYZ(dirX,dirY,dirZ);
	if(fVerbose>3)
	  cout<<"Trk-cand direction is taking based on "<<dir.Mag()<<" cm"<<endl;

	dir*=1./dir.Mag();
	//shift trk out of plane [needed for correct treatment in Kalman Fillter and GEANE]
	double sh_z = -0.035; //350 mkm
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
      unsigned int id = cells7.at(cellNum);
      unsigned int pl = cells9.at(cellNum);
      if(fVerbose>3) cout<<"Trk uses hit#"<<id<<" from pl#"<<pl<<endl;
      PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(hitsd.at(pl).at(id)));
      PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
      PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
      myTCand->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Z()); 
      // myTCand->AddHit(0,hitsd.at(pl).at(id),myHit->GetPosition().Z()); 
    }
    if(fVerbose>3){
      if(flagTrkCandCuts && ((dir.Theta()<0.03 || dir.Theta()>0.05) || fabs(dir.Phi())>0.25))
	cout<<"Ooops, trk-cand has: theta="<<dir.Theta()<<" and phi="<<dir.Phi()<<endl;
      }
    //   hthetaphi->Fill(dir.Theta(),dir.Phi());
    const unsigned int numPts = myTCand->GetNHits(); //read how many points in this track
    if(fVerbose>4) htthetatphiTrk->Fill(dir.Theta(),dir.Phi(),numPts);
    bool cutTrkCand=false;
    if(flagTrkCandCuts){
      double thCentr = dir.Theta()-0.0402;
      if(fabs(thCentr)>0.011 || fabs(dir.Phi())>0.25) cutTrkCand=true;
    }
    if(cutTrkCand) continue;
    //    if(flagTrkCandCuts && ((dir.Theta()<0.03 || dir.Theta()>0.05) || fabs(dir.Phi())>0.25)) continue; //TEST
    if(numPts<3) cout<<"!!! Attention HERE is problem: number of hits in trk-cand = "<<numPts<<"!!!"<<endl;
    if(numPts<3) continue; //TEST
    new((*fTrackCandArray)[NtrkRec]) PndTrackCand(*(myTCand)); //save Track Candidate
    //    new((*fTrackCandArrayTemp)[NtrkRec]) PndTrackCand(*(myTCand)); //save Track Candidate
    NtrkRec++;
    if(fVerbose>3) cout<<"Ntrk No. "<<NtrkRec<<endl;
    delete myTCand;
  }
  // if(fVerbose>0){
  //   timer_save_trks->Stop();
  //   Double_t rtime_save_trks = timer_save_trks->RealTime();
  //   Double_t ctime_save_trks = timer_save_trks->CpuTime();
  //   cout << "Real time for save trks :" << rtime_save_trks << " s, CPU time " << ctime_save_trks << " s" << endl;
  // }  
  
  // //  erase cells;
  // for(int icell=0;icell<11;icell++)
  //   cells.at(icell).erase(cells.at(icell).begin(),cells.at(icell).end());
  cells0.erase(cells0.begin(),cells0.end());
  cells1.erase(cells1.begin(),cells1.end());
  cells2.erase(cells2.begin(),cells2.end());
  cells3.erase(cells3.begin(),cells3.end());
  cells4.erase(cells4.begin(),cells4.end());
  cells5.erase(cells5.begin(),cells5.end());
  cells6.erase(cells6.begin(),cells6.end());
  cells7.erase(cells7.begin(),cells7.end());
  cells8.erase(cells8.begin(),cells8.end());
  cells9.erase(cells9.begin(),cells9.end());
  cells10.erase(cells10.begin(),cells10.end());
  hitsd.erase(hitsd.begin(),hitsd.end());
  // if(fVerbose>0){
  //   timer_exec->Stop();
  //   Double_t rtime_exec = timer_exec->RealTime();
  //   Double_t ctime_exec = timer_exec->CpuTime();
  //   cout << "Real time for Exec:" << rtime_exec << " s, CPU time " << ctime_exec << " s" << endl;
  // }
  if(fVerbose>2){
    Int_t ntcandFin=fTrackCandArray->GetEntriesFast();
    cout<<"Number of Trk-Cands is "<<ntcandFin<<endl;
    cout<< "Evt finsihed--------------"<<endl<<endl;
    cout << endl;
  }
 // delete timer_exec;
 // delete timer_save_trks;
 // delete timer_build_trk_combinations;
 // delete timer_filter_trk_combinations;
 // delete timer_build_all_trk_combinations;
 // delete timer_neighbors_cells;
 // delete timer_cook_cells;
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
  cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! HO-HO!!!!"<<endl;
  // TFile *fout = new TFile("/panda/pandaroot/macro/lmd/tmpOutnewDesign/CA_MultipleTrksCheck/mom_1_5/1trks/tgthetatgpgi_CAcells_1_5GeV_noSecondaries.root","RECREATE");
  if(fVerbose>4){
    //   htthetatphi->Print();
    TTree *nout1 = htthetatphiTrk->CloneTree();
    nout1->Write();
    TTree *nout2 = htthetatphiCells->CloneTree();
    nout2->Write();
  }
  // fout->Write();
  // fout->Close();
}
// -------------------------------------------------------------------------
ClassImp(PndLmdTrackFinderCATask)

