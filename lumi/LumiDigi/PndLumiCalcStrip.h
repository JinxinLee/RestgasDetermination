/*
 * This gives strips (type PndLumiStrip) fired par hit
 */
#ifndef PNDLUMICALCSTRIP_H
#define PNDLUMICALCSTRIP_H

#include "PndLumiStrip.h"
#include "FairGeoVector.h"

#include "TRandom3.h"
#include <TVector2.h>
#include <TVector3.h>

#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>

#include "PndLumiCalcStripDigi.h"//need for only fRNG definition
using std::cout;
using std::endl;
extern TRandom3* fRND;
class PndLumiCalcStrip
{
public :

	PndLumiCalcStrip();

	PndLumiCalcStrip(Double_t pitch, Double_t orient, Double_t width,
			Double_t length, Double_t threshold, Double_t noise);

	std::vector<PndLumiStrip> GetStrips (FairGeoVector in,
        		FairGeoVector out, Double_t eLoss);

private :

    Double_t fPitch;      // strip pitch (cm)
    Double_t fOrient;     // strip orientation angle
    Double_t fSensorWidth;
    Double_t fSensorLength;

    Double_t fThreshold;  // charge threshold
    Double_t fNoise;      // ENC

    //TRandom3* fRNG;     /// Random Number Generator

    //this refers to the Y-coord of hit in the Strip Direction Coord Syst
    Double_t CalcStripFromPoint(Double_t x, Double_t y);

    Double_t CalcDiag(Double_t a, Double_t b);

    /**
     * Apply gaussian distributed noise to given charge
     * @param charge Equivalent Noise Charge
     * @return Smeared Charge
     */
    Double_t AddNoise(Double_t charge);

    ClassDef(PndLumiCalcStrip,1);
};

#endif /* LUMICALCSTRIP_H */
