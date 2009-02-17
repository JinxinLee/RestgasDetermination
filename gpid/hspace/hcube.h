#ifndef HBOX_H
#define HBOX_H

#include<iostream>
#include<vector>
using namespace std;

class hbox{
      private:
      int dim;
      vector<int> coord;
      int dummy;
      public:
      // hcube(int n);
      hbox();
      ~hbox();
      void setDummy(int dum) {dum = dummy; }
      void setCoord(const vector<int> coordinate);
      vector<int> getCoord() {return coord; }
      bool operator== ( const hbox& other);
      bool operator< (const hbox& h) const {return dummy < h.dummy; } 
};

#endif
