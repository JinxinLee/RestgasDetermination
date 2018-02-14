
int drawhistosA4(TString filename="Data/HistoVertexing.root")
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

  plotandfithistosfromfile(filename.Data(),"pdf",3,5,1200);

  return 0;

}



plotandfithistosfromfile(TString filename = "histos.root", TString ext=".pdf", Int_t divx=2, Int_t divy=2, Int_t pix = 1200)
{ // Plot all histograms into a ps file
  // works with TH1, TH2, & TProfile
  TFile* file = new TFile(filename.Data());
  if (!file) {cout<<"File \""<<filename.Data()<<"\" is not there..."<<endl;return;}
  TCanvas* can = new TCanvas();
  Int_t pixx = pix;
  Int_t pixy = ceil(1.2*pix);
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
      TString titel=his->GetTitle();
      TString vorname = his->GetName();
      if(titel.Contains("value")) continue; // skip this one
      if(titel.Contains("#Theta")) continue; // skip this one
      if(titel.Contains("#Phi")) continue; // skip this one
      //if(titel.Contains("kin")) continue; // skip this one
      //if(titel.Contains("pull")&&!titel.Contains("full")) continue; // skip this one
      //if(titel.Contains("Pull")) continue; // skip this one
      if(vorname.EndsWith("the")||vorname.EndsWith("phi"))continue;

      if(titel.Contains("Momentum reco")) his->SetTitle("Momentum Smearing");
      his->GetXaxis()->SetNoExponent(); // put exponents to numbers directly
      his->GetYaxis()->SetNoExponent(); // put exponents to numbers directly
      if(titel.Contains("#chi")||titel.Contains("#Chi"))
      {
        LineFitPlot(his);
      }
      else {
        GaussFitPlot(his);
      }
    } else continue;

    padcount++;
  }

  can->Print(pic.Data());
  pic = picname + "]"; // close ps
  can->Print(pic.Data());
  cout << "closed: " << pic.Data()<<endl;
  TString convertcmd = "test -r ps2pdf && ps2pdf ";
  convertcmd += pic.Data();
  gSystem->Exec(convertcmd.Data());
  return;
}



int ccc=0; // counter to avoid bad naming

void GaussFitPlot(TH1* his) {
  Double_t par[3]={0.,0.,0.};
  TString fitname="mygaus";fitname+=ccc;
  TF1* fitted = new TF1(fitname.Data(),"gaus",-5,5);
  //fitted->SetParameters(par);
  cout<<" --- Fitting Gaussian to histogram "<<his->GetName()<<"  \""<<his->GetTitle()<<"\" ---"<<endl;
  fitted->SetLineColor(4);
  fitted->SetLineWidth(2);
  fitted->SetLineStyle(2);
  his->Fit(fitted);
  fitted->GetParameters(par);
  his->DrawCopy("");
  Double_t mean = par[1];
  Double_t sigma = par[2];
  TString str="Gaussian Fit:";
  DrawText( 0.2, 0.75, str.Data(),0.07,1);
  str=Form("#mu = %.3g",mean);
  DrawText( 0.2, 0.65, str.Data(),0.07,1);
  str=Form("#sigma = %.3g",sigma);
  DrawText( 0.2, 0.55, str.Data(),0.07,1);
  ccc++;
  return;
}


void LineFitPlot(TH1* his) {
  range=1.2*his->GetMaximum();
  his->SetAxisRange(0.,range,"Y");
  Double_t par[2]={0.,0.};
  TString fitname="myline";fitname+=ccc;
  TF1* fitted = new TF1(fitname.Data(),"pol1");
  cout<<" --- Fitting Line to histogram "<<his->GetName()<<"  \""<<his->GetTitle()<<"\" ---"<<endl;
  fitted->SetLineColor(4);
  fitted->SetLineWidth(2);
  fitted->SetLineStyle(2);
  his->Fit(fitted);
  fitted->GetParameters(par);
  his->DrawCopy("");
  TString str="Line Fit:";
  DrawText( 0.2, 0.425, str.Data(),0.07,1);
  str=Form("const = %.3g",par[0]);
  DrawText( 0.2, 0.325, str.Data(),0.07,1);
  str=Form("slope = %.2g",par[1]);
  DrawText( 0.2, 0.225, str.Data(),0.07,1);
  ccc++;
  return;
}

