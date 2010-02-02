//
// C++ Implementation: PndSdsGeoHandling
//
// Description:
//
//
// Author: t.stockmanns <stockman@ikp455>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "PndSdsGeoHandling.h"
#include "PndStringVector.h"
#include <vector>
#include <string>
#include "TROOT.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"

#include <math.h>
#include "stdlib.h"

 ClassImp(PndSdsGeoHandling);

PndSdsGeoHandling::PndSdsGeoHandling()
{
	if (gGeoManager) {
		fGeoMan = gGeoManager;
	} else //if (gROOT->FindObjectAny("FAIRGeom") == 0)
  {
		std::cout << " -E- PndSdsGeoHandling: No Geometry existing!" << std::endl;
		return;
	}
//	fGeoMan = gGeoManager;
  fVerbose = 0;
}

//PndSdsGeoHandling::PndSdsGeoHandling(TString fileName)
//{
//	if (gGeoManager) {
//		fGeoMan = gGeoManager;
//	}else if (gROOT->FindObjectAny("FAIRGeom") == 0){
//		   fGeoMan = new TGeoManager("geoMan","geoMan");
//  		 fGeoMan->Import(fileName.Data());
//	}
////	fGeoMan = gGeoManager;
//  fVerbose = 0;
//}

TString PndSdsGeoHandling::GetCurrentID()
{
 Int_t level;
 Int_t copyNr[100];
 Int_t volNr[100];
 TString result;

 level = fGeoMan->GetLevel();
 level++;

 fGeoMan->GetBranchNumbers(copyNr, volNr);
 for (int i=0; i<level; i++){
 	result += volNr[i];
	result += "_";
	result += copyNr[i];
	result += "/";
 }
 return result;
}

TString PndSdsGeoHandling::GetID(TString path)
{
	TString result;
	TString currentPath = fGeoMan->GetPath();
	fGeoMan->cd(path.Data());
	result = GetCurrentID();
	fGeoMan->cd(currentPath.Data());
	return result;
}


TString PndSdsGeoHandling::GetPath(TString id)
{
	TString result;
	SetGeoManager(gGeoManager);
	//std::cout << "-I- PndSdsGeoHandling::GetPath : " << id.Data() << std::endl;
	std::vector<std::string> idVector;
	PndStringVector pathAna(id.Data(), "/_");
	idVector = pathAna.GetStringVector();

	for(Int_t i = 0; i < idVector.size(); i+=2){
		result += "/";
		Int_t VolId = atoi(idVector[i].c_str());
		Int_t CopyNr = atoi(idVector[i+1].c_str());
//     if(fVerbose>3) std::cout<<" -I- PndSdsGeoHandling::GetPath: VolId = "<<VolId<<std::endl;
		//std::cout << "-I- PndSdsGeoHandling::GetPath : " << VolId;
		//std::cout << " : " << fGeoMan->GetVolume(VolId)->GetName() << std::endl;
		result += fGeoMan->GetVolume(VolId)->GetName();
		result += "_";
		result += CopyNr;
	}
//   if(fVerbose>2) std::cout<<" -I- PndSdsGeoHandling::GetPath: result = "<<result.Data()<<std::endl;
	return result;
}

Bool_t PndSdsGeoHandling::cd(TString id)
{
	return fGeoMan->cd(GetPath(id).Data());
}

TString PndSdsGeoHandling::GetVolumeID(TString name)
{
	TString result;
	TGeoVolume* vol = fGeoMan->FindVolumeFast(name);
	if (vol == 0)
		return result;
	result += vol->GetNumber();
	return result;
}

std::vector<TString> PndSdsGeoHandling::GetNamesLevel(Int_t level, TString startPath, bool fullPath)
{
	TString actPath = fGeoMan->GetPath();
	fLevelNames.clear();

	if (startPath == ""){
		fGeoMan->CdTop();
		fLevel = level;
	}
	else{
		if (fGeoMan->cd(startPath.Data())== 0)
			return fLevelNames;
		else {
			fLevel = fGeoMan->GetLevel() + level;
		}
	}
	FillLevelNames();
	return fLevelNames;
}

void PndSdsGeoHandling::FillLevelNames()
{
	TGeoNode* myNode = fGeoMan->GetCurrentNode();
	if (fLevel == fGeoMan->GetLevel()){
		if (fFullPath)
			fLevelNames.push_back(fGeoMan->GetPath());
		else
			fLevelNames.push_back(myNode->GetName());
	}
	else {
		Int_t nDaughters = myNode->GetNdaughters();
		for (Int_t i = 0; i < nDaughters; i++){
			fGeoMan->CdDown(i);
			FillLevelNames();
			fGeoMan->CdUp();
		}
	}
}

void PndSdsGeoHandling::GetOUVPath(TString path, TVector3& o, TVector3& u, TVector3& v)
{
	Double_t result[3];
	Double_t* temp;
	TString actPath = fGeoMan->GetPath();
	fGeoMan->cd(path);

	TGeoHMatrix* currMatrix = fGeoMan->GetCurrentMatrix();
	temp = currMatrix->GetTranslation();
	o.SetXYZ(temp[0], temp[1], temp[2]);

	temp[0] = 1;
	temp[1] = 0;
	temp[2] = 0;
	fGeoMan->LocalToMasterVect(temp, result);
	u.SetXYZ(result[0], result[1], result[2]);

	temp[0] = 0;
	temp[1] = 1;
	temp[2] = 0;
	fGeoMan->LocalToMasterVect(temp, result);
	v.SetXYZ(result[0], result[1], result[2]);

  if(actPath!="" && actPath!=" ") fGeoMan->cd(actPath);
}

