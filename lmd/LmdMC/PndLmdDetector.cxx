#include "PndLmdDetector.h"

#include "TClonesArray.h"


// -----   Default constructor   -------------------------------------------
PndLmdDetector::PndLmdDetector() {
  fPndSdsCollection = new TClonesArray("PndSdsMCPoint");
  fPosIndex = 0;
  fListOfSensitives.push_back("LumActive");//Lumi
  //if (fVerboseLevel>0) {
    std::cout<<"-I- PndLmdDetector: fListOfSensitives contains:";
    for(size_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  //}
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndLmdDetector::PndLmdDetector (const char* name, Bool_t active)
  : PndSdsDetector(name, active) {
  fPndSdsCollection = new TClonesArray("PndSdsMCPoint");
  fPosIndex = 0;
  fListOfSensitives.push_back("LumActive");//Lumi
  //if (fVerboseLevel>0) {
    std::cout<<"- I - PndLmdDetector: fListOfSensitives contains:";
    for(size_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  //}
}
// -------------------------------------------------------------------------


void PndLmdDetector::SetBranchNames(char* outBranchname, char* folderName)
{
  fOutBranchName = outBranchname;
  fFolderName = folderName;
}


void PndLmdDetector::SetBranchNames()
{
  fOutBranchName = "LMDPoint";
  fFolderName = "PndLmd";
}

void PndLmdDetector::SetDefaultSensorNames()
{
  fListOfSensitives.push_back("LumActive");
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
// -------------------------------------------------------------------------

ClassImp(PndLmdDetector);
