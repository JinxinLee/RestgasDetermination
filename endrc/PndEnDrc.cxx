////////////////////////////////////////////////////////////////////////////////
//
//  PndEnDrc
//
//  Interface to virtual monte carlo
//
//  created 2008 D. Glazier, K. Foehl Edinburgh
//
////////////////////////////////////////////////////////////////////////////////
#include "TLorentzVector.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TGeoVoxelFinder.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TLorentzVector.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairRootManager.h"
#include "FairGeoRootBuilder.h"
#include "FairVolume.h"
#include "FairGeoMedia.h" 
#include "FairGeoMedium.h"
#include "FairMCPoint.h"
#include "PndEnDrc.h"
#include "PndEnDrcPoint.h"
#include "PndGeoEnDrc.h"
#include "PndDetectorList.h"
#include "PndStack.h"


#include <iostream>
using std::cout;
using std::endl;

ClassImp(PndEnDrc)

// -----------------------------------------------------------------------------
PndEnDrc::PndEnDrc()
{
  // created a Collection of PndEnDrcPoints

  // this are all points created in the MC
  fEnDrcPointCollection = new TClonesArray("PndEnDrcPoint");
  fEnDrcTrueCollection = new TClonesArray("PndEnDrcPoint");
  fPAR[0]=-4.610;       //X0T0
  fPAR[1]=1.0798;       //X0T1 
  fPAR[2]=-1.32180E-2;  //X0T2
  fPAR[3]=4.14768E-4;   //X0T3
  fPAR[4]=2.2762;       //X1T0
  fPAR[5]=-0.19994;     //X1T1
  fPAR[6]=5.85314E-3;   //X1T2
  fPAR[7]=-5.56410E-5;  //X1T3
  fPAR[8]=-6.29469E-2;  //X2T0
  fPAR[9]=5.79209E-3;   //X2T1
  fPAR[10]=-1.74648E-4; //X2T2
  fPAR[11]=1.69013E-6;  //X2T3
}

// -----------------------------------------------------------------------------
PndEnDrc::PndEnDrc(const char *name, Bool_t active)
  : FairDetector(name, active)
{
  // created a Collection of PndEnDrcPoints

  // this are all points created in the MC
  fEnDrcPointCollection = new TClonesArray("PndEnDrcPoint");
  fEnDrcTrueCollection = new TClonesArray("PndEnDrcPoint");
 //  fPAR[0]=-4.610;       //X0T0
//   fPAR[1]=1.0798;       //X0T1 
//   fPAR[2]=-1.32180E-2;  //X0T2
//   fPAR[3]=4.14768E-4;   //X0T3
//   fPAR[4]=2.2762;       //X1T0
//   fPAR[5]=-0.19994;     //X1T1
//   fPAR[6]=5.85314E-3;   //X1T2
//   fPAR[7]=-5.56410E-5;  //X1T3
//   fPAR[8]=-6.29469E-2;  //X2T0
//   fPAR[9]=5.79209E-3;   //X2T1
//   fPAR[10]=-1.74648E-4; //X2T2
//   fPAR[11]=1.69013E-6;  //X2T3
  fPAR[0]=-19.7950248;       //X0T0
  fPAR[1]=-2.18328738;       //X0T1 
  fPAR[2]=-2.3155317E-2;  //X0T2
  fPAR[3]=5.61529165E-5;   //X0T3
  fPAR[4]=-3.12414162E-2;       //X1T0
  fPAR[5]=5.55434997E-3;     //X1T1
  fPAR[6]=5.91060472E-4;   //X1T2
  fPAR[7]=1.71806601E-5;  //X1T3
  fPAR[8]=2.05682258E-2;  //X2T0
  fPAR[9]=-1.29137924E-4;   //X2T1
  fPAR[10]=-2.46224899E-5; //X2T2
  fPAR[11]=-7.23990605E-7;  //X2T3
}
// -----------------------------------------------------------------------------
PndEnDrc::~PndEnDrc()
{
  // destructor, needs to clean the PointCollection

  if ( 0 != fEnDrcPointCollection) {
    fEnDrcPointCollection->Delete();
    delete fEnDrcPointCollection;
  }
  if ( 0 != fEnDrcTrueCollection) {
    fEnDrcTrueCollection->Delete();
    delete fEnDrcTrueCollection;
  }
}

