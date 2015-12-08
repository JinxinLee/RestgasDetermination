#ifndef PNDRHOTUPLEQA_H
#define PNDRHOTUPLEQA_H 1

#include "TLorentzVector.h"
#include "TVector3.h"
#include "TString.h"
#include "RhoCandList.h"
#include "PndVtxPoca.h"

class RhoCandidate;
class RhoTuple;
class PndEventShape;
class PndAnalysis;
class RhoFitterBase;

class PndRhoTupleQA {
public:
	PndRhoTupleQA(PndAnalysis *ana=0, double pbarmom=15.15);
    ~PndRhoTupleQA();

	// *** QA methods
	// *** QA for candidates
	void qaCand(TString pre, RhoCandidate *cc, RhoTuple *n, bool skip=false);
	void qaP4(TString pre, TLorentzVector c, RhoTuple *n, bool skip=false);
	void qaP4Cms(TString pre, TLorentzVector c, RhoTuple *n, bool skip=false);
	void qaP4Cov(TString pre, RhoCandidate *c, RhoTuple *n, bool skip=false);
	void qaPos(TString pre, TVector3 p, RhoTuple *n, bool skip=false);
	void qaPull(TString pre, RhoCandidate *c, RhoTuple *n, bool skip=false);

	// *** QA for 2-prong
	void qaComp(TString pre, RhoCandidate *c, RhoTuple *n, bool covs=false, bool pulls=false);
	void qaKs0(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaPi0(TString pre, RhoCandidate *c, RhoTuple *n);

	// *** QA of event shape
	void qaEventShape(TString pre, PndEventShape *evsh, RhoTuple *n);
	void qaEventShapeShort(TString pre, PndEventShape *evsh, RhoTuple *n);

	// *** QA for parts of eventshape
	void qaESPidMult(TString pre, PndEventShape *evsh, double prob, double pmin, RhoTuple *n);
	void qaESMult(TString pre, PndEventShape *evsh, RhoTuple *n);
	void qaESSum(TString pre, PndEventShape *evsh, RhoTuple *n);
	void qaESMinMax(TString pre, PndEventShape *evsh, RhoTuple *n);
	void qaESEventVars(TString pre, PndEventShape *evsh, RhoTuple *n);

	// *** QA track, vtx, PID, decay
	void qaVtx(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaPoca(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaPRG(TString pre, RhoCandidate *c, RhoTuple *n);
	void qa2Body(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaDalitz(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaTrk(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaPid(TString pre, RhoCandidate *c, RhoTuple *n);

	// *** QA PndPidCandidate raw values
	void qaEmc(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaMvd(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaStt(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaGem(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaDrc(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaDsc(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaRich(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaTof(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaMuo(TString pre, RhoCandidate *c, RhoTuple *n);

	void qaRecoFull(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaRecoFullTree(TString pre, RhoCandidate *c, RhoTuple *n);

	// *** QA for the full MC list as array.
	void qaMcList(TString pre, RhoCandList &l, RhoTuple *n, int max=10000);
	void qaMcList(RhoTuple *n, int max=10000);
	// *** QA for the MC truth of candidates and the difference of the relevant properties (pos, p4, etc.)
    void qaMcDiff(TString pre, RhoCandidate *c, RhoTuple *n, bool skip=false);
	void qaMc(TString pre, RhoCandidate *c, RhoTuple *n, bool skip);

	void qaFitter(TString pre, RhoFitterBase* fitter, RhoTuple *n, bool skip=false);

	// *** Setters and getters
	void SetIniP4(TLorentzVector _ini) {fIniP4 = _ini;}
	TLorentzVector GetIniP4() {return fIniP4;}

private:

	PndVtxPoca  *fVtxPoca;
	PndAnalysis *fAnalysis;

	TLorentzVector fIniP4;    // initial 4-momentum
};

#endif
