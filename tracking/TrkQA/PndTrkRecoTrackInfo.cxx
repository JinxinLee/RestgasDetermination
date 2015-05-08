//
// PndTrkRecoTrackInfo.cxx
// 
// 
// 
//
// authors: Lia Lavezzi - University of Torino (2015)
//

#include "PndTrkRecoTrackInfo.h"
#include <iostream>


using namespace std;

PndTrkRecoTrackInfo::PndTrkRecoTrackInfo() :  fNofMvdPixTrueHits(0),  fNofMvdStrTrueHits(0), fNofSttParalTrueHits(0), fNofSttSkewTrueHits(0), fNofGemTrueHits(0), fNofSciTilTrueHits(0), fNofMvdPixFakeHits(0),  fNofMvdStrFakeHits(0), fNofSttParalFakeHits(0), fNofSttSkewFakeHits(0), fNofGemFakeHits(0), fNofSciTilFakeHits(0), fNofMvdPixMissingHits(0),  fNofMvdStrMissingHits(0), fNofSttParalMissingHits(0), fNofSttSkewMissingHits(0), fNofGemMissingHits(0), fNofSciTilMissingHits(0), fMCTrackID(-1), fRecoTrackID(-1), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fCharge(0), fMCTrackInfo(PndTrkMCTrackInfo()), fFlag(-1), fTrue(kTRUE) {}

PndTrkRecoTrackInfo::PndTrkRecoTrackInfo(int recotrackid) : fNofMvdPixTrueHits(0),  fNofMvdStrTrueHits(0), fNofSttParalTrueHits(0), fNofSttSkewTrueHits(0), fNofGemTrueHits(0), fNofSciTilTrueHits(0), fNofMvdPixFakeHits(0),  fNofMvdStrFakeHits(0), fNofSttParalFakeHits(0), fNofSttSkewFakeHits(0), fNofGemFakeHits(0), fNofSciTilFakeHits(0), fNofMvdPixMissingHits(0),  fNofMvdStrMissingHits(0), fNofSttParalMissingHits(0), fNofSttSkewMissingHits(0), fNofGemMissingHits(0), fNofSciTilMissingHits(0), fMCTrackID(-1), fRecoTrackID(recotrackid), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fCharge(0), fMCTrackInfo(PndTrkMCTrackInfo()), fFlag(-1), fTrue(kTRUE) {}

PndTrkRecoTrackInfo::~PndTrkRecoTrackInfo() {}

double PndTrkRecoTrackInfo::GetEfficiency() { 
  if(fMCTrackInfo.GetNofMCPoints() == 0) return -1;  // CHECK
  return (double) GetNofRecoTrueHits()/fMCTrackInfo.GetNofMCPoints(); 
}

double PndTrkRecoTrackInfo::GetMvdPixEfficiency() { 
  if(fMCTrackInfo.GetNofMvdPixPoints() == 0) return -1;  // CHECK
  return (double) GetNofMvdPixTrueHits()/fMCTrackInfo.GetNofMvdPixPoints(); 
}

double PndTrkRecoTrackInfo::GetMvdStrEfficiency() { 
  if(fMCTrackInfo.GetNofMvdStrPoints() == 0) return -1;  // CHECK
  return (double) GetNofMvdStrTrueHits()/fMCTrackInfo.GetNofMvdStrPoints(); 
}

double PndTrkRecoTrackInfo::GetMvdEfficiency() { 
  if(fMCTrackInfo.GetNofMvdPoints() == 0) return -1;  // CHECK
  return (double) GetNofMvdTrueHits()/fMCTrackInfo.GetNofMvdPoints(); 
}

double PndTrkRecoTrackInfo::GetSttParalEfficiency() { 
  if(fMCTrackInfo.GetNofSttParalPoints() == 0) return -1;  // CHECK
  return (double) GetNofSttParalTrueHits()/fMCTrackInfo.GetNofSttParalPoints(); 
}

