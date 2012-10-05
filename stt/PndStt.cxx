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
	    gMC->TrackPosition(fpostotin);// da cancellare
	    Double_t globalPos[3] = {0., 0., 0.}; // stt1 modified
	    Double_t localPos[3] = {0., 0., 0.}; // stt1 modified
	    
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
	  fVolumeID = kSTT;//vol->getMCid();
	  fMass = gMC->TrackMass();   // mass (GeV)
	  gMC->TrackPosition(fPosOut);
	  gMC->TrackMomentum(fMomOut);
	  gMC->TrackPosition(fpostotout);// da cancellare
	  Double_t globalPos[3] = {0., 0., 0.}; // stt1 modified
	  Double_t localPos[3] = {0., 0., 0.}; // stt1 modified
	  
	  gMC->Gdtom(localPos, globalPos, 1);
	  
	  fPos.SetXYZM(globalPos[0], globalPos[1], globalPos[2], 0.0);
	  
	  globalPos[0] = fPosOut.X();
	  globalPos[1] = fPosOut.Y();
	  globalPos[2] = fPosOut.Z();
	  
	  gMC->Gmtod(globalPos, localPos, 1);
	  fPosOutLocal.SetXYZM(localPos[0], localPos[1], localPos[2], 0.0);
	  
	  
	  // string basename("stt1tube");
	  string basename;
	  TString volumename;
	  string
	    hashmark("#"),
	    volName,
	    fullName,
	    number,
	    specialname,
	    volPath(gMC->CurrentVolPath()),
	    volPath2(gMC->CurrentVolPath());
	  
	  volumename = volPath;
	  volName = GetStringPart(volPath, 3, '/');
	  number = GetStringPart(volName, 1, '_');	
	  
	  Int_t start =volPath2.find("stt01tube",0);
	  specialname = volPath2.substr(start,volPath2.find("_",start)-start);

	  if(volumename.Contains("stt01")) basename = "stt01tube";
// 	  if(volumename.Contains("stt02")) basename = "stt02tube";
// 	  if(volumename.Contains("stt03")) basename = "stt03tube";
// 	  if(volumename.Contains("stt04")) basename = "stt04tube";
// 	  if(volumename.Contains("stt05")) basename = "stt05tube";
// 	  if(volumename.Contains("stt06")) basename = "stt06tube";
// 	  if(volumename.Contains("stt07")) basename = "stt07tube";
// 	  if(volumename.Contains("stt08")) basename = "stt08tube";
// 	  if(volumename.Contains("stt09")) basename = "stt09tube";
// 	  if(volumename.Contains("stt10")) basename = "stt10tube";
// 	  if(volumename.Contains("stt11")) basename = "stt11tube";
// 	  if(volumename.Contains("stt12")) basename = "stt12tube";
// 	  if(volumename.Contains("stt13")) basename = "stt13tube";
// 	  if(volumename.Contains("stt14")) basename = "stt14tube";
// 	  if(volumename.Contains("stt15")) basename = "stt15tube";
	  
	  fullName = basename + hashmark + number;
	  
	  //  cout << "volname: " << volName <<  " " << volumename << endl;
	  // 	    cout << "number: " << number << endl;
	  //      cout << gMC->CurrentVolPath() << endl;
	  // 	    cout << "fullname: " << fullName << endl;
 	
	  FairGeoNode 
	    *volnode = dynamic_cast<FairGeoNode*> (fPassNodes->FindObject(fullName.c_str()));
	    
	  if(number=="0") {
	    volnode = dynamic_cast<FairGeoNode*> (fPassNodes->FindObject(specialname.c_str()));
	    //cout<<">>>>"<<endl;
	    //cout<<"special "<<specialname.c_str()<<endl;
	  }
	    
	  if (!volnode)
	    {
	      cout << "-I- PndStt: No volume " << fullName.c_str() << " found in geometry container."  << endl;
	      return kFALSE;
	    }
	    


	  //   cout << "positionc: " << fPos.X() << " " << fPos.Y() << " " << fPos.Z() << endl;	// da cancellare
	  //    cout << "position: " << fpostot.X() << " " << fpostot.Y() << " " << fpostot.Z() << endl;
	  
	  // if(sqrt(fPosInLocal.X()*fPosInLocal.X() + fPosInLocal.Y()*fPosInLocal.Y()) < 0.45) {
	  // 	      cout << "position in : " << sqrt(fPosInLocal.X()*fPosInLocal.X() + fPosInLocal.Y()*fPosInLocal.Y()) << endl;
	  // 	    }
	  // 	    if(sqrt(fPosOutLocal.X()*fPosOutLocal.X() + fPosOutLocal.Y()*fPosOutLocal.Y()) < 0.45)  {
	  // 	      cout << "position out: " << sqrt(fPosOutLocal.X()*fPosOutLocal.X() + fPosOutLocal.Y()*fPosOutLocal.Y()) << endl;

	  // 	    }
 	//for ex2
	fpostot.SetXYZM((fpostotin.X() +  fpostotout.X())/2., (fpostotin.Y() + fpostotout.Y())/2., (fpostotin.Z() + fpostotout.Z())/2.,0.0); // da cancellare
	//for GFKalman
	//fpostot.SetXYZM(fpostotout.X(), fpostotout.Y(), fpostotout.Z(), 0.0);


	  //  cout << "in : " << fpostotin.X() << " " << fpostotin.Y() << " " << fpostotin.Z() << endl;
	  // 	    cout << "out: " << fpostotout.X() << " " << fpostotout.Y() << " " << fpostotout.Z() << endl;
	  // 	    cout << "tot: " << fpostot.X() << " " << fpostot.Y() << " " << fpostot.Z() << endl;
	  // 	    cout << (fpostotin.X() +  fpostotout.X())/2. << " " << (fpostotin.Y() + fpostotout.Y())/2. << " " << (fpostotin.Z() + fpostotout.Z())/2. << endl;

	// CHECK -----------------------------------------------------------
	PndSttMapCreator *mapper = new PndSttMapCreator(fGeoType);
	Int_t tubeID = mapper->GetTubeIDFromPath(gMC->CurrentVolPath());
	//	cout << gMC->CurrentVolPath() << endl;
	//	cout << "tubeID " << tubeID << " " << mapper->GetNameFromTubeID(tubeID) << endl;
	//	cout << fPos.X() << " " << fPos.Y() << " " << fPos.Z() << endl;
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

  TList* volList = Geo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  PndGeoSttPar* par=(PndGeoSttPar*)(rtdb->getContainer("PndGeoSttPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;
  
  
  while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
       if ( node->isSensitive()  ) {
           fSensNodes->AddLast( aVol );
       }else{
           fPassNodes->AddLast( aVol );
       }
  }

  // CHECK to be changed: put a dependance on which file ------
  // is used, for the moment let' s keep it this way!
  fGeoType = 1; // CHECK
  par->SetGeometryType(fGeoType);
  par->SetTubeInRad(0.5);    // cm
  par->SetTubeOutRad(0.001); // cm
  // ----------------------------------------------------------

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
    
