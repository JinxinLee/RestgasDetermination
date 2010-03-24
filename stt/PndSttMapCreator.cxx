#include "PndSttMapCreator.h"
#include "PndSttTube.h"
#include "PndGeoSttPar.h"

#include "FairGeoNode.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoRotation.h"
#include "FairGeoTube.h"

#include "TGeoTube.h"
#include "TVector3.h"
#include "TObjArray.h"
#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoTube.h"
#include "TClonesArray.h"

#include <iostream>

using namespace std;

PndSttMapCreator::PndSttMapCreator(){}

// to use in PndStt
PndSttMapCreator::PndSttMapCreator(Int_t geoType){
  fGeoType = geoType;
  if(fGeoType != 1) cout << "geometry not supported by map" << endl; // CHECK
}

// crete geometry from parameters file
PndSttMapCreator::PndSttMapCreator(PndGeoSttPar *sttPar)
{
  fSttParameters = sttPar;
  // set general par
  SetGeneralParameters();
  // choose geometry type
  fGeoType = sttPar->GetGeometryType(); // classic, optimized, average, detailed, CAD

  if(fGeoType != 1) cout << "geometry not supported by map" << endl; // CHECK
}

PndSttMapCreator::~PndSttMapCreator(){}

void PndSttMapCreator::SetGeneralParameters() {   //  CHECK whether it depends on geometry or not
  fTubeInRad = fSttParameters->GetTubeInRad();   // tube inner radius
  fTubeOutRad = fSttParameters->GetTubeOutRad(); // tube outer radius
}

// during simulation
Int_t PndSttMapCreator::GetTubeIDFromPath(TString path) {
  if(fGeoType == 1) return GetTubeIDFromPathGeoType1(path);
  return -999;
}

// during parameters reading
Int_t PndSttMapCreator::GetTubeIDFromName(TString name){
  if(fGeoType == 1) return GetTubeIDFromNameGeoType1(name);
  return -999;
}

// retrieve parameters from tube ID
PndSttTube * PndSttMapCreator::GetTubeFromTubeID(Int_t tubeid) {
  if(fGeoType == 1) return GetTubeFromTubeIDGeoType1(tubeid);
  return NULL;
}

// fill the tube map at the beginning of the run
TClonesArray * PndSttMapCreator::FillTubeArray() {
  if(fGeoType == 1) return FillTubeArrayGeoType1();
  return NULL;
}

TString PndSttMapCreator::GetNameFromTubeID(Int_t tubeid) {
  if(fGeoType == 1) return GetNameFromTubeIDGeoType1(tubeid);
  else return "NULL";
 }

// ===== GEO TYPE 1 =====
// sensitive volume is gas 
// when reading the parameters we use stt01tube to retrieve geometrical information

// OK
// during simulation
Int_t PndSttMapCreator::GetTubeIDFromPathGeoType1(TString path){
  TString tmpstring = GetNameFromPathGeoType1(path);
  return GetTubeIDFromNameGeoType1(tmpstring);
}

// OK
Int_t PndSttMapCreator::GetTubeIDFromNameGeoType1(TString name){
  // two possibilities:
  // copy : stt01tube#XXX  --> XXX
  // alone: stt01tubeXXX --> XXX   
 
  TString tmpstring = name;

  if(tmpstring.Contains("#")) {
    int start = tmpstring.Index("#") + 1;
    tmpstring = tmpstring(start, tmpstring.Sizeof());
    // add 1 at the end if it is a copy
    tmpstring += 1;
  } 
  else{
    int start = tmpstring.Index("e") + 1;
    tmpstring = tmpstring(start, tmpstring.Sizeof());
    // add 0 at the end if tube alone
    tmpstring += 0;                
  }
  return tmpstring.Atoi();
}

