#ifndef RHOCALCULATIONTOOLS_H
#define RHOCALCULATIONTOOLS_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoCalculationTools                                                  //
//                                                                      //
// Toolset to calculate things centrally.                               //
//   - Magnetic Field Access                                            //
//                                                                      //
// Authors:                                                             //
// Ralf Kliemt, HIM/GSI Feb.2013                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TVector3.h"

class RhoCalculationTools
{
  public:

    /// Allows debugging output for these utilities
    static void SetVerbose ( Int_t level ) {
      fVerbose = level;
    }


    /// Return the magnetic field along the z-axis in [kGs] (1kGs=0.1T)
    static Double_t GetBz ( const TVector3& position );

    /// Force a constant B field value for all positions
    static void ForceConstantBz ( Double_t bz=0. ) {
      fBz=bz;
      fBzSet=kTRUE;
    };
    /// Release the B field to be fetched from the database
    static void UnForceConstantBz() {
      fBzSet=kFALSE;
    };


  private:
    RhoCalculationTools();
    ~RhoCalculationTools();

    static Double_t fBz;
    static Int_t fVerbose;
    static Bool_t fBzSet;
};

#endif // RHOCALCULATIONTOOLS_H
