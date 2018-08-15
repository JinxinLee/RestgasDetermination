//
//  poormantracks.C
//  PANDAROOT
//
//  Author Ralf Kliemt.
//  Copyright (c) 2013 GSI. All rights reserved.
//
//
double fSigVx=0.005; //[cm]
double fSigVy=0.005; //[cm]
double fSigVz=0.005; //[cm]
double fSigPx=0.01; //[GeV/c]
double fSigPy=0.01; //[GeV/c]
double fSigPz=0.01; //[GeV/c]
double fDtheta = 1.; //0.3 [pi]
int    fPID=211;
bool   fAlternate=true;
TParticlePDG* fPDG = 0;
int    fNumTrk=6;
int    niter=1;
int fSeedPrefix=936650;//
TVector3 fVertex(0.,0.,0.);

int fVerbose=0;

TClonesArray* fMcCands=0;
TClonesArray* fCands=0;
//#include "../../run/Tools.C"
int poormantracks(int nevt=250, int laut=0, int seed=-1)
{
  //gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  //gSystem->Load("libRho");
  //gSystem->Load("libAnalysisTools");
  fVerbose=laut;
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/Tools.C");
  LoadPandaStyle();
  int ranseed=fSeedPrefix+seed;
  gRandom->SetSeed(ranseed);
  RhoCalculationTools::ForceConstantBz(20.); // [kGs]=[0.1T]
  PndAnalysisCalcTools::SetVerbose(0);
  fMcCands = new TClonesArray("RhoCandidate");
  fCands = new TClonesArray("RhoCandidate");

  //PArameters
  double  widx=1.5,
  widy=1.5,
  widz=1.5; //[cm] maximum vertex position off origin

  int bins=100;
  double min=0,max=4.5,mdf=2.0; // GeV/c
  double min2=0.13,max2=0.15;
  double min3=1.,max3=1.5;
  double max4=0.3;//cm
  double maxchi = 5;
  double vrange = 0.05; // cm
  double pullrange =4.;
  double momrange=0.1;//GeV/c
  double angdiffran=0.05; //rad

  TList* histolist = new TList();

  // NO FIT
  TH1F *hVtxDiffX=new TH1F("hvtxDiffx","Vertex Smearing;(x_{mc}-x)/cm",bins,-vrange,vrange);
  TH1F *hVtxDiffY=new TH1F("hvtxDiffy","Vertex Smearing;(y_{mc}-y)/cm",bins,-vrange,vrange);
  TH1F *hVtxDiffZ=new TH1F("hvtxDiffz","Vertex Smearing;(z_{mc}-z)/cm",bins,-vrange,vrange);

  TH1F *hVtxDiffPX=new TH1F("hvtxdiffpx","Momentum reco;(p_{x,mc}-p_{x} / GeV/c)",bins,-momrange,momrange);
  TH1F *hVtxDiffPY=new TH1F("hvtxdiffpy","Momentum reco;(p_{y,mc}-p_{y} / GeV/c)",bins,-momrange,momrange);
  TH1F *hVtxDiffPZ=new TH1F("hvtxdiffpz","Momentum reco;(p_{z,mc}-p_{z}) / GeV/c",bins,-momrange,momrange);
  TH1F *hVtxDiffE=new TH1F("hvtxdiffe","Energy reco;(E_{mc}-E) / GeV",bins,-momrange,momrange);

  TH1F *hVtxPullPX=new TH1F("hvtxpullpx","Momentum pull distribution reco;(p_{x,mc}-p_{x})/#sigma_{p_{x}}",bins,-pullrange,pullrange);
  TH1F *hVtxPullPY=new TH1F("hvtxpullpy","Momentum pull distribution reco;(p_{y,mc}-p_{y})/#sigma_{p_{y}}",bins,-pullrange,pullrange);
  TH1F *hVtxPullPZ=new TH1F("hvtxpullpz","Momentum pull distribution reco;(p_{z,mc}-p_{z})/#sigma_{p_{z}}",bins,-pullrange,pullrange);
  TH1F *hVtxPullE=new TH1F("hvtxpulle","Energy pull distribution reco;(E_{mc}-E)/#sigma_{E}",bins,-pullrange,pullrange);

  TH1F *hVtxDiffThe=new TH1F("hvtxdiffthe","Momentum reco;(#Theta_{mc}-#Theta}) / GeV/c",bins,-angdiffran,angdiffran);
  TH1F *hVtxDiffPhi=new TH1F("hvtxdiffphi","Momentum reco;(#Phi_{mc}-#Phi}) / GeV/c",bins,-angdiffran,angdiffran);

  // POCA

  TH1F *hVtxPocaX=new TH1F("hvtxPocax","Vertex POCA;(x_{mc}-x)/cm",bins,-vrange,vrange);
  TH1F *hVtxPocaY=new TH1F("hvtxPocay","Vertex POCA;(y_{mc}-y)/cm",bins,-vrange,vrange);
  TH1F *hVtxPocaZ=new TH1F("hvtxPocaz","Vertex POCA;(z_{mc}-z)/cm",bins,-vrange,vrange);

  TH2F *hVtxPocaXY=new TH2F("hvtxPocaxy","Vertex POCA X-Y view;(x_{mc}-x)/#mum;(y_{mc}-y)/cm",bins,-vrange,vrange,bins,-vrange,vrange);
  TH2F *hVtxPocaRZ=new TH2F("hvtxPocarz","Vertex POCA R-Z view;(z_{mc}-z)/#mum;(r_{mc}-r)/cm",bins,-vrange,vrange,bins,-vrange,vrange);

  TH1F *hVtxPullPocaX=new TH1F("hvtxpullpocax","Vertex pull distribution POCA;(x_{mc}-x)/#sigma_{x}",bins,-pullrange,pullrange);
  TH1F *hVtxPullPocaY=new TH1F("hvtxpullpocay","Vertex pull distribution POCA;(y_{mc}-y)/#sigma_{y}",bins,-pullrange,pullrange);
  TH1F *hVtxPullPocaZ=new TH1F("hvtxpullpocaz","Vertex pull distribution POCA;(z_{mc}-z)/#sigma_{z}",bins,-pullrange,pullrange);

  TH1F *hVtxPocas=new TH1F("hvtxpocas","Vertex POCA D value;D/cm",bins,0.,vrange);
  TH1F *hVtxPocaEmpty=new TH1F("hvtxcpocaempty","",0,0.,1.);

  // FAST FIT

  TH1F *hVtxFastX=new TH1F("hvtxFastx","Vertex fast fit;(x_{mc}-x)/cm",bins,-vrange,vrange);
  TH1F *hVtxFastY=new TH1F("hvtxTasty","Vertex fast fit;(y_{mc}-y)/cm",bins,-vrange,vrange);
  TH1F *hVtxFastZ=new TH1F("hvtxFastz","Vertex fast fit;(z_{mc}-z)/cm",bins,-vrange,vrange);

  TH2F *hVtxFastXY=new TH2F("hvtxfastxy","Vertex fast fit X-Y view;(x_{mc}-x)/#mum;(y_{mc}-y)/cm",bins,-vrange,vrange,bins,-vrange,vrange);
  TH2F *hVtxFastRZ=new TH2F("hvtxfastrz","Vertex fast fit R-Z view;(z_{mc}-z)/#mum;(r_{mc}-r)/cm",bins,-vrange,vrange,bins,-vrange,vrange);

  TH1F *hVtxErrFastX=new TH1F("hvtxerrfastx","Vertex error distribution fast;#sigma_{x}",bins,0.,vrange);
  TH1F *hVtxErrFastY=new TH1F("hvtxerrfasty","Vertex error distribution fast;#sigma_{y}",bins,0.,vrange);
  TH1F *hVtxErrFastZ=new TH1F("hvtxerrfastz","Vertex error distribution fast;#sigma_{z}",bins,0.,vrange);
  TH1F *hVtxPullFastX=new TH1F("hvtxpullfastx","Vertex pull distribution fast;(x_{mc}-x)/#sigma_{x}",bins,-pullrange,pullrange);
  TH1F *hVtxPullFastY=new TH1F("hvtxpullfasty","Vertex pull distribution fast;(y_{mc}-y)/#sigma_{y}",bins,-pullrange,pullrange);
  TH1F *hVtxPullFastZ=new TH1F("hvtxpullfastz","Vertex pull distribution fast;(z_{mc}-z)/#sigma_{z}",bins,-pullrange,pullrange);

  TH1F *hVtxChi2Fast=new TH1F("hvtxchi2fast","Vertex Fast #Chi^{2};#Chi^{2}",bins,0,maxchi);
  TH1F *hVtxChiProbFast=new TH1F("hvtxchipropfast","P(#Chi^{2}) Fast;p",bins,0.,1.);

  // FULL FIT
  TH1F *hVtxFitX=new TH1F("hvtxFitx","Vertex full fit;(x_{mc}-x)/cm",bins,-vrange,vrange);
  TH1F *hVtxFitY=new TH1F("hvtxFity","Vertex full fit;(y_{mc}-y)/cm",bins,-vrange,vrange);
  TH1F *hVtxFitZ=new TH1F("hvtxFitz","Vertex full fit;(z_{mc}-z)/cm",bins,-vrange,vrange);

  TH2F *hVtxFitXY=new TH2F("hvtxfitxy","Vertex full fit X-Y view;(x_{mc}-x)/#mum;(y_{mc}-y)/cm",bins,-vrange,vrange,bins,-vrange,vrange);
  TH2F *hVtxFitRZ=new TH2F("hvtxfitrz","Vertex full fit R-Z view;(z_{mc}-z)/#mum;(r_{mc}-r)/cm",bins,-vrange,vrange,bins,-vrange,vrange);

  TH1F *hVtxErrFitX=new TH1F("hvtxerrfitx","Vertex error distribution full fit;#sigma_{x}",bins,0.,vrange);
  TH1F *hVtxErrFitY=new TH1F("hvtxerrfity","Vertex error distribution full fit;#sigma_{y}",bins,0.,vrange);
  TH1F *hVtxErrFitZ=new TH1F("hvtxerrfitz","Vertex error distribution full fit;#sigma_{z}",bins,0.,vrange);
  TH1F *hVtxPullFitX=new TH1F("hvtxpullfitx","Vertex pull distribution full fit;(x_{mc}-x)/#sigma_{x}",bins,-pullrange,pullrange);
  TH1F *hVtxPullFitY=new TH1F("hvtxpullfity","Vertex pull distribution full fit;(y_{mc}-y)/#sigma_{y}",bins,-pullrange,pullrange);
  TH1F *hVtxPullFitZ=new TH1F("hvtxpullfitz","Vertex pull distribution full fit;(z_{mc}-z)/#sigma_{z}",bins,-pullrange,pullrange);
  TH1F *hVtxDiffFitPX=new TH1F("hvtxdifffitpx","Momentum full fit;(p_{x,mc}-p_{x} / GeV/c)",bins,-momrange,momrange);
  TH1F *hVtxDiffFitPY=new TH1F("hvtxdifffitpy","Momentum full fit;(p_{y,mc}-p_{y} / GeV/c)",bins,-momrange,momrange);
  TH1F *hVtxDiffFitPZ=new TH1F("hvtxdifffitpz","Momentum full fit;(p_{z,mc}-p_{z}) / GeV/c",bins,-momrange,momrange);
  TH1F *hVtxDiffFitE=new TH1F("hvtxdifffite","Energy full fit;(E_{mc}-E) / GeV",bins,-momrange,momrange);
  TH1F *hVtxDiffFitThe=new TH1F("hvtxdifffitthe","Momentum full fit;(#Theta_{mc}-#Theta}) / GeV/c",bins,-angdiffran,angdiffran);
  TH1F *hVtxDiffFitPhi=new TH1F("hvtxdifffitphi","Momentum full fit;(#Phi_{mc}-#Phi}) / GeV/c",bins,-angdiffran,angdiffran);

  TH1F *hVtxPullFitPX=new TH1F("hvtxpullfitpx","Momentum pull distribution full fit;(p_{x,mc}-p_{x})/#sigma_{p_{x}}",bins,-pullrange,pullrange);
  TH1F *hVtxPullFitPY=new TH1F("hvtxpullfitpy","Momentum pull distribution full fit;(p_{y,mc}-p_{y})/#sigma_{p_{y}}",bins,-pullrange,pullrange);
  TH1F *hVtxPullFitPZ=new TH1F("hvtxpullfitpz","Momentum pull distribution full fit;(p_{z,mc}-p_{z})/#sigma_{p_{z}}",bins,-pullrange,pullrange);
  TH1F *hVtxPullFitE=new TH1F("hvtxpullfite","Energy pull distribution full fit;(E_{mc}-E)/#sigma_{E}",bins,-pullrange,pullrange);

  TH1F *hVtxChi2Fit=new TH1F("hvtxchi2fit","Vertex Fit #Chi^{2};#Chi^{2}",bins,0,maxchi);
  TH1F *hVtxChiProbFit=new TH1F("hvtxchipropfit","P(#Chi^{2}) Fit;p",bins,0.,1.);


  // KIN Fit

  TH1F *hVtxKinX=new TH1F("hvtxKinx","Vertex full kin;(x_{mc}-x)/cm",bins,-vrange,vrange);
  TH1F *hVtxKinY=new TH1F("hvtxKiny","Vertex full kin;(y_{mc}-y)/cm",bins,-vrange,vrange);
  TH1F *hVtxKinZ=new TH1F("hvtxKinz","Vertex full kin;(z_{mc}-z)/cm",bins,-vrange,vrange);

  TH2F *hVtxKinXY=new TH2F("hvtxkinxy","Vertex full kin X-Y view;(x_{mc}-x)/#mum;(y_{mc}-y)/cm",bins,-vrange,vrange,bins,-vrange,vrange);
  TH2F *hVtxKinRZ=new TH2F("hvtxkinrz","Vertex full kin R-Z view;(z_{mc}-z)/#mum;(r_{mc}-r)/cm",bins,-vrange,vrange,bins,-vrange,vrange);

  TH1F *hVtxErrKinX=new TH1F("hvtxerrkinx","Vertex error distribution full kin;#sigma_{x}",bins,0.,vrange);
  TH1F *hVtxErrKinY=new TH1F("hvtxerrkiny","Vertex error distribution full kin;#sigma_{y}",bins,0.,vrange);
  TH1F *hVtxErrKinZ=new TH1F("hvtxerrkinz","Vertex error distribution full kin;#sigma_{z}",bins,0.,vrange);
  TH1F *hVtxPullKinX=new TH1F("hvtxpullkinx","Vertex pull distribution full kin;(x_{mc}-x)/#sigma_{x}",bins,-pullrange,pullrange);
  TH1F *hVtxPullKinY=new TH1F("hvtxpullkiny","Vertex pull distribution full kin;(y_{mc}-y)/#sigma_{y}",bins,-pullrange,pullrange);
  TH1F *hVtxPullKinZ=new TH1F("hvtxpullkinz","Vertex pull distribution full kin;(z_{mc}-z)/#sigma_{z}",bins,-pullrange,pullrange);
  TH1F *hVtxDiffKinPX=new TH1F("hvtxdiffkinpx","Momentum full kin;(p_{x,mc}-p_{x} / GeV/c)",bins,-momrange,momrange);
  TH1F *hVtxDiffKinPY=new TH1F("hvtxdiffkinpy","Momentum full kin;(p_{y,mc}-p_{y} / GeV/c)",bins,-momrange,momrange);
  TH1F *hVtxDiffKinPZ=new TH1F("hvtxdiffkinpz","Momentum full kin;(p_{z,mc}-p_{z}) / GeV/c",bins,-momrange,momrange);
  TH1F *hVtxDiffKinE=new TH1F("hvtxdiffkine","Energy full kin;(E_{mc}-E) / GeV",bins,-momrange,momrange);
  TH1F *hVtxDiffKinThe=new TH1F("hvtxdiffkinthe","Momentum full kin;(#Theta_{mc}-#Theta}) / GeV/c",bins,-angdiffran,angdiffran);
  TH1F *hVtxDiffKinPhi=new TH1F("hvtxdiffkinphi","Momentum full kin;(#Phi_{mc}-#Phi}) / GeV/c",bins,-angdiffran,angdiffran);

  TH1F *hVtxPullKinPX=new TH1F("hvtxpullkinpx","Momentum pull distribution full kin;(p_{x,mc}-p_{x})/#sigma_{p_{x}}",bins,-pullrange,pullrange);
  TH1F *hVtxPullKinPY=new TH1F("hvtxpullkinpy","Momentum pull distribution full kin;(p_{y,mc}-p_{y})/#sigma_{p_{y}}",bins,-pullrange,pullrange);
  TH1F *hVtxPullKinPZ=new TH1F("hvtxpullkinpz","Momentum pull distribution full kin;(p_{z,mc}-p_{z})/#sigma_{p_{z}}",bins,-pullrange,pullrange);
  TH1F *hVtxPullKinE=new TH1F("hvtxpullkine","Energy pull distribution full kin;(E_{mc}-E)/#sigma_{E}",bins,-pullrange,pullrange);

  TH1F *hVtxChi2Kin=new TH1F("hvtxchi2kin","Vertex Kin #Chi^{2};#Chi^{2}",bins,0,maxchi);
  TH1F *hVtxChiProbKin=new TH1F("hvtxchipropkin","P(#Chi^{2}) Kin;p",bins,0.,1.);

  // Helix Params
  TH1F *hPrgPull0 = new TH1F("hprgpull0","PRG Pull Distribution (0)",bins,-pullrange,pullrange);
  TH1F *hPrgPull1 = new TH1F("hprgpull1","PRG Pull Distribution (1)",bins,-pullrange,pullrange);
  TH1F *hPrgPull2 = new TH1F("hprgpull2","PRG Pull Distribution (2)",bins,-pullrange,pullrange);
  TH1F *hPrgPull3 = new TH1F("hprgpull3","PRG Pull Distribution (3)",bins,-pullrange,pullrange);
  TH1F *hPrgPull4 = new TH1F("hprgpull4","PRG Pull Distribution (4)",bins,-pullrange,pullrange);

  histolist->Add(hVtxDiffX);
  histolist->Add(hVtxDiffY);
  histolist->Add(hVtxDiffZ);

  histolist->Add(hVtxPocaX);
  histolist->Add(hVtxPocaY);
  histolist->Add(hVtxPocaZ);

  histolist->Add(hVtxFastX);
  histolist->Add(hVtxFastY);
  histolist->Add(hVtxFastZ);

  histolist->Add(hVtxFitX);
  histolist->Add(hVtxFitY);
  histolist->Add(hVtxFitZ);

  histolist->Add(hVtxKinX);
  histolist->Add(hVtxKinY);
  histolist->Add(hVtxKinZ);

  histolist->Add(hVtxErrFastX);
  histolist->Add(hVtxErrFastY);
  histolist->Add(hVtxErrFastZ);

  histolist->Add(hVtxErrFitX);
  histolist->Add(hVtxErrFitY);
  histolist->Add(hVtxErrFitZ);

  histolist->Add(hVtxErrKinX);
  histolist->Add(hVtxErrKinY);
  histolist->Add(hVtxErrKinZ);

  histolist->Add(hVtxPullPocaX);
  histolist->Add(hVtxPullPocaY);
  histolist->Add(hVtxPullPocaZ);

  histolist->Add(hVtxPullFastX);
  histolist->Add(hVtxPullFastY);
  histolist->Add(hVtxPullFastZ);

  histolist->Add(hVtxPullFitX);
  histolist->Add(hVtxPullFitY);
  histolist->Add(hVtxPullFitZ);

  histolist->Add(hVtxPullKinX);
  histolist->Add(hVtxPullKinY);
  histolist->Add(hVtxPullKinZ);

  histolist->Add(hVtxDiffPX);
  histolist->Add(hVtxDiffPY);
  histolist->Add(hVtxDiffPZ);

  histolist->Add(hVtxPullPX);
  histolist->Add(hVtxPullPY);
  histolist->Add(hVtxPullPZ);

  histolist->Add(hVtxDiffFitPX);
  histolist->Add(hVtxDiffFitPY);
  histolist->Add(hVtxDiffFitPZ);

  histolist->Add(hVtxPullFitPX);
  histolist->Add(hVtxPullFitPY);
  histolist->Add(hVtxPullFitPZ);

  histolist->Add(hVtxDiffKinPX);
  histolist->Add(hVtxDiffKinPY);
  histolist->Add(hVtxDiffKinPZ);

  histolist->Add(hVtxPullKinPX);
  histolist->Add(hVtxPullKinPY);
  histolist->Add(hVtxPullKinPZ);

  histolist->Add(hVtxDiffE);
  histolist->Add(hVtxDiffFitE);
  histolist->Add(hVtxDiffKinE);
  histolist->Add(hVtxPullE);
  histolist->Add(hVtxPullFitE);
  histolist->Add(hVtxPullKinE);

  histolist->Add(hVtxDiffThe);
  histolist->Add(hVtxDiffPhi);

  histolist->Add(hVtxDiffFitThe);
  histolist->Add(hVtxDiffFitPhi);

  histolist->Add(hVtxDiffKinThe);
  histolist->Add(hVtxDiffKinPhi);

  histolist->Add(hVtxPocas);
  histolist->Add(hVtxChi2Fast);
  histolist->Add(hVtxChi2Fit);
  histolist->Add(hVtxChi2Kin);
  histolist->Add(hVtxChiProbFast);
  histolist->Add(hVtxChiProbFit);
  histolist->Add(hVtxChiProbKin);

  histolist->Add(hVtxPocaXY);
  histolist->Add(hVtxFastXY);
  histolist->Add(hVtxFitXY);
  histolist->Add(hVtxKinXY);
  histolist->Add(hVtxPocaRZ);
  histolist->Add(hVtxFastRZ);
  histolist->Add(hVtxFitRZ);
  histolist->Add(hVtxKinRZ);

  histolist->Add(hPrgPull0);
  histolist->Add(hPrgPull1);
  histolist->Add(hPrgPull2);
  histolist->Add(hPrgPull3);
  histolist->Add(hPrgPull4);

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

  for(int i=0;i<nevt;i++)
  {
    printf("Event %i of %i. (seed=%i)\n",i,nevt,ranseed);
    RhoCandidate *combiCand=0,*combiCand1=0,*combiCand2=0;
    vertexMC = RollVertexBox(widx,widy,widz);
    fVertex=vertexMC;

    PoorManTracks();
    if (fCands->GetEntriesFast()<2) {
      cout<<"Warning: too few particles created."<<endl;
      continue;
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
      hVtxDiffX->Fill(vertexDiff.X());
      hVtxDiffY->Fill(vertexDiff.Y());
      hVtxDiffZ->Fill(vertexDiff.Z());

      mommc=mccand->P4();
      mom=tmpcand->P4();
      momdiff=mommc-mom;
      amomcov=tmpcand->P4Cov();
      hVtxDiffPX->Fill(momdiff.X());
      hVtxDiffPY->Fill(momdiff.Y());
      hVtxDiffPZ->Fill(momdiff.Z());
      hVtxDiffE->Fill(momdiff.E());

      hVtxPullPX->Fill(momdiff.X()/sqrt(amomcov[0][0]));
      hVtxPullPY->Fill(momdiff.Y()/sqrt(amomcov[1][1]));
      hVtxPullPZ->Fill(momdiff.Z()/sqrt(amomcov[2][2]));
      hVtxPullE->Fill(momdiff.E()/sqrt(amomcov[3][3]));

      hVtxDiffThe->Fill(mommc.Theta()-mom.Theta());
      hVtxDiffPhi->Fill(mommc.Phi()-mom.Phi());

      // Now check the PRG description....
      Bool_t prgcheckmc = PndAnalysisCalcTools::P7toPRG(mccand->Pos(),mccand->P4(),mccand->Charge(),mccand->Cov7(),nullpunkt, mcparams, helixCov, jacobian, kFALSE);
      Bool_t prgchecktc = PndAnalysisCalcTools::P7toPRG(tmpcand->Pos(),tmpcand->P4(),tmpcand->Charge(),tmpcand->Cov7(),nullpunkt, helixparams, helixCov, jacobian, kFALSE);
//      Bool_t prgcheckmc = PndAnalysisCalcTools::P7toHelix(mccand->Pos(),mccand->P4(),mccand->Charge(),mccand->Cov7(), mcparams, helixCov, kFALSE);
//      Bool_t prgchecktc = PndAnalysisCalcTools::P7toHelix(tmpcand->Pos(),tmpcand->P4(),tmpcand->Charge(),tmpcand->Cov7(), helixparams, helixCov, kFALSE);
      hPrgPull0->Fill((mcparams[0]-helixparams[0])/sqrt(helixCov[0][0]));
      hPrgPull1->Fill((mcparams[1]-helixparams[1])/sqrt(helixCov[1][1]));
      hPrgPull2->Fill((mcparams[2]-helixparams[2])/sqrt(helixCov[2][2]));
      hPrgPull3->Fill((mcparams[3]-helixparams[3])/sqrt(helixCov[3][3]));
      hPrgPull4->Fill((mcparams[4]-helixparams[4])/sqrt(helixCov[4][4]));
    }

    // ********** POCA
    RhoVtxPoca vPoca;
    vertexPoc=vertexMC; //seed
    if(fVerbose>0) cout<<"poca "<<flush;
    double dist = vPoca.GetPocaVtx(vertexPoc,combiCand);
    if(fVerbose>0) cout<<" - Dist = "<<dist<<"  \t";if(fVerbose>1)cout<<endl;
    vertexDiff=vertexMC - vertexPoc;
    if(fVerbose>1) vertexMC.Print();
    if(fVerbose>1) vertexPoc.Print();
    if(fVerbose>1) vertexDiff.Print();

    hVtxPocaX->Fill(vertexDiff.X());
    hVtxPocaY->Fill(vertexDiff.Y());
    hVtxPocaZ->Fill(vertexDiff.Z());
    hVtxPocaXY->Fill(vertexDiff.X(),vertexDiff.Y());
    hVtxPocaRZ->Fill(vertexDiff.Z(),vertexMC.Perp()-vertexPoc.Perp());
    hVtxPocas->Fill(dist);
    hVtxPullPocaX->Fill(vertexDiff.X()/dist);
    hVtxPullPocaY->Fill(vertexDiff.Y()/dist);
    hVtxPullPocaZ->Fill(vertexDiff.Z()/dist);

    // ********** FAST FIT
    PndKalmanVtxFitter vFastter(combiCand);
    vFastter.SetSilent();
    //vFastter.SetDebug();
    if(fVerbose>0) cout<<"prg "<<flush;
    vertexFast=nullpunkt; //seed
    double checkfast = vFastter.FitVertexFast(vertexFast,covV);
    double chiq=vFastter.GetChi2();
    int ndf = vFastter.GetNdf();
    double prob = vFastter.GetProb();
    if(fVerbose>0) cout<<" - chiq = "<<chiq<<"  \t";if(fVerbose>1)cout<<endl;
    if(checkfast>0.)
    {
      hVtxChi2Fast->Fill(chiq);
      hVtxChiProbFast->Fill(prob);

      vertexDiffFast=vertexMC - vertexFast;
      if(fVerbose>1) vertexMC.Print();
      if(fVerbose>1) vertexFast.Print();
      if(fVerbose>0) vertexDiffFast.Print();

      hVtxFastX->Fill(vertexDiffFast.X());
      hVtxFastY->Fill(vertexDiffFast.Y());
      hVtxFastZ->Fill(vertexDiffFast.Z());
      hVtxFastXY->Fill(vertexDiffFast.X(),vertexDiffFast.Y());
      hVtxFastRZ->Fill(vertexDiffFast.Z(),vertexMC.Perp()-vertexFast.Perp());

      hVtxErrFastX->Fill(sqrt(covV[0][0]));
      hVtxErrFastY->Fill(sqrt(covV[1][1]));
      hVtxErrFastZ->Fill(sqrt(covV[2][2]));
      hVtxPullFastX->Fill(vertexDiffFast.X()/sqrt(covV[0][0]));
      hVtxPullFastY->Fill(vertexDiffFast.Y()/sqrt(covV[1][1]));
      hVtxPullFastZ->Fill(vertexDiffFast.Z()/sqrt(covV[2][2]));
    }
    // ********** FULL FIT
    PndKalmanVtxFitter vFitter(combiCand1);
    vFitter.SetSilent();
    //PndAnalysisCalcTools::SetVerbose(5);
    //if(laut)vFitter.SetDebug(true);
    vertexFit=nullpunkt; //seed
    if(fVerbose>0)  cout<<"prg  "<<flush;
    //if(laut>0)  PndAnalysisCalcTools::SetVerbose(3);
    bool check = vFitter.Fit();
    double chiqfit = vFitter.GetChi2();
    int ndffit = vFitter.GetNdf();
    double probfit = vFitter.GetProb();
    //if(laut>0)  PndAnalysisCalcTools::SetVerbose(0);
    if(fVerbose>0)  cout<<" - chiq = "<<chiqfit<<"  \t"<<flush;if(fVerbose>1)cout<<endl;
    if (check)
    {
      //PndAnalysisCalcTools::SetVerbose(0);
      hVtxChi2Fit->Fill(chiqfit);
      hVtxChiProbFit->Fill(probfit);

      vertexFit=combiCand1->GetFit()->DecayVtx();
      covVfit=combiCand1->GetFit()->DecayVtx().CovMatrix();
      vertexDiffFit=vertexMC - vertexFit;
      if(fVerbose>1) vertexMC.Print();
      if(fVerbose>1) vertexFit.Print();
      if(fVerbose>1) vertexDiffFit.Print();

      hVtxFitX->Fill(vertexDiffFit.X());
      hVtxFitY->Fill(vertexDiffFit.Y());
      hVtxFitZ->Fill(vertexDiffFit.Z());
      hVtxFitXY->Fill(vertexDiffFit.X(),vertexDiffFit.Y());
      hVtxFitRZ->Fill(vertexDiffFit.Z(),vertexMC.Perp()-vertexFit.Perp());

      hVtxErrFitX->Fill(sqrt(covVfit[0][0]));
      hVtxErrFitY->Fill(sqrt(covVfit[1][1]));
      hVtxErrFitZ->Fill(sqrt(covVfit[2][2]));
      hVtxPullFitX->Fill(vertexDiffFit.X()/sqrt(covVfit[0][0]));
      hVtxPullFitY->Fill(vertexDiffFit.Y()/sqrt(covVfit[1][1]));
      hVtxPullFitZ->Fill(vertexDiffFit.Z()/sqrt(covVfit[2][2]));

      for(int k=0;k<combiCand1->NDaughters();k++)
      {
        tmpcand = combiCand1->Daughter(k);
        mccand = tmpcand->GetMcTruth();
        tmpcand = tmpcand->GetFit();
        mommc=mccand->P4();
        mom=tmpcand->P4();
        momdiff=mommc-mom;
        amomcov=tmpcand->P4Cov();
        hVtxDiffFitPX->Fill(momdiff.X());
        hVtxDiffFitPY->Fill(momdiff.Y());
        hVtxDiffFitPZ->Fill(momdiff.Z());
        hVtxDiffFitE->Fill(momdiff.E());

        hVtxPullFitPX->Fill(momdiff.X()/sqrt(amomcov[0][0]));
        hVtxPullFitPY->Fill(momdiff.Y()/sqrt(amomcov[1][1]));
        hVtxPullFitPZ->Fill(momdiff.Z()/sqrt(amomcov[2][2]));
        hVtxPullFitE->Fill(momdiff.E()/sqrt(amomcov[3][3]));

        hVtxDiffFitThe->Fill(mommc.Theta()-mom.Theta());
        hVtxDiffFitPhi->Fill(mommc.Phi()-mom.Phi());
      }
    }

    // ********** Kin FIT
    RhoKinVtxFitter kFitter(combiCand2);
    vertexFit=nullpunkt; //seed
    if(fVerbose>0)  cout<<"kin  "<<flush;
    bool test = kFitter.Fit();
    double chiqkin = kFitter.GetChi2();
    int ndfkin = kFitter.GetNdf();
    double probkin = kFitter.GetProb();
    //if(laut>0)  PndAnalysisCalcTools::SetVerbose(0);
    if(fVerbose>0)  cout<<" - chiq = "<<chiqkin<<"  \t"<<flush;if(fVerbose>1)cout<<endl;
    if (test)
    {
      //PndAnalysisCalcTools::SetVerbose(0);
      vertexKin=combiCand2->GetFit()->DecayVtx();
      aposcov=combiCand2->GetFit()->DecayVtx().CovMatrix();
      hVtxChi2Kin->Fill(chiqkin);
      hVtxChiProbKin->Fill(probkin);

      vertexDiffKin=vertexMC - vertexKin;
      if(fVerbose>1) vertexMC.Print();
      if(fVerbose>1) vertexKin.Print();
      if(fVerbose>0) vertexDiffKin.Print();

      hVtxKinX->Fill(vertexDiffKin.X());
      hVtxKinY->Fill(vertexDiffKin.Y());
      hVtxKinZ->Fill(vertexDiffKin.Z());
      hVtxKinXY->Fill(vertexDiffKin.X(),vertexDiffKin.Y());
      hVtxKinRZ->Fill(vertexDiffKin.Z(),vertexMC.Perp()-vertexKin.Perp());

      hVtxErrKinX->Fill(sqrt(aposcov[0][0]));
      hVtxErrKinY->Fill(sqrt(aposcov[1][1]));
      hVtxErrKinZ->Fill(sqrt(aposcov[2][2]));
      hVtxPullKinX->Fill(vertexDiffKin.X()/sqrt(aposcov[0][0]));
      hVtxPullKinY->Fill(vertexDiffKin.Y()/sqrt(aposcov[1][1]));
      hVtxPullKinZ->Fill(vertexDiffKin.Z()/sqrt(aposcov[2][2]));

      for(int k=0;k<combiCand2->NDaughters();k++)
      {
        tmpcand = combiCand2->Daughter(k);
        mccand = tmpcand->GetMcTruth();
        tmpcand = tmpcand->GetFit();
        mommc=mccand->P4();
        mom=tmpcand->P4();
        momdiff=mommc-mom;
        amomcov=tmpcand->P4Cov();
        hVtxDiffKinPX->Fill(momdiff.X());
        hVtxDiffKinPY->Fill(momdiff.Y());
        hVtxDiffKinPZ->Fill(momdiff.Z());
        hVtxDiffKinE->Fill(momdiff.E());

        hVtxPullKinPX->Fill(momdiff.X()/sqrt(amomcov[0][0]));
        hVtxPullKinPY->Fill(momdiff.Y()/sqrt(amomcov[1][1]));
        hVtxPullKinPZ->Fill(momdiff.Z()/sqrt(amomcov[2][2]));
        hVtxPullKinE->Fill(momdiff.E()/sqrt(amomcov[3][3]));

        hVtxDiffKinThe->Fill(mommc.Theta()-mom.Theta());
        hVtxDiffKinPhi->Fill(mommc.Phi()-mom.Phi());
      }
    }
  } // event loop

  cout<<" ---------- calculations done, writing histograms."<<endl;
  //manipulate & fit histos now

  //Write and Plot all histos from List

  TString hfile="Data/HistoVertexing";
  if(seed>=0){hfile+="-";hfile+=ranseed;}
  hfile+=".root";
  TFile* histofile = new TFile(hfile.Data(),"RECREATE");
  histofile->cd();
  TListIter iter(histolist);
  TH1* tmph=NULL;
  while( tmph=(TH1*)iter() ) tmph->Write();
  histofile->Close();

  // plothistosfromfile(hfile.Data(),"pdf",3,3);

  //gROOT->Macro("drawhistos.C");//automatic draw 1D histos with gauss fit
  return 0;

}

