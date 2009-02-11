#ifndef PNDGEOSTTPAR_H
#define PNDGEOSTTPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndGeoSttPar : public FairParGenericSet 
{
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  PndGeoSttPar(const char* name="PndGeoSttPar",
	       const char* title="Stt Geometry Parameters",
	       const char* context="TestDefaultContext");
  ~PndGeoSttPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndGeoSttPar,1)
};

#endif /* !PNDGEOSTTPAR_H */
