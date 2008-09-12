#ifndef TCTRACK_H
#define TCTRACK_H

#include<iostream>
#include<string>
#include<vector>
#include<cmath>
#include<algorithm>

#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TCcluster.h"
#include "TCclusterSort.h"

#define PRINTF(X) printf("%s=%10.10E\n",#X,(X));
#define PRINTI(X) printf("%s=%d\n",#X,(X));


class TCtrack : public TObject{
 public:
  TCtrack(); 
  ~TCtrack();
  bool fit(int id1,int id2=-1,int id3=-1,int id4=-1,int id5=-1,int id6=-1,int id7=-1,int id8=-1,int id9=-1,int id10=-1);
  void addCluster(TCcluster& c){
	cl.push_back(c);
  }
  void addClusters(const std::vector<TCcluster>& c){
	cl=c;
  }

  unsigned int nCl(){
	return cl.size();
  }
  unsigned int nClFit(){
    unsigned int ret=0;
    for(int i=0;i<nCl();++i){
      if(cl.at(i).getFit()) ++ret;
    }
    return ret;
  }

  TCcluster getCl(int i){
	if(i<nCl()){
	  return cl.at(i);
	}
	else{
	  std::cerr << "TCtrack::getCl(): id " << i 
				<< " is out of range ->exc" << std::endl;
	  std::string exc("TCtrack::getCl(): id out of range");
	  throw exc;
	}
  }

  double getThX(){
	return 180.*atan(ax)/TMath::Pi();
  }
  double getThY(){
	return 180.*atan(ay)/TMath::Pi();
  }
  double getTh(){
	return  180.*atan(sqrt(ay*ay+ax*ax))/TMath::Pi();
  }
  double getMCThX(){
	return 180.*atan(MCax)/TMath::Pi();
  }
  double getMCThY(){
	return 180.*atan(MCay)/TMath::Pi();
  }
  double getMCTh(){
	return  180.*atan(sqrt(MCay*MCay+MCax*MCax))/TMath::Pi();
  }

  void setPar(double _ax,double _bx,double _ay,double _by){
    ax=_ax;bx=_bx;ay=_ay;by=_by;
  }
  void setMCPar(double _ax,double _bx,double _ay,double _by){
    isMC=true;
	MCax=_ax;MCbx=_bx;MCay=_ay;MCby=_by;
  }

  double getAx(){return ax;}
  double getBx(){return bx;}
  double getAy(){return ay;}
  double getBy(){return by;}
  double getMCAx(){return MCax;}
  double getMCBx(){return MCbx;}
  double getMCAy(){return MCay;}
  double getMCBy(){return MCby;}
  double getDax(){return dax;}
  double getDbx(){return dbx;}
  double getDay(){return day;}
  double getDby(){return dby;}
  double getChi2(){return chi2;}
  int getNDF(){return NDF;}

  void draw(bool stop=true,int _x=50,int _y=50,int _w=600,int _h=600);
  void drawClustering(bool stop=true,int _x=50,int _y=50,int _w=600,int _h=600);
  void clear(){
    isMC=false;
    cl.clear();
    ax=-1.E10;bx=-1.E10;ay=-1.E10;by=-1.E10;
    MCax=-1.E10;MCbx=-1.E10;MCay=-1.E10;MCby=-1.E10;
    dax=-1.E10;dbx=-1.E10;day=-1.E10;dby=-1.E10;
    chi2=-1.E10;
  }

  void useTrivialClustering();
  void sort(){
    std::sort(cl.begin(),cl.end(),TCclusterSortXYZ());
  }

  void setCustomRange(bool flag,
		      double x1=0.,double x2=0.,double y1=0.,
		      double y2=0.,double z1=0.,double z2=0.){
    rangeX1=x1;rangeX2=x2,rangeY1=y1,rangeY2=y2;rangeZ1=z1;rangeZ2=z2;
    customRange=flag;
  }

  void print(){
    std::cout << "--------TCtrack::print()-------" << std::endl;
    for(unsigned int i=0;i<nCl();++i) cl.at(i).print();
    PRINTF(ax);PRINTF(bx);PRINTF(ay);PRINTF(by);PRINTF(dax);PRINTF(dbx);PRINTF(day);PRINTF(dby);PRINTF(chi2);PRINTI(NDF);PRINTF(getTh());PRINTF(getThX());PRINTF(getThY());
    std::cout << "--------================-------" << std::endl;
  }

 private:
  TCanvas *canvDraw;
  TCanvas *canvDrawClust;
  double ax,bx,ay,by,dax,day,dbx,dby;
  double MCax,MCbx,MCay,MCby;
  bool isMC;
  double chi2;
  int NDF;
  std::vector<TCcluster> cl;
  
  //options for drawing
  bool customRange;
  double  rangeX1,rangeX2,rangeY1,rangeY2,rangeZ1,rangeZ2;

 public:
  ClassDef(TCtrack,1)
};
	
#endif
