#ifndef PNDGEOMDTPAR_H
#define PNDGEOMDTPAR_H 1

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoMdtPar : public FairParGenericSet {
public:
  TObjArray* fSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray* fPassNodes; /** List of FairGeoNodes for passive  volumes */

  PndGeoMdtPar(const char* name="PndGeoMdtPar",
             const char* title="MDT Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoMdtPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray* GetSensitiveNodes(){return fSensNodes;}
  TObjArray* GetPassiveNodes(){return fPassNodes;}
  
  ClassDef(PndGeoMdtPar,1)
};

#endif /* !PNDGEOMDTPAR_H */
