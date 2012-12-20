#include "PndMvdRiemannTrackFinder.h"
#include <iostream>
#include <math.h>
#include "PndSdsHit.h"
#include "TString.h"

PndMvdRiemannTrackFinder::PndMvdRiemannTrackFinder():PndRiemannTrackFinder(),fZClosePar(0.1)
{
	fLayers.resize(13);
	if (fUseZeroPos)
		fLayers[0].push_back(0);
	fGeoH = PndGeoHandling::Instance();
	fVerbose = 3;
}

PndMvdRiemannTrackFinder::~PndMvdRiemannTrackFinder(){}

void PndMvdRiemannTrackFinder::AddHits(TClonesArray* hits, Int_t branchId)
{
	TString geoPath;
  //	PndRiemannTrackFinder::AddHits(hits, branchId);
	for (int i = 0; i < hits->GetEntries(); i++){
		FairHit* myHit = (FairHit*)(hits->At(i));
		fHits.push_back(myHit);
		FairLink myID;
		if (myHit->GetEntryNr().GetIndex() < 0){
			myID = FairLink(branchId, i);
		}
		else
			myID = myHit->GetEntryNr();
		fMapHitToID[fHits.size()-1]=myID;
		fMapIDtoHit[myID] = fHits.size()-1;
		PndSdsHit* tempHit=(PndSdsHit*)(hits->At(i));
    
    
		geoPath=fGeoH->GetPath(tempHit->GetSensorID());
    
		//std::cout << "Point " << branchId << "/" << i << " : " << geoPath << std::endl;
    
		int Layer=0;
		bool flag=true;
    ///////////////////////	getting layer's information
    if (flag && geoPath.Contains("PixeloBlo1")){Layer=1;flag=false;}
		if (flag && geoPath.Contains("PixeloSdko(Silicon)_1")){Layer=2;flag=false;}  //naming after MVD2.2
		if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_1")){Layer=2;flag=false;}
    if (flag && geoPath.Contains("PixeloSdko(Silicon)_2")){Layer=3;flag=false;}  //naming after MVD2.2
    if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_2")){Layer=3;flag=false;}
		if (flag && geoPath.Contains("PixeloSdko(Silicon)_3")){Layer=4;flag=false;}  //naming after MVD2.2
		if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_3")){Layer=4;flag=false;}
		if (flag && geoPath.Contains("PixeloSdko(Silicon)_4")){Layer=5;flag=false;}  //naming after MVD2.2
		if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_4")){Layer=5;flag=false;}
		if (flag && geoPath.Contains("PixeloBlo2")){Layer=6;flag=false;}
    if (flag && geoPath.Contains("PixeloLdkoio(Silicon)_1")){Layer=7;flag=false;}  //naming after MVD2.2
    if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_1")){Layer=7;flag=false;}
		if (flag && geoPath.Contains("PixeloLdkoiio(Silicon)_1")){Layer=8;flag=false;}  //naming after MVD2.2
		if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_2")){Layer=8;flag=false;}
		if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_1")){Layer=9;flag=false;}  //naming after MVD2.2
		if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_3")){Layer=9;flag=false;}
    if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_2")){Layer=10;flag=false;}  //naming after MVD2.2
    if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_4")){Layer=10;flag=false;}
		if (flag && geoPath.Contains("PixeloLdkoiio(Silicon)_2")){Layer=11;flag=false;}  //naming after MVD2.2
		if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_5")){Layer=11;flag=false;}
    if (flag && geoPath.Contains("PixeloLdkoio(Silicon)_2")){Layer=12;flag=false;}  //naming after MVD2.2
    if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_6")){Layer=12;flag=false;}
    if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_4")){Layer=13;flag=false;}  //naming after MVD2.2
    if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_7")){Layer=13;flag=false;}
		if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_3")){Layer=14;flag=false;}  //naming after MVD2.2
		if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_8")){Layer=14;flag=false;}
    
		if (flag && geoPath.Contains("StripoBl3o(Silicon)")){Layer=15;flag=false;}
		if (flag && geoPath.Contains("Fwdo(Silicon)_1")){Layer=16;flag=false;}
		if (flag && geoPath.Contains("StripoBl4o(Silicon)")){Layer=17;flag=false;}
		//if (flag && geoPath.Contains("Fwdo(Silicon)_2")){Layer=10;flag=false;}
    
		if (flag && geoPath.Contains("StripoLdkoTrapSoRingAoSilicon_1")){Layer=18;flag=false;}
		if (flag && geoPath.Contains("StripoLdkoTrapSoRingAoSilicon_2")){Layer=19;flag=false;}
		if (flag && geoPath.Contains("StripoLdkoTrapSoRingBoSilicon_1")){Layer=18;flag=false;}
		if (flag && geoPath.Contains("StripoLdkoTrapSoRingBoSilicon_2")){Layer=19;flag=false;}
		if (flag && geoPath.Contains("StripoLdko5-6oTrapSo(Silicon)_1")){Layer=20;flag=false;}
    
    //    	if (flag && geoPath.Contains("PixeloBl1ov2-NEW_1")){Layer=1;flag=false;}
    //		if (flag && geoPath.Contains("PixeloSdk-v2-NEW_1")){Layer=2;flag=false;}
    //		if (flag && geoPath.Contains("PixeloSdk-v2-NEW_2")){Layer=3;flag=false;}
    //		if (flag && geoPath.Contains("PixeloBl2ov3-NEW_1")){Layer=4;flag=false;}
    //		if (flag && geoPath.Contains("PixeloMdkov1-NEW_1")){Layer=5;flag=false;}
    //		if (flag && geoPath.Contains("PixeloMdkov1-NEW_2")){Layer=6;flag=false;}
    //		if (flag && geoPath.Contains("StripoBl3_1")){Layer=7;flag=false;}
    //		if (flag && geoPath.Contains("PixeloMdkov1-NEW_3")){Layer=8;flag=false;}
    //		if (flag && geoPath.Contains("StripoLdk_1")){Layer=8;flag=false;}
    //		if (flag && geoPath.Contains("StripoBl4_1")){Layer=9;flag=false;}
    //		if (flag && geoPath.Contains("PixeloMdkov1-NEW_4")){Layer=10;flag=false;}
    //		if (flag && geoPath.Contains("StripoLdk_2")){Layer=10;flag=false;}
    ////////////////////////////
		fNLayers = fLayers.size();
		while (fNLayers < Layer+1){
			std::vector<int> dummy;
			fLayers.push_back(dummy);
			fNLayers = fLayers.size();
		}
		if (Layer == 0){
			std::cout << "-E- Unassigned Layer: " << geoPath << std::endl;
		}
    
		if (fVerbose > 1) std::cout << "fMapHitToId: " << fHits.size() -1 << " : " << myID << " "
      << tempHit->GetX() << "/" << tempHit->GetY() << "/" << tempHit->GetZ() << " Layer: " << Layer << std::endl;
		fLayers[Layer].push_back(fHits.size()-1);  //putting hit in layers array
	}
	fNLayers = fLayers.size();
}

