#include "TComplex.h"
#include "TColor.h"
#include <iostream>
#include "TArrayD.h"

using std::cout;
using std::endl;
using std::flush;

class CRes {
public:
    CRes();
	CRes(Double_t m0, Double_t G0, Double_t m1, Double_t m2, Double_t Amp=1.0, Double_t Ph=0., Int_t J=0);
  
  	void Set(Double_t m0, Double_t G0, Double_t m1, Double_t m2, Double_t Amp=1.0, Double_t Ph=0., Int_t J=0);
  
	Double_t GetM0() { return fm0; }
	Double_t GetG0() { return fG0; }
	Double_t GetAmp() { return fAmp; }
	Double_t GetPh() { return fPh; }
	Int_t    GetJ() { return fJ; }
	
	Double_t GetQ0() {return fq0;}

	Bool_t GetState() {return fOn;} 
	void   SetState(Bool_t st) {fOn=st;}
  
	void SetM0(Double_t m0)   { fm0=m0; }
	void SetG0(Double_t G0)   { fG0=G0; }
	void SetAmp(Double_t Amp) { fAmp=Amp; }
	void SetPh(Double_t Ph)   { fPh=Ph; }
	void SetJ(Int_t J)        { fJ=J; }
  
//	TComplex GetCoeff() { return TComplex(fAmp*cos(fPh),fAmp*sin(fPh));}
	TComplex GetCoeff() { return TComplex(fAmp,fPh,true);}

	void	 SetCacheLen(Int_t n);
	Int_t    GetCacheLen() {return fnCache;}

	void     SetCache(Int_t i, TComplex c);
	TComplex GetCache(Int_t i) {return TComplex(fCacheRe[i],fCacheIm[i]);}

	Bool_t   IsCacheValid() {return fCacheValid;}
	void     SetCacheValid(Bool_t v) {fCacheValid=v;}
   
private:
	Double_t fm0;
	Double_t fG0;
	Double_t fAmp;
	Double_t fPh;
	Int_t    fJ;
	
	Double_t fq0;

	Bool_t   fOn;
	Bool_t   fCacheValid;

	Int_t    fnCache;

	TArrayD  fCacheRe;
	TArrayD  fCacheIm;
};

//**************************************

CRes::CRes()
{
	SetCacheLen(2);
	fOn = kFALSE;
	fCacheValid = kFALSE;
}

//**************************************

CRes::CRes(Double_t m0, Double_t G0, Double_t m1, Double_t m2, Double_t Amp, Double_t Ph, Int_t J) 
{
	Set(m0, G0, m1, m2, Amp, Ph, J);
	SetCacheLen(2);
	fCacheValid = kFALSE;
}

void CRes::Set(Double_t m0, Double_t G0, Double_t m1, Double_t m2, Double_t Amp, Double_t Ph, Int_t J)
{
	fm0  = m0;
	fG0  = G0;
	fAmp = Amp; 
	fPh  = Ph; 
	fJ   = J;
	
	fOn = kTRUE;

	fq0 = 0.0;
	if ( m0 > (m1+m2) )
		fq0 = sqrt( (m0*m0 - (m1+m2)*(m1+m2)) * (m0*m0 - (m1-m2)*(m1-m2)) )/(2.*m0);
}

//**************************************

void CRes::SetCacheLen(Int_t n)
{
	if (n!=fnCache)
	{
		fCacheRe.Set(n);
		fCacheIm.Set(n);

		fCacheRe.Reset();
		fCacheIm.Reset();
		
		fnCache=n;
	}
}


//**************************************


void CRes::SetCache(Int_t i, TComplex c)
{
	if (i>0 && i<fnCache)
	{
		fCacheRe[i] = c.Re();
		fCacheIm[i] = c.Im();
		//fCacheRe.AddAt(c.Re(),i);
		//fCacheIm.AddAt(c.Im(),i);
	}
}


