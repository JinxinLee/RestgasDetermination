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
        out << "PndSds Digi in: " << digi.GetDetName() << " FE: "
        << digi.GetFE() << " "
        << " charge " << digi.GetCharge() << " e"
        << ", from Point(s) ";
        std::vector<Int_t> indices = digi.GetIndices();
        for (int i = 0; i < indices.size(); i++){
        	std::cout << digi.GetIndex(i) << " " << std::endl;
        }
        return out;
      }

	public : PndSdsDigi();
                 PndSdsDigi(std::vector<Int_t> index, Int_t detID, TString detName, Int_t fe, Double_t charge); 
		 PndSdsDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge);
		/**<constructor
		* \param index position of PndSdsMCPoint in TClonesArray
		* \param detID detector ID (from/for CbmPoint/Hit)
		* \param detName geoH->GetID(gGeoManager->GetPath()) information
		* \param fe front end ID
		* \param charge deposited charge in electrons
		*/
		virtual ~PndSdsDigi(){};

		Int_t GetFE() const { return fFE;}
		TString GetDetName()   const { return fDetName; }
		Double_t GetCharge()	 const { return fCharge; }
		Int_t GetDetID() const { return fDetID;}
		std::vector<Int_t> GetIndices() const { return fIndex;}
		int GetNIndices() {return fIndex.size();}
		Int_t GetIndex(int i = 0) const{ return fIndex[i];}
		Int_t GetNIndices() const { return fIndex.size();}
		void AddIndex(int index){ 				//todo: has to be pure virtual to use AddLink
			fIndex.push_back(index);
			//AddLink(FairLink(kMVDPoint, index));
		}
		void AddIndex(std::vector<Int_t> index){ //todo: has to be pure virtual to use SetLinks
			fIndex = index;
			//SetLinks(FairMultiLinkedData(kMVDPoint, index));
		}
		void AddCharge(double charge){fCharge += charge;}

		virtual void Print() {
			std::cout << *this;
		}
	protected :
		std::vector<Int_t> fIndex;
		Int_t fDetID;
		TString fDetName;
		Double_t fCharge;
		Int_t fFE;

	ClassDef(PndSdsDigi,1);
};

#endif