// -----------------------------------------------------------------------------
PndEnDrcPoint *PndEnDrc::AddPoint(Int_t trackID, Int_t detID, Int_t pdgCode,
				  TVector3 pos, TVector3 mom, Double_t tof,
				  Double_t length, Double_t eLoss,
				  Double_t phote, Double_t fpz, Double_t ang)
{
  
  
  PndStack* stack = (PndStack*) gMC->GetStack();
  stack->AddPoint(kDSK);
  
  TClonesArray& points = *fEnDrcPointCollection;
  Int_t size = points.GetEntriesFast();
  // cout<<"PndEnDrc::AddPoint"<<endl;
  // create a new hit after the last one present in the collection
  
  return new(points[size]) PndEnDrcPoint(trackID, detID, pdgCode, pos, mom, tof, length, eLoss,phote,fpz,ang);
}

// -----------------------------------------------------------------------------
void PndEnDrc::ConstructGeometry()
{
  // Construct the detector using the definitions in the geometry file

  PndGeoEnDrc *dskGeo = new PndGeoEnDrc();
  dskGeo->setGeomFile(GetGeometryFileName());

  // geo interface is the class that reads our geometry file
  FairGeoInterface *geoFace = FairGeoLoader::Instance()->getGeoInterface();
  // geo builder is the class that constructs the geometry
  FairGeoBuilder *geoBuild = FairGeoLoader::Instance()->getGeoBuilder();

    if (GetGeometryFileName().EndsWith(".dat")) {
      std::cout<< "                                               " <<std::endl;
      std::cout<< " ====== EMC::  ConstructASCIIGeometry() ====== " <<std::endl;
      std::cout<< " ============================================= " <<std::endl;
       // Read in geometry file
      geoFace->addGeoModule(dskGeo);
      Bool_t rc = geoFace->readSet(dskGeo);
      // and create the geometry
      if (rc)
	dskGeo->create(geoBuild);
      else
	cout << "[ !! ] PndEnDrc: geometry could not be read!" << endl;

    } else if(GetGeometryFileName().EndsWith(".root")) {
      std::cout<< "                                              " <<std::endl;
      std::cout<< " ====== EMC::  ConstructROOTGeometry() ====== " <<std::endl;
      std::cout<< " ============================================ " <<std::endl;
      ConstructRootGeometry();
    }


  TList *volList = dskGeo->getListOfVolumes();

  // Set active/inactive
  ProcessNodes(volList);
}
// -----------------------------------------------------------------------------
void PndEnDrc::ConstructRootGeometry() {
  TFile *f=new TFile(GetGeometryFileName().Data());
  f->ls();
  TGeoVolume *EnDrc=(TGeoVolume *)f->Get("EnDrc");
  //EnDrc->SetName("EnDrc");//crashes on exit if change name
  TGeoVolume *Cave = gGeoManager->GetTopVolume();
  //TGeoNode *n=EnDrc->GetNode(0); 
  
  gGeoManager->AddVolume(EnDrc);
  TGeoVoxelFinder *voxels = EnDrc->GetVoxels();
  if (voxels) voxels->SetNeedRebuild();
  //Removal of matrices is needed or ROOT crashes after running MC
  //TGeoMatrix *M = n->GetMatrix();
  //M->SetDefaultName();
  //gGeoManager->GetListOfMatrices()->Remove(M);
  TGeoHMatrix *global = gGeoManager->GetHMatrix();             
  gGeoManager->GetListOfMatrices()->Remove(global); //Remove the Identity matrix 
  TGeoTranslation* pos=static_cast<TGeoTranslation*>(f->Get("EnDrcPos"));
  pos->Print();
  pos->SetDefaultName();
  Cave->AddNode(EnDrc,1,pos);
  cout<<"PndEnDrc::ConstructRootGeometry() "<<Cave->GetNodes()->Last()->GetName()<<endl;
  ExpandNode(static_cast<TGeoNode*>(Cave->GetNodes()->Last()));

  // f->Close();
  // delete f;


}
// void PndEnDrc::ExpandNode(TGeoVolume *fVol, TGeoVolume *Cave){
  
