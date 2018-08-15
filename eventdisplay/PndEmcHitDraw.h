// -------------------------------------------------------------------------
// -----                      FairTPCRecoPointDraw header file                    -----
// -----          Created 03/01/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------


/** PndEmcHitDraw
 * @author T. Stockmanns
 * @since 03.01.09
 *   Task to display TPC points
 **
 **/

#ifndef PndEmcHitDraw_H
#define PndEmcHitDraw_H


#include "FairHitDraw.h"


class PndEmcHitDraw : public FairHitDraw {

 public:

  /** Default constructor **/
  PndEmcHitDraw(){
	  SetBoxDimensions(2.0,2.0,2.0);
  };


  /** Standard constructor
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  PndEmcHitDraw(const char* name, Int_t iVerbose = 1):FairHitDraw(name, iVerbose){
	  SetBoxDimensions(2.0,2.0,2.0);
  };

  /** Destructor **/
  virtual ~PndEmcHitDraw(){};

protected:
	virtual Int_t GetValue(TObject* obj, Int_t i);
  ClassDef(PndEmcHitDraw,3);

};


#endif
