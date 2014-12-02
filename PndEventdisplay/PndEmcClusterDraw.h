// -------------------------------------------------------------------------
// -----                      FairTPCRecoPointDraw header file                    -----
// -----          Created 03/01/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------


/** PndEmcClusterDraw
 * @author T. Stockmanns
 * @since 03.01.09
 *   Task to display TPC points
 **
 **/

#ifndef PndEmcClusterDraw_H
#define PndEmcClusterDraw_H


#include "FairBoxSetDraw.h"


class PndEmcClusterDraw : public FairBoxSetDraw {

 public:

  /** Default constructor **/
  PndEmcClusterDraw(){};


  /** Standard constructor
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  PndEmcClusterDraw(const char* name, Int_t iVerbose = 1):FairBoxSetDraw(name, iVerbose){};

  /** Destructor **/
  virtual ~PndEmcClusterDraw(){};

protected:
	virtual TVector3 GetVector(TObject* obj);
  ClassDef(PndEmcClusterDraw,3);

};


#endif
