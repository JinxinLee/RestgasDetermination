#ifndef SPACE_H
#define SPACE_H

#include <set>
#include <iostream>
#include <vector>
#include <map>
#include "hcube.h"
#include "dim.h"

using namespace std;

class space {
  private:
//   set<hbox> fSPACE;
   map<vector<int>,hbox> fSPACE;
   vector<dim> fDIM;
   vector<vector <int> >myspace;
   int nNDIM;
  public:
   space();
   ~space();
 //  void setNDIM(int ndim) { nNDIM = ndim; }
   int getNDIM() {return nNDIM; }
   void AddDim(const dim D);
   void CS();
   void print();
};

#endif