void PndMvdRiemannTrackFinder::FindTracks()
{
	std::vector<std::vector<Int_t> > Tracks = GetStartTracks();				//Get the possible track seeds
  std::vector<int> tooClose;
  fTracks.clear();
  fTrackCand.clear();
	for (unsigned int trackId = 0; trackId < Tracks.size(); trackId++){				//Go through all track seeds and search for additional points
    
		if (Tracks[trackId].size() != 3)
			std::cout << "-E- PndMVDRiemannTrackFinder::FindTracks: Start points: " << Tracks[trackId].size()
      << " in Track: " << trackId << std::endl;
    
		std::vector<Int_t> StartTrack = Tracks[trackId];
    
		if (fVerbose > 1){
			std::cout << "------------------------------------" << std::endl;
			std::cout << "Start Plane from Points: " << fMapHitToID[StartTrack[0]] << " "
					  << fMapHitToID[StartTrack[1]] << " "  << fMapHitToID[StartTrack[2]] << std::endl;
		}
		if (TrackExists(StartTrack) == true){
			if (fVerbose > 1) std::cout << "Track exists already!" << std::endl;
			continue;
		}
		PndRiemannTrack actTrack = CreateRiemannTrack(StartTrack);
		int startLayer=0;
		int startHit=StartTrack[0];
    
		bool flag=false;
		for(int i=1;i<fNLayers;i++){  ///< finding layer's number of start hit
			for(unsigned int j=0;j<fLayers[i].size();j++){
				if(fLayers[i][j]==startHit){
					startLayer=i;
					flag=true;
					break;
				}
			}
			if (flag) break;
		}
    
		int testHit;
		for (int Layer=startLayer;Layer<fNLayers;Layer++){
			//if ((fHits[startHit]->GetZ()<(-fZClosePar)) && (Layer==2 or Layer==3 or Layer==5 or Layer==6 or Layer==8 or Layer==10)) continue;  /// < in case of backward tracks disk layers can't contain hits
			for(unsigned int testHitInLayer=0; testHitInLayer<fLayers[Layer].size();testHitInLayer++){
				testHit=fLayers[Layer][testHitInLayer];
        
				if (fVerbose > 2) std::cout << "Layer: " << Layer << " hitInLayer: " << testHitInLayer << " hitID " << testHit << " ";
				if (fVerbose > 1) std::cout << "Point " << fMapHitToID[testHit] << " ";
				if (CheckHitInTrack(StartTrack, testHit)) continue;
				if (CheckHitDistance(StartTrack[0], testHit)!=true) continue;
				if (CheckHitDistance(StartTrack[1], testHit)!=true) continue;
				if (CheckHitDistance(StartTrack[2], testHit)!=true) continue;
				if (CheckZeroPassing(StartTrack, testHit)== true) continue;
        
        
				//if ((fHits[startHit]->GetZ())*(fHits[testHit]->GetZ())<0 && fabs(fHits[startHit]->GetZ())>fZClosePar && fabs(fHits[testHit]->GetZ())>fZClosePar)  continue; //check the same direction on z axis
        
				PndRiemannHit actHit(fHits[testHit], testHit);
        
				if (CheckRiemannHit(&actTrack, &actHit) != true) continue;
        
				StartTrack.push_back(testHit);
        
				actTrack.addHit(actHit);
				actTrack.refit(false);
				actTrack.szFit(false);
        
				TVectorD orig = actTrack.orig();
				if (fVerbose > 1) std::cout << "actHit added: " << testHit  << " r: " << actTrack.r() << " orig: " << orig[0] << " " << orig[1] << std::endl;
				tooClose=GetTooCloseHitsInLayer(Layer,testHit);
				fHitsTooClose[trackId].insert(fHitsTooClose[trackId].begin(),tooClose.begin(),tooClose.end());
				//break;
			}
		}
    
		if ((int)actTrack.getNumHits() > (fMinNumberOfHits-1))		//if you have a track match
		{
			std::vector<int> hits = fHitsTooClose[trackId];
			for (unsigned int ind = 0; ind < hits.size(); ind++){
				if (fVerbose > 1) std::cout << "Too Close Point " << hits[ind] << ": " << fMapHitToID[hits[ind]];
				if (CheckHitInTrack(StartTrack, hits[ind])) continue;
				PndRiemannHit actHit(fHits[hits[ind]]);
				if (CheckRiemannHit(&actTrack, &actHit)!= true) continue;
				StartTrack.push_back(hits[ind]);
				actTrack.addHit(actHit);
        
			}
      
			actTrack.refit(true);
			actTrack.szFit(true);
      
			fTracks.push_back(actTrack);
			fHitsInTracks.push_back(StartTrack);
			if (fVerbose > 1){
				TVectorD orig = actTrack.orig();
				std::cout << "Track added! " << StartTrack[0] << " " << StartTrack[1]
							<< " " << StartTrack[2] << " r: " << actTrack.r()
							<< " orig: " << orig[0] << " " << orig[1]
							<< " sz-m: " << actTrack.getSZm() << " sz-t: " << actTrack.getSZt()
							<< " dip: " << actTrack.dip()
							<< std::endl;
			}
      
			if (fVerbose > 0) std::cout << "Hits in Track: " << StartTrack.size() << std::endl;
			for (unsigned int i = 0; i < StartTrack.size(); i++)
			{
				if (fVerbose > 0)
					std::cout << " " << fMapHitToID[StartTrack[i]];
			}
			if (fVerbose > 0) {
				TVectorD myOrig = actTrack.orig();
				std::cout << " numHits: " << actTrack.getNumHits() << std::endl;
				std::cout << " curv: " << 1/actTrack.r() << "+/-" << actTrack.dR()/(actTrack.r() * actTrack.r())
						<< " dip: " << actTrack.dip() << "+/-" << actTrack.dDip()
						<< " orig: " << myOrig[0] << "+/-" << actTrack.dX()
						<< " " << myOrig[1] << "+/-" << actTrack.dY() << std::endl;
        //	actTrack.getPforHit(0, 2);
			}
		}
	}
  
  //	std::cout << "In PndMvdRiemannTrackFinder are: " << fTracks.size() << " tracks" << std::endl;
	for (unsigned int n = 0; n < fTracks.size(); n++){
		PndTrackCand myTrackCand;
		std::vector<PndRiemannHit> TrackHits = fTracks[n].getHits();
    
		for (unsigned int p = 0; p < TrackHits.size(); p++){
			if (TrackHits[p].hitID() > -1){
				myTrackCand.AddHit(fMapHitToID[TrackHits[p].hitID()], TrackHits[p].s());
			}
		}
		fTrackCand.push_back(myTrackCand);
    
		std::pair<double,double> CurvDip(1/fTracks[n].r(),fTracks[n].dip());
		fCurvAndDipOfCand.push_back(CurvDip);
	}
  
  //	std::cout << "Hits before merging: " << fTrackCand.size() << std::endl;
  //	MergeTracks();
  //	fTrackCand.clear();
  //	fTrackCand = fMergedTrackCand;
  //	if (fVerbose > 0) {
  //		std::cout << "Tracks after merging:" << fMergedTrackCand.size() << std::endl;
  //		for (unsigned int p = 0; p < fTrackCand.size(); p++){
  //			fTrackCand[p].Print();
  //		}											///todo Create RiemannTracks out of PndTrackCands
  //	}
  
}

