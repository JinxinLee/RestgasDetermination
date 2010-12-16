plothistos(TString filename = "histos.root")
{
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  LoadPandaStyle();
  TFile* file = new TFile(filename.Data());
  if (!file) {cout<<"File not there..."<<endl;return;}
  TCanvas* can = new TCanvas();
  TString picname = filename;
  picname.ReplaceAll(".root",".ps");
  TString pic = picname + "["; // open empty ps
  cout << "opening: " << pic.Data()<<endl;
  can->Print(pic);
  pic=picname;
  
  TList* list = file->GetListOfKeys();
  if (!list) {cout<<"List not there..."<<endl;return;}
  int divx=2, divy=2;
  int padcount = 1;
  TString keyclass="";
  can->Divide(divx, divy);
  for(int i=0;i<list->GetEntries();i++)
  {
    if(padcount > divx+divy)
    {
      can->Print(pic.Data());
      can->Clear();
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
      his->Draw();
    }else if(keyclass.Contains("TH2"))
    {
      //cout<<"try plotting a TH2"<<endl;
      TH2* his2 = (TH2*)key->ReadObj();
      DrawNice2DHisto(his2);
    }else if(keyclass.Contains("TProfile"))
    {
      //cout<<"try plotting a TH2"<<endl;
      TProfile* hpro = (TProfile*)key->ReadObj();
      hpro->Draw();
    } else continue;
    
    padcount++;
  }
  
  can->Print(pic.Data());
  pic = picname + "]"; // close ps
  can->Print(pic.Data());
  cout << "closed: " << pic.Data()<<endl;
  return;
}