// --- Different centeroid finders


TVector3 RollVertexBox(double widx,double widy, double widz)
{
  double rnd1=widx*2*(0.5-gRandom->Rndm());
  double rnd2=widy*2*(0.5-gRandom->Rndm());
  double rnd3=widz*2*(0.5-gRandom->Rndm());
  TVector3 vertex(rnd1,rnd2,rnd3);
  if(fVerbose>2) cout<<"RollVertexBox: ("<<vertex.x()<<","<<vertex.y()<<","<<vertex.z()<<")"<<endl;
  return vertex;
}

void SmearVertex(TVector3& vertex)
{
  double rnd1=gRandom->Gaus(vertex.x(),fSigVx);
  double rnd2=gRandom->Gaus(vertex.y(),fSigVy);
  double rnd3=gRandom->Gaus(vertex.z(),fSigVz);
  if(fVerbose>2) cout<<"SmearVertex (B): ("<<vertex.x()<<","<<vertex.y()<<","<<vertex.z()<<")"<<endl;
  vertex.SetXYZ(rnd1,rnd2,rnd3);
  if(fVerbose>2) cout<<"SmearVertex (A): ("<<vertex.x()<<","<<vertex.y()<<","<<vertex.z()<<")"<<endl;
  return;
}

TVector3 RollMomentumBox(const TVector3& vtx, double dtheta, double ptmin, double ptmax)
{
  // allow whole phi range
  double phi=TMath::TwoPi()*gRandom->Rndm();
  // allow theta at vertex pointing theta +- dtheta
  double theta = vtx.Theta() + dtheta*2*(0.5-gRandom->Rndm());
  // allow transverse momentum in [ptmin,ptmax]
  double pt = ptmin + (ptmax-ptmin)*gRandom->Rndm();
  TVector3 momentum;
  momentum.SetPtThetaPhi(pt,theta,phi);
  if(fVerbose>2) cout<<"RollMomentumBox: ("<<momentum.x()<<","<<momentum.y()<<","<<momentum.z()<<")"<<endl;
  return momentum;
}