std::vector< std::vector<Int_t> > PndMvdRiemannTrackFinder::GetStartTracks()
{
	std::vector<Int_t> actCandidates;
	std::vector<std::vector<Int_t> > Tracks;
	std::vector<int> tooCloseFirst;
	std::vector<int> tooCloseSecond;
	std::vector<int> tooCloseThird;
	if (fHits.size() > 3){
    int shift=0;
    if (fUseZeroPos) shift=1;
    for(int FirstLayer=1-shift;FirstLayer<fNLayers-2;FirstLayer++){ /// going through layers : first, second and third
		  for(int SecondLayer=FirstLayer+1;SecondLayer<fNLayers-1;SecondLayer++){
        for(int ThirdLayer=SecondLayer+1;ThirdLayer<fNLayers;ThirdLayer++){
          
          
          for (unsigned int firstInLayer = 0; firstInLayer < fLayers[FirstLayer].size(); firstInLayer++){
            int first=fLayers[FirstLayer][firstInLayer];
            // if ((fHits[first]->GetZ()<(-fZClosePar)) && (SecondLayer==2 or SecondLayer==3 or SecondLayer==5 or SecondLayer==6 or SecondLayer==8 or SecondLayer==10)) continue;
            for (unsigned int secondInLayer = 0; secondInLayer < fLayers[SecondLayer].size(); secondInLayer++){
              int second=fLayers[SecondLayer][secondInLayer];
              // if ((fHits[second]->GetZ()<(-fZClosePar)) && (ThirdLayer==2 or ThirdLayer==3 or ThirdLayer==5 or ThirdLayer==6 or ThirdLayer==8 or ThirdLayer==10)) continue;
              // if ((fHits[first]->GetZ())*(fHits[second]->GetZ())<0 && fabs(fHits[first]->GetZ())>fZClosePar && fabs(fHits[second]->GetZ())>fZClosePar) {/*printf("Diff Sign of Points  z1=%e  z2=%e \n",fHits[first]->GetZ(),fHits[second]->GetZ()); */continue;} //my// check the same direction on z axis
              
              for (unsigned int thirdInLayer = 0; thirdInLayer < fLayers[ThirdLayer].size(); thirdInLayer++){
                int third=fLayers[ThirdLayer][thirdInLayer];
                //if ((fHits[second]->GetZ())*(fHits[third]->GetZ())<0 && fabs(fHits[second]->GetZ())>fZClosePar && fabs(fHits[third]->GetZ())>fZClosePar) {/*printf("Diff Sign of Points  z1=%e  z2=%e \n",fHits[first]->GetZ(),fHits[second]->GetZ());*/ continue;} //my// check the same direction on z axis
                //if (fVerbose > 1) std::cout << "Checking Points: " << first << " " << second << " " << third << std::endl;
                if (CheckHitDistance(first, third)!= true){tooCloseFirst.push_back(third); continue;}
                if (CheckHitDistance(second, third)!= true){tooCloseSecond.push_back(third); continue;}///<---------
                if (CheckHitInSameSensor(first, third)==true)continue;
                if (CheckHitInSameSensor(second, third)==true)continue;
                
                actCandidates.clear();
                actCandidates.push_back(first);
                actCandidates.push_back(second);
                
                if (CheckZeroPassing(actCandidates, third) == true) continue;
                
                actCandidates.push_back(third);
                
                PndRiemannTrack actTrack;// = new PndRiemannTrack();
                PndRiemannHit hit1(fHits[first]);
                PndRiemannHit hit2(fHits[second]);
                PndRiemannHit hit3(fHits[third]);
                actTrack.addHit(hit1);
                actTrack.addHit(hit2);
                actTrack.addHit(hit3);
                actTrack.refit(false);
                if (CheckSZ(actTrack)!= true) continue;
                TVectorT<double> orig = actTrack.orig();
                if (fVerbose > 1) std::cout << "Base plane from Points: " << first << " " << second << " " << third << " r: " << actTrack.r() << " orig: " << orig[0] << " " << orig[1] << std::endl;
                Tracks.push_back(actCandidates);
                tooCloseFirst=GetTooCloseHitsInLayer(FirstLayer,first);
                tooCloseSecond=GetTooCloseHitsInLayer(SecondLayer,second);
                tooCloseThird=GetTooCloseHitsInLayer(ThirdLayer,third);
                std::vector<int> combHits = tooCloseFirst;
                combHits.insert(combHits.begin(), tooCloseSecond.begin(), tooCloseSecond.end());
                combHits.insert(combHits.begin(), tooCloseThird.begin(), tooCloseThird.end());
                fHitsTooClose.push_back(combHits);
                
                
              }
              
            }
            
          }
          
          
        }
      }
		}
	}
  
  //	if (fVerbose > 1) {
  //		std::cout << "Start Tracks are: " << std::endl;
  //		for (unsigned int i = 0; i < Tracks.size(); i++){
  //			std::vector<int> aTrack = Tracks[i];
  //			for (unsigned int j = 0; j < aTrack.size(); j++){
  //				std::cout << aTrack[j] << " ";
  //			}
  //			std::cout << std::endl;
  //		}
  //	}
	return Tracks;
}

