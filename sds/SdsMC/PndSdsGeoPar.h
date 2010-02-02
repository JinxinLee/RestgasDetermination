#ifndef PNDSDSGEOPAR_H
#define PNDSDSGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"

class PndSdsGeoPar : public FairParGenericSet {
public:
  PndSdsGeoPar(const char* name="PndSdsGeoPar",
	       const char* title="PndSds Geometry Parameters",
	       const char* context="TestDefaultContext");
  ~PndSdsGeoPar(void);
  void clear(void);
  void putParams(FairParamList* l);
  Bool_t getParams(FairParamList* l);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}
private:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  ClassDef(PndSdsGeoPar,2);
};

#endif /* !PNDSDSGEOPAR_H */
