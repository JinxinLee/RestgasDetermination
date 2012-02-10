/*
 * PndLumiGeoPar.h
 *
 *  Created on: Feb 21, 2009
 *      Author: tsito
 */

#ifndef PNDLUMIGEOPAR_H_
#define PNDLUMIGEOPAR_H_

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndLumiGeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  PndLumiGeoPar(const char* name="PndLumiGeoPar",
	    const char* title="PndLumi Geometry Parameters",
	    const char* context="PndLumiDefaultContext");
  ~PndLumiGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndLumiGeoPar,1);
};



#endif /* PNDLUMIGEOPAR_H_ */
