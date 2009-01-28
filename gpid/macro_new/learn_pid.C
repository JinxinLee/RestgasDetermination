void learn_pid()
{
PndMultiClassMVA bdt_train;
//bdt_train.AddVar("stt");
bdt_train.AddVar("p");


//bdt_train.AddVar("mvd");
//bdt_train.AddVar("tof");

//bdt_train.AddVar("thetaC");
bdt_train.AddVar("emc");

bdt_train.AddClass("electron");
bdt_train.AddClass("pion");

 bdt_train.SetNSigTrain("1500");
  bdt_train.SetNBkgTrain("1500");
  bdt_train.SetNSigTest("2");
  bdt_train.SetNBkgTest("2");
  bdt_train.SetPruneStrengthBDT("5.0");
  bdt_train.SetNTreeBDT("50");
  bdt_train.SetBoostTypeBDT("AdaBoost");
  bdt_train.SetNCutsBDT("30");
  bdt_train.SetNKNN("19");
//  bdt_train.SetNCLASS(3);
//  bdt_train.SetNVAR(3);
  bdt_train.SetINFILENAME("/home/vanni/work/pid_ana/part_tree_clean.root");
//  bdt_train.SetAPPNAME("STT_P_MVD_TOF_THETAC_EMC_1_2_1000NN");        
  bdt_train.SetAPPNAME("test1");        
  bdt_train.WriteConfigFile();        
  bdt_train.TrainTest();
}
