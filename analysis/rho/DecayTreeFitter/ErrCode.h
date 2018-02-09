// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef DECAYTREEFITTER_ERRORCODE_H
#define DECAYTREEFITTER_ERRORCODE_H 1

#include <iostream>
#include "Rtypes.h"

namespace DecayTreeFitter
{

  class ErrCode
  {
  public:
    enum Status {success=0,
      pocafailure=1,
      baddistance=2,
      inversionerror=4,
      badsetup=8,
      divergingconstraint=16,
      slowdivergingfit=32,
      fastdivergingfit=64} ;

    ErrCode() : _flag(success) {}

    ErrCode(Status aflag) : _flag(aflag) {}
    
    virtual ~ErrCode(){};

    const ErrCode& operator|=(const ErrCode& rhs) {
      _flag |= rhs._flag ; return *this ; }

    bool operator==(const ErrCode& rhs) const {
      return _flag == rhs._flag ; }

    bool operator==(const ErrCode::Status& rhs) const {
      return *this == ErrCode(rhs) ; }

    void reset() { _flag = success ; }
    bool failure() const { return _flag!=success ; }
    unsigned int flag() const { return _flag ; }
    void Print(std::ostream& os);

  private:
    unsigned int _flag ;
    ClassDef ( ErrCode,1 )
  } ;

  //std::ostream& operator<<(std::ostream& os, ErrCode& code) ;

}

#endif
