/*
 * PndHelixPropagator.cxx
 *
 *  Created on: Sep 23, 2013
 *      Author: stockman
 */

#include "PndHelixPropagator.h"
#include "TMath.h"

#include <iostream>

  ClassImp(PndHelixPropagator)

PndHelixPropagator::PndHelixPropagator() :
	fFieldStrength(0), fOrigin(), fMomentum(), fCharge(0)
{
}

PndHelixPropagator::~PndHelixPropagator() {
	// TODO Auto-generated destructor stub
}

FairTrackPar PndHelixPropagator::PropagateToZ(Double_t zPos)
{
	TVector3 magField(0,0,fFieldStrength);
	TVector3 dirRadius = fMomentum.Cross(magField).Unit();
	TVector3 circleCenter = fOrigin + (fCharge * Radius() * dirRadius);

	Double_t ratioPtPl = fMomentum.Pt() / fMomentum.z();

	TVector3 zeroCircle = fOrigin - circleCenter;

	Double_t zLength = zPos - fOrigin.z();
	Double_t arcLength = ratioPtPl * zLength;
	Double_t deltaPhi = -arcLength / Radius();
	Double_t newPhi = deltaPhi + zeroCircle.Phi();

	TVector3 newOrigin = zeroCircle;
	newOrigin.SetPhi(newPhi);
	newOrigin += circleCenter;
	newOrigin.SetZ(zPos);
	TVector3 newMomentum = fMomentum;

	newMomentum.RotateZ(deltaPhi);

	std::cout << "PndHelixPropagator::PropagateToZ " << zPos << std::endl;
	std::cout << "Origin: "; fOrigin.Print(); std::cout << std::endl;
	std::cout << "Momentum: "; fMomentum.Print(); std::cout << " Ratio Pt/Pl: " << ratioPtPl << " Radius: " << Radius() << " "; dirRadius.Print(); std::cout << std::endl;
	std::cout << "circleCenter: "; circleCenter.Print(); std::cout << std::endl;
	std::cout << "zeroCircle: "; zeroCircle.Print(); std::cout << std::endl;
	std::cout << "Phi: " << zeroCircle.Phi() << " deltaPhi: " << deltaPhi << " newPhi: " << newPhi << std::endl;
	std::cout << "NewOrigin: "; newOrigin.Print(); std::cout << std::endl;
	std::cout << "NewMomentum: "; newMomentum.Print(); std::cout << std::endl;


	FairTrackPar result(newOrigin.x(), newOrigin.y(), newOrigin.z(), newMomentum.Px(), newMomentum.Py(), newMomentum.Pz(), fCharge);

	return result;
}

FairTrackPar  PndHelixPropagator::PropagateToXYPos(TVector2 xyPos)
{
	TVector3 magField(0,0,fFieldStrength);

	TVector3 dirRadius = fMomentum.Cross(magField).Unit();
	TVector3 circleCenter = fOrigin + (fCharge * Radius() * dirRadius);
	circleCenter.SetZ(0);

	Double_t ratioPtPl = fMomentum.Pt() / fMomentum.z();

	TVector3 zeroCircle = fOrigin - circleCenter;

	TVector3 negCircleCenter = -circleCenter;

	Double_t deltaPhi = negCircleCenter.DeltaPhi(zeroCircle);

	TVector3 newOrigin = negCircleCenter.Unit() * Radius() + circleCenter;
	Double_t zPos = deltaPhi * Radius() / ratioPtPl - fOrigin.Z();
	newOrigin.SetZ(zPos);


	TVector3 newMomentum = fMomentum;
	newMomentum.RotateZ(deltaPhi);

	std::cout << "PndHelixPropagator::PropagateToXYPos" << std::endl;
	std::cout << "Origin: "; fOrigin.Print(); std::cout << std::endl;
	std::cout << "Momentum: "; fMomentum.Print(); std::cout << " Ratio Pt/Pl: " << ratioPtPl << " Radius: " << Radius() << " "; dirRadius.Print(); std::cout << std::endl;
	std::cout << "circleCenter: "; circleCenter.Print(); std::cout << " " << circleCenter.Pt() << std::endl;
	std::cout << "zeroCircle: "; zeroCircle.Print(); std::cout << std::endl;
	std::cout << "Phi: " << zeroCircle.Phi() << " deltaPhi: " << deltaPhi << std::endl;
	std::cout << "NewOrigin: "; newOrigin.Print(); std::cout << std::endl;
	std::cout << "NewMomentum: "; newMomentum.Print(); std::cout << std::endl;


	FairTrackPar result(newOrigin.x(), newOrigin.y(), newOrigin.z(), newMomentum.Px(), newMomentum.Py(), newMomentum.Pz(), fCharge);

	return result;
}

FairTrackPar PndHelixPropagator::PropagateByAngle(Double_t step)
{
	TVector3 magField(0,0,fFieldStrength);
	TVector3 dirRadius = fMomentum.Cross(magField).Unit();
	TVector3 circleCenter = fOrigin + (fCharge * Radius() * dirRadius);
	Double_t stepInRad = step / 180 * TMath::Pi();

	Double_t ratioPtPl = 0;
	if (fMomentum.Pz() != 0)
		ratioPtPl = fMomentum.Pt() / fMomentum.Pz();

	TVector3 zeroCircle = fOrigin - circleCenter;

	Double_t newPhi = zeroCircle.Phi() + stepInRad;
	TVector3 newOrigin = zeroCircle;
	newOrigin.SetPhi(newPhi);
	newOrigin += circleCenter;
	if (ratioPtPl != 0)
		newOrigin.SetZ((stepInRad * Radius() * -1 * fCharge / ratioPtPl) + fOrigin.Z());
	else
		newOrigin.SetZ(fOrigin.Z());

	TVector3 newMomentum = fMomentum;

	newMomentum.RotateZ(stepInRad);

	std::cout << "PndHelixPropagator::PropagateByAngle " << step << std::endl;
	std::cout << "Origin: "; fOrigin.Print(); std::cout << std::endl;
	std::cout << "Momentum: "; fMomentum.Print(); std::cout << " Ratio Pt/Pl: " << ratioPtPl << " Radius: " << Radius() << " "; dirRadius.Print(); std::cout << std::endl;
	std::cout << "circleCenter: "; circleCenter.Print(); std::cout << std::endl;
	std::cout << "zeroCircle: "; zeroCircle.Print(); std::cout << std::endl;
	std::cout << "Phi: " << zeroCircle.Phi() << " deltaPhi: " << stepInRad << " newPhi: " << newPhi << std::endl;
	std::cout << "NewOrigin: "; newOrigin.Print(); std::cout << std::endl;
	std::cout << "NewMomentum: "; newMomentum.Print(); std::cout << std::endl;


	FairTrackPar result(newOrigin.x(), newOrigin.y(), newOrigin.z(), newMomentum.Px(), newMomentum.Py(), newMomentum.Pz(), fCharge);
	return result;
}


Double_t PndHelixPropagator::Radius(){
	return fMomentum.Perp() / (fFieldStrength * 0.3)*100;
}