SmearMomentum(TVector3& momentum)
{
  double rnd1=gRandom->Gaus(momentum.Px(),fSigPx);
  double rnd2=gRandom->Gaus(momentum.Py(),fSigPy);
  double rnd3=gRandom->Gaus(momentum.Pz(),fSigPz);
  momentum.SetXYZ(rnd1,rnd2,rnd3);
  return;
}

void PoorManTracks()//TVector3& vtx=fVertex
{
  if(fVerbose>1) cout<<" --------------------------------------------------- "<<endl;
  if(fVerbose>0) cout<<"Poor man tracks."<<endl;
  fMcCands->Clear("C");
  fCands->Clear("C");

  RhoError covPos(3), covP4(4);
  covPos[0][0]=fSigVx*fSigVx;
  covPos[1][1]=fSigVy*fSigVy;
  covPos[2][2]=fSigVz*fSigVz;
  covP4[0][0]=fSigPx*fSigPx;
  covP4[1][1]=fSigPy*fSigPy;
  covP4[2][2]=fSigPz*fSigPz;

  for (int iTr=0; iTr<fNumTrk; iTr++)
  {
    fPDG = TDatabasePDG::Instance()->GetParticle(fPID);
    TVector3 momentum = RollMomentumBox(fVertex,fDtheta*TMath::Pi(),0.9,1.1);

    int sizeMc = fMcCands->GetEntriesFast();
    RhoCandidate *pmc=new ((*fMcCands)[sizeMc]) RhoCandidate(momentum,fPDG);
    //pmc->SetMcTruth(pmc);
    pmc->SetPos(fVertex);
    //pmc->SetType(fPID);
    //pmc->SetMcMotherIdx(-1);
    //if(fabs(fCharge)>0){
    //  bool rc = PndAnalysisCalcTools::FillHelixParams(pmc, true);
    //  if(!rc && fVerbose>0) {
    //    Warning("BuildMcCands()","Faild calculation helix parameters");
    //    std::cout<<*pmc<<std::endl;
    //  }
    //}

    // Smeared "reco"
    int size = fCands->GetEntriesFast();
    TVector3 vertexMeasure = fVertex;
    SmearVertex(vertexMeasure);
    TVector3 momMeasure = momentum;
    SmearMomentum(momMeasure);


    RhoCandidate *p=new ((*fCands)[size]) RhoCandidate(momMeasure,fPDG);//TODO charge
    p->SetMcTruth(pmc);
    p->SetPos(vertexMeasure);
    //p->SetType(fPID);
    //p->SetMcMotherIdx(-1);
    // setting cov's
//    covP4[3][3]= p->Px()*p->Px()*covP4[0][0];
//    covP4[3][3]+=p->Py()*p->Py()*covP4[1][1];
//    covP4[3][3]+=p->Pz()*p->Pz()*covP4[2][2];
//    covP4[3][3]/=(p->E()*p->E());
    p->SetCov7(covPos,covP4);
    //if(fabs(fCharge)>0){
    //  bool rc = PndAnalysisCalcTools::FillHelixParams(p, false);
    //  if(!rc && fVerbose>0) {
    //    Warning("BuildMcCands()","Faild calculation helix parameters");
    //    std::cout<<*p<<std::endl;
    //  }
    //}
    if(fVerbose>1) cout<<" ---------\n pmc, p, pmcpos, ppos, fVertex"<<endl;
    if(fVerbose>1) std::cout<<*pmc<<std::endl;
    if(fVerbose>1) std::cout<<*p<<std::endl;
    if(fVerbose>1) p->P3Cov().Print();
    if(fVerbose>1) pmc->GetPosition().Print();
    if(fVerbose>1) p->GetPosition().Print();
    if(fVerbose>1) fVertex.Print();
    // print helix parameters and cov together with RhoCandidate V-P4 and cov7

    // alternate particle type
    if(fAlternate) fPID*=-1;
  }
  // iterate again to set mother relations
  //  for (int i=0;i<fMcCands->GetEntriesFast();i++)
  //  {
  //    RhoCandidate* aMcCand=(RhoCandidate*)fMcCands->At(i);
  //    mcMotherID=aMcCand->GetMcMotherIdx();
  //    if (mcMotherID<0) continue;
  //    RhoCandidate* aMother=(RhoCandidate*)fMcCands->At(mcMotherID);
  //    if(0 == aMother) continue;
  //    aMcCand->SetMotherLink(aMother); // This should add the mother-daughter and daughter-mother relation
  //  }

}
