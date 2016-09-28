// -------------------------------------------------------------------------
// -----                      PndMQStraightLineTrackFinder                    -----
// -----                  Created 22/10/09  by M. Michel               -----
// -------------------------------------------------------------------------


#ifndef PndMQStraightLineTrackFinder_H
#define PndMQStraightLineTrackFinder_H

#include "FairTask.h"
#include "PndMCTrack.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsClusterStrip.h"
#include "TrackData/PndTrackCand.h"
#include "TrackData/PndSimpleTrack.h"
#include "FairTrackPar.h"
#include "TObject.h"
#include "Rtypes.h"

#ifndef __CINT__
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#endif //__CINT__

#include <string>
#include <vector>
#include <utility>

struct StraightLineParams{
	TVector3 origin;
	TVector3 direction;
	double chi2;
};

class PndMQStraightLineTrackFinder
{
 public:

    /** Default constructor **/
    PndMQStraightLineTrackFinder();
    /** Destructor **/
    virtual ~PndMQStraightLineTrackFinder();

    void SetSearchRadius(Double_t accu) { fdXY = accu; };
    std::vector<PndSimpleTrack> FindTracks(std::vector<PndSdsHit> hits, int eventNr);

 protected:
    void SortHitsToLayers(std::vector<PndSdsHit>& hits);
    Double_t GetTrackDip(PndMCTrack* myTrack);
    Double_t GetTrackCurvature(PndMCTrack* myTrack);
    void SortHitsToLayers();
    int NLayersFilled();
    void ClearLayerInfo();
    std::vector< std::vector<int> > GetStartCombinations(int firstLayer, int secondLayer);
    PndSimpleTrack GenerateTrackParams(std::vector<int>& hitsInTrack);
    StraightLineParams FitTrack(std::vector<int> hitsInTrack);
    PndSimpleTrack FindTrack(std::vector<int>& startCombi, int lastStartPoint);
    StraightLineParams GetLineParameters(std::vector<int> startCombi);
    TVector3 PropagateToXYPlane(StraightLineParams line, double z);
    double DistanceOfPoints(TVector3 first, TVector3 second);

 private:
    
   Double_t fdXY;
   Int_t fNLayers;
   Int_t fEventNr;
   std::vector<PndSdsHit> fHits;
   std::vector<std::vector<std::pair<int, bool> > > fHitsPerLayer; ///< Layers (4) of HitsInLayers(n) as position in vector of hits and used indicator

};

#endif /* PndMQStraightLineTrackFinder_H */

