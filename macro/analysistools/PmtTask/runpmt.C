//
//  pmt.C
//  PANDAROOT
//
//  Author Ralf Kliemt.
//  Copyright (c) 2013 GSI. All rights reserved.
//
//

void runpmt(int nevt=250, int laut=0, int seed=-1)
{
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/Tools.C");
  LoadPandaStyle();
  gRandom->SetSeed(seed);
  RhoCalculationTools::ForceConstantBz(20.); // [kGs]=[0.1T]
  PndAnalysisCalcTools::SetVerbose(0);
  PndPmtTask* myPmt = new PndPmtTask();
  cout<<"init"<<endl;
  myPmt->Init();
  cout<<"event loop"<<endl;
  for(int i=0;i<nevt;i++){
    if( i % 100 == 0 ) cout<<"event "<<i<<"/"<<nevt<<endl;
    myPmt->Exec("");
  }
  cout<<"finish"<<endl;
  myPmt->Finish();
  cout<<"done"<<endl;

}
