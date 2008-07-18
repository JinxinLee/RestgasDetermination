#ifndef PNDGEOMDTPAR_H
#define PNDGEOMDTPAR_H 1

#include "CbmParGenericSet.h"
#include "TObjArray.h"

class PndGeoMdtPar : public CbmParGenericSet {
public:
  TObjArray* fSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray* fPassNodes; /** List of CbmGeoNodes for passive  volumes */

  PndGeoMdtPar(const char* name="PndGeoMdtPar",
             const char* title="MDT Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoMdtPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray* GetSensitiveNodes(){return fSensNodes;}
  TObjArray* GetPassiveNodes(){return fPassNodes;}
  
  ClassDef(PndGeoMdtPar,1)
};

#endif /* !PNDGEOMDTPAR_H */
