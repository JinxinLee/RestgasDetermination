#ifndef PNDSECONDARYTRACKFINDER_H
#define PNDSECONDARYTRACKFINDER_H 1


#include "PndMCTrack.h"
#include "PndSttTrack.h"
#include "FairTask.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParP.h"
#include "PndGeoSttPar.h"

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

class TClonesArray;
class TObjectArray;

class PndSecondaryTrackFinder : public FairTask
{

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


  void OrderHits(TClonesArray *hitarray, Int_t *sorthits);


 private:

  /** Input array of PndSttTube (map of STT tubes) **/
  TClonesArray* fMCTrackArray;
  TClonesArray* fSttTubeArray;

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



  /** object persistence **/
  Bool_t  fPersistence; //!

  PndGeoSttPar *fSttParameters;  //  CHECK added


  /**  Branch names to be used to fetch the hits of the backgound mixed events  **/

  char	fSttBranch[200],
		fMvdPixelBranch[200],
		fMvdStripBranch[200];



  ClassDef(PndSecondaryTrackFinder,1);

};

#endif
