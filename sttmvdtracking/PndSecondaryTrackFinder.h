#ifndef PNDSECONDARYTRACKFINDER_H
#define PNDSECONDARYTRACKFINDER_H 1


#include "PndGeoSttPar.h"
#include "PndTrack.h"
#include "PndMCTrack.h"

#include "FairTask.h"

#include "TH2F.h"
#include "TCanvas.h"
#include "TString.h"

class TClonesArray;
class TObjectArray;

class PndSecondaryTrackFinder : public FairTask {

 public:

  /** Default constructor **/
  PndSecondaryTrackFinder();

  /** First constructor **/
  PndSecondaryTrackFinder(Int_t verbose);

  /** Destructor **/
  ~PndSecondaryTrackFinder();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void WriteHistograms();

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }
 

  void SetParContainers();


  void SetInputBranchName(	char* string1,
			char* string2,
			char* string3
		    )
  {
	sprintf(fSttBranch,"%s", string1);
	sprintf(fMvdPixelBranch,"%s", string2);
	sprintf(fMvdStripBranch,"%s", string3);
	return;
  };


  std::vector<int> OrderHits(TClonesArray *hitarray);
  void DeleteHit(Int_t ihit, std::vector<int> *hits);
  void DeleteHits(TString detectors, std::vector<int> *hits);

 void SwitchOnDisplay() { fDisplayOn = kTRUE; }
 void GetInitialParams(PndTrack * track, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp);
 void GetInitialParamsMC(PndMCTrack * mctrack, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp);
 Double_t CalculatePhi(TVector2 v, TVector2 p, double alpha, double Phi0, int charge);
 Double_t CompareToPreviousPhi(Double_t Fi, Double_t Fi_pre, int charge);


void DrawFoundTracks();
 void DrawMCTracks();

 private:

  /** Input array of PndSttTube (map of STT tubes) **/
  TClonesArray* fMCTrackArray;

   /** Input array of PndSttPoints **/
  TClonesArray* fSttPointArray;
  /** Input array of PndSttHit **/
  TClonesArray* fSttHitArray;
  /** Input array of PndSttTracks **/
  TClonesArray* fSttTrackArray;
 /** Input array of PndTracksCand of Stt **/
  TClonesArray* fSttTrackCandArray;


 /** Input array of MvdPixelHitArray **/
  TClonesArray* fMvdPixelHitArray;
 /** Input array of MvdStripHitArray **/
  TClonesArray* fMvdStripHitArray;
 /** Input array of PndTracksCand of Mvd**/
  TClonesArray* fMvdTrackCandArray;

 /** Input array of MC points  of Mvd**/
  TClonesArray* fMvdMCPointArray;



  /** Output array of PndSttMvd  PndTrackCand **/
  TClonesArray* fSttMvdPndTrackCandArray;
  /** Output array of PndSttMvd   PndTrack **/
  TClonesArray* fSttMvdPndTrackArray;

  /** SttMvdGemTrackCandArray **/ 
 TClonesArray* fSttMvdGemTrackCandArray;
  /** SttMvdGemTrackArray **/ 
 TClonesArray* fSttMvdGemTrackArray;

 
  TClonesArray* fTubeArray;


  /** object persistence **/
  Bool_t  fPersistence; //!
  Bool_t fDisplayOn; //!
  PndGeoSttPar *fSttParameters;  //  CHECK added


  /**  Branch names to be used to fetch the hits of the backgound mixed events  **/

  char	fSttBranch[200],
		fMvdPixelBranch[200],
		fMvdStripBranch[200];

  TH2F *h2;
  TCanvas *display;

  ClassDef(PndSecondaryTrackFinder,1);

};

#endif
