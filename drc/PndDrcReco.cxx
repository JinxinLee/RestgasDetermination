// -----------------------------------------
// PndDrcReco.cxx
//
// Created on: 04.03.2016
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcReco.h"

#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcLutNode.h"

#include "PndGeoHandling.h"
#include "TRandom.h"
#include "TSystem.h"

#include <TLine.h>

#include "TCanvas.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcReco::PndDrcReco() : FairTask("PndDrcReco"){
}

PndDrcReco::PndDrcReco(TString outFile, TString lutFile, TString pdfFile, Int_t verbose)
  :FairTask("PndDrcReco",verbose),fOutFile(outFile),fLutFile(lutFile),fPdfFile(pdfFile),fVerbose(verbose){
}

// -----   Initialization   ------------------------------------------------
InitStatus PndDrcReco::Init(){
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = -1;
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDrcReco::Init: " << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCArray ) {
    cout << "-W- PndDrcReco::Init: " << "No MCTrack array!" << endl;
    return kERROR;
  } 

  // Get bar points array
  fBarPointArray = (TClonesArray*) ioman->GetObject("DrcBarPoint");
  if ( ! fBarPointArray ) {
    cout << "-W- PndDrcReco::Init: " << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

  // Get ev points array
  fEVPointArray = (TClonesArray*) ioman->GetObject("DrcEVPoint");
  if ( ! fEVPointArray ) {
    cout << "-W- PndDrcReco::Init: " << "No DrcEVPoint array!" << endl;
    return kERROR;
  }

  // Get Photon point array
  fPDPointArray = (TClonesArray*) ioman->GetObject("DrcPDPoint");
  if ( ! fPDPointArray ) {
    cout << "-W- PndDrcReco::Init: " << "No DrcPDPoint array!" << endl;
    return kERROR;
  }
  
  // Get hits array
  fPDHitArray = (TClonesArray*) ioman->GetObject("DrcPDHit");
  if ( ! fPDHitArray ) {
    cout << "-W- PndDrcReco::Init: " << "No DrcPDHit array!" << endl;
    return kERROR;
  }

  TString name = fLutFile;
  name.Remove(0,name.Last('/')+1);
  sscanf(name, "lut_e%d_b%d_l%d", &fEvType,&fRadType,&fLensType);
  
  fFile = new TFile(fLutFile);
  fTree=(TTree *) fFile->Get("dircsim") ;
  for(Int_t l=0; l<5; l++){
    fLut[l] = new TClonesArray("PndDrcLutNode");
    fTree->SetBranchAddress(Form("LUT%d",l),&fLut[l]);
  }
  fTree->GetEntry(0);

  Double_t mom(0), theta(0),phi(0), trr(0),  nph(0),
    par1(0), par2(0), par3(0), par4(0), par5(0), par6(0), test1(0), test2(0), test3(0),separation(0);

  fFileOut = new TFile(fOutFile,"recreate");
  fTreeOut = new TTree("barreldirc","SPR");
  
  fTreeOut->Branch("fMom",&fMom,"fMom/D");
  fTreeOut->Branch("fTheta",&fTheta,"fTheta/D");
  fTreeOut->Branch("fPhi",&fPhi,"fPhi/D");
  fTreeOut->Branch("fPidTrue", &fPidTrue,"fPidTrue/I");
  fTreeOut->Branch("fPidDist", &fPidDist,"fPidDist/I");
  fTreeOut->Branch("fPidLike", &fPidLike,"fPidLike[2]/I");
  fTreeOut->Branch("fLikelihood",&fLikelihood,"fLikelihood[2]/D");
  fTreeOut->Branch("fSeparation",&fSeparation,"fSeparation[2]/D");

  fTreeOut->Branch("fSpr", &fSpr,"fSpr[5]/D");
  fTreeOut->Branch("fNph", &fNph,"fNph[5]/D");
  fTreeOut->Branch("fCangle",&fCangle,"fCangle[5]/D");

  
  fFile->cd();
  
  fGeo = new PndGeoDrc();
  fPipehAngle = fGeo->PipehAngle();
  Double_t barwidth = fGeo->BarWidth();
  if(fRadType==3) barwidth = 5.41333;
  fBarPhi = 2*atan(((barwidth + fGeo->barhGap())/2.)/fGeo->radius())*180/TMath::Pi();
  fDphi       = 2.*(180. - 2*fPipehAngle)/(Double_t)fGeo->BBoxNum();

  fHist = new TH1F("cherenkov_angle_hist",";#theta_{c} [rad];entries [#]", 100,0.6,0.9);
  fFit = new TF1("fgaus","[0]*exp(-0.5*((x-[1])/[2])*(x-[1])/[2]) + x*[3]+ [4]",0.35,0.85);
  fSpect = new TSpectrum(10);

  fNx = TVector3(1,0,0);   
  fNy = TVector3(0,1,0);
  fCriticalAngle = asin(1.00028/fGeo->nQuartz());

  Int_t pdg[]={11,13,211,321,2212};
  Double_t mass[] = {0.000511,0.1056584,0.139570,0.49368,0.9382723};    

  for(Int_t i=0; i<5; i++){
    fPdg[i]=pdg[i];
    fMass[i]=mass[i];
    fAngle[i] = 0;
    fFunc[i] = new TF1(Form("f_%d",i),"gaus(0)",0.4,0.9);
    
    fFunc[i]->SetParameter(0,1);
    fFunc[i]->SetParameter(2,0.009);
    if(i==2) fFunc[i]->SetParameter(2,0.0085);
    if(i==3) fFunc[i]->SetParameter(2,0.0085); //95

    fHlk1[i] = new TH1F(Form("fHlk1_%d",i),  ";ln L(K) - ln L(#pi);entries [#]",150,-100,100);
    fHlk2[i] = new TH1F(Form("fHlk2_%d",i),  ";ln L(K) - ln L(#pi);entries [#]",150,-100,100);
    fHtang[i] = new TH1F(Form("cherenkov_angle_hist_%d",i),";#theta_{c} [rad];entries [#]", 100,0.6,0.9);
  }

  
  fMethod=1;
  
  cout << "-I- PndDrcReco: Intialization successfull" << endl;
  return kSUCCESS;
}

