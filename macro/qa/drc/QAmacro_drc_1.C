int QAmacro_drc_1()
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  Bool_t fTest=kFALSE;

  cout << "<W> No QA module(s) for the DRC defined ...yet!" << endl;

  if (fTest){
    cout << " Test passed" << endl;
    cout << " All ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
  }

  exit(0); 
}  
  
