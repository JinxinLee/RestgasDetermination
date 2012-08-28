#include "PndLmdLumiFit.h"


using namespace RooFit;

PndLmdLumiFit::PndLmdLumiFit(double inPlab, double insigtot, double inrho, double inb, double insigma_th):
  Plab(inPlab), sigtot(insigtot), rho(inrho), b(inb), sigma_th(insigma_th)
{

}

PndLmdLumiFit::~PndLmdLumiFit()
{

}

double PndLmdLumiFit::momt(double theta)
{
	const double M=0.938;
	const double Elab=sqrt(M*M+Plab*Plab);
	const double S=2*M*M+2*M*Elab;
	const double pcm2=S/4.-M*M;
	const double gamma=(Elab+M)/sqrt(S);
	double thetacm = 2*TMath::ATan(gamma*TMath::Tan(theta/1000.));
	double tcm = 2*pcm2*(1-cos(thetacm));
  	return tcm;
}

double PndLmdLumiFit::doppelgaus(Double_t* x, Double_t *par)
{
	return 1./par[0]/2.506631205*exp(-0.5*pow((x[0])/par[0],2))+0./par[1]/2.506631205*exp(-0.5*pow((x[0])/par[1],2));
}

double PndLmdLumiFit::mythInt(cs_th& mycs, Double_t min=3., Double_t max=8.){
  const unsigned int N=100000;
  Double_t result=0, step=(max-min)/((double)N);
  for(Double_t i=min; i<max; i+=step){
    result+=mycs.evaluate(i);
  }
  return (max-min)*result;
}