void PndSdsGeoHandling::GetOUVId(TString id, TVector3& o, TVector3& u, TVector3& v)
{
	GetOUVPath(GetPath(id),o,u,v);
}


TVector3 PndSdsGeoHandling::GetSensorDimensionsPath(TString path)
{
  TVector3 dim;
  TString actPath = fGeoMan->GetPath();
  fGeoMan->cd(path);

  TGeoVolume* actVolume = gGeoManager->GetCurrentVolume();
  TGeoBBox* actBox = (TGeoBBox*)(actVolume->GetShape());
  dim.SetX(actBox->GetDX());
  dim.SetY(actBox->GetDY());
  dim.SetZ(actBox->GetDZ());

  if(actPath!="" && actPath!=" ") fGeoMan->cd(actPath);
  return dim;
}

TVector3 PndSdsGeoHandling::GetSensorDimensionsId(TString id)
{
return GetSensorDimensionsPath(GetPath(id));
}

TGeoHMatrix* PndSdsGeoHandling::GetMatrixPath(TString path)
{
	TString actPath = fGeoMan->GetPath();
	fGeoMan->cd(path);

	TGeoHMatrix* currMatrix = fGeoMan->GetCurrentMatrix();
	if(actPath!="" && actPath!=" ") fGeoMan->cd(actPath);

	return currMatrix;

}

TGeoHMatrix* PndSdsGeoHandling::GetMatrixId(TString id)
{
	return GetMatrixPath(GetPath(id));
}

//  ----- conversions of POINTS (not vectors) here -----
TVector3 PndSdsGeoHandling::MasterToLocalId(const TVector3& master, const TString& id)
{ return MasterToLocalPath(master, GetPath(id) ); }

TVector3 PndSdsGeoHandling::MasterToLocalPath(const TVector3& master, const TString& path)
{
//   if(fVerbose>1) std::cout<<" -I- PndSdsGeoHandling::MasterToLocalPath"<<std::endl;
  Double_t result[3];
  Double_t temp[3];

  temp[0] = master.X();
  temp[1] = master.Y();
  temp[2] = master.Z();

  TString actPath = fGeoMan->GetPath();
  fGeoMan->cd(path);
  fGeoMan->MasterToLocal(temp, result);
  if(actPath != "" && actPath != " ") fGeoMan->cd(actPath);
  return TVector3(result[0],result[1],result[2]);
}



TVector3 PndSdsGeoHandling::LocalToMasterId(const TVector3& local, const TString& id)
{ return LocalToMasterPath(local, GetPath(id) ); }

TVector3 PndSdsGeoHandling::LocalToMasterPath(const TVector3& local, const TString& path)
{
  Double_t result[3];
  Double_t temp[3];

  temp[0] = local.X();
  temp[1] = local.Y();
  temp[2] = local.Z();

  TString actPath = fGeoMan->GetPath();
  fGeoMan->cd(path);
  fGeoMan->LocalToMaster(temp, result);
  if(actPath != "" && actPath != " ") fGeoMan->cd(actPath);
  return TVector3(result[0],result[1],result[2]);
}


// ROTATION of error values, CAUTION - these are always psitive defined
TVector3 PndSdsGeoHandling::MasterToLocalErrorsId(const TVector3& master, const TString& id)
{ return MasterToLocalErrorsPath(master, GetPath(id) ); }

TVector3 PndSdsGeoHandling::MasterToLocalErrorsPath(const TVector3& master, const TString& path)
{
  Double_t result[3];
  Double_t temp[3];
  TString actPath = fGeoMan->GetPath();
  fGeoMan->cd(path);

  temp[0] = master.X();
  temp[1] = master.Y();
  temp[2] = master.Z();

  // rotate "error vector"
  fGeoMan->MasterToLocalVect(temp,result);
//  // positive error values
//  for(Int_t i=0;i<3;i++) result[i]=fabs(result[i]);

  if(actPath != "" && actPath != " ") fGeoMan->cd(actPath);
  return TVector3(result[0],result[1],result[2]);


}



TVector3 PndSdsGeoHandling::LocalToMasterErrorsId(const TVector3& local, const TString& id)
{ return LocalToMasterErrorsPath(local, GetPath(id) ); }

TVector3 PndSdsGeoHandling::LocalToMasterErrorsPath(const TVector3& local, const TString& path)
{
  Double_t result[3];
  Double_t temp[3];
  TString actPath = fGeoMan->GetPath();
  fGeoMan->cd(path);

  temp[0] = local.X();
  temp[1] = local.Y();
  temp[2] = local.Z();

  // rotate "error vector"
  fGeoMan->LocalToMasterVect(temp,result);
 // // positive error values
 // for(Int_t i=0;i<3;i++) result[i]=fabs(result[i]);

  if(actPath != "" && actPath != " ") fGeoMan->cd(actPath);
  return TVector3(result[0],result[1],result[2]);

}



