#ifndef CBMGEOPlanePAR_H
#define CBMGEOPlanePAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class CbmGeoPlanePar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

  CbmGeoPlanePar(const char* name="CbmGeoPlanePar",
             const char* title="Plane Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoPlanePar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoPlanePar,1)
};

#endif /* !CBMGEOPlanePAR_H */
