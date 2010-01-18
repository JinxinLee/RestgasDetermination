#ifndef PNDLUMICLUSTER_H
#define PNDLUMICLUSTER_H

#include "TObject.h"
#include <vector>
#include <iostream>
#include "PndDetectorList.h" // contains SensorSide enumeration


//! PndLumiCluster.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief Class to store the Digis which belong to one cluster
//! This class holds the information which Digi belongs to the actual cluster.
//! The information is stored in a vector<Int_t> which contains the
//! position of the digi in the TClonesArray where it is stored.
class PndLumiCluster : public TObject
{

friend std::ostream& operator<< (std::ostream& out, PndLumiCluster& cl){
      std::vector<Int_t> list = cl.GetClusterList();
      out << "Hits in Cluster:" << std::endl;
      for (Int_t i = 0; i<list.size();i++)
        out << list[i] << std::endl;
      out << std::endl;

      return out;
    }

public :
    PndLumiCluster():fSide(kTOP){};
    ~PndLumiCluster(){};
    PndLumiCluster(std::vector<Int_t> list);

    void SetClusterList(std::vector<Int_t> list) {fClusterList = list;}
    std::vector<Int_t> GetClusterList() const {return fClusterList;}
    Int_t GetClusterSize() const {return fClusterList.size();}
    Int_t GetDigiIndex(Int_t i) const {return fClusterList[i];}

    SensorSide GetSensorSide() const {return fSide;}
    void SetSensorSide(SensorSide s) {fSide = s;}
    bool DigiBelongsToCluster(Int_t digiIndex);

    void Print();

private :
    std::vector<Int_t> fClusterList;
    SensorSide fSide;


ClassDef(PndLumiCluster,1);

};

#endif
