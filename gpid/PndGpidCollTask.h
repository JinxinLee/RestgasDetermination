#pragma once
#ifndef PNDGPIDCOLLTASK_H
#define PNDGPIDCOLLTASK_H

//ROOT & PandaRoot Headers
#include "CbmTask.h"
#include "TVector3.h"
#include "CbmGeanePro.h"
#include "CbmGeane.h"
#include "PndTofHit.h"
#include "PndLhePidTrack.h"
#include "PndTpcPoint.h"
#include "PndMvdMCPoint.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheHit.h"
#include "CbmMCTrack.h"

//C++ headers
#include <algorithm>
#include <vector>

class PndGpidCollTask : public CbmTask
{
 public:
  /** Default constructor **/  
  PndGpidCollTask();

  /** Destructor **/
  virtual ~PndGpidCollTask();
  
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  void smear(Double_t& theta, const Double_t& del );
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  //protected:

 private:
  //Functions and methods
  void destroy(void);
  /** Input array of Points **/
  TClonesArray* fTrackArray1;
  TClonesArray* fPointArray1;
  TClonesArray* fPointArray2;
  TClonesArray* fPidTrack;  
  //  CbmPlanePoint	 *fPoint1;
  //  CbmMCTrack	 *fPoint1;
  PndTpcPoint *fTpcPoint;
  PndMvdMCPoint *fMvdPoint;
  PndTpcLheTrack *fTrack1;
  PndTofHit	 *fPoint2;
  
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fPndTrackArr;  
  TClonesArray* fMCTrackArr;  
  
  ClassDef(PndGpidCollTask,1);
};
#endif
