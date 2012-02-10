/*
 * PndLumiTrack.h
 *
 *  Created on: Dec 6, 2009
 *      Author: tsito
 */

#ifndef PNDLUMITRACK_H_
#define PNDLUMITRACK_H_

#include "TObject.h"
#include "TVector3.h"

#include <iostream>
#include <vector>


using std::cout;
using std::endl;

class PndLumiTrack: public TObject
{
public:

	/** Default Constructor */
	PndLumiTrack();

	PndLumiTrack(Int_t mctrkid, std::vector<TVector3>& trck,std::vector<TVector3>& hiterr);
	PndLumiTrack(std::vector<Int_t>& hitids, std::vector<TVector3>& trck,std::vector<TVector3>& hiterr);


	std::vector<Int_t> GetHitIndex() const{return fHitIds;}
	Int_t GetMCIndex() const{return fMCId;}
	std::vector<TVector3> GetTrackCandidates() const{return fTrackCandidates;}
	std::vector<TVector3> GetTrackCandErrors() const{return fTrackCandErrors;}
	std::vector<Double_t> GetTrackXPosition() const;
	std::vector<Double_t> GetTrackYPosition() const;
	std::vector<Double_t> GetTrackZPosition() const;
	Int_t GetNTrackCand() const{return fTrackCandidates.size();}

	/** Destructor */
	virtual ~PndLumiTrack();

	void Print(const Option_t* opt) const;


private:
	Int_t fMCId;
	std::vector<Int_t> fHitIds;
	std::vector<TVector3> fTrackCandidates;
	std::vector<TVector3> fTrackCandErrors;

	ClassDef(PndLumiTrack,2);

};

#endif /* PNDLUMITRACK_H_ */
