//
// C++ Interface: PndSensorNamePar
//
#ifndef PNDSENSORNAMEPAR_H
#define PNDSENSORNAMEPAR_H


#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "FairRun.h"

#include "TObjArray.h"
#include "TObjString.h"

#include <iostream>

//! Unique match between SensorID and path in TGeoManager
class PndSensorNamePar : public FairParGenericSet
{
  public :
    PndSensorNamePar (const char* name="PndSensorNamePar",
          const char* title="Match between GeoManager path and SensorId",
          const char* context="TestDefaultContext");
    ~PndSensorNamePar(void){
    	if(fSensorNames) delete fSensorNames;
    };
    void clear(void){};
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    Int_t AddSensorName(TObjString* name);

    Int_t SensorInList(TObjString* name){
    	for (int i = 0; i < fSensorNames->GetEntries();i++){
    		TObjString* SensName = (TObjString*)fSensorNames->At(i);
    		if (SensName->GetString() == name->GetString())
    			return i;
    	}
    	return -1;
    }

    TString* GetSensorName(Int_t index){
    	return (TString*)fSensorNames->At(index);
    }

    TObjArray* GetSensorNames(){return fSensorNames;}


  private:
    TObjArray* fSensorNames;


  ClassDef(PndSensorNamePar,1);
};

#endif /*!MVDSTRIPDIGIPAR_H*/
