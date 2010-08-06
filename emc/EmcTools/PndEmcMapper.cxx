/////////////////////////////////////////////////////////////
// Description:
//	EMC Mapper class. Map crystal index (fDetectorId) to two coordinate index.
//
// Author List:
// Dima Melnychuk
// 27/04/2007 Stefano Spataro (encoding for the Fwd Endcup)
//
/////////////////////////////////////////////////////////////// 

#include "PndEmcMapper.h"
#include "PndEmcTwoCoordIndex.h"
#include "FairBaseParSet.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
		
#include "TROOT.h"
#include "TFile.h"
#include <iostream>
#include <fstream>		

using namespace std;

PndEmcMapper* PndEmcMapper::_instance = 0;
Int_t PndEmcMapper::fMapVersion = 0;
    
void PndEmcMapper::Init(Int_t mapVersion)
{
	if (mapVersion==0)
	{
		cout<<"Emc mapper version 0 does not exist"<<endl;
	}
	else if (mapVersion!=fMapVersion)
	{	
		fMapVersion=mapVersion;
		switch (mapVersion) {
			case 1:
				_instance = new PndEmcMapperGeoAllDat();
				break;
			case 2:
				_instance = new PndEmcMapperGeo1245Dat3Root();
				break;
			case 3:
				_instance = new PndEmcMapperGeo3Dat();
				break;
			case 4:
				_instance = new PndEmcMapperGeo3Root();
				break;
			case 5:
				_instance = new PndEmcMapperGeo4Root();
				break;
			case 6:
				_instance = new PndEmcMapperGeo125Dat34Root();
				break;
			case 7:
				_instance = new PndEmcMapperGeo1235Dat4Root();
				break;
			case 8:
				_instance = new PndEmcMapperGeo5Root();
				break;
			case 9:
				_instance = new PndEmcMapperGeo12Dat345Root();
			    break;
			case 10:
				_instance = new PndEmcMapperGeoProto60Root();
				break;
			default :
				cout<<"Emc Mapper version "<<mapVersion<<" is not defined"<<endl;
		} 
	}
}

PndEmcMapper* PndEmcMapper::Instance ()
{
  if (_instance == 0) {
		cout<<"Emc mapper should be initialised first with PndEmcMapper::Init()"<<endl;
		return 0;
	}
	return _instance;
}

// -----   Destructor   -----------------------------------
PndEmcMapper::~PndEmcMapper()
{
	delete _instance;
	for(std::map<Int_t,PndEmcTwoCoordIndex* >::iterator iter = fIntTwoCoordMap.begin();
	    iter != fIntTwoCoordMap.end(); ++iter){
		delete (*iter).second;
	}
	fIntTwoCoordMap.clear();
}
// --------------------------------------------------------

