void dummy_ana()
{
  PndMultiClassMVA bdt_train;
  bdt_train.AddVar("vara");
  bdt_train.AddVar("varb");
  bdt_train.AddVar("varc");
  bdt_train.AddClass("pion");
  bdt_train.AddClass("pro");
  bdt_train.AddClass("ele");
  bdt_train.AddClass("muon");
//  bdt_train.AddInFile("ele","eleSimFIle","eleRecoFile");
//  bdt_train.GenerateTree();
  cout<<bdt_train.GetNCLASS()<<"  "<<bdt_train.GetNVAR()<<endl;
  bdt_train.SetNSigTrain("200");
  bdt_train.SetNBkgTrain("200");
  bdt_train.SetNSigTest("200");
  bdt_train.SetNBkgTest("200");
  bdt_train.SetPruneStrengthBDT("5.0");
  bdt_train.SetNTreeBDT("125");
  bdt_train.SetBoostTypeBDT("AdaBoost");
  bdt_train.SetNCutsBDT("30");
//  bdt_train.SetNCLASS(3);
//  bdt_train.SetNVAR(3);
  bdt_train.SetINFILENAME("particle.root");
  bdt_train.SetAPPNAME("dummy");        
  bdt_train.TrainTest();
}
