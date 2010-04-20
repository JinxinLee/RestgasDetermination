#include "PndKnnTrain.h"
#include "TRandom3.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  char name[64];

  if (!(argc==6))
    {
      std::cout << "Please specify the arguments: " << std::endl;
      std::cout << argv[0] 
		<< " <nrofevents> <nrofclasses> <nroffeatures>"
		<<" <outputfile> <seed>" << std::endl;
      exit(-1);
    }
  TRandom3 myran(atoi(argv[5]));

  int totEvtNum = atoi(argv[1]);
  int nrOfClasses = atoi(argv[2]);
  int nrOfFeatures = atoi(argv[3]);

  std::cout << "Nr of Events    = " << totEvtNum << std::endl;
  std::cout << "Nr of Classes   = " << nrOfClasses << std::endl;
  std::cout << "Nr of Features  = " << nrOfFeatures << std::endl;
  std::cout << "Output filename = " << argv[4] << std::endl;
  
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  for (int i=0; i<nrOfClasses; i++)
    {
      sprintf(name,"class%d",i);
      clas.push_back(name);
    }
  for (int i=0; i<nrOfFeatures; i++)
    {
      sprintf(name,"par%d",i);
      nam.push_back(name);
    }
 
  std::string ot   = argv[4];
  const char* file = ot.c_str();
  
  PndKnnTrain bla (file,clas,nam);
  
  std::vector<float> evt;
  
  for(int i=0; i < totEvtNum; i++)
    {
    for (int j=0; j<nrOfClasses; j++)
      {
	evt.clear();
	for (int k=0; k<nrOfFeatures; k++)
	  {
	    evt.push_back(myran.Uniform(0,1));
	  }
	sprintf(name,"class%d",j);
	bla.Train(evt,name);
      }
  }
  return 0;
}

