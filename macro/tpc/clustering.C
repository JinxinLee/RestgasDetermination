#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include <assert.h>
#include <iostream>
#include <sstream>
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TAxis.h"

#define NF 5

void clustering(){
  
  gStyle->SetOptFit();
  gStyle->SetCanvasColor(kWhite);

//   TString files[NF]={"data/pion.15deg.reco.root",
// 		     "data/pion.17deg.reco.root",
// 		     "data/pion.20deg.reco.root",
// 		     "data/pion.30deg.reco.root",
// 		     "data/pion.40deg.reco.root",
// 		     "data/pion.50deg.reco.root",
// 		     "data/pion.60deg.reco.root",
// 		     "data/pion.70deg.reco.root",
// 		     "data/pion.80deg.reco.root",
// 		     "data/pion.85deg.reco.root",
// 		     "data/pion.95deg.reco.root",
// 		     "data/pion.100deg.reco.root",
// 		     "data/pion.110deg.reco.root",
// 		     "data/pion.120deg.reco.root",
// 		     "data/pion.130deg.reco.root",
// 		     "data/pion.140deg.reco.root",
// 		     "data/pion.150deg.reco.root",
//   };

  TString files[NF]={"data/elsatest2.5T.0.1.reco.root",
		     "data/elsatest2.5T.0.15.reco.root",
		     "data/elsatest2.5T.0.2.reco.root",
		     "data/elsatest2.5T.0.25.reco.root",
		     "data/elsatest2.5T.0.3.reco.root"};

  //  int angle[NF]={15,17,20,30,40,50,60,70,80,85,95,100,110,120,130,140,150};
  int angle[NF]={0.1,0.15,0.2,0.25,0.3};

  
  TCanvas* c=new TCanvas("Clusters","Clusters",10,10,500,900);
  c->Divide(3,NF);

  //TCanvas* cp=new TCanvas("Momentum","Momentum",15,15,500,900);
  //cp->Divide(3,NF);

  int count=0;

  TGraph* sigx=new TGraph(NF);sigx->SetMarkerStyle(23);
  TGraph* sigy=new TGraph(NF);sigy->SetMarkerStyle(23);

  TH1F* clsize=new TH1F("clsize","clsize",9,1,10);
  TCanvas* c2=new TCanvas("c2","cluster sizes",20,20,450,400);
   TCanvas* c3=new TCanvas("c3","cluster resolution",30,30,450,400);

   // TCanvas* cp2=new TCanvas("cp2","momentum",40,40,450,400);
   // TGraph* sigp=new TGraph(NF);sigp->SetMarkerStyle(22);
   // sigp->SetMarkerSize(2);
   // sigp->SetMarkerColor(kBlue);
   
   //TCanvas* cp3=new TCanvas("cp3","mom offset",50,50,450,400);
   //TGraphErrors* pstartoff=new TGraphErrors(NF);pstartoff->SetMarkerStyle(22);
   //pstartoff->SetMarkerSize(1);
 

   //TGraphErrors* poff=new TGraphErrors(NF);poff->SetMarkerStyle(22);
   //poff->SetMarkerSize(1);
   //poff->SetMarkerColor(kBlue);poff->SetLineColor(kBlue);



   //TGraph* sigpstart=new TGraph(NF);sigpstart->SetMarkerStyle(22);
   //sigpstart->SetMarkerSize(1);

   TFile* file=NULL;
   TTree* tr=NULL;

  for(int i=0; i<NF; ++i){
    std::cout<<"Opening File "<<files[i]<<std::endl;

    file=TFile::Open(files[i],"READ");
    assert(file!=0);
    tr=(TTree*)file->Get("cbmsim");
    assert(tr!=0);
    tr->Print();
    
    c->cd(++count);
    count+=2;
    std::ostringstream histo;
    histo  <<"hRX"<<count;
    std::ostringstream select;
    select << "TrackFitStat._PndTpcResX>>"<<histo.str()<<"(100,-1,1)";
    tr->Draw(select.str().c_str(),"abs(TrackFitStat._PndTpcResX)<1");
    TH1F* hX = (TH1F*)gDirectory->Get(histo.str().c_str());
    assert(hX!=0);
    hX->Fit("gaus");
    sigx->SetPoint(i,angle[i],hX->GetFunction("gaus")->GetParameter(2));

    std::cout<<"Fitted sigx, file#"<<i<<std::endl;
    std::cout.flush();

//     cp->cd(count);
//     tr->Draw("TrackFitStat._pstart>>hpstart(40,0.4,0.6)");
//     tr->Draw("TrackFitStat._p>>hp","","same");
//     TH1F* hp = (TH1F*)gDirectory->Get("hp");
//     hp->SetLineColor(kBlue);
//     hp->Fit("gaus","","same");
//     hp->GetFunction("gaus")->SetLineColor(kBlue);
//     TF1* f=hp->GetFunction("gaus");
//     double dp_p=f->GetParameter(2)/f->GetParameter(1);
//     sigp->SetPoint(i,angle[i],dp_p);
//     double off=(f->GetParameter(1)-0.5)/0.5;
//     poff->SetPoint(i,angle[i],off);
//     poff->SetPointError(i,0,f->GetParError(1)/0.5);
    

//     TH1F* hpstart = (TH1F*)gDirectory->Get("hpstart");
//     hpstart->Fit("gaus","","same");
//     f=hpstart->GetFunction("gaus");
//     dp_p=f->GetParameter(2)/f->GetParameter(1);
//     sigpstart->SetPoint(i,angle[i],dp_p);

//     off=(f->GetParameter(1)-0.5)/0.5;
//     pstartoff->SetPoint(i,angle[i],off);
//     pstartoff->SetPointError(i,0,f->GetParError(1)/0.5);

//     c->cd(++count);
//     std::ostringstream histo2;
//     std::ostringstream select2;
//     histo2  <<"hRY"<<count;
//     select2 << "TrackFitStat._PndTpcResY>>"<<histo2.str()<<"(100,-1,1)";
//     tr->Draw(select2.str().c_str(),"abs(TrackFitStat._PndTpcResY)<1");
//     TH1F* hY = (TH1F*)gDirectory->Get(histo2.str().c_str());
//     hY->Fit("gaus");
    

//     c->cd(++count);
//     tr->Draw("PndTpcCluster._size","");
//     c2->cd();
//     tr->Draw("PndTpcCluster._size>>clsize","",i==0 ? "" : "same");
//     TH1F* h = (TH1F*)gDirectory->Get("clsize");
//     h->GetYaxis()->SetRangeUser(0,800);
//     h->SetLineColor(i);
    
        //delete file();

  }// end loop over files


  std::cout<<"FINISHED!"<<std::endl; 
  c3->cd();sigx->Draw("AP");
  sigx->SetTitle("Cluster resolution");
  sigx->GetHistogram()->GetXaxis()->SetTitle("#theta");
  sigx->GetHistogram()->GetYaxis()->SetTitle("#sigma [cm]");

//   cp2->cd();
//   sigp->SetTitle("Momentum resolution (at 500MeV)");
//   sigp->Draw("AP");
//   sigp->GetHistogram()->GetYaxis()->SetRangeUser(0,0.04);
//   sigp->GetHistogram()->GetYaxis()->SetTitle("dp/p");
//   sigp->GetHistogram()->GetYaxis()->SetTitleOffset(1.4);
//   sigp->GetHistogram()->GetXaxis()->SetTitle("#theta");
//   sigpstart->Draw("P");

//   cp3->cd();
//   pstartoff->Draw("AP");
//   poff->Draw("P");
//   pstartoff->GetHistogram()->GetYaxis()->SetRangeUser(-0.025,0.01);

  c3->SaveAs("plots/clustPlots/clustres.eps");
  //  cp2->SaveAs("plots/clustPlots/pres_vs_theta.eps");
  //cp3->SaveAs("plots/clustPlots/poffset_vs_theta.eps");



  std::cout<<"FINISHED!"<<std::endl;


  


}