double PndLmdLumiFit::LumiFit(TH1D* thetaDist)
{
 // if(set=="1_5"){ Plab=1.5; sigtot=80.8183136; rho=-0.138515249; b=13.4420786; sigma_th=0.5; posbox=0.9;}
 // if(set=="6"){ Plab=6.; sigtot=65.249176; rho=-0.0626196936; b=11.9211817; sigma_th=0.3; posbox=0.9;}
  //if(set=="15"){ Plab=15.; sigtot=47.2178116; rho=-0.0857419446; b=11.6186924; sigma_th=0.3; posbox=0.6;}

  // S e t u p   c o m p o n e n t   p d f s 
  // ---------------------------------------

  thmin=3.; thmax=8.;

  // Construct observable
  RooRealVar th("th","th",1.8,10.);

  RooNumIntConfig customConfig(*RooAbsReal::defaultIntegratorConfig()) ;
  customConfig.method1D().setLabel("RooAdaptiveGaussKronrodIntegrator1D");

  // Construct landau(t,ml,sl) ;
 // RooRealVar ml("ml","mean landau",5.,-20,20) ;
 // RooRealVar sl("sl","sigma landau",1,0.1,10) ;
 // RooLandau landau("lx","lx",t,ml,sl) ;

  // Construct dsig/dt & dsig/dth ; Rho =  -0.138515249	B =   13.4420786	Sig_tot=   80.8183136mb
  RooRealVar Sigtot("Sigtot","Sigtot",sigtot);//,0,1000);
  RooRealVar B("B","B",b);//,0,1000);
  RooRealVar Rho("Rho","Rho",rho);//,-10,10);
  RooRealVar Leln("Leln","Leln",1000,0,100000000);
  cs_th thsCS("thsCS","CS elastic scattering DPM",th,Sigtot,B,Rho,Plab);

// Define background shape parameter
  RooRealVar linbkgn("linbkgn", "slope of background", 1, 0.8, 1.2);
  RooRealVar linbkgs("linbkgs", "slope of background", 1, 0.8, 1.2);
  RooRealVar linbkgt("linbkgt", "slope of background", 1, 0.8, 1.2);
 /* RooRealVar nbkgn("nbkgn", "events of background", 10000, 0, 100000000);
  RooRealVar nbkgs("nbkgs", "events of background", 10000, 0, 100000000);
  RooRealVar nbkgt("nbkgt", "events of background", 10000, 0, 100000000);*/
// Define linear background PDF
  RooGenericPdf bkgdn("bkgdn", "linbkgn*th", RooArgSet(th,linbkgn));
  //RooPolynomial bkcn("bkcn", "const function for background", th, RooArgList());
  //RooPolynomial bkgdno("bkgdno", "linear function for background", th, RooArgList(linbkgn));
 // RooExtendPdf ebkgdn("ebkgdn","linear function for background",bkgdn,nbkgn);

  // Construct gaussth(t,mg,sg)
  RooRealVar mgth("mgth","mg",0) ;
  RooRealVar sgth("sgth","sg",sigma_th) ;//,0.000001,100.) ;//
  RooGaussian gaussth("gaussth","gauss",th,mgth,sgth) ;

  thsCS.setIntegratorConfig(customConfig) ;
 
  // C o n s t r u c t   c o n v o l u t i o n   p d f 
  // ---------------------------------------

  // Set #bins to be used for FFT sampling to 10000
  th.setBins(10000,"cache"); 

  // Construct cs (x) gauss
  RooFFTConvPdf csxgth("csxgth","cs (X) gauss",th,thsCS,gaussth);

 // RooExtendPdf ethnCS("ethnCS","ethnCS",thnCS,Neln);

  RooRealVar nbkgn("nbkgn", "signal to background", 1, 0, 100);

 // RooAddPdf teCSn("teCSn","teCSn",thnCS,bkgdn,nbkgn);//,RooArgList(Neln,nbkgn));

/*  csxg.setBufferFraction(0.25);
  csxg2.setBufferFraction(0.25);*/

  // S a m p l e ,   f i t   a n d   p l o t   c o n v o l u t e d   p d f 
  // ----------------------------------------------------------------------

  RooDataHist thdatan("thdatas","Theta-Distribution normal",th,thetaDist);

  TLegend *leg1 = new TLegend(0.5,0.7,0.89,0.89);
 
  TLegendEntry *myEntr;

  // Plot cs pdf vs cs (X) gauss pdf
  RooPlot* frame2 = th.frame(Title("CS vs CS x gaus"));
  thsCS.plotOn(frame2,LineStyle(kDashed), LineColor(kBlue));
  csxgth.plotOn(frame2, LineColor(kGreen));
  myEntr = leg1->AddEntry(frame2->findObject("thsCS"),"DPM elastic CS","L");
  myEntr->SetLineColor(kBlue);
  myEntr->SetLineStyle(kDashed);

  // Fit gxlx to data
  RooFitResult* resultths = csxgth.fitTo(thdatan, Range(thmin, thmax), Save());

  // Plot data, cs pdf, cs (X) gauss pdf
  RooPlot* framethn = th.frame(Title("CS"));
  thdatan.plotOn(framethn);
  //thdatan.statOn(framethn, Layout(0.5, 0.9, 0.4));
  thsCS.plotOn(framethn);//,Normalization(1.0,RooAbsReal::RelativeExpected));
  //teCSn.plotOn(framethn,Components(thnCS),LineStyle(kDashed),LineColor(kGreen));
  //teCSn.plotOn(framethn,Components(bkgdn),LineStyle(kDotted),LineColor(kRed));
 // thnCS.plotOn(framethn,LineStyle(kDashed), LineColor(kGreen), Range(thmin, thmax));
 // bkgdn.plotOn(framethn,LineStyle(kDashed), LineColor(kRed), Range(thmin, thmax));
  thsCS.paramOn(framethn, Layout(0.5, 0.9, posbox), ShowConstants(true));

  // Draw frame on canvas
  TCanvas *can1 = new TCanvas("Lumi_convolution","Lumi fitted with convolution of cs+gaus",700,500) ;
 // can1->Divide(1,3);
  can1->cd(1);
  gPad->SetLeftMargin(0.15) ; framethn->GetYaxis()->SetTitleOffset(1.4) ; framethn->Draw() ;

  can1->Print("results/Conv.pdf");
  can1->Print("results/Conv.eps");

 
  Double_t int2 = mythInt(thsCS); //.createIntegral(th,th) ; 

  std::cout.precision(10);

  if(resultths){
    resultths->Print();
    std::cout << std::endl << "Norm CS: " << thsCS.getNorm(th)<< std::endl<<std::endl;
    return thsCS.getNorm(th);
 //const TMatrixDSym& cor1 = resultthn->correlationMatrix() ;
 //const TMatrixDSym& cov1 = resultthn->covarianceMatrix() ;
  // Print correlation, covariance matrix
  //cout << "correlation matrix" << endl ;
  //cor1.Print() ;
  //cout << "covariance matrix" << endl ;
  //cov1.Print() ;
  }

  //delete dpmfill; delete can1; delete frame; delete frameth;
  //delete t_MC_all; delete t_MC_lmdrange; delete t_MC_lmdrange_smeared; delete t_MC_lmdrange_triples; delete th_normal; delete th_smeared; delete th_tripled;

  return 0;
}

