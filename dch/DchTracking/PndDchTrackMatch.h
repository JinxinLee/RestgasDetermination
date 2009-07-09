// -------------------------------------------------------------------------
// -----                   PndDchTrackMatch header file                -----
// -----                    based on CbmTrdTrackMatch                  -----
// -------------------------------------------------------------------------

/** \class PndDchTrackMatch
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 14.03.2008
 *  \brief Data structure describing the matching of a reconstructed PndDchTrack
 * 	 with a Monte Carlo PndMCTrack.
 **/

#ifndef PNDDCHTRACKMATCH_H
#define PNDDCHTRACKMATCH_H 1


// Root includes
#include "TObject.h"
#include<iostream>

class PndDchTrackMatch : public TObject
{

 public:

  /** Default constructor **/
  PndDchTrackMatch();


  /** Standard constructor
  *  \param mcTrackID     Index of matched MCTrack
  *  \param recTrackID    Index of best matching PndDchTrack
  *  \param nTrue         Good hits/digis from matched MCTrack
  *  \param nWrong        Good hits/digis from other MCTracks
  *  \param nFake         Fake hits/digis
  *  \param nTracks       Number of MCTracks with common points
  **/
  PndDchTrackMatch(Int_t mcTrackID, Int_t recTrackID, Int_t nTrue, Int_t nWrong,
		   Int_t nFake, Int_t nTracks);


  /** Destructor **/
  virtual ~PndDchTrackMatch();


  /** Accessors **/
  Int_t GetMCTrackID()     const { return fMCTrackID;     };
  Int_t GetRecTrackID()    const { return fRecTrackID;     };
  Int_t GetNofTrueHorDs()  const { return fNofTrueHorDs;  };
  Int_t GetNofWrongHorDs() const { return fNofWrongHorDs; };
  Int_t GetNofFakeHorDs()  const { return fNofFakeHorDs;  };
  Int_t GetNofMCTracks()   const { return fNofMCTracks;   };

  
  void Print(){
    std::cout << "%%%%%%%%%%%% TrackMatch::Print() %%%%%%%%%%" << std::endl;
    std::cout << " fMCTrackID " << fMCTrackID  
	      << " fRecTrackID " << fRecTrackID
	      << " fNofTrueHorDs " << fNofTrueHorDs
	      << " fNofWrongHorDs " << fNofWrongHorDs
	      << " fNofFakeHorDs " << fNofFakeHorDs
	      << " fNofMCTracks " << fNofMCTracks << std::endl;

  }

 private:

  /** Best matching PndMCTrack  **/
  Int_t fMCTrackID;

  /** Number of good hits/digis belonging to the matched MCTrack **/
  Int_t fNofTrueHorDs;

  /** Number of good hits/digis belonging to other MCTracks **/
  Int_t fNofWrongHorDs;

  /** Number of fake hits/digis **/
  Int_t fNofFakeHorDs;

  /** Number of MCTrackx with common points **/
  Int_t fNofMCTracks;

  /** Best matching PndDchTrack **/
  Int_t fRecTrackID;


  ClassDef(PndDchTrackMatch,1);

};


#endif

