// -------------------------------------------------------------------------
// -----                     CbmGeoHypPar header file                  -----
// -----               Created by A.Sanchez                            -----
// -------------------------------------------------------------------------

#ifndef PNDGEOHYPPAR_H
#define PNDGEOHYPPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoHypPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  PndGeoHypPar(const char* name="PndGeoHypPar",
	     const char* title="Hyp Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoHypPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(PndGeoHypPar,1)
};

#endif /* !PNDGEOHYPPAR_H */
