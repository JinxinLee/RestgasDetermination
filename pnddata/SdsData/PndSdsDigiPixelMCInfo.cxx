/*
 * PndSdsDigiPixelMCInfo.cxx
 *
 *  Created on: Oct 25, 2010
 *      Author: esch
 */

#include "PndSdsDigiPixelMCInfo.h"


PndSdsDigiPixelMCInfo::PndSdsDigiPixelMCInfo() : PndSdsDigiPixel()
{

}
PndSdsDigiPixelMCInfo::PndSdsDigiPixelMCInfo(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t totcharge, Int_t mcPointType, Double_t timeStamp, Double_t chargewonoise, Double_t addnoise, Double_t timewalk, Double_t timewalkcorrection, Double_t tof, Double_t digicharge ) :
  PndSdsDigiPixel(index,detID,sensorID,fe,col,row,totcharge,mcPointType,timeStamp),fMCCharge(chargewonoise),fAddNoise(addnoise), fTimeWalk(timewalk), fTimeWalkCorrection(timewalkcorrection), fTof(tof), fDigiCharge(digicharge)
{


}

PndSdsDigiPixelMCInfo::~PndSdsDigiPixelMCInfo() {
	// TODO Auto-generated destructor stub
}

ClassImp(PndSdsDigiPixelMCInfo);
