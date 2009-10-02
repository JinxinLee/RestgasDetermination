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
#include <string>
#include <sstream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndTrackDraw::PndTrackDraw()
{
	   fPro = new FairGeanePro();

	   fPndTrackList = 0;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndTrackDraw::PndTrackDraw(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fEveTrList( new TObjArray(16))
{
	  fPro = new FairGeanePro();
	  fPndTrackList = 0;
	  fTrackCandDraw = new PndTrackCandDraw(name, iVerbose);
}
// -------------------------------------------------------------------------
InitStatus PndTrackDraw::Init()
{
   if (fVerbose > 1)
		cout << "PndTrackDraw::Init()" << endl;
   if (fPndTrackList == 0){
		FairRootManager* fManager = FairRootManager::Instance();
		fPndTrackList = (TClonesArray *) fManager->GetObject(GetName());
		if (fPndTrackList == 0) {
			cout << "FairMCPointDraw::Init()  branch " << GetName()
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
	if (IsActive())
		return kSUCCESS;
	else
		return kERROR;
}

// -------------------------------------------------------------------------
void PndTrackDraw::Exec(Option_t* option)
{

  if (IsActive()){

	if(fVerbose>1)  cout << " PndTrackDraw::Exec "<< endl;
    PndTrack *tr;
    const Double_t *point;

    Reset();
    fTrackCandDraw->Reset();

    TEveBoxSet* myBoxSet = fTrackCandDraw->CreateBoxSet();

    for (Int_t i=0; i<fPndTrackList->GetEntriesFast(); i++)	{
		if(fVerbose>2) cout << "PndTrackDraw::Exec "<< i << endl;
        tr=(PndTrack *)fPndTrackList->At(i);
        Int_t pidHypo = tr->GetPidHypo();
        FairTrackParP parFirst = tr->GetParamFirst();
        FairTrackParP parLast = tr->GetParamLast();
        PndTrackCand trackCand = tr->GetTrackCand();

        fTrackCandDraw->AddBoxesPndTrackCand(myBoxSet,&trackCand, i);

		TVector3 posFirst = parFirst.GetPosition();
		TVector3 momFirst = parFirst.GetMomentum() * 10;
		//TVector3 resFirst = posFirst + momFirst;

		TEveArrow* myArrowFirst = new TEveArrow(momFirst.X(), momFirst.Y(), momFirst.Z(),
												posFirst.X(), posFirst.Y(), posFirst.Z());
		myArrowFirst->SetMainColor(kRed);
		myArrowFirst->SetTubeR(0.01);
		fArrows.push_back(myArrowFirst);
		gEve->AddElement(myArrowFirst, fEventManager);


        std::cout << "ParamFirst: " << posFirst.X() << " " << posFirst.Y() << " " << posFirst.Z() << std::endl;
        std::cout << "ParamFirst Momentum: " << momFirst.X() << " " << momFirst.Y() << " " << momFirst.Z() << std::endl;

        PropagateTrack(parFirst, pidHypo, kRed);

		TVector3 posLast = parLast.GetPosition();
		TVector3 momLast = parLast.GetMomentum() * 10;
		//TVector3 resLast = posLast + momLast;

		TEveArrow* myArrowLast = new TEveArrow(momLast.X(), momLast.Y(), momLast.Z(),
												posLast.X(), posLast.Y(), posLast.Z());
	    myArrowLast->SetMainColor(kBlue);
        myArrowLast->SetTubeR(0.01);
        fArrows.push_back(myArrowLast);
        gEve->AddElement(myArrowLast, fEventManager);

        std::cout << "ParamLast: " << posLast.X() << " " << posLast.Y() << " " << posLast.Z() << std::endl;
        std::cout << "ParamLast Momentum: " << momLast.X() << " " << momLast.Y() << " " << momLast.Z() << std::endl;


        PropagateTrack(parLast, pidHypo, kBlue);

    }
    gEve->AddElement(myBoxSet, fEventManager);

    for (Int_t i=0; i<fEveTrList->GetEntriesFast(); i++){
       TEveTrackList *TrListIn=( TEveTrackList *) fEveTrList->At(i);
       //TrListIn->FindMomentumLimits(TrListIn, kFALSE);
    }
  //  fEventManager->SetEvtMaxEnergy(MaxEnergyLimit);
  //  fEventManager->SetEvtMinEnergy(MinEnergyLimit);
    gEve->Redraw3D(kFALSE);
 }
}

void PndTrackDraw::PropagateTrack(FairTrackParP& trackPar, Int_t pidHypo, Int_t color)
{
	TVector3 posTrack = trackPar.GetPosition();
	TVector3 momTrack = trackPar.GetMomentum();
	Double_t charge = trackPar.GetQ();

	if (pidHypo == 0)
		pidHypo = 211;
	if (charge < 0)
		pidHypo *= -1;

	if (color < 0) color = fEventManager->Color(pidHypo);

	Int_t Np = 100;

	TVector3 startU(1., 0., 0.);
	TVector3 startV(0., 1., 0.);
	fPro->PropagateFromPlane(startU, startV);

	TVector3 stopTrack(0., 0., posTrack.z() - Np);
	fPro->PropagateToPlane(stopTrack, startU, startV);
	FairTrackParP parResult;
	fPro->setBackProp();
	fPro->Propagate(&trackPar, &parResult, pidHypo);

	TVector3 posStart = parResult.GetPosition();
	TVector3 momStart = parResult.GetMomentum();

	if (fVerbose > 1)
		std::cout << "PosStart: " << posStart.X() << " " << posStart.Y() << " "
			<< posStart.Z() << std::endl;

	TParticle *P = new TParticle(pidHypo, 0, -1, -1, -1, -1, TLorentzVector(
			momStart, 10), TLorentzVector(posStart, 0));
	//PEnergy=P->Energy();
	//MinEnergyLimit=TMath::Min(PEnergy,MinEnergyLimit) ;
	//MaxEnergyLimit=TMath::Max(PEnergy,MaxEnergyLimit) ;
	//if(fVerbose>2)cout << "MinEnergyLimit " << MinEnergyLimit << " MaxEnergyLimit " << MaxEnergyLimit << endl;
	//if (fEventManager->IsPriOnly() && P->GetMother(0)>-1) continue;
	//if (fEventManager->GetCurrentPDG() != 0 && fEventManager->GetCurrentPDG()!= pidHypo) continue;
	// if(fVerbose>2)cout << "PEnergy " << PEnergy << " Min "  << fEventManager->GetMinEnergy() << " Max " << fEventManager->GetMaxEnergy() <<endl;
	//if( (PEnergy<fEventManager->GetMinEnergy()) || (PEnergy >fEventManager->GetMaxEnergy())) continue;

	fTrList = GetTrGroup(pidHypo);
	TEveTrack *track = new TEveTrack(P, pidHypo, fTrPr);
	track->SetLineColor(color);

	fPro->PropagateFromPlane(startU, startV);

	Int_t index = 0;
	for (Int_t n = -Np; n < Np; n++) {
		if (n == 0)
			continue;
		TVector3 stopO(0., 0., posTrack.z() + n);
		fPro->PropagateToPlane(stopO, startU, startV);
		//FairTrackParP parResult;
		if (n * momTrack.Z() < 0)
			fPro->setBackProp();
		fPro->Propagate(&trackPar, &parResult, pidHypo);

		if (fVerbose > 2)
			std::cout << "ParResult " << n << ": " << parResult.GetX() << " "
				<< parResult.GetY() << " " << parResult.GetZ() << std::endl;

		track->SetPoint(index++, parResult.GetX(), parResult.GetY(),
				parResult.GetZ());
		TEveVector pos = TEveVector(parResult.GetX(), parResult.GetY(),
				parResult.GetZ());
		TEvePathMark *path = new TEvePathMark();
		path->fV = pos;
		path->fTime = n;
		if (n == 0) {
			TEveVector mom = TEveVector(parResult.GetPx(), parResult.GetPy(),
					parResult.GetPz());
			path->fP = mom;
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
	fTrList->AddElement(track);
	if (fVerbose > 3)
		cout << "track added " << track->GetName() << endl;
}


// -----   Destructor   ----------------------------------------------------
PndTrackDraw::~PndTrackDraw()
{
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
   for (Int_t j = 0; j < fArrows.size(); j++){
	   gEve->RemoveElement(fArrows[j], fEventManager);
	   //delete(fArrows[j]);
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
    gEve->AddElement( fTrList ,fEventManager );
    fTrList->SetRnrLine(kTRUE);
  }                          
  return fTrList;
}

ClassImp(PndTrackDraw)


