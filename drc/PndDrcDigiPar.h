
#ifndef PNDDRCDIGIPAR_H
#define PNDDRCDIGIPAR_H

#include <TVector2.h>
#include <TArrayD.h>
#include <TMath.h>


#include "FairParGenericSet.h"
#include "FairParamList.h"
//#include "PndGemStation.h"
//#include "PndGemSensor.h"

//! Digitization Parameter Class for GEM part
class PndDrcDigiPar : public FairParGenericSet
{
  public :
    PndDrcDigiPar (const char* name   ="PndDrcParTest",
		   const char* title  ="PndDrc digi parameter",
		   const char* context="TestDefaultContext");
    ~PndDrcDigiPar(void);
    void   clear(void){};
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    
    void Print();


  private:
    
    ClassDef(PndDrcDigiPar,2);
};

#endif /*!GEMSTRIPDIGIPAR_H*/
