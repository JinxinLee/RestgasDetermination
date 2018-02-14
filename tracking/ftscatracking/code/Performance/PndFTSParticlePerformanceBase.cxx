// $Id: PndFTSCAPerformanceBase.cxx,v 1.11 2010/08/26 15:05:50 ikulakov Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#include "PndFTSCounters.h"

#include "PndFTSParticlePerformanceBase.h"
#include "PndFTSPerformanceBase.h"

#include "PndFTSCAGBHit.h"
#include "PndFTSCAMCTrack.h"
#ifndef HLTCA_STANDALONE
#include "PndFTSCAMCPoint.h"
#endif

#include "PndFTSCAGBTrack.h"
#include "PndFTSCAGBTracker.h"

#include "PndFTSCADisplay.h"


#include "TMath.h"
#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"


PndFTSParticlePerformanceBase::PndFTSParticlePerformanceBase()
{
  const int NHisto_tmp = NTrackPulls + NTrackPullsAtProdVertex + NVertexPulls;

  NHisto = NHisto_tmp;
  fHistosInfo = new THistoInfo[NHisto];
  fHistos = new TH1*[NHisto];

  const float coeff1 = 10;
  const THistoInfo tmp[NHisto_tmp]=
  {
    THistoInfo( "resX",       "track X resolution [cm]",  30, -0.25,  0.25 ),
    THistoInfo( "resY",       "track Y resolution [cm]",  30, -0.25,  0.25 ),
    THistoInfo( "resZ",       "track Z resolution [cm]",  30, -0.7,   0.7 ),
    THistoInfo( "resPx",      "track Px resolution",      30, -0.3,   0.3 ),
    THistoInfo( "resPy",      "track Py resolution",      30, -0.3,   0.3 ),
    THistoInfo( "resPz",      "track Pz resolution",      30, -0.3,   0.3 ),
    THistoInfo( "pullX",      "track X pull",             30, -7.,   7. ),
    THistoInfo( "pullY",      "track Y pull",             30, -7.,   7. ),
    THistoInfo( "pullZ",      "track Z pull",             30, -7.,   7. ),
    THistoInfo( "pullPx",     "track Px pull",          30, -7.,   7. ),
    THistoInfo( "pullPy",     "track Py pull",          30, -7.,   7. ),
    THistoInfo( "pullPz",     "track Pz pull",          30, -7.,   7. ),

    THistoInfo( "resAtProdPointX",       "track X resolution at prodaction point [cm]",  30, -0.25*coeff1,  0.25*coeff1 ),
    THistoInfo( "resAtProdPointY",       "track Y resolution at prodaction point [cm]",  30, -0.25*coeff1,  0.25*coeff1 ),
    THistoInfo( "resAtProdPointZ",       "track Z resolution at prodaction point [cm]",  30, -0.7*coeff1,   0.7*coeff1 ),
    THistoInfo( "resAtProdPointPx",      "track Px resolution at prodaction point",      30, -0.3,   0.3 ),
    THistoInfo( "resAtProdPointPy",      "track Py resolution at prodaction point",      30, -0.3,   0.3 ),
    THistoInfo( "resAtProdPointPz",      "track Pz resolution at prodaction point",      30, -0.3,   0.3 ),
    THistoInfo( "pullAtProdPointX",      "track X pull at prodaction point",             30, -7.,   7. ),
    THistoInfo( "pullAtProdPointY",      "track Y pull at prodaction point",             30, -7.,   7. ),
    THistoInfo( "pullAtProdPointZ",      "track Z pull at prodaction point",             30, -7.,   7. ),
    THistoInfo( "pullAtProdPointPx",     "track Px pull at prodaction point",          30, -7.,   7. ),
    THistoInfo( "pullAtProdPointPy",     "track Py pull at prodaction point",          30, -7.,   7. ),
    THistoInfo( "pullAtProdPointPz",     "track Pz pull at prodaction point",          30, -7.,   7. ),

    THistoInfo( "resVertexX",       "vertex X resolution [cm]",  30, -0.25,  0.25 ),
    THistoInfo( "resVertexY",       "vertex Y resolution [cm]",  30, -0.25,  0.25 ),
    THistoInfo( "resVertexZ",       "vertex Z resolution [cm]",  30, -0.7,   0.7 ),
    THistoInfo( "pullVertexX",      "vertex X pull",             30, -7.,   7. ),
    THistoInfo( "pullVertexY",      "vertex Y pull",             30, -7.,   7. ),
    THistoInfo( "pullVertexZ",      "vertex Z pull",             30, -7.,   7. )
  };

  for (int iHisto = 0; iHisto < NHisto; iHisto++){
    fHistosInfo[iHisto] = tmp[iHisto];
  }
  
  for( int i=0; i < NHisto; i++ ){
    fHistos[i] = 0;
  }
}