PndEmcMapperGeoAllDat::PndEmcMapperGeoAllDat()
{
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	// Crys 1-5000; copyNo 1-20; nRow 1-100, nMod 1-6
	// 72(iTheta)x160(iPhi) for barrel part
	// index iTheta from 1 (backward part) till 72 (forward)
	for (Int_t module=1; module<=6; module++)
	{
		if (module ==2)
		{
		for (Int_t row=1; row<=29;row++)
			for (Int_t crystal=1; crystal<=10;crystal++)
				for (Int_t copy=1; copy<=16;copy++)
				{
					iPhi=(11-crystal)+(copy-1)*10;
					iTheta=-row+30;
				
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
					fIntTwoCoordMap[detId]=_tci;
				}	
		}
		else if (module ==1)
		{
		for (Int_t row=1; row<=43;row++)
			for (Int_t crystal=1; crystal<=10;crystal++)
				for (Int_t copy=1; copy<=16;copy++)
				{
					iPhi=(11-crystal)+(copy-1)*10;
					iTheta=row+29;
					
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
					fIntTwoCoordMap[detId]=_tci;
				}
		}
		if (module ==3)
		{
			for (Int_t row=1; row<=40;row++)
						for (Int_t crystal=1; crystal<=40;crystal++)
				for (Int_t copy=1; copy<=4;copy++)
				{
					if (copy==1) {  iX = -row+1; iY =  crystal;   }
					if (copy==2) {  iX = -row+1; iY = -crystal+1; }
					if (copy==3) {  iX =  row;   iY = -crystal+1; }
					if (copy==4) {  iX =  row;   iY =  crystal;   }
					
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					
					_tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
					fIntTwoCoordMap[detId]=_tci;
				}	
		}			
		if (module ==4)
		{
		for (Int_t row=1; row<=18;row++)
			for (Int_t crystal=1; crystal<=18;crystal++)
				for (Int_t copy=1; copy<=4;copy++)
				{
					if (copy==1) {  iX = -row+1; iY =  crystal;   }
					if (copy==2) {  iX = -row+1; iY = -crystal+1; }
					if (copy==3) {  iX =  row;   iY = -crystal+1; }
					if (copy==4) {  iX =  row;   iY =  crystal;   }
				
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					
					_tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
					fIntTwoCoordMap[detId]=_tci;
				}	
		}
		if (module ==5)
		{
			for (Int_t row=1; row<=14;row++)
				for (Int_t crystal=1; crystal<=7;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1) {  iX = -row+1; iY =  crystal;   }
						if (copy==2) {  iX = -row+1; iY = -crystal+1; }
						if (copy==3) {  iX =  row;   iY = -crystal+1; }
						if (copy==4) {  iX =  row;   iY =  crystal;   }
					
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
						
						_tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
						fIntTwoCoordMap[detId]=_tci;
					}	
		}
		if (module ==6)
		{
			for (Int_t row=1; row<=5;row++)
			for (Int_t crystal=1; crystal<=5;crystal++)
			{
				Int_t copy = 1;
				iX = row; 
				iY =  crystal; 
				
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
				
				_tci=new PndEmcTwoCoordIndex(iX+550,iY+550,detId);
				fIntTwoCoordMap[detId]=_tci;
			}	
		}
	}
}

PndEmcMapperGeo1245Dat3Root::PndEmcMapperGeo1245Dat3Root()
{
   // *** All EMC: Barrel, BwEndCap, FS --> read from "emc_module1245.dat" file
   // *** and new version of FwEndCap   --> read from "emc_module3.root" file
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	
	// Crys 1-5000; copyNo 1-20; nRow 1-100, nMod 1-6
	// 72(iTheta)x160(iPhi) for barrel part
	// index iTheta from 1 (backward part) till 72 (forward)
	for (Int_t module=1; module<=6; module++)
	{
		if (module ==2)
		{
			for (Int_t row=1; row<=29;row++)
				for (Int_t crystal=1; crystal<=10;crystal++)
					for (Int_t copy=1; copy<=16;copy++)
					{
						iPhi=(11-crystal)+(copy-1)*10;
						iTheta=-row+30;
						
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
						_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
						fIntTwoCoordMap[detId]=_tci;
					}
		}
		else if (module ==1)
		{
			for (Int_t row=1; row<=43;row++)
				for (Int_t crystal=1; crystal<=10;crystal++)
					for (Int_t copy=1; copy<=16;copy++)
					{
						iPhi=(11-crystal)+(copy-1)*10;
						iTheta=row+29;
						
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
						_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
						fIntTwoCoordMap[detId]=_tci;
					}
		}
		if (module ==3)
		{
			for (Int_t row=1; row<=36;row++)
				for (Int_t crystal=1; crystal<=36;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1)   {	iX = -row+1; iY =  crystal;   }
						if (copy==2)   {	iX = -row+1; iY = -crystal+1; }
						if (copy==3)   {	iX =  row;   iY = -crystal+1; }
						if (copy==4)   {     iX =  row;   iY =  crystal;   }			   
						
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
						
						_tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
						fIntTwoCoordMap[detId]=_tci;
					}
		}

		if (module ==4)
		{
			for (Int_t row=1; row<=18;row++)
				for (Int_t crystal=1; crystal<=18;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1)   {     iX = -row+1; iY =  crystal;   }
						if (copy==2)   {	iX = -row+1; iY = -crystal+1; }
						if (copy==3)   {	iX =  row;   iY = -crystal+1; }
						if (copy==4)   {	iX =  row;   iY =  crystal;   }
						
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
						
						_tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
						fIntTwoCoordMap[detId]=_tci;
					} 
		}
		
		if (module ==5)
		{
			for (Int_t row=1; row<=14;row++)
				for (Int_t crystal=1; crystal<=7;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1)  {	iX = -row+1; iY =  crystal;   }			   
						if (copy==2)  {	iX = -row+1; iY = -crystal+1; }
						if (copy==3)  {	iX =  row;   iY = -crystal+1; }
						if (copy==4)  {	iX =  row;   iY =  crystal;   }
						
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
						
						_tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
						fIntTwoCoordMap[detId]=_tci;
					}
		}
 
		if (module ==6)
		{
			for (Int_t row=1; row<=5;row++)
				for (Int_t crystal=1; crystal<=5;crystal++)
		      {
					Int_t copy = 1;
					iX = row;
					iY =  crystal;
					
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
					
					_tci=new PndEmcTwoCoordIndex(iX+550,iY+550,detId);
					fIntTwoCoordMap[detId]=_tci;
		      }
		} 
	}
}


