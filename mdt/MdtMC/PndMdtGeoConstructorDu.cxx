#include <iostream>


#include "PndMdt.h"

using namespace std;


// -----   Public method ConstructGeometry   ----------------------------------
void PndMdt::ConstructGeometryDu() 
{
  TString fileName=GetGeometryFileName();
  if(fileName.EndsWith(".root"))
    {
      ConstructRootGeometry();
    }
  else
    {
      std::cout<< "Geometry format not supported " <<std::endl;
    }
}


// ----------------------------------------------------------------------------

ClassImp(PndMdt)
