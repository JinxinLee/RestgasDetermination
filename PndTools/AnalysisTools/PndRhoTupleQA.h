#ifndef PNDRHOTUPLEQA_H
#define PNDRHOTUPLEQA_H 1

#include "TLorentzVector.h"
#include "TVector3.h"
#include "TString.h"
#include "RhoCandList.h"

class RhoCandidate;
class RhoTuple;
class PndEventShape;
class PndVtxPoca;
class PndAnalysis;

class PndRhoTupleQA {
public:
	PndRhoTupleQA(PndAnalysis *ana=0, double pbarmom=15.15);
    ~PndRhoTupleQA();

	// *** QA methods
	// *** QA for candidates
	void qaCand(TString pre, RhoCandidate *cc, RhoTuple *n, bool skip=false);
	void qaP4(TString pre, TLorentzVector &c, RhoTuple *n, bool skip=false);
	void qaP4Cms(TString pre, TLorentzVector c, RhoTuple *n, bool skip=false);
    void qaP4Cov(TString pre, RhoCandidate *c, RhoTuple *n, bool skip=false);

	// *** QA for 2-prong
	void qaComp(TString pre, RhoCandidate *c, RhoTuple *n);
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

	// *** QA for the full MC list as array.
	void qaMcList(TString pre, RhoCandList &l, RhoTuple *n);
    void qaMcDiff(TString pre, RhoCandidate *c, RhoTuple *n, bool skip=false);

private:

	PndVtxPoca  *fVtxPoca;
	PndAnalysis *fAnalysis;

	TLorentzVector fIniP4;
};

#endif
