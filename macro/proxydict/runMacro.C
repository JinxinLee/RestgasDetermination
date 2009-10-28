void runMacro(TString command){
  gSystem->Load("libpndbase.so");
  gROOT->ProcessLine(".include $VMCWORKDIR/pndbase/proxydict");
  gROOT->ProcessLine(command);

}
