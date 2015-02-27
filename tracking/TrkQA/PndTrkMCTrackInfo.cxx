//
// PndTrkMCTrackInfo.cxx
// 
// 
// 
//
// authors: Lia Lavezzi - University of Torino (2015)
//

#include "PndTrkMCTrackInfo.h"
#include <iostream>


using namespace std;

PndTrkMCTrackInfo::PndTrkMCTrackInfo() :  fNofMvdPixPoints(0),  fNofMvdStrPoints(0), fNofSttParalPoints(0), fNofSttSkewPoints(0), fNofGemPoints(0), fNofSciTilPoints(0), fIsReconstructable(kFALSE), fMCTrackID(-1), fRecoTrackIDs(TArrayI(0)), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)),  fPosLast(TVector3(0., 0., 0.)), fMomLast(TVector3(0., 0., 0.)), fCharge(0) {}

PndTrkMCTrackInfo::PndTrkMCTrackInfo(int nofmvdpix, int nofmvdstr, int nofsttparal, int nofsttskew, int nofgem, int nofscitil) :  fNofMvdPixPoints(nofmvdpix),  fNofMvdStrPoints(nofmvdstr), fNofSttParalPoints(nofsttparal), fNofSttSkewPoints(nofsttskew), fNofGemPoints(nofgem), fNofSciTilPoints(nofscitil), fIsReconstructable(kFALSE), fMCTrackID(-1), fRecoTrackIDs(TArrayI(0)), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fPosLast(TVector3(0., 0., 0.)), fMomLast(TVector3(0., 0., 0.)), fCharge(0) {}

PndTrkMCTrackInfo::PndTrkMCTrackInfo(const PndTrkMCTrackInfo &info) :  fNofMvdPixPoints(0),  fNofMvdStrPoints(0), fNofSttParalPoints(0), fNofSttSkewPoints(0), fNofGemPoints(0), fNofSciTilPoints(0), fIsReconstructable(kFALSE), fMCTrackID(-1), fRecoTrackIDs(TArrayI(0)), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fPosLast(TVector3(0., 0., 0.)), fMomLast(TVector3(0., 0., 0.)), fCharge(0) {
  *this = info;
}


PndTrkMCTrackInfo::~PndTrkMCTrackInfo() {}

PndTrkMCTrackInfo& PndTrkMCTrackInfo::operator=(const PndTrkMCTrackInfo &info) {
  fNofMvdPixPoints = info.fNofMvdPixPoints;
  fNofMvdStrPoints = info.fNofMvdStrPoints;
  fNofSttParalPoints = info.fNofSttParalPoints;
  fNofSttSkewPoints = info.fNofSttSkewPoints;
  fNofGemPoints = info.fNofGemPoints;
  fNofSciTilPoints = info.fNofSciTilPoints;
  fIsReconstructable = info.fIsReconstructable;
  fMCTrackID = info.fMCTrackID;
  fRecoTrackIDs = info.fRecoTrackIDs;
  fPosFirst = info.fPosFirst;
  fMomFirst = info.fMomFirst;
  fPosLast = info.fPosLast;
  fMomLast = info.fMomLast;
  fCharge = info.fCharge;

  return *this;
}

ClassImp(PndTrkMCTrackInfo)
 
  
