//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class McIdCollection
//      see McIdCollection.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "McIdCollection.h"

// C/C++ Headers ----------------------
#include <algorithm>

using std::vector;

// Collaborating Class Headers --------


// Class Member definitions -----------

ClassImp(McIdCollection)

McIdCollection::McIdCollection(){;}

McIdCollection::~McIdCollection()
{
  fMCIDs.clear();
}

McIdCollection::McIdCollection(const McIdCollection& coll)
 : fMCIDs(coll.fMCIDs)
{}


bool
McIdCollection::AddID(const McId& ID, double w){
  vector<McId>::iterator it=find(fMCIDs.begin(),fMCIDs.end(),ID);
  if(it==fMCIDs.end()){
    fMCIDs.push_back(ID);  
    fMCIDs.back().setweight(ID.weight()*w);
    return true;
  }
  else {
    (*it).IncWeight(ID.weight()*w);
    return false;
  }
};


bool
McIdCollection::AddIDCollection(const McIdCollection& coll, double w){
  unsigned int n=coll.nIDs();
  bool ret=false;
  for(unsigned int i=0; i<n; ++i)ret|=AddID(coll.ID(i),w);
  return ret;
}


double
McIdCollection::TotalWeight() const {
  unsigned int n=nIDs();
  double result=0;
  for(unsigned int i=0; i<n;++i){
    result+=fMCIDs[i].weight();
  }
  return result;
}


double
McIdCollection::Renormalize(){
  double tot=TotalWeight();
  unsigned int n=nIDs();
  for(unsigned int i=0; i<n;++i){
    fMCIDs[i].setweight(fMCIDs[i].weight()/tot);
  }
}

double
McIdCollection::MaxRelWeight() const {
  double result=0;
  double total=0;
  unsigned int n=nIDs();
  for(unsigned int i=0; i<n;++i){
    double w=fMCIDs[i].weight();
    result=std::max(result,w);
    total+=w;
  }
  if(total==0)return 1.05;
  else return result/total;
}

McId 
McIdCollection::DominantID() const {
  unsigned int n=nIDs();
  unsigned int index=0;
  double maxw=0;
  for(unsigned int i=0; i<n;++i){
    double w=fMCIDs[i].weight();
    if(maxw<w){
      maxw=w;
      index=i;
    }
  }
  return fMCIDs[index];
}

std::ostream& operator<< (std::ostream& s, const McIdCollection& me){
  s << "MCID-Collection:\n";
  for(unsigned int i=0; i<me.fMCIDs.size(); ++i)s<<me.fMCIDs[i]<<std::endl;
  return s;
}
