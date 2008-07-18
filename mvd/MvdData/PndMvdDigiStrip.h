// --------------------------------------------------------
// ----     PndMvdDigiStrip header file                   ---
// ----     Created 10.07.07 by Ralf Kliemt             ---
// --------------------------------------------------------

/** PndMvdDigiStrip.h
*@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
**/

#ifndef PNDMVDDIGISTRIP_H
#define PNDMVDDIGISTRIP_H

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include "PndMvdDigi.h"
//! Class for digitised strip hits
/** 
 * Information about digitised hits from strip detectors
 * @author HG Zaunick <hg.zaunick@physik.tu-dresden.de>
 *
 **/
class PndMvdDigiStrip : public PndMvdDigi
{
  public :
    PndMvdDigiStrip();
    PndMvdDigiStrip(Int_t index, Int_t detID, TString detName, Int_t fe,
                    Int_t chan, Double_t charge);
    ~PndMvdDigiStrip(){};

//     Int_t GetIndex() const { return fIndex; }
//     Int_t GetDetID() const { return fDetID;}
//     TString GetDetName() const { return fDetName; }
//     Int_t GetFE() const { return fFE; }
    Int_t GetChannel() const { return fChannel; }
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

    Bool_t operator==(const PndMvdDigiStrip& d2) const;
//     Bool_t const HasNeighbour(const PndMvdDigiStrip& d2);

  private :
//    Int_t fIndex;   /// MC Point index
//    Int_t fDetID;   /// Detector ID
//    TString fDetName;   /// Detector Name
//    Int_t fFE;   /// Frontend Number
    Int_t fChannel;   /// Frontend Channel
//    Double_t fCharge;   /// Charge of Hit
//    Int_t fMCID;   /// MC Track index

    ClassDef(PndMvdDigiStrip,2);
};

#endif