//**************************************
//**************************************
//**************************************

Double_t breakup(Double_t M, Double_t m1, Double_t m2) 
{
	Double_t sum12 = m1+m2;
	Double_t dif12 = m1-m2;
		
	Double_t q = 0.;
	
	if ( M > sum12 ) q = sqrt( (M*M - sum12*sum12) * (M*M - dif12*dif12) )/(2.*M); 
	
	return q;
}

Double_t BarrierP(Double_t q, Double_t q0, Int_t L, Double_t d=5.07) 
{
	Double_t result = 1.0;
	
	Double_t z0 = q0*q0*d*d;
	Double_t z  = q*q*d*d;
	
	switch(L)
	{
	case 1: result = sqrt((1.+z0)/(1.+z));
			break;
			
	case 2: result = sqrt( ( (z0-3.)*(z0-3.)+9.*z0 ) / ( (z-3.)*(z-3.)+9.*z ) );
			break;
			
	default: break;
	}
	
	return result;
}

Double_t Barrier(Double_t q, Int_t L, Double_t d=5.07) 
{
	Double_t result = 1.0;
	
	Double_t z  = q*q*d*d;
	
	switch(L)
	{
	case 1: result = sqrt(2.*z/(1.+z));
			break;
			
	case 2: result = sqrt( ( 13.*z*z ) / ( (z-3.)*(z-3.)+9.*z ) );
			break;
			
	default: break;
	}
	
	return result;
}

Double_t BWGamma(Double_t m, Double_t m0, Double_t Gamma0, Double_t m1, Double_t m2, Int_t L, Double_t d=5.07)
{
	Double_t q0 = breakup(m0,m1,m2);
	Double_t q  = breakup(m,m1,m2);
	
	if (0. == q) return 0.;
	
	Double_t phsp = q/q0;
	
	switch (L)
	{
	case 1: phsp = phsp*phsp*phsp;
			break; 
	case 2: phsp = phsp*phsp*phsp*phsp*phsp;
			break;
	default: break;
	}
	
	Double_t bar = BarrierP(q,q0,L,d);
	
	Double_t G = Gamma0 * phsp * (m0/m) * bar*bar;
	
	return G;
}

TComplex BreitWigner(Double_t m, Double_t m0, Double_t Gamma0, Double_t m1, Double_t m2, Int_t L, Double_t d=5.07)
{
	TComplex I(0.,1.);
	TComplex G(BWGamma(m,m0,Gamma0,m1,m2,L,d));
	
	TComplex BW = G/(m0*m0 - m*m - I*m0*G);
	
	return BW;
}

Double_t lambda(Double_t x, Double_t y, Double_t z)
{
  return x*x + y*y + z*z - 2.*x*y - 2.*y*z - 2.*z*x; 
}

Double_t simin(Int_t i, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t result = 0.;
	
	switch (i)
	{
	case 1:  result = (m2+m3)*(m2+m3);
		break;
	case 2:  result = (m1+m3)*(m1+m3);
		break;
	case 3:  result = (m1+m2)*(m1+m2);
		break;
	}
	return result;
}

Double_t simax(Int_t i, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t result = 0.;
	
	switch (i)
	{
	case 1:  result = (m0-m1)*(m0-m1);
		break;
	case 2:  result = (m0-m2)*(m0-m2);
		break;
	case 3:  result = (m0-m3)*(m0-m3);
		break;
	}
	return result;
}

Double_t s2min(Double_t s1, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s      = m0*m0;
	Double_t lamterm = sqrt( lambda(s1,s,m1*m1) ) * sqrt( lambda(s1, m2*m2, m3*m3) );
	 
	Double_t result  = m1*m1 + m3*m3 + ( (s-s1-m1*m1)*(s1-m2*m2+m3*m3) - lamterm )/(2.*s1);
	
	return result;
}

