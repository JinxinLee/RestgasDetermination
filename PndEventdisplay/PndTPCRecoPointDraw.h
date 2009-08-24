// -------------------------------------------------------------------------
// -----                      FairTPCRecoPointDraw header file                    -----
// -----          Created 03/01/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------


/** PndTPCRecoPointDraw
 * @author T. Stockmanns
 * @since 03.01.09
 *   Task to display TPC points
 **
 **/

#ifndef PndTPCRecoPointDRAW_H
#define PNdTPCRecoPointDRAW_H


#include "FairBoxSetDraw.h"


class PndTPCRecoPointDraw : public FairBoxSetDraw {

 public:

  /** Default constructor **/
  PndTPCRecoPointDraw(){};


  /** Standard constructor
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  PndTPCRecoPointDraw(const char* name, Int_t iVerbose = 1):FairBoxSetDraw(name, iVerbose){};

  /** Destructor **/
  virtual ~PndTPCRecoPointDraw(){};

protected:
	virtual TVector3 GetVector(TObject* obj);
  ClassDef(PndTPCRecoPointDraw,3);

};


#endif
