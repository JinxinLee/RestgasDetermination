/////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	EMC Mapper class. Map crystal index (fDetectorId) to two coordinate index.
//
// This class uses the singleton pattern design				
// Author List:
// Dima Melnychuk
// 27/04/2007 Stefano Spataro (encoding for the Fwd Endcup)
//
/////////////////////////////////////////////////////////////// 

#include "PndEmcMapper.h"
#include "PndEmcTwoCoordIndex.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
		
#include "TROOT.h"
#include "TFile.h"
#include <iostream>
#include <fstream>		

using namespace std;

PndEmcMapper* PndEmcMapper::_instance = 0;
    
PndEmcMapper* PndEmcMapper::Instance (Int_t MapVersion, TString geoFile)
{
  TFile *geoF;
  if (geoFile!="") geoF = TFile::Open(geoFile,"READ");
	if (_instance == 0) {
		if (MapVersion==0){
			cout<<"Map version 0 does not exist"<<endl;
			return 0;
		}
		_instance = new PndEmcMapper(MapVersion);
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
  
PndEmcMapper::PndEmcMapper(Int_t MapVersion):fMapVersion(MapVersion)
{
	PndEmcTwoCoordIndex _tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	
	if (fMapVersion==1)
	{
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
						PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
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
						PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
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
						
						PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
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
						
						PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
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
						
						PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
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
				  
				  PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+550,iY+550,detId);
				  fIntTwoCoordMap[detId]=_tci;
				}	
			  }
		}
	}
   

   // *** All EMC: Barrel, BwEndCap, FS --> read from "emc_module1245.dat" file
   // *** and new version of FwEndCap   --> read from "emc_module3.root" file
   if (fMapVersion==2)
     {
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
			   PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
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
			   PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
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
			   
			   PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
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
			   
			   PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
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
			   
			   PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
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
			 
			 PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+550,iY+550,detId);
			 fIntTwoCoordMap[detId]=_tci;
		      }
	       }	    
	  }
     }
   
   // *** for testing of FwEndCap ONLY --> from "emc_module3.dat" file ***
   // !!! this will be removed in the nearest future !!!
   if (fMapVersion==3)
     {
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
		 
		 PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
		 fIntTwoCoordMap[detId]=_tci;
	      }	
     }	
   
   // *** for testing of FwEndCap ONLY --> from "emc_module3.root" file ***
   // !!! this will be removed in the nearest future !!!
   if (fMapVersion==4)
     {
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
		 PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
		 fIntTwoCoordMap[detId]=_tci;

	      }	
     }	
   
   // *** for testing of BwEndCap ONLY --> from "emc_module4_*.root" file ***
   // 26.02.2009
   if (fMapVersion==5)
     {
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
		 PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
		 fIntTwoCoordMap[detId]=_tci;
	      }	
     }	
   
   // *** for Barrel (like it is), _new_ FwEndCap & _new_ BwEndCap
   // 26.02.2009
   if (fMapVersion==6)
     { 
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
						PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
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
						PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
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
			   
			   PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
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
						
						PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
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
						
						PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
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
				  
				  PndEmcTwoCoordIndex* _tci=new PndEmcTwoCoordIndex(iX+550,iY+550,detId);
				  fIntTwoCoordMap[detId]=_tci;
				}	
			  }
		}
     }
 
   //fill neigbour list
    if ((fMapVersion ==1) || (fMapVersion ==3))
     {
	std::map <int,PndEmcTwoCoordIndex* >::const_iterator tciIter; 
	for(tciIter=fIntTwoCoordMap.begin();tciIter!=fIntTwoCoordMap.end();++tciIter)
	{
		PndEmcTwoCoordIndex* _tci=tciIter->second;
		detId=tciIter->first;
		iTheta=_tci->XCoord();
		iPhi=_tci->YCoord();
		
		for (Int_t i=iTheta-1;i<=iTheta+1;i++)
		  for (Int_t j=iPhi-1;j<=iPhi+1;j++)
		    {
		      if ((i!=  0)&&(i!= 73)&&(j!=  0)&&(j!=161)&& // Boundaries of module 1&2
			  (i!=210)&&(i!=291)&&(j!=210)&&(j!=291)&& // Boundaries of module 3
			  //(i!=214)&&(i!=287)&&(j!=214)&&(j!=287)&& // Boundaries of NEW module 3
			  (i!=332)&&(i!=369)&&(j!=332)&&(j!=369)&& // Boundaries of module 4
			  (i!=465)&&(i!=436)&&(j!=458)&&(j!=443)&& // Boundaries of module 5
			  (i!=556)&&(i!=545)&&(j!=556)&&(j!=545)&& // Boundaries of module 6
			  ((i!=iTheta)||(j!=iPhi)))
			{
			  detId_tmp=PndEmcMapper::GetDetId(i,j);
			  if (!IsValidIndex(detId_tmp)) continue;
			  PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			  _tci->AddToNeighbourList(tci_tmp);
			  fIntTwoCoordMap[detId]=_tci;
			}
		      else if ((i!=0)&&(i!=73)&&(j==0))
			{
			  detId_tmp=PndEmcMapper::GetDetId(i,160);
			  if (!IsValidIndex(detId_tmp)) continue;
			  PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			  _tci->AddToNeighbourList(tci_tmp);
			  fIntTwoCoordMap[detId]=_tci;
			}
		      else if ((i!=0)&&(i!=73)&&(j==161))
			{
			  detId_tmp=PndEmcMapper::GetDetId(i,1);
			  if (!IsValidIndex(detId_tmp)) continue;
			  PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			  _tci->AddToNeighbourList(tci_tmp);
			  fIntTwoCoordMap[detId]=_tci;
			}
		    }
	}
	
     }
    if (fMapVersion ==2 || fMapVersion ==4)
     {
	std::map <int,PndEmcTwoCoordIndex* >::const_iterator tciIter; 
	for(tciIter=fIntTwoCoordMap.begin();tciIter!=fIntTwoCoordMap.end();++tciIter)
	{
		PndEmcTwoCoordIndex* _tci=tciIter->second;
		detId=tciIter->first;
		iTheta=_tci->XCoord();
		iPhi=_tci->YCoord();
		
		for (Int_t i=iTheta-1;i<=iTheta+1;i++)
		  for (Int_t j=iPhi-1;j<=iPhi+1;j++)
		    {
		      if ((i!=  0)&&(i!= 73)&&(j!=  0)&&(j!=161)&& // Boundaries of module 1&2
			  //(i!=210)&&(i!=291)&&(j!=210)&&(j!=291)&& // Boundaries of module 3
			  (i!=214)&&(i!=287)&&(j!=214)&&(j!=287)&& // Boundaries of NEW module 3
			  (i!=332)&&(i!=369)&&(j!=332)&&(j!=369)&& // Boundaries of module 4
			  (i!=465)&&(i!=436)&&(j!=458)&&(j!=443)&& // Boundaries of module 5
			  (i!=556)&&(i!=545)&&(j!=556)&&(j!=545)&& // Boundaries of module 6
			  ((i!=iTheta)||(j!=iPhi)))
			{
			  detId_tmp=PndEmcMapper::GetDetId(i,j);
			  if (!IsValidIndex(detId_tmp)) continue;
			  PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			  _tci->AddToNeighbourList(tci_tmp);
			  fIntTwoCoordMap[detId]=_tci;
			}
		      else if ((i!=0)&&(i!=73)&&(j==0))
			{
			  detId_tmp=PndEmcMapper::GetDetId(i,160);
			  if (!IsValidIndex(detId_tmp)) continue;
			  PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			  _tci->AddToNeighbourList(tci_tmp);
			  fIntTwoCoordMap[detId]=_tci;
			}
		      else if ((i!=0)&&(i!=73)&&(j==161))
			{
			  detId_tmp=PndEmcMapper::GetDetId(i,1);
			  if (!IsValidIndex(detId_tmp)) continue;
			  PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			  _tci->AddToNeighbourList(tci_tmp);
			  fIntTwoCoordMap[detId]=_tci;
			}
		    }
	}
	
     }
     else if (fMapVersion ==4)
     {
	std::map <int,PndEmcTwoCoordIndex* >::const_iterator tciIter; 
	for(tciIter=fIntTwoCoordMap.begin();tciIter!=fIntTwoCoordMap.end();++tciIter)
	  {
	     PndEmcTwoCoordIndex* _tci=tciIter->second;
	     detId=tciIter->first;
	     iTheta=_tci->XCoord();
	     iPhi=_tci->YCoord();
	     
	     for (Int_t i=iTheta-1;i<=iTheta+1;i++)
	       for (Int_t j=iPhi-1;j<=iPhi+1;j++)
		 {
		    if ((i!=214)&&(i!=287)&&(j!=214)&&(j!=287)&& // Boundaries of the NEW module 3
			((i!=iTheta)||(j!=iPhi)))
		      {
			 detId_tmp=PndEmcMapper::GetDetId(i,j);
			 if (!IsValidIndex(detId_tmp)) continue;
			 PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			 _tci->AddToNeighbourList(tci_tmp);
			 fIntTwoCoordMap[detId]=_tci;	
			// cout << "detId_tmp in Mapper - version 4: " << detId_tmp<< "\t" << detId<<endl; // ok
		      }
		    
		 }
	  }
     }

    if (fMapVersion ==5 || fMapVersion ==6)  // NEW FwEndCap + NEW BwEndCap
     {
	std::map <int,PndEmcTwoCoordIndex* >::const_iterator tciIter; 
	for(tciIter=fIntTwoCoordMap.begin();tciIter!=fIntTwoCoordMap.end();++tciIter)
	{
		PndEmcTwoCoordIndex* _tci=tciIter->second;
		detId=tciIter->first;
		iTheta=_tci->XCoord();
		iPhi=_tci->YCoord();
		
		for (Int_t i=iTheta-1;i<=iTheta+1;i++)
		  for (Int_t j=iPhi-1;j<=iPhi+1;j++)
		    {
		      if ((i!=  0)&&(i!= 73)&&(j!=  0)&&(j!=161)&& // Boundaries of module 1&2
			  //(i!=210)&&(i!=291)&&(j!=210)&&(j!=291)&& // Boundaries of module 3
			  (i!=214)&&(i!=287)&&(j!=214)&&(j!=287)&& // Boundaries of NEW module 3
			  //(i!=332)&&(i!=369)&&(j!=332)&&(j!=369)&& // Boundaries of module 4
			  (i!=335)&&(i!=366)&&(j!=335)&&(j!=366)&& // Boundaries of NEW module 4
			  (i!=465)&&(i!=436)&&(j!=458)&&(j!=443)&& // Boundaries of module 5
			  (i!=556)&&(i!=545)&&(j!=556)&&(j!=545)&& // Boundaries of module 6
			  ((i!=iTheta)||(j!=iPhi)))
			{
			  detId_tmp=PndEmcMapper::GetDetId(i,j);
			  if (!IsValidIndex(detId_tmp)) continue;
			  PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			  _tci->AddToNeighbourList(tci_tmp);
			  fIntTwoCoordMap[detId]=_tci;
			}
		      else if ((i!=0)&&(i!=73)&&(j==0))
			{
			  detId_tmp=PndEmcMapper::GetDetId(i,160);
			  if (!IsValidIndex(detId_tmp)) continue;
			  PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			  _tci->AddToNeighbourList(tci_tmp);
			  fIntTwoCoordMap[detId]=_tci;
			}
		      else if ((i!=0)&&(i!=73)&&(j==161))
			{
			  detId_tmp=PndEmcMapper::GetDetId(i,1);
			  if (!IsValidIndex(detId_tmp)) continue;
			  PndEmcTwoCoordIndex* tci_tmp=fIntTwoCoordMap[detId_tmp];
			  _tci->AddToNeighbourList(tci_tmp);
			  fIntTwoCoordMap[detId]=_tci;
			}
		    }
	}
	
     }
}




