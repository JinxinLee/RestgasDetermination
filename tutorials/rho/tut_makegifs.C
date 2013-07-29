tut_makegifs(TString fn)
{
  TFile *f=new TFile(fn,"READ");
    
  TKey *key;
  
  TCanvas *c1=new TCanvas("c1","c1",10,10,700,600);
  c1->cd();
  
  int i=1;
  
  TIter next(f->GetListOfKeys());
  
  while (key = (TKey*)next())
  {
	TString name(key->GetName());
	cout <<name<<endl;
	name+=".gif";
	TH1* h=(TH1*) key->ReadObj();
	h->Draw();
	c1->SaveAs(name);
  }
  //f->Close();
}