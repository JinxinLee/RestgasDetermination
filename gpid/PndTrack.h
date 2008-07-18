#ifndef PND_TRACK_HH
#define PND_TRACK_HH
#include "TObject.h"
/*
any pointer data member here has to be deleted by destroy func
*/
class PndTrack  : public TObject{

private:
	Double_t fTpcDeDx;
	Double_t fMvdDeDx;
	Double_t fP;
        Double_t fEmcE;
        Double_t fTof;
        Double_t fSpeed;
        Double_t fLen;
        void destroy();
public:
	PndTrack();
        ~PndTrack();
	inline Double_t GetTpcDeDx()		const { return fTpcDeDx;}
	inline Double_t GetMvdDeDx()		const { return fMvdDeDx;}
	inline Double_t GetP()			const { return fP;}
	inline Double_t GetEmcE()		const { return fEmcE;}
	inline Double_t GetTof()		const { return fTof;}
	inline Double_t GetSpeed()		const { return fSpeed;}
	inline Double_t GetLen()		const { return fLen;}
	inline void SetTpcDeDx(Double_t f) 	{ fTpcDeDx = f;}
	inline void SetMvdDeDx(Double_t f) 	{ fMvdDeDx = f;}
	inline void SetP(Double_t f) 		{ fP = f;}
	inline void SetEmcE(Double_t f) 	{ fEmcE = f;}
	inline void SetTof(Double_t f) 		{ fTof = f;}
	inline void SetSpeed(Double_t f) 	{ fSpeed = f;}
	inline void SetLen(Double_t f) 		{ fLen = f;}
	ClassDef(PndTrack,1);
};
#endif
