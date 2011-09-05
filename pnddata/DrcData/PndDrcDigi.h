
/** PndGemDigi.h
 ** Data class for digital STS information
 ** 
 ** The index of the (first) MCPoint having activated this channel
 ** is accessible via ???? GetMcPointIndex(). 
 **/


#ifndef PNDDRCDIGI_H
#define PNDDRCDIGI_H 1

#include <vector>

#include "FairMultiLinkedData.h"

#include "PndDetectorList.h"

#include "TObject.h"


// copied from GEM C.S. 5.9.2011

class PndDrcDigi : public FairMultiLinkedData
{

 public:

  /** Default constructor **/
  PndDrcDigi();


  /** Constructor from station number, sector number, 
   ** front/back side and channel number
   **@param iStation  station number (0-255)
   **@param iSector   sector number  (0-32767)
   **@param iSide     0=front side; 1=back side
   **@param iChannel  channel number
   **/
  PndDrcDigi(Int_t iDetectorId, Int_t iPixelNr, Int_t index);

  /** Destructor **/
  virtual ~PndDrcDigi();

  /** Accessors **/
  Int_t GetDetectorId() const { return fDetectorId; }
  Int_t GetPixelNr()  const { return fPixelNr; }

  
  std::vector<Int_t> GetIndices() const 
    { 
      std::vector<Int_t> result; 
      std::set<FairLink> myLinks = GetLinks();
      for (std::set<FairLink>::iterator it = myLinks.begin(); 
	   it != myLinks.end(); it++){
	result.push_back(it->GetIndex());
      } 
      return result; 
    } 

  Int_t GetNIndices() {return GetNLinks();} 
  Int_t GetIndex(int i = 0) const{ return GetLink(i).GetIndex();} 
  
  void AddIndex(int index)
  {AddLink(FairLink("DRCPoint", index));} 
  void AddIndex(std::vector<Int_t> index)
  {SetLinks(FairMultiLinkedData("DRCPoint", index));} 

 private:

  Int_t    fDetectorId;
  Int_t    fPixelNr;
  

  ClassDef(PndDrcDigi,1);

};

#endif
