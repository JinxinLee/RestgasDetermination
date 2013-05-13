/////////////////////////////////////////////////////////////
// PndSttGeometryMap
//
// integration between PndSttStrawMap class and 
// /development/lia/stt/PndSttMapCreator functions to create 
// a map of the STT and retrieve layerID, sectorID, neighboring
// tubes.
//
// This class is called directly inside PndSttMapCreator and 
// fills the PndSttTube with the relevant information.
//
// The user does not need to call this class but can retrieve all
// the info in the PndSttTube obj after having filled the 
// fTubeArray via the PndSttMapCreator.
//
// created: May 2013
// authors: L. Lavezzi (PndSttMapCreator 
//                      in development branch - Nov 2012)
//          M. Mertens (PndSttStrawMap - Apr 16, 2013)
//          
/////////////////////////////////////////////////////////////

#include "PndSttGeometryMap.h"
#include "PndSttTubeParameters.h"
#include "PndSttTube.h"
#include "PndGeoSttPar.h"

#include "FairGeoNode.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoRotation.h"
#include "FairGeoTube.h"
#include "FairRun.h"

#include "TGeoTube.h"
#include "TVector3.h"
#include "TObjArray.h"
#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoTube.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TList.h"

#include <iostream>

using namespace std;

PndSttGeometryMap::PndSttGeometryMap() : fTubeArray(new TClonesArray), fGeoType(-1), fVerbose(0) {}


PndSttGeometryMap::PndSttGeometryMap(TClonesArray *tubearray, Int_t geoType) : fTubeArray(tubearray), fGeoType(geoType), fVerbose(0) {
  if(fGeoType != 1) cout << "ERROR PndSttGeometryMap: fGeoType " << fGeoType << " not supported" << endl;
  SetGeneralParameters();
  GenerateStrawMap(1);
}

PndSttGeometryMap::~PndSttGeometryMap(){}

// ========================================================================
// ********************* "INTERFACE" FUNCTIONS ****************************
void PndSttGeometryMap::SetGeneralParameters() { 
  if(fGeoType == 1) SetGeneralParametersGeoType1();
}

void PndSttGeometryMap::GenerateStrawMap(Int_t map) {
  if(fGeoType == 1) GenerateStrawMapGeoType1(map);
}

Bool_t PndSttGeometryMap::FillGeometryParameters() {
  if(fGeoType == 1) FillGeometryParametersGeoType1();
}