PndEmcMapperGeo3Dat::PndEmcMapperGeo3Dat()
{
   // *** for testing of FwEndCap ONLY --> from "emc_module3.dat" file ***
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;

	Int_t module=3;
	for (Int_t row=1; row<=40;row++)
	  for (Int_t crystal=1; crystal<=40;crystal++)
	    for (Int_t copy=1; copy<=4;copy++)
	      {
		 if (copy==1) {  iX = -row+1; iY =  crystal;   }
		 if (copy==2) {  iX = -row+1; iY = -crystal+1; }
		 if (copy==3) {  iX =  row;   iY = -crystal+1; }
		 if (copy==4) {  iX =  row;   iY =  crystal;   }
		 
		 detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
		 
		 _tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
		 fIntTwoCoordMap[detId]=_tci;
	      }	

}

PndEmcMapperGeo3Root::PndEmcMapperGeo3Root()
{
   // *** for testing of FwEndCap ONLY --> from "emc_module3.root" file ***
  	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;

	Int_t module=3;
	for (Int_t row=1; row<=36;row++)
		for (Int_t crystal=1; crystal<=36;crystal++)
			for (Int_t copy=1; copy<=4;copy++)
	      {
				if (copy==1) {  iX = -row+1; iY =  crystal;   }
				if (copy==2) {  iX = -row+1; iY = -crystal+1; }
				if (copy==3) {  iX =  row;   iY = -crystal+1; }
				if (copy==4) {  iX =  row;   iY =  crystal;   }
				
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
				_tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
				fIntTwoCoordMap[detId]=_tci;
	      }	
}

PndEmcMapperGeo4Root::PndEmcMapperGeo4Root()
{
   // *** for testing of BwEndCap ONLY --> from "emc_module4_*.root" file ***
   // 26.02.2009
  	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	
	Int_t module=4;
	for (Int_t row=1; row<=16;row++)
		for (Int_t crystal=1; crystal<=16;crystal++)
			for (Int_t copy=1; copy<=4;copy++)
	      {
				if (copy==1) {  iX = -row+1; iY =  crystal;   }
				if (copy==2) {  iX = -row+1; iY = -crystal+1; }
				if (copy==3) {  iX =  row;   iY = -crystal+1; }
				if (copy==4) {  iX =  row;   iY =  crystal;   }
				
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
				_tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
				fIntTwoCoordMap[detId]=_tci;
	      }
}

PndEmcMapperGeo5Root::PndEmcMapperGeo5Root()
{
   // *** for testing of Fsc ONLY --> from "emc_module5_*.root" file ***
   // 20.04.2010
  	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;

	Int_t module=5;
	for (Int_t row=1; row<=28;row++)
		for (Int_t crystal=1; crystal<=54;crystal++)
	      {
				Int_t copy = 1;
				iX = row;
				iY =  crystal;
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
				_tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
				fIntTwoCoordMap[detId]=_tci;
	      }
}

