#ifndef HCUBE_H
#define HCUBE_H

#include<iostream>
#include<vector>
using namespace std;

class hcube{
      private:
      int dim;
      vector<int> coord;
      int dummy;
      public:
      // hcube(int n);
      hcube();
      ~hcube();
      void setDummy(int dum) {dum = dummy; }
      void setCoord(const vector<int> coordinate);
      vector<int> getCoord() {return coord; }
      bool operator== ( const hcube& other);
      bool operator< (const hcube& h) const {return dummy < h.dummy; } 
};

#endif
