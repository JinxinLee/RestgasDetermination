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
  //   dXY = 0.01;//TEST
}


// -------------------------------------------------------------------------

// -----   Constructor   -------------------------------------------
PndLmdTrackFinderCATask::PndLmdTrackFinderCATask(const bool missPl, const double setdmax) :
  FairTask("LMD Track Finding Task (Cellular Automation) with/without <<missing planes>> algoritm")
{
  missPlAlgo = missPl;
  fHitBranchStrip = "LMDHitsStrip";
  fClusterBranchStrip = "LMDStripClusterCand";
  fDigiBranchStrip = "LMDStripDigis";
  
  dXY = 0.5;
  hdist = new TH1D("hdist","distance from common point",1e3,0,1.);
  d_max = setdmax;
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

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdTrackFinderCATask::Init()
{

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

  fTrackCandArrayTemp = new TClonesArray("PndTrackCand");
  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("LMDTrackCand", "PndLmd", fTrackCandArray, kTRUE);

  std::cout << "-I- PndLmdTrackFinderCATask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndLmdTrackFinderCATask::Exec(Option_t* opt)
{
  // TStopwatch *timer_exec = new TStopwatch();
  // timer_exec->Start();
  // TStopwatch *timer_start = new TStopwatch();
  // timer_start->Start();
  // fTrackCandArrayTemp = new TClonesArray("PndTrackCand");
  if(fVerbose>2) cout << "Evt started--------------"<<endl<<endl;
  
  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Clear();
  fTrackCandArrayTemp->Clear();
  Int_t nStripHits = fStripHitArray->GetEntriesFast();
  if(nStripHits<2){
    if(fVerbose>2) cout << "Evt finsihed: too less hits-----"<<endl<<endl;
    return;
  }
  std::vector< std::vector<Int_t> > hitsd(4); //hit'ids splitted by detectorplane

  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));
    Int_t sensid = myHit->GetSensorID(); // Sensors: 1..32
    Int_t planeid = floor((sensid)/8.); //8 sensors/plane => Planes: 0..3
    hitsd.at(planeid).push_back(iHit);
  }

  Int_t nPlanes=0;
  for(Int_t iPlane = 0; iPlane < 4; iPlane++){
    if(hitsd.at(iPlane).size()>0) nPlanes++;
  }
  if(nPlanes<3){
    if(fVerbose>2) cout << "Evt finsihed: too less planes-----"<<endl<<endl;
    return;
  }
  
  //print hits (debug)
  TVector3 start, tmp, vec, dstart, dvec; //temp-vars
  if(fVerbose>2){
    if(hitsd.at(0).size()>0){
      for (Int_t i=0; i<hitsd.at(0).size(); i++){
	PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(0).at(i));
	cout<<"Plane0 Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<") with err=("
	    <<hit->GetDx()<<", "<<hit->GetDy()<<", "<<hit->GetDz()<<")"<<endl;
      }
    }
    if(hitsd.at(1).size()>0){
      for (Int_t i=0; i<hitsd.at(1).size(); i++){
	PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(1).at(i));
	cout<<"Plane1 Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<") with err=("
	    <<hit->GetDx()<<", "<<hit->GetDy()<<", "<<hit->GetDz()<<")"<<endl;
      }
    }
    if(hitsd.at(2).size()>0){
      for (Int_t i=0; i<hitsd.at(2).size(); i++){
	PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(2).at(i));
	cout<<"Plane2 Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<") with err=("
	    <<hit->GetDx()<<", "<<hit->GetDy()<<", "<<hit->GetDz()<<")"<<endl;
      }
    }
    if(hitsd.at(3).size()>0){
      for (Int_t i=0; i<hitsd.at(3).size(); i++){
	PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(3).at(i));
	cout<<"Plane3 Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<") with err=("
	    <<hit->GetDx()<<", "<<hit->GetDy()<<", "<<hit->GetDz()<<")"<<endl;
      }
    }
  }
  //timer_start->Stop();
  //  timer_start->Print("u");
  // Double_t rtime = timer_start->RealTime();
  // Double_t ctime = timer_start->CpuTime();
  // cout << "Real time_start " << rtime << " s, CPU time_start " << ctime << " s" << endl;
  // //  cout << endl;
  // delete timer_start;
  ///Track-Finder ----------------------------------------------------------
  //  TStopwatch *timer_makingcells = new TStopwatch();
  // timer_makingcells->Start();
  int Npoints=0;
  const int nplanes = 4;
  int NpointsI[nplanes];
  for(int i=0;i<nplanes;i++){
    NpointsI[i]=hitsd.at(i).size();
    if(hitsd.at(i).size()>Npoints)
      Npoints=hitsd.at(i).size();
  }
  
  vector<int> pv;//current Position value
  vector<int> pv_new;//new Position value
  std::vector< std::vector<Double_t> > cells(10);
  std::vector< std::vector<Int_t> > planes(nplanes);
  int count = 0;
  for(int j=1;j<4;j++){
    int maxI = NpointsI[j-1];
    int maxK = NpointsI[j];
    // cout<<"j="<<j<<endl;
    for(int i=0; i<maxI;i++){ //Assume number of hits is not equal on each plane!
      // cout<<"i="<<i<<endl;
      for(int k=0; k<maxK;k++){
	//	cout<<"k="<<k<<endl;
	//	if(hitsd.at(j-1).size()==0 || hitsd.at(j).size()==0) continue;
	PndSdsHit *hit0=(PndSdsHit*)fStripHitArray->At(hitsd.at(j-1).at(i));
	PndSdsHit *hit1=(PndSdsHit*)fStripHitArray->At(hitsd.at(j).at(k));
	/// Check dPhi, dTheta of cells to reduce wrong combination
	double x0 = hit0->GetX(); double y0 = hit0->GetY();; double z0 = hit0->GetZ();
	double x1 = hit1->GetX(); double y1 = hit1->GetY();; double z1 = hit1->GetZ();
	TVector3 dirc(x1-x0,y1-y0,z1-z0);

	//	cout<<"dirc.Theta() = "<<dirc.Theta()<<" dirc.Phi() = "<<dirc.Phi()<<endl;
	  //	if(dirc.Theta()<0.03 || dirc.Theta()>0.05 || fabs(dirc.Phi())>0.25){
	if(dirc.Theta()>0.01){ //in LUMI frame
	  if(fVerbose>4){
	    cout<<"For cell between #"<<(j-1)<<"."<<i<<" and #"<<j<<"."<<k;
	    cout<<" dirc.Theta() = "<<dirc.Theta()<<" dirc.Phi() = "<<dirc.Phi()<<endl;
	  }
	  //  continue;
	}
	else{
	  if(fVerbose>4){
	    cout<<"GOOD CELL #"<<cells.at(0).size()<<" [between #"<<(j-1)<<"."<<i<<" and #"<<j<<"."<<k<<"]:";
	    cout<<" dirc.Theta() = "<<dirc.Theta()<<" dirc.Phi() = "<<dirc.Phi()<<endl;
	  }
	}
	cells.at(0).push_back(x0);
	cells.at(1).push_back(y0);
	cells.at(2).push_back(z0);
	cells.at(3).push_back(x1);
	cells.at(4).push_back(y1);
	cells.at(5).push_back(z1);
	cells.at(6).push_back(i);
	cells.at(7).push_back(k);
	cells.at(8).push_back(j-1);
	cells.at(9).push_back(j);
	planes.at(j).push_back(1);
	pv.push_back(1);
	pv_new.push_back(1);
	count++;
	//	cout<<"count = "<<count<<endl;
      }
    }
  }
 

  int nc=0;
  int ncI[nplanes];
  ncI[0]=0;
  for(int hk=1;hk<4;hk++){
    int size_i = (planes.at(hk).size());
    ncI[hk]=ncI[hk-1]+size_i;
    if(fVerbose>4) 
      cout<<" ncI["<<hk<<"]="<< ncI[hk]<<endl;
    nc+=size_i;
  }
  // rtime = timer_makingcells->RealTime();
  // ctime = timer_makingcells->CpuTime();
  // cout << "Real time_makingcells " << rtime << " s, CPU time_makingcells " << ctime << " s" << endl;
  // delete timer_makingcells;
  // TStopwatch *timer_makingneighbour = new TStopwatch();
  // timer_makingneighbour->Start();

  
 
  int cd=0;
  bool stop=false;
  vector<int> connect;
  vector<bool> conn_flag;
  //  int nCon;
  while(!stop){
    double d1=0, d2=0;
    for(int ipl=0;ipl<(nplanes-2);ipl++){
      for(int p=ncI[ipl];p<ncI[ipl+1];p++){
	for(int q=ncI[ipl+1];q<ncI[ipl+2];q++){
	  if(fabs(cells.at(5).at(p)-cells.at(2).at(q))>1e-6) continue;
	  if(fabs(cells.at(4).at(p)-cells.at(1).at(q))>1e-6) continue;
	  if(fabs(cells.at(3).at(p)-cells.at(0).at(q))<1e-6){
	    /// v - vector of track direction
	    /// w - vector between middle point and another end of cell
	    /// d= [v,w]/|v| - distance
	    /// [v,w]=(v_y*w_z-v_z*w_y, v_z*w_x-v_x*w_z,v_x*w_y-v_y*w_x)
	    /// |v|=sqrt(v_x^2+v_y^2+v_z^2)
	 
	    double x0 = cells.at(0).at(p); double y0 = cells.at(1).at(p); double z0 = cells.at(2).at(p);
	    double x1 = cells.at(3).at(q); double y1 = cells.at(4).at(q); double z1 = cells.at(5).at(q);
	    double xt = cells.at(3).at(p); double yt = cells.at(4).at(p); double zt = cells.at(5).at(p);
	    ///v(x1-x0,y1-y0,z1-z0)
	    ///w(x0-xt,y0-yt,z0-zt)
	    
	    double d_x =(y1-y0)*(z0-zt)-(z1-z0)*(y0-yt);
	    double d_y =(z1-z0)*(x0-xt)-(x1-x0)*(z0-zt);
	    double d_z =(x1-x0)*(y0-yt)-(y1-y0)*(x0-xt);
	    
	    double norm_v = sqrt(pow((x1-x0),2)+pow((y1-y0),2)+pow((z1-z0),2));
	    double d = sqrt(d_x*d_x+d_y*d_y+d_z*d_z)/norm_v;
	    if(fVerbose>3){
	      cout<<"For cell #"<<p<<" and cell #"<<q<<":"<<endl;
	      cout<<"P0=("<<x0<<","<<y0<<","<<z0<<")"<<endl;
	      cout<<"Pt=("<<xt<<","<<yt<<","<<zt<<")"<<endl;
	      cout<<"P1=("<<x1<<","<<y1<<","<<z1<<")"<<endl;
	      cout<<"d = "<<d<<" norm_v="<<norm_v<<endl;
	      cout<<" "<<endl;
	    }
	    // if(ipl==0){
	    //   d_max = d_max1;
	    //   d1=d;
	    // }
	    
	    // if(ipl==1){
	    //   d_max = d_max2;
	    //   d2=d;
	    // }
	    hdist->Fill(d);
	    //  cout<<"d_max = "<<d_max<<endl;
	    if(d<d_max){
	      if(pv[q]==pv[p]){
		pv_new[p] = pv[p]+1;
		connect.push_back(p);
		conn_flag.push_back(true);
		connect.push_back(q);
		conn_flag.push_back(true);
		//	cout<<"!!!!!    Now we add cell#"<<p<<" and cell#"<<q<<" connect.size()="<<connect.size()<<endl;
	      }
	      //	  else nCon++;
	    }
	    //   else continue;
	  }
	}
	//  if(nCon>0) stop=true;
      }
    }
    int count_stop=0;
    for(int t=(count-1);t>-1;t--){
      if(fVerbose>4) cout<<"pv["<<t<<"]="<<pv[t]<<" pv_new["<<t<<"]="<<pv_new[t]<<endl;
      if(pv[t] == pv_new[t]) count_stop++;
      else{
	pv[t]=pv_new[t];
      }
    }
    if(count_stop==count) stop=true;
  }

  if(fVerbose>4) for(int jg=0;jg<connect.size(); jg++) cout<<"connect["<<jg<<"]="<<connect[jg]<<endl;  
 
  if(connect.size()<2){
    if(fVerbose>2 &&  !missPlAlgo)
      cout << "Evt finsihed: too less cells-----"<<endl<<endl;
    if(!missPlAlgo) return;
  }
  else{
    int last_el=connect.size();
    for(int jg=(connect.size()-1);jg>0;jg--){
      // cout<<"pv[connect[jg]] = "<<pv[connect[jg]]<<" pv[connect[jg-1]] = "<<pv[connect[jg-1]]<<endl;
      //  if(pv[connect[jg]]==3 && pv[connect[jg-1]]==1)  
      if(connect[jg]==connect[0]){
	last_el=jg;
	break;
      }
    }
    // int last_el = pv.size();
    connect.erase((connect.begin()+last_el),(connect.begin()+connect.size())); //delete elements repeated
    conn_flag.erase((conn_flag.begin()+last_el),(conn_flag.begin()+conn_flag.size()));
  }

  if(fVerbose>4){
    cout<<"---------- New connect array:"<<endl;
    for(int jg=0;jg<connect.size(); jg++) cout<<"connect["<<jg<<"]="<<connect[jg]<<endl; 
    cout<<"---------------------------------"<<endl;
  }

  if(missPlAlgo){
    if(fVerbose>3) cout<<"Taking into account missing planes!"<<endl;
    int count_old = count;
    // cout<<"count_old = "<<count_old<<endl;
    std::vector< std::vector<Int_t> > planes_new(4);
    for(int j=2;j<4;j++){
      int maxI = NpointsI[j-2];
      int maxK = NpointsI[j];
      for(int i=0; i<maxI;i++){
	for(int k=0; k<maxK;k++){
	  if(hitsd.at(j-2).size()==0 || hitsd.at(j).size()==0) continue;
	  PndSdsHit *hit0=(PndSdsHit*)fStripHitArray->At(hitsd.at(j-2).at(i));
	  PndSdsHit *hit1=(PndSdsHit*)fStripHitArray->At(hitsd.at(j).at(k));
	  /// Check dPhi, dTheta of cells to reduce wrong combination
	  double x0 = hit0->GetX(); double y0 = hit0->GetY(); double z0 = hit0->GetZ();
	  double x1 = hit1->GetX(); double y1 = hit1->GetY();  double z1 = hit1->GetZ();
	  TVector3 dirc(x1-x0,y1-y0,z1-z0);
	  dirc *= 1./dirc.Mag();
	  // cout<<"dirc.Theta() = "<<dirc.Theta()<<" dirc.Phi() = "<<dirc.Phi()<<endl;
	  // if(dirc.Theta()<0.025 || dirc.Theta()>0.055 || fabs(dirc.Phi())>0.3)
	  //   continue;
	  //	 if(dirc.Theta()<0.03 || dirc.Theta()>0.05 || fabs(dirc.Phi())>0.25){
	  if(dirc.Theta()>0.01){ //in LUMI frame
	    if(fVerbose>4){
	      cout<<"For cell between #"<<(j-2)<<"."<<i<<" and #"<<j<<"."<<k;
	      cout<<" dirc.Theta() = "<<dirc.Theta()<<" dirc.Phi() = "<<dirc.Phi()<<endl;
	    }
	    //	    continue;
	  }
	  else{
	    if(fVerbose>4){
	      cout<<"GOOD CELL: For cell between #"<<(j-1)<<"."<<i<<" and #"<<j<<"."<<k;
	      cout<<" dirc.Theta() = "<<dirc.Theta()<<" dirc.Phi() = "<<dirc.Phi()<<endl;
	    }
	  }

	  cells.at(0).push_back(x0);
	  cells.at(1).push_back(y0);
	  cells.at(2).push_back(z0);
	  cells.at(3).push_back(x1);
	  cells.at(4).push_back(y1);
	  cells.at(5).push_back(z1);
	  cells.at(6).push_back(i);
	  cells.at(7).push_back(k);
	  cells.at(8).push_back(j-2);
	  cells.at(9).push_back(j);
	  planes_new.at(j-2).push_back(1);
	  // if(j==2){
	  //   pv.push_back(1);
	  //   pv_new.push_back(1);
	  // }
	  // else{
	  //   pv.push_back(3);
	  //   pv_new.push_back(3);
	  // }
	  pv.push_back(-1);
	  pv_new.push_back(-1);
	  count++;
	}
      }
    }
    

    int nc_new=0;
    int ncI_new[4];
    for(int hk=0;hk<4;hk++){
      int size_i = (planes_new.at(hk).size());
      if(hk==0) ncI_new[hk]=size_i;
      else ncI_new[hk]=ncI_new[hk-1]+size_i;
      //   cout<<"ncI_new["<<hk<<"]="<<ncI_new[hk]<<" size_i = "<<size_i<<endl;
      nc_new+=size_i;
    }
    stop=false;
    while(!stop){
      double d1=0, d2=0;
      //    cout<<"nplanes = "<<nplanes<<endl;
      //Check conection between cells on 0-2 & 2-3 planes
      //    cout<<"Check conection between cells on 0-2 & 2-3 planes!"<<endl;
      for(int p=(count_old);p<(count_old+ncI_new[0]);p++){
	for(int q=ncI[2];q<ncI[3];q++){
	  // cout<<"p = "<<p<<" q = "<<q<<endl;
	  // cout<<"cells.at(5).at(p) = "<<cells.at(5).at(p)<<" cells.at(2).at(q) = "<<cells.at(2).at(q)<<endl;
	  // cout<<"cells.at(4).at(p) = "<<cells.at(4).at(p)<<" cells.at(1).at(q) = "<<cells.at(1).at(q)<<endl;
	  // cout<<"cells.at(3).at(p) = "<<cells.at(3).at(p)<<" cells.at(0).at(q) = "<<cells.at(0).at(q)<<endl;
	  if(fabs(cells.at(5).at(p)-cells.at(2).at(q))>1e-6) continue;
	  if(fabs(cells.at(4).at(p)-cells.at(1).at(q))>1e-6) continue;
	  if(fabs(cells.at(3).at(p)-cells.at(0).at(q))<1e-6){
	    /// v - vector of track direction
	    /// w - vector between middle point and another end of cell
	    /// d= [v,w]/|v| - distance
	    /// [v,w]=(v_y*w_z-v_z*w_y, v_z*w_x-v_x*w_z,v_x*w_y-v_y*w_x)
	    /// |v|=sqrt(v_x^2+v_y^2+v_z^2)
	    
	    double x0 = cells.at(0).at(p); double y0 = cells.at(1).at(p); double z0 = cells.at(2).at(p);
	    double x1 = cells.at(3).at(q); double y1 = cells.at(4).at(q); double z1 = cells.at(5).at(q);
	    double xt = cells.at(3).at(p); double yt = cells.at(4).at(p); double zt = cells.at(5).at(p);
	    ///v(x1-x0,y1-y0,z1-z0)
	    ///w(x0-xt,y0-yt,z0-zt)
	    
	    double d_x =(y1-y0)*(z0-zt)-(z1-z0)*(y0-yt);
	    double d_y =(z1-z0)*(x0-xt)-(x1-x0)*(z0-zt);
	    double d_z =(x1-x0)*(y0-yt)-(y1-y0)*(x0-xt);
	    
	    double norm_v = sqrt(pow((x1-x0),2)+pow((y1-y0),2)+pow((z1-z0),2));
	    double d = sqrt(d_x*d_x+d_y*d_y+d_z*d_z)/norm_v;
	    if(fVerbose>3){
	      cout<<"P0=("<<x0<<","<<y0<<","<<z0<<")"<<endl;
	      cout<<"Pt=("<<xt<<","<<yt<<","<<zt<<")"<<endl;
	      cout<<"P1=("<<x1<<","<<y1<<","<<z1<<")"<<endl;
	      cout<<"d = "<<d<<" norm_v="<<norm_v<<endl;
	      cout<<" "<<endl;
	    }
	    hdist->Fill(d);
	    if(d<d_max){
	      //  if(pv[q]==pv[p]){
	      if(pv[q]==1 && pv[p]==-1){
		// pv_new[p] = pv[p]+1;
		pv_new[p] = pv[p]-1;
		connect.push_back(p);
		conn_flag.push_back(true);
		connect.push_back(q);
		conn_flag.push_back(true);
		//  cout<<"!!!!!   Now we add cell#"<<p<<" and cell#"<<q<<endl;
	      }
	    }
	  }
	}
      }
      if(fVerbose>4) cout<<" connect.size() = "<<connect.size()<<endl;
      //Check conection between cells on 0-1 & 1-3 planes
      // cout<<"Check conection between cells on 0-1 & 1-3 planes!"<<endl;
      // cout<<"count_old+ncI_new[0] = "<<count_old+ncI_new[0]<<" count_old+ncI_new[1] = "<<count_old+ncI_new[1]<<endl;
      // cout<<"ncI[0] = "<<ncI[0]<<endl;
      for(int q=(count_old+ncI_new[0]);q<(count_old+ncI_new[1]);q++){
	for(int p=0;p<ncI[1];p++){
	  // cout<<"p = "<<p<<" q = "<<q<<endl;
	  // cout<<"cells.at(5).at(p) = "<<cells.at(5).at(p)<<" cells.at(2).at(q) = "<<cells.at(2).at(q)<<endl;
	  // cout<<"cells.at(4).at(p) = "<<cells.at(4).at(p)<<" cells.at(1).at(q) = "<<cells.at(1).at(q)<<endl;
	  // cout<<"cells.at(3).at(p) = "<<cells.at(3).at(p)<<" cells.at(0).at(q) = "<<cells.at(0).at(q)<<endl;
	  if(fabs(cells.at(5).at(p)-cells.at(2).at(q))>1e-6) continue;
	  if(fabs(cells.at(4).at(p)-cells.at(1).at(q))>1e-6) continue;
	  if(fabs(cells.at(3).at(p)-cells.at(0).at(q))<1e-6){
	    /// v - vector of track direction
	    /// w - vector between middle point and another end of cell
	    /// d= [v,w]/|v| - distance
	    /// [v,w]=(v_y*w_z-v_z*w_y, v_z*w_x-v_x*w_z,v_x*w_y-v_y*w_x)
	    /// |v|=sqrt(v_x^2+v_y^2+v_z^2)
	    
	    double x0 = cells.at(0).at(p); double y0 = cells.at(1).at(p); double z0 = cells.at(2).at(p);
	    double x1 = cells.at(3).at(q); double y1 = cells.at(4).at(q); double z1 = cells.at(5).at(q);
	    double xt = cells.at(3).at(p); double yt = cells.at(4).at(p); double zt = cells.at(5).at(p);
	    ///v(x1-x0,y1-y0,z1-z0)
	    ///w(x0-xt,y0-yt,z0-zt)
	    
	    double d_x =(y1-y0)*(z0-zt)-(z1-z0)*(y0-yt);
	    double d_y =(z1-z0)*(x0-xt)-(x1-x0)*(z0-zt);
	    double d_z =(x1-x0)*(y0-yt)-(y1-y0)*(x0-xt);
	    
	    double norm_v = sqrt(pow((x1-x0),2)+pow((y1-y0),2)+pow((z1-z0),2));
	    double d = sqrt(d_x*d_x+d_y*d_y+d_z*d_z)/norm_v;
	    if(fVerbose>3){
	      cout<<"P0=("<<x0<<","<<y0<<","<<z0<<")"<<endl;
	      cout<<"Pt=("<<xt<<","<<yt<<","<<zt<<")"<<endl;
	      cout<<"P1=("<<x1<<","<<y1<<","<<z1<<")"<<endl;
	      cout<<"d = "<<d<<" norm_v="<<norm_v<<endl;
	      cout<<" "<<endl;
	    }
	    hdist->Fill(d);
	    if(d<d_max){
	      if(pv[q]==-1 && pv[p]==3){
		pv_new[q] = pv[q]-1;
		connect.push_back(p);
		conn_flag.push_back(true);
		connect.push_back(q);
		conn_flag.push_back(true);
	      }
	    }
	  }
	}
      }
      int count_stop=0;
      for(int t=(count-1);t>-1;t--){
	if(fVerbose>4) cout<<"pv["<<t<<"]="<<pv[t]<<" pv_new["<<t<<"]="<<pv_new[t]<<endl;
	if(pv[t] == pv_new[t]) count_stop++;
	else{
	  pv[t]=pv_new[t];
	}
      }
      if(count_stop==count) stop=true;
    }
    ///---------------------------------------------------------------------------------------
    if(fVerbose>4) cout<<"count_old="<<count_old<<" count_new = "<<count<<endl;
  }
  
  // TStopwatch *timer_makingtrk = new TStopwatch();
  // timer_makingtrk->Start();
  /// make trk-candidates
  vector<int> pv3,pv2,pv1;
  for(int t=0;t<count;t++){
    if(pv[t]==3) pv3.push_back(t);
    // if(pv[t]==2) pv2.push_back(t);
    if(fabs(pv[t])==2) pv2.push_back(t);
    if(pv[t]==1) pv1.push_back(t);
  }
  const int pv3size = pv3.size();
  const int pv2size = pv2.size();
  const int pv1size = pv1.size();
  if(pv3size==0 && pv2size==0){
    if(fVerbose>4) cout<<"Evt finsihed: Track-Candidate(s) was not found -----"<<endl;
    return;
  }
  //  for(int pl=0;pl<connect.size(); pl++) cout<<"connect["<<pl<<"]="<<connect[pl]<<" pv["<<pl<<"]="<<pv[pl]<<endl;
  if(fVerbose>4) cout<<"pv3size = "<<pv3size<<" pv2size = "<<pv2size<<" pv1size = "<<pv1size<<endl;
 
  //save track for fitting
  int NtrkRec = 0;
  vector<int> trk;
  trk.push_back(-100);
  
  int connectSizeOld=connect.size(),connectSizeNew=1;
  while(connectSizeOld!=connectSizeNew && trk.size()!=0){
    //    cout<<"%$#^^$^#^$%#^%#$$^%"<<endl;
    //    cout<<"connect.size()="<<connect.size()<<" conn_flag.size() = "<<conn_flag.size()<<endl;
    connectSizeOld = connect.size();
    trk.erase(trk.begin(),trk.begin()+trk.size());
    bool addHit = false;
    for(int p=0;p<(connect.size()-1);p++){
      if(addHit) break;
      for(int i=0;i<pv3.size();i++){
	if(addHit) break;
	for(int j=0;j<pv2.size();j++){
	  if(addHit) break;
	  //	  cout<<"connect["<<p<<"]="<<connect[p]<<endl;
	  if(connect[p]==pv3[i] && connect[p+1]==pv2[j]){
	    //cout<<"pv2["<<j<<"]="<<pv2[j]<<" pv3["<<i<<"]="<<pv3[i]<<endl;
	    trk.push_back(pv3[i]);
	    trk.push_back(pv2[j]);
	    connect.erase(connect.begin()+p,connect.begin()+(p+2));
	    conn_flag.erase(conn_flag.begin()+p,conn_flag.begin()+(p+2));
	    p=0;
	    addHit = true;
	  }
	}
      }
    }
    addHit = false;
    for(int p=0;p<connect.size();p++){
      if(addHit) break;
      for(int k=0;k<pv1.size();k++){
	if(addHit) break;
	//	cout<<"connect["<<p<<"]="<<connect[p]<<" trk["<<trk.size()-1<<"]="<<trk[trk.size()-1]<<" pv1["<<k<<"]="<<pv1[k]<<endl;
	if(trk.size()!=0){
	if(connect[p]==trk[trk.size()-1] && connect[p+1]==pv1[k]){
	  //	  cout<<"trk["<<trk.size()-1<<"]="<<trk[trk.size()-1]<<" pv1["<<k<<"]="<<pv1[k]<<endl;
	  trk.push_back(pv1[k]); 
	  conn_flag[p] = false;
	  conn_flag[p+1] = false;
	  //  connect.erase(connect.begin()+p,connect.begin()+(p+2));
	  addHit = true;
	}
	}
      }
    }
    connectSizeNew = connect.size();
 
    if(trk.size()<2) continue;
    if(fVerbose>2) cout<<"trk size "<<trk.size()<<" cells"<<endl;  
    
    
    PndTrackCand *myTCand = new PndTrackCand();
    ///Add seed information to track------------
    double startX = cells.at(0).at(trk[0]);
    double startY = cells.at(1).at(trk[0]);
    double startZ = cells.at(2).at(trk[0]);
    TVector3 posSeed(startX,startY,startZ);
    int max = (trk.size()-1);
    double dirX = cells.at(3).at(trk[max])-cells.at(0).at(trk[0]);
    double dirY = cells.at(4).at(trk[max])-cells.at(1).at(trk[0]);
    double dirZ = cells.at(5).at(trk[max])-cells.at(2).at(trk[0]);
    TVector3 dir(dirX,dirY,dirZ);
    dir*=1./dir.Mag();

    //    if(dir.Theta()<3e-2 || dir.Theta()>5e-2 || fabs(dir.Phi())>0.26) continue;
    if(fVerbose>2){
      cout<<"posSeed:"<<endl;
      posSeed.Print();
      cout<<"dirSeed:"<<endl;
      dir.Print();
    }
    myTCand->setTrackSeed(posSeed,dir,-1);
    // cout<<"trk.size()-1 = "<<trk.size()-1
    // 	  <<" cells.at(7).size() = "<<cells.at(7).size()
    // 	  <<" cells.at(9).size() = "<<cells.at(9).size()<<endl;
    for (Int_t k=0; k<trk.size(); k++){
      int id = cells.at(6).at(trk[k]);
      int pl = cells.at(8).at(trk[k]);

      // cout<<"Save hit#"<<id<<" from plane#"<<pl<<endl;
      PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(hitsd.at(pl).at(id)));
      PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
      PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
      //      myTCand->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Mag());  
      myTCand->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Z());  
    }
    int id = cells.at(7).at(trk[trk.size()-1]);
    int pl = cells.at(9).at(trk[trk.size()-1]);
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(hitsd.at(pl).at(id)));
    PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
    PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
    //    myTCand->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Mag());  
    myTCand->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Z());  
    // cout<<"Save hit#"<<id<<" from plane#"<<trk.size()<<endl;
    
    ///-------------------------------------
    //   new((*fTrackCandArray)[NtrkRec]) PndTrackCand(*(myTCand)); //save Track Candidate
    new((*fTrackCandArrayTemp)[NtrkRec]) PndTrackCand(*(myTCand)); //save Track Candidate
    NtrkRec++;
    // cout<<"Ntrk No. "<<NtrkRec<<endl;
    //cout<<"NtrkRec = "<<NtrkRec<<endl;
    delete myTCand;
    // cout<<" noch ein mal NtrkRec = "<<NtrkRec<<endl;
  }//Track Cand build for 3 cells

  // cout<<"What's up?!"<<endl;
  // rtime = timer_makingtrk->RealTime();
  // ctime = timer_makingtrk->CpuTime();
  // cout << "Real time_makingtrk " << rtime << " s, CPU time_makingtrk " << ctime << " s" << endl;
  // //  cout << endl;
  // delete timer_makingtrk;
  //if(fVerbose>2) cout<<"connect.size()="<<connect.size()<<endl;
  bool cont=false;
  for(int h=0;h<conn_flag.size();h++){
    // cout<<"cont = "<<cont<<endl;
    //cout<<"conn_flag["<<h<<"]="<<conn_flag[h]<<endl;
    if(conn_flag[h]==true)
      cont=true;
  }
  
  if(cont==true){
    if(fVerbose>2) cout<<"We will continue for track with 2 cells!"<<endl;
    //save track for fitting
    trk.clear();
    trk.push_back(-100);
    connectSizeOld=connect.size(),connectSizeNew=1;
    //    cout<<"connectSizeOld = "<<connectSizeOld<<endl;
    while(connectSizeOld!=connectSizeNew && trk.size()!=0){
      connectSizeOld = connect.size();
      trk.erase(trk.begin(),trk.begin()+trk.size());
      bool addHit = false;
      for(int p=0;p<connect.size();p++){
	if(conn_flag[p]==false || conn_flag[p+1]==false) continue;
	if(addHit ) break;
	for(int i=0;i<pv2.size();i++){
	  if(addHit) break;
	  for(int j=0;j<pv1.size();j++){
	    if(addHit) break;
	    if(connect[p]==pv2[i] && connect[p+1]==pv1[j]){
	      trk.push_back(pv2[i]);
	      trk.push_back(pv1[j]);
	      connect.erase(connect.begin()+p,connect.begin()+(p+2));
	      conn_flag.erase(conn_flag.begin()+p,conn_flag.begin()+(p+2));
	      p=0;
	      addHit = true;
	    }
	  }
	}
      }
      connectSizeNew = connect.size();
      
      if(trk.size()<2) continue;
      if(fVerbose>2) cout<<"trk size "<<trk.size()<<" cells"<<endl;
      
      PndTrackCand *myTCand1 = new PndTrackCand();
      ///Add seed information to track------------
      double startX = cells.at(0).at(trk[0]);
      double startY = cells.at(1).at(trk[0]);
      double startZ = cells.at(2).at(trk[0]);
      TVector3 posSeed(startX,startY,startZ);
      int max = (trk.size()-1);
      double dirX = cells.at(3).at(trk[max])-cells.at(0).at(trk[0]);
      double dirY = cells.at(4).at(trk[max])-cells.at(1).at(trk[0]);
      double dirZ = cells.at(5).at(trk[max])-cells.at(2).at(trk[0]);
      TVector3 dir(dirX,dirY,dirZ);
      dir*=1./dir.Mag();
      
      // if(dir.Theta()<3e-2 || dir.Theta()>5e-2 || fabs(dir.Phi())>0.26) continue;
      myTCand1->setTrackSeed(posSeed,dir,-1);
      if(fVerbose>2){
	cout<<"posSeed:"<<endl;
	posSeed.Print();
	cout<<"dirSeed:"<<endl;
	dir.Print();
      }
      
      ///-------------------------------------
      //	  new((*fTrackCandArray)[NtrkRec]) PndTrackCand(*(myTCand1)); //save Track Candidate
      // cout<<"trk.size()-1 = "<<trk.size()-1
      //   <<" cells.at(7).size() = "<<cells.at(7).size()
      //   <<" cells.at(9).size() = "<<cells.at(9).size()<<endl;
      for (Int_t k=0; k<trk.size(); k++){
	int id = cells.at(6).at(trk[k]);
	int pl = cells.at(8).at(trk[k]);
	//	cout<<"Save hit#"<<id<<" from plane#"<<pl<<endl;
	PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(hitsd.at(pl).at(id)));
	PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
	//	myTCand1->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Mag());  
	myTCand1->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Z());  
      }
    
      int id = cells.at(7).at(trk[trk.size()-1]);
      int pl = cells.at(9).at(trk[trk.size()-1]);
     
      //      cout<<"Save hit#"<<id<<" from plane#"<<pl<<endl;
      PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(hitsd.at(pl).at(id)));
      PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
      PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
      // myTCand1->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Mag());  
      myTCand1->AddHit(astripdigi->GetDetID(),hitsd.at(pl).at(id),myHit->GetPosition().Z());  
      
      new((*fTrackCandArrayTemp)[NtrkRec]) PndTrackCand(*(myTCand1)); //save Track Candidate
      //  new((*fTrackCandArray)[NtrkRec]) PndTrackCand(*(myTCand1)); //save Track Candidate
      NtrkRec++;
      //      cout<<"Ntrk No. "<<NtrkRec<<endl;
      delete myTCand1;
    }
  }//Track Cand build from 2 cells
  ///  if(fVerbose>2) cout << "Evt finsihed--------------"<<endl<<endl;
  //	delete fTrackCandArrayTemp;
  // rtime = timer_exec->RealTime();
  // ctime = timer_exec->CpuTime();
  // cout << "Real time_exec " << rtime << " s, CPU time_exec " << ctime << " s" << endl;
  // cout << endl;
  // delete timer_exec;
  //}

  if(missPlAlgo && nPlanes==4){
    if(fVerbose>2) cout<<" ------- Track-Candidates Fillter ------"<<endl;
    ///Track-Candidates Fillter ------------------------------------------
    int NsaveTrk = 0;
    Int_t ntcand=fTrackCandArrayTemp->GetEntriesFast();
    TMatrixD trkcan_point(ntcand,4);
    for(int trkcID=0;trkcID<ntcand;trkcID++)
      for(unsigned int ihit=0; ihit<4; ihit++)
	trkcan_point(trkcID,ihit)=-1;
    for(int trkcID=0;trkcID<ntcand;trkcID++){
      PndTrackCand* trcnd = (PndTrackCand*)fTrackCandArrayTemp->At(trkcID);
      
      const int numPts = trcnd->GetNHits(); //read how many points in this track
      //	if(numPts==4) continue;
      for(unsigned int ihit=0; ihit<numPts; ihit++){ //loop on the hits
	PndTrackCandHit theHit = trcnd->GetSortedHit(ihit); //get hit
	Int_t index = theHit.GetHitId();
	Int_t detectorID = theHit.GetDetId();
	double rho = theHit.GetRho();
	//  	cout<<"rho = "<<rho<<" detectorID = "<<detectorID<<endl;
	int pos_num;
	if(rho<5.) pos_num=0;
	else{
	  if(rho<15.) pos_num=1;
	  else{
	    if(rho<25.) pos_num=2;
	    else{
	      pos_num=3;
	    }
	  }
	}	  
	
	// Int_t detId = theHit.GetDetId();
	//  cout<<"index: "<<index<<" rho: "<<rho<<endl;
	//PndSdsHit* addHit = (PndSdsHit*) fStripHitArray->At(index);
	// if(numPts==3) trkcan_point(trkcID,ihit+1)=index;
	// else trkcan_point(trkcID,ihit)=index;
	trkcan_point(trkcID,pos_num)=index;
	
      }//end of Hits in TCand
    }
    if(fVerbose>4) cout<<"trkcan_point:"<<endl;
    if(fVerbose>4) trkcan_point.Print();
    TMatrixD coincidence(ntcand,4);
    for(int trkcID=0;trkcID<ntcand;trkcID++)
      for(unsigned int ihit=0; ihit<4; ihit++)
	coincidence(trkcID,ihit)=-1;
    for(int trkcID1=ntcand-1;trkcID1>-1;trkcID1--){
      for(int trkcID2=ntcand-2;trkcID2>-1;trkcID2--){
	if(trkcID1==trkcID2) continue;
	for(unsigned int ihit=0; ihit<4; ihit++){ //loop on the hits
	  if(trkcan_point(trkcID1,ihit)==-1 || trkcan_point(trkcID2,ihit)==-1) continue;
	  if(trkcan_point(trkcID1,ihit)==trkcan_point(trkcID2,ihit)){
	    // cout<<"trkcID1 = "<<trkcID1<<" trkcID2 = "<<trkcID2<<endl;
	    // cout<<"trkcan_point(trkcID1,ihit) = "<<trkcan_point(trkcID1,ihit)
	    // 	  <<" trkcan_point(trkcID2,ihit) = "<<trkcan_point(trkcID2,ihit)<<endl;
	    coincidence(trkcID1,ihit)=trkcID2;
	    coincidence(trkcID2,ihit)=trkcID1;
	  }
	}
      }
    }
    if(fVerbose>4) cout<<"coincidence:"<<endl;
    if(fVerbose>4) coincidence.Print();
    TVectorD distance(ntcand);
    TVectorF flag(ntcand);
    for(int trkcID=0;trkcID<ntcand;trkcID++){
      flag(trkcID)=false;
      PndTrackCand* trcnd = (PndTrackCand*)fTrackCandArrayTemp->At(trkcID);
      const int numPts = trcnd->GetNHits(); //read how many points in this track
      int repE[ntcand];
      if(numPts==4){
	if(fVerbose>4) cout<<"trk-cand #"<<trkcID<<" is added (with 4 hits)!"<<endl;
	new((*fTrackCandArray)[NsaveTrk]) PndTrackCand(*(trcnd)); //save Track Candidate
	NsaveTrk++;
	flag(trkcID)=true;
      }
      else{
	if(fVerbose>4)	cout<<"Now we are in case with < 4 hits!"<<endl;
	for(int trkc=0;trkc<ntcand;trkc++) repE[trkc]=1;
	for(unsigned int ihit0=0; ihit0<1; ihit0++){
	  for(unsigned int ihit1=1; ihit1<4; ihit1++){
	    if(ihit0==ihit1) continue;
	    if(coincidence(trkcID,ihit0)==coincidence(trkcID,ihit1)){
	      int el = coincidence(trkcID,ihit0);
	      repE[el]=repE[el]+1;
	      //  cout<<"ihit0 = "<<ihit0<<" ihit1 = "<<ihit1<<" el="<<el<<endl;
	    }
	  }
	}
	bool addTrk=true;
	//	cout<<" "<<endl;
	for(int trkc=0;trkc<ntcand;trkc++){
	   if(fVerbose>5) cout<<"repE["<<trkc<<"]="<<repE[trkc]<<" flag(trkc)="<<flag(trkc)<<endl;
	  if(repE[trkc]==3){
	    addTrk=false;
	    break;
	  }
	  if(repE[trkc]==2){
	    if(flag(trkc)==true){
	      addTrk=false;
	      if(fVerbose>4) cout<<"We've already added trk #"<<trkc<<" with the same hits!"<<endl;
	      break;
	    }
	    int ihit=1;
	    //   cout<<"ihit="<<ihit<<endl;
	    int index_prev = trkcan_point(trkcID,ihit-1);
	    if(index_prev==-1){
	      ihit=ihit+1;
	      index_prev = trkcan_point(trkcID,ihit-1);
	    }
	    //   cout<<"ihit="<<ihit<<endl;
	    int index_mid = trkcan_point(trkcID,ihit);
	    if(index_mid==-1){
	      ihit=ihit+1;
	      index_mid = trkcan_point(trkcID,ihit);
	    }
	    //cout<<"ihit="<<ihit<<endl;
	    int index_next = trkcan_point(trkcID,ihit+1);
	    if(index_next==-1) index_next = trkcan_point(trkcID,ihit+2);
	    
	    if(fVerbose>5) 
	      cout<<"index_prev = "<<index_prev<<" index_mid = "<<index_mid<<" index_next  = "<<index_next<<endl;
	    PndSdsHit* Hit_prev = (PndSdsHit*) fStripHitArray->At(index_prev);
	    TVector3 Hit_prevPos = Hit_prev->GetPosition();
	    
	    PndSdsHit* Hit_mid = (PndSdsHit*) fStripHitArray->At(index_mid);
	    TVector3 Hit_midPos = Hit_mid->GetPosition();
	    
	    PndSdsHit* Hit_next = (PndSdsHit*) fStripHitArray->At(index_next);
	    TVector3 Hit_nextPos = Hit_next->GetPosition();
	  
	    
	    double d_x =(Hit_nextPos.Y()-Hit_prevPos.Y())*(Hit_prevPos.Z()-Hit_midPos.Z())
	      -(Hit_nextPos.Z()-Hit_prevPos.Z())*(Hit_prevPos.Y()-Hit_midPos.Y());
	    double d_y =(Hit_nextPos.Z()-Hit_prevPos.Z())*(Hit_prevPos.X()-Hit_midPos.X())
	      -(Hit_nextPos.X()-Hit_prevPos.X())*(Hit_prevPos.Z()-Hit_midPos.Z());
	    double d_z =(Hit_nextPos.X()-Hit_prevPos.X())*(Hit_prevPos.Y()-Hit_midPos.Y())
	      -(Hit_nextPos.Y()-Hit_prevPos.Y())*(Hit_prevPos.X()-Hit_midPos.X());
	    double norm_v = sqrt(pow((Hit_nextPos.X()-Hit_prevPos.X()),2)
				 +pow((Hit_nextPos.Y()-Hit_prevPos.Y()),2)
				 +pow((Hit_nextPos.Z()-Hit_prevPos.Z()),2));
	    double d = sqrt(d_x*d_x+d_y*d_y+d_z*d_z)/norm_v;

	    ihit=1;
	    index_prev = trkcan_point(trkc,ihit-1);
	    if(index_prev==-1){
	      ihit=ihit+1;
	      index_prev = trkcan_point(trkc,ihit-1);
	    }
	    index_mid = trkcan_point(trkc,ihit);
	    if(index_mid==-1){
	      ihit=ihit+1;
	      index_mid = trkcan_point(trkc,ihit);
	    }
	    index_next = trkcan_point(trkc,ihit+1);
	    if(index_next==-1) index_next = trkcan_point(trkc,ihit+2);

	    Hit_prev = (PndSdsHit*) fStripHitArray->At(index_prev);
	    Hit_prevPos = Hit_prev->GetPosition();
	
	    Hit_mid = (PndSdsHit*) fStripHitArray->At(index_mid);
	    Hit_midPos = Hit_mid->GetPosition();
	    
	    Hit_next = (PndSdsHit*) fStripHitArray->At(index_next);
	    Hit_nextPos = Hit_next->GetPosition();
	    // cout<<"index_prev = "<<index_prev<<" index_mid = "<<index_mid<<" index_next  = "<<index_next<<endl;
	    
	    d_x =(Hit_nextPos.Y()-Hit_prevPos.Y())*(Hit_prevPos.Z()-Hit_midPos.Z())
	      -(Hit_nextPos.Z()-Hit_prevPos.Z())*(Hit_prevPos.Y()-Hit_midPos.Y());
	    d_y =(Hit_nextPos.Z()-Hit_prevPos.Z())*(Hit_prevPos.X()-Hit_midPos.X())
	      -(Hit_nextPos.X()-Hit_prevPos.X())*(Hit_prevPos.Z()-Hit_midPos.Z());
	    d_z =(Hit_nextPos.X()-Hit_prevPos.X())*(Hit_prevPos.Y()-Hit_midPos.Y())
	      -(Hit_nextPos.Y()-Hit_prevPos.Y())*(Hit_prevPos.X()-Hit_midPos.X());
	    norm_v = sqrt(pow((Hit_nextPos.X()-Hit_prevPos.X()),2)
			  +pow((Hit_nextPos.Y()-Hit_prevPos.Y()),2)
			  +pow((Hit_nextPos.Z()-Hit_prevPos.Z()),2));
	    double d_new = sqrt(d_x*d_x+d_y*d_y+d_z*d_z)/norm_v;
	    //	    cout<<"d = "<<d<<" d_new = "<<d_new<<endl;
	    if(d_new<d){
	      addTrk=false;
	      break;
	    }
	    break;
	  }
	}
	//	cout<<"addTrk = "<<addTrk<<endl;
	if(addTrk==true && flag(trkcID)==false){
	  if(fVerbose>4) cout<<"trk-cand #"<<trkcID<<" is added (with 3 hits)!"<<endl;
	  new((*fTrackCandArray)[NsaveTrk]) PndTrackCand(*(trcnd)); //save Track Candidate
	  NsaveTrk++;
	  flag(trkcID)=true;
	}
      }
    }
  }
  else{
    int NsaveTrk = 0;
    Int_t ntcand=fTrackCandArrayTemp->GetEntriesFast();
    if(fVerbose>4) cout<<"ntcand from ArrayTemp is "<<ntcand<<endl;
    for(int trkcID=0;trkcID<ntcand;trkcID++){
      PndTrackCand* trcnd = (PndTrackCand*)fTrackCandArrayTemp->At(trkcID);
      new((*fTrackCandArray)[NsaveTrk]) PndTrackCand(*(trcnd)); //save Track Candidate
      NsaveTrk++;
    }
  }
	
  //	Delete fTrackCandArrayTemp;
  Int_t ntcandFin=fTrackCandArray->GetEntriesFast();
  if(fVerbose>2) cout<<"Number of Trk-Cands is "<<ntcandFin<<endl;
  /// End(Track-Candidates Fillter) -------------------------------------
  if(fVerbose>2) cout << "Evt finsihed--------------"<<endl<<endl;
  // if(fTrackCandArrayTemp) delete fTrackCandArrayTemp;
}
// -------------------------------------------------------------------------

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

