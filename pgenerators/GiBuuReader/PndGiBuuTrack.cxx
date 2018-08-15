/*
 * PndGiBuuTrack.cxx
 *
 *  Created on: Aug 7, 2012
 *      Author: stockman
 */

#include "PndGiBuuTrack.h"

ClassImp(PndGiBuuTrack);

PndGiBuuTrack::PndGiBuuTrack():
	fPdg(0), fMomentum(), fVertex()
{
}

PndGiBuuTrack::PndGiBuuTrack(Int_t pdg, Double_t px, Double_t py, Double_t pz, Double_t vx, Double_t vy, Double_t vz):
		fPdg(pdg), fMomentum(px, py, pz), fVertex(vx, vy, vz)
{
}
