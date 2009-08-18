/*
 * PndLumiCluster.h
 *
 *  Created on: Nov 18, 2008
 *      Author: tsito
 */

#ifndef PNDLUMICLUSTER_H_
#define PNDLUMICLUSTER_H_

#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "TObject.h"
#include "TVector3.h"
#include "TString.h"

#include "PndLumiPoint.h"
#include "PndLumiStrip.h"
#include "PndLumiTransposition.h"

#include <map>
#include <string>
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

class PndLumiCluster: public TObject
{
public:
	PndLumiCluster();
	PndLumiCluster(TString detname, TVector3 mcpos, Double_t ClusterPositionFront,
			Double_t ClusterPositionBack,TGeoHMatrix trans,	TVector3 sensdim,
			Double_t pitch, Double_t orient_front, Double_t orient_back);

	virtual ~PndLumiCluster();

	//Accessory methods
	TString GetDetName() const {return fDetName;}
	Double_t GetClusterPositionFront() const {return fPosFront;}
	Double_t GetClusterPositionBack() const {return fPosBack;}
	TVector3 GetMCPosition() const {return fMCPosition;}
	TGeoHMatrix GetTransformationMatrix() const {return fTransMatrix;}
	TVector3 GetSensorDimension() const {return fSensorDim;}
	Double_t GetStripOrientFront() const {return fOrient_front;}
	Double_t GetStripOrientBack() const {return fOrient_back;}
	//Int_t GetHitNumber() const {return fNHits;}
	//Double_t GetStripZeroId();

private:
	TString fDetName;
	TVector3 fMCPosition;
	TGeoHMatrix fTransMatrix;
	TVector3 fSensorDim;
	Double_t fPitch, fOrient_front, fOrient_back;
	Double_t fPosFront, fPosBack;



	ClassDef(PndLumiCluster,1);

};

#endif /* PNDLUMICLUSTER_H_ */
