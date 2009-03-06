#ifndef TCABSHOUGH_H
#define TCABSHOUGH_H

#include<vector>
#include<cassert>
#include"TVector3.h"
#include"TMatrixT.h"
#include"TCcluster.h"

class TCabsHough{

 public:
  TCabsHough(const TVector3& _yp,const TVector3& _zp);
  virtual ~TCabsHough();

  virtual void convert(std::vector<TCcluster>& _c);
  virtual void doHough()=0;

  void make(std::vector<TCcluster>& _c){
	convert(_c);
	doHough();
  }

  virtual bool hot(int index)=0;

  virtual void draw(bool stop=true,int _x=50,int _y=50,int _w=600,int _h=600)=0;

  void clear(){
	ypHit.clear();
	zpHit.clear();
  }

  unsigned int nHits(){
	assert(ypHit.size()==zpHit.size());
	return ypHit.size();
  }
  


 protected:
  TVector3 yp,zp;
 
  std::vector<double> ypHit;
  std::vector<double> zpHit;
};







#endif
