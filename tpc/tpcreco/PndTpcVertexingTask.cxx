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
#include "GFException.h"

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
    fUseVacuumPropagator(false),
    _trackBranchName("TrackPostFit"),
    _vertexBranchName("GFVertex"),
    fMethod("kalman-smoothing:1"),
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

  // register output arrays
  _vertexArray = new TClonesArray("GFRaveVertex");
  ioman->Register(_vertexBranchName,"Tpc",_vertexArray,_persistence);


  // init vertex factory
  fVertexFactory = new GFRaveVertexFactory(fVerbose, fUseVacuumPropagator);
  fVertexFactory->setBeamspot(fBeamPos, fBeamCov);
  fVertexFactory->setMethod(fMethod);

  fVertexBuffer = new std::vector < GFRaveVertex* >;

  return kSUCCESS;
}


void
PndTpcVertexingTask::SetParContainers() {
  ;
}


void
PndTpcVertexingTask::SetMethod(const TString& method){
  // TString to std::string
  std::string stdMethod(method.Data());
  fMethod = stdMethod;
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
  static unsigned int counter(0);
  std::cout<<"PndTpcVertexingTask::Exec - Event Nr. " << counter++ << "\n";

  // Reset output Array
  if(_vertexArray==0) Fatal("PndTpcVertexingTask::Exec","No GFRaveVertex Array");
  _vertexArray->Delete();

  // clear buffers
  for (unsigned int i=0; i<fVertexBuffer->size(); ++i){
    delete (*fVertexBuffer)[i];
  }
  fVertexBuffer->clear();
  fTrackBuffer.clear();

  // skip if not enough tracks
  unsigned int nTrks=_trackArray->GetEntriesFast();
  if (nTrks < 2) {
    if (fVerbose > 0) std::cout << "less than 2 tracks, skipping ...\n";
    return;
  }

  // put GFTracks into vector
  fTrackBuffer.reserve(nTrks);
  for(unsigned int i=0; i<nTrks; ++i){
    fTrackBuffer.push_back((GFTrack*)_trackArray->At(i));
  }

  // create vertices
  if (fVerbose > 0) std::cout << "find vertices from " << nTrks << " tracks\n";
  fVertexFactory->findVertices(fVertexBuffer, fTrackBuffer, fUseBeamspot);

  // copy vertices into output array
  unsigned int nVert(fVertexBuffer->size());
  for (unsigned int i=0; i<nVert; ++i){
    new((*_vertexArray)[i])  GFRaveVertex(*(*fVertexBuffer)[i]);
    if (fVerbose > 1) (*_vertexArray)[_vertexArray->GetLast()]->Print();
  }

  std::cout << nVert << " vertices found\n";
}

