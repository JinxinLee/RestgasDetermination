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
  double truncMean(double trash = 0.4);
  unsigned int nEntries(){return _data.size();}
  double detdE(int i){return _data.at(i).first;}
  double getdx(int i){return _data.at(i).second;}

  void add(double dE,double dx){
	PAIR p;
	p.first = dE;
	p.second = dx;
	_data.push_back(p);
  }


private:
  std::vector<PAIR> _data;


public:
  ClassDef(PndTpcdEdx,2)

};

#endif


