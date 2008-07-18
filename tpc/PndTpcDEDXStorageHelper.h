#ifndef PNDTPCDEDXSTORAGEHELPER_H
#define PNDTPCDEDXSTORAGEHELPER_H

#include <iostream>

class PndTpcDEDXStorageHelper		
{
public:
     PndTpcDEDXStorageHelper():fde(0.),fdx(0.) {}
	PndTpcDEDXStorageHelper(double de, double dx):fde(de),fdx(dx) {}
	void SetDEDX(double de, double dx);
	void GetDEDX(double &de, double &dx) const;
	void SetEnergyLoss(double de) {fde=de;}
	void SetLength(double dx) {fdx=dx;}
	double GetEnergyLoss() const { return fde; }
	double GetLength() const { return fdx; }
	friend bool operator< (const PndTpcDEDXStorageHelper &s1, const PndTpcDEDXStorageHelper &s2)
	{
		//std::cout << "Comparing: "<< s1.fdx << " " << s2.fdx << " --- " << s1.fdE << " " <<  s2.fde << std::endl;
		if(s1.fdx && s2.fdx)	{
			return s1.fde/s1.fdx <= s2.fde/s2.fdx;
		}
		else 	{
			return s1.fde <= s2.fde;
		}
	}
private:
	double fde;
	double fdx;	
};

#endif

