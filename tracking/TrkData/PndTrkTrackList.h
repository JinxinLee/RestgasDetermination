/** PndTrkTrackList
 ** Class for pattern recognition TrackList
 ** @author Lia Lavezzi 
 **
 ** 
 **/

#ifndef PNDTRKTRACKLIST_H
#define PNDTRKTRACKLIST_H 1

#include "TVector3.h"
#include "PndTrkTrack.h"

class TObjArray;
class PndTrkTrackList : public TObject
{

 public:    

  PndTrkTrackList();
 // copy ctor
  PndTrkTrackList(const PndTrkTrackList& tlist);
  ~PndTrkTrackList();    
  PndTrkTrackList& operator=(const PndTrkTrackList &tlist);

  void AddTrack(PndTrkTrack *track) ;
  void DeleteTrack(Int_t index) ;
  void Reset();
  inline Int_t  GetNofTracks() { return tracklist.GetEntriesFast(); }

  PndTrkTrack * GetTrack(Int_t index) { return (PndTrkTrack*) tracklist[index]; }
 
  void ReplaceTrack(Int_t index, PndTrkTrack *track);
   
 protected:
 
  TObjArray tracklist;
  // std::vector< PndTrkTrack > tracklist;

  ClassDef(PndTrkTrackList,1);
};


#endif