TH1F *hDiff = new TH1F("hDiff",";t_{lut} - t_{daq} [ns];entries [#]",200,-10,10);
TH1F *hTime = new TH1F("hTime",";time [ns];entries [#]",200,0,100);
TH1F *hEnergy = new TH1F("hEnergy",";p [eV];entries [#]",200,0,10);
TH1F *hPathAll = new TH1F("hPathAll",";pathid [#];entries [#]",30,0,30);
TH1F *hPath = new TH1F("hPath",";pathid [#];entries [#]",30,0,30);

TH2F *hSD = new TH2F("hSD",";#theta_{c} [rad];t_{lut} - t_{daq} [ns]",200,0.8,0.85,200,-2,2);
// -----   Execution of Task   ---------------------------------------------
void PndDrcReco::Exec(Option_t* option){
  nevents++;

  Int_t nHits = fPDHitArray->GetEntriesFast();
  if(fVerbose>1) std::cout<<"Event # "<< nevents<<" has "<<nHits<<" hits."<< std::endl;
  else if(fVerbose==1 && nevents%100==0) std::cout<<"Event # "<< nevents<<" has "<<nHits<<" hits."<< std::endl;

  for(Int_t itrack=0; itrack<fMCArray->GetEntriesFast(); itrack++){
    fMCTrack = (PndMCTrack*)fMCArray->At(itrack);
    if( fMCTrack->GetMotherID() != -1) continue;
    fMcTrackId = itrack; 

    Int_t mcBarId, mcBoxId(-1), barId;
    for(int i=0; i<fBarPointArray->GetEntriesFast(); i++){
      fBarPoint = (PndDrcBarPoint*)fBarPointArray->At(i);
      if(itrack == fBarPoint->GetTrackID()){
	mcBoxId = fBarPoint->GetBoxId();
	break;
      }
    }
    fBarPoint->Momentum(fMomInBar);
    fBarPoint->Position(fPosInBar);
    fTimeInBar = fBarPoint->GetTime();
    fPidTrue = fBarPoint->GetPdgCode();
    mcBarId = fBarPoint->GetBarId();

    //fMomInBar = fMCTrack->GetMomentum();

    fMom=fMomInBar.Mag();
    fTheta=fMomInBar.Theta()*180/TMath::Pi();
    fPhi=fMomInBar.Phi()*180/TMath::Pi();
    
    // fMomInBar.Print();
    // fMomInBar = TVector3(0,0,3);
    // fMomInBar.SetTheta(60*TMath::Pi()/180.);
    // fMomInBar.SetPhi((-10.825-180)*TMath::Pi()/180.);
    // fMomInBar.Print();
    
    Double_t boxPhi;
    DetermineBarId(boxPhi, barId);
    fMomInBar.RotateZ(-boxPhi/180.*TMath::Pi());
    DetermineCherenkov(mcBoxId, barId);
  }
}

