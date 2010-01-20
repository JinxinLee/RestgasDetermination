#include <math.h>

#include "PndLumiTrackFinder.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TF1.h"


#include "TDatabasePDG.h"
#include "TParticle.h"


PndLumiTrackFinder::PndLumiTrackFinder() :
	FairTask("Simple LUMI Tracking Task ")
{
	fDTheta = -1.;
	fDPhi = -1.;
	fVerbose = 0;
}

PndLumiTrackFinder::PndLumiTrackFinder(Double_t dth, Double_t dph, Int_t iverb)
{
	fDTheta = dth;
	fDPhi = dph;
	fVerbose = iverb;
}


PndLumiTrackFinder::~PndLumiTrackFinder()
{

}


InitStatus PndLumiTrackFinder::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ){
		cout << "-E- PndLumiTrackFinder::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get input array
	fLumiPoint = (TClonesArray*) ioman->GetObject("LumiPoint");
	if (!fLumiPoint){
		cout << "-W- PndLumiTrackFinder::Init: "
		<< "No LumiPoint collection!" << endl;
		return kERROR;
	}

	fLumiMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
	if (!fLumiPoint){
		cout << "-W- PndLumiTrackFinder::Init: "
		<< "No MCTrack collection!" << endl;
		return kERROR;
	}

	fLumiHit = (TClonesArray*) ioman->GetObject("LumiHit");
	if (!fLumiHit){
		cout << "-W- PndLumiTrackFinder::Init: "
		<< "No LumiHit collection!" << endl;
		return kERROR;
	}

	// Create and register output array
	fLumiTrack = new TClonesArray("PndLumiTrack");
	ioman->Register("LumiTrack", "Lumi", fLumiTrack, kTRUE);

	return kSUCCESS;

}


void PndLumiTrackFinder::Exec(Option_t* opt)
{
	if ( ! fLumiTrack )
		Fatal("Exec", "No Lumi Track Candidate");
	fLumiTrack->Delete();



	Double_t x, y, z;
	Double_t dx, dy, dz;
	Double_t theta, phi;

	TVector2 ThetaAndPhi;
	std::map<Int_t,TVector2> anglemap;
	anglemap.clear();

	std::map<Int_t,TVector3> hitmap;
	hitmap.clear();

	std::map<Int_t,TVector3> errormap;
	errormap.clear();

	std::map<Int_t,Int_t> mcidmap;
	mcidmap.clear();

	TString detname;
	TVector3 hit, hitErr, error;

	PndLumiHit *MyHit = NULL;

	Int_t nHits = fLumiHit->GetEntriesFast();

	for (Int_t iHit = 0; iHit < nHits; iHit++){
		MyHit = (PndLumiHit*)fLumiHit->At(iHit);

		detname = MyHit->GetDetName();
		x = MyHit->GetX(); y = MyHit->GetY(); z = MyHit->GetZ();
		dx = MyHit->GetDx(); dy = MyHit->GetDy(); dz = MyHit->GetDz();

		hit.SetXYZ(x,y,z);
		ThetaAndPhi.Set(hit.Theta(),hit.Phi());

		GetErrorsPerHit(MyHit->GetRefIndex(), error);

		hitErr.SetXYZ(dx+error.X(), dy+error.Y(), dz+error.Z());

		anglemap[iHit]=ThetaAndPhi;
		hitmap[iHit]=hit;
		errormap[iHit]= hitErr;
		mcidmap[iHit]=MyHit->GetRefIndex();

	}


	vector<Int_t> oneclust;
	vector<vector<Int_t> > track;
	track.clear();
	track = GetHitsOnTrack(anglemap);

	vector<TVector3> onetrack;
	vector<vector<TVector3> > TrackCandidates;
	TrackCandidates.clear();

	vector<TVector3> errorvec;
	vector<vector<TVector3> > ErrorsOnTrackHits;
	TrackCandidates.clear();

	vector<Int_t> mcidvec;
	vector<vector<Int_t> > allMCId;
	allMCId.clear();


	for(vector<vector<Int_t> >::size_type u = 0; u<track.size(); u++){
		for (vector<Int_t>::size_type v = 0; v<track[u].size(); v++){
			onetrack.push_back(hitmap.find(track[u][v])->second);
			errorvec.push_back(errormap.find(track[u][v])->second);
			mcidvec.push_back(mcidmap.find(track[u][v])->second);
		}
		TrackCandidates.push_back(onetrack);
		ErrorsOnTrackHits.push_back(errorvec);
		allMCId.push_back(mcidvec);
		onetrack.clear();
		errorvec.clear();
		mcidvec.clear();
	}

	for(vector<vector<TVector3> >::size_type l = 0; l<TrackCandidates.size(); l++){

		PndLumiPoint *mcPoint = (PndLumiPoint*)fLumiPoint->At(allMCId[l][0]);
		Int_t iTrack = mcPoint->GetTrackID();


		new((*fLumiTrack)[l] ) PndLumiTrack(iTrack, TrackCandidates[l], ErrorsOnTrackHits[l]);
	}

	Print();
}


vector<vector<Int_t> > PndLumiTrackFinder::GetHitsOnTrack(map<Int_t,TVector2> anglemap)
{
	map<Int_t,Int_t> flag;
	flag.clear();
	vector<Int_t> one3Dline;
	vector<vector<Int_t> > alltracks;
	alltracks.clear();
	map<Int_t,TVector2>::iterator tmp;

	for (map<Int_t,TVector2>::iterator it = anglemap.begin(); it!= anglemap.end() ; ++it){
		flag[(*it).first] = 1;
	}

	for (map<Int_t,TVector2>::iterator itAngle = anglemap.begin(); itAngle != anglemap.end(); ++itAngle){
		tmp = itAngle;

		if( flag[(*itAngle).first]!=1 ) continue;

		for (map<Int_t,TVector2>::iterator itAngle2 = itAngle; itAngle2!= anglemap.end() ; ++itAngle2){

			if( flag[(*itAngle2).first]!=1 ) continue;

			Int_t th_tmp = ceil((*tmp).second.X()/fDTheta) ;
			Int_t th = ceil((*itAngle2).second.X()/fDTheta) ;
			Int_t ph_tmp = ceil((*tmp).second.Y()/fDPhi);
			Int_t ph = ceil((*itAngle2).second.Y()/fDPhi) ;


			if(fabs(th_tmp-th)<=1 && fabs(ph_tmp-ph)<=1){
				one3Dline.push_back((*itAngle2).first);
				flag[(*itAngle2).first]=-1;
				tmp = itAngle2;
			}
		}
		alltracks.push_back(one3Dline);
		one3Dline.clear();

	}

	return alltracks;

}

void PndLumiTrackFinder::GetErrorsPerHit(Int_t mcindex, TVector3& Err)
{
	PndLumiPoint *mcPoint = (PndLumiPoint*)fLumiPoint->At(mcindex);
	Int_t iTrack = mcPoint->GetTrackID();
	PndMCTrack *mctrack = (PndMCTrack*)fLumiMCTrack->At(iTrack);

	Double_t p = mcPoint->GetMomentum().Mag();

	Double_t eps = mcPoint->GetExitZ()-mcPoint->GetEntryZ();

	Int_t PDGCode = mctrack->GetPdgCode();
	TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
	TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
	Double_t Q= fParticle->Charge()/3.;

	Double_t t_rms = 13.6*Q*TMath::Sqrt(0.11*eps)*(1+0.038*TMath::Log(0.11*eps))/(p*1E3*TMath::Sqrt(2.));


	PndLumiPoint *mcPointref = (PndLumiPoint*)fLumiPoint->At(0);

	Double_t z = 0.5*(mcPoint->GetExitZ()+mcPoint->GetEntryZ())-
	0.5*(mcPointref->GetExitZ()+mcPointref->GetEntryZ());

	Double_t x_rms = z*TMath::Tan(t_rms)/TMath::Sqrt(2.);
	Double_t y_rms = x_rms;
	Double_t z_rms = 0.0;

	Err.SetXYZ(x_rms, y_rms, z_rms);
}

void PndLumiTrackFinder::Print() const
{
	Int_t nTracks = fLumiTrack->GetEntriesFast();

	cout << "-I- PndLumiTrackFinder: " << nTracks << " tracks are reconstructed"<<endl;

    if (fVerbose > 1){
    	for (Int_t i=0; i < nTracks; i++){
    		cout << " - "<<i+1<<"th group of hits : "<<endl;
	    (*fLumiTrack)[i]->Print();
    	}
    }
}

ClassImp(PndLumiTrackFinder)



