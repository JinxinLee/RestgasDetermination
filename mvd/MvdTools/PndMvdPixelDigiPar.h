//
// C++ Interface: PndMvdPixelDigiPar
//
#ifndef PNDMVDPIXELDIGIPAR_H
#define PNDMVDPIXELDIGIPAR_H

#include <TVector2.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

//! Digitization Parameter Class for MVD-Pixel part
class PndMvdPixelDigiPar : public FairParGenericSet
{
  public :
    PndMvdPixelDigiPar (const char* name="PndMvdParTest",
          const char* title="PndMvd pixel digi parameter",
          const char* context="TestDefaultContext");
    ~PndMvdPixelDigiPar(void){};
    void clear(void){};
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    Double_t GetXPitch()  const {return fDimX;}
    Double_t GetYPitch()  const {return fDimY;}
    Double_t GetThreshold() const {return fThreshold;}
    Double_t GetNoise()     const {return fNoise;}
    Int_t GetFECols() const {return fFeCols;}
    Int_t GetFERows() const {return fFeRows;}


    void SetXPitch(Double_t x)  {fDimX = x;}
    void SetYPitch(Double_t x)  {fDimY = x;}
    void SetThreshold(Double_t x) {fThreshold = x;}
    void SetNoise(Double_t x)     {fNoise = x;}
    void SetFECols(Int_t x){fFeCols = x;}
    void SetFERows(Int_t x){fFeRows = x;}


  private:
    // Pixel Parameters
    Double_t fDimX;
    Double_t fDimY;
    Int_t fFeCols;
    Int_t fFeRows;


    Double_t fThreshold; // Discriminator fThreshold
    Double_t fNoise;     // Complete fNoise including fThreshold dispersion
//     Text_t   fSensName;  // Sensor name
//     Text_t   fFeName;    // Frontend name


  ClassDef(PndMvdPixelDigiPar,2);
};

#endif /*!MVDSTRIPDIGIPAR_H*/
