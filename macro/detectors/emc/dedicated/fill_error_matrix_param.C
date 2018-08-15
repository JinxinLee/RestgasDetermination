int fill_error_matrix_param()
{

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	rootlogon();
	basiclibs();

	TString fileName="emc_error_matrix_default.root";
	TFile *errorfile = new TFile(fileName, "RECREATE");
	PndEmcErrorMatrixParObject *parObject=new PndEmcErrorMatrixParObject();
	
	enum {barrel, fwcap, bwcap, fsc};
	Double_t pars1[10], pars2[10], pars3[10], pars4[10];
	
	pars1[0]=0.087543;
	pars1[1]=0.806983;
	pars1[2]=0.084801;
	pars1[3]=0.000000;
	pars1[4]=0.669595;
	pars1[5]=0.336361;
	pars1[6]=0.000000;
	pars1[7]=0.067536;
	pars1[8]=0.348951;
	pars1[9]=0.000000;
	
	parObject->SetErrorMatrix(barrel, pars1);
 	
	pars2[0]=0.091336;
	pars2[1]=0.763612;
	pars2[2]=0.067230;
	pars2[3]=0.000000;
	pars2[4]=0.293973;
	pars2[5]=0.451571;
	pars2[6]=0.234385;
	pars2[7]=0.294116;
	pars2[8]=0.459566;
	pars2[9]=0.237317;
	
	parObject->SetErrorMatrix(fwcap, pars2);
		
	pars3[0]=0.073138;
	pars3[1]=0.874890;
	pars3[2]=0.119693;
	pars3[3]=0.000000;
	pars3[4]=0.455331;
	pars3[5]=0.457373;
	pars3[6]=0.604822;
	pars3[7]=0.558651;
	pars3[8]=0.337999;
	pars3[9]=0.528809;
	
	parObject->SetErrorMatrix(bwcap, pars3);
	
	pars4[0]=0.202292;
	pars4[1]=0.717711;
	pars4[2]=0.083642;
	pars4[3]=0.000000;
	pars4[4]=0.333771;
	pars4[5]=0.715637;
	pars4[6]=0.341344;
	pars4[7]=0.393149;
	pars4[8]=0.397055;
	pars4[9]=0.321306;
	
	parObject->SetErrorMatrix(fsc, pars4);
	
	parObject->Write();
	errorfile->Close();
  return 0;
}