Double_t s2max(Double_t s1, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s      = m0*m0;
	Double_t lamterm = sqrt( lambda(s1,s,m1*m1) ) * sqrt( lambda(s1, m2*m2, m3*m3) );
	 
	Double_t result  = m1*m1 + m3*m3 + ( (s-s1-m1*m1)*(s1-m2*m2+m3*m3) + lamterm )/(2.*s1);
	
	return result;
}

Double_t s3min(Double_t s1, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s      = m0*m0;
	Double_t lamterm = sqrt( lambda(s1,s,m1*m1) ) * sqrt( lambda(s1, m3*m3, m1*m1) );
	 
	Double_t result  = m1*m1 + m2*m2 + ( (s-s1-m1*m1)*(s1-m1*m1+m2*m2) - lamterm )/(2.*s1);
	
	return result;
}

Double_t s3max(Double_t s1, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s      = m0*m0;
	Double_t lamterm = sqrt( lambda(s1,s,m1*m1) ) * sqrt( lambda(s1, m3*m3, m1*m1) );
	 
	Double_t result  = m1*m1 + m2*m2 + ( (s-s1-m1*m1)*(s1-m1*m1+m3*m3) + lamterm )/(2.*s1);
	
	return result;
}

Double_t s1min(Double_t s2, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s      = m0*m0;
	Double_t lamterm = sqrt( lambda(s2,s,m2*m2) ) * sqrt( lambda(s2, m3*m3, m1*m1) );
	 
	Double_t result  = m2*m2 + m3*m3 + ( (s-s2-m2*m2)*(s2-m1*m1+m3*m3) - lamterm )/(2.*s2);
	
	return result;
}

Double_t s1max(Double_t s2, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s      = m0*m0;
	Double_t lamterm = sqrt( lambda(s2,s,m2*m2) ) * sqrt( lambda(s2, m1*m1, m3*m3) );
	 
	Double_t result  = m2*m2 + m3*m3 + ( (s-s2-m2*m2)*(s2-m1*m1+m3*m3) + lamterm )/(2.*s2);
	
	return result;
}

void palette2(Int_t mode = 0)
{  
	const int ncol=99;
	static Int_t col0[99], col1[99], col2[99];
	static Bool_t initialized = kFALSE;

	if (!initialized)
	{	
		{
			Double_t blu[3] = {1,0.2,0};
			Double_t red[3] = {1,0.2,0};
			Double_t grn[3] = {1,0.2,0};
			Double_t stp[3] = {0,0.5,1};
			
			Int_t FI = TColor::CreateGradientColorTable(3,stp,red,grn,blu,ncol);
			for (int i=0; i<ncol; i++) col0[i] = FI+i;
		}
		{
			Double_t blu2[6] = {1,1,1,0,0,0};
			Double_t red2[6] = {1,0,0,0,1,1};
			Double_t grn2[6] = {1,0,1,1,1,0};
			Double_t stp2[6] = {0.0,0.12,0.34,0.56,0.78,1.0};
			
			Int_t FI = TColor::CreateGradientColorTable(6,stp2,red2,grn2,blu2,ncol) ;
			for (int i=0; i<ncol; i++) col1[i] = FI+i;
		}
		{
			Double_t blu3[5] = {0,0,1,0,0};
			Double_t red3[5] = {0,1,1,0,0};
			Double_t grn3[5] = {0,0,1,1,0};
			Double_t stp3[5] = {0.,0.25,0.5,0.75,1.0};
			
			Int_t FI = TColor::CreateGradientColorTable(5,stp3,red3,grn3,blu3,ncol);
			for (int i=0; i<ncol; i++) col2[i] = FI+i;
		}
		initialized = kTRUE;
	}

	if (mode==0) 
		gStyle->SetPalette(ncol,col0);
	else if (mode==1)
		gStyle->SetPalette(ncol,col1);
	else 
		gStyle->SetPalette(ncol,col2);

}

