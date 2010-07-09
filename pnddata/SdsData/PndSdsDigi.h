// --------------------------------------------------------
// ----			PndSdsDigi header file	---
// ----			Created 19.10.07 by T.Stockmanns ---
// --------------------------------------------------------

/** PndSdsDigi.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** \brief Base class for Digi information
 **
 **
 **/

#ifndef PNDSDSDIGI_H
#define PNDSDSDIGI_H

#include "FairMultiLinkedData.h"

#include "TObject.h"
#include "TString.h"
#include "PndDetectorList.h" 
#include <iostream>
#include <vector>


class PndSdsDigi : public FairMultiLinkedData
  {
    friend std::ostream& operator<< (std::ostream& out, PndSdsDigi& digi){
      out << "PndSds Digi in sensor: " << digi.GetSensorID() << " FE: "
      << digi.GetFE() << " "
      << " charge: " << digi.GetCharge() << " e"
      << " timestamp: "<<digi.GetTimestamp()
      << ", from Point(s): ";
      std::vector<Int_t> indices = digi.GetIndices();
      for (unsigned int i = 0; i < indices.size(); i++){
        std::cout << digi.GetIndex(i) << " " << std::endl;
      }
      return out;
    }
    
    public : PndSdsDigi();
    PndSdsDigi(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Double_t charge, Int_t type, Int_t timestamp = -1);
    PndSdsDigi(Int_t index, Int_t detID, Int_t fSensorID, Int_t fe, Double_t charge, Int_t type, Int_t timestamp = -1);
		/**<constructor
     * \param index position of PndSdsMCPoint in TClonesArray
     * \param detID detector ID (from/for CbmPoint/Hit)
     * \param detName geoH->GetID(gGeoManager->GetPath()) information
     * \param fe front end ID
     * \param charge deposited charge in electrons
     */
		virtual ~PndSdsDigi(){};
    
		Int_t GetFE() const { return fFE;}
		//TString GetDetName()   const { return fDetName; }
		Int_t GetSensorID() const { return fSensorID; }
		Double_t GetCharge()	 const { return fCharge; }
		Int_t GetDetID() const { return fDetID;}
		std::vector<Int_t> GetIndices() const { return fIndex;}
		int GetNIndices() {return fIndex.size();}
		Int_t GetIndex(int i = 0) const{ return fIndex[i];}
		Int_t GetNIndices() const { return fIndex.size();}
    Int_t GetTimestamp() const { return fTimestamp; }
    
		virtual void SetMCPointType(Int_t type){fMCPointType = type;}
    
		virtual void AddIndex(int index)
		{
			fIndex.push_back(index);
			AddLink(FairLink(fMCPointType, index));
		}
    
		virtual void AddIndex(std::vector<Int_t> index)
		{
			fIndex = index;
			AddLinks(FairMultiLinkedData(fMCPointType, index));
		}
		void SetCharge(double charge){fCharge = charge;}
    void AddCharge(double charge){fCharge += charge;}
    
		virtual void Print() {
			std::cout << *this;
		}
    protected :
		std::vector<Int_t> fIndex;
		Int_t fDetID;
		//TString fDetName;
		Int_t fSensorID;
		Int_t fFE;
		Double_t fCharge;
		Int_t fMCPointType;
    Int_t fTimestamp;	// Timestamp of event + time of flight [ns]
    
    ClassDef(PndSdsDigi,3);
  };

#endif
