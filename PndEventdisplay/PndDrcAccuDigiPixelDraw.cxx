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
#include "TRandom.h"
#include "TStyle.h"

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
	fRecoHits     = (TClonesArray *)fManager->GetObject("DrcPDHit"); 

	TIter next((TObjArray*)gGeoManager->GetListOfVolumes());
	TGeoVolume *vol;
	while(vol=(TGeoVolume*)next()){
	  TString volumename = vol->GetName();
	  if(volumename(0,4)=="Pipe")  vol->SetVisibility(kFALSE);
	  if(volumename=="DrcAirBox") vol->SetTransparency(80);
	  if(volumename=="DrcEVSensor") vol->SetTransparency(80);
	}
        //TGLViewer* v = gEve->GetDefaultGLViewer();
        //v->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
	fFirstEvent=true;
}

void PndDrcAccuDigiPixelDraw::ReadAllHits(){
  TFile* f = new TFile(fDigiFile);
  TTree *t=(TTree *) f->Get("cbmsim") ;
  TClonesArray* hit_array=new TClonesArray("PndDrcPDHit");
  PndDrcPDHit *hit; 
  TVector3 recoVector,recoLocal;
  TEveBoxSet* bs;
  TGeoHMatrix testMatrix;
  t->SetBranchAddress("DrcPDHit",&hit_array);
  for (Int_t j=0; (j<fNdigiEvents || fNdigiEvents==0) && j<t->GetEntriesFast(); j++)
    {
      t->GetEntry(j);
      if(j%100==0) cout<<"Event No "<<j<<"  #entries "<<hit_array->GetEntriesFast()<<endl;
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
	  bs->AddBox(recoLocal.X()-pixSize/2., recoLocal.Y()-pixSize/2., -0.1);
	  bs->SetDefWidth(pixSize);
	  bs->SetDefHeight(pixSize);	  
	  testMatrix = *(fGeoH->GetMatrixShortId(sensorId));
	  TEveTrans& et = bs->RefMainTrans();
	  et.SetFrom(testMatrix);
	  fHitsArr[detId] = bs;
	  fHitsN[detId] ++;
	}
    }
}

void PndDrcAccuDigiPixelDraw::Exec(Option_t* option)
{
  if(fFirstEvent) ReadAllHits();
  
  gStyle->SetPalette(1);
  Int_t colnums = 30;
  TEveRGBAPalette* pal = new TEveRGBAPalette(0, colnums);
  TEveElement* man = (TEveElement*)fEventManager;
  if(false){ 
    for (int i = 0; i < fRecoHits->GetEntriesFast(); i++){
      PndDrcPDHit *hit=(PndDrcPDHit*)fRecoHits->At(i);

      TVector3 recoVector;
      hit->Position(recoVector);
      Int_t sensorId = hit->GetDetectorID()/100;

      TVector3 recoLocal = fGeoH->MasterToLocalShortId(recoVector, sensorId);
      TString detName = Form("pix %d", hit->GetDetectorID());
      TEveBoxSet* bs = CreateNewBoxSet(detName);
      Float_t pixSize=fGeo->PixelSize();	
      bs->AddBox(recoLocal.X()-pixSize/2., recoLocal.Y()-pixSize/2., -0.005);
      bs->SetDefWidth(pixSize);
      bs->SetDefHeight(pixSize);	  
      TGeoHMatrix testMatrix = *(fGeoH->GetMatrixShortId(sensorId));
      TEveTrans& t = bs->RefMainTrans();
      t.SetFrom(testMatrix); 

      gEve->AddElement(bs, man);

    }
  }else if(fFirstEvent){
    Int_t max = 0;
    for (IntIter it = fHitsN.begin(); it != fHitsN.end(); it++){
      if(it->second > max) max = it->second;
    }
    max -= 0.1*max;
    Float_t hstep =  fBoxHeight/max;
    TEveBoxSet* topbs = new TEveBoxSet("DrcAccuDigiPixel");
    for (boxSetMapIter it = fHitsArr.begin(); it != fHitsArr.end(); it++){
      it->second->SetPalette(pal);
      it->second->DigitValue(fHitsN[it->first]*colnums/max);
      it->second->SetDefDepth(-0.005-hstep*fHitsN[it->first]);
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
	bs->Reset(TEveBoxSet::kBT_AABoxFixedDim, kFALSE, 32);
	return bs;
}

ClassImp(PndDrcAccuDigiPixelDraw);

