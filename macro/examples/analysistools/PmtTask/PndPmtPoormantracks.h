#ifndef PNDPMTPOORMANTRACKS_H
#define PNDPMTPOORMANTRACKS_H 1

#include <cmath>
#include "TVector3.h"
#include "TParticlePDG.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "RhoError.h"


class PndPmtPoormantracks
{
  public:
    PndPmtPoormantracks();
    ~PndPmtPoormantracks();
    void SetSigV(double a){fSigVx=fSigVy=fSigVz=a;};
    void SetSigP(double a){fSigPx=fSigPy=fSigPz=a;};
    void SetPtRange(double min, double max){fPtMin=min;fPtMax=max;};
    void SetDTheta(double a){fDtheta=a;};
    void SetPid(int a){fPID=a;};
    void SetNumTrk(int a){fNumTrk=a;};
    void SetSeed(int a){fSeed=a;};
    void SetVerbose(int a){fVerbose=a;};
    void EnergyCorrelations();

  protected:
    TVector3 RollVertexBox(double widx,double widy, double widz);
    void SmearVertex(TVector3& vertex);
    TVector3 RollMomentumBox(const TVector3& vtx, double dtheta, double ptmin, double ptmax);
    void SmearMomentum(TVector3& momentum);
    void PoorManTracks();
    void EnergyCorrelations(RhoError& covP4, TLorentzVector p4);

    TClonesArray* fMcCands;
    TClonesArray* fCands;



    double fSigVx;
    double fSigVy;
    double fSigVz;
    double fSigPx;
    double fSigPy;
    double fSigPz;
    double fCharge;
    double fDtheta;
    int    fPID;
    TParticlePDG* fPDG;
    int    fNumTrk;
    int    niter;
    int    fNDF;
    int fSeed;
    TVector3 fVertex;
    double  fWidx,  fWidy,  fWidz; //[cm] maximum vertex position off origin
    double fPtMin, fPtMax;
    int fVerbose;



};

#endif // PDNPMTPOORMANTRACKS_H
