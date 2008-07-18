// -------------------------------------------------------------------------
// -----                     CbmGeoHypPar header file                  -----
// -----               Created by A.Sanchez                            -----
// -------------------------------------------------------------------------

#ifndef PNDGEOHYPPAR_H
#define PNDGEOHYPPAR_H

#include "CbmParGenericSet.h"
#include "TObjArray.h"

class PndGeoHypPar : public CbmParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of CbmGeoNodes for passive  volumes */
  
  PndGeoHypPar(const char* name="PndGeoHypPar",
	     const char* title="Hyp Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoHypPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(PndGeoHypPar,1)
};

#endif /* !PNDGEOHYPPAR_H */
