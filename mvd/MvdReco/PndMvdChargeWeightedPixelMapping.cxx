#include "PndMvdChargeWeightedPixelMapping.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TClonesArray.h"
#include "TMath.h"

PndMvdHit PndMvdChargeWeightedPixelMapping::GetCluster()
{
	Double_t col = 0, row = 0, charge = 0;
	Double_t tempCol = 0, tempRow = 0;
	Int_t count = 0;
	Double_t local[3], master[3];

	if (fDigiArray.size() == 1){
		if (fDigiArray[0].GetCharge() > 0){
			col = fDigiArray[0].GetPixelColumn() + ((Int_t)(fDigiArray[0].GetFE()%10)) * fParams[0];
			if (col < 0){
				col -= 0.5;
				col += fParams[1] / 2;
			}
			else col += 0.5;
			row = fDigiArray[0].GetPixelRow()+ (fDigiArray[0].GetFE()/10) * fParams[1];
			if (row < 0){
				row -= 0.5;
				row += fParams[2] /2;
			}
			else row += 0.5;
			if (fVerbose > 1)
				std::cout << "GetCluster:col/row " << col << " " << row << std::endl;
			count = 1;
			charge = fDigiArray[0].GetCharge();
		}
	}
	else {
		//cout << "Multiple Hits!" << std::endl;
		for (UInt_t i = 0; i < fDigiArray.size(); i++){
			//cout << "ActCol / Row" << col << " " << row << " added Col/Row " << fDigiArray[i].GetPixelColumn() << " " << fDigiArray[i].GetPixelRow() << endl;
			if (fDigiArray[i].GetCharge() > 0){
				tempCol = fDigiArray[i].GetPixelColumn() + (Int_t)(fDigiArray[0].GetFE()%10) * fParams[0];
				tempRow = fDigiArray[i].GetPixelRow() + fDigiArray[0].GetFE()/10 * fParams[1];
				col += (tempCol*fDigiArray[i].GetCharge());
				row += (tempRow*fDigiArray[i].GetCharge());
				charge += fDigiArray[i].GetCharge();
				count++;
			}
		}
		if (count > 0){
			if (charge > 0){
				col /= charge;
				row /= charge;
			}
			col += 0.5;
			row += 0.5;
		}
		else col = row = 0;
	}
	if (fVerbose > 1){
		std::cout << "Col: " << col << " Row: " << row << std::endl;
	}


  TVector3 offset = GetSensorDimensions(fDigiArray[0].GetDetName().Data());
  TVector3 locpos( col*fParams[2] - offset.X(), row*fParams[3] - offset.Y(), 0);
  TVector3 pos = fGeoH->LocalToMasterId(locpos,fDigiArray[0].GetDetName().Data());

  Double_t errZ = 2.*fGeoH->GetSensorDimensionsId(fDigiArray[0].GetDetName()).Z()/TMath::Sqrt(12.0);
  TVector3 locdpos(fParams[2]/TMath::Sqrt(12.0),fParams[3]/TMath::Sqrt(12.0),errZ);
  TVector3 dpos = fGeoH->LocalToMasterErrorsId(locdpos,fDigiArray[0].GetDetName().Data());




	return (PndMvdHit(fDigiArray[0].GetDetID(),fDigiArray[0].GetDetName().Data(), pos, dpos, -1, charge, fDigiArray.size()));
}

TGeoHMatrix PndMvdChargeWeightedPixelMapping::GetTransformation(std::string detName)
{
  gGeoManager->cd(fGeoH->GetPath(detName.c_str()));
  TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();
  if (fVerbose > 2)
  	transMat->Print("");
  return *transMat;
}

TVector3 PndMvdChargeWeightedPixelMapping::GetSensorDimensions(std::string detName)
{
	gGeoManager->cd(fGeoH->GetPath(detName.c_str()));
	TGeoVolume* actVolume = gGeoManager->GetCurrentVolume();
	TGeoBBox* actBox = (TGeoBBox*)(actVolume->GetShape());
	TVector3 result;
	result.SetX(actBox->GetDX());
	result.SetY(actBox->GetDY());
	result.SetZ(actBox->GetDZ());

	//result.Dump();

	return result;
}
