void gethistos()
{
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/Tools.C");
  LoadPandaStyle();
  //swotch things off
  gStyle->SetOptTitle(1);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  // use bigger font for many plots on a page
  gStyle->SetTextSize(0.10);
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetTitleSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");
  gStyle->SetTitleSize(0.07,"y");
  gStyle->SetLabelSize(0.07,"z");
  gStyle->SetTitleSize(0.07,"z");
  gStyle->SetPadBottomMargin(0.17);

  TFile *canvasfile = new TFile("Residuals.root","READ");
  
  TCanvas *canne = (TCanvas*)canvasfile->Get("VtxPlots;1");
  TList *canlist = canne->GetListOfPrimitives();
  
  TList *histolisto = new TList();
  
  if (!canlist) {cout<<"List not there..."<<endl;return;}
  
  for(int i=0;i<canlist->GetEntries();i++)
  {
    TPad *apad = (TPad*)canlist->At(i);
    if (!apad) continue;
    TList *padlist = apad->GetListOfPrimitives();
    if (!padlist) continue;
//    for(int k=0;k<canlist->GetEntries();k++)
//    {
//      TObject *anob = (TObject*)padlist->At(k);
//      if (!anob) continue;
//      //cout<<anob->GetName()<<endl;
//    }
    TH1D *his = (TH1D*)padlist->At(1);
    histolisto->Add(his);
  }
  
  canvasfile->Close();
  TFile *histosfile = new TFile("Histos.root","RECREATE");
  for(int j=0;j<histolisto->GetEntries();j++)
  {
    TH1D *his=(TH1D*)histolisto->At(j);
    his->Write();
  }
  histosfile->Close();
  
  plotandfithistosfromfile("Histos.root",".pdf",3,3,900);
  //plothistosfromfile("Histos.root",".pdf",3,5,250);
}





plotandfithistosfromfile(TString filename = "histos.root", TString ext=".pdf", Int_t divx=3, Int_t divy=3, Int_t pix = 1200)
{ // Plot all histograms into a ps file
  // works with TH1, TH2, & TProfile
  TFile* file = new TFile(filename.Data());
  if (!file) {cout<<"File \""<<filename.Data()<<"\" is not there..."<<endl;return;}
  TCanvas* can = new TCanvas();
  Int_t pixx = pix;
  Int_t pixy = ceil(0.8*pix);
  can->SetCanvasSize(pixx,pixy);
  can->Divide(divx, divy);
  TString picname = filename;
  ext="-gfitsA4."+ext;
  ext.ReplaceAll("..",".");
  picname.ReplaceAll(".root",ext); // ps, png, pdf ...
  TString pic = picname + "["; // open empty ps
  cout << "opening: " << pic.Data()<<endl;
  can->Print(pic);
  pic=picname;
  
  TList* list = file->GetListOfKeys();
  if (!list) {cout<<"List not there..."<<endl;return;}
  int padcount = 1;
  TString keyclass="";
  for(int i=0;i<list->GetEntries();i++)
  {
    if(padcount > divx*divy)
    {
      can->Print(pic.Data());
      can->Clear();
      can->SetCanvasSize(pixx,pixy);
      can->Divide(divx, divy);
      padcount=1;
    }
    can->cd(padcount);
    TKey* key = (TKey*)list->At(i);
    keyclass = key->GetClassName();
    //cout<<keyclass.Data()<<endl;
    if(keyclass.Contains("TH1"))
    {
      //cout<<"try plotting a TH1"<<endl;
      TH1* his = (TH1*)key->ReadObj();
      TString titleO = his->GetTitle();
      //TString titel ="#bar{D} ^{0}#rightarrow K^{0}#pi^{+}#pi^{-} Vertex Resolution - ";
      TString titel ="Vertex Resolution - ";
      if(titleO.Contains("Poca")) titel+="POCA";
      else if(titleO.Contains("PRG")) titel+="PRG";
      else if(titleO.Contains("Kin")) titel+="KINVTX";
      his->SetTitle(titel.Data());
      his->GetXaxis()->SetNoExponent(); // put exponents to numbers directly
      his->GetYaxis()->SetNoExponent(); // put exponents to numbers directly
      GaussFitPlot(his);
    } else continue;
    
    padcount++;
  }
  
  can->Print(pic.Data());
  pic = picname + "]"; // close ps
  can->Print(pic.Data());
  cout << "closed: " << pic.Data()<<endl;
  return;
}



int ccc=0; // counter to avoid bad naming

void GaussFitPlot(TH1* his) {
  Double_t par[3]={0.,0.,0.};
  Double_t range=250.;
  TString fitname="mygaus";fitname+=ccc;
  TF1* fitted = new TF1(fitname.Data(),"gaus",-range,range);
  //fitted->SetParameters(par);
  cout<<" --- Fitting Gaussian to histogram "<<his->GetName()<<"  \""<<his->GetTitle()<<"\" ---"<<endl;
  fitted->SetLineColor(4);
  fitted->SetLineWidth(3);
  fitted->SetLineStyle(2);
  his->Fit(fitted,"","",-range,range);
  fitted->GetParameters(par);
  his->DrawCopy("");
  Double_t mean = par[1];
  Double_t sigma = par[2];
  TString str="Gaussian Fit:";
  DrawText( 0.18, 0.75, str.Data(),0.07,1);
  str=Form("#mu = %.3g #mum",mean);
  DrawText( 0.18, 0.65, str.Data(),0.07,1);
  str=Form("#sigma = %.3g #mum",sigma);
  DrawText( 0.18, 0.55, str.Data(),0.07,1);
  ccc++;
  return;
}
