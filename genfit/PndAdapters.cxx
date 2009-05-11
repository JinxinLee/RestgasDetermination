#include"PndAdapters.h"

#include"Track.h"
#include"TrackCand.h"
#include"PndTrack.h"
#include"PndTrackCand.h"

PndTrackCand* GenfitTrackCand2PndTrackCand(const TrackCand& cand){
  PndTrackCand* retVal = new PndTrackCand();
  unsigned int nhits = cand.getNHits();
  unsigned detId,hitId;
  for(unsigned int i=0;i<nhits;++i){
    cand.getHit(i,detId,hitId);
    retVal->AddHit(detId,hitId,0.);
  }
  return retVal;
}

PndTrack* GenfitTrack2PndTrack(Track& tr){
  


}
