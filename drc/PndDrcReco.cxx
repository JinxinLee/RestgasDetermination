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
#include "TSystem.h"

#include <TLine.h>

#include "TStyle.h"
#include "TCanvas.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcReco::PndDrcReco() : FairTask("PndDrcReco"){
}

PndDrcReco::PndDrcReco(TString outFile, TString lutFile, TString pdfFile, Int_t verbose, Double_t r1, Double_t r2)
  :FairTask("PndDrcReco",verbose),fVerbose(verbose),fOutFile(outFile),fLutFile(lutFile),fPdfFile(pdfFile){
  fR1=r1;
  fR2=r2;
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


  //corrections lut
  const Int_t maxpoints(150);
  Double_t meanc2[maxpoints],spr2[maxpoints],meanc3[maxpoints],spr3[maxpoints];
  name = fLutFile;
  name.Remove(name.Last('/')+1);

  Long_t id,size,flags,modtime;
  if(!gSystem->GetPathInfo((name+"corrlut.root").Data(),&id,&size,&flags,&modtime)){    
    TFile *fc = new TFile(name+"corrlut.root");
    TTree *tc = (TTree*)fc->Get("corrlut");
    tc->SetBranchAddress("meanc2",&meanc2);
    tc->SetBranchAddress("meanc3",&meanc3);
    tc->SetBranchAddress("spr2",&spr2);
    tc->SetBranchAddress("spr3",&spr3);
  
    for (Int_t i = 0; i < tc->GetEntriesFast(); i++) {
      tc->GetEvent(i);
      for(Int_t j=0; j<145; j++){
        c_mean[2][i+1][j]=meanc2[j];
	c_mean[3][i+1][j]=meanc3[j];
	c_spr[2][i+1][j]=spr2[j];
	c_spr[3][i+1][j]=spr3[j];
      }
    }
    fc->Close();
  }


  for(Int_t p=0; p<5; p++){
    for(Int_t m=0; m<40; m++){
      for(Int_t t=0; t<120; t++){
	for(Int_t i=0; i<1100; i++){
	  fhPdf[p][m][t][i] = NULL;
	}
      }
    }
  }
  
  if(!gSystem->GetPathInfo(fPdfFile.Data(),&id,&size,&flags,&modtime)){    
    TFile fpdf(fPdfFile);
    TIter nextkey(fpdf.GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)nextkey())) {
      TH1F *fun = (TH1F*)key->ReadObj();
      Int_t p,m,t,i;
      sscanf(fun->GetName(), "pdf_%d_%d_%d_%d", &p,&m,&t,&i);
      fhPdf[p][m][t][i]=fun;
      fhPdf[p][m][t][i]->SetDirectory(0);
    }
    fpdf.Close();
  }

  // //nph lut
  // name = fLutFile;
  // name.Remove(name.Last('/')+1);
  // TFile fn_r(name+"nph.root");
  // TIter nextkey(fn_r.GetListOfKeys());
  // TKey *key;
    
  // while ((key = (TKey*)nextkey())) {
  //   TF1 *fun = (TF1*)key->ReadObj();
  //   Int_t i,m,t;
  //   sscanf(fun->GetName(), "%d_%d_%d", &i,&m,&t);
  //   fhNphArr[i][m][t]=fun;
  // }
  // fn_r.Close();
  
  //Double_t mom(0), theta(0),phi(0), trr(0),  nph(0),
    //par1(0), par2(0), par3(0), par4(0), par5(0), par6(0), test1(0), test2(0), test3(0),separation(0); //[R.K. 01/2017] unused variable?

  if(fOutFile.Contains("reco")) fOutFile.ReplaceAll("reco","rt_reco");
  else fOutFile.ReplaceAll(".root","t.root");
  fFileOut = new TFile(fOutFile,"recreate");
  fTreeOut = new TTree("barreldirc","SPR");
  
  fTreeOut->Branch("fMom",&fMom,"fMom/D");
  fTreeOut->Branch("fTheta",&fTheta,"fTheta/D");
  fTreeOut->Branch("fPhi",&fPhi,"fPhi/D");
  fTreeOut->Branch("fPidTrue", &fPidTrue,"fPidTrue/I");
  fTreeOut->Branch("fPidDist", &fPidDist,"fPidDist/I");

  fTreeOut->Branch("fMissId", &fMissId,"fMissId[5]/D");
  fTreeOut->Branch("fEfficiency", &fEfficiency,"fEfficiency[5]/D");

  fTreeOut->Branch("fPidLike", &fPidLike,"fPidLike[2]/I");
  fTreeOut->Branch("fLikelihood",&fLikelihood,"fLikelihood[2]/D");
  fTreeOut->Branch("fSeparation",&fSeparation,"fSeparation[2]/D");
  fTreeOut->Branch("fSpr", &fSpr,"fSpr[5]/D");
  fTreeOut->Branch("fNph", &fNph,"fNph[5]/D");
  fTreeOut->Branch("fCangle",&fCangle,"fCangle[5]/D");
  fTreeOut->Branch("fR1",&fR1,"fR1/D");
  fTreeOut->Branch("fR2",&fR2,"fR2/D");
  
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


  gStyle->SetOptStat(1);
  gStyle->SetOptTitle(1);
  
  for(Int_t i=0; i<5; i++){
    fParticleArray[pdg[i]]=i;
    fPdg[i]=pdg[i];
    fMass[i]=mass[i];
    fHits[i]=0;
    fEvents[i]=0;
    fEventsEff[i]=0;
    fEventsMis[i]=0;
    fMissId[i]=0;
    fEfficiency[i]=0;
    fAngle[i] = 0;
    fFunc[i] = new TF1(Form("f_%d",i),"gaus(0)",0.4,0.9);
    
    fFunc[i]->SetParameter(0,1);
    fFunc[i]->SetParameter(2,0.01);
    
    if(i==2) fFunc[i]->SetParameter(2,0.0095);
    if(i==3) fFunc[i]->SetParameter(2,0.0095); //95

    fhLk1[i] = new TH1F(Form("fhLk1_%d",i),  ";ln L(K) - ln L(#pi);entries [#]",150,-400,400);
    fhLk2[i] = new TH1F(Form("fhLk2_%d",i),  ";ln L(K) - ln L(#pi);entries [#]",150,-400,400);
    fhTang[i] = new TH1F(Form("cherenkov_angle_hist_%d",i),";#theta_{c} [rad];entries [#]", 100,0.4,0.9);
    fhTang[i]->SetMinimum(0);
    fhDiff[i] = new TH1F(Form("fhDiff_%d",i),";t_{lut} - t_{daq} [ns];entries [#]",200,-10,10);
    fhTime[i] = new TH1F(Form("fhTime_%d",i),";time [ns];entries [#]",200,0,100);
    fhNph[i] = new TH1F(Form("fhNph_%d",i),";detected photons [#];entries [#]",150,0,150);
    
    fhTang[i]->SetStats(0);
  }
  fCanvasList = new TList();
  
  fMethod=1;
  
  cout << "-I- PndDrcReco: Intialization successfull" << endl;
  return kSUCCESS;
}

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

    Int_t mcBoxId(-1), barId; //mcBarId,  //[R.K.03/2017] unused variable
    for(int i=0; i<fBarPointArray->GetEntriesFast(); i++){
      fBarPoint = (PndDrcBarPoint*)fBarPointArray->At(i);
      if(itrack == fBarPoint->GetTrackID()){
	mcBoxId = fBarPoint->GetBoxId();
	break;
      }
    }
    if(mcBoxId==-1) continue;
    
    fBarPoint->Momentum(fMomInBar);
    fBarPoint->Position(fPosInBar);
    fTimeInBar = fBarPoint->GetTime();
    fPidTrue = fBarPoint->GetPdgCode();
    fBarPoint->GetBarId();  //mcBarId =  //[R.K.03/2017] unused variable
    
    //tracking smearing
    TVector3 zz = fMomInBar;	
    fMomInBar.SetTheta(fRandom.Gaus(fMomInBar.Theta(),0.002));
    fMomInBar.Rotate(fRandom.Uniform(2*TMath::Pi()), zz);
    
    fMom=fMomInBar.Mag();
    fTheta=fMCTrack->GetMomentum().Theta()*180/TMath::Pi();
    //fPhi=fMomInBar.Phi()*180/TMath::Pi();
    fPhi=fMCTrack->GetMomentum().Phi()*180/TMath::Pi();

    if(fMom>2.5 && nevents==1 && itrack==0){
      fhLk1[2] = new TH1F("fhLk1_2",  ";ln L(K) - ln L(#pi);entries [#]",150,-100,100);
      fhLk1[3] = new TH1F("fhLk1_3",  ";ln L(K) - ln L(#pi);entries [#]",150,-100,100);
    }
    if(fMom>1.5 && nevents==1 && itrack==0){
      fhLk2[2] = new TH1F("fhLk2_2",  ";ln L(K) - ln L(#pi);entries [#]",150,-100,100);
      fhLk2[3] = new TH1F("fhLk2_3",  ";ln L(K) - ln L(#pi);entries [#]",150,-100,100);
    }
    
    Double_t boxPhi;
    DetermineBarId(boxPhi, barId);
    fMomInBar.RotateZ(-boxPhi/180.*TMath::Pi());
    
    DetermineCherenkov(mcBoxId, barId);
  }
}

