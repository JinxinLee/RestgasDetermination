#ifndef SPACE_H
#define SPACE_H

#include <set>
#include <iostream>
#include "hcube.h"
#include "dim.h"

using namespace std;

class space {
  private:
   vector<hcube> fSPACE;
   vector<dim> fDIM;
   int nNDIM;
  public:
   space();
   ~space();
 //  void setNDIM(int ndim) { nNDIM = ndim; }
   int getNDIM() {return nNDIM; }
   void AddDim(dim D);
   void CreateSpace();
   void CS();
};

#endif
