//
// C++ Implementation: PndGeoHandling
//
// Description:
//
//
// Author: t.stockmanns <stockman@ikp455>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "PndGeoHandling.h"
#include "PndStringSeparator.h"

#include "PndSensorNameContFact.h"
#include <vector>
#include <string>
#include "TROOT.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TList.h"
#include "TTree.h"
#include "FairEventHeader.h"
#include "FairParRootFileIo.h"

#include <math.h>
#include "stdlib.h"

 ClassImp(PndGeoHandling);

// PndGeoHandling * PndGeoHandling::fGeoHandlingInstance= 0;

 //_____________________________________________________________________________
/*PndGeoHandling * PndGeoHandling::Instance(){

         return fGeoHandlingInstance;
 }
*/
PndGeoHandling::PndGeoHandling():fVerbose(0)
{
/*	if (fGeoHandlingInstance){
		Fatal("PndGeoHandling", "Singleton instance already exists.");
		return;
	}
	fGeoHandlingInstance = this;
*/
	if (gGeoManager) {
		fGeoMan = gGeoManager;
	} else
	{
		Fatal("PndGeoHandling","No gGeoManager");
		return;
	}

	FairRun* ana = FairRun::Instance();

	if (ana != 0){
		FairRuntimeDb* rtdb = ana->GetRuntimeDb();
		if (fVerbose > 0){
			std::cout << "-I- PndGeoHandling::PndGeoHandling: Container PndSensorNamePar ";
			if (rtdb->findContainer("PndSensorNamePar"))
				 std::cout << "exists already" << std::endl;
			else
				std::cout << "does not exist" << std::endl;
		}

		fSensorNamePar = (PndSensorNamePar*) (rtdb->getContainer("PndSensorNamePar"));
		if (fSensorNamePar == 0) std::cout << "-W- PndGeoHandling::PndGeoHandling(): No fSensorNamePar!" << std::endl;
	}
}

PndGeoHandling::PndGeoHandling(TString mcFile, TString parFile):fVerbose(0)
{
/*	if (fGeoHandlingInstance){
		Fatal("PndGeoHandling", "Singleton instance already exists.");
		return;
	}
	fGeoHandlingInstance = this;
*/
	if (gGeoManager) {
		fGeoMan = gGeoManager;
	} else
	{
		Fatal("PndGeoHandling","No gGeoManager");
		return;
	}
	TFile* f = new TFile(mcFile.Data());
	TTree* t = (TTree*)f->Get("cbmsim");
	FairEventHeader* header;
	t->SetBranchAddress("EventHeader.", &header);
	t->GetEntry(0);
	Int_t runId = header->GetRunId();

	t->SetBranchStatus("EventHeader.",0);

	GetSensorNamePar(runId, parFile);
}

PndGeoHandling::PndGeoHandling(Int_t runId, TString parFile):fVerbose(0)
{
	if (gGeoManager) {
		fGeoMan = gGeoManager;
	} else
	{
		Fatal("PndGeoHandling","No gGeoManager");
		return;
	}
	GetSensorNamePar(runId, parFile);
}

void PndGeoHandling::GetSensorNamePar(Int_t runId, TString parFile)
{
	FairRuntimeDb* rtdb = FairRuntimeDb::instance();
	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data(),"UPDATE");
	rtdb->setFirstInput(parInput1);

	rtdb->setOutput(parInput1);

	//FairTask* myTask(new FairTask("FairTask List"));
	rtdb->initContainers(runId);

	fSensorNamePar = (PndSensorNamePar*) (rtdb->getContainer("PndSensorNamePar"));

	rtdb->initContainers(runId);

	if (fVerbose > 1){
		rtdb->Print();
		fSensorNamePar->Print();
	}
}


TString PndGeoHandling::GetCurrentID()
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

TString PndGeoHandling::GetID(TString path)
{
	TString result;
	TString currentPath = fGeoMan->GetPath();
	fGeoMan->cd(path.Data());
	result = GetCurrentID();
	fGeoMan->cd(currentPath.Data());
	return result;
}

Int_t PndGeoHandling::GetShortID(TString path)
{
	TObjString myPath(path.Data());
	if (fSensorNamePar != 0)
		return fSensorNamePar->SensorInList(&myPath);
	else
		std::cout << "-E- PndGeoHandling::GetShortID: SensorNamePar is missing!"	<< std::endl;
	return -1;
}

