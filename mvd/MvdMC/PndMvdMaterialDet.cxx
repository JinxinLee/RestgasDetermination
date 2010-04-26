#include "PndMvdMaterialDet.h"

#include "PndMvdGeo.h"
#include "PndMvdGeoPar.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairGeoMedia.h"
#include "FairGeoVolume.h"
#include "FairRunSim.h"
#include "FairVolume.h"


#include "PndMvdMCPoint.h"
#include "PndMvdGeo.h"
#include "PndMvdGeoPar.h"

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TList.h"
#include "TKey.h"
#include "TGeoManager.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"

#include "PndGeoHandling.h"

#include <iostream>
#include <string>
#include <sstream>

class FairVolume;


// -----   Default constructor   -------------------------------------------
PndMvdMaterialDet::PndMvdMaterialDet() {
  fPndMvdCollection = new TClonesArray("PndMvdMCPoint");
  fPosIndex = 0;
  fVerboseLevel = 1;
  fListOfSensitives.push_back("Disk-Sensor");//Root_Test.root
  fListOfSensitives.push_back("Barrel-Sensor");//Root_Test.root
  fListOfSensitives.push_back("StripSensor");//MVD14.root
  fListOfSensitives.push_back("SensorActiveArea");//MVD14.root
  if (fVerboseLevel>0) {
    std::cout<<"-I- PndMvdMaterialDet: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  }
  fTotRadl=0;
  fTotAbso=0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndMvdMaterialDet::PndMvdMaterialDet(const char* name, Bool_t active)
  : FairDetector(name, active) {
  fPndMvdCollection = new TClonesArray("PndMvdMCPoint");
  fPosIndex = 0;
  fVerboseLevel = 1;
  fListOfSensitives.push_back("Disk-Sensor");//Root_Test.root
  fListOfSensitives.push_back("Barrel-Sensor");//Root_Test.root
  fListOfSensitives.push_back("StripSensor");//MVD14.root
  fListOfSensitives.push_back("SensorActiveArea");//MVD14.root
  if (fVerboseLevel>0) {
    std::cout<<"- I - PndMvdMaterialDet: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  }
  fTotRadl=0;
  fTotAbso=0;
}
// -------------------------------------------------------------------------




// -----   Destructor   ----------------------------------------------------
PndMvdMaterialDet::~PndMvdMaterialDet()
{
  if (fPndMvdCollection)
    {
      fPndMvdCollection->Delete();
      delete fPndMvdCollection;
    }
  delete fGeoH;
}
// -------------------------------------------------------------------------
void PndMvdMaterialDet::Initialize()
{
  FairDetector::Initialize();
  fGeoH = new PndGeoHandling();
}


// -----   Public method ProcessHits  --------------------------------------
Bool_t  PndMvdMaterialDet::ProcessHits(FairVolume* vol)
{
  TParticle* currPart = gMC->GetStack()->GetCurrentTrack();

  Float_t a,z,dens,radl,absl;
  Float_t step  = gMC->TrackStep();

  gMC->CurrentMaterial(a,z,dens,radl,absl);
   if(step) {
      if(absl!=0) fTotAbso += step/absl;
      if(radl!=0) fTotRadl += step/radl;
   }



  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {

      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();

      if (fVerboseLevel > 1){
        std::cout << "*******  Info from gMC *************" << std::endl;
        std::cout << "Hit in " << gMC->CurrentVolPath() << " with MCiD: " << vol->getMCid() << " volumeID: " << fVolumeID << std::endl;
        std::cout<<"VolumeID: "<<fGeoH->GetID(gMC->CurrentVolPath())<<std::endl;
        std::cout<<"Radation Lenght is: "<<fTotRadl<<std::endl;
        std::cout<<"Particle Name is: "<< currPart->GetName() <<std::endl;
      }
/// TODO put the summed radiation length into a container here
    }
//}
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void PndMvdMaterialDet::EndOfEvent()
{
  if (fVerboseLevel)
    Print();

  fPndMvdCollection->Clear();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------

void PndMvdMaterialDet::FinishRun()
{
}

// -----   Public method Register   ----------------------------------------
void PndMvdMaterialDet::Register()
{
  FairRootManager::Instance()->Register("MVDPoint", "PndMvd", fPndMvdCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   -----------------------------------
TClonesArray* PndMvdMaterialDet::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fPndMvdCollection;
  else
    return NULL;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndMvdMaterialDet::Print() const
{
  Int_t
    nHits = fPndMvdCollection->GetEntriesFast();

  std::cout << "-I- PndMvdMaterialDet: " << nHits << " points registered in this event."  << std::endl;
  std::cout<<"Total Radation Lenght is: "<<fTotRadl<<std::endl;
  std::cout<<"Total Absorption Lenght is: "<<fTotAbso<<std::endl;

  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++)
      (*fPndMvdCollection)[i]->Print();

}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void PndMvdMaterialDet::Reset()
{
  fPndMvdCollection->Clear();
//   ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void PndMvdMaterialDet::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t
    nEntries = cl1->GetEntriesFast();

  std::cout << "-I- PndMvdMaterialDet: " << nEntries << " entries to add." << std::endl;

  TClonesArray& clref = *cl2;

  PndMvdMCPoint
    *oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++)
     {
       oldpoint = (PndMvdMCPoint*) cl1->At(i);

       Int_t
   index = oldpoint->GetTrackID() + offset;

       oldpoint->SetTrackID(index);
       new (clref[fPosIndex]) PndMvdMCPoint(*oldpoint);
       fPosIndex++;
     }
   std::cout << "-I- PndMvdMaterialDet: " << cl2->GetEntriesFast() << " merged entries."
  << std::endl;
}

// -------------------------------------------------------------------------
void PndMvdMaterialDet::ConstructGeometry()
{
  TString fileName=GetGeometryFileName();
        if(fileName.EndsWith(".geo")){
    ConstructASCIIGeometry();
  }else if(fileName.EndsWith(".root")){
    ConstructRootGeometry();
  }else{
    std::cout<< "Geometry format not supported " <<std::endl;
  }
}

// -------------------------------------------------------------------------
bool PndMvdMaterialDet::CheckIfSensitive(std::string name) const
{
// make all volumes 'sensitive' to get every material involved

  return true;

}


void PndMvdMaterialDet::ConstructRootGeometry()
{

  TFile *f=new TFile(GetGeometryFileName().Data());
  TList *l= f->GetListOfKeys();
  TKey *k=(TKey *) l->At(0);
  TGeoVolume *Stt2=(TGeoVolume *)k->ReadObj();
  TGeoNode *n=Stt2->GetNode(0);
  TGeoVolume *v1=n->GetVolume();
  TGeoVolume *Cave= gGeoManager->GetTopVolume();
  gGeoManager->AddVolume(v1);
        TGeoVoxelFinder *voxels = v1->GetVoxels();
        if (voxels) voxels->SetNeedRebuild();
        TGeoMatrix *M = n->GetMatrix();
        M->SetDefaultName();
        gGeoManager->GetListOfMatrices()->Remove(M);
        TGeoHMatrix *global = gGeoManager->GetHMatrix();
        gGeoManager->GetListOfMatrices()->Remove(global); //Remove the Identity matrix
  Cave->AddNode(v1,0, M);
        ExpandNode(n);
        delete f;

}

void PndMvdMaterialDet::ExpandNode(TGeoNode *fN){

  FairGeoLoader*geoLoad = FairGeoLoader::Instance();
      FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

        TGeoMatrix *Matrix =fN->GetMatrix();
  if(gGeoManager->GetListOfMatrices()->FindObject(Matrix))gGeoManager->GetListOfMatrices()->Remove(Matrix);
  TGeoVolume *v1=fN->GetVolume();
  //v1->RegisterYourself();
        TObjArray *NodeList=v1->GetNodes();
  //Int_t medId=MediaArray1->GetSize();
    for (Int_t Nod=0; Nod<NodeList->GetEntriesFast();Nod++) {
    TGeoNode *fNode =(TGeoNode *)NodeList->At(Nod);
                TGeoMatrix *M =fNode->GetMatrix();
                M->SetDefaultName();
                if(fNode->GetNdaughters()>0) ExpandNode(fNode);
    TGeoVolume *v= fNode->GetVolume();
    Int_t MatId=0;
    TGeoMedium* med1=v->GetMedium();
    if(med1){
      TGeoMaterial*mat1=v->GetMaterial();
      TGeoMaterial *newMat = gGeoManager->GetMaterial(mat1->GetName());
      if( newMat==0){
        std::cout<< "Material " << mat1->GetName() << " is not defined " << std::endl;
        FairGeoMedium *CbmMedium=Media->getMedium(mat1->GetName());
        if (!CbmMedium) {
          std::cout << "Material is not defined in ASCII file nor in Root file" << std::endl;
          CbmMedium=new FairGeoMedium(mat1->GetName());
          Media->addMedium(CbmMedium);
        }
        std::cout << "Create Medium " << mat1->GetName() << std::endl;
        Int_t nmed=geobuild->createMedium(CbmMedium);
        v->SetMedium(gGeoManager->GetMedium(nmed));
        gGeoManager->SetAllIndex();

      }else{
        TGeoMedium *med2= gGeoManager->GetMedium(mat1->GetName());
        v->SetMedium(med2);
      }
    }
    if (!gGeoManager->FindVolumeFast(v->GetName())) {
      if (fVerboseLevel>2)std::cout << "Register Volume : " << v->GetName() << " id "  << std::endl;
      v->RegisterYourself();
    }
    if (CheckIfSensitive(v->GetName())){
      if (fVerboseLevel>2){
              std::cout << "Sensitive Volume : " << v->GetName() << " id "  << std::endl;
            }
      AddSensitiveVolume(v);
    }

  }
}


// -------------------------------------------------------------------------


// -----   Public method ConstructGeometry   -------------------------------
void PndMvdMaterialDet::ConstructASCIIGeometry()
{
  // get pointer to the instantons which interface
  // to monte carlo

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  PndMvdGeo *thePndMvdGeo  = new PndMvdGeo();

  thePndMvdGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(thePndMvdGeo);

  Bool_t rc = geoFace->readSet(thePndMvdGeo);

  if (rc)
    thePndMvdGeo->create(geoLoad->getGeoBuilder());

  TList* volList = thePndMvdGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();

  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();

  PndMvdGeoPar *par= (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();

  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);

  FairGeoNode   *node = NULL;
  FairGeoVolume *aVol = NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
//        if ( node->isSensitive()  ) {
           fSensNodes->AddLast( aVol );
//        }else{
//            fPassNodes->AddLast( aVol );
//        }
  }

  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  ProcessNodes ( volList );
}
// -------------------------------------------------------------------------


// -----   Private method AddHit   -----------------------------------------
PndMvdMCPoint* PndMvdMaterialDet::AddHit(Int_t trackID, Int_t detID, TString detName, TVector3 posIn,              TVector3 posOut,TVector3 momIn, TVector3 momOut,
            Double_t time, Double_t length, Double_t eLoss) const
{
  TClonesArray&
    clref = *fPndMvdCollection;

  Int_t
    size = clref.GetEntriesFast();

    if (fVerboseLevel >= 2)
       std::cout << "-I- PndMvdMaterialDet: Adding Point at (" << posIn.X() << ", " << posIn.Y()
      << ", " << posIn.Z() << ") cm, (" << posOut.X() << ", " << posOut.Y()
      << ", " << posOut.Z() << ") cm,  detector " << detName << " " << detID << ", track "
      << trackID << ", energy loss " << eLoss*1e06 << " keV" << std::endl;

  return new(clref[size]) PndMvdMCPoint(trackID, detID, detName, posIn, posOut,
                        momIn, momOut, time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(PndMvdMaterialDet);
