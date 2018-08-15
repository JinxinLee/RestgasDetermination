#include "PndMvdTopixHitProducer.h"
#include "TMath.h"
#include "TMatrixT.h"

#include <iostream>
#include "FairMQLogger.h"

PndMvdTopixHitProducer::PndMvdTopixHitProducer() : flx(0), fly(0), fcols(0), frows(0), fVerbose(0)
{
}


PndMvdTopixHitProducer::PndMvdTopixHitProducer(double dimX, double dimY, int maxcol, int maxrow) : flx(dimX), fly(dimY), fcols(maxcol), frows(maxrow), fVerbose(0)
{

}

PndSdsHit PndMvdTopixHitProducer::GetHit(std::vector<PndSdsDigiTopix4> pixelArray)
{
	//pixelArray = pixelArray;
	Double_t col = 0, row = 0, charge = 0;
	Double_t tempCol = 0, tempRow = 0;
	std::vector<Double_t> timeValues;
	std::vector<Double_t> timeValueErrors;

	Int_t count = 0, mcindex=-1;
	//Double_t local[2], master[2];
  //TODO: Get away from default 10 Frontends per column?
	LOG(INFO) << "GetHit:";
	if (pixelArray.size() == 1){
//		LOG(INFO) << "SingleHitCluster";
//		col = pixelArray[0].GetPixelColumn() + ((Int_t)(pixelArray[0].GetFE()%10)) * fcols;
//		if (col < 0){
//			col -= 0.5;
//			col += frows / 2;
//		}
//		else col += 0.5;
//		row = pixelArray[0].GetPixelRow()+ (pixelArray[0].GetFE()/10) * frows;
//		if (row < 0){
//			row -= 0.5;
//			row += flx /2;
//		}
//		else row += 0.5;
////		if (fVerbose > 1)
////			LOG(INFO) << "GetCluster:col/row " << col << " " << row << std::endl;
//		count = 1;
//		charge = pixelArray[0].GetCharge();
		PndSdsDigiTopix4 hit = pixelArray[0];
		LOG(INFO) << "SingleDigi: " << pixelArray[0];
		LOG(INFO) << "TimeStamp: " << pixelArray[0].GetCharge() << " " << pixelArray[0].GetTimeStamp();
		LOG(INFO) << "TimeStamp: " << hit.GetTimeStamp();
		LOG(INFO) << "SingleDigi: " << pixelArray[0];
		  timeValues.push_back(pixelArray[0].GetTimeStamp());
		  timeValueErrors.push_back(pixelArray[0].GetTimeStampError());

	}
	else {
		LOG(INFO) << "Multiple Hits!";
		for (UInt_t i = 0; i < pixelArray.size(); i++){
			//cout << "ActCol / Row" << col << " " << row << " added Col/Row " << pixelArray[i].GetPixelColumn() << " " << pixelArray[i].GetPixelRow() << endl;
			PndSdsDigiTopix4 hit = pixelArray[i];
			LOG(INFO) << i << " : Digi : " << pixelArray[i];
			LOG(INFO) << i << " : Digi : " << pixelArray[i].GetTimeStamp();
			LOG(INFO) << i << " : Digi : " << hit.GetTimeStamp();
//			tempCol = pixelArray[i].GetPixelColumn() + (Int_t)(pixelArray[0].GetFE()%10) * fcols;
//			tempRow = pixelArray[i].GetPixelRow() + pixelArray[0].GetFE()/10 * frows;
//			col += (tempCol*pixelArray[i].GetCharge());
//			row += (tempRow*pixelArray[i].GetCharge());
//			charge += pixelArray[i].GetCharge();
//			timeValues.push_back(pixelArray[i].GetTimeStamp());
//			timeValueErrors.push_back(pixelArray[i].GetTimeStampError());
//			count++;
		}
//		if (count > 0){
//			if (charge > 0){
//				col /= charge;
//				row /= charge;
//			}
//			col += 0.5;
//			row += 0.5;
//		}
//		else col = row = 0;
	}
	if (fVerbose > 1){
		LOG(INFO) << "Col: " << col << " Row: " << row << std::endl;
	}

//  TVector3 offset = GetSensorDimensions(pixelArray[0].GetSensorID());
//  TVector3 locpos( col*flx - offset.X(), row*fly - offset.Y(), 0);
//  TVector3 pos = locpos;
//  pos.SetZ(6 * pixelArray[0].GetSensorID() + 10);
//
//  LOG(INFO) << "Calculated Position: " << col << "/" << row << " : " << pos.X() << "/" << pos.Y();
//
//  Double_t errZ = 2.*GetSensorDimensions(pixelArray[0].GetSensorID()).Z();
//  TMatrixD locCov(3,3);
//  locCov[0][0]=flx*flx/12.;
//  locCov[1][1]=fly*fly/12.;
//  locCov[2][2]=errZ*errZ/12;
//  TMatrixD hitCov=locCov;
//  TVector3 dpos(sqrt(hitCov[0][0]),sqrt(hitCov[1][1]),sqrt(hitCov[2][2]));
 //LOG(INFO) << "-I- PndMvdTopixHitProducer Error DPos: " << dpos.x() << " " << dpos.y() << " " << dpos.z() << std::endl;

  Double_t meanTime = 0;
  //Double_t meanTimeError = 0;
  Double_t sumVar = 0;

  LOG(INFO) << "TimeCalculation:";
  for (UInt_t t = 0; t < timeValues.size(); t++){
	  LOG(INFO) << t << " : " << timeValues[t];
	  meanTime += timeValues[t] / (timeValueErrors[t] * timeValueErrors[t]);
	  sumVar += 1/(timeValueErrors[t] * timeValueErrors[t]);
  }

  if (sumVar > 0)
	  meanTime /= sumVar;

  LOG(INFO) << "MeanTime: " << meanTime;


  PndSdsHit thehit;//pixelArray[0].GetDetID(),pixelArray[0].GetSensorID(), pos, dpos, -1, charge, pixelArray.size(),mcindex);
  //thehit.SetCov(hitCov);
//  thehit.SetTimeStamp(meanTime);
//  thehit.SetTimeStampError(1/sumVar);
  //LOG(INFO) << "-I- PndMvdTopixHitProducer TimeStamp: " << tempTime/count << std::endl;
  return thehit;
}

TGeoHMatrix PndMvdTopixHitProducer::GetTransformation(Int_t sensorID)
{

  TGeoHMatrix transMat;
  Double_t vec[3];
  vec[0] = 0.;
  vec[1] = 0.;
  vec[2] = sensorID * 6 + 10;
  return transMat;
}

TVector3 PndMvdTopixHitProducer::GetSensorDimensions(Int_t sensorID)
{
	TVector3 result(0.2/2, 0.32/2,0.1/2);
	return result;
}
