// -------------------------------------------------------------------------
// -----                   PndGeoDrcPar header file                    -----
// -----               Created 11/10/06  by A. Cecchi                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEODRCPAR_H
#define PNDGEODRCPAR_H


#include "CbmParGenericSet.h"
#include "TObjArray.h"

class PndGeoDrcPar : public CbmParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of CbmGeoNodes for passive  volumes */

  PndGeoDrcPar      (const char* name="PndGeoDrcPar",
             const char* title="Dirc Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoDrcPar      (void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndGeoDrcPar,1)
};

#endif /* !PndGeoDrcPar_H */


