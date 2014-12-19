// -------------------------------------------------------------------------
// -----                       PndStt source file                      -----
// -------------------------------------------------------------------------

#include "PndStt.h"

#include "PndGeoSttPar.h"
#include "PndGeoStt.h"
#include "PndSttPoint.h"
#include "PndDetectorList.h"
#include "PndStack.h"
#include "PndSttMapCreator.h"

#include "FairRun.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TObjArray.h"
#include "TGeoTube.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"

#include <iostream>
#include <math.h>

using std::cout;
using std::endl;
using std::string;

// TODO: read this from geant initialization
#define innerStrawDiameter 1.
//#define redefineLambdaChargedDecay 0

// -----   Default constructor   -------------------------------------------
PndStt::PndStt()
: fTrackID(0), fVolumeID(0), fPos(0,0,0,0), fPosIn(0,0,0,0), fPosOut(0,0,0,0), fPosInLocal(0,0,0,0), fPosOutLocal(0,0,0,0),
    fMomIn(0,0,0,0), fMomOut(0,0,0,0), fTime(0), fLength(0), fELoss(0), fMass(0), fIsInitialized(kFALSE), fPosIndex(0),
    fSttCollection(0), fpostot(0,0,0,0), fpostotin(0,0,0,0), fpostotout(0,0,0,0), fPassNodes(new TObjArray()), fGeoType(0), fInFlag(0)
{
    fSttCollection = new TClonesArray("PndSttPoint");
    fVerboseLevel = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndStt::PndStt(const char* name, Bool_t active)
  : FairDetector(name, active),
    fTrackID(0), fVolumeID(0), fPos(0,0,0,0), fPosIn(0,0,0,0), fPosOut(0,0,0,0), fPosInLocal(0,0,0,0), fPosOutLocal(0,0,0,0),
    fMomIn(0,0,0,0), fMomOut(0,0,0,0), fTime(0), fLength(0), fELoss(0), fMass(0), fIsInitialized(kFALSE), fPosIndex(0),
    fSttCollection(0), fpostot(0,0,0,0), fpostotin(0,0,0,0), fpostotout(0,0,0,0), fPassNodes(new TObjArray()), fGeoType(0), fInFlag(0)
{
    fSttCollection = new TClonesArray("PndSttPoint");
    fVerboseLevel = 0;
}
// -------------------------------------------------------------------------




// -----   Destructor   ----------------------------------------------------
PndStt::~PndStt() 
{
    if (fSttCollection) 
    {
	fSttCollection->Delete(); 
	delete fSttCollection;
    }
}
// -------------------------------------------------------------------------



// -----   Private method GetSquaredDistanceFromWire -----------------------
float PndStt::GetSquaredDistanceFromWire()
{
    TLorentzVector
	entryPosition;  
    
    float
	positionInMother[3],
	positionInStraw[3];
    
    gMC->TrackPosition(entryPosition);
    positionInMother[0] = entryPosition.X();
    positionInMother[1] = entryPosition.Y();
    positionInMother[2] = entryPosition.Z();
    gMC->Gmtod(positionInMother, positionInStraw, 1);
    
    return positionInStraw[0] * positionInStraw[0] + 
	positionInStraw[1] * positionInStraw[1];
}
// -------------------------------------------------------------------------

bool PndStt::Split(string &aDest, string &aSrc, char aDelim)
{
   if(aSrc.empty())
      return false;

   string::size_type 
       pos = aSrc.find(aDelim);

   aDest = aSrc.substr(0, pos);

   if(pos != string::npos)
       aSrc = aSrc.substr(pos + 1);
   else
       aSrc = "";

   return true;
}

string PndStt::GetStringPart(string &aSrc, Int_t part, char aDelim)
{	
    string 
	retval = "",
	sub;

    int
	counter = 0;

    while(Split(sub, aSrc, aDelim))
    {
	if (counter == part)
	{
	    retval = sub;
	    break;
	}
	counter++;
    }

    return retval;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t  PndStt::ProcessHits(FairVolume* vol)
{
  //new>>>>>>>>>>>>>>>>>>

  TString vol_name(gMC->CurrentVolName()); 
  TGeoHMatrix M;
  gMC->GetTransformation(gMC->CurrentVolPath(),M);
  TString name(gMC->CurrentVolName());
  Bool_t skew = kFALSE;
  if(name.Contains("skew")) skew = kTRUE;
  //new>>>>>>>>>>>>>>>>>>
  
  TGeoMedium *medium = (TGeoMedium*) vol->getGeoNode()->getRootVolume()->GetMedium();
  Double_t epsil = medium->GetParam(6);

  if (gMC->TrackCharge() != 0.)
    {
      
      if ( gMC->IsTrackEntering() &&
	   fabs(sqrt(GetSquaredDistanceFromWire()) - (innerStrawDiameter / 2.)) < epsil)
	{
	  fInFlag = kTRUE;
	    // Set parameters at entrance of volume. Reset ELoss.
	    fELoss  = 0.;
	    fTime   = gMC->TrackTime() * 1.0e09;
	    fLength = gMC->TrackLength();
	    gMC->TrackPosition(fPos);
	    gMC->TrackMomentum(fMomIn);
	    gMC->TrackPosition(fpostotin); // CHECK delete this?
	    Double_t globalPos[3] = {0., 0., 0.};
	    Double_t localPos[3] = {0., 0., 0.}; 
	    
	    globalPos[0] = fPos.X();
	    globalPos[1] = fPos.Y();
	    globalPos[2] = fPos.Z();
	    
	    gMC->Gmtod(globalPos, localPos, 1);
	    fPosInLocal.SetXYZM(localPos[0], localPos[1], localPos[2], 0.0);
	}

      
      // Sum energy loss for all steps in the active volume
      fELoss += gMC->Edep();
      
      // Create PndSttPoint at exit of active volume -- but not into the wire -- with eloss in the tube (to make it work with TGeant4)
      if (gMC->IsTrackExiting() && 
	  fInFlag == kTRUE && fabs(sqrt(GetSquaredDistanceFromWire()) - (innerStrawDiameter / 2.)) < epsil
	  && fELoss != 0)
	{
	  fInFlag = kFALSE;
	  fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
	  fVolumeID = kSTT;                // vol->getMCid();
	  fMass = gMC->TrackMass();        // mass (GeV)
	  gMC->TrackPosition(fPosOut);
	  gMC->TrackMomentum(fMomOut);
	  gMC->TrackPosition(fpostotout);  // CHECK (delete this?)
	  Double_t globalPos[3] = {0., 0., 0.}; 
	  Double_t localPos[3] = {0., 0., 0.}; 
	  
	  gMC->Gdtom(localPos, globalPos, 1);
	  
	  fPos.SetXYZM(globalPos[0], globalPos[1], globalPos[2], 0.0);
	  
	  globalPos[0] = fPosOut.X();
	  globalPos[1] = fPosOut.Y();
	  globalPos[2] = fPosOut.Z();
	  
	  gMC->Gmtod(globalPos, localPos, 1);
	  fPosOutLocal.SetXYZM(localPos[0], localPos[1], localPos[2], 0.0);
	  
	  fpostot.SetXYZM((fpostotin.X() +  fpostotout.X())/2., (fpostotin.Y() + fpostotout.Y())/2., (fpostotin.Z() + fpostotout.Z())/2.,0.0);  // CHECK (delete this?)

	  // CHECK -----------------------------------------------------------
	  PndSttMapCreator mapper(fGeoType);
	  Int_t tubeID = mapper.GetTubeIDFromPath(gMC->CurrentVolPath());
	  // -----------------------------------------------------------------

	  AddHit(fTrackID, fVolumeID, tubeID,
	       TVector3(fpostot.X(), fpostot.Y(), fpostot.Z()),
	       TVector3(fPosInLocal.X(),   fPosInLocal.Y(),   fPosInLocal.Z()),
	       TVector3(fPosOutLocal.X(),  fPosOutLocal.Y(),  fPosOutLocal.Z()),
	       TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	       TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	       fTime, fLength, fELoss, fMass); 
	    
	  // Increment number of stt points for TParticle
	  PndStack* stack = (PndStack*) gMC->GetStack();
          stack->AddPoint(kSTT);
	  ResetParameters();
	}
    }
 
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void PndStt::EndOfEvent() 
{
    if (fVerboseLevel) 
	Print();
    fSttCollection->Delete();
    fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Public method Register   ----------------------------------------
void PndStt::Register() 
{
    FairRootManager::Instance()->Register("STTPoint", "Stt", fSttCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   -----------------------------------
TClonesArray* PndStt::GetCollection(Int_t iColl) const 
{
    if (iColl == 0) 
	return fSttCollection;
    else 
	return NULL;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndStt::Print() const 
{
    Int_t 
	nHits = fSttCollection->GetEntriesFast();
    
    cout << "-I- PndStt: " << nHits << " points registered in this event."  << endl;
    
    if (fVerboseLevel>1)
	for (Int_t i=0; i<nHits; i++) 
	    (*fSttCollection)[i]->Print();
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void PndStt::Reset() 
{
    fSttCollection->Delete();
    ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void PndStt::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t 
	nEntries = cl1->GetEntriesFast();
    
    cout << "-I- PndStt: " << nEntries << " entries to add." << endl;
    
    TClonesArray& clref = *cl2;
    
    PndSttPoint
	*oldpoint = NULL;
    for (Int_t i=0; i<nEntries; i++) 
    {
	oldpoint = (PndSttPoint*) cl1->At(i);
	
	Int_t 
	    index = oldpoint->GetTrackID() + offset;
	
	oldpoint->SetTrackID(index);
	new (clref[fPosIndex]) PndSttPoint(*oldpoint);
	fPosIndex++;
    }
    cout << "-I- PndStt: " << cl2->GetEntriesFast() << " merged entries." 
	 << endl;
}
// -------------------------------------------------------------------------



// -----   Public method ConstructGeometry   -------------------------------
void PndStt::ConstructGeometry() 
{
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  PndGeoStt*       Geo  = new  PndGeoStt();
  Geo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(Geo);

  Bool_t rc = geoFace->readSet(Geo);
  if (rc) Geo->create(geoLoad->getGeoBuilder());

  // store geo parameter with PndSttMapCreator
  TList* volList = Geo->getListOfVolumes();
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  PndGeoSttPar* par=(PndGeoSttPar*)(rtdb->getContainer("PndGeoSttPar"));
  TListIter iter(volList);
  // CHECK 
  if(GetGeometryFileName().Contains("straws_skewed_blocks_35cm_pipe.geo")) fGeoType = 1; 
  else cout << "-E- STT: this geometry is not supported now" << endl;

  PndSttMapCreator mapper(fGeoType);
  int tubecounter =  mapper.FillSttTubeParameters(par, volList);
  cout << "-I- STT total number of tubes: " << tubecounter << endl;
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);
  
  ProcessNodes ( volList );
  
}
// -------------------------------------------------------------------------




// -----   Private method AddHit   -----------------------------------------
PndSttPoint* PndStt::AddHit(Int_t trackID, Int_t detID, Int_t tubeID,
			    TVector3 pos, TVector3 posInLocal, TVector3 posOutLocal, 
			    TVector3 momIn, TVector3 momOut,
			    Double_t time, Double_t length, Double_t eLoss,  Double_t mass)
{
  TClonesArray& 
      clref = *fSttCollection;

  Int_t 
      size = clref.GetEntriesFast();

  PndSttPoint *pointnew = new(clref[size]) PndSttPoint(trackID, detID, pos, posInLocal, posOutLocal,
						       momIn, momOut, time, 
						       length, eLoss, mass);
  pointnew->SetTubeID(tubeID);
}
// -------------------------------------------------------------------------

 

ClassImp(PndStt)
    
