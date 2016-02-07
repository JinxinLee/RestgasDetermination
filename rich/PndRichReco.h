#ifndef PNDRICHRECO_H
#define PNDRICHRECO_H

#include "FairGeoSet.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "PndRichGeo.h"
#include "PndRichPhoton.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <map>

class TClonesArray;

using std::map;
using std::vector; 

class  PndRichReco //: public FairGeoSet
{

 private:
    // removed from PndPidCorrelator.h
    TClonesArray* fRichPDHit;         //! PndRichPDHit TCA
    
    PndRichGeo* fGeo;
    UInt_t fEvent;
    UInt_t fGeoVersion;
    UInt_t fParticleID;
    Double_t fMirrorLength;
    TVector3 fTrackPosition;
    TVector3 fTrackDirection;
    // flat mirror parametrs
    UInt_t fNumberOfFlatMirrorSegments;
    std::vector<TVector3> fMiddleFlatMirrorPoint;
    std::vector<TVector3> fSizeOfFlatMirror;
    std::vector<TVector3> fNormalOfFlatMirror;
    Double_t fPhDetAngle;
    Double_t fZamid;

 protected:
    
  public:
    PndRichReco();
    ~PndRichReco();

    void Init();
    void Register();

    void SetGeoVersion(UInt_t version) { fGeoVersion = version; };
    void RichFullReconstruction(TVector3 pos, TVector3 dir, Float_t ts, Float_t &chi2, Float_t &chTh, Float_t &dChTh, Int_t &nph );
    std::vector<double> GetDThetas();
    std::vector<PndRichPhoton> CherenkovPhotonListFlat( TVector3 pos, TVector3 dir, Double_t time );
    double BetaPeakFinding(std::vector<PndRichPhoton> photons, Double_t nopt, Double_t nnz);
    void HitSelection(std::vector<double> &ph, std::vector<double> &th,
                      std::vector<PndRichPhoton> photons, Double_t beta, Double_t nopt, Double_t nnz);
    vector<TVector3>  FlatMirrorReflections( TVector3 point1, TVector3 point2 );
    ClassDef(PndRichReco,1)
};

#endif