Int_t gg_pathid=0;
Int_t gg_nevents2(0),gg_nevents3(0),gg_nph(0),gg_nph2(0),gg_nph3(0),gg_nph2t(0),gg_nph3t(0);
void PndDrcReco::DetermineCherenkov(Int_t  boxId, Int_t barId){
  
  for(Int_t i=0; i<5; i++) {
    fLk1[i]=0;
    fLk2[i]=0;
    fAngle[i] = acos(sqrt(fMom*fMom + fMass[i]*fMass[i])/fMom/1.473) + 0.00; //1.4738 = 370 = 3.35
    fFunc[i]->SetParameter(1,fAngle[i]);
  }

  gg_nph=0;
  gg_nph2=0;
  gg_nph3=0;
  for(Int_t h=0; h<fPDHitArray->GetEntriesFast(); h++) {
    fPDHit = (PndDrcPDHit*)fPDHitArray->At(h);
 
    Int_t sensorId = fPDHit->GetSensorId();
    fTimeHit = fPDHit->GetTime()-fTimeInBar;
    
    Int_t pointID = fPDHit->GetLink(1).GetIndex();
    Int_t eventID = fPDHit->GetLink(1).GetEntry();
    
    fPDPoint = (PndDrcPDPoint*)fPDPointArray->At(pointID);
    fBarPoint = (PndDrcBarPoint*)fBarPointArray->At(fPDPoint->GetBarPointID());

    if(fPDPoint->GetTrackID()<1) continue;
    fMCTrack =(PndMCTrack*)fMCArray->At(fPDPoint->GetTrackID());
    if(fMcTrackId!=fMCTrack->GetMotherID()) continue;

    Double_t en = 1.2398/(fMCTrack->GetMomentum().Mag()*1E6);
    hEnergy->Fill(fMCTrack->GetMomentum().Mag()*1E9);
    
    if(fBarPoint->GetBoxId() != boxId || fBarPoint->GetBarId() != barId) continue;

    Int_t nev=0;
    TVector3 vec;
    gg_pathid=0;
    for(int i=0; i<fEVPointArray->GetEntriesFast(); i++){
      PndDrcEVPoint *fEVPoint = (PndDrcEVPoint*)fEVPointArray->At(i);
      if(fPDPoint->GetTrackID() == fEVPoint->GetTrackID()){
	nev++;
	vec = fEVPoint->GetNormal();
	gg_pathid += (vec.X()+vec.Y()*10 + vec.Z()*100)*1000*nev;
      }
    }
    
    if(fMethod==1 || fMethod==0) LookUpTable(barId,sensorId);
    if(fMethod==2 || fMethod==0) TimeImaging(sensorId);
  }
  
  if(fMethod==1 || fMethod==0){
    if(fPidTrue==211) fHlk1[2]->Fill(fLk1[2]-fLk1[3]);
    if(fPidTrue==321) fHlk1[3]->Fill(fLk1[2]-fLk1[3]);
  }
  if(fMethod==2 || fMethod==0){
    if(fPidTrue==211)  fHlk2[2]->Fill(fLk2[2]-fLk2[3]);
    if(fPidTrue==321)  fHlk2[3]->Fill(fLk2[2]-fLk2[3]);
  }

  fLikelihood[0] = fLk1[2]-fLk1[3];
  fLikelihood[1] = fLk2[2]-fLk2[3];

  if(gg_nph2>0) { gg_nevents2++; gg_nph2t+=gg_nph2;}
  if(gg_nph3>0) { gg_nevents3++; gg_nph3t+=gg_nph3;}
  
  std::cout<<"gg_nhp "<< gg_nph<< "  "<< fLk1[2] <<" "<< fLk1[3]<<"  "<< fLk1[2]-fLk1[3]<< std::endl;
  // std::cout<<"fLk1[2] fLk1[3] "<<fLk1[2]<< "   " <<fLk1[3] <<std::endl;
  // if( fLk1[2] > fLk1[3]) std::cout<<" nph  "<<fPDHitArray->GetEntriesFast() << " pdg " <<fPidTrue<<std::endl;
  
  if(false){// &&   fLk1[2] > fLk1[3]){
    TCanvas* c = new TCanvas("c","c",0,0,800,600);
    fHist->Scale(1/fHist->GetMaximum());
    fHist->SetTitle(Form("%d",fPidTrue));
    fHist->Draw();


    fFunc[2]->SetLineColor(kBlue);
    fFunc[2]->Draw("same");
    fFunc[3]->SetLineColor(kRed);
    fFunc[3]->Draw("same");
  

    TLine *line = new TLine(0,0,0,1000);
    line->SetX1(fAngle[2]);
    line->SetX2(fAngle[2]);
    line->SetY1(gPad->GetUymin());
    line->SetY2(fHist->GetMaximum()*1.05);
    line->SetLineColor(kBlue);
    line->Draw();

  
    TLine *line1 = new TLine(0,0,0,1000);
    line1->SetX1(fAngle[3]);
    line1->SetX2(fAngle[3]);
    line1->SetY1(gPad->GetUymin());
    line1->SetY2(fHist->GetMaximum()*1.05);
    line1->SetLineColor(kRed);
    line1->Draw();
      
    c->Modified();
    c->Update();
    c->WaitPrimitive();
  }
   
  fHist->Reset();
}

