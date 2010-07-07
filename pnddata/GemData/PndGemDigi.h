//* $Id: */

// -------------------------------------------------------------------------
// -----                      PndGemDigi header file                   -----
// -----                  Created 28/08/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** PndGemDigi.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 28.08.06
 **@version 1.0
 **
 ** Data class for digital STS information
 ** Data level: RAW
 **
 ** The detector ID consists of:
 **   system ID (0-31, kSTS=2), bits 0-4, see base class
 **   station number (0-255), bits 5-12 
 **   sector number (0-32767), bits 13-27
 **   side (0=front side, 1=back side, bit 28, always 0 for pixel sectors).
 ** 
 ** The index of the (first) MCPoint having activated this channel
 ** is accessible via GetMcPointIndex(). 
 **/


#ifndef PNDGEMDIGI_H
#define PNDGEMDIGI_H 1

#include <vector>

#include "FairMultiLinkedData.h"

#include "PndDetectorList.h"

#include "TObject.h"

class PndGemDigi : public FairMultiLinkedData
{

 public:

  /** Default constructor **/
  PndGemDigi();


  /** Constructor from station number, sector number, 
   ** front/back side and channel number
   **@param iStation  station number (0-255)
   **@param iSector   sector number  (0-32767)
   **@param iSide     0=front side; 1=back side
   **@param iChannel  channel number
   **/
  PndGemDigi(Int_t iDetectorId, Int_t iChannel, Int_t index);

  /** Destructor **/
  virtual ~PndGemDigi();

  void SetADC(Double_t iADC) { fDigiADC = iADC; }
  void SetTDC(Double_t iTDC) { fDigiTDC = iTDC; }
  void SetCor(Double_t iCor) { fDigiCor = iCor; }

  void AddADC(Double_t iADC) { fDigiADC+= iADC; }

  /** Accessors **/
  Int_t GetDetectorId() const { return fDetectorId; }
  Int_t GetChannelNr()  const { return fChannelNr; }

  Int_t    GetSystemId()   const { 
    return ( ( fDetectorId & (  31<<27) ) >> 27); }
  Int_t    GetStationNr()  const { 
    return ( ( fDetectorId & (8191<< 8) ) >>  8 ); }
  Int_t    GetSensorNr()   const {  // sensor number within station
    return ( ( fDetectorId & (   3<< 6) ) >>  6 ); }
  Int_t    GetSide()       const {
    return ( ( fDetectorId & (   1<< 5) ) >>  5 ); }  // 0=front, 1=back

  Double_t GetADC()          const { return fDigiADC; }
  Double_t GetTDC()          const { return fDigiTDC; }
  Double_t GetCor()          const { return fDigiCor; }
  
  std::vector<Int_t> GetIndices() const { 
    std::vector<Int_t> result; 
    std::set<FairLink> myLinks = GetLinks();
    for (std::set<FairLink>::iterator it = myLinks.begin(); it != myLinks.end(); it++){
      result.push_back(it->GetIndex());
    } 
    return result; 
  } 
  Int_t GetNIndices() {return GetNLinks();} 
  Int_t GetIndex(int i = 0) const{ return GetLink(i).GetIndex();} 
  
  void AddIndex(int index){ 
    AddLink(FairLink(kGemPoint, index)); 
  } 
  void AddIndex(std::vector<Int_t> index){ 
    SetLinks(FairMultiLinkedData(kGemPoint, index)); 
  } 

 private:

  Int_t    fDetectorId;
  Int_t    fChannelNr;
  
  Double_t fDigiADC;
  Double_t fDigiTDC;
  Double_t fDigiCor;

  ClassDef(PndGemDigi,1);

};

#endif
