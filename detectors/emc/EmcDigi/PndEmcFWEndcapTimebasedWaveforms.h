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

class PndEmcMapper;
class PndEmcFWEndcapDigiPar;
class PndEmcGeoPar;
class PndEmcWaveformBuffer;
class PndEmcAbsWaveformSimulator;
class PndEmcMultiWaveformSimulator;
class PndEmcAbsWaveformModifier;
class PndEmcAbsPulseshape;
class TClonesArray;

#define MULTI

/**
 * @brief Taks to create waveforms from hits.
 * 
 * This is an alternative to PndEmcHitsToWaveform, which despite its name could be 
 * used for all the EMC, not just FwEndcap.
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcFWEndcapTimebasedWaveforms : public FairTask
{
public:
  // Constructors
  PndEmcFWEndcapTimebasedWaveforms(Int_t verbose=0, Bool_t storewaves=kFALSE);
  // Destructor
  virtual ~PndEmcFWEndcapTimebasedWaveforms();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

  void RunTimebased(Bool_t timebased = kTRUE) { fActivateBuffering = timebased; };
  void SetExternalSimulator(PndEmcAbsWaveformSimulator* simulator) { fExternalSimulator = simulator; }
  void SetStorageOfData(Bool_t storeWaves = kTRUE) {fStoreWaves = storeWaves;} ; //!< Method to specify whether waveforms are stored or not.
  void StoreDataClass(Bool_t storeData = kTRUE) { fStoreDataClass = storeData; };

protected:  
  /** Get parameter containers **/
  virtual void SetParContainers();

private:
   // don't allow copying (-Weffc++)
   PndEmcFWEndcapTimebasedWaveforms(const PndEmcFWEndcapTimebasedWaveforms&);	// no implementation
   PndEmcFWEndcapTimebasedWaveforms& operator= (const PndEmcFWEndcapTimebasedWaveforms&);	// no implementation

private:

  /** Input array of PndEmcHits **/
  TClonesArray* fHitArray;

  /** Output array of PndEmcWaveforms **/
  PndEmcWaveformBuffer* fWaveformBuffer;

  Bool_t fStoreWaves;
  Bool_t fStoreDataClass;
  Bool_t fActivateBuffering;

  PndEmcFWEndcapDigiPar*    fDigiPar; 	//!< Digitisation parameter container
  PndEmcGeoPar*     fGeoPar;       	//!< Geometry parameter container

  Bool_t fUse_photon_statistic; 
  Double_t fNPhotoElectronsPerMeV;
  Double_t fExcessNoiseFactor;

  PndEmcAbsWaveformSimulator* fExternalSimulator;
  PndEmcMultiWaveformSimulator* fAPD_LOWHIGH;

  ClassDef(PndEmcFWEndcapTimebasedWaveforms, 2);
};

#endif
