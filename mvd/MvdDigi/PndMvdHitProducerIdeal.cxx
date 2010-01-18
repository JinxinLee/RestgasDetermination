// -------------------------------------------------------------------------
// -----               PndMvdHitProducerIdeal source file             -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"

#include "FairRootManager.h"
#include "PndMvdHitProducerIdeal.h"
#include "PndMvdHit.h"
#include "PndMvdHitInfo.h"
#include "PndMvdMCPoint.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"


// -----   Default constructor   -------------------------------------------
PndMvdHitProducerIdeal::PndMvdHitProducerIdeal() :
  FairTask("Ideal MVD Hit Producer")
{
	fBranchName 	= "MVDPoint";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndMvdHitProducerIdeal::~PndMvdHitProducerIdeal()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdHitProducerIdeal::Init()
{
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdHitProducerIdeal::Init: "
	   << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject(fBranchName);

  if ( ! fPointArray )
    {
      std::cout << "-W- PndMvdHitProducerIdeal::Init: "
	   << "No MVDPoint array!" << std::endl;
      return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndMvdHit");
  ioman->Register("MVDHit", "MVD", fHitArray, kTRUE);

  std::cout << "-I- PndMvdHitProducerIdeal: Intialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndMvdHitProducerIdeal::SetParContainers()
{
  // Get Base Container
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMvdHitProducerIdeal::Exec(Option_t* opt)
{
  // Reset output array
  if ( ! fHitArray )
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();

  // Declare some variables
  PndMvdMCPoint *point = 0;

  Int_t
    detID = 0,       // Detector ID
    trackID = 0;     // Track index

  TVector3
    pos, dpos;       // Position and error vectors

  // Loop over PndMvdMCPoints
  Int_t
    nPoints = fPointArray->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++)
    {
      point = (PndMvdMCPoint*) fPointArray->At(iPoint);
//	std::cout << " Ideal Hit Producer -Point-: " << point << std::endl;
      if ( ! point)
	continue;

      // Detector ID
      detID = point->GetDetectorID();

      // MCTrack ID
      trackID = point->GetTrackID();

     TVector3
	position(point->GetX(), point->GetY(), point->GetZ());
	dpos.SetXYZ(0.005,0.005,0.005);


      // Create new hit
      new ((*fHitArray)[iPoint]) PndMvdHit(detID, point->GetDetName(), position, dpos, -1, point->GetEnergyLoss(),1, iPoint);
//	std::cout << "Hit created for module: " << point->GetDetName() << std::endl;



    }   // Loop over MCPoints

  // Event summary
//  std::cout << "-I- PndMvdHitProducerIdeal: " << nPoints << " PndMvdMCPoints, "
 //      << nPoints << " Hits created." << std::endl;

}
// -------------------------------------------------------------------------


ClassImp(PndMvdHitProducerIdeal)
