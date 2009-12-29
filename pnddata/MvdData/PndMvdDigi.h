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

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>

#include "PndDetectorList.h"


class PndMvdDigi : public TObject
{
    friend std::ostream& operator<< (std::ostream& out, PndMvdDigi& digi){
        out << "PndMvd Digi in: " << digi.GetDetName() << " FE: "
        << digi.GetFE() << " "
        << " charge " << digi.GetCharge() << " e"
        << ", from Point(s) ";
        std::vector<Int_t> indices = digi.GetIndices();
        for (int i = 0; i < indices.size(); i++){
        	std::cout << digi.GetIndex(i) << " " << std::endl;
        }
        return out;
      }

	public : PndMvdDigi();
		 PndMvdDigi(std::vector<Int_t> index, Int_t detID, TString detName, Int_t fe, Double_t charge);
		 PndMvdDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge);
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
		Int_t GetDetID() const { return fDetID;}
		std::vector<Int_t> GetIndices() const { return fIndex;}
		int GetNIndices() {return fIndex.size();}
		Int_t GetIndex(int i = 0) const{ return fIndex[i];}
		Int_t GetNIndices() const { return fIndex.size();}
		void AddIndex(int index){
		             fIndex.push_back(index); 
		          //  AddLink(kMVDPoint, index);
			    }
		void AddIndex(std::vector<Int_t> index){
			fIndex = index;
			//SetLinks(kMVDPoint, index);
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

	ClassDef(PndMvdDigi,1);
};

#endif
