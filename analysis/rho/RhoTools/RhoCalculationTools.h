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

#include "RhoCandidate.h"

#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "RhoMath/RhoError.h"

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


/**********************************************************
 Description: Class PndAnaCovTool
 Taken from Base class for particles used for Vertex fitting
 Authors: Dipak K. Mishra
          R. Kliemt (taking away the cov converters)
***********************************************************/
    static TMatrixDSym       GetConverted6(TMatrixDSym) ;
    static TMatrixD          GetConverted6(TMatrixD) ;
    static TMatrixD          GetConverted7(TMatrixD) ;
    static TMatrixDSym       GetCovMat(TMatrixD) ;
    static TMatrixDSym       GetCovMat1(TMatrixD) ;
    static TMatrixDSym       GetFitError(TMatrixDSym) ;
    static TMatrixD          GetFitError(TLorentzVector, TMatrixD) ;



    /// Calculator functions
//    static Bool_t FillHelixParams(RhoCandidate* cand, Bool_t skipcov=kFALSE);
    static Bool_t P7toHelix(const TVector3& pos, const TLorentzVector& p4, const Double_t Q,
                            const TMatrixD& cov77, Float_t* helixparams, TMatrixD& helixCov, Bool_t skipcov=kFALSE);
    static Bool_t P7toPRG(const TVector3& pos, const TLorentzVector& p4, const Double_t Q, const TMatrixD& cov77,
                          const TVector3& expPoint, Float_t* helixparams, TMatrixD& helixCov, TMatrixD& jacobian, Bool_t skipcov=kFALSE);
//    static Bool_t SDtoHelix(FairTrackParH* par, RhoCandidate* cand, Bool_t skipcov=kFALSE);
    static Bool_t P6FromTrajectory( TVectorD& mom6, TMatrixDSym& cov6, RhoCandidate* cand, double z, double ztolerance);
    static Double_t StateFromTrajectory( TVectorD& state, TMatrixDSym& cov, RhoCandidate* track, double vx, double vy, double vz, double ztolerance );

    static void TransportToZ(RhoCandidate* cand, Double_t z=0);
    static void PrintMatrix(TMatrixT<double> m);
    static void PrintMatrix(TMatrixTSym<double> m);
    static void PrintMatrix(RhoError m);

  private:
    
    static std::ostream& bold_on(std::ostream& os)
    {
      return os << "\e[1m";
    }

    static std::ostream& bold_off(std::ostream& os)
    { 
      return os << "\e[0m"; 
    }

    RhoCalculationTools();
    ~RhoCalculationTools();

    static Double_t fBz;
    static Int_t fVerbose;
    static Bool_t fBzSet;
};

#endif
