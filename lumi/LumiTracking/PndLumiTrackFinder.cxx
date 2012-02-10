#include "PndLumiTrackFinder.h"
#include "PndLumiTransposition.h"

#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TF1.h"

#include "TDatabasePDG.h"
#include "TParticle.h"

#include <exception>

PndLumiTrackFinder::PndLumiTrackFinder() :
	FairTask("LUMI Tracking Task ")
{
	fn = 0;
	fMom= 0;
	fEps= 0;
	fPow =0.0;
	fVerbose = 0;
	fUseHough = false;
}

PndLumiTrackFinder::PndLumiTrackFinder(Double_t nsig, Double_t mom, Double_t thick,
		Double_t pow, Int_t iverb, bool usehough)
{
	fn = nsig;
	fMom= mom;
	fEps= thick;
	fPow = pow;
	fVerbose = iverb;
	fUseHough = usehough;
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

	std::map<Int_t,TVector3> hitmap;
	hitmap.clear();

	std::map<Int_t,Int_t> detIdmap;
	detIdmap.clear();

	std::map<Int_t,Int_t> mcidmap;
	mcidmap.clear();

	TString detname;
	PndLumiHit *recohit = NULL;

	Int_t mcid;

	Int_t nHits = fLumiHit->GetEntriesFast();
	TVector3 hit;
	Int_t detId;

	for (Int_t iHit = 0; iHit < nHits; iHit++){
		recohit = (PndLumiHit*)fLumiHit->At(iHit);
		detname = recohit->GetDetName();
		hit=recohit->GetPosition();
		mcid= recohit->GetRefIndex();
		detId = recohit->GetDetectorID();
		hitmap[iHit]=hit;
		mcidmap[iHit]=mcid;
		detIdmap[iHit]=detId;
	}
	Int_t itrk = fLumiTrack->GetEntriesFast();
	Bool_t test = IsGoodCollection(detIdmap);

	if(test){
		//             Do track finder
		//---------------------------------------

		std::vector<std::vector<Int_t> > trackIds;
		trackIds.clear();
		if(fUseHough){
			cout<<"******** Find track by using Hough transformation ******* "<<endl;
			trackIds = gethitsontrackbyangle(detIdmap, hitmap);
		}else{
			cout<<"******** Find track in Cartesian coordinates ******* "<<endl;
			trackIds=gethitsontrackbycoord(detIdmap,hitmap);
		}

		std::vector<TVector3> onetrack;
		Int_t index;
		std::vector<TVector3> hiterrors;

		for(std::vector<std::vector<Int_t> >::size_type u = 0; u<trackIds.size(); u++){
			index = trackIds[u][0];
			for (std::vector<Int_t>::size_type v = 0; v<trackIds[u].size(); v++){
				TVector3 tmp = hitmap.find(trackIds[u][v])->second;
				onetrack.push_back(tmp);
			}
			new((*fLumiTrack)[itrk] ) PndLumiTrack(index, onetrack, hiterrors);
			onetrack.clear();
		}
	}

	Print();
	return;
}

std::vector<std::vector<Int_t> > PndLumiTrackFinder::
gethitsontrackbyangle(std::map<Int_t,Int_t> detIdmap, std::map<Int_t,TVector3> hitmap)
{

	std::map<Int_t,TVector3> hitmap_2;

	std::map<Int_t,TVector3>::iterator ithit;
	TVector3 zdir(0,0,1);
	for(ithit=hitmap.begin(); ithit!=hitmap.end(); ++ithit){
		TVector3 tmp;
		tmp = doHitTransf((*ithit).second);
		tmp.Rotate(TMath::Pi()/4.0,zdir);
		hitmap_2[(*ithit).first]=tmp;
	}

	Double_t theta = getMultiScatterAngle();

	std::vector<std::vector<Int_t> > tracks;
	tracks.clear();

	std::vector<Int_t> firsts;
	std::vector<Int_t> seconds;
	std::vector<Int_t> thirds;
	std::vector<Int_t> forths;

	for (std::map<Int_t,Int_t>::iterator itid = detIdmap.begin(); itid != detIdmap.end(); ++itid){
		Int_t detId;
		detId = (*itid).second;
		if(((detId-1)/4)==0) firsts.push_back((*itid).first);
		else if(((detId-1)/4)==1) seconds.push_back((*itid).first);
		else if(((detId-1)/4)==2) thirds.push_back((*itid).first);
		else forths.push_back((*itid).first);
	}

	TVector3 xfirst, xsecond, xthird, xforth;
	std::vector<Int_t> oneCand;
	Double_t theta_0;

	for(Int_t ifirst=0; ifirst<firsts.size(); ifirst++){
		std::map<Int_t,TVector3>::iterator itfirst;
		itfirst =hitmap_2.find(firsts[ifirst]);
		xfirst = (*itfirst).second;

		oneCand.clear();
		oneCand.push_back(firsts[ifirst]);

		for(Int_t iforth=0; iforth<forths.size(); iforth++){
			std::map<Int_t,TVector3>::iterator itforth;
			itforth =hitmap_2.find(forths[iforth]);
			xforth = (*itforth).second;

			oneCand.push_back(forths[iforth]);
			TVector3 u14 = (xforth-xfirst).Unit();

			for(Int_t isec=0; isec<seconds.size(); isec++){
				std::map<Int_t,TVector3>::iterator itsec;
				itsec =hitmap_2.find(seconds[isec]);
				xsecond = (*itsec).second;
				TVector3 u12 = (xsecond-xfirst).Unit();
				theta_0 = acos(u12.Dot(u14));
				if(theta_0<=fn*theta) oneCand.push_back(seconds[isec]);
			}

			for(Int_t ithr=0; ithr<thirds.size(); ithr++){
				std::map<Int_t,TVector3>::iterator itthr;
				itthr =hitmap_2.find(seconds[ithr]);
				xthird = (*itthr).second;

				TVector3 u34 = (xforth-xthird).Unit();
				theta_0 = acos(u14.Dot(u34));
				if(theta_0<=fn*theta) oneCand.push_back(thirds[ithr]);
			}
		}
	}
	tracks.push_back(oneCand);
	return tracks;
}

