//
// C++ Interface: PndTutPar
//
#ifndef PNDTUTPAR_H
#define PNDTUTPAR_H

#include <TVector3.h>
#include <TObjString.h>

#include "CbmParGenericSet.h"
#include "CbmParamList.h"

class PndTutPar : public CbmParGenericSet
{
	public :
		PndTutPar (const char* name="PndTutParTest",
			    const char* title="Tutorial  parameter",
			    const char* context="TestDefaultContext");
		~PndTutPar(void){};
		void clear(void){};
		void putParams(CbmParamList* list);
		Bool_t getParams(CbmParamList* list);
		
		void Print();
  /** Accessor functions **/
  Double_t GetTopPitch()        const {return fTopPitch;}
  TVector3 GetTopAnchor()       const {return fTopAnchor;}
  Int_t    GetNrTopFE()         const {return fTopNrFE;}
  const char*  GetFeType()      const {return fFeType.Data();}

  void SetTopPitch(Double_t x)  {fTopPitch = x;}
  void SetTopAnchor(TVector3 x) {fTopAnchor = x;}
  void SetNrTopFE(Int_t x)      {fTopNrFE = x;}
  void SetFeType(TString x)     {fFeType = x;}

  private:
    // Strip Parameters
    Double_t fTopPitch;     // Strip pitch on top wafer side
    TVector3 fTopAnchor;    // Anchor point of top strip#0
    Int_t    fTopNrFE;      // Number of FE attached to top wafer side
    TString  fFeType;       // Frontend type name

	ClassDef(PndTutPar,1); // don't set this to 0, the container would not be stored then!
};

#endif /* !PNDTUTPAR_H*/

