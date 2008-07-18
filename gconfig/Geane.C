
// Configuration macro for Geane VirtualMC 

void Config()
{
 
          gMC3= new  TGeant3TGeo("C++ Interface to Geant3");
      cout << "-I- G3Config: Geant3 with TGeo has been created for Geane."
           << endl;
  // create Cbm Specific Stack
  CbmStack *st = new CbmStack(10);
  gMC3->SetStack( st ) ;

  // ******* GEANEconfiguration for simulated Runs  *******
  gMC3->SetDEBU(0, 0, 1);
  gMC3->SetSWIT(4, 10);
  
  gMC3->SetDCAY(0);
  gMC3->SetPAIR(0);
  gMC3->SetCOMP(0);
  gMC3->SetPHOT(0);
  gMC3->SetPFIS(0); //
  gMC3->SetDRAY(0);
  gMC3->SetANNI(0);
  gMC3->SetBREM(1);
  gMC3->SetMUNU(0);
  gMC3->SetCKOV(0);
  gMC3->SetHADR(0);         //Select pure GEANH (HADR 1) or GEANH/NUCRIN (HADR 3)//4 fluka 5 gcalor
  gMC3->SetLOSS(4);
  gMC3->SetMULS(1); 		//1=Moliere,3=Gaussian
  gMC3->SetRAYL(0);
  gMC3->SetSTRA(0);

  gMC3->SetAUTO(1);         //Select automatic STMIN etc... calc. (AUTO 1) or manual (AUTO 0)
  gMC3->SetABAN(0);        //Restore 3.16 behaviour for abandoned tracks
  gMC3->SetOPTI(0);         //Select optimisation level for GEANT geometry searches (0,1,2)
  gMC3->SetERAN(5.e-7);


    Float_t cut =  1.e-3;        // 1MeV cut by default
    Float_t cuth =  1.e-4;        // 0.1MeV cut by default
    Float_t tofmax = 1.e10;
    // Float_t cut2 =  1.e-10; // 1 keV?
    Float_t cut2 = cut;
    //Float_t cut3 =50.e-6 ; // Threshold for delta electrons
    Float_t cut3 =1.e4 ; // Threshold for delta electrons
    Float_t usrcuts[5]={0.,0.,0.,0.,0.};
    Float_t gcalpha=0.9996;
    cout<<"Energy straggling area parameter from user set to: "<<gcalpha<<endl;
    if(gcalpha<0.9) 
    {
    	gcalpha=0.9;
	cout<<"User alpha parameter too low: forced to 0.9"<<endl;
    }
    
    // set cuts here 
    //             GAM ELEC NHAD CHAD MUON EBREM MUHAB EDEL MUDEL MUPA TOFMAX
    gMC3->SetCUTS(cut2, cut2, cuth, cuth, cuth, cut2, cut2, cut3, cut2, cut2,
                    tofmax,usrcuts);
    gMC3->SetECut(gcalpha);
}


