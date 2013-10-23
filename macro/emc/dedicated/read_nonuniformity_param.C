void read_nonuniformity_param(Int_t type=1)
{
	TString fileName="EmcDigiNoniformityPars2.root";
	TFile *nonunifile = new TFile(fileName,"READ");
	PndEmcDigiNonuniParObject *parObject=new PndEmcDigiNonuniParObject();
	nonunifile->GetObject("PndEmcDigiNonuniParObject",parObject);
	
	Double_t pars1[3];
	
	parObject->GetNonuniformity(type, pars1);
	
	std::cout<<pars1[0]<<std::endl;
	std::cout<<pars1[1]<<std::endl;
	std::cout<<pars1[2]<<std::endl;
	
	nonunifile->Close();
}

