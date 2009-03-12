/*
 * Macro to use for selecting and training classifiers. 
 *
 * Created by:
 * S.Vanniarajan
 * Modified:
 * M.Babai
 */
void learn_pid()
{
  // Create classifier object
  PndMultiClassMVA bdt_train;
  
  // Choose the features (parameters) to be used.
  bdt_train.AddVar("stt");
  bdt_train.AddVar("emc");
  bdt_train.AddVar("p");

  //bdt_train.AddVar("mvd");
  //bdt_train.AddVar("tof");
  //bdt_train.AddVar("thetaC");

  
  // Select classes
  bdt_train.AddClass("electron");
  bdt_train.AddClass("pion");
  
  // Set number of signal and background events to be used
  bdt_train.SetNSigTrain("30000");
  bdt_train.SetNBkgTrain("30000");
  
  bdt_train.SetNSigTest("200");
  bdt_train.SetNBkgTest("200");
  
  // Set the classifier properties
  // BDT
  bdt_train.SetPruneStrengthBDT("5.0");
  bdt_train.SetNTreeBDT("50");
  bdt_train.SetBoostTypeBDT("AdaBoost");
  bdt_train.SetNCutsBDT("30");

  // KNN
  bdt_train.SetNKNN("100");
  bdt_train.SetKNNTreeOptDepth(6);
  bdt_train.SetKNNScaleFrac(0.8);
  bdt_train.SetKNNKernel("Trim");
  
  // MLP
  bdt_train.SetMLPNeuronType("tanh");
  bdt_train.SetNuOfCycle(200);
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
  
  // Select which classifier to train.
  /*
   * Possible MVA's are:
   * TMKNN, TMBDT, TMMLP, MulClsKNN, LVQ1, LVQ21
   */
  MVAType bla = TMKNN;
  //bdt_train.TrainClassifier(bla);
  
  bla = TMBDT;
  //bdt_train.TrainClassifier(bla);
  
  bla = TMMLP;
  //bdt_train.TrainClassifier(bla);
  
  bla = LVQ21;//LVQ1;
  TString OutFile = "LVQ2TestOut.root";
  bdt_train.SetNumLvqProto(30);
  bdt_train.SetOutFileName(OutFile);
  bdt_train.TrainClassifier(bla);
}
