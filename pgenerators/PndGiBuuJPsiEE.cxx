/*
 * PndGiBuuJPsiEE.cxx
 *
 *  Created on: Aug 9, 2012
 *      Author: stockman
 */

#include "PndGiBuuJPsiEE.h"
#include "PndGiBuuTrack.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "TVector3.h"
#include "TLorentzVector.h"
#include "TRandom.h"

#include <iostream>

ClassImp(PndGiBuuJPsiEE);

PndGiBuuJPsiEE::PndGiBuuJPsiEE() {
}

PndGiBuuJPsiEE::~PndGiBuuJPsiEE() {
}

std::vector<PndGiBuuTrack> PndGiBuuJPsiEE::DecayTrack(PndGiBuuTrack track)
{

	std::vector<PndGiBuuTrack> result;

	if (443 != track.GetPdgId()){
		std::cout << "-E- Particle is not a J/Psi! Given pdgCode is: " << track.GetPdgId() << std::endl;
		result.push_back(track);
		return result;
	}

	TDatabasePDG* pdgBase = TDatabasePDG::Instance();
	if ( ! pdgBase ) Fatal("FairPrimaryGenerator", "No TDatabasePDG instantiated");

	TParticlePDG* JPsi = pdgBase->GetParticle(track.GetPdgId());

	TVector3 JPsiP3;
	TLorentzVector JPsiP4;

	JPsiP3 = track.GetMomentum();

	Double_t JPsiMass = JPsi->Mass();

	JPsiP4.SetXYZM(JPsiP3.X(),JPsiP3.Y(),JPsiP3.Z(),JPsiMass);

	TParticlePDG* eplusPDG = pdgBase->GetParticle(-11);
	TParticlePDG* eminusPDG = pdgBase->GetParticle(11);

	Double_t eMass = eplusPDG->Mass();

	Double_t pee = sqrt(JPsiMass*JPsiMass/4.-eMass*eMass);

	Double_t ctheta = gRandom->Rndm();
	ctheta = 2.*ctheta - 1.;
	Double_t stheta = sqrt(1.-ctheta*ctheta);

	Double_t pi = 3.141592654;
	Double_t phi = gRandom->Rndm();
	phi = 2.*pi*phi;

	TVector3 epP3(pee*cos(phi)*stheta,pee*sin(phi)*stheta,pee*ctheta);
	TVector3 emP3 = -epP3;
	TLorentzVector epP4(epP3,sqrt(pee*pee+eMass*eMass));
	TLorentzVector emP4(emP3,sqrt(pee*pee+eMass*eMass));

	TVector3 vJPsi = JPsiP4.BoostVector();

	epP4.Boost(vJPsi);
	emP4.Boost(vJPsi);


	PndGiBuuTrack eplus(-11, epP4.Px(),epP4.Py(),epP4.Pz(), 0,0,0);
	PndGiBuuTrack eminus(11, emP4.Px(),emP4.Py(),emP4.Pz(), 0,0,0);

	result.push_back(eplus);
	result.push_back(eminus);

	return result;
}