PndEmcMapperGeo125Dat34Root::PndEmcMapperGeo125Dat34Root()
{
   // *** for Barrel (like it is), _new_ FwEndCap & _new_ BwEndCap
   // 26.02.2009
  	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	for (Int_t module=1; module<=6; module++)
	{
		if (module ==2)
		{
		for (Int_t row=1; row<=29;row++)
			for (Int_t crystal=1; crystal<=10;crystal++)
				for (Int_t copy=1; copy<=16;copy++)
				{
					iPhi=(11-crystal)+(copy-1)*10;
					iTheta=-row+30;
				
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
					fIntTwoCoordMap[detId]=_tci;
				}	
		}
		else if (module ==1)
		{
		for (Int_t row=1; row<=43;row++)
			for (Int_t crystal=1; crystal<=10;crystal++)
				for (Int_t copy=1; copy<=16;copy++)
				{
					iPhi=(11-crystal)+(copy-1)*10;
					iTheta=row+29;
					
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
					fIntTwoCoordMap[detId]=_tci;
				}
		}

		if (module ==3)
		{
			for (Int_t row=1; row<=36;row++)
				for (Int_t crystal=1; crystal<=36;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1)   {	iX = -row+1; iY =  crystal;   }
						if (copy==2)   {	iX = -row+1; iY = -crystal+1; }
						if (copy==3)   {	iX =  row;   iY = -crystal+1; }
						if (copy==4)   {  iX =  row;   iY =  crystal;   }
						
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
						
						_tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
						fIntTwoCoordMap[detId]=_tci;
					}
		}
		if (module ==4)
		{
			for (Int_t row=1; row<=16;row++)
				for (Int_t crystal=1; crystal<=16;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1) {  iX = -row+1; iY =  crystal;   }
						if (copy==2) {  iX = -row+1; iY = -crystal+1; }
						if (copy==3) {  iX =  row;   iY = -crystal+1; }
						if (copy==4) {  iX =  row;   iY =  crystal;   }
					
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
						
						_tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
						fIntTwoCoordMap[detId]=_tci;					
					}	
		}
		if (module ==5)
		{
			for (Int_t row=1; row<=14;row++)
				for (Int_t crystal=1; crystal<=7;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1) {  iX = -row+1; iY =  crystal;   }
						if (copy==2) {  iX = -row+1; iY = -crystal+1; }
						if (copy==3) {  iX =  row;   iY = -crystal+1; }
						if (copy==4) {  iX =  row;   iY =  crystal;   }
					
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
						
						_tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
						fIntTwoCoordMap[detId]=_tci;
					}	
		}
		if (module ==6)
		{
			for (Int_t row=1; row<=5;row++)
				for (Int_t crystal=1; crystal<=5;crystal++)
				{
					Int_t copy = 1;
					iX = row; 
					iY =  crystal; 
					
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					
					_tci=new PndEmcTwoCoordIndex(iX+550,iY+550,detId);
					fIntTwoCoordMap[detId]=_tci;
				}	
		}
	}
}

PndEmcMapperGeo12Dat345Root::PndEmcMapperGeo12Dat345Root()
{
   // *** for Barrel (like it is), _new_ FwEndCap & _new_ BwEndCap
   // 26.02.2009
  	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	for (Int_t module=1; module<=6; module++)
	{
		if (module ==2)
		{
		for (Int_t row=1; row<=29;row++)
			for (Int_t crystal=1; crystal<=10;crystal++)
				for (Int_t copy=1; copy<=16;copy++)
				{
					iPhi=(11-crystal)+(copy-1)*10;
					iTheta=-row+30;

					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
					_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
					fIntTwoCoordMap[detId]=_tci;
				}
		}
		else if (module ==1)
		{
		for (Int_t row=1; row<=43;row++)
			for (Int_t crystal=1; crystal<=10;crystal++)
				for (Int_t copy=1; copy<=16;copy++)
				{
					iPhi=(11-crystal)+(copy-1)*10;
					iTheta=row+29;

					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
					_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
					fIntTwoCoordMap[detId]=_tci;
				}
		}

		if (module ==3)
		{
			for (Int_t row=1; row<=36;row++)
				for (Int_t crystal=1; crystal<=36;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1)   {	iX = -row+1; iY =  crystal;   }
						if (copy==2)   {	iX = -row+1; iY = -crystal+1; }
						if (copy==3)   {	iX =  row;   iY = -crystal+1; }
						if (copy==4)   {  iX =  row;   iY =  crystal;   }

						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;

						_tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
						fIntTwoCoordMap[detId]=_tci;
					}
		}
		if (module ==4)
		{
			for (Int_t row=1; row<=16;row++)
				for (Int_t crystal=1; crystal<=16;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1) {  iX = -row+1; iY =  crystal;   }
						if (copy==2) {  iX = -row+1; iY = -crystal+1; }
						if (copy==3) {  iX =  row;   iY = -crystal+1; }
						if (copy==4) {  iX =  row;   iY =  crystal;   }

						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;

						_tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
						fIntTwoCoordMap[detId]=_tci;
					}
		}
		if (module ==5)
		{
			for (Int_t row=1; row<=28;row++)
				for (Int_t crystal=1; crystal<=54;crystal++)
			      {
						Int_t copy = 1;
						iX = row;
						iY =  crystal;
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
						_tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
						fIntTwoCoordMap[detId]=_tci;

			      }
		}
		if (module ==6)
		{
			for (Int_t row=1; row<=5;row++)
				for (Int_t crystal=1; crystal<=5;crystal++)
				{
					Int_t copy = 1;
					iX = row;
					iY =  crystal;

					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;

					_tci=new PndEmcTwoCoordIndex(iX+550,iY+550,detId);
					fIntTwoCoordMap[detId]=_tci;
				}
		}
	}
}

