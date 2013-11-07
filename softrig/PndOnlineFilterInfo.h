#ifndef PNDONLINEFILTERINFO_H
#define PNDONLINEFILTERINFO_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndOnlineFilterInfo	                                                //
//                                                                      //
// Information Container for Online Filter Results                      //
//                                                                      //
// Author: Klaus Goetzen, GSI, 2013                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "TObject.h"

class PndOnlineFilterInfo : public TObject 
{

 public:

	PndOnlineFilterInfo();
	~PndOnlineFilterInfo();

	int TagTotal();
	int	TagPhiKK() const { return fNTagPhiKK; }
	int TagLamppi() const {return fNTagLamppi; } 
	int TagJpsill() const {return fNTagJpsill; }
	int TagD0Kpi() const {return fNTagD0Kpi; }
	int TagD0Kpipi0() const {return fNTagD0Kpipi0; }
	int TagD0K3pi() const {return fNTagD0K3pi ; }
	int TagDpmKpipi() const {return fNTagDpmKpipi; }
	int TagDpmK2pipi0() const {return fNTagDpmK2pipi0; }
	int TagDpmKspipi0() const {return fNTagDpmKspipi0; }
	int TagDpmKs3pi() const {return fNTagDpmKs3pi; }	
	int TagDsKKpi() const {return fNTagDsKKpi; }
	int TagDsKKpip0() const {return fNTagDsKKpip0; }
	int TagLamcpKpi() const {return fNTagLamcpKpi; }
	int TagEtacKKpi0() const {return fNTagEtacKKpi0; }
	int TagEtacKKspi() const {return fNTagEtacKKspi; }
	int TagEtacetapipi() const {return fNTagEtacetapipi; }
	int TagEtacgg() const {return fNTagEtacgg; }
	int TagChic02pi2pi0() const {return fNTagChic02pi2pi0; }
	int TagChic04pi() const {return fNTagChic04pi; }
	
	void SetTagPhiKK(const int tag) { fNTagPhiKK=tag; }
	void SetTagLamppi(const int tag) { fNTagLamppi=tag; } 
	void SetTagJpsill(const int tag) { fNTagJpsill=tag; }
	void SetTagD0Kpi(const int tag) { fNTagD0Kpi=tag; }
	void SetTagD0Kpipi0(const int tag) { fNTagD0Kpipi0=tag; }
	void SetTagD0K3pi(const int tag) { fNTagD0K3pi=tag; }
	void SetTagDpmKpipi(const int tag) { fNTagDpmKpipi=tag; }
	void SetTagDpmK2pipi0(const int tag) { fNTagDpmK2pipi0=tag; }
	void SetTagDpmKspipi0(const int tag) { fNTagDpmKspipi0=tag; }
	void SetTagDpmKs3pi(const int tag) { fNTagDpmKs3pi=tag; }	
	void SetTagDsKKpi(const int tag) { fNTagDsKKpi=tag; }
	void SetTagDsKKpip0(const int tag) { fNTagDsKKpip0=tag; }
	void SetTagLamcpKpi(const int tag) { fNTagLamcpKpi=tag; }
	void SetTagEtacKKpi0(const int tag) { fNTagEtacKKpi0=tag; }
	void SetTagEtacKKspi(const int tag) { fNTagEtacKKspi=tag; }
	void SetTagEtacetapipi(const int tag) { fNTagEtacetapipi=tag; }
	void SetTagEtacgg(const int tag) { fNTagEtacgg=tag; }
	void SetTagChic02pi2pi0(const int tag) { fNTagChic02pi2pi0=tag; }
	void SetTagChic04pi(const int tag) { fNTagChic04pi=tag; }
	
	void Reset();
	void Print();

 protected:
	
	int SetTagCnt(const int tag);
	 
	bool fTag;
	int fNTagTotal;
	
	int fNTagPhiKK;
	int fNTagLamppi;
	int fNTagJpsill;
	int fNTagD0Kpi;
	int fNTagD0Kpipi0;
	int fNTagD0K3pi;
	int fNTagDpmKpipi;
	int fNTagDpmK2pipi0;
	int fNTagDpmKspipi0;
	int fNTagDpmKs3pi;	
	int fNTagDsKKpi;
	int fNTagDsKKpip0;
	int fNTagLamcpKpi;
	int fNTagEtacKKpi0;
	int fNTagEtacKKspi;
	int fNTagEtacetapipi;
	int fNTagEtacgg;
	int fNTagChic02pi2pi0;
	int fNTagChic04pi;
	
    ClassDef(PndOnlineFilterInfo,1);
    
};


#endif                                           


