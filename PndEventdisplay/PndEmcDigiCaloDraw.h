// -------------------------------------------------------------------------
// -----                      FairTPCRecoPointDraw header file                    -----
// -----          Created 03/01/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------


/** PndEmcDigiCaloDraw
 * @author T. Stockmanns
 * @since 03.01.09
 *   Task to display TPC points
 **
 **/

#ifndef PndEmcDigiCaloDraw_H
#define PndEmcDigiCaloDraw_H


#include "PndCaloDraw.h"


class PndEmcDigiCaloDraw : public PndCaloDraw {

 public:

  /** Default constructor **/
  PndEmcDigiCaloDraw(){
  };


  /** Standard constructor
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  PndEmcDigiCaloDraw(const char* name, Int_t iVerbose = 1):PndCaloDraw(name, iVerbose){
  };

  /** Destructor **/
  virtual ~PndEmcDigiCaloDraw(){};

protected:
	virtual TVector3 GetVector(TObject* obj);
	virtual Double_t GetEnergy(TObject* obj);
  ClassDef(PndEmcDigiCaloDraw,1);

};


#endif
