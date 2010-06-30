// -------------------------------------------------------------------------
// -----                PndLmdStripClusterTask source file             -----
// -------------------------------------------------------------------------
/*
 * Updated by h.xu@fz-juelich.de on Jun30.2010
 * To match the base class SdsStripClusterTask,the virtual functions SetClusterType() and
 * SetCalculators() were reloaded.
 */


//LUMI
#include "PndLmdStripClusterTask.h"
//PANDA
//#include "PndSdsContFact.h"
#include "PndLmdContFact.h"
//FAIR
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndSdsTotChargeConversion.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsChargeWeightingAlgorithms.h"
#include "PndSdsSimpleStripClusterFinder.h"
#include "PndSdsStripAdvClusterFinder.h"
#include "PndSdsTotDigiPar.h"

//ROOT
#include "TList.h"

// -----   Default constructor   -------------------------------------------

PndLmdStripClusterTask::PndLmdStripClusterTask() :
  PndSdsStripClusterTask("Lmd Cluster Task")
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
//  PndSdsContFact* thelmdcontfact = (PndSdsContFact*)rtdb->getContFactory("PndSdsContFact");
  PndLmdContFact* thelmdcontfact = (PndLmdContFact*)rtdb->getContFactory("PndLmdContFact");
  TList* theContNames = thelmdcontfact->GetDigiParNames();
//  cout<<"The digiparname"<<theContNames<<endl;
  Info("SetParContainers()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    Info("SetParContainers()",parsetname.Data());
    if(parsetname.BeginsWith("LmdStripDigiPar")){
      PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)(rtdb->getContainer(parsetname.Data()));
      fDigiParameterList->Add(digipar);
    }
  }
  PndSdsStripClusterTask::SetParContainers();
}

void PndLmdStripClusterTask::SetCalculators()
{
	  Info("SetCalculators","lmd");
	  PndSdsStripClusterTask::SetCalculators();
		TIter params(fDigiParameterList);

		while( PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)params() ){

			if ( 0==digipar ) continue;
			const char* senstype = digipar->GetSensType();
			cout<<"sensor type is "<<senstype<<endl;

	    //fChargeAlgos[senstype] = new PndSdsChargeWeightingAlgorithms(fDigiArray, digipar->GetRaisingTime(),digipar->GetFallingRatio(),digipar->GetThreshold());
	    Int_t ClusterMod = digipar->GetClusterMod();
	    Int_t RadChannel = digipar->GetRadChannel();
	    Int_t RadTime    = digipar->GetRadTime();
	    cout<<"The CLuster mode is"<<ClusterMod<<endl;
	    if(0==ClusterMod) {
	    	fClusterFinderList[senstype] = new PndSdsSimpleStripClusterFinder( RadChannel ); //search radius in channel no.
	    } else if(1==ClusterMod) {
	    	fClusterFinderList[senstype] = new PndSdsStripAdvClusterFinder(RadChannel, RadTime);
	    }
		//PndSdsStripClusterTask::SetCurrentCalculators(digipar);
		}

}

/*
Bool_t PndLmdStripClusterTask::Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr,
 	                TVector3 &hitPos, TVector3 &hitErr, TString &detname)
{
  // BACKMAPPING
  // get the backmapped point

  TVector3 localpos, locDpos;
  Double_t t, b;
  Double_t errZ = 2.*fGeoH->GetSensorDimensionsPath(detname).Z()/TMath::Sqrt(12.0);

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
  hitPos = fGeoH->LocalToMasterPath(localpos,detname.Data());
 	
  // calculate the errors corresponding to a skewed system!
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*cos(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*cos(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  locDpos.SetX( sqrt(t*t+b*b) );
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*sin(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*sin(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  locDpos.SetY( sqrt(t*t+b*b) );
  locDpos.SetZ( errZ );
  hitErr = locDpos;
 	 
  //do the transformation from sensor to lab frame
//  hitErr = fGeoH->LocalToMasterErrorsPath(locDpos,detname.Data());
//  hitErr = fGeoH->MasterToLocalErrorsShortId(locDpos,detname.Dat());
 	
  return kTRUE;
}
*/
ClassImp(PndLmdStripClusterTask);

