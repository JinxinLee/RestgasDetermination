void merge(TString ntp, TString f1, TString f2, TString fout)
{
	TChain n(ntp);
	n.Add(f1);
	if (f2!="") n.Add(f2);
	n.Merge(fout);
}