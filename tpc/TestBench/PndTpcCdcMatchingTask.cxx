//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      TPC-CDC Matching Routine
//
//
// Environment:
//      Software developed for the Prototype Detector at FOPI
//
// Author List:
//      Robert Münzer          TUM
//
//-----------------------------------------------------------



// This Class' Header ------------------
#include "PndTpcCdcMatchingTask.h"

// C++ headers
#include <iostream>
#include <algorithm>
#include <vector>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h" 
#include "TVector3.h"
#include "PndTpcCluster.h"
#include "TrackFitStat.h"

#include "TGeoManager.h"

#include "GFAbsTrackRep.h"
#include "GFAbsRecoHit.h"
#include "GFDetPlane.h"
#include "GFException.h"
#include "GFKalman.h"
//#include "GFTools.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "FopiEvent.h"
#include "RKTrackRep.h"
#include "CdcCircle.h"
#include "GFDetPlane.h"
#include "PseudoSpacePoint.h"

#define DEBUG 0


PndTpcCdcMatchingTask::PndTpcCdcMatchingTask()
  : fTpcClusterBranchName("PndTpcCluster"),
    fTpcTrackBranchName("TrackPostFit"),    
    fCdcTrackBranchName("PndTpcCdcTrack"),
    fCdcHitBranchName("PndTpcCdcHit"),
    fCdcEvBranchName("PndTpcCdcEvent"),
    fmaxmatdistance(3),
    fmaxmatphi(4*acos(0.)),
    fminhitsperlength(0.), 
    fmatablecdctracks(0),
    fmatchedcdctracks(0),    
    fPersistence(kTRUE),
    fSecondarySupp(kFALSE),
    fNumReps(1)
{;}

PndTpcCdcMatchingTask::~PndTpcCdcMatchingTask(){
  delete fPar;
}


