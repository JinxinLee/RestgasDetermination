//
// C++ Interface: PndSdsStripDigiPar
//
#ifndef PNDSDSSTRIPDIGIPAR_H
#define PNDSDSSTRIPDIGIPAR_H

#include <TVector2.h>
// #include <TString.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

//! Digitization Parameter Class for MVD-Strip part
class PndSdsStripDigiPar : public FairParGenericSet
{
	public :
		PndSdsStripDigiPar (const char* name="PndSdsParTest",
			    const char* title="PndSds strip digi parameter",
			    const char* context="TestDefaultContext");
		~PndSdsStripDigiPar(void){};
		void clear(void){};
		void putParams(FairParamList* list);
		Bool_t getParams(FairParamList* list);
		
		void Print();
  /** Accessor functions **/
  Double_t GetTopPitch()        const {return fTopPitch;}
  Double_t GetBotPitch()        const {return fBotPitch;}
  Double_t GetOrient()          const {return fOrient;}
  Double_t GetSkew()            const {return fSkew;}
  TVector2 GetTopAnchor()       const {return fTopAnchor;}
  TVector2 GetBotAnchor()       const {return fBotAnchor;}
  Int_t    GetNrTopFE()         const {return fTopNrFE;}
  Int_t    GetNrBotFE()         const {return fBotNrFE;}
  Int_t    GetNrFECh()          const {return fFeChannels;}
  Double_t GetThreshold()       const {return fThreshold;}
  Double_t GetNoise()           const {return fNoise;}
  const char*  GetSensType()    const {return fSensType.Data();}
  const char*  GetFeType()      const {return fFeType.Data();}
  Int_t GetClusterMod()         const {return fClusterMod;}
  Int_t GetRadChannel()         const {return fRadChannel;}
  Int_t GetRadTime()            const {return fRadTime;}
  Double_t GetChargeCut()       const {return fChargeCut;}
  
  void SetTopPitch(Double_t x)  {fTopPitch = x;}
  void SetBotPitch(Double_t x)  {fBotPitch = x;}
  void SetOrient(Double_t x)    {fOrient = x;}
  void SetSkew(Double_t x)      {fSkew = x;}
  void SetTopAnchor(TVector2 x) {fTopAnchor = x;}
  void SetBotAnchor(TVector2 x) {fBotAnchor = x;}
  void SetNrTopFE(Int_t x)      {fTopNrFE = x;}
  void SetNrBotFE(Int_t x)      {fBotNrFE = x;}
  void SetNrFECh(Int_t x)       {fFeChannels = x;}
  void SetThreshold(Double_t x) {fThreshold = x;}
  void SetNoise(Double_t x)     {fNoise = x;}
  void SetSensType(TString x)   {fSensType = x;}
  void SetFeType(TString x)     {fFeType = x;}
  void SetClusterMod(Int_t  x)  {fClusterMod = x;}
  void SetRadChnnel(Int_t x)    {fRadChannel = x;}
  void SetRadTime(Int_t x)      {fRadTime = x;}
  void SetChargeCut(Double_t x) {fChargeCut = x;}
  
  private:
    // Strip Parameters
    Double_t fTopPitch;     // Strip pitch on top wafer side
    Double_t fBotPitch;     // Strip pitch on bottom wafer side
    Double_t fOrient;       // orientation angle of top strips
    Double_t fSkew;         // skew angle of bottom strips wrt top strips
    Double_t fThreshold;    // Discriminator threshold
    Double_t fNoise;        // Complete noise including threshold dispersion
    Double_t fChargeCut;    // Cluster charge corralation for double sided sensors
    TVector2 fTopAnchor;    // Anchor point of top strip#0
    TVector2 fBotAnchor;    // Anchor point of bottom strip#0
    Int_t    fFeChannels;   // Number of Channels per FE
    Int_t    fTopNrFE;      // Number of FE attached to top wafer side
    Int_t    fBotNrFE;      // Number of FE attached to bottom wafer side
    Int_t    fClusterMod;   // Clusterfinder mode: 1,2,3,4,5
    Int_t    fRadChannel;   // Clusterfinder channel search radius
    Int_t    fRadTime;      // Clusterfinder timing search radius
    TString  fSensType;     // Sensor type name (rect, trap...)
    TString  fFeType;       // Frontend type name (APV25, CBM-XYTER, ...)
	ClassDef(PndSdsStripDigiPar,2);
};

#endif /* !PNDSDSSTRIPDIGIPAR_H*/