void PndDrcReco::LookUpTable(Int_t barId, Int_t sensorId){
  TVector3 dird, dir;
  Double_t evtime, luttime, luttheta, tangle, noise(0.5);
  
  if(fMCTrack->GetMomentum().Z()>0) fReflected = kTRUE;
  else  fReflected = kFALSE;

  fLenz = fPosInBar.Z()+119;
  if(fReflected) fLenz = 2*240 - fLenz;
  
  PndDrcLutNode *node = (PndDrcLutNode*) fLut[barId]->At(sensorId);
  Int_t size = node->Entries();

  Bool_t isGood(false);
  for(int i=0; i<size; i++){
    dird = node->GetEntry(i);
    evtime = node->GetTime(i);
    //if(node->GetPathId(i)>100 )continue;

    hPathAll->Fill(node->GetPathId(i));
    if((Int_t)gg_pathid != (Int_t)node->GetPath(i)){
      continue;
    }else{
      hPath->Fill(node->GetPathId(i));
    }
    //if(node->GetPathId(i)>10) continue;

    for(int u=0; u<4; u++){
      if(u == 0) dir = dird;
      if(u == 1) dir.SetXYZ(-dird.X(), dird.Y(), dird.Z());
      if(u == 2) dir.SetXYZ( dird.X(),-dird.Y(), dird.Z());
      if(u == 3) dir.SetXYZ(-dird.X(),-dird.Y(), dird.Z());
      if(fReflected) dir.SetXYZ( dir.X(), dir.Y(),-dir.Z());
      
      if(dir.Angle(fNx) < fCriticalAngle || dir.Angle(fNy) < fCriticalAngle) continue;

      luttheta = dir.Theta();	
      if(luttheta > TMath::Pi()/2.) luttheta = TMath::Pi()-luttheta;
      luttime = fLenz/cos(luttheta)/19.8 + evtime;

      tangle = fMomInBar.Angle(dir);
      //      if(tangle<fAngle[3]-0.04 || tangle>fAngle[2]+0.05) continue;
      if(tangle < 0.6 ||  tangle > 0.9) continue; 
      hDiff->Fill(luttime -fTimeHit);
      hTime->Fill(fTimeHit);
      if(fabs(luttime -fTimeHit)>1.5) continue;


      if(tangle > 0.75 && tangle < 0.85) isGood=true;
      
      hSD->Fill(tangle,luttime -fTimeHit);

      
      fLk1[2] += TMath::Log(fFunc[2]->Eval(tangle)+noise); // 211
      fLk1[3] += TMath::Log(fFunc[3]->Eval(tangle)+noise); // 321

      if(fPidTrue==211) fHtang[2]->Fill(tangle);
      if(fPidTrue==321) fHtang[3]->Fill(tangle);
     
      fHist->Fill(tangle);
    }
  }
  if(isGood){
    gg_nph++;
    if(fPidTrue==211) gg_nph2++;
    if(fPidTrue==321) gg_nph3++;
  }
}

