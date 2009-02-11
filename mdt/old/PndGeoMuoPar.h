#ifndef PNDGEOMUOPAR_H
#define PNDGEOMUOPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoMuoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

  PndGeoMuoPar(const char* name="PndGeoMuoPar",
             const char* title="Muo Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoMuoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  Float_t barrel_y;
  Float_t dplane;
  Float_t barrel_z;
  Float_t dstrip;
  Float_t barrel_z0;
  Int_t nbox;
  Float_t dbox;
  Float_t dtube;
  Int_t ntube;
  
  ClassDef(PndGeoMuoPar,1)
};

#endif /* !PNDGEOMUOPAR_H */
