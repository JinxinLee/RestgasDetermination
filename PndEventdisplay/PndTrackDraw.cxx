// -------------------------------------------------------------------------
// -----                        PndTrackDraw source file                  -----
// -----                  Created 10/12/07  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "PndTrackDraw.h"
#include "TEveTrack.h"
#include <iostream>
#include "TEveTrackPropagator.h"
#include "TGeoTrack.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TEveManager.h"
#include "FairEventManager.h"
#include "PndTrack.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TEveBoxSet.h"
#include "PndHelixPropagator.h"
#include <string>
#include <sstream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndTrackDraw::PndTrackDraw(Bool_t propagate)
	:fTimeWindowPlus(0.),
	 fTimeWindowMinus(0.),
	 fUseEventTime(kTRUE),
	 fDoPropagation(propagate),
   fListOfTracks(0)
{
	   fPndTrackList = 0;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndTrackDraw::PndTrackDraw(const char* name, Bool_t propagate, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTimeWindowPlus(0.),
    fTimeWindowMinus(0.),
    fUseEventTime(kTRUE), fEveTrList( new TObjArray(16)),
    fDoPropagation(propagate), fListOfTracks(0)
{
	 // fPro = new FairGeanePro();
	  fPndTrackList = 0;
	  TString trackCandName(name);
//	  trackCandName.Append("_TrackCand");
	  fTrackCandDraw = new PndTrackCandDraw(trackCandName.Data(), iVerbose);
}
// -------------------------------------------------------------------------
InitStatus PndTrackDraw::Init()
{
	//fPro = new FairGeanePro();
   if (fVerbose > 1)
		cout << "PndTrackDraw::Init()" << endl;
   if (fPndTrackList == 0){
		FairRootManager* fManager = FairRootManager::Instance();
		fPndTrackList = (TClonesArray *) fManager->GetObject(GetName());
		if (fPndTrackList == 0) {
			cout << "PndTrackDraw::Init()  branch " << GetName()
					<< " Not found! Task will be deactivated " << endl;
			SetActive(kFALSE);
		}
		if (fVerbose > 2)
			cout << "PndTrackDraw::Init() get track list" << fPndTrackList << endl;
		//if(fVerbose>2) cout<<  "PndTrackDraw::Init()  create propagator" << endl;
		fEventManager = FairEventManager::Instance();
		if (fVerbose > 2)
			cout << "PndTrackDraw::Init() get instance of FairEventManager "
					<< endl;
		fEvent = "Current Event";
		MinEnergyLimit = fEventManager->GetEvtMinEnergy();
		MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
		PEnergy = 0;

		fTrackCandDraw->Init();
   }
   fStartFunctor = new StopTime();
   fStopFunctor = new StopTime();

   if (IsActive())
		return kSUCCESS;
	else
		return kERROR;
}
// -----   Destructor   ----------------------------------------------------
PndTrackDraw::~PndTrackDraw()
{
	delete fTrackCandDraw;
}

// -------------------------------------------------------------------------
void PndTrackDraw::Exec(Option_t* option)
{

  if (IsActive()){

	if(fVerbose>1)  cout << " PndTrackDraw::Exec "<< endl;
    PndTrack *tr;
    //const Double_t *point; //[R.K. 01/2017] unused variable

    Reset();
    fTrackCandDraw->Reset();

    FairBoxSet* myBoxSet = fTrackCandDraw->CreateBoxSet();

    //fListOfTracks->DestroyElements();

	TString trackName(GetName());
	trackName.Append("_Track");
    fListOfTracks = new TEveElementList(trackName.Data(),"List of PndTracks");

    gEve->AddElement(fListOfTracks,fEventManager);

	if (FairRunAna::Instance()->IsTimeStamp()) {
		std::cout << "Inside TimeStamp" << std::endl;
		fPndTrackList->Clear();
		Double_t eventTime = FairRootManager::Instance()->GetEventTime();
		if (fUseEventTime) {
			fStartTime = eventTime - fTimeWindowMinus;
		}
		cout << "EventTime: " << eventTime << " TimeWindow: " << fStartTime
				<< " - " << eventTime + fTimeWindowPlus << std::endl;

		fPndTrackList = FairRootManager::Instance()->GetData(GetName(),
				fStartFunctor, fStartTime, fStopFunctor,
				eventTime + fTimeWindowPlus); //FairRootManager::Instance()->GetEventTime() +
		std::cout << fStartTime << " " << eventTime + fTimeWindowPlus << " FoundTracks: " << fPndTrackList->GetEntriesFast() << std::endl;
	}

    for (Int_t i=0; i<fPndTrackList->GetEntriesFast(); i++)	{
		if(fVerbose>2) cout << "PndTrackDraw::Exec "<< i << endl;
        tr=(PndTrack *)fPndTrackList->At(i);
        Int_t pidHypo = tr->GetPidHypo();
        
   //     Int_t q= tr->GetParamFirst().GetQ();
   //     Int_t pidHypo = 0;
   //     if (q > 0)
   //     	pidHypo = 211;
   //     else
   //     	pidHypo = -211;
        FairTrackParP parFirst = tr->GetParamFirst();
        FairTrackParP parLast = tr->GetParamLast();
        PndTrackCand trackCand = tr->GetTrackCand();

        if (fVerbose> 1){
        	cout << "PndTrack " << i << ":" << std::endl;
        	tr->Print();
        }

        fTrackCandDraw->AddBoxesPndTrackCand(myBoxSet,&trackCand, i);

		TVector3 posFirst = parFirst.GetPosition();
		TVector3 momFirst;
		if (parFirst.GetMomentum().Mag() < 0.1) {
			momFirst = parFirst.GetMomentum() * 100;
		} else {
			momFirst = parFirst.GetMomentum() * 10;
		}
		//TVector3 resFirst = posFirst + momFirst;
		TString groupName("Track_");
		groupName += i;
		TEveElementList* arrowList = new TEveElementList(groupName.Data(), "PndMCTrack");


		TEveArrow* myArrowFirst = new TEveArrow(momFirst.X(), momFirst.Y(), momFirst.Z(),
												posFirst.X(), posFirst.Y(), posFirst.Z());
		myArrowFirst->SetMainColor(kRed);
		myArrowFirst->SetTubeR(0.01);
		fArrows.push_back(myArrowFirst);
		//gEve->AddElement(myArrowFirst, fEventManager);
		arrowList->AddElement(myArrowFirst);

        std::cout << "ParamFirst: " << posFirst.X() << " " << posFirst.Y() << " " << posFirst.Z() << std::endl;
        std::cout << "ParamFirst Momentum: " << momFirst.X() << " " << momFirst.Y() << " " << momFirst.Z() << std::endl;
        std::cout << "Charge: " << parFirst.GetQ() << std::endl;

        if (fDoPropagation)
        	PropagateTrackHelix(parFirst, pidHypo, kRed, arrowList);

		TVector3 posLast = parLast.GetPosition();
		TVector3 momLast;
		if (parLast.GetMomentum().Mag() < 0.1) {
			momLast = parLast.GetMomentum() * 100;
		} else {
			momLast = parLast.GetMomentum() * 10;
		}
		//TVector3 resLast = posLast + momLast;

		TEveArrow* myArrowLast = new TEveArrow(momLast.X(), momLast.Y(), momLast.Z(),
												posLast.X(), posLast.Y(), posLast.Z());
	    myArrowLast->SetMainColor(kBlue);
        myArrowLast->SetTubeR(0.01);
        fArrows.push_back(myArrowLast);
        //gEve->AddElement(myArrowLast, fEventManager);
        arrowList->AddElement(myArrowLast);

        std::cout << "ParamLast: " << posLast.X() << " " << posLast.Y() << " " << posLast.Z() << std::endl;
        std::cout << "ParamLast Momentum: " << momLast.X() << " " << momLast.Y() << " " << momLast.Z() << std::endl;
        std::cout << "Charge: " << parLast.GetQ() << std::endl;

        if (fDoPropagation)
        	PropagateTrackHelix(parLast, pidHypo, kBlue, arrowList);

//        	PropagateTrack(parLast, pidHypo, kBlue, arrowList);
        fListOfTracks->AddElement(arrowList);

    }
    gEve->AddElement((TEveBoxSet*)myBoxSet, fEventManager);

    //for (Int_t i=0; i<fEveTrList->GetEntriesFast(); i++){ //[R.K. 01/2017] unused variable
       //TEveTrackList *TrListIn=( TEveTrackList *) fEveTrList->At(i); //[R.K. 01/2017] unused variable
       //TrListIn->FindMomentumLimits(TrListIn, kFALSE);
    //} //[R.K. 01/2017] unused variable
  //  fEventManager->SetEvtMaxEnergy(MaxEnergyLimit);
  //  fEventManager->SetEvtMinEnergy(MinEnergyLimit);
    gEve->Redraw3D(kFALSE);
 }
}

void PndTrackDraw::PropagateTrackHelix(FairTrackParP& trackPar, Int_t pidHypo, Int_t color, TEveElement* group)
{
	Int_t Np = 100;

	TVector3 posTrack = trackPar.GetPosition();
	TVector3 momTrack = trackPar.GetMomentum();
	Double_t charge = trackPar.GetQ();

	PndHelixPropagator prop(2.0, posTrack, momTrack, charge);
	prop.PropagateToXYPos(TVector2(0,0));

	if (pidHypo == 0){
		pidHypo = 211;
		if (charge < 0)
			pidHypo *= -1;
	}

	TParticle *P = new TParticle(pidHypo, 0, -1, -1, -1, -1, TLorentzVector(momTrack, 10), TLorentzVector(posTrack, 0));

	fTrList = GetTrGroup(pidHypo);
	TEveTrack *track = new TEveTrack(P, pidHypo, fTrPr);
	track->SetLineColor(color);

	Int_t index = 0;
	for (Int_t n = -Np; n < Np; n++){
		FairTrackPar result = prop.PropagateByAngle(n);

		track->SetPoint(index++, result.GetX(), result.GetY(), result.GetZ());
		TEveVector pos = TEveVector(result.GetX(), result.GetY(), result.GetZ());
		TEvePathMark *path = new TEvePathMark();
		path->fV = pos;
		path->fTime = n;
		if (n == 0) {
			TEveVector mom = TEveVector(result.GetPx(), result.GetPy(),result.GetPz());
			path->fP = mom;
			track->SetPoint(index++, posTrack.X(), posTrack.Y(), posTrack.Z());
		}
		if (fVerbose > 3)
			cout << "Path marker added " << path << endl;
		#if ROOT_VERSION_CODE <= ROOT_VERSION(5,18,0)
				track->AddPathMark(path);
		#else
				track->AddPathMark(*path);
		#endif
		if (fVerbose > 3)
			cout << "Path marker added " << path << endl;
	}
	if (group != 0)
		group->AddElement(track);
	else
		fTrList->AddElement(track);
	if (fVerbose > 3)
		cout << "track added " << track->GetName() << endl;
}

void PndTrackDraw::PropagateTrack(FairTrackParP& trackPar, Int_t pidHypo, Int_t color, TEveElement* group)
{
//	TVector3 posTrack = trackPar.GetPosition();
//	TVector3 momTrack = trackPar.GetMomentum();
//	Double_t charge = trackPar.GetQ();
//
//	if (pidHypo == 0){
//		pidHypo = 211;
//		if (charge < 0)
//			pidHypo *= -1;
//	}
//
//	if (color < 0) color = fEventManager->Color(pidHypo);
//
//	Int_t Np = 100;
//
//	TVector3 startU(1., 0., 0.);
//	TVector3 startV(0., 1., 0.);
//	fPro->PropagateFromPlane(startU, startV);
//
//	TVector3 stopTrack(0., 0., posTrack.z() - Np);
//	fPro->PropagateToPlane(stopTrack, startU, startV);
//	FairTrackParP parResult;
//	fPro->setBackProp();
//	fPro->Propagate(&trackPar, &parResult, pidHypo);
//
//	TVector3 posStart = parResult.GetPosition();
//	TVector3 momStart = parResult.GetMomentum();
//
//	if (fVerbose > 1)
//		std::cout << "PosStart: " << posStart.X() << " " << posStart.Y() << " "
//			<< posStart.Z() << std::endl;
//
//	TParticle *P = new TParticle(pidHypo, 0, -1, -1, -1, -1, TLorentzVector(
//			momStart, 10), TLorentzVector(posStart, 0));
//
//	fTrList = GetTrGroup(pidHypo);
//	TEveTrack *track = new TEveTrack(P, pidHypo, fTrPr);
//	track->SetLineColor(color);
//
//	fPro->PropagateFromPlane(startU, startV);
//
//
//	Int_t index = 0;
//	for (Int_t n = -Np; n < Np; n++) {
//		if (n == 0)
//			continue;
//		TVector3 stopO(0., 0., posTrack.z() + (0.1) * n);
//		fPro->PropagateToPlane(stopO, startU, startV);
//		//FairTrackParP parResult;
//		if (n * momTrack.Z() < 0)
//			fPro->setBackProp();
//		fPro->Propagate(&trackPar, &parResult, pidHypo);
//
//		if (fVerbose > 2)
//			std::cout << "ParResult " << n << ": " << parResult.GetX() << " "
//				<< parResult.GetY() << " " << parResult.GetZ() << std::endl;
//
//		track->SetPoint(index++, parResult.GetX(), parResult.GetY(),
//				parResult.GetZ());
//		TEveVector pos = TEveVector(parResult.GetX(), parResult.GetY(),
//				parResult.GetZ());
//		TEvePathMark *path = new TEvePathMark();
//		path->fV = pos;
//		path->fTime = n;
//		if (n == 0) {
//			TEveVector mom = TEveVector(parResult.GetPx(), parResult.GetPy(),
//					parResult.GetPz());
//			path->fP = mom;
//			track->SetPoint(index++, posTrack.X(), posTrack.Y(), posTrack.Z());
//		}
//		if (fVerbose > 3)
//			cout << "Path marker added " << path << endl;
//#if ROOT_VERSION_CODE <= ROOT_VERSION(5,18,0)
//		track->AddPathMark(path);
//#else
//		track->AddPathMark(*path);
//#endif
//		if (fVerbose > 3)
//			cout << "Path marker added " << path << endl;
//	}
//	if (group != 0)
//		group->AddElement(track);
//	else
//		fTrList->AddElement(track);
//	if (fVerbose > 3)
//		cout << "track added " << track->GetName() << endl;
}



// -------------------------------------------------------------------------
void PndTrackDraw::SetParContainers()
{

}

// -------------------------------------------------------------------------
void PndTrackDraw::Finish()
{

}
// -------------------------------------------------------------------------
void PndTrackDraw::Reset()
{
   for (Int_t i=0; i<fEveTrList->GetEntriesFast(); i++){
      TEveTrackList  *ele=( TEveTrackList *) fEveTrList->At(i);
      gEve->RemoveElement(ele,fEventManager); 
   }
   fEveTrList->Clear();
   for (size_t j = 0; j < fArrows.size(); j++){
	   gEve->RemoveElement(fArrows[j], fEventManager);
	   //delete(fArrows[j]);
   }
   if (fListOfTracks != 0){
	   gEve->RemoveElement(fListOfTracks, fEventManager);
	   //delete(fListOfTracks);
   }
   fArrows.clear();
}

TEveTrackList *PndTrackDraw::GetTrGroup(Int_t pid)
{
	std::ostringstream myStream;
	myStream << pid;
	std::string pidString(myStream.str());
  fTrList=0;
  for (Int_t i=0; i<fEveTrList->GetEntriesFast(); i++){
     TEveTrackList *TrListIn=( TEveTrackList *) fEveTrList->At(i);
     std::string name(TrListIn->GetName());
     if ( name == pidString ) {
	 fTrList= TrListIn;
         break;
     }
  }
  if(fTrList ==0){
    fTrPr=new TEveTrackPropagator();
    fTrList= new  TEveTrackList(pidString.c_str(),fTrPr );
    fTrList->SetMainColor(fEventManager->Color(pid));
    fEveTrList->Add(fTrList);
   // gEve->AddElement( fTrList ,fEventManager );
    fTrList->SetRnrLine(kTRUE);
  }                          
  return fTrList;
}

void PndTrackDraw::SetTimeWindowMinus(Double_t val)
{
  fTimeWindowMinus = val;
}

void PndTrackDraw::SetTimeWindowPlus(Double_t val)
{
  fTimeWindowPlus = val;
}


ClassImp(PndTrackDraw)