PndEmcMapperGeo1235Dat4Root::PndEmcMapperGeo1235Dat4Root()
{
   // *** for Barrel (like it is), _old_ FwEndCap & _new_ BwEndCap
   // 27.04.2009
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	
	// Crys 1-5000; copyNo 1-20; nRow 1-100, nMod 1-6
	// 72(iTheta)x160(iPhi) for barrel part
	// index iTheta from 1 (backward part) till 72 (forward)
	for (Int_t module=1; module<=6; module++)
	{
		if (module ==2)
		{
			for (Int_t row=1; row<=29;row++)
				for (Int_t crystal=1; crystal<=10;crystal++)
					for (Int_t copy=1; copy<=16;copy++)
					{
						iPhi=(11-crystal)+(copy-1)*10;
						iTheta=-row+30;
					
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
						_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
						fIntTwoCoordMap[detId]=_tci;
					}	
		}
		else if (module ==1)
		{
		for (Int_t row=1; row<=43;row++)
			for (Int_t crystal=1; crystal<=10;crystal++)
				for (Int_t copy=1; copy<=16;copy++)
				{
					iPhi=(11-crystal)+(copy-1)*10;
					iTheta=row+29;
					
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
					fIntTwoCoordMap[detId]=_tci;
				}
		}

		if (module ==3)
		{
			for (Int_t row=1; row<=40;row++)
				for (Int_t crystal=1; crystal<=40;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1)   {	iX = -row+1; iY =  crystal;   }
						if (copy==2)   {	iX = -row+1; iY = -crystal+1; }
						if (copy==3)   {	iX =  row;   iY = -crystal+1; }
						if (copy==4)   {     iX =  row;   iY =  crystal;   }			   
						
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
						
						_tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
						fIntTwoCoordMap[detId]=_tci;
					}
		}
		if (module ==4)
		{
			for (Int_t row=1; row<=16;row++)
				for (Int_t crystal=1; crystal<=16;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1) {  iX = -row+1; iY =  crystal;   }
						if (copy==2) {  iX = -row+1; iY = -crystal+1; }
						if (copy==3) {  iX =  row;   iY = -crystal+1; }
						if (copy==4) {  iX =  row;   iY =  crystal;   }
					
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
						
						_tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
						fIntTwoCoordMap[detId]=_tci;					
					}	
		}
		if (module ==5)
		{
			for (Int_t row=1; row<=14;row++)
				for (Int_t crystal=1; crystal<=7;crystal++)
					for (Int_t copy=1; copy<=4;copy++)
					{
						if (copy==1) {  iX = -row+1; iY =  crystal;   }
						if (copy==2) {  iX = -row+1; iY = -crystal+1; }
						if (copy==3) {  iX =  row;   iY = -crystal+1; }
						if (copy==4) {  iX =  row;   iY =  crystal;   }
					
						detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
						
						_tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
						fIntTwoCoordMap[detId]=_tci;
					}	
		}
		if (module ==6)
		{
			for (Int_t row=1; row<=5;row++)
				for (Int_t crystal=1; crystal<=5;crystal++)
				{
				  Int_t copy = 1;
				  iX = row; 
				  iY =  crystal; 
				  
				  detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
				  
				  _tci=new PndEmcTwoCoordIndex(iX+550,iY+550,detId);
				  fIntTwoCoordMap[detId]=_tci;
				}	
		}
	}
}