//Double_t ggg;
Int_t gg_pathid=0;
void PndDrcReco::DetermineCherenkov(Int_t  boxId, Int_t barId){
  
  for(Int_t i=0; i<5; i++) {
    fLk1[i]=0;
    fLk2[i]=0;
    fHitsE[i]=0;
    fAngle[i] = acos(sqrt(fMom*fMom + fMass[i]*fMass[i])/fMom/1.473) + 0.00; //1.4738 = 370 = 3.35
    fFunc[i]->SetParameter(1,fAngle[i]);
    Int_t momid=fMom*10+0.5;
    Int_t thetaid=fTheta+0.5;
    //fFnph[i]=fhNphArr[i][momid][thetaid];

    // std::cout<< momid<<" "<<thetaid <<"  "<< c_spr[2][momid][thetaid] <<" "<<c_spr[3][momid][thetaid] <<std::endl;
    
    if(c_spr[i][momid][thetaid]>0.003 && c_spr[i][momid][thetaid] < 0.1) fFunc[i]->SetParameter(2,c_spr[i][momid][thetaid]);
  }

  //ggg=fRandom.Gaus(0,0.003);
  for(Int_t h=0; h<fPDHitArray->GetEntriesFast(); h++) {
    fPDHit = (PndDrcPDHit*)fPDHitArray->At(h);
 
    Int_t sensorId = fPDHit->GetSensorId();
    fTimeHit = fPDHit->GetTime()-fTimeInBar;
    
    Int_t pointID = fPDHit->GetLink(1).GetIndex();
    //Int_t eventID = fPDHit->GetLink(1).GetEntry(); //[R.K. 01/2017] unused variable?
    
    fPDPoint = (PndDrcPDPoint*)fPDPointArray->At(pointID);
    fBarPoint = (PndDrcBarPoint*)fBarPointArray->At(fPDPoint->GetBarPointID());

    if(fPDPoint->GetTrackID()<1) continue;
    fMCTrack =(PndMCTrack*)fMCArray->At(fPDPoint->GetTrackID());
    if(fMcTrackId!=fMCTrack->GetMotherID()) continue;

    //Double_t en = 1.2398/(fMCTrack->GetMomentum().Mag()*1E6); //[R.K. 01/2017] unused variable?
    hEnergy->Fill(fMCTrack->GetMomentum().Mag()*1E9);
    
    //if(fBarPoint->GetBoxId() != boxId || fBarPoint->GetBarId() != barId) continue;

    Int_t nev=0;
    TVector3 vec;
    gg_pathid=0;
    for(int i=0; i<fEVPointArray->GetEntriesFast(); i++){
      fEVPoint = (PndDrcEVPoint*)fEVPointArray->At(i);
      if(fPDPoint->GetTrackID() == fEVPoint->GetTrackID()){
	nev++;
	vec = fEVPoint->GetNormal();
	gg_pathid += (vec.X()+vec.Y()*10 + vec.Z()*100)*1000*nev;
      }
    }
    
    if(fMethod==0 || fMethod==5) LookUpTable(barId,sensorId);
    if(fMethod==1 || fMethod==5) TimeImaging(sensorId);
  }

  Int_t pid=fParticleArray[fPidTrue];
 
  // std::cout<<"HHHHHHHHHH "<<fLk1[2];
  // fLk1[2]+=fHitsE[pid]*TMath::Log(fFnph[2]->Eval(fHitsE[pid]));
  // fLk1[3]+=fHitsE[pid]*TMath::Log(fFnph[3]->Eval(fHitsE[pid]));
  // std::cout<<" --- "<<fLk1[2]  <<std::endl;
  
  fLikelihood[0] = fLk1[3]-fLk1[2];
  fLikelihood[1] = fLk2[3]-fLk2[2];

  if(fMethod==0 || fMethod==5) {
    fhLk1[pid]->Fill(fLikelihood[0]);
    fHits[pid]+=fHitsE[pid];
    fhNph[pid]->Fill(fHitsE[pid]);
    std::cout<<"method 0  LK = "<< fLikelihood[0] <<std::endl;
  }
  if(fMethod==1 || fMethod==5){
    fhLk2[pid]->Fill(fLikelihood[1]);
    fHits[pid]+= fPDHitArray->GetEntriesFast();
    fhNph[pid]->Fill(fPDHitArray->GetEntriesFast());
    std::cout<<"method 1  LK = "<< fLikelihood[1] << " "<< fPidTrue<<std::endl;
  }

  if(fLikelihood[0]>0) {
    if(fPidTrue==321) fEventsEff[3]++;
    else fEventsMis[3]++;
  }else{
    if(fPidTrue==211) fEventsEff[2]++;
    else fEventsMis[2]++;
  }
  fEvents[pid]++;
  
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
  Double_t evtime, luttime, luttheta, tangle, noise(0.3);
  Int_t pid=fParticleArray[fPidTrue];
  
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

    hPathAll->Fill(node->GetPathId(i));
    if((Int_t)gg_pathid != (Int_t)node->GetPath(i)){
      //continue;
    }else{
      hPath->Fill(node->GetPathId(i));
    }

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

      Double_t tdiff=luttime -fTimeHit;

      tangle = fMomInBar.Angle(dir);

      // tangle += 0.002*tdiff;

      // //mean shift correction
      // if(fPidTrue==211) tangle += shiftPi[(Int_t)(fTheta+0.2-22)];
      // if(fPidTrue==321) tangle += shiftK[(Int_t)(fTheta+0.2-22)];

      Int_t momid=fMom*10+0.5;
      Int_t thetaid=fTheta+0.5;
      if(fabs(c_mean[pid][momid][thetaid])<0.008) tangle += c_mean[pid][momid][thetaid];
      //tangle += 0.5*(c_mean[2][momid][thetaid]+c_mean[3][momid][thetaid]);
      
      // if(tangle<fAngle[3]-0.04 || tangle>fAngle[2]+0.04) continue;
      if(tangle < 0.4 || tangle > 0.9) continue; 

      fhTime[pid]->Fill(fTimeHit);
      fhDiff[pid]->Fill(tdiff);
      
      if(fabs(tdiff)>1.0) continue;

      if(fabs(tangle-fAngle[pid])<0.1) isGood=true;
      //  if(tangle > 0.75 && tangle < 0.85) isGood=true;
      
      hSD->Fill(tangle,tdiff);

      fLk1[2] += TMath::Log((fFunc[2]->Eval(tangle)+noise)); // 211
      fLk1[3] += TMath::Log((fFunc[3]->Eval(tangle)+noise)); // 321

      
      fhTang[pid]->Fill(tangle);
      fHist->Fill(tangle);
    }
  }
  if(isGood) fHitsE[pid]++;
}

