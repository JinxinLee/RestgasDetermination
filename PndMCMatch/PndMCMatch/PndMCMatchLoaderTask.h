// -------------------------------------------------------------------------
// -----                 PNDMCMATCHCREATORTASK header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------


/** PNDMCMATCHCREATORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PNDMCMATCHLOADERTASK_H
#define PNDMCMATCHLOADERTASK_H


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCMatchLoaderTask : public FairTask
{
 public:

  /** Default constructor **/
	PndMCMatchLoaderTask();

  /** Destructor **/
  virtual ~PndMCMatchLoaderTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:

		TClonesArray* fMCLink;

		int fEventNr;
		PndMCMatch* fMCMatch;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(PndMCMatchLoaderTask,1);

};

#endif
