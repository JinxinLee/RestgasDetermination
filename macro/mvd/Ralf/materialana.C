// root macro to analyze the simulation output
#include <map>
#include <string>
#include <iostream>
#include <vector>

// #include <TStopwatch.h>
#include <TFile.h>
#include <TTree.h>
#include <TGeoManager.h>
#include <TClonesArray.h>
#include <TH1D.h>
#include <TROOT.h> // for global root variables
#include <TH2D.h>
#include <TVector3.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TStopwatch.h>
#include <THStack.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TColor.h>

// called from pandaroot/macro/mvd/Ralf
//how to do this better?
#include "$VMCWORKDIR/base/FairRadLenPoint.h"
#include "$VMCWORKDIR/pnddata/PndMCTrack.h"
// #include "$VMCWORKDIR/mvd/MvdTools/PndGeoHandling.h"
#include "$VMCWORKDIR/macro/mvd/Tools.C"

int materialana(int nEvents = 10, bool verbose = false)
{
// int nEvents = 100;
//   bool verbose  = false;
  bool draw1    = true;
  bool draw2    = true;
  bool draw3    = true;

  // -----  Load libraries   ------------------------------------------------
//   gROOT->Macro("../Libs.C");
//   gROOT->LoadMacro("../Tools.C");
  LoadPandaStyle();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
//   std::string inFile = "../data/mvdmaterial.root";
    std::string inFile = "../data/mvdTestGeo.root";

  TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("cbmsim") ;

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);//Branch names

  TClonesArray* rad_array=new TClonesArray("FairRadLenPoint");
  t->SetBranchAddress("RadLen",&rad_array);

  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");
//   PndGeoHandling* fGeoH = new PndGeoHandling();

  // histos
  int res = 200; int angres = 180;
  TH1D* hRadLenDistMat = new TH1D("radldm","Material thicknesses",100,0,15);
  TH1D* hRadLenDistEff = new TH1D("radlde","Effective Radiation length values per volume",100,0,0.1);
  TH2D* hisxy = new TH2D("hisxy","MC Points, xy view",res,-15.,15.,res,-15.,15.);
  TH2D* hisrz = new TH2D("hisrz","MC Points, rz view",res,-20.,20.,res,-15.,25.);

  TClonesArray* arrhisxy = new TClonesArray("TH2D");
  TClonesArray* arrhisrz = new TClonesArray("TH2D");
  TClonesArray* arrprothe = new TClonesArray("TProfile");
  TClonesArray* arrprophi = new TClonesArray("TProfile");

  std::vector<std::string> namesList;
//   namesList.push_back("StripSensor");
//   namesList.push_back("SensorActive");
  namesList.push_back("pipe");
  namesList.push_back("cave");//Reste, ausser "/cave"
  namesList.push_back("Sensor");
//   namesList.push_back("Support");
//   namesList.push_back("MVD");

  std::map<int,double> radlList;
  int maxnames=namesList.size();
  for(int i=0;i<maxnames;i++){
    TString name  = "hisxy-";
    TString name2 = "hisrz-";
    TString name3 = "hthe-";
    TString name4 = "hphi-";
    name += i;   name2 += i;   name3 += i;   name4 += i;
    new ((*arrhisxy)[i]) TH2D(name.Data(),(namesList[i]).c_str(),2*res,-15.,15.,2*res,-15.,15.);
    new ((*arrhisrz)[i]) TH2D(name2.Data(),(namesList[i]).c_str(),2*res,-15.,15.,2*res,-15.,15.);
    new ((*arrprothe)[i]) TProfile(name3.Data(),(namesList[i]).c_str(),angres,0.,TMath::Pi());
    new ((*arrprophi)[i]) TProfile(name4.Data(),(namesList[i]).c_str(),angres,-1.*TMath::Pi(),TMath::Pi());
    radlList[i]=0.;
  }
  new ((*arrhisxy)[maxnames]) TH2D("hisxy-rest","Support",2*res,-15.,15.,2*res,-15.,15.);
  new ((*arrhisrz)[maxnames]) TH2D("hisrz-rest","Support",2*res,-15.,15.,2*res,-15.,15.);
  new ((*arrprothe)[maxnames]) TProfile("hthe-rest","Support",angres,0.,TMath::Pi());
  new ((*arrprophi)[maxnames]) TProfile("hphi-rest","Support",angres,-1.*TMath::Pi(),TMath::Pi());
  radlList[maxnames]=0.;

  TProfile* thetaprofile = new TProfile("thetaprof","Radiaion length (#theta)",angres,0.,TMath::Pi());
  TProfile* phiprofile = new TProfile("phiprof","Radiaion length (#phi)",angres,-1.*TMath::Pi(),TMath::Pi());