void PndFTSParticlePerformanceBase::CreateHistos(string histoDir, TFile* outFile)
{
  TDirectory *curdir = gDirectory;
  if ( (histoDir != "") && outFile) {  // create in file
    outFile->cd();
    fHistoDir = outFile->mkdir( TString(histoDir) );
    fHistoDir->cd();
    gDirectory->mkdir( "TrackInput" );
    gDirectory->cd( "TrackInput" );

    int ih = 0; // i of Histo
    for( int i = 0; i < NTrackPulls; i++, ih++ ){
      fHistos[ih] = new TH1D(fHistosInfo[ih].name, fHistosInfo[ih].title, fHistosInfo[ih].nx, fHistosInfo[ih].left, fHistosInfo[ih].right);
    }

    gDirectory->cd( ".." );
    gDirectory->mkdir( "TrackAtVertex" );
    gDirectory->cd( "TrackAtVertex" );

    for( int i = 0; i < NTrackPullsAtProdVertex; i++, ih++ ){
      fHistos[ih] = new TH1D(fHistosInfo[ih].name, fHistosInfo[ih].title, fHistosInfo[ih].nx, fHistosInfo[ih].left, fHistosInfo[ih].right);
    }

    gDirectory->cd( ".." );
    gDirectory->mkdir( "Vertex" );
    gDirectory->cd( "Vertex" );

    for( int i = 0; i < NVertexPulls; i++, ih++ ){
      fHistos[ih] = new TH1D(fHistosInfo[ih].name, fHistosInfo[ih].title, fHistosInfo[ih].nx, fHistosInfo[ih].left, fHistosInfo[ih].right);
    }

    gDirectory->cd( ".." );


    gDirectory->mkdir("KFParticlesFinder");
    gDirectory->cd("KFParticlesFinder");
    histodir = gDirectory;
    gDirectory->mkdir("Particles");
    gDirectory->cd("Particles");
    {
      for(int iPart=0; iPart<fParteff.nParticles; ++iPart)
      {
        gDirectory->mkdir(fParteff.partName[iPart].Data());
        gDirectory->cd(fParteff.partName[iPart].Data());
        {
          TString res = "res";
          TString pull = "pull";
  
          gDirectory->mkdir("DaughtersQA");
          gDirectory->cd("DaughtersQA");
          {
            TString parName[nFitQA/2] = {"X","Y","Z","Px","Py","Pz","E","M"};
            int nBins = 100;
//            float xMax[nFitQA/2] = {0.15,0.15,0.03,0.01,0.01,0.06,0.06,0.01};
            float xMax[nFitQA/2] = {2.,2.,5.,0.3,0.3,0.3,0.03,0.03};
  
            for( int iH=0; iH<nFitQA/2; iH++ ){
              hFitDaughtersQA[iPart][iH]   = new TH1F((res+parName[iH]).Data(),
                                                      (res+parName[iH]).Data(), 
                                                      nBins, -xMax[iH],xMax[iH]);
              hFitDaughtersQA[iPart][iH+8] = new TH1F((pull+parName[iH]).Data(),
                                                      (pull+parName[iH]).Data(), 
                                                      nBins, -6,6);
            }
          }
          gDirectory->cd(".."); //particle directory
  
          gDirectory->mkdir("FitQA");
          gDirectory->cd("FitQA");
          {
            TString parName[nFitQA/2] = {"X","Y","Z","Px","Py","Pz","E","M"};
            int nBins = 50;
//            float xMax[nFitQA/2] = {0.15,0.15,1.2,0.02,0.02,0.15,0.15,0.006};
            float xMax[nFitQA/2] = {2.,2.,5.,0.3,0.3,0.3,0.03,0.03};
  
            for( int iH=0; iH<nFitQA/2; iH++ ){
              hFitQA[iPart][iH]   = new TH1F((res+parName[iH]).Data(),
                                            (res+parName[iH]).Data(), 
                                            nBins, -xMax[iH],xMax[iH]);
              hFitQA[iPart][iH+8] = new TH1F((pull+parName[iH]).Data(),
                                            (pull+parName[iH]).Data(), 
                                            nBins, -6,6);
            }
          }
          gDirectory->cd(".."); //particle directory
  
          gDirectory->mkdir("Parameters");
          gDirectory->cd("Parameters");
          {
            TString parName[nHistoPartParam] = {"M","p","p_{t}","y","DL","c#tau","chi2ndf","prob","#theta","phi","z","l/dl"};
            TString parAxisName[nHistoPartParam] = {"m [GeV/c^{2}]","p [GeV/c]","p_{t} [GeV/c]",
                                                    "y","Decay length [cm]","Life time c#tau [cm]",
                                                    "chi2/ndf","prob","#theta [rad]",
                                                    "phi [rad]","z [cm]", "l/dl"};
            int nBins[nHistoPartParam] = {1000,100,100,100,100,100,100,100,100,100,100,1000};
            float xMin[nHistoPartParam] = {fParteff.partMHistoMin[iPart],  0., 0., 0., -5.,  0.,  0., 0., -2., -2., -5., -1.};
            float xMax[nHistoPartParam] = {fParteff.partMHistoMax[iPart], 10., 3., 6., 55., 30., 20., 1.,  2.,  2., 55., 35.};
  
            for(int iH=0; iH<nHistoPartParam; iH++)
            {
              hPartParam[iPart][iH]       = new TH1F(parName[iH].Data(),parName[iH].Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);
              hPartParam[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
            }
  
            hPartParam2D[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                              nBins[3],xMin[3],xMax[3],
                                              nBins[2],xMin[2],xMax[2]);
            hPartParam2D[iPart][0]->GetXaxis()->SetTitle("y");
            hPartParam2D[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");
  
            gDirectory->mkdir("Signal");
            gDirectory->cd("Signal");
            {
              for(int iH=0; iH<nHistoPartParam; iH++)
              {
                hPartParamSignal[iPart][iH] = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);
                hPartParamSignal[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
              }
  
              hPartParam2DSignal[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                                      nBins[3],xMin[3],xMax[3],
                                                      nBins[2],xMin[2],xMax[2]);
              hPartParam2DSignal[iPart][0]->GetXaxis()->SetTitle("y");
              hPartParam2DSignal[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");
  
              gDirectory->mkdir("QA");
              gDirectory->cd("QA");
              {
                int nBinsQA = 50;
                float xMaxQA[nHistoPartParamQA] = {0.01,0.001,0.001};
                for( int iH=0; iH<nHistoPartParamQA; iH++ ){
                  hPartParamQA[iPart][iH] = 
                    new TH1F((res+parName[iH]).Data(), (res+parName[iH]).Data(), nBinsQA, -xMaxQA[iH],xMaxQA[iH]);
                  hPartParamQA[iPart][iH+nHistoPartParamQA] = 
                    new TH1F((pull+parName[iH]).Data(), (pull+parName[iH]).Data(), nBinsQA, -6,6);
                }
              }
              gDirectory->cd(".."); // particle directory / Parameters / Signal
            }
            gDirectory->cd(".."); // particle directory / Parameters
            gDirectory->mkdir("Background");
            gDirectory->cd("Background");
            {
              for(int iH=0; iH<nHistoPartParam; iH++)
              {
                hPartParamBG[iPart][iH]     = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);
                hPartParamBG[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
              }
  
              hPartParam2DBG[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                                  nBins[3],xMin[3],xMax[3],
                                                  nBins[2],xMin[2],xMax[2]);
              hPartParam2DBG[iPart][0]->GetXaxis()->SetTitle("y");
              hPartParam2DBG[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");
            }
  //           gDirectory->cd(".."); // particle directory
  //           gDirectory->mkdir("CorrBG");
  //           gDirectory->cd("CorrBG");
  //           {
  //             for(int iH=0; iH<nHistoPartParam; iH++)
  //             {
  //               hPartParamCorrBG[iPart][iH]     = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
  //                                             nBins[iH],xMin[iH],xMax[iH]);
  //               hPartParamCorrBG[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
  //             }
  // 
  //             hPartParam2DCorrBG[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
  //                                                 nBins[3],xMin[3],xMax[3],
  //                                                 nBins[2],xMin[2],xMax[2]);
  //             hPartParam2DCorrBG[iPart][0]->GetXaxis()->SetTitle("y");
  //             hPartParam2DCorrBG[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");
  //           }
            gDirectory->cd(".."); // particle directory
            gDirectory->mkdir("Ghost");
            gDirectory->cd("Ghost");
            {
              for(int iH=0; iH<nHistoPartParam; iH++)
              {
                hPartParamGhost[iPart][iH]     = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);
                hPartParamGhost[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
              }

              hPartParam2DGhost[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                                  nBins[3],xMin[3],xMax[3],
                                                  nBins[2],xMin[2],xMax[2]);
              hPartParam2DGhost[iPart][0]->GetXaxis()->SetTitle("y");
              hPartParam2DGhost[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");
            }
            gDirectory->cd(".."); // particle directory
          }
          gDirectory->cd(".."); //particle directory
        }
        gDirectory->cd(".."); //Particles
      }
    }
    gDirectory->cd(".."); //L1
    gDirectory->mkdir("PrimaryVertexQA");
    gDirectory->cd("PrimaryVertexQA");
    {
      struct {
        string name;
        string title;
        Int_t n;
        Double_t l,r;
      } Table[nHistosPV]=
      {
        {"PVResX", "x_{rec}-x_{mc}, cm",     100, -0.2f, 0.2f},
        {"PVResY", "y_{rec}-y_{mc}, cm",     100, -0.2f, 0.2f},
        {"PVResZ", "z_{rec}-z_{mc}, cm",     100, -0.2f, 0.2f},
        {"PVPullX", "Pull X",     100, -6.f, 6.f},
        {"PVPullY", "Pull Y",     100, -6.f, 6.f},
        {"PVPullZ", "Pull Z",     100, -6.f, 6.f}
      };
      for(int iHPV=0; iHPV<nHistosPV; ++iHPV){
        hPVFitQa[iHPV] = new TH1F(Table[iHPV].name.data(),Table[iHPV].title.data(),
                                  Table[iHPV].n, Table[iHPV].l, Table[iHPV].r);
      }
    }
    gDirectory->cd(".."); //L1
    gDirectory->mkdir("TrackParameters");
    gDirectory->cd("TrackParameters");
    {
      TString chi2Name = "Chi2Prim";
      for(int iPart=0; iPart < KFPartEfficiencies::nParticles; iPart++)
      {
        TString chi2NamePart = "Chi2Prim";
        chi2NamePart += "_";
        chi2NamePart += fParteff.partName[iPart].Data();
        hTrackParameters[iPart] = new TH1F(chi2NamePart.Data(), chi2NamePart.Data(), 1000, 0, 100);

      }
      hTrackParameters[KFPartEfficiencies::nParticles] = new TH1F("Chi2Prim", "Chi2Prim", 1000, 0, 100);
    }
    gDirectory->cd(".."); //particle directory


    curdir->cd();    

  }
  else{ // create not in file
    static int iaddName = 0; // haven't any subfolders so create with different names
    TString addName = TString(iaddName);
    int ih = 0; // i of Histo
    for( int i = 0; i < NTrackPulls + NTrackPullsAtProdVertex + NVertexPulls; i++, ih++, addName = TString(iaddName++) ){
      fHistos[ih] = new TH1D(fHistosInfo[ih].name+addName, fHistosInfo[ih].title, fHistosInfo[ih].nx, fHistosInfo[ih].left, fHistosInfo[ih].right);
    }
    
    for( int i = 0; i < NHisto; i++ ){
      fHistos[i]->SetDirectory(0);
    }
  }
  SetHistoCreated();
}


void PndFTSParticlePerformanceBase::FillHistos()
{

} // void PndFTSParticlePerformanceBase::FillHistos()
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE

