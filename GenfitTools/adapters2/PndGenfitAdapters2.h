//modified by Elisabetta Prencipe, 19/05/2014

#ifndef PNDGENFITADAPTERS2_H
#define PNDGENFITADAPTERS2_H

class PndTrack;
class PndTrackCand;
namespace genfit {
  class Track;
  class AbsTrackRep;
  class TrackCand;

}

PndTrackCand* Genfit2TrackCand2PndTrackCand(const genfit::TrackCand*);
genfit::TrackCand* PndTrackCand2Genfit2TrackCand(PndTrackCand*);
PndTrack* Genfit2Track2PndTrack(const genfit::Track*);

#endif
