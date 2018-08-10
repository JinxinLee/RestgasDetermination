/**
 @class PndSmpCand

 @author Klaus Goetzen <k [dot] goetzen (at) gsi [dot] de>

 @brief Simple particle candidate to perform simple combinatorics and particle counting for event filtering
 
 This class defines a simple particle candidate, which allows simple combinatoric with overlap check 
 and provides access to its internal 4-vector.

 */
#ifndef PndSmpCand_H
#define PndSmpCand_H

#include "TLorentzVector.h"
#include <iostream>

using std::cout;
using std::endl;

// -------------------------------------------------------------------------

class PndSmpCand {
public:
	// Constructors
	/** @brief Default constructor. **/
	PndSmpCand() {};
	
	/** @brief Constructor from Lorentz vector reference. **/
	PndSmpCand(TLorentzVector lv, Float_t chrg, Int_t pdg = 0, Int_t uid = -1);
	
	/** @brief Constructor from pointer to Lorentz vector . **/
	PndSmpCand(TLorentzVector *lv, Float_t chrg, Int_t pdg = 0, Int_t uid = -1);
	
	/** @brief Constructor from 2 daughters. Calls CombCand **/
	PndSmpCand(Int_t pdg, PndSmpCand &c0, PndSmpCand &c1) { CombCand(pdg, &c0, &c1); }
	/** @brief Constructor from 3 daughters. Calls CombCand **/
	PndSmpCand(Int_t pdg, PndSmpCand &c0, PndSmpCand &c1, PndSmpCand &c2) { CombCand(pdg, &c0, &c1, &c2); }
	/** @brief Constructor from 4 daughters. Calls CombCand **/
	PndSmpCand(Int_t pdg, PndSmpCand &c0, PndSmpCand &c1, PndSmpCand &c2, PndSmpCand &c3) { CombCand(pdg, &c0, &c1, &c2, &c3); }
	/** @brief Constructor from 5 daughters. Calls CombCand **/
	PndSmpCand(Int_t pdg, PndSmpCand &c0, PndSmpCand &c1, PndSmpCand &c2, PndSmpCand &c3, PndSmpCand &c4) { CombCand(pdg, &c0, &c1, &c2, &c3, &c4); }
	
	/** @brief Constructor from up to 5 daughter pointers. Calls CombCand **/
	PndSmpCand(Int_t pdg, PndSmpCand *c0, PndSmpCand *c1, PndSmpCand *c2=0, PndSmpCand *c3=0, PndSmpCand *c4=0) { CombCand(pdg, c0, c1, c2, c3, c4); }
	
	// Modifiers
	/** @brief Sets LorentzVector **/
	void SetP4(TLorentzVector p4) { fLV     = p4; }
	/** @brief Sets mass (used for different mass hypotheses in filter. **/
	void SetM(Double_t m) { fLV.SetVectM(fLV.Vect(), m); }
	/** @brief Sets charge. **/
	void SetCharge(Float_t ch)    { fCharge = ch; }

	// Methods
 	/** @brief Overload of == operator needed to compare std::vectors of candidates **/
    Bool_t operator== ( const PndSmpCand &c1) const { return (c1.fMarker == fMarker ) && ( c1.fPdgCode == fPdgCode) ;}
 	/** @brief Overload of == operator needed to compare std::vectors of candidates **/
    Bool_t operator!= ( const PndSmpCand &c1) const { return !(*this==c1); }

 	/** @brief Combines this candidate from several daughters (adds P4 and charges). **/
	void            CombCand(Int_t pdg, PndSmpCand *c0, PndSmpCand *c1, PndSmpCand *c2=0, PndSmpCand *c3=0, PndSmpCand *c4=0);
	
	TLorentzVector  P4()     { return fLV; }
	Float_t         Charge() { return fCharge; }
	Int_t           Pdg()    { return fPdgCode; }
	Int_t           NDau()   { return fNDau; }
	ULong_t         Marker() { return fMarker; }	
	PndSmpCand*     Dau(Int_t idx) { if (idx>=0 && idx<fNDau) return fDaughter[idx]; else return 0; }
	Bool_t          Overlap(PndSmpCand *c) { return fMarker & c->fMarker; }
	Bool_t          Overlap(PndSmpCand &c) { return fMarker & c.fMarker; }	
	void            Print(); 
	
private:
	// Private methods
 	/** @brief Prints a unsigned int as bit string **/
	void PrintBinary(ULong_t x)
	{
		ULong_t bit=ULong_t(1)<<63;
		while (bit) {cout << ((x&bit)?1:0);  bit>>=1; if (bit & 0x8080808080808080) cout <<" ";}
	}
	
	// Member variables
	TLorentzVector fLV;             // the Lorentz vector
	Float_t        fCharge;         // charge
	Int_t          fPdgCode;        // pdg code
	Int_t          fUniqueID;       // unique id, only applied for initial (non-composite) candidates
	ULong_t        fMarker;         // a bit marker used for overlap check; limits number of initial candidate to 64 (long = 64 bits)
	Int_t          fNDau;           // number of daughters of composites (up to 5)
	PndSmpCand  *fDaughter[5];  // pointers to daughters

};


// -------------------------------------------------------------------------

#endif