Double_t Z_Ralt(Double_t s, Double_t smin, Double_t smax, Int_t J, double m_R=0., double m_ab=0., double m_c=0.)
{
	Double_t res=1.0;
	
	Double_t x = (s-smin)/(smax-smin)*2.-1.0;
	
	double relcorr = 1.0, xi2=1.;
	
	if (m_R>1e-6)
	{
		xi2 = pow((m_R*m_R + m_ab*m_ab - m_c*m_c)/(2*m_ab*m_R),2)-1;
	}
	
	//switch(J)
	//{
	//case 1: res = x;
			//if (m_R>1e-6) relcorr = sqrt(1+xi2);
			//break;
	//case 2: res= 1.5*x*x-0.5;
			//if (m_R>1e-6) relcorr = sqrt(xi2+1.5);
			//break;
	//}
	
	//return relcorr*fabs(res);  // *** new: return now sqrt(angle) to cope for computation of the intensity = amp*amp
	
	switch(J)
	{
	case 1: res = 3.*x*x;
			if (m_R>1e-6) relcorr = sqrt(1.+xi2);
			break;
	case 2: res= 5.*(x*x-1./3.)*(x*x-1./3.)*9./4.;
			if (m_R>1e-6) relcorr = sqrt(xi2+1.5);
			break;
	}
	
	return relcorr*sqrt(res);  // *** new: return now sqrt(angle) to cope for computation of the intensity = amp*amp
}

Double_t Z_R(Double_t s, Double_t sab, Double_t sac, Double_t sbc, int i, Double_t mR, Int_t J, double fma, double fmb, double fmc)
{
	double res = 1.;

	if (J==0) 
		return res;	

	double mR2 = 2*mR;

	double mA2=fma*fma, mB2=fmb*fmb, mC2=fmc*fmc;
	
	//switch (i)
	//{
	//case 1: mA2 = fm2*fm2; mB2 = fm3*fm3; mC2 = fm1*fm1;
		//break;
	//case 2: mA2 = fm1*fm1; mB2 = fm3*fm3; mC2 = fm2*fm2;
		//break;
	//case 3: mA2 = fm1*fm1, mB2 = fm2*fm2, mC2 = fm3*fm3;
		//break;
	//}
	
	switch(J)
	{
	case 1: res = sac - sbc + (s-mC2)*(mA2-mB2)/mR2; //(mAC*mAC-mBC*mBC+((mD*mD-mC*mC)*(mB*mB-mA*mA)/(mdenom*mdenom)))
		break;
	case 2: res = pow(sbc - sac + ((s-mC2)*(mA2-mB2)/mR),2) - 1./3.*(sab-2*s-2*mC2 + pow((s-mC2)/mR,2))*(sab-2*mA2-2*mB2 + pow((mA2-mB2)/mR,2));
		break;
	}
	
	return res;
}

TComplex getAmp(CRes *r, Double_t m, Double_t m1, Double_t m2, Double_t qR, Double_t qM)
{
	TComplex bw1  = BreitWigner(m, r->GetM0(), r->GetG0(), m1, m2, r->GetJ());
	TComplex c   = r->GetCoeff();
	c*=1./sqrt(r->GetG0()); // weight taking into account width (give narrow resonances more weight)
	Double_t bar1 = Barrier(qR,r->GetJ());
	Double_t bar2 = BarrierP(qM,breakup(r->GetM0(),m1,m2),r->GetJ());
	return bar1*bar2*c*bw1;
}

