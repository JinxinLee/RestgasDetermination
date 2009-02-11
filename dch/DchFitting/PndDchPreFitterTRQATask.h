// ---------------------------------------------------------------------------
// -----              PndDchPreFitterTRQATask2 header file               -----
// -----         Created 03.11.2008  by A. Wronska, P.Hawranek           -----
// ---------------------------------------------------------------------------


/** \class PndDchPreFitterTRQATask
 *  \author A.Wronska, P.Hawranek <aleksandra.wronska@uj.edu.pl>
 *  \date 03.11.2008
 *  \brief PndDchPreFitterTRQATask allows to judge performance of analytic PreFitter
 *         for forward tracks in dch
 **/

#ifndef PNDDCHPREFITTERTRQATASK_HH
#define PNDDCHPREFITTERTRQATASK_HH

// Panda Headers ----------------------
#include "FairTask.h"

// ROOT Headers -----------------------
#include "TString.h"

class TClonesArray;
class TH1D;
class TH2D;
class TCanvas;

class PndDchPreFitterTRQATask : public FairTask {
public:

  /**  Constructor  **/
  PndDchPreFitterTRQATask();

  /**  Destructor **/
  ~PndDchPreFitterTRQATask();

  /** public method Init() **/
  virtual InitStatus Init();

  /** public method Exec(Option_t* opt) **/
  virtual void Exec(Option_t* opt);
  

  /** public method PlotHistograms()
      allows a fast glance at the results of fitting **/
  void PlotHistograms();

  /** public method Finish(Option_t* opt) **/
  virtual void Finish();
  

private:

  TClonesArray* fTrackArray;        ///< pointer to array of Track objects
  TClonesArray* fMCTrackArray;      ///< pointer to array of MCTracks
  TClonesArray* fDchTrackMatchArray;///< pointer to array of DchTrackMatches
  
  TH1D* fhP;             ///< momentum histogram;
  TH2D* fThetaH;         ///< theta histogram;
  TH2D* fPhiH;           ///< phi histogram;
  TH1D* fhChi2;          ///< chi2 histogram;
  TCanvas* fCanvas;      ///< pointer to a canvas for drawing fit results
  
  /** private method WriteHistograms()
      writes out histograms with results of fitting into a file **/
  Bool_t WriteHistograms();

public:
  ClassDef(PndDchPreFitterTRQATask,1)

};

#endif
