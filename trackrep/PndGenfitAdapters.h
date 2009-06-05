#ifndef PNDGENFITADAPTERS_H
#define PNDGENFITADAPTERS_H

class PndTrack;
class PndTrackCand;
class Track;
class AbsTrackRep;
class TrackCand;

PndTrackCand* GenfitTrackCand2PndTrackCand(const TrackCand*);
PndTrack* GenfitTrack2PndTrack(const Track*);

#endif
