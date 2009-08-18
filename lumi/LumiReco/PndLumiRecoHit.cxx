/*
 * PndLumiRecoHit.cxx
 *
 *  Created on: Nov 10, 2008
 *      Author: tsito
 */

#include "PndLumiRecoHit.h"

PndLumiRecoHit::PndLumiRecoHit()
{

}

PndLumiRecoHit::PndLumiRecoHit(TVector3 MCPos, TVector3 RecoPos)
{
	fMCPosition = MCPos;
	fRecoPosition = RecoPos;
}
void PndLumiRecoHit::Print(const Option_t* opt) const
{
	std::cout << std::endl
		  << "*** Process Reconstruction for LM : " << std::endl
	      << " MC Hit Position : (" << fMCPosition.X() << " , " << fMCPosition.Y() << " , " << fMCPosition.Z() << ") [cm]"  <<std:: endl
	      << " RC Hit Position : (" << fRecoPosition.X() << " , " << fRecoPosition.Y() << ", " << fRecoPosition.Z() << ") [cm]" << std::endl
	      << "------------------------------------------------------------------" <<std::endl;

}

ClassImp(PndLumiRecoHit)
