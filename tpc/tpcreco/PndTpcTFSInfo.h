#ifndef PndTpcTFSInfo_H
#define PndTpcTFSInfo_H

#include <vector>
class TClonesArray;

class PndTpcTFSInfo
{
public: 
	PndTpcTFSInfo(TClonesArray *trackArray, TClonesArray *trackFitStatArray):_trackArray(trackArray), _trackFitStatArray(trackFitStatArray), bTrackFitStatInfoAvailable(false), _pdgselect(false) {}
	void LoadTrackFitStatInfo();	
	bool CheckPdg(int TrackNr) const;
	int GetPdg(int TrackNr) const	{ return ( isPDGokay.size()>TrackNr ? isPDGokay[TrackNr] : 0 ); }
	int GetMotherID(int TrackNr) const	{ return ( MotherIds.size()>TrackNr ? MotherIds[TrackNr] : -2 ); }
	double GetMCP(int TrackNr) const	{ return ( mcps.size()>TrackNr ? mcps[TrackNr] : -100. ); }
private:
     TClonesArray *_trackArray;
	TClonesArray *_trackFitStatArray;
	bool bTrackFitStatInfoAvailable;
	bool _pdgselect;
	std::vector<int> isPDGokay;
	std::vector<int> MotherIds;
	std::vector<double> mcps; 	
};
#endif
