// -------------------------------------------------------------------------
// -----                     PndGeoEmcPar header file                  -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEOEMCPAR_H
#define PNDGEOEMCPAR_H

#include "CbmParGenericSet.h"
#include "TObjArray.h"

class PndGeoEmcPar : public CbmParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of CbmGeoNodes for passive  volumes */
  
  PndGeoEmcPar(const char* name="PndGeoEmcPar",
	     const char* title="Emc Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoEmcPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}
  
  ClassDef(PndGeoEmcPar,1)
};

#endif /* !PNDGEOEMCPAR_H */
