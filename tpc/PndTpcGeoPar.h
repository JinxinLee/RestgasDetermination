#ifndef TPCGEOPAR_H
#define TPCGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndTpcGeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  PndTpcGeoPar(const char* name="PndTpcGeoPar",
	    const char* title="PndTpc Geometry Parameters",
	    const char* context="PndTpcDefaultContext");
  ~PndTpcGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndTpcGeoPar,1)
};

#endif
