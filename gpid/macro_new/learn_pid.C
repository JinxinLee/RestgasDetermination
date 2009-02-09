void learn_pid()
{
  //Create classifier object
  PndMultiClassMVA bdt_train;
  
  //Choose the features to be used.
  bdt_train.AddVar("stt");
  bdt_train.AddVar("p");

  //bdt_train.AddVar("mvd");
  //bdt_train.AddVar("tof");
  
  //bdt_train.AddVar("thetaC");
  bdt_train.AddVar("emc");
  
  //Select classes
  bdt_train.AddClass("electron");
  bdt_train.AddClass("pion");
  
  //Set number of signal and background events to be used
  bdt_train.SetNSigTrain("1000");
  bdt_train.SetNBkgTrain("1000");
  bdt_train.SetNSigTest("50");
  bdt_train.SetNBkgTest("50");
  
  //Set the classifier properties
  bdt_train.SetPruneStrengthBDT("5.0");
  bdt_train.SetNTreeBDT("50");
  bdt_train.SetBoostTypeBDT("AdaBoost");
  bdt_train.SetNCutsBDT("30");
  bdt_train.SetNKNN("100");
  
  //MLP
  bdt_train.SetMLPNeuronType("tanh");
  bdt_train.SetNuOfCycle(10);
  //bdt_train.SetNumOfHiddenLayers(5);
  bdt_train.SetTestRate(5);
  
  //  bdt_train.SetNCLASS(3);
  //  bdt_train.SetNVAR(3);
  //Select the name of the input file which contains 
  //the training elements.
  bdt_train.SetINFILENAME("/media/daq/babaiexp/VanniFiles/part_tree_clean.root");
  
  //  bdt_train.SetAPPNAME("STT_P_MVD_TOF_THETAC_EMC_1_2_1000NN");        
  //Select application name, this is the file name where config info
  // will be recorded.
  bdt_train.SetAPPNAME("test");        
  bdt_train.WriteConfigFile();        
  //train and test the classifiers.
  bdt_train.TrainTest();
}
