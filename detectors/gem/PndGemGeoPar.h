// -------------------------------------------------------------------------
// -----                PndGemGeoPar header file                       -----
// -----                Created 15/02/2009  by R.Karabowicz            -----
// -------------------------------------------------------------------------

/** \class PndGemGeoPar
 *  \author R. Karabowicz
 *  \date 15/02/2009
 *  \Storing list of GEM volumes
 **/

#ifndef PNDGEMGEOPAR_H
#define PNDGEMGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"

class PndGemGeoPar : public FairParGenericSet {
public:
  PndGemGeoPar(const char* name="PndGemGeoPar",
	       const char* title="PndGem Geometry Parameters",
	       const char* context="TestDefaultContext");
  ~PndGemGeoPar(void);
  void clear(void);
  void putParams(FairParamList* l);
  Bool_t getParams(FairParamList* l);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}
private:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  ClassDef(PndGemGeoPar,2);
};

#endif /* !PNDGEMGEOPAR_H */