TString PndGeoHandling::GetPath(Int_t shortID)
{
	if (fSensorNamePar != 0)
		return (fSensorNamePar->GetSensorName(shortID));
	else {
		std::cout << "-E- PndGeoHandling::GetPath(Int_t shortID): Missing SensorNamePar"	<< std::endl;
		abort();
	}
}


TString PndGeoHandling::GetPath(TString id)
{
	TString result;
	SetGeoManager(gGeoManager);
	//std::cout << "-I- PndGeoHandling::GetPath : " << id.Data() << std::endl;
	std::vector<std::string> idVector;
	PndStringSeparator pathAna(id.Data(), "/_");
	idVector = pathAna.GetStringVector();

	for(UInt_t i = 0; i < idVector.size(); i+=2){
		result += "/";
		Int_t VolId = atoi(idVector[i].c_str());
		Int_t CopyNr = atoi(idVector[i+1].c_str());
//     if(fVerbose>3) std::cout<<" -I- PndGeoHandling::GetPath: VolId = "<<VolId<<std::endl;
		//std::cout << "-I- PndGeoHandling::GetPath : " << VolId;
		//std::cout << " : " << fGeoMan->GetVolume(VolId)->GetName() << std::endl;
		result += fGeoMan->GetVolume(VolId)->GetName();
		result += "_";
		result += CopyNr;
	}
//   if(fVerbose>2) std::cout<<" -I- PndGeoHandling::GetPath: result = "<<result.Data()<<std::endl;
	return result;
}

Bool_t PndGeoHandling::cd(TString id)
{
	return fGeoMan->cd(GetPath(id).Data());
}

TString PndGeoHandling::GetVolumeID(TString name)
{
	TString result;
	TGeoVolume* vol = fGeoMan->FindVolumeFast(name);
	if (vol == 0)
		return result;
	result += vol->GetNumber();
	return result;
}

std::vector<TString> PndGeoHandling::GetNamesLevel(Int_t level, TString startPath, bool fullPath)
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

void PndGeoHandling::FillLevelNames()
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

void PndGeoHandling::GetOUVPath(TString path, TVector3& o, TVector3& u, TVector3& v)
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

void PndGeoHandling::GetOUVId(TString id, TVector3& o, TVector3& u, TVector3& v)
{
	GetOUVPath(GetPath(id),o,u,v);
}


TVector3 PndGeoHandling::GetSensorDimensionsPath(TString path)
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

TVector3 PndGeoHandling::GetSensorDimensionsId(TString id)
{
return GetSensorDimensionsPath(GetPath(id));
}

TGeoHMatrix* PndGeoHandling::GetMatrixPath(TString path)
{
	TString actPath = fGeoMan->GetPath();
	fGeoMan->cd(path);

	TGeoHMatrix* currMatrix = fGeoMan->GetCurrentMatrix();
	if(actPath!="" && actPath!=" ") fGeoMan->cd(actPath);

	return currMatrix;

}

TGeoHMatrix* PndGeoHandling::GetMatrixId(TString id)
{
	return GetMatrixPath(GetPath(id));
}

//  ----- conversions of POINTS (not vectors) here -----
TVector3 PndGeoHandling::MasterToLocalId(const TVector3& master, const TString& id)
{ return MasterToLocalPath(master, GetPath(id) ); }

