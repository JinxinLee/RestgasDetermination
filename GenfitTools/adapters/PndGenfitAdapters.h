//modified by Elisabetta Prencipe, 19/05/2014

#ifndef PNDGENFITADAPTERS_H
#define PNDGENFITADAPTERS_H

class PndTrack;
class PndTrackCand;
class Track;
class AbsTrackRep;
class TrackCand;

PndTrackCand* GenfitTrackCand2PndTrackCand(const TrackCand*);
TrackCand* PndTrackCand2GenfitTrackCand(PndTrackCand*);
PndTrack* GenfitTrack2PndTrack(const Track*);

#endif
