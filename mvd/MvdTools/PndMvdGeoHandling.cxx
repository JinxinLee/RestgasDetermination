//
// C++ Implementation: PndMvdGeoHandling
//
// Description: 
//
//
// Author: t.stockmanns <stockman@ikp455>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "PndMvdGeoHandling.h"
#include "PndStringVector.h"
#include <vector>
#include <string>
#include "PndStringVector.h"
#include "TROOT.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include <math.h>
#include "stdlib.h"

 ClassImp(PndMvdGeoHandling);

PndMvdGeoHandling::PndMvdGeoHandling()
{
	if (gROOT->FindObjectAny("CBMGeom") == 0){
		std::cout << " -E- PndMvdGeoHandling: No Geometry existing!" << std::endl;
		return;
	}
	fGeoMan = gGeoManager;
  fVerbose = 0;
}

PndMvdGeoHandling::PndMvdGeoHandling(TString fileName)
{
	if (gROOT->FindObjectAny("CBMGeom") == 0){
		 TGeoManager* fGeoMan = new TGeoManager("geoMan","geoMan");
  		 fGeoMan->Import(fileName.Data());
	}
	fGeoMan = gGeoManager;
  fVerbose = 0;
}

TString PndMvdGeoHandling::GetCurrentID()
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

TString PndMvdGeoHandling::GetID(TString path)
{
	TString result;
	TString currentPath = fGeoMan->GetPath();
	fGeoMan->cd(path.Data());
	result = GetCurrentID();
	fGeoMan->cd(currentPath.Data());
	return result;
}


TString PndMvdGeoHandling::GetPath(TString id)
{
	TString result;
	std::vector<std::string> idVector;
	PndStringVector pathAna(id.Data(), "/_");
	idVector = pathAna.GetStringVector();
	
	for(Int_t i = 0; i < idVector.size(); i+=2){
		result += "/";
		Int_t VolId = atoi(idVector[i].c_str());
		Int_t CopyNr = atoi(idVector[i+1].c_str());
//     if(fVerbose>3) std::cout<<" -I- PndMvdGeoHandling::GetPath: VolId = "<<VolId<<std::endl;
		result += fGeoMan->GetVolume(VolId)->GetName();
		result += "_";
		result += CopyNr;
	}
//   if(fVerbose>2) std::cout<<" -I- PndMvdGeoHandling::GetPath: result = "<<result.Data()<<std::endl;
	return result;
}

Bool_t PndMvdGeoHandling::cd(TString id)
{
	return fGeoMan->cd(GetPath(id).Data());
}

TString PndMvdGeoHandling::GetVolumeID(TString name)
{
	TString result;
	TGeoVolume* vol = fGeoMan->FindVolumeFast(name);
	if (vol == 0)
		return result;
	result += vol->GetNumber();
	return result;
}

std::vector<TString> PndMvdGeoHandling::GetNamesLevel(Int_t level, TString startPath, bool fullPath)
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

void PndMvdGeoHandling::FillLevelNames()
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

void PndMvdGeoHandling::GetOUVPath(TString path, TVector3& o, TVector3& u, TVector3& v)
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

void PndMvdGeoHandling::GetOUVId(TString id, TVector3& o, TVector3& u, TVector3& v)
{
	GetOUVPath(GetPath(id),o,u,v);
}


//  ----- conversions of POINTS (not vectors) here -----
TVector3 PndMvdGeoHandling::MasterToLocalId(const TVector3& master, const TString& id)
{ return MasterToLocalPath(master, GetPath(id) ); }

TVector3 PndMvdGeoHandling::MasterToLocalPath(const TVector3& master, const TString& path)
{
//   if(fVerbose>1) std::cout<<" -I- PndMvdGeoHandling::MasterToLocalPath"<<std::endl;
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



TVector3 PndMvdGeoHandling::LocalToMasterId(const TVector3& local, const TString& id)
{ return LocalToMasterPath(local, GetPath(id) ); }

TVector3 PndMvdGeoHandling::LocalToMasterPath(const TVector3& local, const TString& path)
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
TVector3 PndMvdGeoHandling::MasterToLocalErrorsId(const TVector3& master, const TString& id)
{ return MasterToLocalErrorsPath(master, GetPath(id) ); }

TVector3 PndMvdGeoHandling::MasterToLocalErrorsPath(const TVector3& master, const TString& path)
{
  Double_t result[3];
  Double_t temp[3];
  TString actPath = fGeoMan->GetPath();
  fGeoMan->cd(path);
  TGeoHMatrix* currMatrix = fGeoMan->GetCurrentMatrix();

  // rotate from global into the sensor system
  Double_t* rotation = currMatrix->GetRotationMatrix();
  
  temp[0] = master.X();
  temp[1] = master.Y();
  temp[2] = master.Z();

//   std::cout<<"Rotation of error values:"<<std::endl;
//   std::cout<<"temp[3] = {"<<temp[0]<<", "<<temp[1]<<", "<<temp[2]<<"}"<<std::endl; 
//   std::cout<<"Rotation[3][3] = {"<<std::endl; 
  for(Int_t ii=0;ii<3;ii++) 
  {
//     std::cout<<rotation[3*ii+0]<<", "<<rotation[3*ii+1]<<", "<<rotation[3*ii+2]<<std::endl;
    result[ii] = fabs(   rotation[3*ii+0] * temp[0]
                       + rotation[3*ii+1] * temp[1]
                       + rotation[3*ii+2] * temp[2] 
                               );
  }
//   std::cout<<"}"<<std::endl; 
//   std::cout<<"result[3] = {"<<result[0]<<", "<<result[1]<<", "<<result[2]<<"}"<<std::endl; 

  if(actPath != "" && actPath != " ") fGeoMan->cd(actPath);
  return TVector3(result[0],result[1],result[2]);


}



TVector3 PndMvdGeoHandling::LocalToMasterErrorsId(const TVector3& local, const TString& id)
{ return LocalToMasterErrorsPath(local, GetPath(id) ); }

TVector3 PndMvdGeoHandling::LocalToMasterErrorsPath(const TVector3& local, const TString& path)
{
  Double_t result[3];
  Double_t tmp[3];
  TString actPath = fGeoMan->GetPath();
  fGeoMan->cd(path);
  TGeoHMatrix* currMatrix = fGeoMan->GetCurrentMatrix();

  // rotation back from sensor to lab
  const Double_t* rotation = (currMatrix->Inverse()).GetRotationMatrix();


  tmp[0] = local.X();
  tmp[1] = local.Y();
  tmp[2] = local.Z();

//   std::cout<<"Rotation of error values:"<<std::endl;
//   std::cout<<"tmp[3] = {"<<tmp[0]<<", "<<tmp[1]<<", "<<tmp[2]<<"}"<<std::endl; 
//   std::cout<<"Rotation[3][3] = {"<<std::endl; 
  for(Int_t ii=0;ii<3;ii++) 
  {
//     std::cout<<rotation[3*ii+0]<<", "<<rotation[3*ii+1]<<", "<<rotation[3*ii+2]<<std::endl;
    result[ii] = fabs(   rotation[3*ii+0] * tmp[0]
                       + rotation[3*ii+1] * tmp[1]
                       + rotation[3*ii+2] * tmp[2] 
                               );
  }
//   std::cout<<"}"<<std::endl; 
//   std::cout<<"result[3] = {"<<result[0]<<", "<<result[1]<<", "<<result[2]<<"}"<<std::endl; 

  if(actPath != "" && actPath != " ") fGeoMan->cd(actPath);
  return TVector3(result[0],result[1],result[2]);

}



