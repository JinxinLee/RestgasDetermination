#include "PndTpcTFSInfo.h"
#include "TClonesArray.h"
#include "TrackFitStat.h"
#include <iostream>
#include <assert.h>

using namespace std;

void PndTpcTFSInfo::LoadTrackFitStatInfo()
{
	//Get inforamtion out of TrackFitStat
	int nTrack=_trackArray->GetEntriesFast();
	if(_trackFitStatArray != 0 )	{
		int nStat=_trackFitStatArray->GetEntriesFast();
		cout << "PndTpcRecoDEdxTask::Exec: " << "_trackFitStatArray - Number of Tracks: " << nStat << endl;
		for(int i=0; i<nStat; i++)
		{
			TrackFitStat *pFitStat=(TrackFitStat*)_trackFitStatArray->At(i);
			isPDGokay.push_back(pFitStat->GetPdgCode());
			MotherIds.push_back(pFitStat->GetMotherID());
			mcps.push_back(pFitStat->GetMCP());
		}
		bTrackFitStatInfoAvailable = true;
	}
	else	 {
		cout << "PndTpcRecoDEdxTask::Exec: " << "TrackFitStat not available!" << endl;
		bTrackFitStatInfoAvailable = false;
	}
	if(!(isPDGokay.size()==nTrack))	{
		cout << "PndTpcRecoDEdxTask::Exec: " << "Error: Number of tracks doesn't match number of FitStats" << endl;
		if(_pdgselect)	{
			cout << "PndTpcRecoDEdxTask::Exec: " << "No PDG Selection will be performed!" << endl;
		}
		bTrackFitStatInfoAvailable = false;
	}
}

bool PndTpcTFSInfo::CheckPdg(int TrackNr) const
{
	if(bTrackFitStatInfoAvailable)	{
		assert(isPDGokay.size()>TrackNr);
		if(!isPDGokay[TrackNr] && _pdgselect )	{
			return false;
		}
	}
	return true;
}
