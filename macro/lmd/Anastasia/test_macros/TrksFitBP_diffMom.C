/////////////////////////////////////////////////////////////////////////////////////////
///                                                                                   /// 
/// Script for comparision results                                   ///
/// with diff mom assumption for back-propagation     ///
/// author: Anastasia Karavdina                                    ///
/// date: 12/05/2013                                                      ///
///                                                                                  /// 
///////////////////////////////////////////////////////////////////////////////////////
#include "TString.h"
#include "TStopwatch.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
using namespace std;
void TrksFitBP_diffMom(TString pathNameG="/panda/curpandaroot/macro/lmd/testPixel/32042013/diffMom/")
{
  gROOT->Macro("/panda/curpandaroot/macro/lmd/Anastasia/test_macros/Style_Imported_Style.C");
  gROOT->SetStyle("Imported_Style"); 
  // const int ndiffMom=12;
  // double diffMom[ndiffMom]={0,1e-7,1e-6,1e-5,1e-4,1e-3,1e-2,1e-1,1e0,1e1,1e2,1e3};
  // TString diffMomName[ndiffMom]={"0","0_0000001","0_000001","0_00001","0_0001","0_001","0_01","0_1","1","10","100","1000"};

  const int ndiffMom=8;
  double diffMom[ndiffMom]={1e-10,1e-6,1e-5,1e-4,1e-3,1e-2,1e-1,1e0};
  TString diffMomName[ndiffMom]={"0","0_000001","0_00001","0_0001","0_001","0_01","0_1","1"};
  
  // const int ndiffMom=3;
  // double diffMom[ndiffMom]={0,1e-1,1e2};
  // TString diffMomName[ndiffMom]={"0","0_1","100"};

  const int nBeamPoints=5;
  double p_beam[nBeamPoints]={1.5, 4.06, 8.9, 11.91, 15};

 //  const int nMethods=6;
 //  TString Method[nMethods]={"_MINUIT_BPGEANE","_MINUIT_BPRK","_KALMANGEANE_BPGEANE","_KALMANGEANE_BPRK","_KALMANRK_BPGEANE","_KALMANRK_BPRK"};
 // TString MethodName[nMethods]={"MINUIT, BP=GEANE","MINUIT, BP=RK","KALMAN=GEANE, BP=GEANE","KALMAN=GEANE, BP=RK","KALMAN=RK, BP=GEANE","KALMAN=RK, BP=RK"};
  const int nMethods=2;
  TString Method[nMethods]={"_MINUIT_BPGEANE","_MINUIT_BPRK"};
  TString MethodName[nMethods]={"GEANE BP","Runge-Kutta BP"};

 
//Creat and fill graphs with pbeam dependence
  TGraphErrors *gEff[nBeamPoints][nMethods];
  TGraphErrors *gResPx_mean[nBeamPoints][nMethods], *gResPx_sigma[nBeamPoints][nMethods], *gResPy_mean[nBeamPoints][nMethods], *gResPy_sigma[nBeamPoints][nMethods], *gResPz_mean[nBeamPoints][nMethods], *gResPz_sigma[nBeamPoints][nMethods];
  TGraphErrors *gResX_mean[nBeamPoints][nMethods], *gResX_sigma[nBeamPoints][nMethods], *gResY_mean[nBeamPoints][nMethods], *gResY_sigma[nBeamPoints][nMethods], *gResZ_mean[nBeamPoints][nMethods], *gResZ_sigma[nBeamPoints][nMethods];
  TGraphErrors *gResTheta_mean[nBeamPoints][nMethods], *gResTheta_sigma[nBeamPoints][nMethods], *gResPhi_mean[nBeamPoints][nMethods], *gResPhi_sigma[nBeamPoints][nMethods];

  TMultiGraph *mgEff = new TMultiGraph("mgEff",""); 
 TMultiGraph *mgResPx_mean = new TMultiGraph("mgResPx_mean",""); TMultiGraph *mgResPx_sigma = new TMultiGraph("mgResPx_sigma","");
  TMultiGraph *mgResPy_mean = new TMultiGraph("mgResPy_mean",""); TMultiGraph *mgResPy_sigma = new TMultiGraph("mgResPy_sigma","");
  TMultiGraph *mgResPz_mean = new TMultiGraph("mgResPz_mean",""); TMultiGraph *mgResPz_sigma = new TMultiGraph("mgResPz_sigma","");
  TMultiGraph *mgResX_mean = new TMultiGraph("mgResX_mean",""); TMultiGraph *mgResX_sigma = new TMultiGraph("mgResX_sigma","");
  TMultiGraph *mgResY_mean = new TMultiGraph("mgResY_mean",""); TMultiGraph *mgResY_sigma = new TMultiGraph("mgResY_sigma","");
  TMultiGraph *mgResZ_mean = new TMultiGraph("mgResZ_mean",""); TMultiGraph *mgResZ_sigma = new TMultiGraph("mgResZ_sigma","");
  TMultiGraph *mgResTheta_mean = new TMultiGraph("mgResTheta_mean",""); TMultiGraph *mgResTheta_sigma = new TMultiGraph("mgResTheta_sigma","");
  TMultiGraph *mgResPhi_mean = new TMultiGraph("mgResPhi_mean",""); TMultiGraph *mgResPhi_sigma = new TMultiGraph("mgResPhi_sigma","");

  double Eff[nBeamPoints][nMethods][ndiffMom];
  double ResPx_mean[nBeamPoints][nMethods][ndiffMom], ResPx_sigma[nBeamPoints][nMethods][ndiffMom], ResPy_mean[nBeamPoints][nMethods][ndiffMom], ResPy_sigma[nBeamPoints][nMethods][ndiffMom], ResPz_mean[nBeamPoints][nMethods][ndiffMom], ResPz_sigma[nBeamPoints][nMethods][ndiffMom];
  double ResX_mean[nBeamPoints][nMethods][ndiffMom], ResX_sigma[nBeamPoints][nMethods][ndiffMom],ResY_mean[nBeamPoints][nMethods][ndiffMom], ResY_sigma[nBeamPoints][nMethods][ndiffMom],ResZ_mean[nBeamPoints][nMethods][ndiffMom], ResZ_sigma[nBeamPoints][nMethods][ndiffMom];
  double ResTheta_mean[nBeamPoints][nMethods][ndiffMom], ResTheta_sigma[nBeamPoints][nMethods][ndiffMom], ResPhi_mean[nBeamPoints][nMethods][ndiffMom], ResPhi_sigma[nBeamPoints][nMethods][ndiffMom];
 
  double erResPx_mean[nBeamPoints][nMethods][ndiffMom], erResPx_sigma[nBeamPoints][nMethods][ndiffMom], erResPy_mean[nBeamPoints][nMethods][ndiffMom], erResPy_sigma[nBeamPoints][nMethods][ndiffMom], erResPz_mean[nBeamPoints][nMethods][ndiffMom], erResPz_sigma[nBeamPoints][nMethods][ndiffMom];
  double erResX_mean[nBeamPoints][nMethods][ndiffMom], erResX_sigma[nBeamPoints][nMethods][ndiffMom], erResY_mean[nBeamPoints][nMethods][ndiffMom], erResY_sigma[nBeamPoints][nMethods][ndiffMom], erResZ_mean[nBeamPoints][nMethods][ndiffMom], erResZ_sigma[nBeamPoints][nMethods][ndiffMom];
  double erResTheta_mean[nBeamPoints][nMethods][ndiffMom], erResTheta_sigma[nBeamPoints][nMethods][ndiffMom], erResPhi_mean[nBeamPoints][nMethods][ndiffMom], erResPhi_sigma[nBeamPoints][nMethods][ndiffMom];

  for(int idiff=0;idiff<ndiffMom;idiff++){
    TString pathName=pathNameG+"diff_";
    pathName+=diffMomName[idiff];
    

    TString compfilename = "Lumi_compare_MC_and_REC_trks";
    for(int ip=0;ip<nBeamPoints;ip++){
      TString fileNameG = pathName+"/mom_";
      //    cout<<"Now read files from: "<<pathName<<endl;
      switch (ip){
      case 0: fileNameG+="1_5/";
	break;
      case 1: fileNameG+="4_06/";
	break;
      case 2: fileNameG+="8_9/";
	break;
      case 3: fileNameG+="11_91/";
	break;
      case 4: fileNameG+="15/";
	break;
      default: cout << "?????";
	break;
      }
      fileNameG+= compfilename;
 

  for(int im=0;im<nMethods;im++){
    TString fileName = fileNameG;
    fileName+=Method[im];
    fileName+=".root";
    TFile *fileIN = new TFile(fileName,"READ");

  //Resolution

  TH1F* hResPointPx = (TH1F*)fileIN->Get("hResPointPx");
  TF1 *fResPx  = new TF1("fitPPx","gaus",-10,10);
  hResPointPx->Fit(fResPx,"qr");
  ResPx_mean[ip][im][idiff] = 1e3*fResPx->GetParameter(1);
  //  cout<<" ResPx_mean["<<ip<<"]["<<im<<"]["<<idiff<<"] = "<< ResPx_mean[ip][im][idiff]<<endl;
  erResPx_mean[ip][im][idiff] = 1e3*fResPx->GetParError(1) ;
  ResPx_sigma[ip][im][idiff] = 1e3*fResPx->GetParameter(2);
  erResPx_sigma[ip][im][idiff] = 1e3*fResPx->GetParError(2) ;

  TH1F* hResPointPy = (TH1F*)fileIN->Get("hResPointPy");
  TF1 *fResPy = new TF1("fitPPy","gaus",-10,10);
  hResPointPy->Fit(fResPy,"qr");
  ResPy_mean[ip][im][idiff] = 1e3*fResPy->GetParameter(1);
  ResPy_sigma[ip][im][idiff] = 1e3*fResPy->GetParameter(2);
  erResPy_mean[ip][im][idiff] = 1e3*fResPy->GetParError(1);
  erResPy_sigma[ip][im][idiff] = 1e3*fResPy->GetParError(2);
  
  TH1F* hResPointPz = (TH1F*)fileIN->Get("hResPointPz");
  TF1 *fResPz  = new TF1("fitPPz","gaus",-10,10);
  hResPointPz->Fit(fResPz,"qr");
  ResPz_mean[ip][im][idiff] = 1e3*fResPz->GetParameter(1);
  ResPz_sigma[ip][im][idiff] = 1e3*fResPz->GetParameter(2);
  erResPz_mean[ip][im][idiff] = 1e3*fResPz->GetParError(1);
  erResPz_sigma[ip][im][idiff] = 1e3*fResPz->GetParError(2);

  TH1F* hResPointX = (TH1F*)fileIN->Get("hResPointX");
  TF1 *fResX  = new TF1("fitPX","gaus",-10,10);
  hResPointX->Fit(fResX,"qr");
  ResX_mean[ip][im][idiff] = fResX->GetParameter(1);
  ResX_sigma[ip][im][idiff] = fResX->GetParameter(2);
  erResX_mean[ip][im][idiff] = fResX->GetParError(1);
  erResX_sigma[ip][im][idiff] = fResX->GetParError(2);

  TH1F* hResPointY = (TH1F*)fileIN->Get("hResPointY");
  TF1 *fResY  = new TF1("fitPY","gaus",-10,10);
  hResPointY->Fit(fResY,"qr");
  ResY_mean[ip][im][idiff] = fResY->GetParameter(1);
  ResY_sigma[ip][im][idiff] = fResY->GetParameter(2);
  erResY_mean[ip][im][idiff] = fResY->GetParError(1);
  erResY_sigma[ip][im][idiff] = fResY->GetParError(2);
  
  TH1F* hResPointZ = (TH1F*)fileIN->Get("hResPointZ");
  TF1 *fResZ  = new TF1("fitPZ","gaus",-100,100);
  hResPointZ->Fit(fResZ,"qr");
  ResZ_mean[ip][im][idiff] = fResZ->GetParameter(1);
  ResZ_sigma[ip][im][idiff] = fResZ->GetParameter(2);
  erResZ_mean[ip][im][idiff] = fResZ->GetParError(1);
  erResZ_sigma[ip][im][idiff] = fResZ->GetParError(2);

  TH1F* hResTheta = (TH1F*)fileIN->Get("hResTheta");
  TF1 *fResTheta = new TF1("fitpth","gaus",-10,10);
  hResTheta->Fit(fResTheta,"qr");
  ResTheta_mean[ip][im][idiff] = 1e3*fResTheta->GetParameter(1);
  ResTheta_sigma[ip][im][idiff] = 1e3*fResTheta->GetParameter(2);
  erResTheta_mean[ip][im][idiff] = 1e3*fResTheta->GetParError(1);
  erResTheta_sigma[ip][im][idiff] = 1e3*fResTheta->GetParError(2);

  TH1F* hResPhi = (TH1F*)fileIN->Get("hResPhi");
  TF1 *fResPhi = new TF1("fitpphi","gaus",-10,10);
  hResPhi->Fit(fResPhi,"qr");
  ResPhi_mean[ip][im][idiff] = fResPhi->GetParameter(1);
  ResPhi_sigma[ip][im][idiff] = fResPhi->GetParameter(2);
  erResPhi_mean[ip][im][idiff] = fResPhi->GetParError(1);
  erResPhi_sigma[ip][im][idiff] = fResPhi->GetParError(2);

  //****************************************

  TH1F *hhits= (TH1F*)fileIN->Get("hhits");
  double Nrec =  hhits->GetEntries();
  TNtuple *nBadTrks = (TNtuple*)fileIN->Get("nBadTrks");
  double NbadRec = nBadTrks->GetEntries();
  Eff[ip][im][idiff] = 100*(Nrec-NbadRec)/1e5;

  
    //TODO: theta resolution???
  //  TH2F*hThetaResTheta = (TH2F*)fileIN->Get("hThetaResTheta");
  }//methods
  }//pbeam
  }//diffMom

  
  
  // for(int idiff=ndiffMom-1;idiff>=0;idiff--){
  for(int ip=0;ip<nBeamPoints;ip++){
    for(int im=0;im<nMethods;im++){
    // int markerstsh=20;
    // if(im>3) markerstsh=29;
      int markerstsh=23;
    int markerstshadd=4;
    if(im==3) markerstshadd=32-23;
    if(im==4) markerstshadd=27-29-4;
    if(im==5) markerstshadd=28-29-5;
    //    if(im>3) markerstshadd=-3;
    int matkercolsh = 1+ip;
    if(im>4) matkercolsh = 2+ip;

    gEff[ip][im] = new TGraphErrors(ndiffMom,diffMom,Eff[ip][im],0,0);
    gEff[ip][im]->SetMarkerStyle(markerstsh+im);
    gEff[ip][im]->SetMarkerColor(matkercolsh);
    gEff[ip][im]->SetMarkerSize(2.5);
    mgEff->Add(gEff[ip][im],"LP");

    gResPx_mean[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResPx_mean[ip][im],0,erResPx_mean[ip][im]);
    gResPx_sigma[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResPx_sigma[ip][im],0,erResPx_sigma[ip][im]);
    gResPx_mean[ip][im]->SetMarkerStyle(markerstsh+im);
    gResPx_sigma[ip][im]->SetMarkerStyle(markerstsh+im);
    gResPx_mean[ip][im]->SetMarkerColor(matkercolsh);
    gResPx_sigma[ip][im]->SetMarkerColor(matkercolsh);
    gResPx_mean[ip][im]->SetMarkerSize(2.5);
    gResPx_sigma[ip][im]->SetMarkerSize(2.5);
    mgResPx_mean->Add(gResPx_mean[ip][im],"LP");
    mgResPx_sigma->Add(gResPx_sigma[ip][im],"LP");
   
    gResPy_mean[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResPy_mean[ip][im],0,erResPy_mean[ip][im]);
    gResPy_sigma[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResPy_sigma[ip][im],0,erResPy_sigma[ip][im]);
    gResPy_mean[ip][im]->SetMarkerStyle(markerstsh+im);
    gResPy_sigma[ip][im]->SetMarkerStyle(markerstsh+im);
    gResPy_mean[ip][im]->SetMarkerColor(matkercolsh);
    gResPy_sigma[ip][im]->SetMarkerColor(matkercolsh);
    gResPy_mean[ip][im]->SetMarkerSize(2.5);
    gResPy_sigma[ip][im]->SetMarkerSize(2.5);
    mgResPy_mean->Add(gResPy_mean[ip][im],"LP");
    mgResPy_sigma->Add(gResPy_sigma[ip][im],"LP");

    gResPz_mean[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResPz_mean[ip][im],0,erResPz_mean[ip][im]);
    gResPz_sigma[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResPz_sigma[ip][im],0,erResPz_sigma[ip][im]);
    gResPz_mean[ip][im]->SetMarkerStyle(markerstsh+im);
    gResPz_sigma[ip][im]->SetMarkerStyle(markerstsh+im);
    gResPz_mean[ip][im]->SetMarkerColor(matkercolsh);
    gResPz_sigma[ip][im]->SetMarkerColor(matkercolsh);
    gResPz_mean[ip][im]->SetMarkerSize(2.5);
    gResPz_sigma[ip][im]->SetMarkerSize(2.5);
    mgResPz_mean->Add(gResPz_mean[ip][im],"LP");
    mgResPz_sigma->Add(gResPz_sigma[ip][im],"LP");
   
    gResX_mean[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResX_mean[ip][im],0,erResX_mean[ip][im]);
    gResX_sigma[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResX_sigma[ip][im],0,erResX_sigma[ip][im]);
    gResX_mean[ip][im]->SetMarkerStyle(markerstsh+im);
    gResX_sigma[ip][im]->SetMarkerStyle(markerstsh+im);
    gResX_mean[ip][im]->SetMarkerColor(matkercolsh);
    gResX_sigma[ip][im]->SetMarkerColor(matkercolsh);
    gResX_mean[ip][im]->SetMarkerSize(2.5);
    gResX_sigma[ip][im]->SetMarkerSize(2.5);
    mgResX_mean->Add(gResX_mean[ip][im],"LP");
    mgResX_sigma->Add(gResX_sigma[ip][im],"LP");
  
    gResY_mean[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResY_mean[ip][im],0,erResY_mean[ip][im]);
    gResY_sigma[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResY_sigma[ip][im],0,erResY_sigma[ip][im]);
    gResY_mean[ip][im]->SetMarkerStyle(markerstsh+im);
    gResY_sigma[ip][im]->SetMarkerStyle(markerstsh+im);
    gResY_mean[ip][im]->SetMarkerColor(matkercolsh);
    gResY_sigma[ip][im]->SetMarkerColor(matkercolsh);
    gResY_mean[ip][im]->SetMarkerSize(2.5);
    gResY_sigma[ip][im]->SetMarkerSize(2.5);
    mgResY_mean->Add(gResY_mean[ip][im],"LP");
    mgResY_sigma->Add(gResY_sigma[ip][im],"LP");
   

    gResZ_mean[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResZ_mean[ip][im],0,erResZ_mean[ip][im]);
    gResZ_sigma[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResZ_sigma[ip][im],0,erResZ_sigma[ip][im]);
    gResZ_mean[ip][im]->SetMarkerStyle(markerstsh+im);
    gResZ_sigma[ip][im]->SetMarkerStyle(markerstsh+im);
    gResZ_mean[ip][im]->SetMarkerColor(matkercolsh);
    gResZ_sigma[ip][im]->SetMarkerColor(matkercolsh);
    gResZ_mean[ip][im]->SetMarkerSize(2.5);
    gResZ_sigma[ip][im]->SetMarkerSize(2.5);
    mgResZ_mean->Add(gResZ_mean[ip][im],"LP");
    mgResZ_sigma->Add(gResZ_sigma[ip][im],"LP");
  

    gResTheta_mean[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResTheta_mean[ip][im],0,erResTheta_mean[ip][im]);
    gResTheta_sigma[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResTheta_sigma[ip][im],0,erResTheta_sigma[ip][im]);
    gResTheta_mean[ip][im]->SetMarkerStyle(markerstsh+im);
    gResTheta_sigma[ip][im]->SetMarkerStyle(markerstsh+im);
    gResTheta_mean[ip][im]->SetMarkerColor(matkercolsh);
    gResTheta_sigma[ip][im]->SetMarkerColor(matkercolsh);
    gResTheta_mean[ip][im]->SetMarkerSize(2.5);
    gResTheta_sigma[ip][im]->SetMarkerSize(2.5);
    mgResTheta_mean->Add(gResTheta_mean[ip][im],"LP");
    mgResTheta_sigma->Add(gResTheta_sigma[ip][im],"LP");
       

    gResPhi_mean[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResPhi_mean[ip][im],0,erResPhi_mean[ip][im]);
    gResPhi_sigma[ip][im] = new TGraphErrors(ndiffMom,diffMom,ResPhi_sigma[ip][im],0,erResPhi_sigma[ip][im]);
    gResPhi_mean[ip][im]->SetMarkerStyle(markerstsh+im);
    gResPhi_sigma[ip][im]->SetMarkerStyle(markerstsh+im);
    gResPhi_mean[ip][im]->SetMarkerColor(matkercolsh);
    gResPhi_sigma[ip][im]->SetMarkerColor(matkercolsh);
    gResPhi_mean[ip][im]->SetMarkerSize(2.5);
    gResPhi_sigma[ip][im]->SetMarkerSize(2.5);
    mgResPhi_mean->Add(gResPhi_mean[ip][im],"LP");
    mgResPhi_sigma->Add(gResPhi_sigma[ip][im],"LP");
   
  }
  //******************************************

 }//Pbeam

  //Draw & Save results
  TString resname= pathNameG+"/TrkFit_BackPropag_diffMom_Results";
  TString resname_pdf = resname+".pdf";

  TString resThetaMeanName =  pathNameG+"/Theta_mean_diffMom.pdf";
  TString resThetaSigmaName =  pathNameG+"/Theta_sigma_diffMom.pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";
 TString resname_root = resname+".root";
  TFile *fout = new TFile(resname_root,"RECREATE");

  //TLegend *leg = new TLegend(0.7,0.6,0.98,0.99);

  //  TLegend *leg = new TLegend(0.7,0.2,0.98,0.7);
  TLegend *leg = new TLegend(0.15,0.5,0.45,0.95);
  leg->SetFillColor(0);
  for(int im=nMethods-1;im>=0;im--){
    TString MethodName_ext2 = MethodName[im];
    leg->AddEntry(gResPx_mean[0][im],MethodName_ext2,"lep");
    if(im==0){
      TString descr="P, GeV/c:";
      leg->AddEntry((TObject*)0,descr," ");
      //  for(int idiff=0;idiff<ndiffMom;idiff++){
      for(int ip=0;ip<nBeamPoints;ip++){
	TString MomLeg=" ";
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(2) << p_beam[ip];
	MomLeg+=ss.str();
	leg->AddEntry(gResPx_mean[ip][im],MomLeg,"lep");
      }
    }
  }

  TCanvas c1;
  mgEff->Draw("AP"); 

  //  mgEff->GetXaxis()->SetLimits(diffMom[0],diffMom[ndiffMom-1]);
  mgEff->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgEff->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgEff->GetYaxis()->SetTitle("% trks with |P_{REC} - P_{beam}|<0.1*P_{beam}");
  leg->Draw();
  c1.SetLogx(1);// set log scale
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

  //Draw resolution only
 // c1.SetLogy(1);// set log scale
  mgResPx_mean->Draw("AP");  
  mgResPx_mean->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResPx_mean->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResPx_mean->GetYaxis()->SetTitle("(P^{MC}_{x} - P^{REC}_{x}), MeV/c");
  leg->Draw();
  c1.SetLogx(1);// set log scale
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

  mgResPx_sigma->Draw("AP");
  mgResPx_sigma->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResPx_sigma->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResPx_sigma->GetYaxis()->SetTitle("#sigma(P^{REC}_{x}), MeV/c");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

  mgResPy_mean->Draw("AP");  
  mgResPy_mean->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResPy_mean->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResPy_mean->GetYaxis()->SetTitle("(P^{MC}_{y} - P^{REC}_{y}), MeV/c");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

  mgResPy_sigma->Draw("AP");
  mgResPy_sigma->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResPy_sigma->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResPy_sigma->GetYaxis()->SetTitle("#sigma(P^{REC}_{y}), MeV/c");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

  mgResPz_mean->Draw("AP");  
  mgResPz_mean->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResPz_mean->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResPz_mean->GetYaxis()->SetTitle("(P^{MC}_{z} - P^{REC}_{z}), MeV/c");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

  mgResPz_sigma->Draw("AP");
  mgResPz_sigma->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResPz_sigma->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResPz_sigma->GetYaxis()->SetTitle("#sigma(P^{REC}_{z}), MeV/c");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();


  mgResX_mean->Draw("AP");  
  mgResX_mean->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResX_mean->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResX_mean->GetYaxis()->SetTitle("(X^{MC} - X^{REC}), cm");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
 
  mgResX_sigma->Draw("AP");
  mgResX_sigma->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResX_sigma->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResX_sigma->GetYaxis()->SetTitle("#sigma(X^{REC}), cm");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

 
  mgResY_mean->Draw("AP");  
  mgResY_mean->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResY_mean->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResY_mean->GetYaxis()->SetTitle("(Y^{MC} - Y^{REC}), cm");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

  mgResY_sigma->Draw("AP");
  mgResY_sigma->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResY_sigma->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResY_sigma->GetYaxis()->SetTitle("#sigma(Y^{REC}), cm");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

  mgResZ_mean->Draw("AP");  
  mgResZ_mean->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResZ_mean->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResZ_mean->GetYaxis()->SetTitle("|Z^{MC} - Z^{REC}|, cm");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
 
  mgResZ_sigma->Draw("AP");
  mgResZ_sigma->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResZ_sigma->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResZ_sigma->GetYaxis()->SetTitle("#sigma(Z^{REC}), cm");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  
  mgResTheta_mean->Draw("AP");  
  mgResTheta_mean->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResTheta_mean->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResTheta_mean->GetYaxis()->SetTitle("(#theta^{MC} - #theta^{REC}), mrad");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resThetaMeanName);
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  
  mgResTheta_sigma->Draw("AP");
  mgResTheta_sigma->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
  mgResTheta_sigma->GetXaxis()->SetTitle("#delta P, GeV/c");
  mgResTheta_sigma->GetYaxis()->SetTitle("#sigma(#theta^{REC}), mrad");
  c1.SetLogx(1);// set log scale
  leg->Draw();
  c1.Print(resThetaSigmaName);
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgResPhi_mean->Draw("AP");  
  mgResPhi_mean->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
 mgResPhi_mean->GetXaxis()->SetTitle("#delta P, GeV/c");
 mgResPhi_mean->GetYaxis()->SetTitle("(#phi^{MC} - #phi^{REC}), rad");
 c1.SetLogx(1);// set log scale
 leg->Draw();
 
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  
 // c1.SetLogy(1);// set log scale
  mgResPhi_sigma->Draw("AP");
  mgResPhi_sigma->GetXaxis()->SetLimits((diffMom[0]-1e-12),(diffMom[ndiffMom-1]+6e-1));
 mgResPhi_sigma->GetXaxis()->SetTitle("#delta P, GeV/c");
 mgResPhi_sigma->GetYaxis()->SetTitle("#sigma(#phi^{REC}), rad");
 c1.SetLogx(1);// set log scale
 leg->Draw();
  
 // c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
 // c1.Clear();
  //*************************************************************

  c1.Print(resname_pdf_c); //write canvas and close the pdf file open
  c1.Clear();
  mgEff->Write();
  mgResPx_mean->Write();
  mgResPx_sigma->Write();
  mgResPy_mean->Write();  
  mgResPy_sigma->Write();
  mgResPz_mean->Write();  
  mgResPz_sigma->Write();
  mgResX_mean->Write();  
  mgResX_sigma->Write();
  mgResY_mean->Write();  
  mgResY_sigma->Write();
  mgResZ_mean->Write();  
  mgResZ_sigma->Write();
  mgResTheta_mean->Write();  
  mgResTheta_sigma->Write();
  mgResPhi_mean->Write();  
  mgResPhi_sigma->Write();
  leg->Write();
  fout->Write();
}
