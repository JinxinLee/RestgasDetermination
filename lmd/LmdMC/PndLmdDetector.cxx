/*
 * PndLmdDetector.cxx
 */

#include "PndLmdDetector.h"
#include "PndLmdGeo.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRun.h"

#include "TClonesArray.h"
#include "TGeoManager.h"

#include "PndSdsGeoPar.h"



// -----   Default constructor   -------------------------------------------
PndLmdDetector::PndLmdDetector()
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndLmdDetector::PndLmdDetector (const char* name, Bool_t active)
  : PndSdsDetector(name, active) {

 }// -------------------------------------------------------------------------


void PndLmdDetector::SetDefaultSensorNames(){
  fListOfSensitives.push_back("lmdTrapStrip");

  if (fVerboseLevel>0) {
    std::cout<<"- I - PndLmdDetector: fListOfSensitives contains:";
    for(UInt_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  }
}

// -----   Destructor   ----------------------------------------------------
PndLmdDetector::~PndLmdDetector()
{
  if (fPndSdsCollection)
    {
      fPndSdsCollection->Delete();
      delete fPndSdsCollection;
    }
  delete fGeoH;
}


void PndLmdDetector::SetBranchNames(TString outBranchname, TString folderName)
{
  fOutBranchName = outBranchname;
  fFolderName = folderName;
}


void PndLmdDetector::SetBranchNames()
{
  fOutBranchName = "LMDPoint";
  fFolderName = "PndLmd";
}


void PndLmdDetector::SetSpecialPhysicsCuts()
{  // Switched off, default of the MC engine is used
  return;
}
// -------------------------------------------------------------------------
void PndLmdDetector::ConstructASCIIGeometry()
{

	  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
	  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	  PndLmdGeo *thePndSdsGeo  = new PndLmdGeo();

	  thePndSdsGeo->setGeomFile(GetGeometryFileName());
	  geoFace->addGeoModule(thePndSdsGeo);

	  Bool_t rc = geoFace->readSet(thePndSdsGeo);

	  if (rc)
	    thePndSdsGeo->create(geoLoad->getGeoBuilder());

	  TList* volList = thePndSdsGeo->getListOfVolumes();

	  // store geo parameter
	  FairRun *fRun = FairRun::Instance();
	  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
	  PndSdsGeoPar *par= (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));
	  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
	  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

	  TListIter iter(volList);

	  FairGeoNode   *node = NULL;
	  FairGeoVolume *aVol = NULL;

	  while( (node = (FairGeoNode*)iter.Next()) ) {
	    aVol = dynamic_cast<FairGeoVolume*> ( node );
	    if ( node->isSensitive()  ) {
	      fSensNodes->AddLast( aVol );
	    }else{
	      fPassNodes->AddLast( aVol );
	    }
	  }

	  par->setChanged();
	  par->setInputVersion(fRun->GetRunId(),1);

	  ProcessNodes ( volList );
}

ClassImp(PndLmdDetector);
