// -------------------------------------------------------------------------
// -----                 PndFastSim header file                        -----
// -----                 Created 20/11/07  by K. Goetzen               -----
// -------------------------------------------------------------------------

#ifndef PNDFASTSIM_H
#define PNDFASTSIM_H 1


#include "FairTask.h"
#include "TVector3.h"
#include "TMatrixD.h"
#include <string>
#include <list>
#include "TString.h"

class TClonesArray;
class TObjectArray;
class PndFsmTrack;
class TRandom3;
class TLorentzVector;
class PndFsmAbsDet;
class PndFsmResponse;
class PndFsmDetFactory;
class TF1;
class TDatabasePDG;
class RhoCandList;

typedef std::list<PndFsmAbsDet*> FsmAbsDetList;
typedef std::list<PndFsmResponse*> FsmResponseList;

class PndFastSim : public FairTask
{

 public:
    //typedef std::map<Int_t, Float_t> mapper;

  /** Default constructor **/
  PndFastSim(bool persist=true);


  /** Destructor **/
  ~PndFastSim();

  void  Register();                 //


  /** Virtual method Init **/
  virtual InitStatus Init();

  virtual void Finish();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  bool AddDetector(std::string name, std::string params="");
  bool AddDetector(PndFsmAbsDet* det);
  void SetVerbosity(int vb) {fVb=vb;}
  bool EnableSplitoffs(std::string fname="splitpars.dat");
  bool MergeNeutralClusters(bool merge=true, double par=0.389) {fMergeNeutralClusters=merge; fMergeProbPar=par; return true;}
  void EnableElectronBremsstrahlung(bool brems=true){fElectronBrems=brems;}
  void EnablePropagation(bool propagate=true, bool tostartvtx=true, bool usecovmatrix=true, double tolerance=0.0);
  void SetUseFlatCov(bool v=true) {fUseFlatCovMatrix=v;};
  
  void SetMultFilter(TString type, int min, int max=1000);
  void SetInvMassFilter(TString filter, double min, double max, int mult=1);

  void SetSeed(unsigned int seed=65539);
  //void CreateStructure();

 private:

  PndFsmResponse* sumResponse(FsmResponseList respList);

  int acceptFilters(RhoCandList &l);
  int  chCon(int i);
  void copyAndSetMass(RhoCandList &l, RhoCandList &nl, int pdg);
  
  bool cutAndSmear(PndFsmTrack *t, PndFsmResponse *r);
  bool cutAndSmear(PndFsmTrack *t);

  void smearEnergy(PndFsmTrack *t, double dE);
  void smearMomentum(PndFsmTrack *t, double dp);
  void smearTheta(PndFsmTrack *t, double dtheta);
  void smearPhi(PndFsmTrack *t, double dphi);
  void smearVertex(PndFsmTrack *t, TVector3 dV);
  void smearM(PndFsmTrack *t, double dm);
  void smearM2(PndFsmTrack *t, double m2);
  void smearMvddEdx(PndFsmTrack *t, double dedx);
  void smearTpcdEdx(PndFsmTrack *t, double dedx);
  void smearSttdEdx(PndFsmTrack *t, double dedx);
  void SetFlatCovMatrix(PndFsmTrack *t, double dp=0., double dtheta=0., double dphi=0., double dE=0., double dx=0., double dy=0., double dz=0.);

  /** Output array of Candidates **/
  TClonesArray* fMcCandidates;
  TClonesArray* fPidChargedCand;
  TClonesArray* fPidNeutralCand;
  TClonesArray* fMicroCandidates;
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles
  TClonesArray* fPidNeutralProb;    //! PndPidProbability TCA for neutral particles

  std::map<TString,TClonesArray*>        fPidArrayList;      //! PndPidProbability TCA's for individual detectors
  //output array EventInfo
  TClonesArray* fEventInfo;

  TRandom3  *fRand;
  int       fVb;   //verbosity level
  int       evtcnt; //event counter for output
  TF1       *fspo[5][4];
  TF1       *fBremsEnergy;
  bool      fGenSplitOffs;
  bool      fMergeNeutralClusters;
  bool      fElectronBrems;
  double    fMergeProbPar;
  bool      fPropagate;
  bool      fToStartVtx;
  bool      fUseCovMatrix;
  bool      fUseFlatCovMatrix;
  double    fTolerance;
  
  // filter vars
  bool      fApplyFilter;
  int       fMultMin[6];
  int       fMultMax[6];
  TString   fInvMassFilter;
  double    fInvMassMin;
  double    fInvMassMax;
  int       fInvMassMult;
  int       fCombIndex[5];
  int       fCombMult;
  bool      fChargeConj;
  int       fNAccept;
  
  bool 		fPersist;

  PndFsmDetFactory *fDetFac;
  std::string fAddedDets;
  FsmAbsDetList fDetList;

  TDatabasePDG* fdbPdg;


	/** Get parameter containers **/
  virtual void SetParContainers();


  bool smearTrack(PndFsmTrack *t, int idx = -1);

  static TMatrixD fRho;
  static TMatrixD fEta;

  ClassDef(PndFastSim,1);

};

#endif