//   TH2D* histhephi = new TH2D("hThePhi","",100,0.2,TMath::Pi()-0.3,100,-1.*TMath::Pi(),TMath::Pi());
  TProfile2D* histhephi = new TProfile2D("hThePhi","",90,0.2,2.8,90,-1.*TMath::Pi(),TMath::Pi());
  histhephi->SetTitle("Radiation length map #theta & #phi;#theta;#phi");

  TH1D* hisx = new TH1D("hX","X",100,-15.,15.);
  TH1D* hisy = new TH1D("hY","Y",100,-15.,15.);
  TH1D* hisz = new TH1D("hZ","Z",100,-15.,15.);

  TString detname,volname;
  double radlen=0.,effradl=0.,effradlsum=0.,theta=0.,phi=0.;
  TVector3 in, out, dist, point;
  TVector3 vtx, mom;

  for (Int_t event=0; event<nEvents && event<t->GetEntriesFast(); event++)
  {
    t->GetEntry(event);
    if(verbose) cout<<"Event No "<<event<<endl;
    else if (!(event%1000)) cout <<"Event No "<<event<<endl;

    for (Int_t trackno=0; trackno<mc_array->GetEntriesFast();trackno++){
      PndMCTrack* aTrack = (PndMCTrack*)mc_array->At(trackno);
      mom = aTrack->GetMomentum();
      theta = mom.Theta(); phi = mom.Phi();
//       if(theta < 0.05 || theta > (TMath::Pi()-0.05)) continue; // cut strange angles
//       vtx = aTrack->GetStartVertex();
      effradl=0.; effradlsum=0.;
      for(uint i=0;i<radlList.size();i++) radlList[i]=0.;
      for (Int_t k=0; k<rad_array->GetEntriesFast(); k++){
        FairRadLenPoint* radpoint = (FairRadLenPoint*)rad_array->At(k);
        if (radpoint->GetTrackID() != trackno) continue;
        radlen = radpoint->GetRadLength();
        in = radpoint->GetPosition();
        out = radpoint->GetPositionOut();
        dist = in - out;
        point.SetXYZ(0.5*(in.x()+out.x()),0.5*(in.y()+out.y()),0.5*(in.z()+out.z()));
        if(in.Mag() < 0.02) continue; // cut target
        effradl = dist.Mag()/radlen;
        effradlsum += effradl;
        TGeoNode* node = geoMan->FindNode(point.x(),point.y(),point.z());
        if( 0==node) 
        {
          std::cout<<"Warning: There is a node not defined properly!"<<std::endl;
          cout<<"\tEvent No "<<event<<" \t RadLenPoint No."<< k<<endl;
          continue;
        }
        node->cd();
//         detname = geoMan->GetPath();
        volname = node->GetVolume()->GetName();
//         if(verbose)cout<<detname.Data()<<endl;
        if("cave"==volname && point.Mag() > 20.) continue; // cut mainly the outer cave
        if(verbose){
          cout<<"--> "<<volname.Data()<<endl;
        }

        hRadLenDistMat->Fill(dist.Mag());
        hRadLenDistEff->Fill(effradl);

        hisx->Fill(in.X(),effradl);
        hisy->Fill(in.Y(),effradl);
        hisz->Fill(in.Z(),effradl);

        hisxy->Fill(in.X(),in.Y());
        if(in.Y()>0.) hisrz->Fill(in.Z(),in.Perp());
        else hisrz->Fill(in.Z(),-1.*in.Perp());

        int selected = maxnames;
        for(uint i=0;i<namesList.size();i++){
          if (volname.Contains( (namesList[i]).c_str())){
            selected=i;
            break;
          }
        }
        radlList[selected]+=effradl;
        ((TH2D*)arrhisxy->At(selected))->Fill(point.X(),point.Y(),effradl);
        if(point.Y()>0.) ((TH2D*)arrhisrz->At(selected))->Fill(point.Z(),point.Perp(),effradl);
        else ((TH2D*)(arrhisrz->At(selected)))->Fill(point.Z(),-1.*point.Perp(),effradl);
      }//radpoints 
      thetaprofile->Fill(theta,effradlsum);
      phiprofile->Fill(phi,effradlsum);
      histhephi->Fill(theta,phi,effradlsum);

      for(int i=0;i<=maxnames;i++){
        effradl=radlList[i];
        ((TProfile*)arrprothe->At(i))->Fill(theta,effradl);
        ((TProfile*)arrprophi->At(i))->Fill(phi,effradl);
      }

    }//tracks

  }// end for event



