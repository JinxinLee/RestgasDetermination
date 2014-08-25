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

	bool Tagged() const {return fTag;}
	int TagTotal() const { return fNTagTotal; }
	int	TagPhiKK() const { return fNTagPhiKK; }
	int TagLamppi() const {return fNTagLamppi; } 
	int TagJpsi2e() const {return fNTagJpsi2e; }
	int TagJpsi2mu() const {return fNTagJpsi2mu; }
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
	int TagChic02pi2K() const {return fNTagChic02pi2K; }
	int Tag2e() const {return fNTag2e; }
	int Tag2mu() const {return fNTag2mu; }
	int Tag2gam() const {return fNTag2gam; }
	
	
	void SetTagPhiKK(const int tag) { SetTagCnt(fNTagPhiKK=tag); }
	void SetTagLamppi(const int tag) {SetTagCnt( fNTagLamppi=tag); } 
	void SetTagJpsi2e(const int tag) { SetTagCnt(fNTagJpsi2e=tag); }
	void SetTagJpsi2mu(const int tag) { SetTagCnt(fNTagJpsi2mu=tag); }
	void SetTagD0Kpi(const int tag) { SetTagCnt(fNTagD0Kpi=tag); }
	void SetTagD0Kpipi0(const int tag) { SetTagCnt(fNTagD0Kpipi0=tag); }
	void SetTagD0K3pi(const int tag) { SetTagCnt(fNTagD0K3pi=tag); }
	void SetTagDpmKpipi(const int tag) { SetTagCnt(fNTagDpmKpipi=tag); }
	void SetTagDpmK2pipi0(const int tag) { SetTagCnt(fNTagDpmK2pipi0=tag); }
	void SetTagDpmKspipi0(const int tag) { SetTagCnt(fNTagDpmKspipi0=tag); }
	void SetTagDpmKs3pi(const int tag) { SetTagCnt(fNTagDpmKs3pi=tag); }	
	void SetTagDsKKpi(const int tag) { SetTagCnt(fNTagDsKKpi=tag); }
	void SetTagDsKKpip0(const int tag) { SetTagCnt(fNTagDsKKpip0=tag); }
	void SetTagLamcpKpi(const int tag) { SetTagCnt(fNTagLamcpKpi=tag); }
	void SetTagEtacKKpi0(const int tag) { SetTagCnt(fNTagEtacKKpi0=tag); }
	void SetTagEtacKKspi(const int tag) { SetTagCnt(fNTagEtacKKspi=tag); }
	void SetTagEtacetapipi(const int tag) {SetTagCnt( fNTagEtacetapipi=tag); }
	void SetTagEtacgg(const int tag) {SetTagCnt( fNTagEtacgg=tag); }
	void SetTagChic02pi2pi0(const int tag) { SetTagCnt(fNTagChic02pi2pi0=tag); }
	void SetTagChic04pi(const int tag) { SetTagCnt(fNTagChic04pi=tag); }
	void SetTagChic02pi2K(const int tag) { SetTagCnt(fNTagChic02pi2K=tag); }
	void SetTag2e(const int tag) { SetTagCnt(fNTag2e=tag); }
	void SetTag2mu(const int tag) { SetTagCnt(fNTag2mu=tag); }
	void SetTag2gam(const int tag) { SetTagCnt(fNTag2gam=tag); }

	void Reset();
	void Print();

 protected:
	
	int SetTagCnt(const int tag);
	 
	bool fTag;
	int fNTagTotal;
	
	int fNTagPhiKK;
	int fNTagLamppi;
	
	int fNTagJpsi2e;
	int fNTagJpsi2mu;
	
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
	int fNTagChic02pi2K;
	
	int fNTag2e;
	int fNTag2mu;
	int fNTag2gam;
	
    ClassDef(PndOnlineFilterInfo,1);
    
};


#endif                                           


