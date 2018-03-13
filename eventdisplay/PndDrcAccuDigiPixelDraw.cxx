// -----------------------------------------
// PndDrcAccuDigiPixelDraw.cpp
//
// Created on: 03.06.2013
// Author: R.Dzhygadlo
// ------------------------------------------

#include "PndDrcAccuDigiPixelDraw.h"

#include "FairRootManager.h"
#include "PndDrcPDHit.h"

#include "TGeoMatrix.h"
#include "TClonesArray.h"
#include "TEveTrans.h"
#include "TEveManager.h"
#include "TEveBoxSet.h"
#include "TGLViewer.h"
#include "TGLSAViewer.h"
#include "TRandom.h"
#include "TStyle.h"

using std::cout;
using std::endl;

PndDrcAccuDigiPixelDraw::PndDrcAccuDigiPixelDraw() : fPixelSize(0.01), fBoxSize(1)
{
  fGeo = new PndGeoDrc();
}

PndDrcAccuDigiPixelDraw::PndDrcAccuDigiPixelDraw(const char* name,TString digifile, Double_t height, Int_t ndigievents, Int_t iVerbose) : FairBoxSetDraw(name, iVerbose), fPixelSize(0.01), fBoxSize(1)
{
  fDigiFile = digifile;
  fBoxHeight = height;
  fNdigiEvents = ndigievents;
  fFirstEvent=true;
  fGeo = new PndGeoDrc();
}

PndDrcAccuDigiPixelDraw::~PndDrcAccuDigiPixelDraw()
{
  fHitsArr.clear();
  fHitsN.clear();
  if (fGeo) delete fGeo;
  delete(fGeoH);
}

InitStatus PndDrcAccuDigiPixelDraw::Init()
{
  FairBoxSetDraw::Init();
  fGeoH = PndGeoHandling::Instance();

  TIter next((TObjArray*)gGeoManager->GetListOfVolumes());
  TGeoVolume *vol;
  while((vol=(TGeoVolume*)next())){
    TString volumename = vol->GetName();
    if(!volumename.Contains("Drc")) vol->SetVisibility(kFALSE);
    if(volumename.Contains("DrcBarSupport")) vol->SetVisibility(kFALSE);
    if(volumename.Contains("DrcAirBox")) vol->SetTransparency(80);
    if(volumename.Contains("DrcEV")) vol->SetTransparency(80);
    if(volumename.Contains("DrcBarSensor")) vol->SetTransparency(80);
    if(volumename.Contains("DrcLENS")) vol->SetTransparency(95);
    if(volumename.Contains("DrcMirr")) vol->SetTransparency(50);
    if(volumename.Contains("DrcEVgrease")) vol->SetTransparency(60);

    //vol->SetTransparency(80);
    // vol->SetLineColor(17);
  }

  gGeoManager->SetNsegments(400);

  TGLViewer *v = gEve->GetDefaultGLViewer();
  //v->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
  TGLSAViewer *sav = (TGLSAViewer *)v;
  sav->SetDrawCameraCenter(true);
  TGLCamera & cam=(TGLOrthoCamera &)v->CurrentCamera();
  cam.SetExternalCenter(true);
  cam.SetCenterVec(46.8, 8.9, -120.);

  //Double_t c[] = {46.8, 8.9, -120.};
  //cam.Configure(1.3,1,c,-0.4,2.);
  //v->DoDraw();

  fFirstEvent=true;
  return kSUCCESS;
}

void PndDrcAccuDigiPixelDraw::ReadAllHits(){
  TFile* f = new TFile(fDigiFile);
  TTree *t=(TTree *) f->Get("pndsim") ;
  TClonesArray* hit_array=new TClonesArray("PndDrcPDHit");
  PndDrcPDHit *hit;
  TVector3 recoVector,recoLocal;
  TEveBoxSet* bs;
  TGeoHMatrix testMatrix;
  t->SetBranchAddress("DrcPDHit",&hit_array);
  for (Int_t j=0; (j<fNdigiEvents || fNdigiEvents==0) && j<t->GetEntriesFast(); j++){
    t->GetEntry(j);
    if(j%1==0) cout<<"Event No "<<j<<"  #entries "<<hit_array->GetEntriesFast()<<endl;
    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
      {
	hit=(PndDrcPDHit*)hit_array->At(i);
	hit->Position(recoVector);
	Int_t detId = hit->GetDetectorID();
	Int_t sensorId = detId/100;

	recoLocal = fGeoH->MasterToLocalShortId(recoVector, sensorId);

	TString detName = Form("pix %d", detId);
	bs = CreateNewBoxSet(detName);
	Float_t pixSize=fGeo->PixelSize();
	bs->SetDefWidth(pixSize);
	bs->SetDefHeight(pixSize);
	bs->AddBox(recoLocal.X()-pixSize/2., recoLocal.Y()-pixSize/2., -0.01);
	testMatrix = *(fGeoH->GetMatrixShortId(sensorId));
	TEveTrans& et = bs->RefMainTrans();
	et.SetFrom(testMatrix);
	fHitsArr[detId] = bs;
	fHitsN[detId] ++;
      }
  }
}

void PndDrcAccuDigiPixelDraw::Exec(Option_t*)
{
  if(fFirstEvent) ReadAllHits();

  gStyle->SetPalette(1);
  Int_t colnums = 256;
  TEveRGBAPalette* pal = new TEveRGBAPalette(0, colnums);
  TEveElement* man = (TEveElement*)fEventManager;
  if(fFirstEvent){
    Double_t max = 0;
    for (IntIter it = fHitsN.begin(); it != fHitsN.end(); it++){
      if(it->second > max) max = it->second;
    }
    max -= 0.2*max;
    Double_t hstep =  fBoxHeight/max;
    TEveBoxSet* topbs = new TEveBoxSet("DrcAccuDigiPixel");
    for (boxSetMapIter it = fHitsArr.begin(); it != fHitsArr.end(); it++){
      //if(hstep*fHitsN[it->first]<0.3) continue;
      it->second->SetPalette(pal);
      Int_t color = fHitsN[it->first]*colnums/max;
      if(color<1) color =1;
      it->second->DigitValue(color);
      Double_t s = 0.01+hstep*fHitsN[it->first];
      it->second->SetDefDepth(s); //-0.005
      TEveTrans& et = it->second->RefMainTrans();
      TVector3 pos = et.GetPos();
      et.SetPos(pos.X(),pos.Y(),pos.Z()-s);
      topbs->AddElement(it->second);
    }
    gEve->AddElement(topbs, man);
  }
  gEve->Redraw3D(kFALSE);

  fFirstEvent = false;
}

TEveBoxSet* PndDrcAccuDigiPixelDraw::CreateNewBoxSet(TString& name)
{
  TEveBoxSet* bs = new TEveBoxSet(name);
  bs->Reset(TEveBoxSet::kBT_AABoxFixedDim, kFALSE, 64);
  return bs;
}

ClassImp(PndDrcAccuDigiPixelDraw);

