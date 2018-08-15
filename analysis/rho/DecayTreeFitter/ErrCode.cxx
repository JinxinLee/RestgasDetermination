// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include "ErrCode.h"


using namespace DecayTreeFitter;
ClassImp(ErrCode);

// std::ostream& operator<<(std::ostream& os, ErrCode& ec)
// {
//   unsigned int flag = ec.flag() ;
//   os << "flag=" << flag << " " ;
//   if(flag) {
//     if(flag&ErrCode::pocafailure) os << "pocafailure " ;
//     if(flag&ErrCode::baddistance) os << "baddistance " ;
//     if(flag&ErrCode::inversionerror) os << "inversionerror " ;
//     if(flag&ErrCode::badsetup) os << "badsetup " ;
//     if(flag&ErrCode::divergingconstraint) os << "divergingconstraint " ;
//     if(flag&ErrCode::slowdivergingfit) os << "slowdivergingfit " ;
//     if(flag&ErrCode::fastdivergingfit) os << "fastdivergingfit " ;
//   } else {
//     os << "success " ;
//   }
//   return os ;
// }



void ErrCode::Print(std::ostream& os)
{
  os << "flag=" << _flag << " " ;
  if(_flag) {
    if(_flag&ErrCode::pocafailure) os << "pocafailure " ;
    if(_flag&ErrCode::baddistance) os << "baddistance " ;
    if(_flag&ErrCode::inversionerror) os << "inversionerror " ;
    if(_flag&ErrCode::badsetup) os << "badsetup " ;
    if(_flag&ErrCode::divergingconstraint) os << "divergingconstraint " ;
    if(_flag&ErrCode::slowdivergingfit) os << "slowdivergingfit " ;
    if(_flag&ErrCode::fastdivergingfit) os << "fastdivergingfit " ;
  } else {
    os << "success " ;
  }
  return;
}