PndEmcTwoCoordIndex* PndEmcMapper::GetTCI(Int_t DetectorId)
{
	PndEmcTwoCoordIndex* tci= fIntTwoCoordMap[DetectorId];
	return tci;
};


// This function check is the volume with given detectorId is really exist in geometry
bool PndEmcMapper::IsValidIndex(Int_t detectorId)
{
	// Geometry loading
	TGeoManager *geom;
	if (gGeoManager) {
		geom = gGeoManager;
	}
	else {
		geom = (TGeoManager*) gROOT->FindObject("FAIRGeom");
	}
	
	if (geom==0){	
		std::cout<<"FAIRGeom object is not found"<<std::endl;
		abort();
	}
	
	Short_t module =detectorId/100000000;
	Short_t row=(detectorId/1000000)%100;
	Short_t crystal=detectorId%10000;
   	//Short_t copy=(detectorId/10000)%100;
   
   Text_t volname[30];
   sprintf(volname,"emc0%dr%dc%d",module, row, crystal);
   
   TGeoVolume *vol = geom->FindVolumeFast(volname);
   if (!vol){
   //std::cout<<"Volume name "<< volname << "\t" << detectorId<<" is not found "<<std::endl;
      return false;
   }  
   //     }
   return true;
};

const std::map<Int_t,PndEmcTwoCoordIndex* >& PndEmcMapper::GetTciMap()
{
   std::map <int,PndEmcTwoCoordIndex* >::const_iterator tciIter;
   for(tciIter=fIntTwoCoordMap.begin();tciIter!=fIntTwoCoordMap.end();++tciIter)
   {
   	//cout << "@@@  tciIter->first " << tciIter->first << endl; // 1st ID not correct
   }
   
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
		crystal=(iPhi-1)%10+1;
	}
	else if ((iTheta>29)&&(iTheta<73))
	{
		module=1;
		row=iTheta-29;
		copy=(iPhi-1)/10+1;
		crystal=(iPhi-1)%10+1;
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