Int_t a=3,b=3;
int resol = 250;
if(draw1){
TCanvas* can1 = new TCanvas("can1","MCHit view in MVD",0,0,a*resol,b*resol);
can1->Divide(a,b);

can1->cd(1);
hisxy->SetStats(false);
hisxy->DrawCopy("colz");

can1->cd(2);
hisrz->SetStats(false);
hisrz->DrawCopy("colz");

can1->cd(3);
TPad* mypad = (TPad*) gPad;
mypad->Divide(2,2);
mypad->cd(1);
hisx->SetFillColor(38);
hisx->DrawCopy();
mypad->cd(2);
hisy->SetFillColor(45);
hisy->DrawCopy();
mypad->cd(3);
hisz->SetFillColor(30);
hisz->DrawCopy();
mypad->cd(4);
// THStack* hs = new THStack("hs","test stacked histograms");
// //makes no sens - just test technics
// hs->Add(hisx);
// hs->Add(hisy);
// hs->Add(hisz);
// hs->Draw();
// // TLegend* legend = BuildLegend_THStack( hs, 0.8, 0.55, 0.98, 0.98);
// TLegend* legend = new TLegend(0.8, 0.55, 0.98, 0.98);
// legend->AddEntry(hisx,"","F");
// legend->AddEntry(hisy,"","F");
// legend->AddEntry(hisz,"","F");
// legend->Draw();

can1->cd(4);
gPad->SetLogy();
hRadLenDistMat->SetFillColor(38);
hRadLenDistMat->DrawCopy();

can1->cd(5);
gPad->SetLogy();
hRadLenDistEff->SetFillColor(38);
hRadLenDistEff->DrawCopy();

can1->cd(6);

can1->cd(7);
gPad->SetLogy();
thetaprofile->SetFillColor(30);
thetaprofile->SetLineColor(30);
thetaprofile->Draw("hist");

can1->cd(8);
gPad->SetLogy();
phiprofile->SetFillColor(45);
phiprofile->SetLineColor(45);
phiprofile->Draw("hist");

can1->cd(9);
histhephi->SetStats(false);
histhephi->DrawCopy("colz");

}


if(draw2){
resol = 250;
a=2,b=1;
TCanvas* can2 = new TCanvas("can2","MCHit view in MVD",50,50,a*2*resol,b*2*resol);
can2->Divide(a,b);

resol = 300;
a=2,b=2;
TCanvas* can4 = new TCanvas("can4","MCHit view in MVD",100,100,a*resol,b*resol);
can4->Divide(a,b);

/*EColor colors[12] = 
  {kOrange,kAzure,kTeal ,kRed,kBlue,kGreen, kMagenta,kCyan,kYellow ,kPink,kViolet,kSpring };*/
int colors[4] = {38,17,45,30};
int coloff = -0;
THStack* thetastack = new THStack("thetastack","");
thetastack->SetTitle("Stacked Radiation lengths #theta;#theta;X/X_{0}    ");
THStack* phistack = new THStack("phistack","");
phistack->SetTitle("Stacked Radiation lengths #phi;#phi;X/X_{0}    ");
TProfile* aprof=0;
TH1D* ahist=0;
TLegend* legMat=new TLegend(0.6, 0.75, 0.8, 0.98);

for(int i=maxnames;i>=0;i--){
  aprof=(TProfile*)arrprothe->At(i);
  ahist=aprof->ProjectionX();
  ahist->SetFillColor(colors[i] + coloff);
  ahist->SetLineColor(colors[i] + coloff);
  legMat->AddEntry(ahist,"","F");
  can4->cd(i+1);
  gPad->SetLogy();
  ahist->DrawCopy("hist");
  thetastack->Add(ahist);  

  aprof=(TProfile*)arrprophi->At(i);
  ahist=aprof->ProjectionX();
  ahist->SetFillColor(colors[i] + coloff);
  ahist->SetLineColor(colors[i] + coloff);
  phistack->Add(ahist);  
}
can2->cd(1);
thetastack->SetMaximum(0.1);
thetastack->Draw("hist");
legMat->Draw();

can2->cd(2);
phistack->SetMaximum(0.1);
phistack->Draw("hist");
legMat->Draw();



}

if(draw3){

a=maxnames+1;
b=2;
if(a>4) resol = int(1200/a);
TCanvas* can3 = new TCanvas("can3","MCHit view in MVD",150,150,a*resol,b*resol);
can3->Divide(a,b);

for(int i=0;i<a;i++){
  can3->cd(i+1);
  ((TH2D*)arrhisxy->At(i))->DrawCopy("colz");
  can3->cd(i+a+1);
  ((TH2D*)arrhisrz->At(i))->DrawCopy("colz");
}
}

// can1->Print("outAnaMvdSim.ps");
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //cout << "Output file is "    << outFile << endl;
  //cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

  return 0;
}