bool PndMvdRiemannTrackFinder::CheckSZ(PndRiemannTrack aTrack)
{
	aTrack.szFit(false);
	double r = aTrack.r();
	double dip=aTrack.dip();
	bool sign;
	if ((aTrack.getHit(0)->z())>0 )
		sign=true;
	else sign=false;
	if (aTrack.szChi2() > GetMaxSZChi2(r,dip,sign)){
		if (fVerbose > 1) std::cout << "sz-Fit does not match, Chi2: " << aTrack.szChi2() << " max: " << GetMaxSZChi2(r,dip,sign) << std::endl;
		return false;
	}
	return true;
}

bool PndMvdRiemannTrackFinder::CheckRiemannHit(PndRiemannTrack* track, PndRiemannHit* hit)
{
	double dist = track->dist(hit);
	double szDist = track->szDist(hit);
	double szChi2 = track->calcSZChi2(hit);
	double r = track->r();
	double dip=track->dip();
	bool sign;
	if ((track->getHit(1)->z())>0 )
		sign=true;
	else sign=false;
	if (fVerbose > 1) std::cout << ": dist " << dist << " szDist " << szDist << " szChi2 " << szChi2 << std::endl;
  
	if (fabs(dist) > GetMaxPlaneDist(r,dip,sign)){
		if (fVerbose > 1) std::cout << "dist larger than " << GetMaxPlaneDist(r,dip,sign) << std::endl;
		return false;
	}
	if (szChi2 > GetMaxSZChi2(r,dip,sign)){
		if (fVerbose > 1) std::cout << " SZ Chi2 too big! Cut at: " << GetMaxSZChi2(r,dip,sign) << std::endl;
		return false;
	}
	if (fabs(szDist) > fMaxSZDist){
		if (fVerbose > 1) std::cout << "SZ Dist too big! Cut at: " << fMaxSZDist << std::endl;
		return false;
	}
	return true;
}

