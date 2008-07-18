#ifndef PNDRPCGEOPAR_H
#define PNDRPCGEOPAR_H

#include "CbmParGenericSet.h"
#include "TObjArray.h"

class PndRpcGeoPar : public CbmParGenericSet {
 public:
  TObjArray        *fGeoSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of CbmGeoNodes for passive  volumes */

  PndRpcGeoPar(const char* name="PndRpcGeoPar",
             const char* title="RpcTof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndRpcGeoPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndRpcGeoPar,1)
};

#endif /* !PNDRPCGEOPAR_H */
