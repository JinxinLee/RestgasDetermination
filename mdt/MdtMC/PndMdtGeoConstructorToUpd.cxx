
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
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"

#include "FairVolume.h"
// add on for debug
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairModule.h"

#include "PndDetectorList.h"
#include "PndStack.h"
#include "PndMdt.h"
#include "PndMdtGeoConstructorToUpd.h"


using namespace std;


// -----   Public method ConstructGeometryToUpd   -----------------------------
void PndMdt::ConstructGeometryToUpd() 
{

/*
    TString vname = "cave";
    vname = vname.Strip();
    TGeoVolume* vcave = gGeoManager->FindVolumeFast(vname.Data());

    TGeoVolume* mdt =  new TGeoVolumeAssembly("Mdt");
    mdt->Import(GetGeometryFileName(),"mdt","");
    AddSensitiveVolume(mdt);
    
    vcave->AddNode(mdt,1);
*/

    CreateGeometryToUpd("muoparsupd.root");
}
// ----------------------------------------------------------------------------


// -----   Private method ProcessHitsToUpd  -----------------------------------
Bool_t PndMdt::ProcessHitsToUpd(FairVolume* vol) 
{
  TString name = vol->GetName();
  if (!(name.BeginsWith("muon"))) 
    cout << "Error <PndMdt::ProcessHits> : " << name << " not MDT volume" << endl;
  else {
    if (gMC->IsTrackEntering() || gMC->IsNewTrack() ){
      fPos_In.SetXYZM(0.,0.,0.,0.);
      fMom_In.SetXYZM(0.,0.,0.,0.);
      gMC->TrackPosition(fPos_In);
      gMC->TrackMomentum(fMom_In);
      fTrkIn = gMC->GetStack()->GetCurrentTrackNumber();
    }; // end entering

    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() ){
      Int_t TrNo=gMC->GetStack()->GetCurrentTrackNumber();
      Int_t pdg= gMC->TrackPid();	  
      if ( TrNo == fTrkIn ){
	TLorentzVector lPos, lMom;
	int ilayer;
	int iplate;
	sscanf(name,"muon%i",&iplate);
	ilayer = iplate%8;
	if(ilayer>12) ilayer += 37;
	else ilayer *= 2;
	gMC->TrackPosition(lPos); // cm
	gMC->TrackMomentum(lMom); // GeV
	TClonesArray& clref = *fMdtCollection;
	Int_t size = fMdtCollection->GetEntriesFast();
	PndMdtPoint *P= new(clref[size]) PndMdtPoint (TrNo,ilayer, lPos.Vect(), lMom.Vect(), gMC->TrackTime(),
			  gMC->TrackLength(), gMC->Edep(), gMC->GetStack()->GetCurrentParentTrackNumber(),pdg,
			  fPos_In.Vect(), fMom_In.Vect());
        /**if you add a point then tell the stack! here*/
	PndStack* stack = (PndStack*) gMC->GetStack();
    	stack->AddPoint(kMDT);
     if ( ( (GetModule()==1) && (TMath::Even(ilayer)) )  ||  (GetModule()==2)  )
	{ // Set the correct MCTrack->GetMdtPoints()

	}
 
      };
  
      ResetParameters();
    };

  }
  return kTRUE;
  
}
// ----------------------------------------------------------------------------


