#ifndef PNDSTTGEOMETRYMAP_H
#define PNDSTTGEOMETRYMAP_H

#include <iostream>
#include <map>
#include <vector>
#include "TObject.h"
#include "TArrayI.h"

class PndSttTubeParameters;
class PndSttTube; 
class PndGeoSttPar;
class FairGeoNode;

using std::map;
using std::vector;

class PndSttGeometryMap : public TObject
{
 public:

  PndSttGeometryMap(); 
  PndSttGeometryMap(TClonesArray *tubearray, Int_t geoType);
  ~PndSttGeometryMap();
  // ----------------------------
  void SetGeneralParameters();
  void GenerateStrawMap(Int_t map);
  Bool_t FillGeometryParameters();
  // ----------------------------

  // *** GEO TYPE 1 *** 
  void SetGeneralParametersGeoType1();
  void GenerateStrawMapGeoType1(Int_t map);
  Bool_t FillGeometryParametersGeoType1();

  void GenerateStrawMapTubeIDGeoType1();
  void GenerateStrawMapAngleGeoType1();

  int GetRow(int strawindex) const {return fLayerOfStraw.at(strawindex);}
  int GetSector(int strawindex) const {return fSectorOfStraw.at(strawindex);}
  const vector<int>& GetStrawRow(int sector, int row) const {return (fStrawIndex.find(sector))->second.at(row);}
  const vector<vector<int> >& GetStrawSector(int sector) const {return (fStrawIndex.find(sector))->second;}

  bool IsEdgeStraw(int strawindex) const;
  int IsSectorBorderStraw(int strawindex) const;

  bool IsAxialStraw(int strawindex) const {return fAxialStraw.at(strawindex);}
  bool IsSkewedStraw(int strawindex) const {return !(fAxialStraw.at(strawindex));}

  bool IsAxialRow(int rowindex) const;
  bool IsSkewedRow(int rowindex) const;

  Bool_t FindNeighborings(PndSttTube *tube);
  TArrayI GetNeighborings(int tubeId);

  // *********************

  void SetVerbose(int ver) { fVerbose = ver; }


 private:
  Int_t fGeoType, fVerbose;


  void GenerateAngles();
  map<int, vector< vector<int> > > fStrawIndex;
  vector<int> fSectorOfStraw;
  vector<int> fLayerOfStraw;
  vector<bool> fAxialStraw;
  vector<double> fSectorStart;
  vector<double> fSectorEnd;
  bool fStrawMapInitialized;


  // ------------------ SECTORS/LAYERS
  Int_t fNLayers, fNSectors, fNTubes, fNTubes_inner_parallel, fNTubes_outer_parallel, fNTubes_fillup_parallel, fNTubes_skewed, fNLayers_inner_parallel, fNLayers_skew, fNLayers_outer_parallel, fNLayers_fillup_parallel;
  Int_t **fStartTube, **fEndTube, **fShift;

  
  TClonesArray *fTubeArray;

 protected:
  
  
  ClassDef(PndSttGeometryMap,1) 
    
    };

#endif
