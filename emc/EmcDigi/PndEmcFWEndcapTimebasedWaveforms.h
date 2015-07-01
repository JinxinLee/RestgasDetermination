//----------------------------------------------------------------------
// Author List:
//      Phil Strother                  	Original author
// 	Dima Melnichuk 			- adaption for PANDA
// 	Philippp Mahlberg		- integrtion in timebased simulation concept
//----------------------------------------------------------------------

//#pragma once
#ifndef PndEmcFWEndcapTimebasedWaveforms_H
#define PndEmcFWEndcapTimebasedWaveforms_H


#include "FairTask.h"
#include <string>
#include <map>

class PndEmcMapper;
class PndEmcDigiPar;
class PndEmcGeoPar;
class PndEmcWaveformBuffer;
class PndEmcAbsWaveformSimulator;
class PndEmcAbsWaveformModifier;
class PndEmcAbsPulseshape;

class TClonesArray;

#define USEBUFFER

class PndEmcFWEndcapTimebasedWaveforms : public FairTask
{

public:

  // Constructors

  PndEmcFWEndcapTimebasedWaveforms(Int_t verbose=0, Bool_t storewaves=kTRUE);

  // Destructor

  virtual ~PndEmcFWEndcapTimebasedWaveforms();

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  
  void SetStorageOfData(Bool_t val); // Method to specify whether waveforms are stored or not.

#ifdef USEBUFFER
  void StoreDataClass(Bool_t storeData = kTRUE) { fStoreDataClass = storeData; };
  void RunTimebased(Bool_t timebased = kTRUE) { fActivateBuffering = timebased; };
#endif
  
private:

  /** Input array of PndEmcHits **/
  TClonesArray* fHitArray;

  /** Output array of PndEmcWaveforms **/

#ifdef USEBUFFER
  PndEmcWaveformBuffer* fWaveformBuffer;
  Bool_t fStoreDataClass;
  Bool_t fActivateBuffering;
#else
  TClonesArray* fWaveformArray;  
#endif

  PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Verbosity level **/
  Int_t fVerbose;

  Bool_t fStoreWaves;


  PndEmcFWEndcapTimebasedWaveforms(const  PndEmcFWEndcapTimebasedWaveforms& L);
  PndEmcFWEndcapTimebasedWaveforms& operator= (const  PndEmcFWEndcapTimebasedWaveforms&) {return *this;}

  PndEmcAbsWaveformSimulator* fVPTT_LOW;
  PndEmcAbsWaveformSimulator* fVPTT_HIGH;
  PndEmcAbsWaveformSimulator* fVPTT_MULT;
  PndEmcAbsWaveformSimulator* fAPD_LOW;
  PndEmcAbsWaveformSimulator* fAPD_HIGH;

  PndEmcAbsWaveformModifier* fVPTT_LOW_Shaping;
  PndEmcAbsWaveformModifier* fVPTT_HIGH_Shaping;
  PndEmcAbsWaveformModifier* fAPD_LOW_Shaping;
  PndEmcAbsWaveformModifier* fAPD_HIGH_Shaping;

  PndEmcAbsWaveformModifier* fVPTT_LOW_Digitize;
  PndEmcAbsWaveformModifier* fVPTT_HIGH_Digitize;
  PndEmcAbsWaveformModifier* fAPD_LOW_Digitize;
  PndEmcAbsWaveformModifier* fAPD_HIGH_Digitize;

  PndEmcAbsPulseshape* fVPTT_LOW_Pulseshape;
  PndEmcAbsPulseshape* fVPTT_HIGH_Pulseshape;
  PndEmcAbsPulseshape* fAPD_LOW_Pulseshape;
  PndEmcAbsPulseshape* fAPD_HIGH_Pulseshape;


  ClassDef(PndEmcFWEndcapTimebasedWaveforms, 1);

};

#endif
