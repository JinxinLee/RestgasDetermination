// -------------------------------------------------------------------------
// -----                      PndTrackCandDraw header file                    -----
// -----          Created 10/12/07  by M. Al-Turany                    -----
// -------------------------------------------------------------------------


/** PndTrackCandDraw
 * @author M. Al-Turany
 * @since 03.01.08
 *   Task to display MC points
 **
 **/

#ifndef PndTrackCandDraw_H
#define PndTrackCandDraw_H


#include "FairHitDraw.h"
#include "TClonesArray.h"
#include "TVector3.h"

class FairBoxSet;

class PndTrackCandDraw : public FairBoxSetDraw {

 public:

  /** Default constructor **/
  PndTrackCandDraw(){};


  /** Standard constructor
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  PndTrackCandDraw(const char* TrackCandName, Int_t iVerbose = 1);

  /** Destructor **/
  virtual ~PndTrackCandDraw();
  virtual InitStatus Init();

  virtual void AddBoxesTrackCand(FairBoxSet* set, TObject* obj, Int_t i = 0);
  virtual void AddBoxesPndTrackCand(FairBoxSet* set, TObject* obj, Int_t i = 0);


protected:
	  virtual TVector3 GetVector(TObject* obj){};
	  virtual TVector3 GetVector(Int_t detId, Int_t hitId);
	  //virtual Int_t GetValue(TObject* obj,Int_t i);
	  virtual void AddBoxes(FairBoxSet* set, TObject* obj, Int_t i = 0);


  TClonesArray *fPixPointList; //!
  TClonesArray *fStripPointList; //!
  TClonesArray *fTrackCandList; //!
  TClonesArray *fSttHelixList; //!
  TClonesArray *fGemHitList; //!

  ClassDef(PndTrackCandDraw,1);

};


#endif
