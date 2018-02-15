// -------------------------------------------------------------------------
// -----                      FairTPCRecoPointDraw header file                    -----
// -----          Created 03/01/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------


/** PndEmcHitCaloDraw
 * @author T. Stockmanns
 * @since 03.01.09
 *   Task to display TPC points
 **
 **/

#ifndef PndEmcHitCaloDraw_H
#define PndEmcHitCaloDraw_H


#include "PndCaloDraw.h"


class PndEmcHitCaloDraw : public PndCaloDraw {

 public:

  /** Default constructor **/
  PndEmcHitCaloDraw(){
  };


  /** Standard constructor
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  PndEmcHitCaloDraw(const char* name, Int_t iVerbose = 1):PndCaloDraw(name, iVerbose){
  };

  /** Destructor **/
  virtual ~PndEmcHitCaloDraw(){};

protected:
	virtual TVector3 GetVector(TObject* obj);
	virtual Double_t GetEnergy(TObject* obj);
  ClassDef(PndEmcHitCaloDraw,1);

};


#endif
