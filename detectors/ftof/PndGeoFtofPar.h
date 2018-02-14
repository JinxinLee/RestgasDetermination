// -------------------------------------------------------------------------
// -----                     PndGeoTofPar  header file                  -----
// -----               Created 30.03.09 by A. Sanchez            -----
// -------------------------------------------------------------------------

#ifndef PNDGEOFTOFPAR_H
#define PNDGEOFTOFPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoFtofPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  PndGeoFtofPar(const char* name="PndGeoFtofPar",
	     const char* title="Ftof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~PndGeoFtofPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(PndGeoFtofPar,1)
};

#endif /* !PNDGEOFTOFPAR_H */
