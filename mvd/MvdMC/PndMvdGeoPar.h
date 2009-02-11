#ifndef PNDMVDGEOPAR_H
#define PNDMVDGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"

class PndMvdGeoPar : public FairParGenericSet {
public:
  PndMvdGeoPar(const char* name="PndMvdGeoPar",
	       const char* title="PndMvd Geometry Parameters",
	       const char* context="TestDefaultContext");
  ~PndMvdGeoPar(void);
  void clear(void);
  void putParams(FairParamList* l);
  Bool_t getParams(FairParamList* l);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}
private:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  ClassDef(PndMvdGeoPar,2);
};

#endif /* !PNDMVDGEOPAR_H */
