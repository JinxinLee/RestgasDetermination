int fill_emc_cluster_calibration_param()
{

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	rootlogon();
	basiclibs();

	TString fileName="emc_cluster_correction_par_default.root";
	TFile *calibrParFile = new TFile(fileName, "RECREATE");
	PndEmcClusterCalibrationParObject *parObject=new PndEmcClusterCalibrationParObject();
	
	enum {barrel_low=1, barrel_high=2, fwcap=3, bwcap=4, fsc=5};
	Double_t pars1[10], pars2[10], pars3[10], pars4[10], pars5[5];
	
	pars1[0] =  4.13189e-02; 
	pars1[1] = -2.03834e-02;  
	pars1[2] = -2.58086e-03;   
	pars1[3] = -1.77821e-03;   
	pars1[4] = -1.73738e-02;   
	pars1[5] =  7.40362e-02;   
	pars1[6] = -6.41892e-02;   
	pars1[7] = -9.85564e-02;   
	pars1[8] =  1.50123e-01;   
	pars1[9] = -7.87742e-04;   
	

	parObject->SetCalibrationPar(barrel_low, pars1);
 	
	pars2[0] =  5.05003e-02;  
	pars2[1] = -3.47672e-02;   
	pars2[2] =  3.72767e-02;   
	pars2[3] = -1.26492e-02;   
	pars2[4] = -2.16876e-02;   
	pars2[5] =  1.02682e-01;   
	pars2[6] = -9.85242e-02;   
	pars2[7] = -1.39872e-01;   
	pars2[8] =  2.02309e-01;   
	pars2[9] =  1.11696e-03;   
	
	parObject->SetCalibrationPar(barrel_high, pars2);
		
	pars3[0] = 1.81631;
	pars3[1] =-1.71202e-02;
	pars3[2] = 3.59161e-03;
	pars3[3] =-3.46712e-04;
	pars3[4] =-3.73691e-01;
	pars3[5] =-1.56688;
	pars3[6] =-1.62618;
	pars3[7] =-4.10972e-01;
	pars3[8] = 2.2222;
	pars3[9] = 4.60908e-03;

	parObject->SetCalibrationPar(fwcap, pars3);
	
	pars4[0] = 1.45312;
	pars4[1] = 2.79086e-02;
	pars4[2] = 3.91932e-04;
	pars4[3] =-1.23117e-03;
	pars4[4] = 2.72270e-01;
	pars4[5] =-1.31540;
	pars4[6] = 1.44447;
	pars4[7] =-4.05724e-01;
	pars4[8] =-2.07396;
	pars4[9] = 4.80507e-02;
	
	
	parObject->SetCalibrationPar(bwcap, pars4);
	
	pars5[0]=3.31694;
	pars5[1]=0.0183379;
	pars5[2]=0.0327113;
	pars5[3]=0.00040156;
	pars5[4]=0.00641305;
	
	parObject->SetCalibrationPar(fsc, pars5);
	
	parObject->Write();
	calibrParFile->Close();
  return 0;
}