std::vector<int> PndMvdRiemannTrackFinder::GetTooCloseHitsInLayer(int LayerNumber , int HitNumber )
{
	std::vector<int> result;
	int testN;
	for(unsigned int i=0;i<fLayers[LayerNumber].size();i++){
		testN=fLayers[LayerNumber][i];
		if (CheckHitDistance(HitNumber, testN)!= true){
			if (testN != HitNumber){
				result.push_back(testN);
			}
		}
	}
	return result;
  
}
////////////////////////////////////
double PndMvdRiemannTrackFinder::GetMaxPlaneDist(double radius, double dip , bool sign)
{
	double Pt=((radius/100)*2*3*1E8)/1E9;
	double Theta;
	if (fabs(TMath::ACos(dip)) < 1E-100){
		Theta = 0;
	}
	else {
	  if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi(); ///calc Theta from dip
	  else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	}
  
  if(fCutDistH!=NULL){
    double minPt=fCutDistH->GetXaxis()->GetXmin();
    double maxPt=fCutDistH->GetXaxis()->GetXmax();
    double minTh=fCutDistH->GetYaxis()->GetXmin();
    double maxTh=fCutDistH->GetYaxis()->GetXmax();
    
    int binPt=int(floor((Pt-minPt)*fCutDistH->GetXaxis()->GetNbins()/(maxPt-minPt)))+1;
    int binTh=int(floor((Theta-minTh)*fCutDistH->GetYaxis()->GetNbins()/(maxTh-minTh)))+1;
    
    if (binPt<1) binPt=1;
    if (binPt>fCutDistH->GetXaxis()->GetNbins()) binPt=fCutDistH->GetXaxis()->GetNbins();
    if (binTh<1) binTh=1;
    if (binTh>fCutDistH->GetYaxis()->GetNbins()) binTh=fCutDistH->GetYaxis()->GetNbins();
    return fCutDistH->GetBinContent(binPt,binTh);
    
  }
  else
    return fMaxPlaneDist;
}

