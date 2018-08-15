// -------------------------------------------------------------------------
// -----                         PndMvdDetector header file                    -----
// -----                  Created 6/04/06  by T. Stockmanns            -----
// -----                          Based on PndMvdDetector                      -----
// -------------------------------------------------------------------------

/**  PndMvdDetector.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Defines the active detector PndMvdDetector. Constructs the geometry and
 ** registers MCPoints.
 **/

#ifndef PNDMVDDETECTOR_H
#define PNDMVDDETECTOR_H

#include "PndSdsDetector.h"


class PndMvdDetector : public PndSdsDetector
{

 public:

  /** Default constructor **/
  PndMvdDetector();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndMvdDetector(const char* name, Bool_t active);

  /** Destructor **/
  virtual ~PndMvdDetector();

  /** Setting in/output names for MVD **/
  void SetBranchNames(TString outBranchname, TString folderName);
  virtual void SetBranchNames();
  virtual void SetSpecialPhysicsCuts();
  virtual void SetDefaultSensorNames();
  virtual void Initialize(){PndSdsDetector::Initialize();};
 
private:
  

  ClassDef(PndMvdDetector,6);

};

#endif