void PndDrcReco::TimeImaging(Int_t sensorId){
  Double_t noise = 1e-3;
  Int_t momid=fMom*10+0.5;
  Int_t thetaid=fTheta+0.5;
  Int_t pid=fParticleArray[fPidTrue];
  
  fhTime[pid]->Fill(fTimeHit);
  if(fhPdf[2][momid][thetaid][sensorId] && fhPdf[3][momid][thetaid][sensorId]){
    // TCanvas* c = new TCanvas("c","c",0,0,800,600);
    // fhPdf[2][momid][thetaid][sensorId]->Draw();
    // fhPdf[3][momid][thetaid][sensorId]->Draw("same");
    // c->Update();
    // c->WaitPrimitive();
    fLk2[2] += TMath::Log(fhPdf[2][momid][thetaid][sensorId]->GetBinContent(fhPdf[2][momid][thetaid][sensorId]->FindBin(fTimeHit))+noise); 
    fLk2[3] += TMath::Log(fhPdf[3][momid][thetaid][sensorId]->GetBinContent(fhPdf[3][momid][thetaid][sensorId]->FindBin(fTimeHit))+noise);
    
  }
}

void PndDrcReco::DetermineBarId(Double_t &boxPhi, Int_t &barId){
  Double_t startPhi = fPosInBar.Phi()/TMath::Pi()*180;
  if(startPhi < 0) startPhi = 360 + startPhi;
  if(startPhi >= 0 && startPhi < 90) boxPhi = TMath::Floor(startPhi/fDphi) *fDphi + fDphi/2.;
  if(startPhi >= 90 && startPhi < 270) boxPhi = 90  + fPipehAngle + TMath::Floor((startPhi-90-fPipehAngle)/fDphi) *fDphi + fDphi/2.;
  if(startPhi >= 270 && startPhi < 360) boxPhi = 270 + fPipehAngle + TMath::Floor((startPhi-270-fPipehAngle)/fDphi) *fDphi + fDphi/2.;
  
  if(fRadType==5) barId = (int) (2.5 + (boxPhi-startPhi)/fBarPhi);
  if(fRadType==4) barId = (int) (2 + (boxPhi-startPhi)/fBarPhi);
  if(fRadType==3) barId = (int) (1.5 + (boxPhi-startPhi)/fBarPhi);
  if(fRadType==2) barId = (int) (1 + (boxPhi-startPhi)/fBarPhi);
  if(fRadType==1) barId = 0;
  if(barId>4 || barId<0){
    std::cout<<"Error in PndDrcReco:  Bar Id is wrong. barId = "<< barId <<std::endl;
    barId = -1;
  }
}

