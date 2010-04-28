// --------------------------------------------------------
// ----			PndMvdDigi header file	---
// ----			Created 19.10.07 by T.Stockmanns ---
// --------------------------------------------------------

/** PndMvdDigi.h
*@author T.Stockmanns <t.stockmanns@fz-juelich.de>
**
** \brief Base class for Digi information
**
**
**/

#ifndef PNDMVDDIGI_H
#define PNDMVDDIGI_H

#include "FairMultiLinkedData.h"

#include "TObject.h"
#include "TString.h"
#include "PndDetectorList.h"
#include <iostream>
#include <vector>


class PndMvdDigi : public FairMultiLinkedData
{
    friend std::ostream& operator<< (std::ostream& out, PndMvdDigi& digi){
        out << "PndMvd Digi in: " << digi.GetDetName() << " FE: "
        << digi.GetFE() << " "
        << " charge " << digi.GetCharge() << " e"
        << ", from Point(s) ";
        std::vector<Int_t> indices = digi.GetIndices();
        for (unsigned int i = 0; i < indices.size(); i++){
        	std::cout << digi.GetIndex(i) << " " << std::endl;
        }
        return out;
      }

	public : PndMvdDigi();
		PndMvdDigi(std::vector<Int_t> index, Int_t detID, TString detName, Int_t fe, Double_t charge, Double_t tot = -1);
		PndMvdDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge, Double_t tot = -1);
		/**<constructor
		* \param index position of PndMvdMCPoint in TClonesArray
		* \param detID detector ID (from/for CbmPoint/Hit)
		* \param detName geoH->GetID(gGeoManager->GetPath()) information
		* \param fe front end ID
		* \param charge deposited charge in electrons
		*/

		virtual ~PndMvdDigi(){};

		Int_t GetFE() const { return fFE;}
		TString GetDetName()   const { return fDetName; }
		Double_t GetCharge()	 const { return fCharge; }
		Double_t GetTot()	 const { return fTot; }
		Int_t GetDetID() const { return fDetID;}
		std::vector<Int_t> GetIndices() const {
			std::vector<Int_t> result;
			std::vector<FairLink> myLinks = GetLinks();
			for (unsigned int i = 0; i < myLinks.size(); i++){
				result.push_back(myLinks[i].GetIndex());
			}
			return result;
		}
		int GetNIndices() {return GetNLinks();}
		Int_t GetIndex(int i = 0) const{ return GetLink(i).GetIndex();}
		Int_t GetNIndices() const { return GetNLinks();}
		void AddIndex(int index){
			AddLink(FairLink(kMVDPoint, index));
		}
		void AddIndex(std::vector<Int_t> index){
			SetLinks(FairMultiLinkedData(kMVDPoint, index));
		}
		void AddCharge(double charge){fCharge += charge;}
		void SetTot(double tot){fTot = tot;}

		virtual void Print() {
			std::cout << *this;
		}
	protected :
		Int_t fDetID;
		TString fDetName;
		Double_t fCharge;
		Double_t fTot;
		Int_t fFE;

	ClassDef(PndMvdDigi,1);
};

#endif
