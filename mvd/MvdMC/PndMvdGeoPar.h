#ifndef PNDMVDGEOPAR_H
#define PNDMVDGEOPAR_H

#include "CbmParGenericSet.h"
#include "TH1F.h"

class PndMvdGeoPar : public CbmParGenericSet {
public:
  PndMvdGeoPar(const char* name="PndMvdGeoPar",
	       const char* title="PndMvd Geometry Parameters",
	       const char* context="TestDefaultContext");
  ~PndMvdGeoPar(void);
  void clear(void);
  void putParams(CbmParamList* l);
  Bool_t getParams(CbmParamList* l);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}
private:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for sensitive volumes

  ClassDef(PndMvdGeoPar,2);
};

#endif /* !PNDMVDGEOPAR_H */
