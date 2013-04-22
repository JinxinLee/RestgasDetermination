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

class TClonesArray;

using std::map;
using std::vector;

class PndSttStrawMap : public TObject
{
public:

	PndSttStrawMap();

	const vector<int>& GetStrawRow(int sector, int row) const {return (fStrawIndex.find(sector))->second.at(row);}
	const vector<vector<int> >& GetStrawSector(int sector) const {return (fStrawIndex.find(sector))->second;}
	int GetSector(int strawindex) const {return fSectorOfStraw.at(strawindex);}
	int FindPhiSector(double phi) const;
	int GetRow(int strawindex) const {return fRowOfStraw.at(strawindex);}
	int GetLeftSector(int sector) const {return ((sector +1) % 6);}
	int GetRightSector(int sector) const {return ((sector +5) % 6);}

	bool IsEdgeStraw(int strawindex) const;
	int IsSectorBorderStraw(int strawindex) const;

	void GenerateStrawMap(const TClonesArray* const stt_tube_array);

private:

	map<int, vector< vector<int> > > fStrawIndex;
	vector<int> fSectorOfStraw;
	vector<int> fRowOfStraw;
	bool fStrawMapInitialized;

	vector<double> fSectorStart;
	vector<double> fSectorEnd;

	ClassDef(PndSttStrawMap,1)

};


#endif /* PNDSTTSTRAWMAP_H_ */
