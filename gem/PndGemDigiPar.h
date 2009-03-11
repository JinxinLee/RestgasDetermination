//
// C++ Interface: PndGemDigiPar
//
#ifndef PNDGEMDIGIPAR_H
#define PNDGEMDIGIPAR_H

#include <TVector2.h>
#include <TArrayD.h>
#include <TMath.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "PndGemStation.h"
#include "PndGemSensor.h"

//! Digitization Parameter Class for GEM part
class PndGemDigiPar : public FairParGenericSet
{
  public :
    PndGemDigiPar (const char* name="PndGemParTest",
		   const char* title="PndGem digi parameter",
		   const char* context="TestDefaultContext");
    ~PndGemDigiPar(void);
    void clear(void){};
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    
    void Print();

    /** Accessors **/
    Int_t GetNStations() { return fStations->GetEntries(); }
    Int_t GetNSensors()  { return fNSensors; }
    Int_t GetNChannels() { return fNChannels; }
    TObjArray*     GetStations() { return fStations; }
    PndGemStation* GetStation(Int_t iStation);                                       /** Station by index **/
    PndGemStation* GetStationByNr(Int_t stationNr);                                  /** Station by number **/
    PndGemSensor*  GetSensor     (Int_t stationNr, Int_t sensorNr);                  /** Sensor  by number **/
    Int_t GetDetectorIdByName(TString sensorName);
    PndGemSensor* GetSensorByName    (TString sensorName);
    
  private:
    void CreateStations();

    TArrayD  fGemParameters;

    TObjArray* fStations;       /** Array of FairStsStation **/
    Int_t fNSensors;            /** Total number of sensors **/
    Int_t fNChannels;           /** Total number of channels **/
    
    std::map<Int_t,   PndGemStation*> fStationMap; //! Map from number to station
    std::map<TString, Int_t>          fDetIdByName;
    std::map<TString, PndGemSensor*>  fSensorByName;
    
    ClassDef(PndGemDigiPar,2);
};

#endif /*!GEMSTRIPDIGIPAR_H*/
