// -------------------------------------------------------------------------
// -----                 PndBranchTask header file                    -----
// -----           Created 28/02/18  by T. StockmannsD.		         -----
// -------------------------------------------------------------------------


/** PndBranchTask.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Version 28/02/18 by T.Stockmanns
 **
 ** This class handles the persistency parameter used to define if an output branch is stored in a root file
 ** or not.
 **/


#ifndef PndBranchTask_H
#define PndBranchTask_H

#include "FairTask.h"

class PndBranchTask : public FairTask
{

 public:

  PndBranchTask();
  PndBranchTask(const char* name, Int_t iVerbose = 1);
  virtual ~PndBranchTask();

  void SetPersistency(Bool_t val=kTRUE){ fPersistency = val; }
  Bool_t GetPersistency(){ return fPersistency; }

 private:
  Bool_t fPersistency;                    //!   ///< Persistency flag


  ClassDef(PndBranchTask,1)


};






#endif
