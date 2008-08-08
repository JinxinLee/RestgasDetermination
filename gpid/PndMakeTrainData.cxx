#include "PndMakeTrainData.h"

PndMakeTrainData::PndMakeTrainData()
{
 fNCLASS = 0;
}

PndMakeTrainData::~PndMakeTrainData()
{
}

void PndMakeTrainData::AddInFile(string className,string simFileName,string recoFileName)
{
 pair<string,string> stringPair;
 stringPair.first = simFileName;
 stringPair.second = recoFileName;
 cout<<"  -I-  Adding Input files for the class"<<className<<endl;
 if (fInFileNameArray.find(className) == fInFileNameArray.end()) 
  {
    vector<pair<string,string> > stringVec;
    fInFileNameArray.insert(pair<string,vector<pair<string,string> > >(className,stringVec));
    (fInFileNameArray.find(className)->second).push_back(stringPair); 
    fNCLASS = fNCLASS +1;
    fClassNameArray.push_back(className);
    cout<<"No of Classes added = "<<fNCLASS<<endl;   
  }
  else
  {
   (fInFileNameArray.find(className)->second).push_back(stringPair); 
  }
}



void PndMakeTrainData::GenerateTree()
{
 cout<<"  -I-  Generating the Input Tree"<<endl;
 system("sleep 2");
 for(int i=0; i <fNCLASS; i++)
 {
  string className;
  vector < pair<string,string> > fileNameVec;
  className = fClassNameArray[i];
  fileNameVec = fInFileNameArray.find(className)->second;
  for(int j = 0; j < fileNameVec.size();j++ )
   {
    string simFile,recoFile;
    pair<string,string> filePair;
    filePair = fileNameVec.at(j);
    cout<<filePair.first<<"  "<<filePair.second<<endl;   
   }
 }
}