// ************************ GEO TYPE 1 SPECIFIC ***************************
// ========================================================================
// development PndSttMapCreator
void PndSttGeometryMap::SetGeneralParametersGeoType1() {   //  CHECK whether it depends on geometry or not
 
  //   fNLayers = 18;
  //   fNSectors = 6;
  //   const int size = 18* 6; // CHECK
 
  fNLayers = 26;
  fNSectors = 6;
  const int size = 26* 6; // CHECK
 
 
  fNTubes_inner_parallel = 1000;
  fNTubes_outer_parallel = 884;
  fNTubes_fillup_parallel = 914;
  fNTubes_skewed = 1744;
  fNTubes = fNTubes_inner_parallel + fNTubes_outer_parallel + fNTubes_fillup_parallel + fNTubes_skewed;

  fNLayers_inner_parallel = 8;
  fNLayers_skew = 8;
  fNLayers_outer_parallel = 4;
  fNLayers_fillup_parallel = 6;
  
  fStartTube = (int**) malloc(sizeof(int*) * fNSectors);
  fEndTube   = (int**) malloc(sizeof(int*) * fNSectors);
  fShift = (int**) malloc(sizeof(int*) * fNSectors);
  for(int i = 0; i < fNSectors; i++) {
    fStartTube[i] = (int*) malloc(sizeof(int*) * fNLayers);
    fEndTube[i] = (int*) malloc(sizeof(int*) * fNLayers);
    fShift[i] = (int*) malloc(sizeof(int*) * fNLayers);
  }


  int start_list[size] = { 1, 105, 215, 331, 453, 581, 715, 855, 1001, 1195, 1395, 1601, 1813, 2037, 2267, 2503, 2745, 2957, 3175, 3399, 3629, 3853, 4051, 4231, 4381, 4489, 
			   17, 122, 233, 350, 473, 602, 737, 878, 1033, 1228, 1428, 1635, 1850, 2075, 2305, 2542, 2779, 2992, 3211, 3436, 3666, 3886, 4081, 4256, 4399, 4498, 
			   36, 142, 254, 372, 496, 626, 762, 904, 1066, 1262, 1465, 1673, 1888, 2114, 2347, 2585, 2816, 3030, 3250, 3476, 3704, 3919, 4111, 4281, 4417, 4507, 
			   53, 160, 273, 392, 517, 648, 785, 928, 1098, 1295, 1498, 1707, 1925, 2152, 2385, 2624, 2851, 3066, 3287, 3514, 3741, 3952, 4141, 4306, 4435, 4516, 
			   69, 177, 291, 411, 537, 669, 807, 951, 1130, 1328, 1531, 1741, 1962, 2190, 2423, 2663, 2885, 3101, 3323, 3551, 3778, 3985, 4171, 4331, 4453, 4525, 
			   88, 197, 312, 433, 560, 693, 832, 977, 1163, 1362, 1568, 1779, 2000, 2229, 2465, 2706, 2922, 3139, 3362, 3591, 3816, 4018, 4201, 4356, 4471, 4534};
 
  int end_list[size] = { 16, 121, 232, 349, 472, 601, 736, 877,  1032, 1227, 1427, 1634, 1849, 2074, 2304, 2541, 2778, 2991, 3210, 3435, 3665, 3885, 4080, 4255, 4398, 4497,
			 35, 141, 253, 371, 495, 625, 761, 903, 1065, 1261, 1464, 1672, 1887, 2113, 2346, 2584, 2815, 3029, 3249, 3475, 3703, 3918, 4110, 4280, 4416, 4506, 
			 52, 159, 272, 391, 516, 647, 784, 927, 1097, 1294, 1497, 1706, 1924, 2151, 2384, 2623, 2850, 3065, 3286, 3513, 3740, 3951, 4140, 4305, 4434, 4515, 
			 68, 176, 290, 410, 536, 668, 806, 950, 1129, 1327, 1530, 1740, 1961, 2189, 2422, 2662, 2884, 3100, 3322, 3550, 3777, 3984, 4170, 4330, 4452, 4524,
			 87, 196, 311, 432, 559, 692, 831, 976, 1162, 1361, 1567, 1778, 1999, 2228, 2464, 2705, 2921, 3138, 3361, 3590, 3815, 4017, 4200, 4355, 4470, 4533, 
			 104, 214, 330, 452, 580, 714, 854, 1000, 1194, 1394, 1600, 1812, 2036, 2266, 2502, 2744, 2956, 3174, 3398, 3628, 3852, 4050, 4230, 4380, 4488, 4542};
  
  int shift_list[size] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4,  7, 11, 16,
			  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 7, 10, 14, 19,
			  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 7, 10, 14, 19,
			  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4,  7, 11, 16,
			  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 7, 10, 14, 19,
  			  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 7, 10, 14, 19};


  int counter = 0;
  for(int i = 0; i < fNSectors; i++) {
    for(int j = 0; j < fNLayers; j++) {
      fStartTube[i][j] = start_list[counter];
      fEndTube[i][j] = end_list[counter];
      fShift[i][j] = shift_list[counter];
      counter++;
    }
  }

  if(fVerbose > 0) {
    // =============== PRINT
    for(int irow = 0; irow < fNSectors; irow++) {
      for(int icol = 0; icol < fNLayers; icol++) {
	cout << " " << fStartTube[irow][icol];
      }
      cout << endl;
    }
    cout << endl;
    for(int irow = 0; irow < fNSectors; irow++) {
      for(int icol = 0; icol < fNLayers; icol++) {
	cout << " " << fEndTube[irow][icol];
      }
      cout << endl;
    }
    cout << endl;
    for(int irow = 0; irow < fNSectors; irow++) {
      for(int icol = 0; icol < fNLayers; icol++) {
	cout << " " << fShift[irow][icol];
      }
      cout << endl;
    }
    cout << endl;
  }

}

Bool_t PndSttGeometryMap::FillGeometryParametersGeoType1() {

  for (int itube = 1; itube < fTubeArray->GetEntriesFast(); itube++) {
    PndSttTube* tube = (PndSttTube*) fTubeArray->At(itube);
    tube->SetLayerLimitFlag(IsEdgeStraw(itube));
    tube->SetSectorLimitFlag(IsSectorBorderStraw(itube));

    bool found = false;
    found =  FindNeighborings(tube);
    if(!found) cout << "ERROR IN FINDIN NEIGHBORING " << endl;
     
  }
}

