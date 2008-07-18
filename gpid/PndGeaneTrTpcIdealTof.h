// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007

#ifndef PNDGEANETRTPCIDEALTOF_H
#define PNDGEANETRTPCIDEALTOF_H 1


#include "CbmTask.h"
#include "TVector3.h"
#include "CbmGeanePro.h"
#include "CbmGeane.h"
#include "PndTofPoint.h"
#include "PndTpcPoint.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheHit.h"
#include "CbmMCTrack.h"
#include <algorithm>
#include <vector>
class TGeant3;
class TClonesArray;
class TTree;
class TFile;

class PndGeaneTrTpcIdealTof : public CbmTask
{
 public:
  /** Default constructor **/  
  PndGeaneTrTpcIdealTof();

  /** Destructor **/
  virtual ~PndGeaneTrTpcIdealTof();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
 private:
  /** Input array of Points **/
  TClonesArray* fTrackArray1;
  TClonesArray* fPointArray1;
  TClonesArray* fPointArray2;
  //  CbmPlanePoint	 *fPoint1;
//  CbmMCTrack	 *fPoint1;
  PndTpcPoint *fPoint1;
  PndTpcLheTrack *fTrack1;
  PndTofPoint	 *fPoint2;

//  TTree *t;
//  TFile *f;
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fPndTrackArr;  
  TClonesArray* fMCTrackArr;  
//  TClonesArray* fTrackParFinal;  
  void destroy(void);
  TGeant3 *gMC3;
  CbmGeane *fGeane;
  Int_t fEvent;
  CbmGeanePro *fPro;
  ClassDef(PndGeaneTrTpcIdealTof,1);
};

#endif
