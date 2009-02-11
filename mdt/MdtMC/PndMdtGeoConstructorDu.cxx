
#include <iostream>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TParticle.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoArb8.h"
#include "TGeoTrd2.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"

#include "FairVolume.h"
// add on for debug
#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairModule.h"

#include "PndMdt.h"


using namespace std;


// -----   Public method ConstructGeometry   ----------------------------------
void PndMdt::ConstructGeometryDu() 
{
}
// ----------------------------------------------------------------------------


// -----   Public method ProcessHits  --------------------------------------
Bool_t PndMdt::ProcessHitsDu(FairVolume* vol) 
{
  return kTRUE;
}
// ----------------------------------------------------------------------------

ClassImp(PndMdt)
