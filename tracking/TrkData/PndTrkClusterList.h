/** PndTrkClusterList
 ** Class for pattern recognition ClusterList
 ** @author Lia Lavezzi 
 **
 ** fSensorID: identifies the specific sensor of the MVD, 
               the specific tube of the STT
 **/

#ifndef PNDTRKCLUSTERLIST_H
#define PNDTRKCLUSTERLIST_H 1

#include "TVector3.h"
#include "PndTrkCluster.h"

class TClonesArray;
class PndTrkClusterList : public TObject
{

 public:    

  PndTrkClusterList();
 // copy ctor
  PndTrkClusterList(const PndTrkClusterList& clist);
  ~PndTrkClusterList();    
  PndTrkClusterList& operator=(const PndTrkClusterList &clist);

  void AddCluster(PndTrkCluster *cluster) ;
  void DeleteCluster(Int_t index) ;
  void Reset();
  void Clear(Option_t* opt = "");

  inline Int_t  GetNofClusters() { return fClusterList.GetEntriesFast(); }

  PndTrkCluster * GetCluster(Int_t index) { return (PndTrkCluster*) fClusterList[index]; }
 
/*   void ReplaceCluster(Int_t index, PndTrkCluster *cluster); */
   
 protected:
 
  TClonesArray fClusterList;
  // TObjArray clusterlist;
  // std::vector< PndTrkCluster > clusterlist;

  ClassDef(PndTrkClusterList,1);
};


#endif
