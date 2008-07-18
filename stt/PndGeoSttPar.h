#ifndef PNDGEOSTTPAR_H
#define PNDGEOSTTPAR_H

#include "CbmParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndGeoSttPar : public CbmParGenericSet 
{
public:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for sensitive volumes

  PndGeoSttPar(const char* name="PndGeoSttPar",
	       const char* title="Stt Geometry Parameters",
	       const char* context="TestDefaultContext");
  ~PndGeoSttPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndGeoSttPar,1)
};

#endif /* !PNDGEOSTTPAR_H */
