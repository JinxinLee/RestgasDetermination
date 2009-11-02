void runMacro(TString command){
  gSystem->Load("libPndBase.so");
  gROOT->ProcessLine(".include $VMCWORKDIR/pndbase/proxydict");
  gROOT->ProcessLine(command);

}