// OK
// name as in geo file: from path "_" to name "#"
TString PndSttMapCreator::GetNameFromPathGeoType1(TString path){
  // two possibilities:
  // copy : /cave_1/stt01assembly_0/stt01tube_XXX/stt01gas_1     --> stt01tube#XXX
  // alone: /cave_1/stt01assembly_0/stt01tubeXXX_0/stt01gasXXX_0 --> stt01tubeXXX

  TString tmpstring = path;

  // cut /cave_1/stt01assembly_0/
  TString cavename;
  cavename = "/cave_1/stt01assembly_0/";
  tmpstring = tmpstring(cavename.Sizeof() - 1, tmpstring.Sizeof());
 
  // cut /stt01gas...
  int index = tmpstring.Index("/");
  tmpstring = tmpstring(0, index);

  // replace _ with #
  if(tmpstring.Contains("_0")) {
    tmpstring.Chop(); // cut "0"
    tmpstring.Chop(); // cut "_"
  }
  else{
    tmpstring.Replace(9, 1, "#");
  }

  return tmpstring;
}

// OK
// name "#"
TString PndSttMapCreator::GetNameFromTubeIDGeoType1(Int_t tubeid) {

  // two possibilities:
  // copy : XXX1 --> stt01tube#XXX  
  // alone: XXX --> stt01tubeXXX

  TString tmpstring; 
  tmpstring += tubeid ;

  if(tmpstring.EndsWith("1")) {
    tmpstring.Chop();
    tmpstring.Prepend("stt01tube#");
  }
  else if(tmpstring.EndsWith("0")) {
    tmpstring.Chop();
    tmpstring.Prepend("stt01tube");
    //    tmpstring.Append("#0");
  }
  else cout << "PndSttMapCreator::GetTubeFromTubeIDGeoType1: the tube id does not end with 0 nor with 1" << endl;

  return tmpstring;

}

// OK
PndSttTube * PndSttMapCreator::GetTubeFromTubeIDGeoType1(Int_t tubeid) {

  TString tubename = GetNameFromTubeIDGeoType1(tubeid);
  TObjArray *geoPassNodes = fSttParameters->GetGeoPassiveNodes();
  FairGeoNode *pnode = (FairGeoNode*) geoPassNodes->FindObject(tubename);
  if(!pnode) {
    cout << "PndSttMapCreator::GetTubeFromTubeIDGeoType1: tube " << tubename << " not found" << endl; 
    return NULL;
  }
      
  FairGeoTransform *lab = pnode->getLabTransform();
  FairGeoVector     tra = lab->getTransVector();
  FairGeoRotation   rot = lab->getRotMatrix();
  
  // geometrical info
  double x = tra.getX()/10.; // in cm
  double y = tra.getY()/10.; // in cm
  double z = tra.getZ()/10.; // in cm
  double r[3][3];
  for(int i = 0; i < 3; i++)for(int j = 0; j < 3; j++) r[i][j] = rot.getElement(i,j);
  
  TGeoVolume* rootvol = pnode->getRootVolume();
  TGeoTube *tube = (TGeoTube*) rootvol->GetShape();
  Double_t halflength = tube->GetDz(); // in cm
  
  
  return new PndSttTube((float)x,(float)y,(float)z,  
		    r[0][0],r[0][1],r[0][2],
		    r[1][0],r[1][1],r[1][2],
		    r[2][0],r[2][1],r[2][2],
		    fTubeInRad, fTubeOutRad, halflength);
}

TClonesArray* PndSttMapCreator::FillTubeArrayGeoType1() { 
  
  TObjArray *geoPassNodes = fSttParameters->GetGeoPassiveNodes();
  TClonesArray *tubeArray = new TClonesArray("PndSttTube");
  tubeArray->Clear();
  for(int i = 0; i < geoPassNodes->GetEntriesFast(); i++) {
    FairGeoNode *pnode = (FairGeoNode*) geoPassNodes->At(i);
    if(!pnode) continue;
    TString tubename = pnode->GetName();
    if(!tubename.Contains("stt01tube")) continue;
    Int_t tubeID = GetTubeIDFromNameGeoType1(tubename);

    PndSttTube *stttube = GetTubeFromTubeIDGeoType1(tubeID); 
    // correspondance position in TCA <-> tubeID
    new((*tubeArray)[tubeID]) PndSttTube(*stttube);
  }

  return tubeArray;
}

ClassImp(PndSttMapCreator)
