//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcVertexingTask
//      see PndTpcVertexingTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Johannes Rauch    TUM            (original author)
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcVertexingTask.h"

// C/C++ Headers ----------------------
#include <map>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

#include "GFTrack.h"
#include "RKTrackRep.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"
#include "FairRunAna.h"
#include "GFDetPlane.h"
#include "GFFieldManager.h"

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TVector3.h"
#include "TMath.h"
#include "TDatabasePDG.h"

#include "PndDetectorList.h"

#include "GFRaveVertexFactory.h"
#include "GFRaveVertex.h"

#include <cmath>

using namespace std;

// Class Member definitions -----------

ClassImp(PndTpcVertexingTask)

PndTpcVertexingTask::PndTpcVertexingTask()
  : FairTask("PndTpcVertexing"),
    _persistence(kFALSE),
    _trackBranchName("TrackPostFit"),
    fMethod("default"),
    fUseBeamspot(false),
    fBeamCov(3,3)
  {
    fVerbose = 0;
  }

PndTpcVertexingTask::~PndTpcVertexingTask(){
}


InitStatus
PndTpcVertexingTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0){
    Error("PndTpcVertexingTask::Init","RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  if(_trackArray==0){
    Error("PndTpcVertexingTask::Init","GFTrack-array not found!");
    return kERROR;
  }

  /*_mvdArray=(TClonesArray*) ioman->GetObject("MVDPoint");
  if(_mvdArray==0){
    Error("PndTpcVertexingTask::Init","mvd-array not found!");
  }*/

  _vertexArray = new TClonesArray("GFRaveVertex");
  ioman->Register("GFVertex","Tpc",_vertexArray,_persistence);

  fVertexFactory = new GFRaveVertexFactory(fVerbose);
  fVertexFactory->setBeamspot(fBeamPos, fBeamCov);

  // init fVertexBuffer
  fVertexBuffer = new std::vector < GFRaveVertex* >;

  return kSUCCESS;
}


void
PndTpcVertexingTask::SetParContainers() {

  std::cout<<"PndTpcClusterFinderTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

}


void
PndTpcVertexingTask::SetBeamspot(const TVector3 & pos, const TMatrixT<double> & cov3x3){
  fBeamPos = pos;
  fBeamCov = cov3x3;
  fUseBeamspot = true;
}


void
PndTpcVertexingTask::SetBeamspot(const TVector3 & pos, double err){
  fBeamPos = pos;
  fBeamCov.UnitMatrix();
  fBeamCov *= err;
  fUseBeamspot = true;
}


void
PndTpcVertexingTask::Exec(Option_t* opt)
{

  std::cout<<"PndTpcVertexingTask::Exec \n";

  // Reset output Arrays
  if(_trackArray==0) Fatal("PndTpcVertexingTask::Exec","No GFTrack Array");
  _vertexArray->Delete();

  //clear and delete fVertexBuffer
  for (unsigned int i=0; i<fVertexBuffer->size(); ++i){
    delete (*fVertexBuffer)[i];
  }
  fVertexBuffer->clear();
  delete fVertexBuffer;

  // put GFTracks into vector
  unsigned int nTrks=_trackArray->GetEntriesFast();
  std::vector<GFTrack*> tracks;
  tracks.reserve(nTrks);
  for(unsigned int i=0; i<nTrks; ++i){
    tracks.push_back((GFTrack*)_trackArray->At(i));
  }

  std::cout<<nTrks;

  // create vertices
  fVertexBuffer = fVertexFactory->create(tracks, fUseBeamspot);

  // copy vertices into output array
  unsigned int nVert(fVertexBuffer->size());
  for (unsigned int i=0; i<nVert; ++i){
    GFRaveVertex* vert = new((*_vertexArray)[i]) GFRaveVertex(*((*fVertexBuffer)[i]));
  }

  std::cout << nVert << " vertices found\n";

}

