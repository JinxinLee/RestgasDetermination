#ifndef CBMGEOPlanePAR_H
#define CBMGEOPlanePAR_H

#include "CbmParGenericSet.h"
#include "TObjArray.h"

class CbmGeoPlanePar : public CbmParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of CbmGeoNodes for passive  volumes */

  CbmGeoPlanePar(const char* name="CbmGeoPlanePar",
             const char* title="Plane Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoPlanePar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoPlanePar,1)
};

#endif /* !CBMGEOPlanePAR_H */