TVector3 PndGeoHandling::MasterToLocalPath(const TVector3& master, const TString& path)
{
//   if(fVerbose>1) std::cout<<" -I- PndGeoHandling::MasterToLocalPath"<<std::endl;
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



TVector3 PndGeoHandling::LocalToMasterId(const TVector3& local, const TString& id)
{ return LocalToMasterPath(local, GetPath(id) ); }

TVector3 PndGeoHandling::LocalToMasterPath(const TVector3& local, const TString& path)
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
TVector3 PndGeoHandling::MasterToLocalErrorsId(const TVector3& master, const TString& id)
{ return MasterToLocalErrorsPath(master, GetPath(id) ); }

TVector3 PndGeoHandling::MasterToLocalErrorsPath(const TVector3& master, const TString& path)
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



TVector3 PndGeoHandling::LocalToMasterErrorsId(const TVector3& local, const TString& id)
{ return LocalToMasterErrorsPath(local, GetPath(id) ); }

TVector3 PndGeoHandling::LocalToMasterErrorsPath(const TVector3& local, const TString& path)
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

TString PndGeoHandling::FindNodePath(TGeoNode* node)
{
  // Find a nodes full path by going there in the gGeoManager
  // With many volumes this becomes surely slow.
  const char* oldpath = fGeoMan->GetPath();
  fGeoMan->CdTop(); // dive down from top node
  DiveDownToNode(node);
  TString pathname = fGeoMan->GetPath();
  fGeoMan->cd(oldpath);
  return pathname;
}

void PndGeoHandling::DiveDownToNode(TGeoNode* node)
{
  // cd gGeoManager from the current node to a given node
  TGeoNode *currentNode = fGeoMan->GetCurrentNode();
  if (currentNode == node) return;
  for (Int_t iNod=0; iNod<currentNode->GetNdaughters();iNod++)
  {
    fGeoMan->CdDown(iNod);
    DiveDownToNode(node);
    if (fGeoMan->GetCurrentNode() == node) return;
    fGeoMan->CdUp();
  }
}

void PndGeoHandling::DiveDownToNodeContainingString(TString name)
{
	TGeoNode *currentNode = fGeoMan->GetCurrentNode();
	TString nodeName(currentNode->GetName());
	if (nodeName.Contains(name)){
		return;
	}
	for (Int_t iNod = 0; iNod < currentNode->GetNdaughters(); iNod++) {
		fGeoMan->CdDown(iNod);
		DiveDownToNodeContainingString( name);
		nodeName = fGeoMan->GetCurrentNode()->GetName();
		if (nodeName.Contains(name)){
			return;
		}
		fGeoMan->CdUp();
	}
}

void PndGeoHandling::DiveDownToFillSensNamePar(std::vector<std::string> listOfSensitives)
{
	if (fSensorNamePar != 0){
		TGeoNode *currentNode = fGeoMan->GetCurrentNode();
		TString nodeName(currentNode->GetName());
		//std::cout << nodeName.Data() << std::endl;

		if (VolumeIsSensitive(nodeName, listOfSensitives)){
			PndStringSeparator sep(nodeName.Data(), "/");
			std::vector<std::string> sepString = sep.GetStringVector();
			if (sepString.size() > 0  && sepString[sepString.size() - 1].find("PartAss") == std::string::npos){
				TObjString* myName = new TObjString(fGeoMan->GetPath());
				fSensorNamePar->AddSensorName(myName);
			}
		}
		for (Int_t iNod = 0; iNod < currentNode->GetNdaughters(); iNod++) {
			fGeoMan->CdDown(iNod);
			DiveDownToFillSensNamePar(listOfSensitives);
			nodeName = fGeoMan->GetCurrentNode()->GetName();
			if (VolumeIsSensitive(nodeName, listOfSensitives)){
				PndStringSeparator sep(nodeName.Data(), "/");
				std::vector<std::string> sepString = sep.GetStringVector();
				if (sepString.size() > 0  && sepString[sepString.size() - 1].find("PartAss") == std::string::npos){
					TObjString* myName = new TObjString(fGeoMan->GetPath());
					fSensorNamePar->AddSensorName(myName);
				}
			}
			fGeoMan->CdUp();
		}
	}
	else
		std::cout << "-E- PndMvdGeoHandling::DiveDownToFillSensNamePar: fSensorNamePar does not exist!" << std::endl;
}

void PndGeoHandling::cd(TGeoNode* node)
{
  // go to a node in the gGeoManager without knowing the full path
  // With many volumes this becomes surely slow.
  fGeoMan->CdTop(); // dive down from top node
  DiveDownToNode(node);
  return;
}

void PndGeoHandling::CreateUniqueSensorId(TString startName, std::vector<std::string> listOfSensitives)
{
	fGeoMan->CdTop();
	DiveDownToNodeContainingString(startName);
	if (fVerbose > 0)
		std::cout << "-I- PndMvdGeoHandling::CreateUniqueSensorId: StartNode: " << fGeoMan->GetPath() << std::endl;
	DiveDownToFillSensNamePar(listOfSensitives);
}


bool PndGeoHandling::VolumeIsSensitive(TString& path, std::vector<std::string>& listOfSensitives)
{
	for (unsigned int i = 0; i < listOfSensitives.size(); i++){
		if (path.Contains(listOfSensitives[i].c_str()))
			return true;
	}
	return false;
}





