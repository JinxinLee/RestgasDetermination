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
    Double_t GetClustRadius() const {return fRadius;}

    Double_t GetRaisingTime() const {return fraisingtime;}
    Double_t GetFallingRatio() const {return ffallingratio;}
    Double_t GetClockFrequency() const {return fclockfrequency;}

    void SetXPitch(Double_t x)  {fDimX = x;}
    void SetYPitch(Double_t x)  {fDimY = x;}
    void SetThreshold(Double_t x) {fThreshold = x;}
    void SetNoise(Double_t x)     {fNoise = x;}
    void SetFECols(Int_t x){fFeCols = x;}
    void SetFERows(Int_t x){fFeRows = x;}
    void SetClustRadius(Double_t x){fRadius=x;}

    void SetRaisingTime(Double_t x){fraisingtime = x;}
    void SetFallingRatio(Double_t x){ffallingratio = x;}
    void SetClockFrequency(Double_t x){fclockfrequency = x;}

  private:
    // Pixel Parameters
    Int_t fFeCols;		// Colums read per Frontend
    Int_t fFeRows;  	// Rows read per Frontend
    Double_t fDimX; 	// Pixel cell size X
    Double_t fDimY; 	// Pixel cell size Y
    Double_t fRadius;  	// Clusterfinder search radius (channel numbers)
    Double_t fThreshold;// Discriminator Threshold (electrons)
    Double_t fNoise;    // Gaussian electronics noise including threshold dispersion (electrons)
    Double_t fraisingtime;		// time until capacitor is loaded [ns]
    Double_t ffallingratio;		// current unloading the capacitor [e/ns]
    Double_t fclockfrequency;	// clockfrequency of the readout chip [MHz]
//     Text_t   fSensName;  // Sensor name
//     Text_t   fFeName;    // Frontend name


  ClassDef(PndMvdPixelDigiPar,3);
};

#endif /*!MVDSTRIPDIGIPAR_H*/
