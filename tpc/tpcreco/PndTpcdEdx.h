#ifndef PND2TPCDEDX_HH
#define PND2TPCDEDX_HH


#include "TObject.h"
#include<vector>
#include<utility>


typedef std::pair<double,double> PAIR;

class PndTpcdEdx : public TObject {
public:



  PndTpcdEdx(){}

  double simpleMean();
  double truncMean();
  void add(double dE,double dx){
	PAIR p;
	p.first = dE;
	p.second = dx;
	_data.push_back(p);
  }


private:
  std::vector<PAIR> _data;


public:
  ClassDef(PndTpcdEdx,1)

};

#endif


