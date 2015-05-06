//==========================================================================
// File and Version Information:
// 	$Id: FsmDetTypes.hh,v 1.4 2006/10/05 14:05:53 aida Exp $
//
//--------------------------------------------------------------------------
//  Description:
//  this structure is helpful for the easy treatment of  the individual subdetectors. 
//  Attention: the correct order for "enum" and "FsmDetName::name" is mandatory"
//
//  Author List:
//	Bertram Kopf            (Original author)
//
//
//==========================================================================


#ifndef FSMDETTYPES_H
#define FSMDETTYPES_H

struct FsmDetEnum {
  enum {Mvd=0, Mvd2, Tpc, Stt, Tof, Drc, EmcBarrel, EmcBwCap, EmcFwCap, EmcFS, Muo, Dch1, Dch2, Dch3, Dch4, Dch5, Dch6, Dch7, Dch8, FwkRch, CmpDet, numSystems}; // attention: if you want to add/remove detectors don't forget to modify FsmDetName; keep an eye on the correct order!!!

};

  struct FsmDetName {
    static const std::string& name(unsigned int t)
    {
      static std::string detName[FsmDetEnum::numSystems+1]={"Mvd","Mvd2","Tpc","Stt", "Tof", "Drc","EmcBarrel","EmcBwCap","EmcFwCap","EmcFS","Muo","Dch1","Dch2","Dch3","Dch4","Dch5","Dch6","Dch7","Dch8","FwkRch","CmpDet","unknown"};
      if (t>=FsmDetEnum::numSystems) t=FsmDetEnum::numSystems;
      return detName[t];
    }
  };


#endif