//   FairGeoLoader*geoLoad = FairGeoLoader::Instance();
//   FairGeoInterface *geoFace = geoLoad->getGeoInterface();
//   FairGeoMedia *Media = geoFace->getMedia();
//   FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();
   
//   TObjArray *nodeList=fVol->GetNodes();  
//   Int_t nodes = nodeList->GetEntries();

//   for (Int_t nod=0; nod < nodes; nod++){
    
//     TGeoNode *fNode =(TGeoNode *)nodeList->At(nod);  
//     TGeoMatrix *Matrix =fNode->GetMatrix();
//     Matrix->SetDefaultName();
//     if(gGeoManager->GetListOfMatrices()->FindObject(Matrix))gGeoManager->GetListOfMatrices()->Remove(Matrix);
//     TGeoVolume *v= fNode->GetVolume();
    
//      if(fNode->GetNdaughters()>0) 	
//       ExpandNode(v,Cave);

//     TGeoMedium* med1=v->GetMedium();
//     std::cout<< "DEBUG NodeName = " << fNode->GetName() << std::endl;
//     if (med1) {
//       std::cout<< "DEBUG medium  = " << med1->GetName() << std::endl;
//       TGeoMaterial*mat1=v->GetMaterial(); 
//       TGeoMaterial *newMat = gGeoManager->GetMaterial(mat1->GetName());
//       if (newMat==0) {
// 	std::cout<< "Material " << mat1->GetName() << " is not defined " << std::endl;
// 	FairGeoMedium *CbmMedium=Media->getMedium(mat1->GetName());
// 	if (!CbmMedium) {
// 	  std::cout << "Material is not defined in ASCII file nor in Root file" << std::endl;
// 	  CbmMedium=new FairGeoMedium(mat1->GetName());
// 	  Media->addMedium(CbmMedium);
// 	}
// 	std::cout << "Create Medium " << mat1->GetName() << std::endl;
// 	Int_t nmed=geobuild->createMedium(CbmMedium);
// 	v->SetMedium(gGeoManager->GetMedium(nmed));
// 	v->GetMedium()->Print();
// 	gGeoManager->SetAllIndex();
//       } else {
// 	//	if(fVerboseLevel>2)  
// 	  std::cout<< "DEBUG material was defined  MaterialName= " << mat1->GetName() << std::endl;
// 	TGeoMedium *med2= gGeoManager->GetMedium(mat1->GetName());
// 	v->SetMedium(med2);
//       }
//     }

//     TString name = v->GetName();
    
