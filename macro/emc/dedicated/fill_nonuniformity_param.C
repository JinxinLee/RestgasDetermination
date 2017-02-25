// Set linear non-uniformity for barrel EMC crystals
// 1-11 - barrel, 12 - forward endcap, 13 - backward endcap
int fill_nonuniformity_param()
{
	TString fileName="EmcDigiNoniformityPars2.root";
	TFile *nonunifile = new TFile(fileName, "RECREATE");
	PndEmcDigiNonuniParObject *parObject=new PndEmcDigiNonuniParObject();
	
	Double_t pars[3];
	
	pars[0]=1.;
	pars[1]=-3./200.;
	pars[2]=0;

	for (Int_t i =1; i<=11; i++)
	{
		parObject->SetNonuniformity(i, pars);
	}
	
	parObject->Write();
	nonunifile->Close();
  return 0;
}

