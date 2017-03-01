int mergemct(TString ntp, TString fout, TString f1, TString f2="", TString f3="", TString f4="", TString f5="")
{
	TChain n(ntp);
	
	n.Add(f1);
	if (f2!="") n.Add(f2);
	if (f3!="") n.Add(f3);
	if (f4!="") n.Add(f4);
	if (f5!="") n.Add(f5);
	
	TFile f(fout,"RECREATE");
	TTree *newtree = n.CopyTree("xmct||mode%1000==900");
	f.Write();
	f.Close();
  return 0;
}
