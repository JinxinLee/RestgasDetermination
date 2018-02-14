/******************************************************
Poorman Tracks and Fitter profiling
Author: Ralf Kliemt (GSI,2013)
*******************************************************/

#include "TMath.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "RhoCandidate.h"
#include "RhoCalculationTools.h"
#include "PndAnalysisCalcTools.h"
#include "RhoVtxPoca.h"
#include "RhoKalmanVtxFitter.h"
#include "RhoKinVtxFitter.h"

#include "PndPmtTask.h"
#include <string>
#include <iostream>


using std::cout;
using std::flush;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndPmtTask::PndPmtTask() : PndPmtPoormantracks()   // : FairTask("Poorman Track Task")
{
 fSwAll.Start();
 fHistoList = new TList();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndPmtTask::~PndPmtTask() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
//InitStatus
void PndPmtTask::Init() {

  gRandom->SetSeed(fSeed);
  RhoCalculationTools::ForceConstantBz(20.); // [kGs]=[0.1T]
  PndAnalysisCalcTools::SetVerbose(0);

  //PArameters
  fWidx=1.5,
  fWidy=1.5,
  fWidz=1.5; //[cm] maximum vertex position off origin

  int bins=100;
  double min=0,max=4.5,mdf=2.0; // GeV/c
  double min2=0.13,max2=0.15;
  double min3=1.,max3=1.5;
  double max4=0.3;//cm
  double minchi = -0.05;
  double maxchi = 2.5;
  double vrange = 0.05; // cm
  double pullrange =4.;
  double momrange=0.1;//GeV/c
  double angdiffran=0.05; //rad


  // NO FIT
  fHVtxDiffX=new TH1F("hvtxDiffx","Vertex Smearing;(x_{mc}-x)/cm",bins,-vrange,vrange);
  fHVtxDiffY=new TH1F("hvtxDiffy","Vertex Smearing;(y_{mc}-y)/cm",bins,-vrange,vrange);
  fHVtxDiffZ=new TH1F("hvtxDiffz","Vertex Smearing;(z_{mc}-z)/cm",bins,-vrange,vrange);

  fHVtxDiffPX=new TH1F("hvtxdiffpx","Momentum reco;(p_{x,mc}-p_{x} / GeV/c)",bins,-momrange,momrange);
  fHVtxDiffPY=new TH1F("hvtxdiffpy","Momentum reco;(p_{y,mc}-p_{y} / GeV/c)",bins,-momrange,momrange);
  fHVtxDiffPZ=new TH1F("hvtxdiffpz","Momentum reco;(p_{z,mc}-p_{z}) / GeV/c",bins,-momrange,momrange);
  fHVtxDiffE=new TH1F("hvtxdiffe","Energy reco;(E_{mc}-E) / GeV",bins,-momrange,momrange);

  fHVtxPullPX=new TH1F("hvtxpullpx","Momentum pull distribution reco;(p_{x,mc}-p_{x})/#sigma_{p_{x}}",bins,-pullrange,pullrange);
  fHVtxPullPY=new TH1F("hvtxpullpy","Momentum pull distribution reco;(p_{y,mc}-p_{y})/#sigma_{p_{y}}",bins,-pullrange,pullrange);
  fHVtxPullPZ=new TH1F("hvtxpullpz","Momentum pull distribution reco;(p_{z,mc}-p_{z})/#sigma_{p_{z}}",bins,-pullrange,pullrange);
  fHVtxPullE=new TH1F("hvtxpulle","Energy pull distribution reco;(E_{mc}-E)/#sigma_{E}",bins,-pullrange,pullrange);

  fHVtxDiffThe=new TH1F("hvtxdiffthe","Momentum reco;(#Theta_{mc}-#Theta}) / GeV/c",bins,-angdiffran,angdiffran);
  fHVtxDiffPhi=new TH1F("hvtxdiffphi","Momentum reco;(#Phi_{mc}-#Phi}) / GeV/c",bins,-angdiffran,angdiffran);

  // POCA

  fHVtxPocaX=new TH1F("hvtxPocax","Vertex POCA;(x_{mc}-x)/cm",bins,-vrange,vrange);
  fHVtxPocaY=new TH1F("hvtxPocay","Vertex POCA;(y_{mc}-y)/cm",bins,-vrange,vrange);
  fHVtxPocaZ=new TH1F("hvtxPocaz","Vertex POCA;(z_{mc}-z)/cm",bins,-vrange,vrange);

  fHVtxPocaXY=new TH2F("hvtxPocaxy","Vertex POCA X-Y view;(x_{mc}-x)/#mum;(y_{mc}-y)/cm",bins,-vrange,vrange,bins,-vrange,vrange);
  fHVtxPocaRZ=new TH2F("hvtxPocarz","Vertex POCA R-Z view;(z_{mc}-z)/#mum;(r_{mc}-r)/cm",bins,-vrange,vrange,bins,-vrange,vrange);

  fHVtxPullPocaX=new TH1F("hvtxpullpocax","Vertex pull distribution POCA;(x_{mc}-x)/#sigma_{x}",bins,-pullrange,pullrange);
  fHVtxPullPocaY=new TH1F("hvtxpullpocay","Vertex pull distribution POCA;(y_{mc}-y)/#sigma_{y}",bins,-pullrange,pullrange);
  fHVtxPullPocaZ=new TH1F("hvtxpullpocaz","Vertex pull distribution POCA;(z_{mc}-z)/#sigma_{z}",bins,-pullrange,pullrange);

  fHVtxPocas=new TH1F("hvtxpocas","Vertex POCA D value;D/cm",bins,0.,vrange);
  fHVtxPocaEmpty=new TH1F("hvtxcpocaempty","",0,0.,1.);

  // FAST FIT

  fHVtxFastX=new TH1F("hvtxFastx","Vertex fast fit;(x_{mc}-x)/cm",bins,-vrange,vrange);
  fHVtxFastY=new TH1F("hvtxTasty","Vertex fast fit;(y_{mc}-y)/cm",bins,-vrange,vrange);
  fHVtxFastZ=new TH1F("hvtxFastz","Vertex fast fit;(z_{mc}-z)/cm",bins,-vrange,vrange);

  fHVtxFastXY=new TH2F("hvtxfastxy","Vertex fast fit X-Y view;(x_{mc}-x)/#mum;(y_{mc}-y)/cm",bins,-vrange,vrange,bins,-vrange,vrange);
  fHVtxFastRZ=new TH2F("hvtxfastrz","Vertex fast fit R-Z view;(z_{mc}-z)/#mum;(r_{mc}-r)/cm",bins,-vrange,vrange,bins,-vrange,vrange);

  fHVtxErrFastX=new TH1F("hvtxerrfastx","Vertex error distribution fast;#sigma_{x}",bins,0.,vrange);
  fHVtxErrFastY=new TH1F("hvtxerrfasty","Vertex error distribution fast;#sigma_{y}",bins,0.,vrange);
  fHVtxErrFastZ=new TH1F("hvtxerrfastz","Vertex error distribution fast;#sigma_{z}",bins,0.,vrange);
  fHVtxPullFastX=new TH1F("hvtxpullfastx","Vertex pull distribution fast;(x_{mc}-x)/#sigma_{x}",bins,-pullrange,pullrange);
  fHVtxPullFastY=new TH1F("hvtxpullfasty","Vertex pull distribution fast;(y_{mc}-y)/#sigma_{y}",bins,-pullrange,pullrange);
  fHVtxPullFastZ=new TH1F("hvtxpullfastz","Vertex pull distribution fast;(z_{mc}-z)/#sigma_{z}",bins,-pullrange,pullrange);

  fHVtxChi2Fast=new TH1F("hvtxchi2fast","Vertex Fast #Chi^{2};#Chi^{2}",bins,minchi,maxchi);
  fHVtxChiProbFast=new TH1F("hvtxchipropfast","P(#Chi^{2}) Fast;p",bins,0.,1.);

  // FULL FIT
  fHVtxFitX=new TH1F("hvtxFitx","Vertex full fit;(x_{mc}-x)/cm",bins,-vrange,vrange);
  fHVtxFitY=new TH1F("hvtxFity","Vertex full fit;(y_{mc}-y)/cm",bins,-vrange,vrange);
  fHVtxFitZ=new TH1F("hvtxFitz","Vertex full fit;(z_{mc}-z)/cm",bins,-vrange,vrange);

  fHVtxFitXY=new TH2F("hvtxfitxy","Vertex full fit X-Y view;(x_{mc}-x)/#mum;(y_{mc}-y)/cm",bins,-vrange,vrange,bins,-vrange,vrange);
  fHVtxFitRZ=new TH2F("hvtxfitrz","Vertex full fit R-Z view;(z_{mc}-z)/#mum;(r_{mc}-r)/cm",bins,-vrange,vrange,bins,-vrange,vrange);

  fHVtxErrFitX=new TH1F("hvtxerrfitx","Vertex error distribution full fit;#sigma_{x}",bins,0.,vrange);
  fHVtxErrFitY=new TH1F("hvtxerrfity","Vertex error distribution full fit;#sigma_{y}",bins,0.,vrange);
  fHVtxErrFitZ=new TH1F("hvtxerrfitz","Vertex error distribution full fit;#sigma_{z}",bins,0.,vrange);
  fHVtxPullFitX=new TH1F("hvtxpullfitx","Vertex pull distribution full fit;(x_{mc}-x)/#sigma_{x}",bins,-pullrange,pullrange);
  fHVtxPullFitY=new TH1F("hvtxpullfity","Vertex pull distribution full fit;(y_{mc}-y)/#sigma_{y}",bins,-pullrange,pullrange);
  fHVtxPullFitZ=new TH1F("hvtxpullfitz","Vertex pull distribution full fit;(z_{mc}-z)/#sigma_{z}",bins,-pullrange,pullrange);
  fHVtxDiffFitPX=new TH1F("hvtxdifffitpx","Momentum full fit;(p_{x,mc}-p_{x} / GeV/c)",bins,-momrange,momrange);
  fHVtxDiffFitPY=new TH1F("hvtxdifffitpy","Momentum full fit;(p_{y,mc}-p_{y} / GeV/c)",bins,-momrange,momrange);
  fHVtxDiffFitPZ=new TH1F("hvtxdifffitpz","Momentum full fit;(p_{z,mc}-p_{z}) / GeV/c",bins,-momrange,momrange);
  fHVtxDiffFitE=new TH1F("hvtxdifffite","Energy full fit;(E_{mc}-E) / GeV",bins,-momrange,momrange);
  fHVtxDiffFitThe=new TH1F("hvtxdifffitthe","Momentum full fit;(#Theta_{mc}-#Theta}) / GeV/c",bins,-angdiffran,angdiffran);
  fHVtxDiffFitPhi=new TH1F("hvtxdifffitphi","Momentum full fit;(#Phi_{mc}-#Phi}) / GeV/c",bins,-angdiffran,angdiffran);

  fHVtxPullFitPX=new TH1F("hvtxpullfitpx","Momentum pull distribution full fit;(p_{x,mc}-p_{x})/#sigma_{p_{x}}",bins,-pullrange,pullrange);
  fHVtxPullFitPY=new TH1F("hvtxpullfitpy","Momentum pull distribution full fit;(p_{y,mc}-p_{y})/#sigma_{p_{y}}",bins,-pullrange,pullrange);
  fHVtxPullFitPZ=new TH1F("hvtxpullfitpz","Momentum pull distribution full fit;(p_{z,mc}-p_{z})/#sigma_{p_{z}}",bins,-pullrange,pullrange);
  fHVtxPullFitE=new TH1F("hvtxpullfite","Energy pull distribution full fit;(E_{mc}-E)/#sigma_{E}",bins,-pullrange,pullrange);

  fHVtxChi2Fit=new TH1F("hvtxchi2fit","Vertex Fit #Chi^{2};#Chi^{2}",bins,minchi,maxchi);
  fHVtxChiProbFit=new TH1F("hvtxchipropfit","P(#Chi^{2}) Fit;p",bins,0.,1.);

  // KIN Fit

  fHVtxKinX=new TH1F("hvtxKinx","Vertex full kin;(x_{mc}-x)/cm",bins,-vrange,vrange);
  fHVtxKinY=new TH1F("hvtxKiny","Vertex full kin;(y_{mc}-y)/cm",bins,-vrange,vrange);
  fHVtxKinZ=new TH1F("hvtxKinz","Vertex full kin;(z_{mc}-z)/cm",bins,-vrange,vrange);

  fHVtxKinXY=new TH2F("hvtxkinxy","Vertex full kin X-Y view;(x_{mc}-x)/#mum;(y_{mc}-y)/cm",bins,-vrange,vrange,bins,-vrange,vrange);
  fHVtxKinRZ=new TH2F("hvtxkinrz","Vertex full kin R-Z view;(z_{mc}-z)/#mum;(r_{mc}-r)/cm",bins,-vrange,vrange,bins,-vrange,vrange);

  fHVtxErrKinX=new TH1F("hvtxerrkinx","Vertex error distribution full kin;#sigma_{x}",bins,0.,vrange);
  fHVtxErrKinY=new TH1F("hvtxerrkiny","Vertex error distribution full kin;#sigma_{y}",bins,0.,vrange);
  fHVtxErrKinZ=new TH1F("hvtxerrkinz","Vertex error distribution full kin;#sigma_{z}",bins,0.,vrange);
  fHVtxPullKinX=new TH1F("hvtxpullkinx","Vertex pull distribution full kin;(x_{mc}-x)/#sigma_{x}",bins,-pullrange,pullrange);
  fHVtxPullKinY=new TH1F("hvtxpullkiny","Vertex pull distribution full kin;(y_{mc}-y)/#sigma_{y}",bins,-pullrange,pullrange);
  fHVtxPullKinZ=new TH1F("hvtxpullkinz","Vertex pull distribution full kin;(z_{mc}-z)/#sigma_{z}",bins,-pullrange,pullrange);
  fHVtxDiffKinPX=new TH1F("hvtxdiffkinpx","Momentum full kin;(p_{x,mc}-p_{x} / GeV/c)",bins,-momrange,momrange);
  fHVtxDiffKinPY=new TH1F("hvtxdiffkinpy","Momentum full kin;(p_{y,mc}-p_{y} / GeV/c)",bins,-momrange,momrange);
  fHVtxDiffKinPZ=new TH1F("hvtxdiffkinpz","Momentum full kin;(p_{z,mc}-p_{z}) / GeV/c",bins,-momrange,momrange);
  fHVtxDiffKinE=new TH1F("hvtxdiffkine","Energy full kin;(E_{mc}-E) / GeV",bins,-momrange,momrange);
  fHVtxDiffKinThe=new TH1F("hvtxdiffkinthe","Momentum full kin;(#Theta_{mc}-#Theta}) / GeV/c",bins,-angdiffran,angdiffran);
  fHVtxDiffKinPhi=new TH1F("hvtxdiffkinphi","Momentum full kin;(#Phi_{mc}-#Phi}) / GeV/c",bins,-angdiffran,angdiffran);

  fHVtxPullKinPX=new TH1F("hvtxpullkinpx","Momentum pull distribution full kin;(p_{x,mc}-p_{x})/#sigma_{p_{x}}",bins,-pullrange,pullrange);
  fHVtxPullKinPY=new TH1F("hvtxpullkinpy","Momentum pull distribution full kin;(p_{y,mc}-p_{y})/#sigma_{p_{y}}",bins,-pullrange,pullrange);
  fHVtxPullKinPZ=new TH1F("hvtxpullkinpz","Momentum pull distribution full kin;(p_{z,mc}-p_{z})/#sigma_{p_{z}}",bins,-pullrange,pullrange);
  fHVtxPullKinE=new TH1F("hvtxpullkine","Energy pull distribution full kin;(E_{mc}-E)/#sigma_{E}",bins,-pullrange,pullrange);

  fHVtxChi2Kin=new TH1F("hvtxchi2kin","Vertex Kin #Chi^{2};#Chi^{2}",bins,minchi,maxchi);
  fHVtxChiProbKin=new TH1F("hvtxchipropkin","P(#Chi^{2}) Kin;p",bins,0.,1.);

  // Helix Params
  fHPrgPull0 = new TH1F("hprgpull0","PRG Pull Distribution (0)",bins,-pullrange,pullrange);
  fHPrgPull1 = new TH1F("hprgpull1","PRG Pull Distribution (1)",bins,-pullrange,pullrange);
  fHPrgPull2 = new TH1F("hprgpull2","PRG Pull Distribution (2)",bins,-pullrange,pullrange);
  fHPrgPull3 = new TH1F("hprgpull3","PRG Pull Distribution (3)",bins,-pullrange,pullrange);
  fHPrgPull4 = new TH1F("hprgpull4","PRG Pull Distribution (4)",bins,-pullrange,pullrange);

  fHCpu = new TH1D("hcpu","CPU loads (ms)",5,0,5);
  fHCpu->GetXaxis()->SetBinLabel(1,"PMT");
  fHCpu->GetXaxis()->SetBinLabel(2,"POCA");
  fHCpu->GetXaxis()->SetBinLabel(3,"PRG Fast");
  fHCpu->GetXaxis()->SetBinLabel(4,"PRG Full");
  fHCpu->GetXaxis()->SetBinLabel(5,"KinVtx");

  fHistoList->Add(fHVtxDiffX);
  fHistoList->Add(fHVtxDiffY);
  fHistoList->Add(fHVtxDiffZ);

  fHistoList->Add(fHVtxPocaX);
  fHistoList->Add(fHVtxPocaY);
  fHistoList->Add(fHVtxPocaZ);

  fHistoList->Add(fHVtxFastX);
  fHistoList->Add(fHVtxFastY);
  fHistoList->Add(fHVtxFastZ);

  fHistoList->Add(fHVtxFitX);
  fHistoList->Add(fHVtxFitY);
  fHistoList->Add(fHVtxFitZ);

  fHistoList->Add(fHVtxKinX);
  fHistoList->Add(fHVtxKinY);
  fHistoList->Add(fHVtxKinZ);

  fHistoList->Add(fHVtxErrFastX);
  fHistoList->Add(fHVtxErrFastY);
  fHistoList->Add(fHVtxErrFastZ);

  fHistoList->Add(fHVtxErrFitX);
  fHistoList->Add(fHVtxErrFitY);
  fHistoList->Add(fHVtxErrFitZ);

  fHistoList->Add(fHVtxErrKinX);
  fHistoList->Add(fHVtxErrKinY);
  fHistoList->Add(fHVtxErrKinZ);

  fHistoList->Add(fHVtxPullPocaX);
  fHistoList->Add(fHVtxPullPocaY);
  fHistoList->Add(fHVtxPullPocaZ);

  fHistoList->Add(fHVtxPullFastX);
  fHistoList->Add(fHVtxPullFastY);
  fHistoList->Add(fHVtxPullFastZ);

  fHistoList->Add(fHVtxPullFitX);
  fHistoList->Add(fHVtxPullFitY);
  fHistoList->Add(fHVtxPullFitZ);

  fHistoList->Add(fHVtxPullKinX);
  fHistoList->Add(fHVtxPullKinY);
  fHistoList->Add(fHVtxPullKinZ);

  fHistoList->Add(fHVtxDiffPX);
  fHistoList->Add(fHVtxDiffPY);
  fHistoList->Add(fHVtxDiffPZ);

  fHistoList->Add(fHVtxPullPX);
  fHistoList->Add(fHVtxPullPY);
  fHistoList->Add(fHVtxPullPZ);

  fHistoList->Add(fHVtxDiffFitPX);
  fHistoList->Add(fHVtxDiffFitPY);
  fHistoList->Add(fHVtxDiffFitPZ);

  fHistoList->Add(fHVtxPullFitPX);
  fHistoList->Add(fHVtxPullFitPY);
  fHistoList->Add(fHVtxPullFitPZ);

  fHistoList->Add(fHVtxDiffKinPX);
  fHistoList->Add(fHVtxDiffKinPY);
  fHistoList->Add(fHVtxDiffKinPZ);

  fHistoList->Add(fHVtxPullKinPX);
  fHistoList->Add(fHVtxPullKinPY);
  fHistoList->Add(fHVtxPullKinPZ);

  fHistoList->Add(fHVtxDiffE);
  fHistoList->Add(fHVtxDiffFitE);
  fHistoList->Add(fHVtxDiffKinE);
  fHistoList->Add(fHVtxPullE);
  fHistoList->Add(fHVtxPullFitE);
  fHistoList->Add(fHVtxPullKinE);

  fHistoList->Add(fHVtxDiffThe);
  fHistoList->Add(fHVtxDiffPhi);

  fHistoList->Add(fHVtxDiffFitThe);
  fHistoList->Add(fHVtxDiffFitPhi);

  fHistoList->Add(fHVtxDiffKinThe);
  fHistoList->Add(fHVtxDiffKinPhi);

  fHistoList->Add(fHVtxPocas);
  fHistoList->Add(fHVtxChi2Fast);
  fHistoList->Add(fHVtxChi2Fit);
  fHistoList->Add(fHVtxChi2Kin);
  fHistoList->Add(fHVtxChiProbFast);
  fHistoList->Add(fHVtxChiProbFit);
  fHistoList->Add(fHVtxChiProbKin);

  fHistoList->Add(fHVtxPocaXY);
  fHistoList->Add(fHVtxFastXY);
  fHistoList->Add(fHVtxFitXY);
  fHistoList->Add(fHVtxKinXY);
  fHistoList->Add(fHVtxPocaRZ);
  fHistoList->Add(fHVtxFastRZ);
  fHistoList->Add(fHVtxFitRZ);
  fHistoList->Add(fHVtxKinRZ);

  fHistoList->Add(fHPrgPull0);
  fHistoList->Add(fHPrgPull1);
  fHistoList->Add(fHPrgPull2);
  fHistoList->Add(fHPrgPull3);
  fHistoList->Add(fHPrgPull4);

  fHistoList->Add(fHCpu);

  fSwPMT.Reset();
  fSwPoca.Reset();
  fSwPrgfast.Reset();
  fSwPrgfull.Reset();
  fSwKin.Reset();
}

void PndPmtTask::SetParContainers() {
  // Get run and runtime database
  //FairRunAna* run = FairRunAna::Instance();
  //if ( ! run ) Fatal("SetParContainers", "No analysis run");
  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndPmtTask::Exec(Option_t*) {


  RhoCandidate *aCand=0,*bCand=0,*cCand=0,*dCand=0;
  RhoCandidate *tmpcand=0,*mccand=0;
  TVector3 vertexMC,vertexPoc,vertexFast,vertexFit,vertexKin,vertexDiff,vertexDiffFast,vertexDiffFit,vertexDiffKin;
  TLorentzVector mom,mommc,momdiff;
  const TVector3 nullpunkt(0.,0.,0.);
  double rnd1=0.,rnd2=0.,rnd3=0.;
  RhoError amomcov(4);
  TMatrixD aposcov(3,3);
  TMatrixD helixCov(5,5);
  TMatrixD jacobian(5,7);
  Float_t mcparams[5];
  Float_t helixparams[5];
  TMatrixD covV(3,3);
  TMatrixD covVfit(3,3);

  //for(int i=0;i<nevt;i++)
  //{
    //    printf("Event %i of %i. (seed=%i)\n",i,nevt,ranseed);
    RhoCandidate *combiCand=0,*combiCand1=0,*combiCand2=0;
    vertexMC = RollVertexBox(fWidx,fWidy,fWidz);
    fVertex=vertexMC;

    fSwPMT.Start(kFALSE);
    PoorManTracks();
    fSwPMT.Stop();
    if (fCands->GetEntriesFast()<2) {
      cout<<"Warning: too few particles created."<<endl;
      return;
    }

    aCand=(RhoCandidate*)fCands->At(0);
    bCand=(RhoCandidate*)fCands->At(1);
    if(fCands->GetEntriesFast()==3)
    {
      cCand=(RhoCandidate*)fCands->At(2);
      combiCand = aCand->Combine(bCand,cCand);
      combiCand1 = aCand->Combine(bCand,cCand);
      combiCand2 = aCand->Combine(bCand,cCand);
    }else if(fCands->GetEntriesFast()==4)
    {
      cCand=(RhoCandidate*)fCands->At(2);
      dCand=(RhoCandidate*)fCands->At(3);
      combiCand = aCand->Combine(bCand,cCand,dCand);
      combiCand1 = aCand->Combine(bCand,cCand,dCand);
      combiCand2 = aCand->Combine(bCand,cCand,dCand);
    }else{
      combiCand = aCand->Combine(bCand);
      combiCand1 = aCand->Combine(bCand);
      combiCand2 = aCand->Combine(bCand);
    }

    if(fVerbose>1) cout<<"aCand, bCand, comboCand, aPos, bPos, combipos"<<endl;
    if(fVerbose>1) cout<<*aCand<<endl;
    if(fVerbose>1) cout<<*bCand<<endl;
    if(fVerbose>1) cout<<*combiCand<<endl;
    if(fVerbose>1) aCand->GetPosition().Print();
    if(fVerbose>1) bCand->GetPosition().Print();
    if(fVerbose>1) combiCand->GetPosition().Print();


    // CLEAN
    for(int k=0;k<combiCand->NDaughters();k++)
    {
      tmpcand = combiCand->Daughter(k);
      mccand = tmpcand->GetMcTruth();

      vertexDiff=mccand->Pos();vertexDiff-=tmpcand->Pos();
      fHVtxDiffX->Fill(vertexDiff.X());
      fHVtxDiffY->Fill(vertexDiff.Y());
      fHVtxDiffZ->Fill(vertexDiff.Z());

      mommc=mccand->P4();
      mom=tmpcand->P4();
      momdiff=mommc-mom;
      amomcov=tmpcand->P4Cov();
      fHVtxDiffPX->Fill(momdiff.X());
      fHVtxDiffPY->Fill(momdiff.Y());
      fHVtxDiffPZ->Fill(momdiff.Z());
      fHVtxDiffE->Fill(momdiff.E());

      fHVtxPullPX->Fill(momdiff.X()/sqrt(amomcov[0][0]));
      fHVtxPullPY->Fill(momdiff.Y()/sqrt(amomcov[1][1]));
      fHVtxPullPZ->Fill(momdiff.Z()/sqrt(amomcov[2][2]));
      fHVtxPullE->Fill(momdiff.E()/sqrt(amomcov[3][3]));

      fHVtxDiffThe->Fill(mommc.Theta()-mom.Theta());
      fHVtxDiffPhi->Fill(mommc.Phi()-mom.Phi());

    }

    // ********** POCA
    RhoVtxPoca vPoca;
    vertexPoc=vertexMC; //seed
    if(fVerbose>0) cout<<"poca "<<flush;
    fSwPoca.Start(kFALSE);
    double dist = vPoca.GetPocaVtx(vertexPoc,combiCand);
    fSwPoca.Stop();
    if(fVerbose>0) cout<<" - Dist = "<<dist<<"  \t";if(fVerbose>1)cout<<endl;
    vertexDiff=vertexMC - vertexPoc;
    if(fVerbose>1) vertexMC.Print();
    if(fVerbose>1) vertexPoc.Print();
    if(fVerbose>1) vertexDiff.Print();

    fHVtxPocaX->Fill(vertexDiff.X());
    fHVtxPocaY->Fill(vertexDiff.Y());
    fHVtxPocaZ->Fill(vertexDiff.Z());
    fHVtxPocaXY->Fill(vertexDiff.X(),vertexDiff.Y());
    fHVtxPocaRZ->Fill(vertexDiff.Z(),vertexMC.Perp()-vertexPoc.Perp());
    fHVtxPocas->Fill(dist);
    fHVtxPullPocaX->Fill(vertexDiff.X()/dist);
    fHVtxPullPocaY->Fill(vertexDiff.Y()/dist);
    fHVtxPullPocaZ->Fill(vertexDiff.Z()/dist);

    // ********** FAST FIT
    RhoKalmanVtxFitter vFastter(combiCand);
    vFastter.SetSilent();
    //vFastter.SetDebug();
    //PndAnalysisCalcTools::SetVerbose(3);
    if(fVerbose>0) cout<<"prg "<<flush;
    vertexFast=nullpunkt; //seed
    double chiq = -5454;
    vFastter.SetExpansionPoint(vertexFast);
    fSwPrgfast.Start(kFALSE);
    chiq = vFastter.FitVertexFast(vertexFast,covV,false,2); // 2 iterations, don't skip cov
    fSwPrgfast.Stop();
    if (chiq>=0){
      if(fVerbose>0) cout<<" - chiq = "<<chiq<<"  \t";if(fVerbose>1)cout<<endl;

      fHVtxChi2Fast->Fill(vFastter.GetChi2());
      fHVtxChiProbFast->Fill(vFastter.GetProb());

      vertexDiffFast=vertexMC - vertexFast;
      if(fVerbose>1) vertexMC.Print();
      if(fVerbose>1) vertexFast.Print();
      if(fVerbose>0) vertexDiffFast.Print();

      fHVtxFastX->Fill(vertexDiffFast.X());
      fHVtxFastY->Fill(vertexDiffFast.Y());
      fHVtxFastZ->Fill(vertexDiffFast.Z());
      fHVtxFastXY->Fill(vertexDiffFast.X(),vertexDiffFast.Y());
      fHVtxFastRZ->Fill(vertexDiffFast.Z(),vertexMC.Perp()-vertexFast.Perp());

      fHVtxErrFastX->Fill(sqrt(covV[0][0]));
      fHVtxErrFastY->Fill(sqrt(covV[1][1]));
      fHVtxErrFastZ->Fill(sqrt(covV[2][2]));
      fHVtxPullFastX->Fill(vertexDiffFast.X()/sqrt(covV[0][0]));
      fHVtxPullFastY->Fill(vertexDiffFast.Y()/sqrt(covV[1][1]));
      fHVtxPullFastZ->Fill(vertexDiffFast.Z()/sqrt(covV[2][2]));
    }
    //vFastter.SetSilent();
    //PndAnalysisCalcTools::SetVerbose(0);

    // ********** FULL FIT
    RhoKalmanVtxFitter vFitter(combiCand1);
    vFitter.SetSilent();
    //PndAnalysisCalcTools::SetVerbose(5);
    //if(laut)vFitter.SetDebug(true);
    vertexFit=nullpunkt; //seed
    vFitter.SetNIterations(2);
    if(fVerbose>0)  cout<<"prg  "<<flush;
    //if(laut>0)  PndAnalysisCalcTools::SetVerbose(3);
    fSwPrgfull.Start(kFALSE);
    bool stat = vFitter.Fit();
    fSwPrgfull.Stop();
    //if(laut>0)  PndAnalysisCalcTools::SetVerbose(0);
    //if(fVerbose>0)  cout<<" - chiq = "<<chiqfit<<"  \t"<<flush;if(fVerbose>1)cout<<endl;
    if (stat)
    {
      //PndAnalysisCalcTools::SetVerbose(0);
      //      fHVtxChi2Fit->Fill(chiqfit/fNDF);
      //      fHVtxChiProbFit->Fill(TMath::Prob(chiqfit,fNDF));
      fHVtxChi2Fit->Fill(vFitter.GetChi2());
      fHVtxChiProbFit->Fill(vFitter.GetProb());

      vertexFit=combiCand1->GetFit()->DecayVtx();
      covVfit=combiCand1->GetFit()->DecayVtx().CovMatrix();
      vertexDiffFit=vertexMC - vertexFit;
      if(fVerbose>1) vertexMC.Print();
      if(fVerbose>1) vertexFit.Print();
      if(fVerbose>1) vertexDiffFit.Print();

      fHVtxFitX->Fill(vertexDiffFit.X());
      fHVtxFitY->Fill(vertexDiffFit.Y());
      fHVtxFitZ->Fill(vertexDiffFit.Z());
      fHVtxFitXY->Fill(vertexDiffFit.X(),vertexDiffFit.Y());
      fHVtxFitRZ->Fill(vertexDiffFit.Z(),vertexMC.Perp()-vertexFit.Perp());

      fHVtxErrFitX->Fill(sqrt(covVfit[0][0]));
      fHVtxErrFitY->Fill(sqrt(covVfit[1][1]));
      fHVtxErrFitZ->Fill(sqrt(covVfit[2][2]));
      fHVtxPullFitX->Fill(vertexDiffFit.X()/sqrt(covVfit[0][0]));
      fHVtxPullFitY->Fill(vertexDiffFit.Y()/sqrt(covVfit[1][1]));
      fHVtxPullFitZ->Fill(vertexDiffFit.Z()/sqrt(covVfit[2][2]));

      for(int k=0;k<combiCand1->NDaughters();k++)
      {
        tmpcand = combiCand1->Daughter(k);
        mccand = tmpcand->GetMcTruth();
        tmpcand = tmpcand->GetFit();
        mommc=mccand->P4();
        mom=tmpcand->P4();
        momdiff=mommc-mom;
        amomcov=tmpcand->P4Cov();
        fHVtxDiffFitPX->Fill(momdiff.X());
        fHVtxDiffFitPY->Fill(momdiff.Y());
        fHVtxDiffFitPZ->Fill(momdiff.Z());
        fHVtxDiffFitE->Fill(momdiff.E());

        fHVtxPullFitPX->Fill(momdiff.X()/sqrt(amomcov[0][0]));
        fHVtxPullFitPY->Fill(momdiff.Y()/sqrt(amomcov[1][1]));
        fHVtxPullFitPZ->Fill(momdiff.Z()/sqrt(amomcov[2][2]));
        fHVtxPullFitE->Fill(momdiff.E()/sqrt(amomcov[3][3]));

        fHVtxDiffFitThe->Fill(mommc.Theta()-mom.Theta());
        fHVtxDiffFitPhi->Fill(mommc.Phi()-mom.Phi());
            // Now check the PRG description....
        Bool_t prgcheckmc = PndAnalysisCalcTools::P7toPRG(mccand->Pos(),mccand->P4(),mccand->Charge(),mccand->Cov7(),nullpunkt, mcparams, helixCov, jacobian, kFALSE);
        Bool_t prgchecktc = PndAnalysisCalcTools::P7toPRG(tmpcand->Pos(),tmpcand->P4(),tmpcand->Charge(),tmpcand->Cov7(),nullpunkt, helixparams, helixCov, jacobian, kFALSE);
        fHPrgPull0->Fill((mcparams[0]-helixparams[0])/sqrt(helixCov[0][0]));
        fHPrgPull1->Fill((mcparams[1]-helixparams[1])/sqrt(helixCov[1][1]));
        fHPrgPull2->Fill((mcparams[2]-helixparams[2])/sqrt(helixCov[2][2]));
        fHPrgPull3->Fill((mcparams[3]-helixparams[3])/sqrt(helixCov[3][3]));
        fHPrgPull4->Fill((mcparams[4]-helixparams[4])/sqrt(helixCov[4][4]));
      }
    }

    // ********** Kin FIT
    RhoKinVtxFitter kFitter(combiCand2);
    kFitter.SetNIterationsExact(2);
    vertexFit=nullpunkt; //seed
    if(fVerbose>0)  cout<<"kin  "<<flush;
    fSwKin.Start(kFALSE);
    kFitter.Fit();
    fSwKin.Stop();
    bool ksta = kFitter.GetChi2();
    //if(laut>0)  PndAnalysisCalcTools::SetVerbose(0);
    //if(fVerbose>0)  cout<<" - chiq = "<<chiqfit<<"  \t"<<flush;if(fVerbose>1)cout<<endl;
    if (ksta)
    {
      //PndAnalysisCalcTools::SetVerbose(0);
      vertexKin=combiCand2->GetFit()->DecayVtx();
      aposcov=combiCand2->GetFit()->DecayVtx().CovMatrix();
      fHVtxChi2Kin->Fill(kFitter.GetChi2());
      fHVtxChiProbKin->Fill(kFitter.GetProb());

      vertexDiffKin=vertexMC - vertexKin;
      if(fVerbose>1) vertexMC.Print();
      if(fVerbose>1) vertexKin.Print();
      if(fVerbose>0) vertexDiffKin.Print();

      fHVtxKinX->Fill(vertexDiffKin.X());
      fHVtxKinY->Fill(vertexDiffKin.Y());
      fHVtxKinZ->Fill(vertexDiffKin.Z());
      fHVtxKinXY->Fill(vertexDiffKin.X(),vertexDiffKin.Y());
      fHVtxKinRZ->Fill(vertexDiffKin.Z(),vertexMC.Perp()-vertexKin.Perp());

      fHVtxErrKinX->Fill(sqrt(aposcov[0][0]));
      fHVtxErrKinY->Fill(sqrt(aposcov[1][1]));
      fHVtxErrKinZ->Fill(sqrt(aposcov[2][2]));
      fHVtxPullKinX->Fill(vertexDiffKin.X()/sqrt(aposcov[0][0]));
      fHVtxPullKinY->Fill(vertexDiffKin.Y()/sqrt(aposcov[1][1]));
      fHVtxPullKinZ->Fill(vertexDiffKin.Z()/sqrt(aposcov[2][2]));

      for(int k=0;k<combiCand2->NDaughters();k++)
      {
        tmpcand = combiCand2->Daughter(k);
        mccand = tmpcand->GetMcTruth();
        tmpcand = tmpcand->GetFit();
        mommc=mccand->P4();
        mom=tmpcand->P4();
        momdiff=mommc-mom;
        amomcov=tmpcand->P4Cov();
        fHVtxDiffKinPX->Fill(momdiff.X());
        fHVtxDiffKinPY->Fill(momdiff.Y());
        fHVtxDiffKinPZ->Fill(momdiff.Z());
        fHVtxDiffKinE->Fill(momdiff.E());

        fHVtxPullKinPX->Fill(momdiff.X()/sqrt(amomcov[0][0]));
        fHVtxPullKinPY->Fill(momdiff.Y()/sqrt(amomcov[1][1]));
        fHVtxPullKinPZ->Fill(momdiff.Z()/sqrt(amomcov[2][2]));
        fHVtxPullKinE->Fill(momdiff.E()/sqrt(amomcov[3][3]));

        fHVtxDiffKinThe->Fill(mommc.Theta()-mom.Theta());
        fHVtxDiffKinPhi->Fill(mommc.Phi()-mom.Phi());
      }
    }
    // } // event loop

    //cout<<" ---------- calculations done, writing histograms."<<endl;
    //manipulate & fit histos now


}
// -------------------------------------------------------------------------

void PndPmtTask::Finish()
{
  fSwAll.Stop();
  //Read Stopwatches
  //fHCpu->Fill("PMT",fSwPMT.RealTime()/fSwAll.RealTime());
  //fHCpu->Fill("POCA",fSwPoca.RealTime()/fSwAll.RealTime());
  //fHCpu->Fill("PRG Fast",fSwPrgfast.RealTime()/fSwAll.RealTime());
  //fHCpu->Fill("PRG Full",fSwPrgfull.RealTime()/fSwAll.RealTime());
  //fHCpu->Fill("KinVtx",fSwKin.RealTime()/fSwAll.RealTime());
  fHCpu->Fill("PMT",1000*fSwPMT.RealTime()/fSwPMT.Counter());
  fHCpu->Fill("POCA",1000*fSwPoca.RealTime()/fSwPMT.Counter());
  fHCpu->Fill("PRG Fast",1000*fSwPrgfast.RealTime()/fSwPMT.Counter());
  fHCpu->Fill("PRG Full",1000*fSwPrgfull.RealTime()/fSwPMT.Counter());
  fHCpu->Fill("KinVtx",1000*fSwKin.RealTime()/fSwPMT.Counter());


  //Write and Plot all histos from List
  TString hfile="Data/HistoVertexing";
  //  if(seed>=0){hfile+="-";hfile+=ranseed;}
  hfile+=".root";
  TFile* histofile = new TFile(hfile.Data(),"RECREATE");
  histofile->cd();
  TListIter iter(fHistoList);
  TH1* tmph=NULL;
  while( tmph=(TH1*)iter() ) tmph->Write();
  histofile->Close();
}



ClassImp(PndPmtTask)