std::vector<std::vector<Int_t> > PndLumiTrackFinder::
gethitsontrackbycoord(std::map<Int_t,Int_t> detIdmap, std::map<Int_t,TVector3> hitmap)
{

	std::vector<std::vector<Int_t> > tracks;
	tracks.clear();

	std::vector<Int_t> firsts;
	std::vector<Int_t> lasts;
	std::vector<Int_t> middles;
	for (std::map<Int_t,Int_t>::iterator itid = detIdmap.begin(); itid != detIdmap.end(); ++itid){
		Int_t detId;
		detId = (*itid).second;
		if(((detId-1)/4)==0) firsts.push_back((*itid).first);
		else if(((detId-1)/4)==3) lasts.push_back((*itid).first);
		else middles.push_back((*itid).first);
	}

	Double_t xfirst, xlast;
	Double_t yfirst, ylast;
	Double_t zfirst, zlast;
	Double_t sx, sy;
	std::vector<Int_t> oneCand;

	for(Int_t ifirst=0; ifirst<firsts.size(); ifirst++){
		std::map<Int_t,TVector3>::iterator itfirst;
		itfirst =hitmap.find(firsts[ifirst]);
		xfirst = (*itfirst).second.X();
		yfirst = (*itfirst).second.Y();
		zfirst = (*itfirst).second.Z();

		oneCand.clear();
		oneCand.push_back(firsts[ifirst]);

		for(Int_t ilast=0; ilast<lasts.size(); ilast++){
			std::map<Int_t,TVector3>::iterator itlast;
			itlast =hitmap.find(lasts[ilast]);
			xlast = (*itlast).second.X();
			ylast = (*itlast).second.Y();
			zlast = (*itlast).second.Z();
			oneCand.push_back(lasts[ilast]);

			sx=(xlast-xfirst)/(zlast-zfirst);
			sy=(ylast-yfirst)/(zlast-zfirst);

			for(Int_t imid=0; imid<middles.size(); imid++){
				std::map<Int_t,TVector3>::iterator itmid;
				itmid =hitmap.find(middles[imid]);
				TVector3 pos = (*itmid).second;
				Double_t dx =fabs(-pos.X() + xfirst +sx*(pos.Z()-zfirst)) ;
				Double_t dy =fabs(-pos.Y() + yfirst +sy*(pos.Z()-zfirst));

				Double_t theta = getMultiScatterAngle();
				Int_t k = fabs(pos.Z()-zlast)<fabs(pos.Z()-zfirst)?1:-1;
				Double_t Dx;
				if(k>0) Dx=fabs(pos.Z()-zfirst);
				if(k<0) Dx=fabs(pos.Z()-zlast);
				Double_t win = fn*Dx*tan(theta);
				if(dx<=win && dy<=win){
					oneCand.push_back(middles[imid]);
				}
			}
		}

	}
	tracks.push_back(oneCand);
	return tracks;
}

void PndLumiTrackFinder::Print() const
{
	Int_t nTracks = fLumiTrack->GetEntriesFast();

	cout << "-I- PndLumiTrackFinder: " << nTracks << " tracks are reconstructed"<<endl;

    if (fVerbose > 0){
    	for (Int_t i=0; i < nTracks; i++){
    		cout << " ** Track Candidate No "<<i+1<<" : "<<endl;
	    (*fLumiTrack)[i]->Print();
    	}
    }
}

bool PndLumiTrackFinder::IsGoodCollection(std::map<Int_t,Int_t> detIdmap)
{
	std::vector<Int_t> detId;
	Int_t planeId;
	for(std::map<Int_t,Int_t>::iterator it = detIdmap.begin(); it!=detIdmap.end(); ++it){
		planeId=int(((*it).second)-1)/4;
		detId.push_back(planeId);
	}

	if(detId.size()<4){
		return false;
	}
	else{

		sort(detId.begin(), detId.end());

		std::vector<Int_t>::iterator new_end_pos;
		new_end_pos = unique(detId.begin(), detId.end());
		detId.erase(new_end_pos, detId.end());

		if(detId.size()!=4) {
			return false;
		}
	}

	return true;
}

TVector3 PndLumiTrackFinder::doHitTransf(TVector3& hit)
{
	TVector3 result;
	Double_t z_dip = 477.86; //cm
	Double_t z0 = hit.Z()-z_dip;
	TVector3 vec(0.0, 1.0, 0.0);
	hit.SetZ(z0);
	hit.Rotate(-fPow, vec);
	result.SetXYZ(hit.X(),hit.Y(),hit.Z()+z_dip);
	return result;
}

Double_t PndLumiTrackFinder::getMultiScatterAngle()
{
	Double_t sig;
	Double_t x = fEps*2.329/21.82;
	sig = 13.6e-3*sqrt(x)*(1+0.038*log(x))/fMom;
	return sig;
}

ClassImp(PndLumiTrackFinder)



