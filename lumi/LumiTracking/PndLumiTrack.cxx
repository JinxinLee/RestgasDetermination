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

PndLumiTrack::PndLumiTrack(Int_t mctrkid, std::vector<TVector3>& trck,std::vector<TVector3>& hiterr)
{                                                                                                                                             
  fMCId = mctrkid;                                                                                                                       
  fTrackCandidates = trck;                                                                                                               
  fTrackCandErrors = hiterr;                                                                                                             
}  

PndLumiTrack::PndLumiTrack(std::vector<Int_t>& hitids, std::vector<TVector3>& trck,std::vector<TVector3>& hiterr)
{
	fHitIds = hitids;
	fTrackCandidates = trck;
	fTrackCandErrors = hiterr;
}

PndLumiTrack::~PndLumiTrack()
{

}
std::vector<Double_t> PndLumiTrack::GetTrackXPosition() const
{
	std::vector<Double_t> x;
	for(int i =0; i< fTrackCandidates.size() ;i++){
			x.push_back(fTrackCandidates[i].X());
	}
			return x;
}

std::vector<Double_t> PndLumiTrack::GetTrackYPosition() const
{
		std::vector<Double_t> x;
			for(int i =0; i< fTrackCandidates.size() ;i++){
					x.push_back(fTrackCandidates[i].Y());
			}
			return x;


}

std::vector<Double_t> PndLumiTrack::GetTrackZPosition() const
{

	std::vector<Double_t> x;
			for(int i =0; i< fTrackCandidates.size() ;i++){
					x.push_back(fTrackCandidates[i].Z());
			}
			return x;

}



void PndLumiTrack::Print(const Option_t* opt) const
{

	if(fTrackCandidates.size()!=0){
//		cout<<" Track Index : "<<fMCId<<endl;
		for(std::vector<TVector3> ::size_type l = 0; l<fTrackCandidates.size(); l++){
			cout<< "( "<<fTrackCandidates[l].X()<<" , "<<fTrackCandidates[l].Y()<<" , "<<fTrackCandidates[l].Z()<<" )"<<endl;
//				cout<< " +/- ( "<< fTrackCandErrors[l].X()<<" , "<<fTrackCandErrors[l].Y()<<" , "<<fTrackCandErrors[l].Z()<<" )."<<endl;

		}
	}

}


ClassImp(PndLumiTrack)
