#ifndef CLUSTERSPLIT1_C
#define CLUSTERSPLIT1_C

#include "../src/TCcluster.h"
#include "../src/TCclusterSort.h"
#include "../src/TCtrack.h"
#include <algorithm>
#include <vector>
#include <set>

//pitches
#define UPITCH 6200 //integer in microns

TCtrack clusterSplit1(TCtrack t){

  std::vector<TCcluster> tpc;//will hold clusters in TPC
  std::vector<TCcluster> others;//will hold clusters NOT in TPC
  std::vector<TCcluster> output;
  std::cout << "t.Cl(): " << t.nCl() << std::endl;
  for(int i=0;i<t.nCl();++i){
    TCcluster c = t.getCl(i);
    std::cout << "c.getId(): " << c.getId() << std::endl;
    if(c.getId()<=200&&c.getId()<=299){
      tpc.push_back(c);
    }
    else{
      others.push_back(c);
    }
  }

  for(unsigned int i=0;i<others.size();++i){
    output.push_back( others.at(i) );
  }
  others.clear();//not needed anymore ->destruct these copies

  std::cout << "tpc.size() : " << tpc.size() << std::endl;


  //loop over clusters in tpc
  for(unsigned int icl=0;icl<tpc.size();++icl){
    //will hold split clusters from this cluster tpc.at(icl)
    std::vector< std::vector<TCcluster> > split;
    //extract raw clusters
    std::vector<TCcluster> raw;
    for(unsigned int iraw=0;iraw<tpc.at(icl).nRaw();++iraw){
      raw.push_back(tpc.at(icl).getRaw(iraw));
    }
    //sort the raw clusters in U
    sort(raw.begin(),raw.end(),TCclusterSortU());

    
    std::vector<TCcluster> buf;
    buf.push_back( raw.at(0) );
    std::cout << "raw.size() : " << raw.size() << std::endl;
    if(raw.size()>1){//more than one raw cluster, so sort in padrows
      std::cout << "## " << buf.at(0).posUVW().X() << std::endl;
      for(int iraw=1;iraw<raw.size();++iraw){
	std::cout << "##$$ " << raw.at(iraw).posUVW().X() << std::endl;
	std::cout << "ALPHA" << std::endl;
	
	if(fabs(buf.at(0).posUVW().X()-raw.at(iraw).posUVW().X())<1.E-5){//same U pas row
	  std::cout << "A" << std::endl;
	  buf.push_back(raw.at(iraw));

	  //if this is the last raw, also push_back onto split
	  if(iraw==(raw.size()-1)){
	    std::cout << "A1" << std::endl;
	    std::cout << "push_back || buf size: " << buf.size() << std::endl;
	    split.push_back(buf );
	    buf.clear();
	  }
	}
	else{//next U pad row
	  std::cout << "B" << std::endl;
	  //push the buffer on the split vector because this pad row is finished
	  split.push_back(buf);
	  std::cout << "push_back || buf size: " << buf.size() << std::endl;
	  buf.clear();
	  buf.push_back(raw.at(iraw));
	  //if this is the last raw, also push_back onto split
	  if(iraw==(raw.size()-1)){
	    std::cout << "B1" << std::endl;
	    std::cout << "push_back || buf size: " << buf.size() << std::endl;
	    split.push_back(raw);
	    buf.clear();
	  }

	}
      }
    }
    else{//only one raw cluster in this cluster
      split.push_back(buf);
      std::cout << "C" << std::endl;
      std::cout << "push_back || buf size: " << buf.size() << std::endl;
    }
    std::cout << "split.size() : " << split.size() << std::endl;
    for(unsigned int isplit=0;isplit<split.size();++isplit){
      for(int i=0;i<split.at(isplit).size();++i){
	std::cout << "@@@@ " << split.at(isplit).at(i).getId() << std::endl;
      }
      TCcluster arow(split.at(isplit) );
      arow.setFit(tpc.at(icl).getFit());
      output.push_back(arow);
    }
  }


//   for(unsigned int isplit=0;isplit<split.size();++isplit){
//     TCcluster arow(split.at(isplit) );
//     output.push_back(arow);
//   }
  TCtrack returnTrack;
  returnTrack.addClusters(output);
  std::cout << "returnTrack.nCl(): " << returnTrack.nCl() << std::endl;
  return returnTrack;
  
  

  /*
  std::set<int> uvals;
  std::set<int> vvals;

  for(int i=0;i<tpc.size();++i){
    double u,v;
    u=tpc.at(i).posUVW().X();
    v=tpc.at(i).posUVW().Y();
    uvals.insert( (int) (u*10000));
    vvals.insert( (int) (v*10000));
  }

  bool first;

  int lastu;
  first = true;
  for(std::set<int>::iterator it=uvals.begin();it!=uvals.end();++it){
    if(first){
      first=false;
      lastu=(*it)-UP;
    }
    if(((*it)-lastu) > (UP+2)) {
      return false;
    }
    lastu=*it;
  }
  int lastv;
  first = true;
  for(std::set<int>::iterator it=vvals.begin();it!=vvals.end();++it){
    if(first){
      first=false;
      lastv=(*it)-VP;
    }
    if(((*it)-lastv) > (VP+2)) {
      return false;
    }
    lastv=*it;
  }

  return true;
  */
}


#endif
