//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndOnlineFilterInfo	                                                //
//                                                                      //
// Information Container for Online Filter Results                      //
//                                                                      //
// Author: Klaus Goetzen, GSI, 2013                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "PndOnlineFilterInfo.h"


PndOnlineFilterInfo::PndOnlineFilterInfo() :	
	bool fTag(false),
	fNTagTotal(0),
	fNTagPhiKK(0),
	fNTagLamppi(0),
	fNTagJpsill(0),
	fNTagD0Kpi(0),
	fNTagD0Kpipi0(0),
	fNTagD0K3pi(0),
	fNTagDpmKpipi(0),
	fNTagDpmK2pipi0(0),
	fNTagDpmKspipi0(0),
	fNTagDpmKs3pi(0),	
	fNTagDsKKpi(0),
	fNTagDsKKpip0(0),
	fNTagLamcpKpi(0),
	fNTagEtacKKpi0(0),
	fNTagEtacKKspi(0),
	fNTagEtacetapipi(0),
	fNTagEtacgg(0),
	fNTagChic02pi2pi0(0),
	fNTagChic04pi(0)
{
}


PndOnlineFilterInfo::~PndOnlineFilterInfo()
{
}

int PndOnlineFilterInfo::SetTagCnt(int tag)
{
	if (tag>0) 
	{
		fTag=true;
		fNTagTotal += tag;
	} 
	return tag;
}

void PndOnlineFilterInfo::Reset()
{
	fTag=false;
	fNTagTotal=0;
	fNTagPhiKK=0;
	fNTagLamppi=0;
	fNTagJpsill=0;
	fNTagD0Kpi=0;
	fNTagD0Kpipi0=0;
	fNTagD0K3pi=0;
	fNTagDpmKpipi=0;
	fNTagDpmK2pipi0=0;
	fNTagDpmKspipi0=0;
	fNTagDpmKs3pi=0;	
	fNTagDsKKpi=0;
	fNTagDsKKpip0=0;
	fNTagLamcpKpi=0;
	fNTagEtacKKpi0=0;
	fNTagEtacKKspi=0;
	fNTagEtacetapipi=0;
	fNTagEtacgg=0;
	fNTagChic02pi2pi0=0;
	fNTagChic04pi=0;
}

void PndOnlineFilterInfo::Print()
{
  std::cout <<"PndOnlineFilterInfo found triggers:   "<<std::endl;
  std::cout <<"fNTagTotal        : "<< fNTagTotal        <<std::endl;
  std::cout <<"fNTagPhiKK        : "<< fNTagPhiKK        <<std::endl;
  std::cout <<"fNTagLamppi       : "<< fNTagLamppi       <<std::endl;
  std::cout <<"fNTagJpsill       : "<< fNTagJpsill       <<std::endl;
  std::cout <<"fNTagD0Kpi        : "<< fNTagD0Kpi        <<std::endl;
  std::cout <<"fNTagD0Kpipi0     : "<< fNTagD0Kpipi0     <<std::endl;
  std::cout <<"fNTagD0K3pi       : "<< fNTagD0K3pi       <<std::endl;
  std::cout <<"fNTagDpmKpipi     : "<< fNTagDpmKpipi     <<std::endl;
  std::cout <<"fNTagDpmK2pipi0   : "<< fNTagDpmK2pipi0   <<std::endl;
  std::cout <<"fNTagDpmKspipi0   : "<< fNTagDpmKspipi0   <<std::endl;
  std::cout <<"fNTagDpmKs3pi     : "<< fNTagDpmKs3pi     <<std::endl;
  std::cout <<"fNTagDsKKpi       : "<< fNTagDsKKpi       <<std::endl;
  std::cout <<"fNTagDsKKpip0     : "<< fNTagDsKKpip0     <<std::endl;
  std::cout <<"fNTagLamcpKpi     : "<< fNTagLamcpKpi     <<std::endl;
  std::cout <<"fNTagEtacKKpi0    : "<< fNTagEtacKKpi0    <<std::endl;
  std::cout <<"fNTagEtacKKspi    : "<< fNTagEtacKKspi    <<std::endl;
  std::cout <<"fNTagEtacetapipi  : "<< fNTagEtacetapipi  <<std::endl;
  std::cout <<"fNTagEtacgg       : "<< fNTagEtacgg       <<std::endl;
  std::cout <<"fNTagChic02pi2pi0 : "<< fNTagChic02pi2pi0 <<std::endl;
  std::cout <<"fNTagChic04pi     : "<< fNTagChic04pi     <<std::endl;
}


ClassImp(PndOnlineFilterInfo)

