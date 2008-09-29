#include "PndKnnClassify.h"
#include "TRandom3.h"
#include "TStopwatch.h"
#include <stdlib.h>

int main(int argc, char** argv)
{
  char name[64];

  TRandom3 myran(NULL);

  if (!(argc==6))
    {
      std::cout << "Please specify the arguments: " << std::endl;
      std::cout << argv[0] << " <nrofevents> <nrofclasses> <nroffeatures> <nrofneighbours> <inputfile>" << std::endl;
      exit(-1);
    }

  int totEvtNum = atoi(argv[1]);
  int nrOfClasses = atoi(argv[2]);
  int nrOfFeatures = atoi(argv[3]);
  int nrOfNeighbours= atoi(argv[4]);

  std::cout << "Nr of Events     = " << totEvtNum << std::endl;
  std::cout << "Nr of Classes    = " << nrOfClasses << std::endl;
  std::cout << "Nr of Features   = " << nrOfFeatures << std::endl;
  std::cout << "Nr of Neighbours = " << nrOfNeighbours << std::endl;
  std::cout << "Input filename   = " << argv[5] << std::endl;

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
  
  PndKnnClassify cls (argv[5],clas,nam);
  
  std::vector<float> evt;
  std::map<std::string, float> res;

  TStopwatch timer;
  timer.Start();

  for (int i=0; i<totEvtNum; i++)
    {
      evt.clear();
      res.clear();
      for (int j=0; j<nrOfFeatures; j++)
	{
	  evt.push_back(myran.Uniform(0,1));
	  std::cout << evt[j] << " " ;
	}
      std::cout << std::endl;

      cls.Classify(evt,nrOfNeighbours,res);

      for( std::map<std::string,float>::iterator ii=res.begin();ii != res.end(); ++ii)
	{                                                            
	  std::cout << (*ii).first << ": " << (*ii).second << std::endl;
	} 
    }
  timer.Stop();
  std::cout << "Cpu Time=" << timer.CpuTime() << " Real Time=" << timer.RealTime() << std::endl;
  return 0;
}