double PndTrkRecoTrackInfo::GetSttSkewEfficiency() { 
  if(fMCTrackInfo.GetNofSttSkewPoints() == 0) return -1;  // CHECK
  return (double) GetNofSttSkewTrueHits()/fMCTrackInfo.GetNofSttSkewPoints(); 
}

double PndTrkRecoTrackInfo::GetSttEfficiency() { 
  if(fMCTrackInfo.GetNofSttPoints() == 0) return -1;  // CHECK
  return (double) GetNofSttTrueHits()/fMCTrackInfo.GetNofSttPoints(); 
}

double PndTrkRecoTrackInfo::GetGemEfficiency() { 
  if(fMCTrackInfo.GetNofGemPoints() == 0) return -1;  // CHECK
  return (double) GetNofGemTrueHits()/fMCTrackInfo.GetNofGemPoints(); 
}

double PndTrkRecoTrackInfo::GetSciTilEfficiency() { 
  if(fMCTrackInfo.GetNofSciTilPoints() == 0) return -1;  // CHECK
  return (double) GetNofSciTilTrueHits()/fMCTrackInfo.GetNofSciTilPoints(); 
}


double PndTrkRecoTrackInfo::GetPurity() { 
  if(GetNofRecoHits() == 0) return -1;  // CHECK
  return (double) GetNofRecoTrueHits()/GetNofRecoHits(); 
}
double PndTrkRecoTrackInfo::GetSttParalPurity() { 
  if(GetNofSttParalHits() == 0) return -1;  // CHECK
  return (double) GetNofSttParalTrueHits()/GetNofSttParalHits(); 
}
double PndTrkRecoTrackInfo::GetSttSkewPurity() { 
  if(GetNofSttSkewHits() == 0) return -1;  // CHECK
  return (double) GetNofSttSkewTrueHits()/GetNofSttSkewHits(); 
}
double PndTrkRecoTrackInfo::GetSttPurity() { 
  if(GetNofSttHits() == 0) return -1;  // CHECK
  return (double) GetNofSttTrueHits()/GetNofSttHits(); 
}
double PndTrkRecoTrackInfo::GetMvdPixPurity() { 
  if(GetNofMvdPixHits() == 0) return -1;  // CHECK
  return (double) GetNofMvdPixTrueHits()/GetNofMvdPixHits(); 
}
double PndTrkRecoTrackInfo::GetMvdStrPurity() { 
  if(GetNofMvdStrHits() == 0) return -1;  // CHECK
  return (double) GetNofMvdStrTrueHits()/GetNofMvdStrHits(); 
}
double PndTrkRecoTrackInfo::GetMvdPurity() { 
  if(GetNofMvdHits() == 0) return -1;  // CHECK
  return (double) GetNofMvdTrueHits()/GetNofMvdHits(); 
}
double PndTrkRecoTrackInfo::GetGemPurity() { 
  if(GetNofGemHits() == 0) return -1;  // CHECK
  return (double) GetNofGemTrueHits()/GetNofGemHits();
}
double PndTrkRecoTrackInfo::GetSciTilPurity() { 
  if(GetNofSciTilHits() == 0) return -1;  // CHECK
  return (double) GetNofSciTilTrueHits()/GetNofSciTilHits();
}

