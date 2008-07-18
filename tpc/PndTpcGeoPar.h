#ifndef TPCGEOPAR_H
#define TPCGEOPAR_H

#include "CbmParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndTpcGeoPar : public CbmParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for sensitive volumes

  PndTpcGeoPar(const char* name="PndTpcGeoPar",
	    const char* title="PndTpc Geometry Parameters",
	    const char* context="PndTpcDefaultContext");
  ~PndTpcGeoPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndTpcGeoPar,1)
};

#endif
