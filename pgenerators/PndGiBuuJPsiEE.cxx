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

#include <iostream>

ClassImp(PndGiBuuJPsiEE);

PndGiBuuJPsiEE::PndGiBuuJPsiEE() {
	// TODO Auto-generated constructor stub

}

PndGiBuuJPsiEE::~PndGiBuuJPsiEE() {
	// TODO Auto-generated destructor stub
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

	TParticlePDG* eplusPDG = pdgBase->GetParticle(-11);
	TParticlePDG* eminusPDG = pdgBase->GetParticle(11);

	PndGiBuuTrack eplus(11, 1,1,1, 0,0,0);
	result.push_back(eplus);

	return result;
}
