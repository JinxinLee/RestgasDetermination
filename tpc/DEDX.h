#ifndef DEDX_HH
#define DEDX_HH
#include "TObject.h"
#include <iostream>

class DEDx  : public TObject
{
	public:
		DEDx();
		virtual ~DEDx();
		//~DEDx();
		friend bool operator< (const DEDx &s1, const DEDx &s2)
		{
			//std::cout << "Comparing: "<< s1.fDx << " " << s2.fDx << " --- " << s1.fDE << " " <<  s2.fDE << std::endl;
			if(s1.fDx && s2.fDx)	{
				return s1.fDE/s1.fDx <= s2.fDE/s2.fDx;
			}
			else 	{
				return s1.fDE <= s2.fDE;
			}
		}
		Double_t fDE;
		Double_t fDx;
		
	public:
	  ClassDef(DEDx,1)
	  
};

#endif
