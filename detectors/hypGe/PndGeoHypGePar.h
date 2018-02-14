// -------------------------------------------------------------------------
// -----                     PndGeoHypGePar header file                  -----

// -------------------------------------------------------------------------

#ifndef PNDGEOHYPGEPAR_H
#define PNDGEOHYPGEPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoHypGePar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  PndGeoHypGePar(const char* name="PndGeoHypGePar",
	     const char* title="HypGe Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoHypGePar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 /* Float_t barrel_y;
  Float_t dplane;
  Float_t barrel_z;
  Float_t dstrip;
  Float_t barrel_z0;
  Int_t nbox;
  Float_t dbox;
  Float_t dtube;
  Int_t ntube; */
  
  ClassDef(PndGeoHypGePar,2)
};

#endif /* !PNDGEOHYPGEPAR_H */
