#include "PndLumiTransposition.h"

PndLumiTransposition::PndLumiTransposition()
{
	if (gGeoManager) {
		fGeoMan = gGeoManager;
	} else {
		std::cout << " -E- PndLumiTransposition: No Geometry existing!" << std::endl;
		return;
	}
	//	fGeoMan = gGeoManager;
	fVerbose = 0;

}

PndLumiTransposition::PndLumiTransposition(Int_t verbose, TGeoManager  *geoMan)
{
	if (geoMan == 0) std::cout << "-E- PndLumiTransposition: Not a valid GeoManager" << std::endl;
	fGeoMan = geoMan;
	fVerbose = verbose;
}

void PndLumiTransposition::GetLocalHitPoints(PndLumiPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut)
{
	if (fVerbose > 6){
		cout << "GetLocalHitPoints for Detector: " << myPoint->GetDetName().Data() << endl;
	}
	TGeoHMatrix trans = GetTransformation(myPoint->GetDetName().Data());

	Double_t posIn[3];
	Double_t posOut[3];
	Double_t posInLocal[3];
	Double_t posOutLocal[3];

	posIn[0] = myPoint->GetEntryPoint().X();
	posIn[1] = myPoint->GetEntryPoint().Y();
	posIn[2] = myPoint->GetEntryPoint().Z();

	posOut[0] = myPoint->GetExitPoint().X();
	posOut[1] = myPoint->GetExitPoint().Y();
	posOut[2] = myPoint->GetExitPoint().Z();

	if (fVerbose > 6){
		for (Int_t i = 0; i < 3; i++){
			cout << "posIn "<< i << ": " << posIn[i] << endl;
		}
		// trans.Print("");
	}

	trans.MasterToLocal(posIn, posInLocal);
	trans.MasterToLocal(posOut, posOutLocal);

	TVector3 offset = GetSensorDimensions(myPoint->GetDetName().Data() );
	if (fVerbose > 5)
		cout << " offset : "<< offset.x() << " , "<<offset.y() << " , "<<offset.z()<<endl;

	posInLocal[0] += offset.x();
	posInLocal[1] += offset.y();
	posInLocal[2] += offset.z();

	posOutLocal[0] += offset.x();
	posOutLocal[1] += offset.y();
	posOutLocal[2] += offset.z();
    if (fVerbose > 6) {
       for (Int_t i = 0; i < 3; i++){
         cout << "posInLocal "<< i << ": " << posInLocal[i] << endl;
       }
    }

    myHitIn.setVector(posInLocal);
    myHitOut.setVector(posOutLocal);

 }

TGeoHMatrix PndLumiTransposition::GetTransformation(std::string detName)
{
	fGeoMan->cd(detName.c_str());
	TGeoHMatrix* transMat = fGeoMan->GetCurrentMatrix();
	if (fVerbose > 5)
		transMat->Print("");
	return *transMat;
}


TVector3 PndLumiTransposition::GetSensorDimensions(std::string detName)
{
	fGeoMan->cd(detName.c_str());
	TGeoVolume* actVolume = fGeoMan->GetCurrentVolume();
	TGeoBBox* actBox = (TGeoBBox*)(actVolume->GetShape());
	TVector3 result;
	result.SetX(actBox->GetDX());
	result.SetY(actBox->GetDY());
	result.SetZ(actBox->GetDZ());

  return result;
}

TVector3 PndLumiTransposition::LocalToStripOrientation(Double_t angle,
		FairGeoVector  loc)
{

	TVector3 strip_orient;

	Double_t x = loc.getX();
	Double_t y = loc.getY();
	Double_t z = loc.getZ();

	strip_orient.SetX(x*cos(angle) + y*sin(angle));
	strip_orient.SetY(-x*sin(angle) + y*cos(angle));
	strip_orient.SetZ(z);

	return strip_orient;
}

ClassImp(PndLumiTransposition)