//     if (!gGeoManager->FindVolumeFast(v->GetName())) {
//       v->RegisterYourself();      
//       if(fVerboseLevel>2) std::cout<< "Volume ->  " <<v->GetName()<< " --> Registered to gGeoManager" << endl;
//       if (name.Contains("Crystal")){
// 	AddSensitiveVolume(v); 
// 	if(fVerboseLevel>2) std::cout<< "Volume " <<v->GetName()<< " is added to sensitives "<<endl;
//       }  
//     }
//   }
// }
void PndEnDrc::ExpandNode(TGeoNode *N0){
 
  FairGeoLoader*geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();
  cout<<"PndEnDrc::ExpandNode "<<N0->GetName()<<" placement "<<N0->GetMatrix()->GetName()<<endl;
  TGeoMatrix *Matrix =N0->GetMatrix();
  Matrix->SetDefaultName();
  cout<<Matrix->GetName()<<endl;
  if(gGeoManager->GetListOfMatrices()->FindObject(Matrix))gGeoManager->GetListOfMatrices()->Remove(Matrix);
  TGeoVolume *v1=N0->GetVolume();
  //v1->RegisterYourself();
  TObjArray *NodeList=v1->GetNodes();
  //Int_t medId=MediaArray1->GetSize();
  //for (Int_t Nod=0; Nod<NodeList->GetEntriesFast();Nod++) {   
  //TGeoNode *Node =(TGeoNode *)NodeList->At(Nod);
  // TGeoMatrix *M =N0->GetMatrix();
  //M->SetDefaultName();
  if(N0->GetNdaughters()>0){
    for (Int_t Nod=0; Nod<NodeList->GetEntriesFast();Nod++)
      ExpandNode((TGeoNode *)NodeList->At(Nod));
  }
  TGeoVolume *v= N0->GetVolume();
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
  if (TString(v->GetName()).Contains("lg")){
    // if (fVerboseLevel>2){
      std::cout << "Sensitive Volume : " << v->GetName() << " id "  << std::endl;
      // }
    AddSensitiveVolume(v);
  }
  if (TString(v->GetName()).Contains("disc")){
    // if (fVerboseLevel>2){
      std::cout << "Sensitive Volume : " << v->GetName() << " id "  << std::endl;
      // }
    AddSensitiveVolume(v);
  }
  if (TString(v->GetName()).Contains("lg"))v->SetLineColor(2);
  if (TString(v->GetName()).Contains("lif"))v->SetLineColor(3);
}



// -----------------------------------------------------------------------------
void PndEnDrc::EndOfEvent()
{
  // Clear the hit collection at the end of event

  Reset();
}

// -----------------------------------------------------------------------------
TClonesArray *PndEnDrc::GetCollection(Int_t iColl) const
{
  // Accessor ot the hit collection

  if (iColl == 0)
    return fEnDrcPointCollection;
  if (iColl == 1)
    return fEnDrcTrueCollection;
  return NULL;
}

// -----------------------------------------------------------------------------
Bool_t PndEnDrc::ProcessHits(FairVolume *vol)
{
  //store information on real particle hits
  if(TString(vol->GetName()).Contains("disc")) return ProcessTrueHits(vol);
  // Defines the action to be taken when a step is inside the active volume.
  // Creates PndEnDrcPoints and adds them to the collection
  TLorentzVector tmp;

  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
  fDetID   = vol->getMCid();
  fPdgCode = gMC->TrackPid();
  gMC->TrackPosition(tmp);
  fPos     = tmp.Vect();
  gMC->TrackMomentum(tmp);
  fMom     = tmp.Vect();
  fTof     = gMC->TrackTime() * 1.0e09;
  fLength  = gMC->TrackLength();
  fELoss   = gMC->Edep();
  // const Double_t* MasterPoint=gGeoManager->GetCurrentNavigator()->GetCurrentPoint();
  //Get Y coordinate on LiF, lightguide interface for parameterisation
  Double_t MasterPoint[]={fPos.X(),fPos.Y(),fPos.Z()};
  Double_t LocalPoint[3];
  gGeoManager->GetCurrentNavigator()->MasterToLocal(MasterPoint,LocalPoint);
  Double_t Dy=(static_cast<TGeoBBox*>(gGeoManager->GetVolume(vol->GetName())->GetShape())->GetDY());
  Double_t Dz=(static_cast<TGeoBBox*>(gGeoManager->GetVolume(vol->GetName())->GetShape())->GetDZ());
  Double_t InterHit[]={-LocalPoint[0],LocalPoint[2]+Dz,LocalPoint[1]-Dy};

  //Get theta for parameterisation
  //Get "next point" if the phtotn wasn't stopped
  //fMom.Unit().Print();
  //fPos.Print();
  // cout<<"Energy "<<tmp.E()*1E6<<endl;
  TVector3 next=fPos+fMom.Unit();
  Double_t MomDir[]={next.X(),next.Y(),next.Z()};//in global
  //get next point in local coords
  gGeoManager->GetCurrentNavigator()->MasterToLocal(MomDir,LocalPoint);
  //transform into "interface" frame
  // cout<<"Next "<<-LocalPoint[0]<<" "<<LocalPoint[2]+Dz<<" "<<LocalPoint[1]-Dy<<endl;
  //also need to subtract off original position in interframe
  Double_t InterNext[]={-LocalPoint[0]-InterHit[0],
			LocalPoint[2]+Dz-InterHit[1],
			LocalPoint[1]-Dy-InterHit[2]};

  Double_t InterTheta=0;
  // if (InterNext[1]!=0) InterTheta=atan(-InterNext[2]/InterNext[1])*TMath::RadToDeg()-;
  if (InterNext[1]!=0) InterTheta=atan(-InterNext[2]/InterNext[1])*TMath::RadToDeg();
  //  cout<<"Inter "<<InterNext[0]<<" "<<InterNext[1]<<" "<<InterNext[2]<<" "<<endl;
  //InterTheta=InterTheta*TMath::DegToRad();
  //calculate zpos (pm position)
  //apply offsets
  InterTheta=InterTheta-40.;
  InterHit[1]=InterHit[1]-10;
  Double_t zpos =(fPAR[0]+InterTheta*(fPAR[1]+InterTheta*(fPAR[2]+fPAR[3]*InterTheta)))\
  +  InterHit[1]*(fPAR[4]+InterTheta*(fPAR[5]+InterTheta*(fPAR[6]+fPAR[7]*InterTheta)))\
    +InterHit[1]*InterHit[1]*(fPAR[8]+InterTheta*(fPAR[9]+InterTheta*(fPAR[10]+fPAR[11]*InterTheta)));
  //cout<<"ZZZZZ "<<zpos<<endl;
  zpos=-zpos+60;
  //cout<<"PndEnDrc::ProcessHits"<<fTrackID<<" "<<fPdgCode<< " "<<gGeoManager->GetVolume(vol->GetName())->GetName()<<" "<<InterHit[0]<<" "<<InterHit[1]<<" "<<InterHit[2]<<endl<<" interY "<<InterHit[1]<<" interTh "<<InterTheta<<endl;
  // fPos.Print();
  AddPoint(fTrackID, fDetID, fPdgCode, fPos, fMom, fTof, fLength, fELoss,tmp.E()*1E9,zpos,InterTheta);
  gMC->StopTrack();
  return kTRUE;
}