// -----   Public method ProcessHitsToUpd  ------------------------------------
void PndMdt::CreateGeometryToUpd(TString outName)
{
    TString vname = "cave";
    vname = vname.Strip();
    TGeoVolume* vcave = gGeoManager->FindVolumeFast(vname.Data());

//General definitions
    TGeoVolume* mdt        = new TGeoVolumeAssembly("Mdt");
    TGeoVolume* mdtBarrel  = new TGeoVolumeAssembly("MdtBarrel");
    TGeoVolume* mdtEndcap  = new TGeoVolumeAssembly("MdtEndcap");
    
//Additional definitions
    Text_t buffer[30];
    Text_t longbuffer[250];
    Double_t mpx, mpy, mpz;
    Double_t mx0, my0, mz0;
    Double_t my;
    int k;
    Double_t dx1, dx2, dy1, dy2, dz;
    TGeoRotation tRot, tRotSlice;
    Double_t angle;

//ArCO2
    Int_t kMatArCO2 = 40;
    Float_t aP[3] = {39.948, 12.01, 16.00};
    Float_t zP[3] = {18., 6., 8.} ;
    Float_t wP[3] = {1., 1., 2.} ;
    Float_t dP = 1.636 ;
    Int_t   nP = 3;
    Float_t sumWeight = 0.0;
    for (Int_t i=0; i<nP; i++) sumWeight += aP[i]*wP[i];
    for (Int_t i=0; i<nP; i++) wP[i] *= aP[i]/sumWeight;
    TGeoMaterial* matArCO2 = gGeoManager->Mixture("ArCO2",aP,zP,dP,nP,wP,kMatArCO2);
    
    Int_t kMedArCO2 = 40;
    TGeoMedium* medArCO2 = gGeoManager->Medium("ArCO2", kMedArCO2, kMatArCO2, 1, 1, 30., 10.0, 0.1, 0.1, 0.1, 0.1);

//creating the holes
    TGeoBBox* hbox1 = new TGeoBBox("hbox1",((Double_t)PndMdt_H01_Length)/10.0,((Double_t)PndMdt_H01_Length)/10.0,((Double_t)PndMdt_SV12)/10.0,0);
    TGeoBBox* hbox2 = new TGeoBBox("hbox2",((Double_t)PndMdt_H02_H)/10.0,10.0+((Double_t)PndMdt_SVThickness)/10.0,((Double_t)PndMdt_H02_V)/10.0,0);
    TGeoBBox* hbox3 = new TGeoBBox("hbox3",((Double_t)PndMdt_H03_H)/10.0,10.0+((Double_t)PndMdt_SVThickness)/10.0,((Double_t)PndMdt_H03_V)/10.0,0);
    TGeoBBox* hbox4 = new TGeoBBox("hbox4",((Double_t)PndMdt_H04_H)/10.0,10.0+((Double_t)PndMdt_SVThickness)/10.0,((Double_t)PndMdt_H04_V)/10.0,0);
    TGeoBBox* hbox5 = new TGeoBBox("hbox5",((Double_t)PndMdt_H05_H)/10.0,10.0+((Double_t)PndMdt_SVThickness)/10.0,((Double_t)PndMdt_H05_V)/10.0,0);

//MdtBarrel
    tRot.RotateX(90.0);
    mpy = ((Double_t)PndMdt_Barrel_Length)/2.0;
    mpz = (Double_t)PndMdt_SVThickness;
//    mx0 = 0.0;
    mz0 = (Double_t)PndMdt_Barrel_Displacement;
    for(int i=1; i<13; i++)
    {
	    switch(i)
	    {
		case 1:
	    	    my0 = (Double_t)PndMdt_SV01;
		    break;
    		case 2:
		    my0 = (Double_t)PndMdt_SV02;
		    break;
		case 3:
		    my0 = (Double_t)PndMdt_SV03;
		    break;
		case 4:
		    my0 = (Double_t)PndMdt_SV04;
		    break;
		case 5:
		    my0 = (Double_t)PndMdt_SV05;
		    break;
		case 6:
		    my0 = (Double_t)PndMdt_SV06;
		    break;
		case 7:
		    my0 = (Double_t)PndMdt_SV07;
		    break;
		case 8:
		    my0 = (Double_t)PndMdt_SV08;
		    break;
		case 9:
		    my0 = (Double_t)PndMdt_SV09;
		    break;
		case 10:
		    my0 = (Double_t)PndMdt_SV10;
		    break;
		case 11:
		    my0 = (Double_t)PndMdt_SV11;
		    break;
		case 12:
		    my0 = (Double_t)PndMdt_SV12;
		    break;
	    };
	    my = my0;
	    mpx = (my - mpz)*(TMath::Tan(TMath::ACos(-1.0)/8.0));
	    sprintf(buffer,"box%i",i);
	    TGeoBBox* box = new TGeoBBox(buffer,mpx/10.0,mpy/10.0,mpz/10.0,0);
	    TGeoTranslation* tgt = new TGeoTranslation(0.0,mz0/10.0,0.0);
	    tgt->SetName("tgt");
	    tgt->RegisterYourself();
	    sprintf(buffer,"box%i:tgt-hbox1",i);
	    TGeoCompositeShape* tgcs = new TGeoCompositeShape("tgcs",buffer);
	for(int j=0; j<8; j++)
	{
	    if(j==0 || j==4) 
	    {
		k=j+2;
		mx0 = my*TMath::Cos(((Double_t)k)*TMath::ACos(-1.0)/4.0);
		my0 = my*TMath::Sin(((Double_t)k)*TMath::ACos(-1.0)/4.0);
		sprintf(buffer,"muon%i",(int)(8*(i-1)+j));
		TGeoVolume* volume = new TGeoVolume(buffer,tgcs,gGeoManager->GetMedium("ArCO2"));
AddSensitiveVolume(volume);
		volume->SetLineColor(1);
		mdtBarrel->AddNode(volume,8*(i-1)+j,new TGeoCombiTrans(mx0/10.0,my0/10.0,0.0,new TGeoRotation(tRot)));
	    }
	    else
	    {
		if(j==2 || j==6) k=j-2;
		else k=j;
		mx0 = my*TMath::Cos(((Double_t)k)*TMath::ACos(-1.0)/4.0);
		my0 = my*TMath::Sin(((Double_t)k)*TMath::ACos(-1.0)/4.0);
		sprintf(buffer,"muon%i",(int)(8*(i-1)+j));
		TGeoVolume* volume = new TGeoVolume(buffer,box,gGeoManager->GetMedium("ArCO2"));
AddSensitiveVolume(volume);
		volume->SetLineColor(1);
		mdtBarrel->AddNode(volume,8*(i-1)+j,new TGeoCombiTrans(mx0/10.0,my0/10.0,mz0/10.0,new TGeoRotation(tRot)));
	    };
	    tRot.RotateZ(-45.0);
	};
    };
    mdt->AddNode(mdtBarrel,1);


//MdtEndcap
    
    TGeoRotation tgrl;
    tgrl.RotateY(0.0);
    TGeoRotation* tgr0 = new TGeoRotation(tgrl);
    tgr0->SetName("tgr0");
    tgr0->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation* tgr1 = new TGeoRotation(tgrl);
    tgr1->SetName("tgr1");
    tgr1->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation* tgr2 = new TGeoRotation(tgrl);
    tgr2->SetName("tgr2");
    tgr2->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation* tgr3 = new TGeoRotation(tgrl);
    tgr3->SetName("tgr3");
    tgr3->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation* tgr4 = new TGeoRotation(tgrl);
    tgr4->SetName("tgr4");
    tgr4->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation* tgr5 = new TGeoRotation(tgrl);
    tgr5->SetName("tgr5");
    tgr5->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation* tgr6 = new TGeoRotation(tgrl);
    tgr6->SetName("tgr6");
    tgr6->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation* tgr7 = new TGeoRotation(tgrl);
    tgr7->SetName("tgr7");
    tgr7->RegisterYourself();

    my = ((Double_t)PndMdt_Endcap_Height)/2.0;
    dx1 = ((Double_t)PndMdt_Endcap_Height)*(TMath::Tan(TMath::ACos(-1.0)/8.0));
    dx2 = 0.0;
    dy1 = (Double_t)PndMdt_SVThickness;
    dy2 = (Double_t)PndMdt_SVThickness;
    dz  = ((Double_t)PndMdt_Endcap_Height)/2.0;
    TGeoTrd2* trd = new TGeoTrd2("trd",dx1/10.0,dx2/10.0,dy1/10.0,dy2/10.0,dz/10.0);
    TGeoTranslation* tgt1 = new TGeoTranslation(0.0,0.0,my/(-10.0));
    tgt1->SetName("tgt1");
    tgt1->RegisterYourself();

    for(int i=13; i<17; i++)
    {
	switch(i)
	{
	    case 13:
		mz0 = (Double_t)PndMdt_SV13;
		break;
	    case 14:
		mz0 = (Double_t)PndMdt_SV14;
		break;
	    case 15:
		mz0 = (Double_t)PndMdt_SV15;
		break;
	    case 16:
		mz0 = (Double_t)PndMdt_SV16;
		break;
	};
	for(int j=0; j<8; j++)
	{
	    sprintf(buffer,"tgcs%i",(int)(8*(i-1)+j));
	    sprintf(longbuffer,"trd:tgt1-hbox%i:tgr%i",i-11,j);
	    TGeoCompositeShape* tgcs = new TGeoCompositeShape(buffer,longbuffer);
	    sprintf(buffer,"muon%i",(int)(8*(i-1)+j));
	    TGeoVolume* volume = new TGeoVolume(buffer,tgcs,gGeoManager->GetMedium("ArCO2"));
AddSensitiveVolume(volume);
	    volume->SetLineColor(1);
	    mdtEndcap->AddNode(volume,8*(i-1)+j,new TGeoCombiTrans(0.0,0.0,mz0/10.0,new TGeoRotation(tRot)));
	    tRot.RotateZ(-45.0);
	};
    };
    
    mdt->AddNode(mdtEndcap,1);


//    mdt->Export(outName,"mdt","");
    
//    AddSensitiveVolume(mdt);
    vcave->AddNode(mdt,1);
//    mdt->Export(outName,"mdt","");

    return;
}
// ----------------------------------------------------------------------------



ClassImp(PndMdt)
