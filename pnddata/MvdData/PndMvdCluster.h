#ifndef PNDMVDCLUSTER_H
#define PNDMVDCLUSTER_H

//#include "FairMultiLinkedData.h"
#include "PndDetectorList.h" // contains SensorSide enumeration

#include "FairMultiLinkedData.h"

#include "TObject.h"
#include <vector>
#include <iostream>



//! PndMvdCluster.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief Class to store the Digis which belong to one cluster
//! This class holds the information which Digi belongs to the actual cluster.
//! The information is stored in a vector<Int_t> which contains the
//! position of the digi in the TClonesArray where it is stored.
class PndMvdCluster : public FairMultiLinkedData
{

friend std::ostream& operator<< (std::ostream& out, PndMvdCluster& cl){
      std::vector<Int_t> list = cl.GetClusterList();
      out << "Hits in Cluster:" << std::endl;
      for (unsigned int i = 0; i<list.size();i++)
        out << list[i] << std::endl;
      out << std::endl;

      return out;
    }

public :
    PndMvdCluster(){};
    ~PndMvdCluster(){};
    PndMvdCluster(std::vector<Int_t> list);

    virtual void SetClusterList(std::vector<Int_t> list) = 0;
    std::vector<Int_t> GetClusterList() const {return fClusterList;}
    Int_t GetClusterSize() const {return fClusterList.size();}
    Int_t GetDigiIndex(Int_t i) const {return fClusterList[i];}

    bool DigiBelongsToCluster(Int_t digiIndex);

    virtual void Print();

protected :
    std::vector<Int_t> fClusterList;


ClassDef(PndMvdCluster,2);

};

#endif