// -----------------------------------------------------------------------------
Bool_t PndEnDrc::ProcessTrueHits(FairVolume *vol)
{
  fPdgCode = gMC->TrackPid();
  if(fPdgCode==50000050)return kFALSE;
  TLorentzVector tmp;
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
  fDetID   = vol->getMCid();
  gMC->TrackPosition(tmp);
  fPos     = tmp.Vect();
  gMC->TrackMomentum(tmp);
  fMom     = tmp.Vect();
  fTof     = gMC->TrackTime() * 1.0e09;
  fLength  = gMC->TrackLength();
  fELoss   = gMC->Edep();

  TClonesArray& points = *fEnDrcTrueCollection;
  Int_t size = points.GetEntriesFast();
  
  for(Int_t i=0;i<size;i++)
    if(static_cast<PndEnDrcPoint*>(points[i])->GetTrackID()==fTrackID)return kFALSE;
  cout<<"Process True hits "<<fTrackID<< " "<<fPdgCode<<endl;
  new(points[size]) PndEnDrcPoint(fTrackID, fDetID, fPdgCode, fPos, fMom, fTof, fLength, fELoss,0.,0.,0.);
  return kTRUE;
}
void PndEnDrc::Register()
{
  // Registers the hit collection to the ROOT manager

  // All entries in fEnDrcPointCollection will be added to ROOTFILE->cbmsim->EnDrcPoint
  FairRootManager::Instance()->Register("EnDrcPoint", "EnDrc", fEnDrcPointCollection, kTRUE);
  FairRootManager::Instance()->Register("EnDrcTrue", "EnDrc", fEnDrcTrueCollection, kTRUE);
}

// -----------------------------------------------------------------------------
void PndEnDrc::Reset()
{
  // Steps needed to clear the hit collection at the end of event

  fEnDrcPointCollection->Clear();
  fEnDrcTrueCollection->Clear();
}
