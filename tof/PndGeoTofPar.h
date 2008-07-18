// -------------------------------------------------------------------------
// -----                     CbmGeoHypPar header file                  -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEOTOFPAR_H
#define PNDGEOTOFPAR_H

#include "CbmParGenericSet.h"
#include "TObjArray.h"

class PndGeoTofPar : public CbmParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of CbmGeoNodes for passive  volumes */
  
  PndGeoTofPar(const char* name="PndGeoTofPar",
	     const char* title="Tof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoTofPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(PndGeoTofPar,1)
};

#endif /* !PNDGEOTOFPAR_H */
