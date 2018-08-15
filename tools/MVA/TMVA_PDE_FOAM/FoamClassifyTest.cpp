#include "PndPDEFoamClassify.h"

int main()
{
  std::vector<std::string> clas;
  std::vector<std::string> name;

  // Classes (container to hold the class names)
  clas.push_back("Elect"); clas.push_back("Pion"); 
  //clas.push_back("Kaon"); //clas.push_back("Gamma"); 
  //clas.push_back("Muon"); clas.push_back("Proton");
  
  // Event structure
  name.push_back("p"); name.push_back("emc");
  name.push_back("thetaC"); name.push_back("mvd");
  name.push_back("tof");
 
  PndPDEFoamClassify bb ("../TrainProtoFiles/UniformFoam.root", clas, name);
  std::vector<float>evt(5 , 0.3);
  std::map<std::string, float> res;
  bb.GetMvaValues(evt, res);
  return 0;
}
/*
  void PndPDEFoamTrain::TestFoam()
  {
  const std::vector< std::pair<std::string, std::vector<float>*> >& EvtData = m_dataSets.GetData();
  int ra = EvtData.size() - 30;
  std::vector<float>* evtVect = EvtData[ra].second;
  std::string name = EvtData[ra].first;
  
  double density_sig, density_bg = 0.0;
  
  TMVA::EKernel kk = TMVA::EKernel(0);
  density_sig = m_foams[0]->GetCellDensity(*evtVect, kk);
  density_bg  = m_foams[1]->GetCellDensity(*evtVect, kk);
  
  std::cout << " name is " << name 
  << "\n Density Sig = " << density_sig 
  << " With foam " <<  m_foams[0]->GetFoamName() 
  << std::endl 
  << " Density BG = " << density_bg 
  << " With foam " <<  m_foams[1]->GetFoamName()
  << std::endl;
  double disc = 0.0;
  if( (density_sig+density_bg) > 0 )
  disc = density_sig/(density_sig+density_bg);
  else
  disc = 0.5;
  std::cout << "Disc is " << disc << " Voor " << std::endl;
  
  // READ the foam
  
  TFile rf ("TestFoam.root", "READ");
  TMVA::PDEFoam* ff = (TMVA::PDEFoam*) rf.Get("PionFoam");
  std::cout << ff->GetNCells() << std::endl;
  rf.Close();
  
  }
  void PndPDEFoamTrain::TestFoam()
  {
  const std::vector< std::pair<std::string, std::vector<float>*> >& EvtData = m_dataSets.GetData();
  int ra = EvtData.size() - 30;
  std::vector<float>* evtVect = EvtData[ra].second;
  std::string name = EvtData[ra].first;
  
  double density_sig, density_bg = 0.0;
  
  TMVA::EKernel kk = TMVA::EKernel(0);
  density_sig = m_foams[0]->GetCellDensity(*evtVect, kk);
  density_bg  = m_foams[1]->GetCellDensity(*evtVect, kk);
  
  std::cout << " name is " << name 
  << "\n Density Sig = " << density_sig 
  << " With foam " <<  m_foams[0]->GetFoamName() 
  << std::endl 
  << " Density BG = " << density_bg 
  << " With foam " <<  m_foams[1]->GetFoamName()
  << std::endl;
  double disc = 0.0;
  if( (density_sig+density_bg) > 0 )
  disc = density_sig/(density_sig+density_bg);
  else
  disc = 0.5;
  std::cout << "Disc is " << disc << " Voor " << std::endl;
  
  // READ the foam
  
  TFile rf ("TestFoam.root", "READ");
  TMVA::PDEFoam* ff = (TMVA::PDEFoam*) rf.Get("PionFoam");
  std::cout << ff->GetNCells() << std::endl;
  rf.Close();
  
  }
*/
