void learn_pid()
{
  //Create classifier object
  PndMultiClassMVA bdt_train;
  
  //Choose the features to be used.
  bdt_train.AddVar("stt");
  bdt_train.AddVar("emc");
  bdt_train.AddVar("p");

  //bdt_train.AddVar("mvd");
  //bdt_train.AddVar("tof");
  //bdt_train.AddVar("thetaC");

  
  //Select classes
  bdt_train.AddClass("electron");
  bdt_train.AddClass("pion");
  
  //Set number of signal and background events to be used
  bdt_train.SetNSigTrain("300");
  bdt_train.SetNBkgTrain("300");
  
  bdt_train.SetNSigTest("20");
  bdt_train.SetNBkgTest("20");
  
  //Set the classifier properties
  //BDT
  bdt_train.SetPruneStrengthBDT("5.0");
  bdt_train.SetNTreeBDT("30");
  bdt_train.SetBoostTypeBDT("AdaBoost");
  bdt_train.SetNCutsBDT("30");

  //KNN
  bdt_train.SetNKNN("10");
  bdt_train.SetKNNTreeOptDepth(6);
  bdt_train.SetKNNScaleFrac(0.8);
  bdt_train.SetKNNKernel("Trim");
  
  //MLP
  bdt_train.SetMLPNeuronType("tanh");
  bdt_train.SetNuOfCycle(10);
  bdt_train.SetNumOfHiddenLayers("N-1,N");
  bdt_train.SetTestRate(5);
  
  //Select the name of the input file which contains 
  //the training elements.
  TString InName = "/media/daq/babaiexp/VanniFiles/part_tree_clean.root";
  bdt_train.SetINFILENAME(InName);
  
  //Select application name, this is the file name where config info
  // will be recorded.
  
  bdt_train.SetAPPNAME("test");        
  bdt_train.WriteConfigFile();        
  
  //train and test the classifiers.
  //bdt_train.TrainTest();
  MVAType bla = TMKNN;
  //bdt_train.TrainClassifier(bla);
  
  bla = TMBDT;
  //bdt_train.TrainClassifier(bla);
  
  bla = TMMLP;
  //bdt_train.TrainClassifier(bla);
  
  bla = LVQ1;//MulClsKNN;//
  TString OutFile = "ZZTestOut.root";
  bdt_train.SetNumLvqProto(5);
  bdt_train.SetOutFileName(OutFile);
  bdt_train.TrainClassifier(bla);
}
