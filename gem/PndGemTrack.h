// -------------------------------------------------------------------------
// -----                      PndGemTrack header file                  -----
// -----                  Created 19.03.2009 by R. Karabowicz          -----
// -----                      base on PndDchTrack                      -----
// -------------------------------------------------------------------------

/** \class PndGemTrack
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief Class for Gem track representation
 * 
 *   Gem local track. Holds lists of PndGemHits or PndGemDigi and
 *   the fitted track parameters.
 **/

#ifndef PNDGEMTRACK_H
#define PNDGEMTRACK_H 1

// Pnd includes
#include "FairTrackParam.h"

// ROOT includes
#include "TArrayI.h"
#include "TObject.h"

// c++ includes
#include <map>
using std::map;
using std::multimap;


class PndGemHit;
class PndGemDigi;

class PndGemTrack : public TObject
{
public:
  
  /** Default constructor **/
  PndGemTrack();
  
  /** Destructor **/
  virtual ~PndGemTrack();
  
  /** Public method AddHit
   * \param hitID - index of hit in global array of hits
   * \param hit - added new hit
   **/
  void AddHit(PndGemHit* hit, Int_t hitID);
  
  /** Public method AddDigi
   * \param digiID - index of digi in global array of digis
   * \param digi - added new digi
   **/
  void AddDigi(PndGemDigi* digi, Int_t digiID);
  
  /** Public method Print **/
  void Print();
  
  
  /** Public methods SortHits/SortDigi
   ** Sorts the hits/digis in each array in downstream direction
   ** and writes the hit/digi indices into the member TArrayI
   **/
  void SortHits();          ///<Sort hits according to Z coordinate
  void SortDigis(); //!TODO
  
  
  /** Accessors  **/
  Int_t GetNofGemHits()               const { return fGemHits.GetSize(); }          ///<return number of hits in track
  Int_t GetNofGemDigis()              const { return fGemDigis.GetSize(); }         ///<return number of digis in track
  Int_t GetGemHitIndex(Int_t iHit)          const { return fGemHits.At(iHit); }           ///<return index in global Hit array
  Int_t GetGemDigiIndex(Int_t iDigi)  const { return fGemDigis.At(iDigi); }         ///<return index in global Digi array
  Int_t GetFlag()                     const { return fFlag; }
  Double_t GetChi2()                  const { return fChi2; }
  Int_t    GetNDF()                   const { return fNDF; }
  Double_t GetELoss()                 const { return fELoss;}
  FairTrackParam* GetParamFirst() { return &fParamFirst; }
  FairTrackParam* GetParamLast()  { return &fParamLast ; }

  
  /** Modifiers  **/
  void SetParamFirst(FairTrackParam& par)    { fParamFirst = par;   }
  void SetParamLast(FairTrackParam& par)     { fParamLast  = par;   }
  void SetFlag(Int_t flag)                  { fFlag       = flag;  }
  void SetChi2(Double_t chi2)               { fChi2       = chi2;  }
  void SetNDF(Int_t ndf)                    { fNDF        = ndf;   }
  void SetELoss(Double_t eLoss)             { fELoss      = eLoss; }
	
  
  /** Function for sorting **/
  static Bool_t CompareChi2(const PndGemTrack* a, const PndGemTrack* b)
    {
      return ( a->GetChi2()/(Double_t)a->GetNDF() < b->GetChi2()/(Double_t)b->GetNDF() );
    };
  
  static Bool_t CompareMomentum(PndGemTrack* a, PndGemTrack* b)
    {
      Double_t mom_a = 1./TMath::Abs(a->GetParamLast()->GetQp());
      Double_t mom_b = 1./TMath::Abs(b->GetParamLast()->GetQp());
      return ( mom_a > mom_b );
    };


 private:
  
  TArrayI      fGemHits;         ///< array of hits
  TArrayI		 fGemDigis;        ///< array of digis
  
  /** Track parameters at first and last fitted hit **/
  FairTrackParam fParamFirst;
  FairTrackParam fParamLast;
  
  /** Quality flag **/
  Int_t fFlag;
  
  /** RMS deviation of hit coordinates to track **/
  Double32_t fChi2;
  
  /** Number of degrees of freedom **/
  Int_t fNDF;
  
  /** Total energy loss in TRD **/
  Double_t fELoss;
  
  /** Maps from hit z position to hit index. STL multimap is used because it
   *  is automatically sorted. Temporary only; not for storgage.
   *  The Hit index arrys will be filled by the method SortHits.
   **/
  std::multimap<Double_t, Int_t> fGemHitMultimap;
  
  /** Maps from cylinder hit z position to cylinder hit index **/
  std::multimap<Double_t, Int_t> fGemCylinderHitMultimap;
  
  /** Maps from digi plane position to hit index. STL multimap is used because it
   *  is automatically sorted. Temporary only; not for storgage.
   *  The Hit index arrys will be filled by the method SortHits.
   **/
  std::multimap<Double_t, Int_t> fGemDigiMultimap;
  
  ClassDef(PndGemTrack, 1);
};

#endif

