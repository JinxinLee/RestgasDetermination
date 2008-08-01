/*
 *  
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 */



#include "PndGpidTask.h"
#include "Event.h"
#include <iostream>
using namespace std;

PndGpidTask::PndGpidTask()
{
}

PndGpidTask::~PndGpidTask()
{
}

InitStatus PndGpidTask::Init()
{
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- PndGpidTask::Init: "
         << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }

  fEvtArray = (TClonesArray*) ioman->GetObject("Event");
  if ( ! fEvtArray ) {
    cout << "-W- PndGpidTask::Init: "
         << "No SamEvt array!" << endl;
    return kERROR;
  }
Config();
AddVar();
BookingMVA();

 return kSUCCESS;
}
void PndGpidTask::Config()
{
  ifstream inFile;
 string sbuff,sfName,small_buff,vName;
 char buff[512];
 sfName = fAPPNAME + ".dat";
 inFile.open(sfName.c_str(),ios::in);
 inFile.getline(buff,512);
 fNVAR = (int) atoi(buff);
 inFile.getline(buff,512);
 fNCLASS = (int) atoi(buff);
 inFile.getline(buff,512);
 
 cout<<fNVAR<<endl;
 cout<<fNCLASS<<endl;
 sbuff=buff;
while(1)
 {
 size_t prompt = sbuff.find(":");
 sbuff = sbuff.substr(prompt+1,sbuff.size());
 if( sbuff.size() == 0)break;
 vName = sbuff.substr(0,sbuff.find(":"));
 fVarNameArray.push_back(vName); 
 cout<<vName<<endl; 
 }
 inFile.getline(buff,512);
 sbuff=buff;
while(1)
 {
 size_t prompt = sbuff.find(":");
 sbuff = sbuff.substr(prompt+1,sbuff.size());
 if( sbuff.size() == 0)break;
 vName = sbuff.substr(0,sbuff.find(":"));
 fClassNameArray.push_back(vName); 
 cout<<vName<<endl; 
 }
}

void PndGpidTask::AddVar()
{
 for ( int i = 0 ; i < fNVAR ; i++  )
 {
  varArray[i] = 0.0;  
 }
 int count = 0;

 for ( int i = 0 ; i < fNVAR ; i++  )
 {
  TString varName,s;
  varName = fVarNameArray.at(i);
   for ( int j = 0; j < fNCLASS ; j++)
   {
    reader[j].AddVariable(varName, &(varArray[i]));  
   }
 }
}


void PndGpidTask::BookingMVA()
{
 for (int i = 0 ; i < fNCLASS ; i++  )
 {
  string anaFile;
  switch (fMVAmode)
   {
   case BDT: 
   	anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_BDT.weights.txt";
   	reader[i].BookMVA("BDT method", anaFile );
        fClassifier = "BDT method";
        break;

   case KNN: 
   	anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_KNN.weights.txt";
   	reader[i].BookMVA("KNN method", anaFile );
        fClassifier = "KNN method";
        break;

   case MLP:
   	anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_MLP.weights.txt";
   	reader[i].BookMVA("MLP method", anaFile );
        fClassifier = "MLP method";
        break;
   }
 }
}


void PndGpidTask::Exec(Option_t* opt)
{
  for (Int_t i=0; i<fEvtArray->GetEntriesFast(); i++)
  {
  Event*  events = (Event *) fEvtArray->At(i);
   
 for (int i = 0 ; i< fNVAR; i++)
   {
   string varName;
   varName = fVarNameArray.at(i);
   varArray[i]=events->Get(varName);
   cout<<varArray[i]<<" "<<varName<<endl;
   }

   for (int i = 0 ; i < fNCLASS; i++)
   {
   cout<<reader[i].EvaluateMVA(fClassifier)<<endl;
   }
  }
}
ClassImp(PndGpidTask);