Bool_t PndSttGeometryMap::FindNeighborings(PndSttTube *tube) {
  double tolerance = 1.5; // CHECK tolerance
  TArrayI neighboring;

  int isector = tube->GetSectorID();
  int ilayer = tube->GetLayerID();
  //  cout << "LAYER/SECTOR " << ilayer << " " << isector << endl;


  // possible lay/sec to check:
  // same layer/same sector, before, after
  // layer up/same sector, before, after
  // layer down/same sector, before, after
  int possible_lay_sec[9][2] = {{ilayer, isector}, {ilayer, isector + 1}, {ilayer, isector - 1}, {ilayer + 1, isector}, {ilayer + 1, isector + 1}, {ilayer + 1, isector - 1}, {ilayer - 1, isector}, {ilayer - 1, isector + 1}, {ilayer - 1, isector - 1}};


  std::vector<int> neigh_candidates;
  for(int itest = 0; itest < 9; itest++) {
    
    PndSttTube *tube2 = NULL;
    int tubeid = -1;
    //    cout << "is " << possible_lay_sec[itest][0] << " " << possible_lay_sec[itest][1] << "good?" << endl;
    // check it is an existing layer/sec
    if(possible_lay_sec[itest][0] < 0 || possible_lay_sec[itest][0] >= fNLayers) continue;
    if(possible_lay_sec[itest][1] < 0 || possible_lay_sec[itest][1] >= fNSectors) continue;
    // skip che pipe
    if((isector == 0 && possible_lay_sec[itest][1] == 5) || (isector == 5 && possible_lay_sec[itest][1] == 0) || (isector == 2 && possible_lay_sec[itest][1] == 3) || (isector == 3 && possible_lay_sec[itest][1] == 2)) continue;
    // if not @ limit of the sector
    if(tube->IsSectorLimit() == kFALSE && (possible_lay_sec[itest][1] == isector + 1 ||  possible_lay_sec[itest][1] == isector - 1))  continue;

    //    cout << "CHECKING " << possible_lay_sec[itest][0] <<  " " << possible_lay_sec[itest][1] << endl;
    neigh_candidates = GetStrawRow(possible_lay_sec[itest][1], possible_lay_sec[itest][0]);

    for(int itube = 0; itube < neigh_candidates.size(); itube++) {
      tubeid = neigh_candidates.at(itube);
      if(tube->GetTubeID()  == tubeid) continue;
      if(tubeid != -1) tube2 = (PndSttTube*) fTubeArray->At(tubeid);
      double distance = 1000;
      if(tube2) distance = tube->GetDistance(tube2);
      if(distance < tolerance) {
	int size =  neighboring.GetSize();
	neighboring.Set(size + 1);
	neighboring.AddAt(tubeid, size);
	//	cout << "ADD " << tubeid << " " << distance << endl; 
      }
    }
  }
  tube->SetNeighborings(neighboring); // CHECK
  return kTRUE;
}




void PndSttGeometryMap::GenerateStrawMapGeoType1(Int_t map) {
  if(map == 0) GenerateStrawMapTubeIDGeoType1();
  else if(map == 1) GenerateStrawMapAngleGeoType1();
}

// ========================================================================
// PndSttStrawMap class functions
void PndSttGeometryMap::GenerateAngles()
{
  fSectorStart.push_back(1.57);
  fSectorEnd.push_back(2.62);
  fSectorStart.push_back(2.62);
  fSectorEnd.push_back(-2.62+2*TMath::Pi());
  fSectorStart.push_back(-2.62+2*TMath::Pi());
  fSectorEnd.push_back(-1.57+2*TMath::Pi());
  fSectorStart.push_back(-1.57+2*TMath::Pi());
  fSectorEnd.push_back(-0.52+2*TMath::Pi());
  fSectorStart.push_back(-0.52+2*TMath::Pi());
  fSectorEnd.push_back(0.52);
  fSectorStart.push_back(0.52);
  fSectorEnd.push_back(1.57);
}

