#ifndef PNDEMCGEOPAR_H
#define PNDEMCGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndEmcGeoPar : public FairParGenericSet 
{
public:
  PndEmcGeoPar(const char* name="PndEmcGeoPar",
	       const char* title="Emc Geometry Version Flag",
	       const char* context="TestDefaultContext");
  ~PndEmcGeoPar(void);
  void clear(void);
 
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
 
  Int_t  GetMapperVersion(){return fMapperVersion;}
  void   SetMapperVersion(Int_t mapperVersion){    //AB
     fMapperVersion = mapperVersion;
  }
  
  void InitEmcMapper();

 private:
  Int_t fMapperVersion; 
  
  ClassDef(PndEmcGeoPar,1)
};

#endif /* !PNDEMCGEOPAR_H */
