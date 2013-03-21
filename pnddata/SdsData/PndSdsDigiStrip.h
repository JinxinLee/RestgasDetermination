// --------------------------------------------------------
// ----     PndSdsDigiStrip header file                   ---
// ----     Created 10.07.07 by Ralf Kliemt             ---
// --------------------------------------------------------

/** PndSdsDigiStrip.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **/

#ifndef PNDSDSDIGISTRIP_H
#define PNDSDSDIGISTRIP_H


#include "PndSdsDigi.h"
#include "PndDetectorList.h"

#include "TObject.h"
#include "TString.h"
#include <iostream>

//! Class for digitised strip hits
/**
 * Information about digitised hits from strip detectors
 * @author HG Zaunick <hg.zaunick@physik.tu-dresden.de>
 *
 **/


class PndSdsDigiStrip : public PndSdsDigi
  {
    public :
    PndSdsDigiStrip();
    
    PndSdsDigiStrip(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge, Double_t timestamp = -1);
    PndSdsDigiStrip(Int_t index, Int_t detID, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge, Double_t timestamp = -1);
    ~PndSdsDigiStrip(){};
    
    friend std::ostream& operator<< (std::ostream& out, const PndSdsDigiStrip& digi);

    
    //     Int_t GetIndex() const { return fIndex; }
    //     Int_t GetDetID() const { return fDetID;}
    //     TString GetDetName() const { return fDetName; }
    //     Int_t GetFE() const { return fFE; }
    Int_t GetChannel() const { return fChannel; }
    //    Int_t GetTimestamp() const { return fTimestamp; }
    //     Double_t GetCharge() const { return fCharge; }
    //     Int_t GetMCID() const { return fMCID; }
    
    //     void SetIndex ( Int_t index ) { fIndex = index; }
    //     void SetDetID ( Int_t detID ) { fDetID = detID; }
    //     void SetDetName ( const TString& detName ) { fDetName = detName; }
    //     void SetFE ( Int_t fe ) { fFE = fe; }
    //     void SetChannel ( Int_t channel ) { fChannel = channel; }
    //     void SetCharge ( Double_t charge ) { fCharge = charge; }
    //     void SetMCID ( Int_t mcID ) { fMCID = mcID; }
    
    //     const void Print();
    
    Bool_t operator==(const PndSdsDigiStrip& d2) const;
    //     Bool_t const HasNeighbour(const PndSdsDigiStrip& d2);

    virtual bool equal(FairTimeStamp* data) const;

    virtual bool operator<(const PndSdsDigiStrip& myDigi) const{
		if (fDetID < myDigi.GetDetID()) 		return true;	else if(fDetID > myDigi.GetDetID()) return false;
		if (fSensorID < myDigi.GetSensorID()) 	return true; 	else if (fSensorID > myDigi.GetSensorID()) return false;
		if (fFE < myDigi.GetFE()) 				return true;	else if (fFE > myDigi.GetFE()) return false;
		if (fChannel < myDigi.GetChannel()) 	return true;	else if (fChannel > myDigi.GetChannel()) return false;
		return false;
    }

    PndSdsDigiStrip& operator=(const PndSdsDigiStrip& strip){
    	if (this != &strip){
    		this->PndSdsDigi::operator=(strip);
    		fChannel = strip.GetChannel();
    	}
    	return *this;
    }

    std::ostream& Print(std::ostream& out = std::cout) const{
    	out << GetDetID() << " PndSds DigiStrip in sensor: " << GetSensorID() << " FE: "
    	      << GetFE() << " Channel: " << GetChannel()
    	      << " charge: " << GetCharge() << " (e or tot)"
    	      << " timestamp: "<< GetTimeStamp()
    	      << ", from Point(s): ";
    	      std::vector<Int_t>indices = GetIndices();
    	      for (unsigned int i = 0; i < indices.size(); i++){
    	        out << indices[i] << "  ";
    	      }
    	      out << std::endl;
    	      return out;
    }
    
    private :
    //    Int_t fIndex;   /// MC Point index
    //    Int_t fDetID;   /// Detector ID
    //    TString fDetName;   /// Detector Name
    //    Int_t fFE;   /// Frontend Number
    Int_t fChannel;   /// Frontend Channel
    //    Int_t fTimestamp;	/// Timestamp of ev
    //    Double_t fCharge;   /// Charge of Hit
    //    Int_t fMCID;   /// MC Track index
    
    ClassDef(PndSdsDigiStrip,6);
  };

#endif
