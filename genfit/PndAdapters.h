#ifndef ADAPTERS_H
#define ADAPTERS_H

class PndTrack;
class PndTrackCand;
class Track;
class TrackCand;

PndTrackCand* GenfitTrackCand2PndTrackCand(const TrackCand&);
PndTrack* GenfitTrack2PndTrack(Track&);

#endif
