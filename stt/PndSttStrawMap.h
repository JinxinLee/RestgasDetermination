/*
 * PndSTTStrawMap.h
 *
 *  Created on: Apr 16, 2013
 *      Author: mertens
 */

#ifndef PNDSTTSTRAWMAP_H_
#define PNDSTTSTRAWMAP_H_

#include <map>
#include <vector>

#include "TObject.h"
#include "TClonesArray.h"

class PndSttTube;

using std::map;
using std::vector;

class PndSttStrawMap : public TObject
{
public:

	PndSttStrawMap();
	PndSttStrawMap(TClonesArray* const stt_tube_array);
  PndSttStrawMap(const PndSttStrawMap& o) :
	  fStrawIndex(o.fStrawIndex),
	  fSectorOfStraw(o.fSectorOfStraw),
	  fRowOfStraw(o.fRowOfStraw),
	  fAxialStraw(o.fAxialStraw),
	  fStrawMapInitialized(o.fStrawMapInitialized),
	  fTubeArray(o.fTubeArray),
	  fSectorStart(o.fSectorStart),
	  fSectorEnd(o.fSectorEnd)
  {};
  PndSttStrawMap& operator=(const PndSttStrawMap& o)
  {
	  fStrawIndex=o.fStrawIndex;
	  fSectorOfStraw=o.fSectorOfStraw;
	  fRowOfStraw=o.fRowOfStraw;
	  fAxialStraw=o.fAxialStraw;
	  fStrawMapInitialized=o.fStrawMapInitialized;
	  fTubeArray=o.fTubeArray;
	  fSectorStart=o.fSectorStart;
	  fSectorEnd=o.fSectorEnd;
  };

	const vector<int>& GetStrawRow(int sector, int row) const {return (fStrawIndex.find(sector))->second.at(row);}
	const vector<vector<int> >& GetStrawSector(int sector) const {return (fStrawIndex.find(sector))->second;}
	int GetSector(int strawindex) const {return fSectorOfStraw.at(strawindex);}
	int FindPhiSector(double phi) const;
	int GetRow(int strawindex) const {return fRowOfStraw.at(strawindex);}
	int GetLeftSector(int sector) const {return ((sector +1) % 6);}
	int GetRightSector(int sector) const {return ((sector +5) % 6);}

	bool IsEdgeStraw(int strawindex) const;
	int IsSectorBorderStraw(int strawindex) const;

	bool IsAxialStraw(int strawindex) const {return fAxialStraw.at(strawindex);}
	bool IsSkewedStraw(int strawindex) const {return !(fAxialStraw.at(strawindex));}

	bool IsAxialRow(int rowindex) const;
	bool IsSkewedRow(int rowindex) const;

	PndSttTube* GetTube(int strawindex) const {return (PndSttTube*)fTubeArray->At(strawindex);}

	void GenerateStrawMap(TClonesArray* const stt_tube_array);

private:

	void GenerateAngles();

	map<int, vector< vector<int> > > fStrawIndex;
	vector<int> fSectorOfStraw;
	vector<int> fRowOfStraw;
	vector<bool> fAxialStraw;
	bool fStrawMapInitialized;

	TClonesArray* fTubeArray;

	vector<double> fSectorStart;
	vector<double> fSectorEnd;

	ClassDef(PndSttStrawMap,1)

};


#endif /* PNDSTTSTRAWMAP_H_ */
