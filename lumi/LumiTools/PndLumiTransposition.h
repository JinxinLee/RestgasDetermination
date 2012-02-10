/*
 * PndLumiTransposition.h
 *
 * This class is needed to transform the position of
 * the hit in local coordinate system to his position
 * in global coordinate system, and vice-versa
 *
 *  Created on: Aug 26, 2008
 *      Author: tsito
 */

#ifndef PNDLUMITRANSPOSITION_H_
#define PNDLUMITRANSPOSITION_H_

#include "PndLumiPoint.h"
//#include "PndLumiCluster.h"

#include "FairGeoVector.h"
#include "FairGeoNode.h"
#include "FairGeoTransform.h"

#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "TRotation.h"

#include "TVector3.h"
#include "TVector2.h"
#include "TString.h"
#include "TMath.h"

#include <cmath>
#include <vector>
#include <string>

using std::cout;
using std::endl;

class PndLumiTransposition
{
public:

	PndLumiTransposition();

	PndLumiTransposition(Int_t verbose, TGeoManager *geoman);

	~PndLumiTransposition(){};

	void GetLocalHitPoints(PndLumiPoint* myPoint,
			FairGeoVector& myHitIn, FairGeoVector& myHitOut);

	TGeoHMatrix GetTransformation(std::string detName);

	TVector3 GetSensorDimensions(std::string detName);

	TVector3 LocalToStripOrientation(Double_t angle,
			FairGeoVector  loc);

private:

	Int_t fVerbose;
	TGeoManager* fGeoMan;

	ClassDef(PndLumiTransposition,1);
};

#endif /* PNDLUMITRANSPOSITIONL_H_ */
