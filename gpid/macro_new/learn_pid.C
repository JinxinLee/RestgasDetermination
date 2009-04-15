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
  bdt_train.AddVar("p");
  bdt_train.AddVar("emc");
  bdt_train.AddVar("stt");
  bdt_train.AddVar("mvd");
  bdt_train.AddVar("tof");
  bdt_train.AddVar("thetaC");
  
  // Select classes
  bdt_train.AddClass("Elect");
  bdt_train.AddClass("Pion");
  bdt_train.AddClass("Kaon");
  bdt_train.AddClass("Muon");
  bdt_train.AddClass("Gamma");
  bdt_train.AddClass("Proton");
  
  // Set number of signal and background events to be used
  bdt_train.SetNSigTrain("20000");
  bdt_train.SetNBkgTrain("80000");
  
  bdt_train.SetNSigTest("100");
  bdt_train.SetNBkgTest("100");
  
  // Set the classifier properties
  // BDT
  bdt_train.SetPruneStrengthBDT("5.0");
  bdt_train.SetNTreeBDT("40");
  bdt_train.SetBoostTypeBDT("AdaBoost");
  bdt_train.SetNCutsBDT("30");

  // KNN
  bdt_train.SetNKNN("150");
  bdt_train.SetKNNTreeOptDepth(6);
  bdt_train.SetKNNScaleFrac(0.8);
  //bdt_train.SetKNNSelOpt("Trim");
  
  // MLP
  bdt_train.SetMLPNeuronType("tanh");
  bdt_train.SetNuOfCycle(200);
  bdt_train.SetNumOfHiddenLayers("N-1,N");
  bdt_train.SetTestRate(5);
  
  //Select the name of the input file which contains 
  //the training elements.
  TString InName = "../Gpid_files/EventFeaturesTrain.root";
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
  
  bla = LVQ1;//LVQ1
  TString OutFile = "LVQ1TestOut.root";
  bdt_train.SetNumLvqProto(30);
  bdt_train.SetOutFileName(OutFile);
  bdt_train.SetLearnPrameters(0.2,0.1,0.00001,1000);
  //bdt_train.TrainClassifier(bla);

  bla = LVQ21;//LVQ2.1
  TString OutFile1 = "LVQ2TestOut.root";
  bdt_train.SetNumLvqProto(30);
  bdt_train.SetOutFileName(OutFile1);
  bdt_train.SetLearnPrameters(0.5, 0.1, 0.00001, 1000);
  bdt_train.TrainClassifier(bla);
}
