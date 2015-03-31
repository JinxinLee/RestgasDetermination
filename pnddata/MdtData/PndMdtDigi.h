#ifndef PNDMDTDIGI_HH
#define PNDMDTDIGI_HH

#include "FairMultiLinkedData_Interface.h"
#include "TVector3.h"
#include <vector>

class PndMdtDigi : public FairMultiLinkedData_Interface {

 public:
 
  PndMdtDigi();
  
  PndMdtDigi (Int_t detID, TVector3& pos, std::vector<Int_t> pointList);
  
  virtual ~PndMdtDigi();
  
  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t* opt = 0) const {;}
  
  
  /** Public method Clear
   ** Resets the flag to -1
   **/
  //void Clear();  // not implemented

  /** Accessors **/
  void SetPosition(const TVector3& pos) { fLabPos = pos; };

  TVector3 GetLabPosition() const { return fLabPos; };
  Int_t GetDetectorID()     const { return fDetectorID; };  
  Short_t GetModule()       const { return (fDetectorID/1000000);};
  Short_t GetSector()       const { return ((fDetectorID/100000)%10);};
  Short_t GetLayerID()      const { return ((fDetectorID/1000)%100);};
  Short_t GetBox()          const { return ((fDetectorID/10)%100);};
  Short_t GetWire()         const { return (fDetectorID%10);};
  Short_t GetStrip()        const { return (fDetectorID%1000);};

  Short_t GetNPoints()      const { return fPointList.size(); }
  const std::vector<Int_t> &GetPointList() {return fPointList; }
 
 private:
  
  Int_t fDetectorID;                // Detectior ID
  TVector3 fLabPos;                 // Position in the la frame
  std::vector<Int_t> fPointList;    // List of MdtPoint indices
  
  ClassDef(PndMdtDigi,1);
};


#endif