double PndTrkRecoTrackInfo::GetContamination() { 
 if(GetNofRecoHits() == 0) return -1;  // CHECK
 return (double) GetNofRecoFakeHits()/GetNofRecoHits(); 
}
double PndTrkRecoTrackInfo::GetSttParalContamination() { 
  if(GetNofSttParalHits() == 0) return -1;  // CHECK
  return (double) GetNofSttParalFakeHits()/GetNofSttParalHits(); 
}
double PndTrkRecoTrackInfo::GetSttSkewContamination() { 
  if(GetNofSttSkewHits() == 0) return -1;  // CHECK
  return (double) GetNofSttSkewFakeHits()/GetNofSttSkewHits(); 
}
double PndTrkRecoTrackInfo::GetSttContamination() { 
  if(GetNofSttHits() == 0) return -1;  // CHECK
  return (double) GetNofSttFakeHits()/GetNofSttHits(); 
}
double PndTrkRecoTrackInfo::GetMvdPixContamination() { 
  if(GetNofMvdPixHits() == 0) return -1;  // CHECK
  return (double) GetNofMvdPixFakeHits()/GetNofMvdPixHits(); 
}
double PndTrkRecoTrackInfo::GetMvdStrContamination() { 
  if(GetNofMvdStrHits() == 0) return -1;  // CHECK
  return (double) GetNofMvdStrFakeHits()/GetNofMvdStrHits(); 
}
double PndTrkRecoTrackInfo::GetMvdContamination() { 
  if(GetNofMvdHits() == 0) return -1;  // CHECK
  return (double) GetNofMvdFakeHits()/GetNofMvdHits(); 
}
double PndTrkRecoTrackInfo::GetGemContamination() { 
  if(GetNofGemHits() == 0) return -1;  // CHECK
  return (double) GetNofGemFakeHits()/GetNofGemHits();
}
double PndTrkRecoTrackInfo::GetSciTilContamination() { 
  if(GetNofSciTilHits() == 0) return -1;  // CHECK
  return (double) GetNofSciTilFakeHits()/GetNofSciTilHits(); 
}

double PndTrkRecoTrackInfo::GetInefficiency() { 
  if(fMCTrackInfo.GetNofMCPoints() == 0) return -1;  // CHECK
  return (double) GetNofMissingHits()/fMCTrackInfo.GetNofMCPoints(); 
}

double PndTrkRecoTrackInfo::GetMvdPixInefficiency() { 
  if(fMCTrackInfo.GetNofMvdPixPoints() == 0) return -1;  // CHECK
  return (double) GetNofMvdPixMissingHits()/fMCTrackInfo.GetNofMvdPixPoints(); 
}

double PndTrkRecoTrackInfo::GetMvdStrInefficiency() { 
  if(fMCTrackInfo.GetNofMvdStrPoints() == 0) return -1;  // CHECK
  return (double) GetNofMvdStrMissingHits()/fMCTrackInfo.GetNofMvdStrPoints(); 
}

double PndTrkRecoTrackInfo::GetMvdInefficiency() { 
  if(fMCTrackInfo.GetNofMvdPoints() == 0) return -1;  // CHECK
  return (double) GetNofMvdMissingHits()/fMCTrackInfo.GetNofMvdPoints(); 
}

double PndTrkRecoTrackInfo::GetSttParalInefficiency() { 
  if(fMCTrackInfo.GetNofSttParalPoints() == 0) return -1;  // CHECK
  return (double) GetNofSttParalMissingHits()/fMCTrackInfo.GetNofSttParalPoints(); 
}

double PndTrkRecoTrackInfo::GetSttSkewInefficiency() { 
  if(fMCTrackInfo.GetNofSttSkewPoints() == 0) return -1;  // CHECK
  return (double) GetNofSttSkewMissingHits()/fMCTrackInfo.GetNofSttSkewPoints(); 
}

double PndTrkRecoTrackInfo::GetSttInefficiency() { 
  if(fMCTrackInfo.GetNofSttPoints() == 0) return -1;  // CHECK
  return (double) GetNofSttMissingHits()/fMCTrackInfo.GetNofSttPoints(); 
}

double PndTrkRecoTrackInfo::GetGemInefficiency() { 
  if(fMCTrackInfo.GetNofGemPoints() == 0) return -1;  // CHECK
  return (double) GetNofGemMissingHits()/fMCTrackInfo.GetNofGemPoints(); 
}

double PndTrkRecoTrackInfo::GetSciTilInefficiency() { 
  if(fMCTrackInfo.GetNofSciTilPoints() == 0) return -1;  // CHECK
  return (double) GetNofSciTilMissingHits()/fMCTrackInfo.GetNofSciTilPoints(); 
}


ClassImp(PndTrkRecoTrackInfo)
 
