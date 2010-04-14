// -------------------------------------------------------------------------
// -----                PndLmdStripClusterTask source file             -----
// -------------------------------------------------------------------------

//LUMI
#include "PndLmdStripClusterTask.h"
//PANDA
#include "PndSdsContFact.h"
//FAIR
#include "FairRun.h"
#include "FairRuntimeDb.h"
//ROOT
#include "TList.h"

// -----   Default constructor   -------------------------------------------

PndLmdStripClusterTask::PndLmdStripClusterTask() :
  PndSdsStripClusterTask()
{
  fyRotation=0.;
/*  fChargeCut = 1.e8; // this ist really large and shall have no effect
  fGeoFile = "";
  fClusterMod=ClusterMod;
  if(fClusterMod>1 || fClusterMod<0) fClusterMod=0;
  fRadChannel=RadChannel;
  fRadTime=RadTime;*/
}


// -----   Destructor   ----------------------------------------------------
PndLmdStripClusterTask::~PndLmdStripClusterTask()
{
  //if(0!=fGeoH)  delete fGeoH;
  //if(0!=fChargeAlgos) delete fChargeAlgos;
}
// -------------------------------------------------------------------------

void PndLmdStripClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fBranchName = inBranchname;
  fHitBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

void PndLmdStripClusterTask::SetBranchNames()
{
  fBranchName = "LMDStripDigis";
  fHitBranchName = "LMDHitsStrip";
  fClustBranchName = "LMDStripClusterCand";
  fFolderName = "PndLmd";
}

void PndLmdStripClusterTask::SetParContainers()
{
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet.
  // This will be done just before this Tasks Init() is called.

  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  PndSdsContFact* thelmdcontfact = (PndSdsContFact*)rtdb->getContFactory("PndSdsContFact");
  TList* theContNames = thelmdcontfact->GetDigiParNames();
  Info("SetParContainers()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    Info("SetParContainers()",parsetname.Data());
    if(parsetname.BeginsWith("SDSStripDigiPar")){
      PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)(rtdb->getContainer(parsetname.Data()));
      fDigiParameterList->Add(digipar);
    }
  }

}

Bool_t PndLmdStripClusterTask::Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr,
 	                TVector3 &hitPos, TVector3 &hitErr, TString &detname)
{
  // BACKMAPPING
  // get the backmapped point

  TVector3 localpos, locDpos;
  Double_t t, b;
  Double_t errZ = 2.*fGeoH->GetSensorDimensionsId(detname).Z()/TMath::Sqrt(12.0);

  TVector2 onsensorPoint =
    CalcLineCross(meantopPoint, fCurrentStripCalcTop->GetStripDirection(), meanbotPoint, fCurrentStripCalcBot->GetStripDirection() );
  // here we assume the sensor system to be in the _Middle_ of the volume
  if(fyRotation==0.){  						//TODO: make this generaly
    localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), 0.);
  }else{
    localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), onsensorPoint.X()*tan(fyRotation));
  }
 	
  // let's see if we're still on the sensor (cut combinations with noise off)
  if(fabs(localpos.X()) > fabs(fCurrentDigiPar->GetTopAnchor().X())) return kFALSE;
  if(fabs(localpos.Y()) > fabs(fCurrentDigiPar->GetTopAnchor().Y())) return kFALSE;
	
  //do the transformation from sensor to lab frame
  hitPos = fGeoH->LocalToMasterId(localpos,detname.Data());
 	
  // calculate the errors corresponding to a skewed system!
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*cos(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*cos(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  locDpos.SetX( sqrt(t*t+b*b) );
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*sin(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*sin(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  locDpos.SetY( sqrt(t*t+b*b) );
  locDpos.SetZ( errZ );
 	 
  //do the transformation from sensor to lab frame
  hitErr = fGeoH->LocalToMasterErrorsId(locDpos,detname.Data());
 	
  return kTRUE;
}

ClassImp(PndLmdStripClusterTask);

