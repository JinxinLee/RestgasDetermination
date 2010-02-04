#include "PndLmdDetector.h"

#include "TClonesArray.h"


// -----   Default constructor   -------------------------------------------
PndLmdDetector::PndLmdDetector() {
  fPndSdsCollection = new TClonesArray("PndSdsMCPoint");
  fPosIndex = 0;
  fListOfSensitives.push_back("Disk-Sensor");//Root_Test.root
  fListOfSensitives.push_back("Barrel-Sensor");//Root_Test.root
  fListOfSensitives.push_back("PixelActive");//Root_Test.root
  fListOfSensitives.push_back("StripSensor");//MVD14.root
  fListOfSensitives.push_back("SensorActiveArea");//MVD14.root
  fListOfSensitives.push_back("StripActive");//MVD_v1.0.root
  fListOfSensitives.push_back("PixelActive");//MVD_v1.0.root
  //if (fVerboseLevel>0) {
    std::cout<<"-I- PndLmdDetector: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
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
  fListOfSensitives.push_back("Disk-Sensor");//Root_Test.root
  fListOfSensitives.push_back("Barrel-Sensor");//Root_Test.root
  fListOfSensitives.push_back("PixelActive");
  fListOfSensitives.push_back("StripActive");
  fListOfSensitives.push_back("StripSensor");//MVD14.root
  fListOfSensitives.push_back("SensorActiveArea");//MVD14.root
  fListOfSensitives.push_back("StripActive");//MVD_v1.0.root
  fListOfSensitives.push_back("PixelActive");//MVD_v1.0.root
  //if (fVerboseLevel>0) {
    std::cout<<"- I - PndLmdDetector: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  //}
}
// -------------------------------------------------------------------------


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
