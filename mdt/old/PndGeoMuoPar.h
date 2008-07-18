#ifndef PNDGEOMUOPAR_H
#define PNDGEOMUOPAR_H

#include "CbmParGenericSet.h"
#include "TObjArray.h"

class PndGeoMuoPar : public CbmParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of CbmGeoNodes for passive  volumes */

  PndGeoMuoPar(const char* name="PndGeoMuoPar",
             const char* title="Muo Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoMuoPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
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
