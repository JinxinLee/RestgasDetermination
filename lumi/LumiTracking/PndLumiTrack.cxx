/*
 * PndLumiTrack.cxx
 *
 *  Created on: Dec 6, 2009
 *      Author: tsito
 */
#include "PndLumiTrack.h"

PndLumiTrack::PndLumiTrack()
{

}

PndLumiTrack::PndLumiTrack(Int_t mctrkid, std::vector<TVector3> trck,std::vector<TVector3> hiterr)
{
	fMCId = mctrkid;
	fTrackCandidates = trck;
	fTrackCandErrors = hiterr;
}

PndLumiTrack::~PndLumiTrack()
{

}

void PndLumiTrack::Print(const Option_t* opt) const
{

	if(fTrackCandidates.size()!=0){
		for(std::vector<TVector3> ::size_type l = 0; l<fTrackCandidates.size(); l++){
			cout<< "( "<<fTrackCandidates[l].X()<<" , "<<fTrackCandidates[l].Y()<<" , "<<fTrackCandidates[l].Z()<<" )";
				cout<< " +/- ( "<< fTrackCandErrors[l].X()<<" , "<<fTrackCandErrors[l].Y()<<" , "<<fTrackCandErrors[l].Z()<<" )."<<endl;

		}
	}



}
ClassImp(PndLumiTrack)
