// -------------------------------------------------------------------------
// -----                      PndLmdTrackFinderTask                    -----
// -----                  Created 22/10/09  by M. Michel               -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndLmdTrackFinderTask.h"

#include "PndSdsDigiStrip.h"
// #include "PndSdsPixelCluster.h"


// -----   Default constructor   -------------------------------------------
PndLmdTrackFinderTask::PndLmdTrackFinderTask() :
  FairTask("LMD Track Finding Task")
{
   fHitBranchStrip = "LMDHitsStrip";
   fClusterBranchStrip = "LMDStripClusterCand";
   fDigiBranchStrip = "LMDStripDigis";
   dXY = 0.5;
}


// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndLmdTrackFinderTask::~PndLmdTrackFinderTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndLmdTrackFinderTask::SetParContainers()
{
  // Get Base Container
/*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));
*/
}

InitStatus PndLmdTrackFinderTask::ReInit()
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
InitStatus PndLmdTrackFinderTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndLmdTrackFinderTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fStripHitArray = (TClonesArray*) ioman->GetObject(fHitBranchStrip);
  if ( !fStripHitArray){
    std::cout << "-W- PndLmdTrackFinderTask::Init: " << "No fStripHitArray!" << std::endl;
    return kERROR;
  }

  fStripClusterArray = (TClonesArray*) ioman->GetObject(fClusterBranchStrip);
  if ( !fStripClusterArray){
    std::cout << "-W- PndLmdTrackFinderTask::Init: " << "No StripclusterArray!" << std::endl;
    return kERROR;
  }

  fStripDigiArray = (TClonesArray*) ioman->GetObject(fDigiBranchStrip);
  if ( !fStripDigiArray){
    std::cout << "-W- PndLmdTrackFinderTask::Init: " << "No StripdigiArray!" << std::endl;
    return kERROR;
  }

  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("LMDTrackCand", "PndLmd", fTrackCandArray, kTRUE);

  std::cout << "-I- PndLmdTrackFinderTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndLmdTrackFinderTask::Exec(Option_t* opt)
{
  if(fVerbose>2) cout << "Evt started--------------"<<endl<<endl;

  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Clear();

  Int_t nStripHits = fStripHitArray->GetEntriesFast();

  if(nStripHits<2){
    if(fVerbose>2) cout << "Evt finsihed: too less hits-----"<<endl<<endl;
    return;
  }
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
  std::vector< std::vector<Int_t> > hitsd(detZ.size()); //hit'ids splitted by detectorplane

//sort in plane's
  for(Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(iHit));

    Double_t z = myHit->GetZ();
    for(Int_t idet = 0; idet < detZ.size(); idet++){ //planes
     
      // if( z == detZ.at(idet) ){
      if( fabs(z-detZ.at(idet))<9. ){ //[for using with Dipole]
        hitsd.at(idet).push_back(iHit);
	//	cout<<"detZ.at("<<idet<<")="<<detZ.at(idet)<<" z="<<z<<endl;
      }
    }
  }

   cout << "Hits: " << nStripHits << endl;
  // if(fVerbose>2) {
  //   cout << "Hits: " << nStripHits << " in " << detZ.size() << " plane(s)." << endl;
  //   for(Int_t idet = 0; idet < detZ.size(); idet++)
  //     cout << "Plane: "<< idet <<" DiscHits: "<< hitsd.at(idet).size() <<endl;
  // }

  if(detZ.size()<2){
    if(fVerbose>2) cout << "Evt finsihed: too less planes-----"<<endl<<endl;
    return;
  }

  std::vector<Double_t> trackStartx, trackStarty, trackStartz, trackVecx, trackVecy, trackVecz;//pseudo tracks
  std::vector<Double_t> trackStartdx, trackStartdy, trackStartdz, trackVecdx, trackVecdy, trackVecdz;//save errors 
  std::vector<Int_t> trackID1, trackID2; //for TrackCand

