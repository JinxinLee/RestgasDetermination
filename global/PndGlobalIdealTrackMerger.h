// -------------------------------------------------------------------------
// -----                   PndGlobalIdealTrackMerger header file       -----
// -----                  Created 23.12.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndGlobalIdealTrackMerger
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 23.12.2009
 *  \brief ideal merging of tracks from different detectors
 * 
 **/


#ifndef PNDGLOBALIDEALTRACKMERGER_H
#define PNDGLOBALIDEALTRACKMERGER_H

#include "TH1F.h"
#include "TH2F.h"

#include "FairMCPoint.h"
#include "FairTask.h"
#include "PndMCTrack.h"

#include "PndTrackCand.h"

#include <vector>

class PndGlobalIdealTrackMerger : public FairTask
{

 public:

  /** Default constructor **/
  PndGlobalIdealTrackMerger();

  /** Default constructor **/
  PndGlobalIdealTrackMerger(Int_t iVerbose);
  
  /** Destructor **/
  virtual ~PndGlobalIdealTrackMerger();
  
  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Public modifiers **/
  void SetVerbose(const Int_t& verbose) { fVerbose = verbose; };

  void UseMvdSttGemFts(const Bool_t useMvd, const Bool_t useStt, const Bool_t useGem, const Bool_t useFts) {
    fIncludeDet[0] = useMvd;
    fIncludeDet[1] = useStt;
    fIncludeDet[2] = useGem;
    fIncludeDet[3] = useFts;
  };

 private:

  /** Arrays of MC information **/
  TClonesArray*       fMCTrackArray;
  std::vector<Bool_t> fMCTrackSeen;

  /** Tracks in different detectors: MVD, STT, GEM, FTS **/
  Bool_t                           fIncludeDet[4];
  TClonesArray*                    fTrackArray[4];
  std::vector<std::vector<Int_t> > fMCDetTracks;
  TString fDetName[4];

  /** Output array of global tracks **/
  TClonesArray* fGlobalTrackArray;     	    ///< Output array of PndTrackCands 
  TClonesArray* fGlobalTrackCandArray;

  /** Event counter **/
  Int_t fTNofEvents;  ///< event counter
  Int_t fTNofTracks;

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Finish **/
  virtual void Finish();

  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  ClassDef(PndGlobalIdealTrackMerger,1);

};

#endif
