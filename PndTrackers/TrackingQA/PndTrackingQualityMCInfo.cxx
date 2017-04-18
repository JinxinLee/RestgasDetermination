//
// PndTrackingQualityMCInfo.cxx
// 
// 
// 
//
// authors: Lia Lavezzi - University of Torino (2015)
//

#include "PndTrackingQualityMCInfo.h"
#include <iostream>


using namespace std;

PndTrackingQualityMCInfo::PndTrackingQualityMCInfo() :  fNofMvdPixelPoints(0),  fNofMvdStripPoints(0), fNofSttParalPoints(0), fNofSttSkewPoints(0), fNofGemPoints(0), fNofFtsPoints(0), fReconstructabilityStatus(-1), fMCTrackID(-1), fRecoTrackIDs(TArrayI(0)), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)),  fPosLast(TVector3(0., 0., 0.)), fMomLast(TVector3(0., 0., 0.)), fCharge(0), fPDGCode(-1), fQuality(0.), fMCQuality(0), fAssoRecoTrackID(-1) {}

PndTrackingQualityMCInfo::PndTrackingQualityMCInfo(int nofmvdpix, int nofmvdstr, int nofsttparal, int nofsttskew, int nofgem, int noffts) :  fNofMvdPixelPoints(nofmvdpix),  fNofMvdStripPoints(nofmvdstr), fNofSttParalPoints(nofsttparal), fNofSttSkewPoints(nofsttskew), fNofGemPoints(nofgem), fNofFtsPoints(noffts),fReconstructabilityStatus(-1), fMCTrackID(-1), fRecoTrackIDs(TArrayI(0)), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fPosLast(TVector3(0., 0., 0.)), fMomLast(TVector3(0., 0., 0.)), fCharge(0), fPDGCode(-1), fQuality(0.), fMCQuality(0), fAssoRecoTrackID(-1) {}

PndTrackingQualityMCInfo::PndTrackingQualityMCInfo(const PndTrackingQualityMCInfo &info) : TObject(info), fNofMvdPixelPoints(0),  fNofMvdStripPoints(0), fNofSttParalPoints(0), fNofSttSkewPoints(0), fNofGemPoints(0), fNofFtsPoints(0), fReconstructabilityStatus(-1), fMCTrackID(-1), fRecoTrackIDs(TArrayI(0)), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fPosLast(TVector3(0., 0., 0.)), fMomLast(TVector3(0., 0., 0.)), fCharge(0), fPDGCode(-1), fQuality(0.), fMCQuality(0), fAssoRecoTrackID(-1) {
  *this = info;
}


PndTrackingQualityMCInfo::~PndTrackingQualityMCInfo() {}

PndTrackingQualityMCInfo& PndTrackingQualityMCInfo::operator=(const PndTrackingQualityMCInfo &info) {
  fNofMvdPixelPoints = info.fNofMvdPixelPoints;
  fNofMvdStripPoints = info.fNofMvdStripPoints;
  fNofSttParalPoints = info.fNofSttParalPoints;
  fNofSttSkewPoints = info.fNofSttSkewPoints;
  fNofGemPoints = info.fNofGemPoints;
  fNofFtsPoints = info.fNofFtsPoints;
  fReconstructabilityStatus = info.fReconstructabilityStatus;
  fMCTrackID = info.fMCTrackID;
  fRecoTrackIDs = info.fRecoTrackIDs;
  fPosFirst = info.fPosFirst;
  fMomFirst = info.fMomFirst;
  fPosLast = info.fPosLast;
  fMomLast = info.fMomLast;
  fCharge = info.fCharge;
  fPDGCode = info.fPDGCode;
  fQuality = info.fQuality;
  fMCQuality = info.fMCQuality;

  return *this;
}

ClassImp(PndTrackingQualityMCInfo)
 
  