Double_t PndDrcReco::FindPeak(){
  Double_t cherenkovreco = -1;

  if(fHist->Integral()>20 ){
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

void PndDrcReco::CanvasAdd(TString name,Int_t w, Int_t h){
  fCanvasList->Add(new TCanvas(name,name,0,0,w,h));
}

void PndDrcReco::CanvasSave(TString path){
  gROOT->SetBatch(1);
  TIter next(fCanvasList);
  TCanvas *c=0;
  gSystem->mkdir(path,kTRUE);
  while((c = (TCanvas*) next())){
    TString name = c->GetName();
    TCanvas *cc = new TCanvas(name+"exp","cExport",0,0,800,400);
    cc = (TCanvas*) c->DrawClone();
    cc->SetCanvasSize(800,400);
    cc->SetBottomMargin(0.12);
    TIter nexth(cc->GetListOfPrimitives());
    TObject *obj;
    while((obj = nexth())){
      if(obj->InheritsFrom("TH1")){
	TH1F *hh = (TH1F*)obj;
	hh->GetXaxis()->SetTitleSize(0.06);
	hh->GetYaxis()->SetTitleSize(0.06);

	hh->GetXaxis()->SetLabelSize(0.05);
	hh->GetYaxis()->SetLabelSize(0.05);

	hh->GetXaxis()->SetTitleOffset(0.85);
	hh->GetYaxis()->SetTitleOffset(0.85);
      }
    }
    cc->Modified();
    cc->Update();
    TString uid("");
    if(fOutFile.Contains("/")){
      TString tname = fOutFile;
      path= tname.Remove(fOutFile.Last('/')) + "/";
      tname = fOutFile;
      uid=tname.Remove(0,fOutFile.Last('/')+1);
    }
    cc->Print(path+uid+name+".png");
  }
  gROOT->SetBatch(0);
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcReco::Finish(){

  Double_t step_mom=0.1; //[0,4]
  Double_t step_theta=1; //[22,140]
  Double_t step_phi=0.4; //[0,22]

  Int_t id_mom = fMom/step_mom+0.1;
  Int_t id_theta = fTheta/step_theta+0.2;
  Int_t id_phi = fPhi/step_phi+0.2;
  TString strrun=Form("_%d_%d_%d",id_mom,id_theta,id_phi);
  
  CanvasAdd("hLikelihood"+strrun);
  TF1 *ff;
  Double_t m1,m2,s1,s2;
  
  if(fMethod==0){
    if(fhLk1[2]->Integral()>10){
      fhLk1[2]->Fit("gaus","S");
      ff = fhLk1[2]->GetFunction("gaus");
      m1=ff->GetParameter(1);
      s1=ff->GetParameter(2);
    }
    if(fhLk1[3]->Integral()>10){
      fhLk1[3]->Fit("gaus","S");
      ff = fhLk1[3]->GetFunction("gaus");
      m2=ff->GetParameter(1);
      s2=ff->GetParameter(2);
    }

    fSeparation[0] = (fabs(m2-m1))/(0.5*(s1+s2));
    std::cout<<"separation1 "<< fSeparation[0] <<std::endl;

    fhLk1[2]->SetTitle(Form("S = %2.2f",fSeparation[0]));
    fhLk1[2]->SetLineColor(4);
    fhLk1[3]->SetLineColor(2);
    fhLk1[2]->Draw();
    fhLk1[3]->Draw("same");
    
    CanvasAdd("hDiff"+strrun);
    fhDiff[2]->SetLineColor(4);
    fhDiff[2]->Draw();
    fhDiff[3]->SetLineColor(2);
    fhDiff[3]->Draw("same");

    CanvasAdd("hPathAll"+strrun);
    hPathAll->Draw();
    hPath->SetLineColor(2);
    hPath->Draw("same");
  
    CanvasAdd("hAngle"+strrun);
    for(Int_t i=2; i<4; i++){
      fFit->SetParameter(1,fAngle[i]); // peak
      fFit->SetParameter(2,0.01);      // width
      if(fhTang[i]->Integral()>10){
	fhTang[i]->Fit("fgaus","Q","",fAngle[i]-0.05,fAngle[i]+0.05);
	fhTang[i]->Fit("fgaus","QM","",fAngle[i]-0.05,fAngle[i]+0.05);
	fSpr[i]=fFit->GetParameter(2);
	fCangle[i]=fFit->GetParameter(1);
      }
    }
    fhTang[2]->SetTitle(Form("#theta_{C} = %2.3f  #sigma = %2.4f      #theta_{C} = %2.3f  #sigma = %2.4f",fCangle[3],fSpr[3],fCangle[2],fSpr[2]));
 
    fhTang[2]->SetLineColor(4);
    fhTang[2]->Draw();

    fhTang[3]->SetLineColor(2);
    fhTang[3]->Draw("same");
  
    TLine *line = new TLine(0,0,0,1000);
    line->SetX1(fAngle[2]);
    line->SetX2(fAngle[2]);
    line->SetY1(gPad->GetUymin());
    line->SetY2(fhTang[2]->GetMaximum()*1.05);
    line->SetLineColor(kBlue);
    line->Draw();
  
    TLine *line1 = new TLine(0,0,0,1000);
    line1->SetX1(fAngle[3]);
    line1->SetX2(fAngle[3]);
    line1->SetY1(gPad->GetUymin());
    line1->SetY2(fhTang[2]->GetMaximum()*1.05);
    line1->SetLineColor(kRed);
    line1->Draw();

    CanvasAdd("hSD"+strrun);
    hSD->Draw("colz");
  }
  
  if(fMethod==1){
    if(fhLk2[2]->Integral()>10){
      fhLk2[2]->Fit("gaus","S");
      ff = fhLk2[2]->GetFunction("gaus");
      m1=ff->GetParameter(1);
      s1=ff->GetParameter(2);
    }
    if(fhLk2[3]->Integral()>10){
      fhLk2[3]->Fit("gaus","S");
      ff = fhLk2[3]->GetFunction("gaus");
      m2=ff->GetParameter(1);
      s2=ff->GetParameter(2);
    }

    fSeparation[1] = (fabs(m2-m1))/(0.5*(s1+s2));
    std::cout<<"separation2 "<< fSeparation[1] <<std::endl;
    
    fhLk2[2]->SetTitle(Form("S = %2.2f",fSeparation[1]));
    fhLk2[2]->SetLineColor(4);
    fhLk2[3]->SetLineColor(2);
    fhLk2[2]->Draw();
    fhLk2[3]->Draw("same");
  }
   
  CanvasAdd("hTime"+strrun);
  fhTime[2]->SetLineColor(4);
  fhTime[2]->Draw();
  fhTime[3]->SetLineColor(2);
  fhTime[3]->Draw("same");

  // nph pdf
  // TFile fn("nph_"+strrun+".root","recreate");
  // CanvasAdd("hNph"+strrun);
  // for(Int_t i=2; i<4; i++){
  //   if(fhNph[i]->Integral()>10){
  //     fhNph[i]->Fit("gaus","Q");
  //     fFnph[i]=fhNph[i]->GetFunction("gaus");
  //     //    fFnph[i]->SetParameter(0,1);
  //     fFnph[i]->SetName(Form("%d_%d_%d",i,id_mom,id_theta));
  //     fFnph[i]->Write();
  //   }
  // }
  // fn.Close();
  
  fhNph[2]->SetLineColor(4);
  fhNph[2]->Draw();
  fhNph[3]->SetLineColor(2);
  fhNph[3]->Draw("same");

  CanvasAdd("hEnergy"+strrun);
  hEnergy->Draw();

  for(Int_t l=0; l<5; l++) fLut[l]->Clear();
  for(Int_t i=0; i<5; i++){
    if(fEvents[i]<1) continue;
    fNph[i]=fHits[i]/(Double_t)fEvents[i];
    fEfficiency[i] = fEventsEff[i]/(Double_t)fEvents[i];
    fMissId[i] = fEventsMis[i]/(Double_t)fEvents[i];
  }

  std::cout<<"N pi "<<fNph[2] << "    N K " <<fNph[3] <<std::endl;
  std::cout<<"Eff_K  "<< fEfficiency[3] << "  Mis_K  "<<fMissId[3] <<std::endl;
  std::cout<<"Eff_pi  "<< fEfficiency[2] << "  Mis_pi  "<<fMissId[2] <<std::endl;
  
  fFileOut->cd();
  fhTang[2]->Write();
  fhTang[3]->Write();
  fTreeOut->Fill();
  fTreeOut->Write();
  fFileOut->Write();

  CanvasSave(Form("data/reco/%d/",id_mom));
  
  cout << "-I- PndDrcReco: Finish" << endl; 
}

ClassImp(PndDrcReco)
