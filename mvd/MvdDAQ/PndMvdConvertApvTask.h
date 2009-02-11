// -------------------------------------------------------------------------
// -----                 PndMvdConvertApvTask header file              -----
// -----                  Created 12/01/09  by L.Ackermann             -----
// -------------------------------------------------------------------------


/** PndMvdConvertApvTask.h
 *@author L.Ackermann <lars.ackermann@physik.tu-dresden.de>
 **
 ** Converting Task from ascci hit file to clones array of PndMvdDigiStrip
 **/


#ifndef PNDMVDAPVCONVERTTASK_H
#define PNDMVDAPVCONVERTTASK_H


// framework includes
#include "FairTask.h"
#include "PndMvdConvertApv.h"
// #include "PndMvdGeoHandling.h"

#include <vector>
#include <map>

class TClonesArray;
class PndMvdDigiStrip;

class PndMvdConvertApvTask : public FairTask
{
 public:

  /** Default constructor **/  
  PndMvdConvertApvTask(PndMvdConvertApv* Apvconvert);

  /** Destructor **/
  ~PndMvdConvertApvTask();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  virtual InitStatus Init();
  virtual void Finish();
  
 private:
		PndMvdConvertApv* fApvConvert;
		TClonesArray* fStripArray;
		Int_t iStrip;
  void Register();
  
  void Reset();

  ClassDef(PndMvdConvertApvTask,1);

};

#endif