void PndDrcReco::TimeImaging(Int_t sensorId){

}

void PndDrcReco::DetermineBarId(Double_t &boxPhi, Int_t &barId){
  Double_t startPhi = fPosInBar.Phi()/TMath::Pi()*180;
  if(startPhi < 0) startPhi = 360 + startPhi;
  if(startPhi >= 0 && startPhi < 90) boxPhi = TMath::Floor(startPhi/fDphi) *fDphi + fDphi/2.;
  if(startPhi >= 90 && startPhi < 270) boxPhi = 90  + fPipehAngle + TMath::Floor((startPhi-90-fPipehAngle)/fDphi) *fDphi + fDphi/2.;
  if(startPhi >= 270 && startPhi < 360) boxPhi = 270 + fPipehAngle + TMath::Floor((startPhi-270-fPipehAngle)/fDphi) *fDphi + fDphi/2.;
  
  if(fRadType==5) barId = (int) (2.5 + (boxPhi-startPhi)/fBarPhi);
  if(fRadType==3) barId = (int) (1.5 + (boxPhi-startPhi)/fBarPhi);
  if(barId>4 || barId<0){
    std::cout<<"Error in PndDrcReco:  Bar Id is wrong. barId = "<< barId <<std::endl;
    barId = -1;
  }
}

Double_t PndDrcReco::FindPeak(){
  Double_t cherenkovreco = -1;

  if(fHist->GetEntries()>20 ){
    TCanvas* c = new TCanvas("c","c",0,0,800,600);
    Int_t nfound = fSpect->Search(fHist,1,"",0.6);
    Float_t *xpeaks = (Float_t*)fSpect->GetPositionX();
    if(nfound>0) cherenkovreco = xpeaks[0];
    fFit->SetParameter(1,cherenkovreco);   // peak
    fFit->SetParameter(2,0.01); // width
    fHist->Fit("fgaus","Q","",cherenkovreco-0.02,cherenkovreco+0.02);
    cherenkovreco = fFit->GetParameter(1);
    std::cout<<"sigma  "<< fFit->GetParameter(2) <<std::endl;
    
    if(cherenkovreco<0 || cherenkovreco>1 ) cherenkovreco = 0;
  
    if(fVerbose>1){
      fHist->GetXaxis()->SetTitle("#theta_{C}, [rad]");
      fHist->GetYaxis()->SetTitle("Entries, [#]");
      fHist->Draw();
      c->Modified();
      c->Update();
      c->WaitPrimitive();
      // c->Print(Form("pic/animpid/animpid_%d.png",g_num++));
    }
  }
  fHist->Reset();

  return cherenkovreco;
}

