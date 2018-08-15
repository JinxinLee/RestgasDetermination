// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
//
//  State.h
//  TreeFitter
//
//  Created by Ralf Kliemt on 13/02/15.
//  Copyright (c) 2015 Ralf Kliemt. All rights reserved.
//

#ifndef DECAYTREEFITTER_State_h
#define DECAYTREEFITTER_State_h 1

// **************************************************************************
// * *
// * ! ! ! A T T E N T I O N ! ! ! *
// * *
// * This file was created automatically by GaudiObjDesc, please do not *
// * delete it or edit it by hand. *
// * *
// * If you want to change this file, first change the corresponding *
// * xml-file and rerun the tools from GaudiObjDesc (or run make if you *
// * are using it from inside a Gaudi-package). *
// * *
// **************************************************************************

// Include files
#include <cmath>
#include <vector>
#include <ostream>
//#include "StateVector.h"
#include "Rtypes.h"
#include "TVector3.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"

// Forward declarations

namespace DecayTreeFitter
{

  class State
  {
  public:

    typedef std::vector<State*> Vector;
    typedef std::vector<const State*> ConstVector;

    State(): m_flags(0),
    m_stateVector(5),
    m_covariance(5)
    {};

    //State(const DecayTreeFitter::StateVector& stateVec) : m_flags(0),
    //m_stateVector(stateVec.parameters()),
    //m_covariance() {};

    virtual ~State() {};

    inline unsigned int nParameters() const {      return (unsigned int) m_stateVector.GetNrows();    };

    //    void positionAndMomentum(TVector3& pos,
    //                             TVector3& mom,
    //                             TMatrixD& cov6D) const {
    //      pos = position();
    //      mom = momentum();
    //      cov6D = posMomCovariance();
    //    };

    //    void positionAndMomentum(TVector3& pos,
    //                             TVector3& mom) const {
    //      pos = position();
    //      mom = momentum();
    //    };

    inline TVector3 position() const { return TVector3( m_stateVector[0], m_stateVector[1], m_stateVector[2]);    };

    inline double x() const {return m_stateVector[0];};
    inline double y() const {return m_stateVector[1];};
    inline double z() const {return m_z;};

    inline TVector3 slopes() const {return TVector3( m_stateVector[2], m_stateVector[3], 1.);};

    inline double tx() const {return m_stateVector[2];};
    inline double ty() const {return m_stateVector[3];};

    inline double qOverP() const {return m_stateVector[4];};
    //    inline virtual double p() const;
    //    inline virtual double pt() const;

    //    TVector3 momentum() const {
    //      TVector3 mom = slopes();
    //      mom *= ( p() / mom.Mag() );
    //      return mom;
    //    };

    //    double qOverPperp() const {
    //      const double tx2 = tx() * tx();
    //      return ( qOverP() * sqrt( (1.+ tx2 + ty()*ty()) / (1. + tx2 ) ) );
    //    };

    //    inline virtual TMatrixD posMomCovariance() const;

    //    TMatrixD errPosition() const;

    inline double errX2() const {return m_covariance(0,0);};

    inline double errY2() const {return m_covariance(1,1);};

    //inline double errZ2() const {return 0.;};

    //    TMatrixD errSlopes() const;

    inline double errTx2() const {return m_covariance(2,2);};

    inline double errTy2() const {return m_covariance(3,3);};

    inline double errQOverP2() const {return m_covariance(4,4);};
    //    inline virtual double errP2() const;
    //    inline virtual TMatrixD errMomentum() const;
    //    inline virtual double errQOverPperp2() const;
    //    inline virtual State* clone() const;

    //void setState(const DecayTreeFitter::StateVector& state) {
    //  m_stateVector = state.parameters() ;
    //};

    inline void setState(const TVectorD& state) {m_stateVector = state;};

    inline void setState(double _x, double _y, double _z, double _tx, double _ty, double _qOverP)
    { m_stateVector[0] = _x;  m_stateVector[1] = _y; m_z = _z; m_stateVector[2] = _tx; m_stateVector[3] = _ty; m_stateVector[4] = _qOverP; };

    inline void setCovariance(const TMatrixDSym& value) {m_covariance = value;};

    inline void setX(double value)  {m_stateVector[0] = value;};
    inline void setY(double value)  {m_stateVector[1] = value;};
    inline void setZ(double value)  {m_z = value;};
    inline void setTx(double value) {m_stateVector[2] = value;};
    inline void setTy(double value) {m_stateVector[3] = value;};
    inline void setQOverP(double value) {m_stateVector[4] = value;};

    //inline virtual void setErrQOverP2(double value) {};
    //inline void linearTransportTo(double az) {};

    inline virtual std::ostream& fillStream(std::ostream& os) const {os<<"DecayTreeFitter::State::fillStream() not implemented";return os;};

    inline unsigned int flags() const { return m_flags;};
    inline void setFlags(unsigned int value) { m_flags = value;};

    inline const TVectorD& stateVector() const { return m_stateVector;};
    inline TVectorD& stateVector() { return m_stateVector;};

    inline const TMatrixDSym& covariance() const { return m_covariance;};
    inline TMatrixDSym& covariance() { return m_covariance;};

  protected:

    unsigned int m_flags;
    double m_z;
    TVectorD m_stateVector;
    TMatrixDSym m_covariance;

  private:

    ClassDef ( State,1 )
  }; // class State

  inline std::ostream& operator<< (std::ostream& str, const State& obj)
  {
    return obj.fillStream(str);
  }

} // namespace 

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------


//inline DecayTreeFitter::State::State(const DecayTreeFitter::StateVector& stateVec,
//                           const TMatrixDSym& cov,
//                           double z) : m_flags(0),
//m_stateVector(stateVec),
//m_covariance(cov),
//m_z(z)
//{
//
//}
//
//inline DecayTreeFitter::State::State(const DecayTreeFitter::StateVector& stateVec,
//                           double z) : m_flags(0),
//m_stateVector(stateVec),
//m_covariance(),
//m_z(z)
//{
//
//
//}








#endif
