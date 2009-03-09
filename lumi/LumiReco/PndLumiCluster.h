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
	PndLumiCluster(TString detname, Double_t side,TVector3 pos_strip,
			TVector3 pos_glo, std::map<Int_t, PndLumiStrip> cluster,
			TGeoHMatrix transMat, TVector3 sensordim, Int_t nhits,
			TVector2 stripzero, Double_t pitch, Double_t orient);

	virtual ~PndLumiCluster();

	//Accessory methods
	TString GetDetName() const {return fDetName;}
	Int_t GetLeftId() const {return fLeftId;}
	Int_t GetRightId() const {return fRightId;}
	Double_t GetRightCharge() const {return fQ_r;}
	Double_t GetLeftCharge() const {return fQ_l;}
	Int_t GetClusterSize() const {return fSize;}
	TVector3 GetStripPosition() const {return fPositionStrip;}
	TVector3 GetMCPosition() const {return fMCPosition;}
	TGeoHMatrix GetTransformationMatrix() const {return fTransMatrix;}
	TVector3 GetSensorDimension() const {return fSensorDim;}
	Int_t GetHitNumber() const {return fNHits;}
	Double_t GetStripZeroId();

private:
	Int_t fLeftId, fRightId;
	Double_t fQ_l, fQ_r;
	Int_t fSize;
	TString fDetName;
	Double_t fSide;
	TVector3 fPositionStrip, fMCPosition;
	TGeoHMatrix fTransMatrix;
	TVector3 fSensorDim;
	Int_t fNHits;
	TVector2 fStripZeroId;
	Double_t fPitch, fOrient;

	ClassDef(PndLumiCluster,1);

};

#endif /* PNDLUMICLUSTER_H_ */