void PndSttGeometryMap::GenerateStrawMapAngleGeoType1()
{
  GenerateAngles();	

  vector<int> currentRow;
  int sector = 0;
  int lastsector = 0;
  int row = 0;
  fSectorOfStraw.push_back(-1);
  fLayerOfStraw.push_back(-1);
  fAxialStraw.push_back(false);
  if(fVerbose > 0)  cout << "Generating straw map for " << fTubeArray->GetEntriesFast() << " straws." << endl;
  for (int i = 1; i < fTubeArray->GetEntriesFast(); i++) {
    if(fVerbose > 0) cout << "Finding tube." << endl;
    PndSttTube* tube = (PndSttTube*) fTubeArray->At(i);
    if(fVerbose > 0) cout << "Tube address: " << tube << endl;
    bool isaxial = (tube->GetWireDirection().Theta() < 0.001);
    if(fVerbose > 0) {
      cout << "Axial Straw: " << isaxial << endl;
      cout << "Wire Direction: ";
      tube->GetWireDirection().Print();
    }
    double phi = tube->GetPosition().Phi();
    if(fVerbose > 0) cout << "Phi: " << phi << endl;
    if (phi < 0) phi += 2*TMath::Pi();
    if(fVerbose > 0) cout << "Checking sector." << endl;
    while ( !( (phi > fSectorStart.at(sector)) && (phi < fSectorEnd.at(sector)) )
	    && !( (sector == 4) && ( (phi > fSectorStart.at(4)) || (phi < fSectorEnd.at(4)) ) ) ) {
      sector++;
      sector %= 6;
    }
    if (sector != lastsector) {
      fStrawIndex[lastsector].push_back(currentRow);
      currentRow.clear();
      if(fVerbose > 0) cout << "SECTOR COMPLETE: Row " << row << " added to sector " << lastsector << endl;
    }
    if (sector < lastsector) row++;
    lastsector = sector;
    currentRow.push_back(i);
    fSectorOfStraw.push_back(sector);
    fLayerOfStraw.push_back(row);
    fAxialStraw.push_back(isaxial);
    if(fVerbose > 0) cout << "Straw " << i << " added to " << sector << ", " << row << endl;

    tube->SetLayerID(row);
    tube->SetSectorID(sector); 
    

  }
  fStrawIndex[lastsector].push_back(currentRow);
  if(fVerbose > 0) cout << "STT COMPLETE. SECTOR COMPLETE: Row " << row << " added to sector " << lastsector << endl;

  fStrawMapInitialized = true;
}

bool PndSttGeometryMap::IsEdgeStraw(int strawindex) const
{
  if (GetRow(strawindex) == 0) return true;
  int endstraw = 0; //there is no straw index 0
  switch (GetSector(strawindex)) {
  case 0: endstraw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).front();
    break;
  case 2: endstraw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).back();
    break;
  case 3: endstraw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).front();
    break;
  case 5: endstraw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).back();
    break;
  default: return false;
  };
  return (strawindex == endstraw);
}

int PndSttGeometryMap::IsSectorBorderStraw(int strawindex) const
{
  int endstrawcw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).front();
  int endstrawccw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).back();
  if (strawindex == endstrawcw) return -1;
  if (strawindex == endstrawccw) return 1;
  return 0;
}

bool PndSttGeometryMap::IsAxialRow(int rowindex) const
{
  //only works if all straws in one row are of the same type
  //which is the case for the current geometry
  return IsAxialStraw(GetStrawRow(0, rowindex).at(0));
}

bool PndSttGeometryMap::IsSkewedRow(int rowindex) const
{
  //only works if all straws in one row are of the same type
  //which is the case for the current geometry
  return IsSkewedStraw(GetStrawRow(0, rowindex).at(0));
}


// ========================================================================
// Mixed functions
void PndSttGeometryMap::GenerateStrawMapTubeIDGeoType1()
{
  vector<int> currentRow;
  int isector = 0, ilayer = 0;
  int lastsector = 0;
  fSectorOfStraw.push_back(-1);
  fLayerOfStraw.push_back(-1);

  if(fVerbose > 0) cout << "Generating straw map for " << fTubeArray->GetEntriesFast() << " straws." << endl;
  for (int itube = 1; itube < fTubeArray->GetEntriesFast(); itube++) {
    if(fVerbose > 0)  cout << "Finding tube." << endl;
    PndSttTube* tube = (PndSttTube*) fTubeArray->At(itube);
    bool isaxial = (tube->GetWireDirection().Theta() < 0.001);
    if(fVerbose > 0)  {
      cout << "Axial Straw: " << isaxial << endl;
      cout << "Wire Direction: ";
      tube->GetWireDirection().Print();
    }
   
    while(!(itube >= fStartTube[0][ilayer] && itube <= fEndTube[5][ilayer])) {
      ilayer++;
      isector = 0;
    }

    while(!(itube >= fStartTube[isector][ilayer] && itube <= fEndTube[isector][ilayer])) {
      isector++;
      isector %= 6;
    }
    
    if (isector != lastsector) {
      fStrawIndex[lastsector].push_back(currentRow);
      currentRow.clear();
      if(fVerbose > 0) cout << "SECTOR COMPLETE: Row " << ilayer << " added to sector " << lastsector << endl;
    }
    // if (isector < lastsector) ilayer++;
    lastsector = isector;
    currentRow.push_back(itube);
    fSectorOfStraw.push_back(isector);
    fLayerOfStraw.push_back(ilayer);

    tube->SetLayerID(ilayer); 
    tube->SetSectorID(isector); 
  
    if(fVerbose > 0) cout << "Straw " << itube << " added to " << isector << ", " << ilayer << endl;
  }
  fStrawIndex[lastsector].push_back(currentRow);
  if(fVerbose > 0) cout << "SECTOR COMPLETE: Row " << ilayer << " added to sector " << lastsector << endl;

}

ClassImp(PndSttGeometryMap)
