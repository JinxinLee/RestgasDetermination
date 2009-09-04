#ifndef TCFAST2DHOUGH_H
#define TCFAST2DHOUGH_H

#include "TCabsHough.h"

class TCevent;
class TCcluster;
class TVector3;
class Hyperplane2D;
class Hough2DNode;
class TH2D;

class TCfast2DHough : public TCabsHough{
 public:
  TCfast2DHough(const TVector3 firstAxis, const TVector3 secondAxis);
  TCfast2DHough(const TVector3 firstAxis, const TVector3 secondAxis, bool dynamicParSpace_=false);
  ~TCfast2DHough();
  void setParameterSpace(float m_min,float m_max, float t_min, float t_max);
  void setThreshold(int thr){THRESHOLD=thr;}
  void setDepth(int dpt){TREE_DEPTH=dpt;}
  void doHough(); 
  virtual void convert(std::vector<TCcluster>& _c);
  bool hot(int index){
    if(index<nClusters){
      if(debug){
	std::cout<<solution_list.at(index)<<" hot"<<std::endl;
      }
      return solution_list.at(index);
    }else{
      return false;
    }
  } //not valid
  int getNmax(){
    return solution_list.size();
  }
  void setDebug(bool debug_=true){
    debug=debug_;
  }
  void draw(bool stop=true,int _x=50,int _y=50,
	    int _w=600,int _h=600,TCevent* mcTruth= NULL);
  virtual void make(std::vector<TCcluster>& _c);
  void clear();
 protected:
  bool debug;
  std::vector<TBox*> boxlist;
  std::vector<Hyperplane2D*> hyperplanes;
  std::vector<bool> solution_list;
  std::vector<Hough2DNode*> solution_node_list;
  std::vector<Hough2DNode*> node_list;
  bool dynamicParSpace;
  TCanvas* canv;
  int nClusters;
  TH2D* houghSpace;
  float m_Max;
  float m_Min;
  float t_Max;
  float t_Min;
  int TREE_DEPTH;  //number of space divisions
  int THRESHOLD;
};
#endif
