// -------------------------------------------------------------------------
// -----                     CbmGeoHypPar header file                  -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEOTOFPAR_H
#define PNDGEOTOFPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoTofPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  PndGeoTofPar(const char* name="PndGeoTofPar",
	     const char* title="Tof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoTofPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(PndGeoTofPar,1)
};

#endif /* !PNDGEOTOFPAR_H */
