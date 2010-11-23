/////////////////////////////////////////////////////////////
// PndTpcTBDetector
//
// Class for PndTpcTBDetector
//
/////////////////////////////////////////////////////////////

#include "PndTpcTBDetector.h"

#include "FairRootManager.h"
#include "FairVolume.h"

#include "PndDetectorList.h"
#include "PndTpcTBMCPoint.h"
#include "PndStack.h"
#include "PndTpcAlignmentManager.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"

#include <iostream>
#include <string>
#include <sstream>

class FairGeoLoader;
class FairGeoMedia;
class FairRunSim;
class FairGeoInterface;
class FairGeoRootBuilder;
class FairGeoNode;
class FairRun;
class FairRuntimeDb;
class FairGeoVolume;

class TGeoVoxelFinder;
class TObjArray;
class TList;
class TGeoMatrix;
class TParticle;
class TLorentzVector;
class TKey;


// -----   Default constructor   -------------------------------------------
PndTpcTBDetector::PndTpcTBDetector() : fUseRadDamOption(false) {
  fPndTpcTBCollection = new TClonesArray("PndTpcTBMCPoint");
  fPosIndex = 0;
  fListOfSensitives.push_back("Sensor");
  if (fVerboseLevel>0) {
    std::cout<<"-I- PndTpcTBDetector: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  }
  alMan=PndTpcAlignmentManager::getInstance();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndTpcTBDetector::PndTpcTBDetector (const char* name, Bool_t active)
  : FairDetector(name, active), fUseRadDamOption(false) {
  fPndTpcTBCollection = new TClonesArray("PndTpcTBMCPoint");
  fPosIndex = 0;
  fListOfSensitives.push_back("Sensor");//Probably not needed
}
// -------------------------------------------------------------------------




// -----   Destructor   ----------------------------------------------------
PndTpcTBDetector::~PndTpcTBDetector()
{
  if (fPndTpcTBCollection)
    {
      fPndTpcTBCollection->Delete();
      delete fPndTpcTBCollection;
    }
//   delete fGeoH;
}
// -------------------------------------------------------------------------
void PndTpcTBDetector::Initialize()
{
  std::cout<<" -I- Initializing PndTpcTBDetector()"<<std::endl;
  FairDetector::Initialize();
  if(0==gGeoManager) {
    std::cout<<" -E- No gGeoManager in PndTpcTBDetector::Initialize()!"
	     <<std::endl;
    abort();
  }
//   fGeoH = new PndTpcTBGeoHandling(gGeoManager);
}


// -----   Public method ProcessHits  --------------------------------------
Bool_t  PndTpcTBDetector::ProcessHits(FairVolume* vol)
{
  if ( gMC->IsTrackEntering() ) {
    // Set parameters at entrance of volume. Reset ELoss.
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  // What is the active volume?
  fELoss += gMC->Edep();
  // Create PndTpcTBMCPoint at exit of active volume

  if (gMC->IsTrackExiting()    ||
      gMC->IsTrackStop()       ||
      gMC->IsTrackDisappeared()   ) {

      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();

/*      if(0==fGeoH) {
        std::cout<<" -E- No PndTpcTBGeoHandling loaded."<<std::endl;
        abort();
      }*/
      if (fVerboseLevel > 1){
        std::cout <<std::endl<<std::endl<< "*********************************  Info from gMC ********************************" << std::endl;
        std::cout << "Hit in " << gMC->CurrentVolPath()<<std::endl 
                  << " with MCiD: " << vol->getMCid()<<std::endl;
	  //                  << " PixelDetectorID: " << kGEM << std::endl;
	// std::cout <<"VolumeID: "
	//      <<fGeoH->GetID(gMC->CurrentVolPath())<<std::endl;
	  std::cout << "PosIn: " << fPosIn.X() 
                  << " " << fPosIn.Y() 
                  << " " << fPosIn.Z() 
                  << " " << fELoss << std::endl;
      }

      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      if (fUseRadDamOption == false){
    	  if (fELoss == 0.) return kFALSE;
      }

     TString detPath = gMC->CurrentVolPath();
     
     AddHit(fTrackID, kGEM, detPath,//fGeoH->GetID(detPath),
	    TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	    TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	    TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	    TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	    fTime, fLength, fELoss);
     //------------------------------
     //Not needed, only bookkeeping?
     //-------------------------------
     // Increment number of PndTpcTB points for TParticle
     //
     //PndStack* stack = (PndStack*) gMC->GetStack();
     //stack->AddPoint(kGEM);
     
     ResetParameters();
  }
  
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void PndTpcTBDetector::EndOfEvent()
{
  if (fVerboseLevel)
    Print();

  fPndTpcTBCollection->Clear();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------

void PndTpcTBDetector::FinishRun()
{

}

// -----   Public method Register   ----------------------------------------
void PndTpcTBDetector::Register()
{
  FairRootManager::Instance()->Register("GEMPoint", "PndTpcTB", fPndTpcTBCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   -----------------------------------
TClonesArray* PndTpcTBDetector::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fPndTpcTBCollection;
  else
    return NULL;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndTpcTBDetector::Print() const
{
  Int_t
    nHits = fPndTpcTBCollection->GetEntriesFast();

  std::cout << "-I- PndTpcTBDetector: " << nHits << " points registered in this event."  << std::endl;

  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++)
      (*fPndTpcTBCollection)[i]->Print();
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void PndTpcTBDetector::Reset()
{
  fPndTpcTBCollection->Clear();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void PndTpcTBDetector::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t
    nEntries = cl1->GetEntriesFast();

  std::cout << "-I- PndTpcTBDetector: " << nEntries << " entries to add." << std::endl;

  TClonesArray& clref = *cl2;

  PndTpcTBMCPoint *oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++)    {
    oldpoint = (PndTpcTBMCPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndTpcTBMCPoint(*oldpoint);
    fPosIndex++;
  }
  std::cout << "-I- PndTpcTBDetector: " << cl2->GetEntriesFast() 
            << " merged entries." << std::endl;
}

// -------------------------------------------------------------------------
void PndTpcTBDetector::ConstructGeometry(){
  TString fileName=GetGeometryFileName();
//   if(fileName.EndsWith(".geo")){
//     ConstructASCIIGeometry();
//   }else 
  if(fileName.EndsWith(".root")){
    ConstructRootGeometry();
  }else{
    std::cout<< "Geometry format not supported " <<std::endl;
  }
}

// -------------------------------------------------------------------------
bool PndTpcTBDetector::CheckIfSensitive(std::string name){
  for (Int_t i = 0; i < fListOfSensitives.size(); i++){
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}



// -----   Public method SetExclusiveSensorType   -------------------------------
void PndTpcTBDetector::SetExclusiveSensorType(const TString sens)
{
  //Set one exclusive sensor type for testing purposes
  fListOfSensitives.clear();
  fListOfSensitives.push_back(sens.Data());
  std::cout<<"-I- PndTpcTBDetector: Only active sensor type is set to \""<<sens.Data()<<"\","<<std::endl;
  std::cout<<"    this is not a default setting."<<std::endl;
}

// -------------------------------------------------------------------------



// -----   Private method AddHit   -----------------------------------------
PndTpcTBMCPoint* PndTpcTBDetector::AddHit(Int_t trackID, 
                                    Int_t detID, 
                                    TString detName, 
                                    TVector3 posIn, TVector3 posOut,
                                    TVector3 momIn, TVector3 momOut,
                                    Double_t time, Double_t length, 
                                    Double_t eLoss){
  TClonesArray& clref = *fPndTpcTBCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel >= 2)
    std::cout << "-I- PndTpcTBDetector: Adding Point at (" << posIn.X() << ", " << posIn.Y()
              << ", " << posIn.Z() << ") cm,"<<std::endl
	      <<" (" << posOut.X() << ", " << posOut.Y()<< ", " << posOut.Z() << ") cm,"
	      <<" detector " << detName << " " << detID << std::endl
	      <<"track "<< trackID << ", energy loss " << eLoss*1e06 << " keV" << std::endl;

  return new(clref[size]) PndTpcTBMCPoint(trackID, detID, detName, posIn, posOut,
                        momIn, momOut, time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(PndTpcTBDetector);
