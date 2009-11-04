void runMacro(TString command){
  gSystem->Load("libPndBase.so");
  gROOT->ProcessLine(".include $VMCWORKDIR/pndbase/proxydict");
  gROOT->ProcessLine(".include $VMCWORKDIR/pndbase/PndStdUtils");
  gROOT->ProcessLine(".include $VMCWORKDIR/pndbase/AssocTools");
  gROOT->ProcessLine(command);

}
