#include "PndDrpcAnalysisMaterial.h"

#include "FairRootManager.h"
#include "FairRadLenPoint.h"


#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::list;

/** Loop procedure **/
void  PndDrpcAnalysisMaterial::Exec(Option_t* option)
{
  fEvent++;
  if (fVerbose>0)
    Info("Exec", "Event %d.", fEvent);
  InitTree();
  FairRadLenPoint* p;
  Int_t i;
  Int_t n;
  Double_t r=54.0;
  Double_t rn;
  TVector3 newp;
  TVector3 oldp(0.0, 0.0, 0.0);
  TVector3 d;
//  if (TMath::Abs(fX)>594||TMath::Abs(fY)>474) return;
//  if (TMath::Abs(fX)<42&&TMath::Abs(fY)<42) return;
  n=fRadLen->GetEntriesFast();
  fX0=0; fIntL=0;
  for(i=0;i<n;i++)
  {
    p=(FairRadLenPoint*)fRadLen->At(i);
    newp=p->GetPositionOut();
//    cout << d.Mag() << "	" << p->GetRadLength() << "	" << newp.Z() << "	" << fX0 << endl;
    rn=newp.Pt();
    if (rn>r) break;
    d=newp-oldp;
//    cout << p->GetLength() << "	" << p->GetRadLength() << "	" << fEvent << "	" <<p->GetXOut() << "	" << p->GetYOut() << "	" << p->GetZOut() << endl;
    fX0+=d.Mag()/p->GetRadLength();
    oldp=newp;
  }
  fX=newp.X()/rn*r;
  fY=newp.Y()/rn*r;
  fZ=newp.Z()/rn*r;
  fTree->Fill();
}


/** Initializes tree **/
void  PndDrpcAnalysisMaterial::InitTree()
{
  if (fTree) return;
  fTree=new TTree("rad","Radiation lenght tree");
  fTree->Branch("ev", &fEvent, "ev/I");
  fTree->Branch("x", &fX, "x/D");
  fTree->Branch("y", &fY, "Y/D");
  fTree->Branch("z", &fZ, "z/D");
  fTree->Branch("x0", &fX0, "x0/D");
  fTree->Branch("intl", &fIntL, "intl/D");
}

 PndDrpcAnalysisMaterial:: PndDrpcAnalysisMaterial(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose)
{
  fTree=NULL;
}

/** Initing routine **/
InitStatus  PndDrpcAnalysisMaterial::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fRadLen=(TClonesArray*)fManager->GetObject("RadLen");
  if (!fRadLen)
  {
    Fatal("Init", "Can't find an array of radiation length points.");
    return kFATAL;
  }
  fTree=NULL;

  return kSUCCESS;
}

/** Finishing routine **/
void  PndDrpcAnalysisMaterial::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp( PndDrpcAnalysisMaterial)