Int_t PndDrcReco::FindPdg(Double_t mom, Double_t cangle){
  Double_t tdiff, diff=100;
  Int_t minid=0;
  for(Int_t i=0; i<5; i++){
    tdiff = fabs(cangle - acos(sqrt(mom*mom + fMass[i]*fMass[i])/mom/1.46907)); //1.46907 - fused silica
    if(tdiff<diff){
      diff = tdiff;
      minid = i;
    }
  }
  return fPdg[minid]; 
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcReco::Finish(){
  TF1 *ff;
  Double_t m1,m2,s1,s2;
  if(fHlk1[2]->GetEntries()>10){
    fHlk1[2]->Fit("gaus","S");
    ff = fHlk1[2]->GetFunction("gaus");
    m1=ff->GetParameter(1);
    s1=ff->GetParameter(2);
  }
  if(fHlk1[3]->GetEntries()>10){
    fHlk1[3]->Fit("gaus","S");
    ff = fHlk1[3]->GetFunction("gaus");
    m2=ff->GetParameter(1);
    s2=ff->GetParameter(2);
  }
  fSeparation[0] = (fabs(m2-m1))/(0.5*(s1+s2));
  std::cout<<"separation "<< fSeparation[0] <<std::endl;

  if(fSeparation[0]>0) fPidLike[0] = 321; 
  else fPidLike[0] = 211; 
  
  fHlk1[2]->SetLineColor(4);
  fHlk1[3]->SetLineColor(2);
  
  TCanvas* c1 = new TCanvas("c1","c1",0,0,800,600);
  fHlk1[2]->Draw();
  fHlk1[3]->Draw("same");

  TCanvas* c2 = new TCanvas("c2","c2",0,0,800,600);
  hDiff->Draw();
  
  // TCanvas* c4 = new TCanvas("c4","c4",0,0,800,600);
  // hTime->Draw();

  // TCanvas* c5 = new TCanvas("c5","c5",0,0,800,600);
  // hSD->Draw("colz");

  // TCanvas* c6 = new TCanvas("c6","c6",0,0,800,600);
  // hEnergy->Draw();

  TCanvas* c7 = new TCanvas("c7","c7",0,0,800,600);
  hPathAll->Draw();
  hPath->SetLineColor(2);
  hPath->Draw("same");
  
  TCanvas* c3 = new TCanvas("c3","c3",0,0,800,600);

  fFit->SetParameter(2,0.01); // width
  for(Int_t i=2; i<4; i++){
    fFit->SetParameter(1,fAngle[i]);   // peak
    fHtang[i]->Fit("fgaus","Q","",0.78,0.87);
    fSpr[i]=fFit->GetParameter(2);
    fCangle[i]=fFit->GetParameter(1); 
  }
 
  fHist1->SetLineColor(4);
  fHist1->Draw();

  fHist2->SetLineColor(2);
  fHist2->Draw("same");
  
  TLine *line = new TLine(0,0,0,1000);
  line->SetX1(fAngle[2]);
  line->SetX2(fAngle[2]);
  line->SetY1(gPad->GetUymin());
  line->SetY2(fHist1->GetMaximum()*1.05);
  line->SetLineColor(kBlue);
  line->Draw();
  
  TLine *line1 = new TLine(0,0,0,1000);
  line1->SetX1(fAngle[3]);
  line1->SetX2(fAngle[3]);
  line1->SetY1(gPad->GetUymin());
  line1->SetY2(fHist1->GetMaximum()*1.05);
  line1->SetLineColor(kRed);
  line1->Draw();

  
  for(Int_t l=0; l<5; l++) fLut[l]->Clear();

  fNph[2]=gg_nph2t/(Double_t)gg_nevents2;
  fNph[3]=gg_nph3t/(Double_t)gg_nevents3;
  
  std::cout<<"N pi "<<fNph[2] << "    N K " <<fNph[3] <<std::endl;
 
  fFileOut->cd();
  fHist1->Write();
  fHist2->Write();
  fTreeOut->Fill();
  fTreeOut->Write();
  fFileOut->Write();
  
  cout << "-I- PndDrcReco: Finish" << endl; 
}

ClassImp(PndDrcReco)
