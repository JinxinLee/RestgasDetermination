#ifndef PNDRPCGEOPAR_H
#define PNDRPCGEOPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndRpcGeoPar : public FairParGenericSet {
 public:
  TObjArray        *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

  PndRpcGeoPar(const char* name="PndRpcGeoPar",
             const char* title="RpcTof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndRpcGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndRpcGeoPar,1)
};

#endif /* !PNDRPCGEOPAR_H */