//iterate first discs-hits with all seconds, save pseudo-tracks
  TVector3 start, tmp, vec, dstart, dvec; //temp-vars
  if(fVerbose>2){
    if(detZ.size()>0){
      for (Int_t i=0; i<hitsd.at(0).size(); i++){
	PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(0).at(i));
	cout<<"Plane0 Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<")"<<endl;
      }
    }
    if(detZ.size()>1){
      for (Int_t i=0; i<hitsd.at(1).size(); i++){
	PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(1).at(i));
	cout<<"Plane1 Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<")"<<endl;
      }
    }
    if(detZ.size()>2){
      for (Int_t i=0; i<hitsd.at(2).size(); i++){
	PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(2).at(i));
	cout<<"Plane2 Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<")"<<endl;
      }
    }
    if(detZ.size()>3){
      for (Int_t i=0; i<hitsd.at(3).size(); i++){
	PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(3).at(i));
	cout<<"Plane3 Hit=("<<hit->GetX()<<", "<<hit->GetY()<<", "<<hit->GetZ()<<")"<<endl;
      }
    }
  }
  
  for (Int_t i=0; i<hitsd.at(0).size(); i++)
    {
    PndSdsHit *hit1=(PndSdsHit*)fStripHitArray->At(hitsd.at(0).at(i));
    start.SetXYZ(hit1->GetX(), hit1->GetY(), hit1->GetZ());
    for (Int_t k=0; k<hitsd.at(1).size(); k++)
    {
      PndSdsHit *hit2=(PndSdsHit*)fStripHitArray->At(hitsd.at(1).at(k));
      tmp.SetXYZ(hit2->GetX(), hit2->GetY(), hit2->GetZ());
      vec = tmp - start;   //calc direction vector for FINDING
      trackStartx.push_back(start.x());
      trackStarty.push_back(start.y());
      trackStartz.push_back(start.z());
      trackStartdx.push_back(hit1->GetDx());
      trackStartdy.push_back(hit1->GetDy());
      trackStartdz.push_back(hit1->GetDz());
      trackVecx.push_back(vec.x());        //save vector from start to second
      trackVecy.push_back(vec.y());
      trackVecz.push_back(vec.z());
      trackVecdx.push_back(hit2->GetDx()); //save error of second point for FIT
      trackVecdy.push_back(hit2->GetDy()); //NOT error of direction vector
      trackVecdz.push_back(hit2->GetDz());
      trackID1.push_back(hitsd.at(0).at(i));  //save Hit-Id's for TrackCand
      trackID2.push_back(hitsd.at(1).at(k));
    }//end of disc 2 hits
  }//end of disc 1 hits

  if(fVerbose>1) cout << "Pseudos: "<< trackStartx.size() <<endl;

  std::vector<Int_t> ids;

  Int_t trackCnt=0;
//check if other discs have hits in track, add points for fitting ---------------
  for (Int_t i=0; i<trackStartx.size(); i++) //pseudo-loop
  {
    ids.clear();
    Int_t pntcnt=2;
    start.SetXYZ(trackStartx.at(i), trackStarty.at(i), trackStartz.at(i));
    dstart.SetXYZ(trackStartdx.at(i), trackStartdy.at(i), trackStartdz.at(i));
    vec.SetXYZ(trackVecx.at(i), trackVecy.at(i), trackVecz.at(i));
    dvec.SetXYZ(trackVecdx.at(i), trackVecdy.at(i), trackVecdz.at(i));
    ids.push_back(trackID1.at(i));
    ids.push_back(trackID2.at(i));

    for (Int_t idet=2; idet < detZ.size(); idet++){
      Double_t distClosest = 2*idet*dXY; //just bigger as possible
      Bool_t first = true;
      for (Int_t ihit=0; ihit<hitsd.at(idet).size(); ihit++)
      {
        PndSdsHit *hit=(PndSdsHit*)fStripHitArray->At(hitsd.at(idet).at(ihit));
        Double_t scale = (hit->GetZ()-start.z())/vec.z();
        tmp = start + scale*vec; //extend search-vector to hit-plane
        Double_t distTmp = sqrt((tmp.x()-hit->GetX())*(tmp.x()-hit->GetX()) + (tmp.y()-hit->GetY())*(tmp.y()-hit->GetY()));
        if( distTmp<(idet*dXY) ){ //if in diameter
          if(first){
            pntcnt++;
            ids.push_back(hitsd.at(idet).at(ihit));
            distClosest=distTmp;
            first=false;
          }else{
            if(distTmp<distClosest){ //change if nearer
              ids.pop_back();
              ids.push_back(hitsd.at(idet).at(ihit)); 
            }            
          }
        }//endif in diameter
      }//end of disc n hits
    }//end of discs

    if(fVerbose>2) cout << "  Track: "<< i << "#Planes: " << ids.size() <<endl;

    //save track für fitting
    if(ids.size()>2){ //third hit found <=> !track found!
      PndTrackCand *myTCand = new PndTrackCand();

      for (Int_t id=0; id<ids.size(); id++){
        PndSdsHit* myHit = (PndSdsHit*)(fStripHitArray->At(ids.at(id)));
        PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
        PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
        myTCand->AddHit(astripdigi->GetDetID(),ids.at(id),myHit->GetPosition().Mag());    
      }

      new((*fTrackCandArray)[trackCnt]) PndTrackCand(*(myTCand)); //save Track Candidate
      trackCnt++;
      delete myTCand;
    }//Track Cand build
  }//end of pseudo-tracks
  if(fVerbose>2) cout << "Evt finsihed--------------"<<endl<<endl;
}
// -------------------------------------------------------------------------

Double_t PndLmdTrackFinderTask::GetTrackCurvature(PndMCTrack* myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (2/TMath::Sqrt(p.Px()*p.Px() + p.Py()*p.Py()));
}
// -------------------------------------------------------------------------

Double_t PndLmdTrackFinderTask::GetTrackDip(PndMCTrack* myTrack)
{
  TVector3 p= myTrack->GetMomentum();
  return (p.Mag()/TMath::Sqrt(p.Px()*p.Px() + p.Py()*p.Py()));
}


// -------------------------------------------------------------------------
ClassImp(PndLmdTrackFinderTask);