InitStatus
PndTpcCdcMatchingTask::Init() {

  //Get ROOT Manager ------------------
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0)
    {
      Error("PndTpcCdcMatchingTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input ------------------------
  fTpcClusterArray=(TClonesArray*) ioman->GetObject(fTpcClusterBranchName);
  if(fTpcClusterArray==0) {
    Error("PndTpcCdcMatchingTask::Init","PndTpcCluster array not found!");
    return kERROR;
  }
  fTpcTrackArray=(TClonesArray*) ioman->GetObject(fTpcTrackBranchName);
  if(fTpcTrackArray==0) {
    Error("PndTpcCdcMatchingTask::Init","GFTrack array not found!");
    return kERROR;
  }
  fCdcHitArray=(TClonesArray*) ioman->GetObject(fCdcHitBranchName);
  if(fCdcHitArray==0) {
    Error("PndTpcCdcMatchingTask::Init","Cdc Hits array not found!");
    return kERROR;
  }  
  fCdcTrackArray=(TClonesArray*) ioman->GetObject(fCdcTrackBranchName);
  if(fCdcTrackArray==0) {
    Error("PndTpcCdcMatchingTask::Init","CDC track array not found!");
    return kERROR;
  }  
  fCdcEventArray=(TClonesArray*) ioman->GetObject(fCdcEvBranchName);
  if(fCdcEventArray==0) {
    Error("PndTpcCdcMatchingTask::Init","CDC event not found!");
    return kERROR;
  } 


  //  std::cout<<"PRE INIT"<<std::endl;
  //  fCdcTpcMatArray = new TClonesArray("GFTrack");
  
  fCdcTpcMatArrayOutArray = new TClonesArray("TMatrixT<double>"); 
  fCdcTpcMatPreFitOutArray = new TClonesArray("GFTrack");
  ioman->Register("PndTpcCdcMatArray","PndTpc",fCdcTpcMatArrayOutArray,fPersistence);
  ioman->Register("PndTpcCdcPreFit","PndTpc",fCdcTpcMatPreFitOutArray,fPersistence);
  
  //  std::cout<<"INIT OUT ARRAYS"<<std::endl;
  

  return kSUCCESS;
}

void 
PndTpcCdcMatchingTask::SetParContainers() {

  //  std::cout<<"PndTpcResidualTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  fPar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (!fPar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}
bool sortfunction(TVector3 i,TVector3 j){
  return(i.Perp()<j.Perp());
}

void
PndTpcCdcMatchingTask::Exec(Option_t* opt) {
  std::cerr << "PndTpcCdcMatchingTask::Exec" << std::endl;
  if(fCdcTpcMatPreFitOutArray==0)
     Fatal("PndTpcCdcDataReaderTask::Exec()","No SampleOutArray");
  fCdcTpcMatPreFitOutArray->Delete();
  fCdcTpcMatArrayOutArray->Clear();
  std::cerr << "PndTpcCdcMatchingTask: delete" << std::endl;
  Int_t NumCdcTracks = fCdcTrackArray->GetEntriesFast();
  Int_t NumTpcTracks = fTpcTrackArray->GetEntriesFast();

  Int_t TpcId = 8;

  CdcCircle cdctrackcircle[NumCdcTracks];
  //CdcEvent *aCdcevent = (CdcEvent*) fCdcEventArray->At(0);
  //Int_t NumHits = aCdcevent->GetNhit();
  //TVector3 Vertex = aCdcevent->GetVertex();
  //std::cout << "vertex Z: " << Vertex.Z() << std::endl;

  std::vector<unsigned int> candIDs;

  TMatrixT<double> matchcdctpcvalue;
  TMatrixT<double> matchcdctpc;
  matchcdctpc.ResizeTo(NumCdcTracks,NumTpcTracks);
  matchcdctpcvalue.ResizeTo(NumCdcTracks,NumTpcTracks);
 
  Int_t CdcMatAbleTracks = 0;
  for(Int_t cdctrack=0;cdctrack<NumCdcTracks;cdctrack++){

    CdcTrack *cdcsingletrack = (CdcTrack*) fCdcTrackArray->At(cdctrack);
    cdctrackcircle[cdctrack].SetRXYP(cdcsingletrack->GetRadius(),cdcsingletrack->GetMx(),cdcsingletrack->GetMy(),cdcsingletrack->GetPhi());
    //std::cout<<"Tracklength("<<cdctrack<<") :"<<cdctrackcircle[cdctrack].GetTpcLength()<<" "<<ftpczoffset<<std::endl;# 
    if( cdcsingletrack->GetNpoint()<30 ) continue;
    // in case this could be implemented in the CDC reference system 
    //    if( cdcsingletrack->GetZ0()>-10||cdcsingletrack->GetZ0()<-50 ) continue;
    CdcMatAbleTracks += 1;
    for(Int_t tpctrack=0;tpctrack<NumTpcTracks;tpctrack++){
      matchcdctpcvalue[cdctrack][tpctrack] = 0;
      matchcdctpc[cdctrack][tpctrack] = -1;
      GFTrack* tpcsingletrack = (GFTrack*) fTpcTrackArray->At(tpctrack);
      GFTrackCand cand   = tpcsingletrack->getCand();

      candIDs.clear(); 
      candIDs = cand.GetHitIDs(TpcId);
      if(DEBUG){
	std::cout<<"tpctrack: "<<tpctrack<<" "<<candIDs.size()<<std::endl;
      }  
      for (Int_t nhit=0;nhit<candIDs.size();nhit++){

	PndTpcCluster *tpcsinglecluster = (PndTpcCluster*) fTpcClusterArray->At(candIDs[nhit]);
	TVector3 hitpoint = tpcsinglecluster->pos();
	Float_t amp       = tpcsinglecluster->amp();
	Float_t size      = tpcsinglecluster->get2DSize();
	//if(size<2) continue;
	//	if(amp<30) continue;
	if(cdctrackcircle[cdctrack].MatPoint(hitpoint,fmaxmatdistance,fmaxmatphi)!=0)
	  matchcdctpcvalue[cdctrack][tpctrack] += 1/(cdctrackcircle[cdctrack].GetTpcLength()*cdctrackcircle[cdctrack].MatPoint(hitpoint,fmaxmatdistance,fmaxmatphi));
	
      }	

    }

  } 
  if(DEBUG){
    matchcdctpcvalue.Print();
  }
  Int_t NumMatchTrack=std::min(NumCdcTracks,NumTpcTracks);

  TMatrixT<double> tpc; // n Entrey returns Id of TPC Track with matches to CDC-Track with id n
  tpc.ResizeTo(NumTpcTracks,1);  
  std::cerr << "PndTpcCdcMatchingTask::Exec late" << std::endl;
  for (Int_t itertrack=0;itertrack<NumMatchTrack;itertrack++){
    Float_t maximalvalue=fminhitsperlength;
    Int_t cdctrackmaximum=-1;
    Int_t tpctrackmaximum=-1;
    for (Int_t tp=0;tp<NumTpcTracks;tp++){
      tpc[tp][0]=tp+1;
      for( Int_t ct=0;ct<NumCdcTracks;ct++){
	if(matchcdctpcvalue[ct][tp]>maximalvalue&&matchcdctpc[ct][tp]==-1){
	  maximalvalue=matchcdctpcvalue[ct][tp];
	  cdctrackmaximum=ct;
	  tpctrackmaximum=tp;
	}
      }
    }
    for (Int_t tp=0;tp<NumTpcTracks;tp++){
      for( Int_t ct=0;ct<NumCdcTracks;ct++){
	if(tp==tpctrackmaximum&&ct==cdctrackmaximum){
	  matchcdctpc[ct][tp]=1;		  
	}
	else if(tp==tpctrackmaximum||ct==cdctrackmaximum){
	  matchcdctpc[ct][tp]=0; 
	}
	
      }
    }      
  } 
  TMatrixT<double>* mattrackid = new((*fCdcTpcMatArrayOutArray)[fCdcTpcMatArrayOutArray->GetEntriesFast()]) TMatrixT<double>(NumCdcTracks,9);
  //TMatrixT<double> mattrackid; // n Entrey returns Id of TPC Track with matches to CDC-Track with id n
  TMatrixT<double> cdctotpc =matchcdctpc*tpc ;
  if(DEBUG){
    matchcdctpc.Print();
  }
  Int_t intnum=0;
  for( Int_t ct=0;ct<NumCdcTracks;ct++){ 
    (*mattrackid)[intnum][0] = -1;
    (*mattrackid)[intnum][1] = -1;
    (*mattrackid)[intnum][2] = -1; 
    (*mattrackid)[intnum][3] = -1000.;
    (*mattrackid)[intnum][4] = -1000.;
    (*mattrackid)[intnum][5] = -1000.;
    (*mattrackid)[intnum][6] = -1000.;
    (*mattrackid)[intnum][7] = -1000.;
    (*mattrackid)[intnum][8] = -1000.;		  
    if(cdctotpc[ct][0]<1){
      continue;
    }
    (*mattrackid)[intnum][0] = intnum;
    (*mattrackid)[intnum][1] = ct;
    (*mattrackid)[intnum][2] = cdctotpc[ct][0]-1;
    (*mattrackid)[intnum][3] = -1000.;
    (*mattrackid)[intnum][4] = -1000.;
    (*mattrackid)[intnum][5] = -1000.; 
    (*mattrackid)[intnum][6] = -1000.;
    (*mattrackid)[intnum][7] = -1000.;
    (*mattrackid)[intnum][8] = -1000.;		 
    intnum += 1;
  } 
  (*mattrackid).ResizeTo(intnum,9); 
  if(DEBUG) { 
    std::cout<<"--------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------"<<std::endl;
  }
  fmatablecdctracks += CdcMatAbleTracks;
  fmatchedcdctracks += intnum;
  if(intnum<1) {
    if(DEBUG) { 
      std::cout<<"-----------'intum < 1' -> return--------"<<std::endl;
    }
    return;
  } 
  else{ 
    Double_t matchingefficiency = (Double_t) fmatchedcdctracks/(Double_t) fmatablecdctracks;
    std::cout<<"# of CDC/TPC tracks matched found: "<<CdcMatAbleTracks<<"|"<<NumTpcTracks<<"==>"<<intnum<<" eff( "<<fmatchedcdctracks<<"|"<<fmatablecdctracks<<") : "<<matchingefficiency<<std::endl;
  }
  for(Int_t tid=0;tid<intnum;tid++){
    Int_t cdctrack=(Int_t) (*mattrackid)[tid][1];
    Int_t tpctrack=(Int_t) (*mattrackid)[tid][2];
    CdcTrack *cdcsingletrack = (CdcTrack*) fCdcTrackArray->At(cdctrack); 
    GFTrack* tpcsingletrack = (GFTrack*) fTpcTrackArray->At(tpctrack);
    
    std::vector<unsigned int> hitIDs;
    hitIDs.clear();
    hitIDs = cdcsingletrack->GetCdcIdHits();  
    std::vector<TVector3> hitIDsorter;    
    for (Int_t nhit=0;nhit<hitIDs.size();nhit++){
      CdcHit* chit = (CdcHit*) fCdcHitArray->At(hitIDs[nhit]);      
      TVector3 hitpoint = chit->GetHitPos();
      hitpoint.SetZ((Float_t) hitIDs[nhit]);
      hitIDsorter.push_back(hitpoint);
    }
    sort(hitIDsorter.begin(),hitIDsorter.end(),sortfunction);

    CdcHit* cchit = (CdcHit*) fCdcHitArray->At((Int_t) hitIDsorter.at(0).Z());
    GFAbsRecoHit *thehit = tpcsingletrack->getHit(0)->clone();
    TMatrixT<double> thehitpoint = thehit->getRawHitCoord();	
    TVector3 clpos = cchit->GetHitPos();
    clpos.SetXYZ(thehitpoint[0][0],thehitpoint[1][0],thehitpoint[2][0]);

    cchit = (CdcHit*) fCdcHitArray->At((Int_t) hitIDsorter.at(1).Z()); 
    thehit = tpcsingletrack->getHit(1)->clone();
    thehitpoint = thehit->getRawHitCoord();
    TVector3 mom = cchit->GetHitPos()-clpos;    
    Float_t cdcmomentum=cdcsingletrack->GetMom();
    Float_t cdctheta=cdcsingletrack->GetTheta();
    Float_t cdcphi=cdcsingletrack->GetPhi();
    Float_t cdcz0 = cdcsingletrack->GetZ0();
    mom.SetPhi(cdcphi);
    TVector3 mom1=mom;
    TVector2 centpoint;
    //    if(fabs(atan(tan(centpoint.Phi()-mom.Phi())))>acos(0.)){
    if(cdcsingletrack->GetRadius()<0){
      centpoint.Set(-clpos.Y()+cdcsingletrack->GetMy(),-clpos.X()+cdcsingletrack->GetMx());
      //      std::cout<<"Changed: "<<cdcsingletrack->GetRadius()<<std::endl;
    }
    else{
      centpoint.Set(clpos.Y()-cdcsingletrack->GetMy(),clpos.X()-cdcsingletrack->GetMx());
      //      std::cout<<"UnChanged: "<<cdcsingletrack->GetRadius()<<std::endl;
      }
    Float_t trackphi = centpoint.Phi() ;
    mom.SetPhi(-trackphi);
    //    mom1.Print();
    //mom.Print();
    mom.SetTheta(cdctheta);
    mom.SetMag(cdcmomentum);
    // Set up Track Representation
    TVector3 poserr(0.03,0.03,6.0);
    //large mom error in the unknown projection:
    TVector3 momerr(mom.X()*0.1,mom.Y()*0.1,mom.Z()*0.2); 
    //Particle ID
    Float_t cdcmass=cdcsingletrack->GetMass();
    Float_t cdccharge=cdcsingletrack->GetCharge();
    int pdg;
    
    if(cdccharge<0&&!cdcmass>0.32){      
      pdg = -211; // pi-
    }
    else if(cdccharge<0&&cdcmass>0.32){
      pdg = -321; //k- 
    }
    else if(!cdcmass>0.32){
      pdg = 211; // pi+
    }
    else if(cdcmass<0.67){
      pdg = 321; //k+ 
    } 
    else {
      pdg = 2212; // p
    }
    
    RKTrackRep* rep= new RKTrackRep(clpos,mom,poserr,momerr,pdg);

    Int_t NumTpcHits=tpcsingletrack->getNumHits();

    GFTrack* trk=new((*fCdcTpcMatPreFitOutArray)[fCdcTpcMatPreFitOutArray->GetEntriesFast()]) GFTrack(rep);
    
    GFAbsTrackRep *singletpcrep = tpcsingletrack->getTrackRep(0);
    try{
      //      std::cout<<"Start fitting TPC tracks"<<std::endl;
      TVector3 Point1(0.,0.,0.);
      TVector3 Point2(0.,0.,10.);
      TVector3 poca;
      TVector3 normVec;
      TVector3 poca_onwire;
      singletpcrep->extrapolateToLine(Point1,Point2,poca,normVec,poca_onwire);
      if(DEBUG) {
      poca.Print();
      }
      (*mattrackid)[tid][6] = poca.X();
      (*mattrackid)[tid][7] = poca.Y();
      (*mattrackid)[tid][8] = poca.Z();
    }
    catch (GFException e){
      //      std::cout<<e.what()<<std::endl;
    }

    for(Int_t tpchit=0;tpchit<NumTpcHits;tpchit++){
      GFAbsRecoHit *theHit = tpcsingletrack->getHit(tpchit)->clone();
      trk->addHit(theHit,2.,candIDs[tpchit]);
    }  
    Float_t lastradius=0;

    for (Int_t nhit=0;nhit<hitIDs.size();nhit++){	 
      CdcHit* chit = (CdcHit*) fCdcHitArray->At((Int_t) hitIDsorter.at(nhit).Z());      
      TVector3 hitpoint = chit->GetHitPos();
      lastradius=hitpoint.Perp();
      //hitpoint.SetZ(hitpoint.Perp()/tan(cdctheta)+cdcz0);
      TVector3 hitXerr  = chit->GetXposErr();
      TVector3 hitYerr  = chit->GetYposErr();
      TVector3 hitZerr  = chit->GetZposErr();
      //hitXerr.SetMag(hitXerr.Mag()*50);
      //hitYerr.SetMag(hitYerr.Mag()*50);
      //hitZerr.SetMag(hitZerr.Mag()/50.);
      PseudoSpacePoint* thishit = new PseudoSpacePoint(hitpoint,hitXerr,hitYerr,hitZerr);
       trk->addHit(thishit,200.,hitIDs[nhit]);
    }
    
    GFKalman fitter;
    fitter.setNumIterations(3);
    try{
      //      std::cout<<"Start fitting TPC-CDC tracks"<<std::endl;
      fitter.processTrack(trk);
      TVector3 Point1(0.,0.,0.);
      TVector3 Point2(0.,0.,10.);
      TVector3 poca;
      TVector3 normVec;
      TVector3 poca_onwire;
      rep->extrapolateToLine(Point1,Point2,poca,normVec,poca_onwire);
      if(DEBUG) {
      poca.Print();
      }
      (*mattrackid)[tid][3] = poca.X();
      (*mattrackid)[tid][4] = poca.Y();
      (*mattrackid)[tid][5] = poca.Z();
    }
    catch (GFException e){
      //      std::cout<<e.what()<<std::endl;
    }
  }
}

ClassImp(PndTpcCdcMatchingTask)
