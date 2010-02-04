// -------------------------------------------------------------------------
// -----                         PndLmdDetector header file                    -----
// -----                  Created 6/04/06  by T. Stockmanns            -----
// -----                          Based on PndMvdDetector                      -----
// -------------------------------------------------------------------------

/**  PndLmdDetector.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 *@author M.Michel <michel@kph.uni-mainz.de>
 **
 ** Defines the active detector PndLmdDetector. Constructs the geometry and
 ** registers MCPoints.
 **/



#ifndef PNDLMDDETECTOR_H
#define PNDLMDDETECTOR_H

#include "PndSdsDetector.h"

class PndLmdDetector : public PndSdsDetector
{

 public:

  /** Default constructor **/
  PndLmdDetector();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndLmdDetector(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~PndLmdDetector();

  virtual void SetBranchNames(TString outBranchname, TString folderName);
  virtual void SetBranchNames();

  ClassDef(PndLmdDetector,6);

};

#endif

