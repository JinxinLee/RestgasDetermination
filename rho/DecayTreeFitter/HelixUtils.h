// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef HELIXUTILS_H
#define HELIXUTILS_H 1

#include <string>
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TVector3.h"
#include "Rtypes.h"

typedef TVector3 BField ;

namespace DecayTreeFitter
{

  class VtkHelixUtils
  {
  public:
    enum VertexCoor {in_x=0,in_y,in_z,in_px,in_py,in_pz} ;
    enum HelixCoor  {ex_d0=0,
      ex_phi0=1,
      ex_omega=2,
      ex_z0=3,
      ex_tanDip=4,
      ex_flt=5} ;

    static void vertexFromHelix(const TVectorD& helixpar,
                                const BField& fieldmap,
                                TVectorD& vertexpar, int& charge) ;

    static void helixFromVertex(const TVectorD& vertexpar, int charge,
                                const BField& fieldmap,
                                TVectorD& helixpar, TMatrixD& jacobian) ;

    static void helixFromVertexNumerical(const TVectorD& vertexpar, int charge,
                                         const BField& fieldmap,
                                         TVectorD& helixpar, TMatrixD& jacobian) ;

    static std::string helixParName(int i) ;
    static std::string vertexParName(int i) ;
    static void printHelixPar(const TVectorD& helixpar) ;
    static void printVertexPar(const TVectorD& vertexpar, int charge) ;

    static double helixPoca(const TVectorD& helixpar1,
                            const TVectorD& helixpar2,
                            double& flt1, double& flt2,
                            TVector3& v, bool parallel=false) ;
    static double helixPoca(const TVectorD& helixpar,const TVector3& point,
                            double& flt) ;
    static double phidomain(const double phi) ;
    ClassDef ( VtkHelixUtils,1 )
  } ;

}


#endif