PndEmcMapperGeoProto60Root::PndEmcMapperGeoProto60Root(){
	//Proto60
	//18.01.10
	PndEmcTwoCoordIndex *_tci;
	Int_t detId = 0;
	Int_t module = 7;
	Int_t copy = 1;
	for(Int_t row=1; row<=6; row ++)
		for(Int_t crystal=1;crystal<=10;crystal++)
		{
			detId = module*100000000 + row*1000000 + copy*10000 + crystal;
			_tci = new PndEmcTwoCoordIndex(row+1000,crystal+1000,detId);
			fIntTwoCoordMap[detId]=_tci;
		}
}


PndEmcTwoCoordIndex* PndEmcMapper::GetTCI(Int_t DetectorId)
{
	PndEmcTwoCoordIndex* tci= fIntTwoCoordMap[DetectorId];
	return tci;
}



const std::map<Int_t,PndEmcTwoCoordIndex* >& PndEmcMapper::GetTciMap()
{
	return fIntTwoCoordMap;
}

Int_t PndEmcMapper::GetDetId(Int_t iTheta,Int_t iPhi)
{
  Int_t module,row,copy,crystal,detId;
  module = row = copy = crystal = detId = 0;
	if (iTheta<=29) 
	{
		module=2;
		row=30-iTheta;
		copy=(iPhi-1)/10+1;
		crystal=10 - (iPhi-1)%10;
	}
	else if ((iTheta>29)&&(iTheta<73))
	{
		module=1;
		row=iTheta-29;
		copy=(iPhi-1)/10+1;
		crystal=10 - (iPhi-1)%10;
	}
	if ((iTheta>200)&&(iTheta<300))
	  {
	    module=3; 
	    if ((iTheta<=250)&& (iPhi>250))
	      {
		copy = 1;  row = 1-(iTheta-250);  crystal = iPhi-250;
	      }
	    if ((iTheta<=250)&& (iPhi<=250))
	      {
		copy = 2;  row = 1-(iTheta-250);    crystal = 1-(iPhi-250);
	      }
	    if ((iTheta>250)&& (iPhi<=250))
	      {
		copy = 3;  row = (iTheta-250);    crystal = 1-(iPhi-250);
	      }
	    if ((iTheta>250)&& (iPhi>250))
	      {
		copy = 4;  row = iTheta-250;        crystal = iPhi-250;
	      }
	  }

	if ((iTheta>300)&&(iTheta<400))
	  {
	    module=4; 
	    if ((iTheta<=350)&& (iPhi>350))
	      {
		copy = 1;  row = 1-(iTheta-350);  crystal = iPhi-350;
	      }
	    if ((iTheta<=350)&& (iPhi<=350))
	      {
		copy = 2;  row = 1-(iTheta-350);    crystal = 1-(iPhi-350);
	      }
	    if ((iTheta>350)&& (iPhi<=350))
	      {
		copy = 3;  row = (iTheta-350);    crystal = 1-(iPhi-350);
	      }
	    if ((iTheta>350)&& (iPhi>350))
	      {
		copy = 4;  row = (iTheta-350);        crystal = iPhi-350;
	      }
	  }
	if ((iTheta>400)&&(iTheta<500))
	  {
	    module=5; 
	    if ((iTheta<=450)&& (iPhi>450))
	      {
		copy = 1;  row = 1-(iTheta-450);  crystal = iPhi-450;
	      }
	    if ((iTheta<=450)&& (iPhi<=450))
	      {
		copy = 2;  row = 1-(iTheta-450);    crystal = 1-(iPhi-450);
	      }
	    if ((iTheta>450)&& (iPhi<=450))
	      {
		copy = 3;  row = (iTheta-450);    crystal = 1-(iPhi-450);
	      }
	    if ((iTheta>450)&& (iPhi>450))
	      {
		copy = 4;  row = (iTheta-450);        crystal = iPhi-450;
	      }
	  }
	if ((iTheta>500)&&(iTheta<600))
	  {
	    module=6; 
	    copy = 1;  
	    row = iTheta-550;
	    crystal = iPhi-550;
	  }
	
	detId =  module*100000000 + row*1000000 + copy*10000 + crystal;

   return detId;
}

ClassImp(PndEmcMapper)
