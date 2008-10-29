#include <iostream>
#include <fstream>
#include "TString.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TROOT.h"

#include "../src/TCtrack.h"
#include "../src/TCcluster.h"


//#include "consecCut.C"
//#include "clusterSplit1.C"
//#include "oneRow3.C"

void plots(TString files){

  //gSystem->Load("libtestChamber"); 


  //gROOT->Macro("macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;

//   TH1D *yresid = new TH1D("yresidall","",500,-1,1);
//   yresid->SetXTitle("residual y [cm]");
//   TH1D *xresid = new TH1D("xresidall","",500,-1,1);
//   xresid->SetXTitle("residual x [cm]");
  
//   TH1D *chi2 = new TH1D("chi2","chi2/NDF",200,-4,10);

  myChain.SetBranchAddress("track", &intr);
 
  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);
    //if(tr.nClFit()<5)continue;
    /*
    TCtrack clone (tr);
    std::vector<TCcluster> newCl;
    for(int i=0;i<clone.nCl();++i){
      TCcluster c = clone.getCl(i);
      c.setId(260);
      newCl.push_back(c);
    }
    clone.clear();
    clone.addClusters(newCl);
    clone.fit(260);
    */
    //clone.draw(false,650,50,500,500);

    /*
    TCtrack *clone2 = new TCtrack(tr);
    //clone2->useTrivialClustering();
    clone2->draw(true,650,50,500,500);
    tr.setCustomRange(true,0,9,0,1,0,8);
    tr.drawClustering(true,50,50,500,500);
    //tr.draw(true,50,50,500,500);
    delete clone2;
    */
    //    intr->draw();
    // if(tr.nClFit()<7)continue;

    //TCtrack trSplit = clusterSplit1(tr);

    tr.drawClustering(false,20,50,400,400);
    //tr.useTrivialClustering();
    tr.draw(true,650,50,400,400);


    //tr.drawClustering(false,20,50,800,400);
    //tr.draw(true,700,50,800,400);

    //    tr.draw();
    //tr.setCustomRange(true,0,9,0,1,0,8);
    //    tr.draw();//false,50,50,500,500);
    //    tr.setCustomRange(false);
    //tr.useTrivialClustering();
    //tr.fit(200);
    //tr.draw(true,650,50,500,500);
    //tr.sort();
    //for(int i=0;i<tr.nCl();++i){
    //  tr.getCl(i).print();
    //}
    //tr.fit(200);
    //TCtrack clone (tr);
    //clone.setCustomRange(true,0,9,0,1,0,8);
    //clone.draw(false,50,50,500,500);
    //clone.useTrivialClustering();
    //clone.draw(false,650,50,500,500);

    //    std::cout << consecCut(tr) << std::endl;
    //        std::cout << oneRow3(tr) << std::endl;
  }
    //tr.setCustomRange(true,0,9,0,1,0,8);
    //tr.drawClustering(true,50,50,500,500);

    

    //    continue;
//     if(tr.nClFit()>4.)
//       chi2->Fill(tr.getChi2()/tr.getNDF());
//     for(int i=0;i<tr.nCl();++i){
//       TCcluster c = tr.getCl(i);
//       std::cout << i << " " << c.nRaw()<<std::endl;
//     }

//     std::cout << tr.nClFit() << std::endl;
/*
    if(tr.nClFit()>4.){
      for(int i=0;i<tr.nCl();++i){
	TCcluster c = tr.getCl(i);
	if(c.getFit()){//was used in fit
	  yresid->Fill(c.getRes().Y());
	  xresid->Fill(c.getRes().X());
	}
      }
    }
*/
  //yresid->Draw();
  //xresid->Draw();
  //chi2->Draw();

}

