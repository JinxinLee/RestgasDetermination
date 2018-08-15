// -------------------------------------------------------------------------
// -----                   PndGeoDrcPar header file                    -----
// -----               Created 11/10/06  by A. Cecchi                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEODRCPAR_H
#define PNDGEODRCPAR_H


#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoDrcPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

  PndGeoDrcPar      (const char* name="PndGeoDrcPar",
             const char* title="Dirc Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoDrcPar      (void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndGeoDrcPar,1)
};

#endif /* !PndGeoDrcPar_H */


