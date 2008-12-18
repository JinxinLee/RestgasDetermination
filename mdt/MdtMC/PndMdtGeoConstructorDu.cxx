
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

#include "CbmVolume.h"
// add on for debug
#include "CbmGeoG3Builder.h"
#include "CbmRuntimeDb.h"
#include "CbmRun.h"
#include "CbmModule.h"

#include "PndMdt.h"


using namespace std;


// -----   Public method ConstructGeometry   ----------------------------------
void PndMdt::ConstructGeometryDu() 
{
}
// ----------------------------------------------------------------------------


// -----   Public method ProcessHits  --------------------------------------
Bool_t PndMdt::ProcessHitsDu(CbmVolume* vol) 
{
  return kTRUE;
}
// ----------------------------------------------------------------------------

ClassImp(PndMdt)
