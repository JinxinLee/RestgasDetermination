/*
 * PndGiBuuTrack.h
 *
 *  Created on: Aug 7, 2012
 *      Author: stockman
 */

#ifndef PNDGIBUUTRACK_H_
#define PNDGIBUUTRACK_H_

#include "TVector3.h"

class PndGiBuuTrack {
public:
	PndGiBuuTrack();
	PndGiBuuTrack(Int_t pdg, Double_t px, Double_t py, Double_t pz, Double_t vx, Double_t vy, Double_t vz);
	virtual ~PndGiBuuTrack(){};

	Int_t GetPdgId(){ return fPdg; }
	TVector3 GetMomentum() { return fMomentum; }
	TVector3 GetVertex() { return fVertex; }

private:
	Int_t fPdg;
	TVector3 fMomentum;
	TVector3 fVertex;

	ClassDef(PndGiBuuTrack, 1);
};

#endif /* PNDGIBUUTRACK_H_ */