double PndMvdRiemannTrackFinder::GetMaxSZChi2(double radius, double dip , bool sign)
{
	double Pt=((radius/100)*2*3*1E8)/1E9;
	double Theta;
	if (fabs(TMath::ACos(dip)) < 1E-100){
		Theta = 0;
	}
	else {
	  if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();   ///calc Theta from dip
	  else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	}
	if(fCutChi2H!=NULL){
    double minPt=fCutChi2H->GetXaxis()->GetXmin();
    double maxPt=fCutChi2H->GetXaxis()->GetXmax();
    double minTh=fCutChi2H->GetYaxis()->GetXmin();
    double maxTh=fCutChi2H->GetYaxis()->GetXmax();
    
    int binPt=int(floor((Pt-minPt)*fCutChi2H->GetXaxis()->GetNbins()/(maxPt-minPt)))+1;
    int binTh=int(floor((Theta-minTh)*fCutChi2H->GetYaxis()->GetNbins()/(maxTh-minTh)))+1;
    
    if (binPt<1) binPt=1;
    if (binPt>fCutChi2H->GetXaxis()->GetNbins()) binPt=fCutChi2H->GetXaxis()->GetNbins();
    if (binTh<1) binTh=1;
    if (binTh>fCutChi2H->GetYaxis()->GetNbins()) binTh=fCutChi2H->GetYaxis()->GetNbins();
    return fCutChi2H->GetBinContent(binPt,binTh);
    
  }
  else
    return fMaxSZChi2;
}
//////////////////////////////