TComplex getAmpEvt(CRes *r, double sab, double sac, double sbc, double ma, double mb, double mc)
{
  double mAB2=sab;
  double mBC2=sbc;
  double mAC2=sac;
  double mA2=ma*ma; 
  double mB2=mb*mb; 
  double mC2=mc*mc;
  double mD2= sab + sac +sbc - mA2 -mB2 -mC2;
  
  double mR=r->GetM0();
  double mR2 = mR*mR;
  double gammaR=r->GetG0();
  double mdenom = mR;

  double pAB=sqrt( (((mAB2-mA2-mB2)*(mAB2-mA2-mB2)/4.0) - mA2*mB2)/(mAB2));
  double pR=sqrt( (((mR2-mA2-mB2)*(mR2-mA2-mB2)/4.0) - mA2*mB2)/(mR2));
  double pD= (((mD2-mR2-mC2)*(mD2-mR2-mC2)/4.0) - mR2*mC2)/(mD2);
  if ( pD>0 ) { pD=sqrt(pD); } else {pD=0;}
  double pDAB=sqrt( (((mD2-mAB2-mC2)*(mD2-mAB2-mC2)/4.0) - mAB2*mC2)/(mD2));

  double fR=1;
  double fD=1;
  int power=0;
  int _spin = r->GetJ();
  switch (_spin) {
  case 0:
    fR=1.0;
    fD=1.0;
    power=1;
    break;
  case 1:
    fR=sqrt(1.0+1.5*1.5*pR*pR)/sqrt(1.0+1.5*1.5*pAB*pAB);
    fD=sqrt(1.0+5.0*5.0*pD*pD)/sqrt(1.0+5.0*5.0*pDAB*pDAB);
    power=3;
    break;
  case 2:
    fR = sqrt( (9+3*pow((1.5*pR),2)+pow((1.5*pR),4))/(9+3*pow((1.5*pAB),2)+pow((1.5*pAB),4)) );
    fD = sqrt( (9+3*pow((5.0*pD),2)+pow((5.0*pD),4))/(9+3*pow((5.0*pDAB),2)+pow((5.0*pDAB),4)) );
    power=5;
    break;
   }

  double gammaAB= gammaR*pow(pAB/pR,power)*(mR/sqrt(mAB2))*fR*fR;

  TComplex ampl=sqrt(r->GetG0())*r->GetCoeff()*fR*fD/(mR2-mAB2-TComplex(0.0,mR*gammaAB));

  //double gammaAB= gammaR*pow(pAB/pR,power)*(mR/mAB)*fR*fR;
  //switch (_spin) {
  //case 0:
  //  ampl= r->GetCoeff()*fR*fD/(mR2-mAB2-TComplex(0.0,mR*gammaAB));
  //  break;
  //case 1:
  //  ampl=_r->GetCoeff()*(fR*fD*(mAC2-mBC2+((mD2-mC2)*(mB2-mA2)/(mdenom*mdenom)))/
  //     (mR2-mAB2-TComplex(0.0,mR*gammaAB)));
  //  break;
  //case 2:
  //  ampl=r->GetCoeff()*fR*fD/(mR2-mAB2-TComplex(0.0,mR*gammaAB))*
  //    (pow((mBC2-mAC2+(mD2-mC2)*(mA2-mB2)/(mdenom*mdenom)),2)-
  //     (1.0/3.0)*(mAB2-2*mD2-2*mC2+pow((mD2- mC2)/mdenom, 2))*
  //     (mAB2-2*mA2-2*mB2+pow((mA2-mB2)/mdenom,2))); 
  //break;

  //}

  return ampl;

}
//Double_t sumAmps(TComplex &A, CRes **r, Double_t s, Double_t ss, Double_t smin, Double_t smax, Double_t m1, Double_t m2, Double_t qR)
//{
	//Double_t m=sqrt(s);
	//Double_t A_in=0.0;

	//Double_t qM = breakup(m,m1,m2);
	
	//for (int i=0; i<2; i++)
	//{
		//if (r[i]->GetState()==kFALSE) continue;

		//TComplex Atmp = getAmp(r[i], m, m1, m2, qR, qM);
		//Double_t Z   = Z_R(ss, smin, smax, r[i]->GetJ());
		
		//Atmp *= Z;
		
		//A    += Atmp;  // coherent
		//A_in += Atmp.Rho();
	//}
	
	//return A_in;
//}

