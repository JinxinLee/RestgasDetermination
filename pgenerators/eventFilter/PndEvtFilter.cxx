// -------------------------------------------------------------------------
// -----                     PndEvtFilter source file                 -----
// -------------------------------------------------------------------------


#include "PndEvtFilter.h"


// -----   Default constructor   -------------------------------------------
PndEvtFilter::PndEvtFilter(): FairEvtFilter(){
}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
PndEvtFilter::PndEvtFilter(const char* name, const char* title)
: FairEvtFilter(name, title){
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndEvtFilter::~PndEvtFilter() { }
// -------------------------------------------------------------------------




Bool_t PndEvtFilter::FillList ( RhoCandList& rhoOutList, Int_t inPdgCode, Double_t pdgCodeCharge )
{
	rhoOutList.Cleanup();


	// Get charge corresponding to pdgCode if it has not been passed as an argument by the user
	if ( kNoChargeSpecified == pdgCodeCharge ){
		if ( kFALSE == GetCharge( inPdgCode, &pdgCodeCharge ) ) {
			return kFALSE;
		}
	}



	// find all TParticle on stack with a charge(particle) == charge(pdgCode)
	for (Int_t iPart=0; iPart<fParticleList->GetEntries(); ++iPart) {
		TParticle *particle = (TParticle*)fParticleList->At(iPart);

		// get charge for particle
		TParticlePDG*  pdt = particle->GetPDG();
		if (0==pdt) continue; // unknown particle type (KG, 10/2015)

		Double_t pCharge = pdt->Charge()/3.; // TParticlePDG contains charge in units of |e|/3

		if ( pdgCodeCharge != pCharge ){ continue; } // skip all particles with different charge


		//		// mark stable particles
		//		Bool_t isStable = kFALSE;
		//		switch(abs(particle->GetPdgCode())) {
		//		case 22:   isStable = true; break;
		//		case 11:   isStable = true; break;
		//		case 13:   isStable = true; break;
		//		case 211:  isStable = true; break;
		//		case 321:  isStable = true; break;
		//		case 2212: isStable = true; break;
		//		}
		//
		//		if (kFALSE == isStable) { continue; } // skip all particles which are not considered stable


		// store TParticle as RhoCandidate in output TCA and modify 4 momentum to match particle hypothesis
		TVector3 pVertex(particle->Vx(),particle->Vy(),particle->Vz());
		TLorentzVector p4(particle->Px(),particle->Py(),particle->Pz(),particle->Energy());
		// assign mass hypothesis according to pdgCode
		p4.SetVectM(p4.Vect(),fdbPdg->GetParticle(inPdgCode)->Mass());
		// construct RhoCandidate
		RhoCandidate rhoCand(p4,pCharge);
		rhoCand.SetMcTruth(&rhoCand);
		rhoCand.SetPos(pVertex);
		rhoCand.SetType(inPdgCode);
		// save to output
		rhoOutList.Add(&rhoCand);
	}//trackloop


	return kTRUE;
}

ClassImp(PndEvtFilter)
