#ifndef PNDEMCGEOPAR_H
#define PNDEMCGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndEmcGeoPar : public FairParGenericSet 
{
public:
  TObjArray            *fGeometryVersion;

  PndEmcGeoPar(const char* name="PndEmcGeoPar",
	       const char* title="Emc Geometry Version Flag",
	       const char* context="TestDefaultContext");
  ~PndEmcGeoPar(void);
  void clear(void);
 
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
 
   //TObjArray             *GetGeometryVersion(const Int_t geomVersion){return fGeometryVersion;}
   //TObjArray         *GetGeometryVersion(){return fGeometryVersion;}
  Int_t  GetMapperVersion(){return fMapperVersion;}
  void   SetMapperVersion(Int_t mapperVersion){    //AB
     fMapperVersion = mapperVersion;
  }

 private:
  Int_t fMapperVersion; 
  
  ClassDef(PndEmcGeoPar,1)
};

#endif /* !PNDEMCGEOPAR_H */
