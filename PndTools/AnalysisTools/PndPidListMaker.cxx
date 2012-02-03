#include "PndPidListMaker.h"

#include <string>

#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/VAbsPidSelector.h"
#include "RhoSelector/TPidSelector.h"

PndPidListMaker::PndPidListMaker()
{
	Init();
}

PndPidListMaker::PndPidListMaker(TCandList &l)
{
	SetBaseList(l);
	Init();
}

PndPidListMaker::~PndPidListMaker()
{
	fBaseList.Cleanup();
}

void PndPidListMaker::Init()
{
	// init the selectors;
	plusSel    = new TPidPlusSelector;
	minusSel   = new TPidMinusSelector;
	
	// **** pid selectors for generic list creation
	//
	eSel   = new TPidSimpleElectronSelector();
	muSel  = new TPidSimpleMuonSelector();
	piSel  = new TPidSimplePionSelector();
	kSel   = new TPidSimpleKaonSelector();
	pSel   = new TPidSimpleProtonSelector();
	
	// initialize Known Names
	std::vector<std::string> pNames;
	std::vector<std::string> critNames;
	
	pNames.push_back("Electron");
	pNames.push_back("Muon");
	pNames.push_back("Pion");
	pNames.push_back("Kaon");
	pNames.push_back("Proton");
	
	critNames.push_back("VeryLoose");
	critNames.push_back("Loose");
	critNames.push_back("Tight");
	critNames.push_back("VeryTight");
	critNames.push_back("Best");
	critNames.push_back("All");
	
	fListNames.clear();
	
	for (unsigned int i=0;i<pNames.size();i++)
	{
		for (unsigned int j=0;j<critNames.size();j++)
		{
			fListNames.push_back(pNames[i]+critNames[j]);
			fListNames.push_back(pNames[i]+critNames[j]+"Plus");
			fListNames.push_back(pNames[i]+critNames[j]+"Minus");
		}
	}
}

void PndPidListMaker::SetBaseList(TCandList &l)
{
	fBaseList=l;
}

bool PndPidListMaker::FillList(TCandList &l, std::string listkey)
{
	//TCandidate *tc;
	l.Cleanup();
	
	if (listkey=="Charged") 
	{
		l=fBaseList;
		return true;
	}
	
	if (listkey=="Plus")
	{
		l.Select(fBaseList,plusSel);
		return true;
	}
	
	if (listkey=="Minus")
	{
		l.Select(fBaseList,minusSel);
		return true;
	}
	
	bool listExists=false;
	int i=0, n=fListNames.size();
	
	while (i<n && !listExists) listExists=(listkey==fListNames[i++]);
	
	if (!listExists) return false;
	
	--i;
	  
	int ptype=i/12;           	// particle type: 0=electron, ..., 4=proton
	int ctype=(i%12)/5;			// criterion: 0=veryLoose,..., 3=veryTight, 4=Best, 5=all	
	int select_chrg=(i%5);		// select charge: 0=all, 1=plus, 2=minus
  
	VAbsPidSelector *pidSel=NULL;
	
	if (select_chrg==1) 
		l.Select(fBaseList,plusSel);
	else if (select_chrg==2)
		l.Select(fBaseList,minusSel);
	
	switch (ptype)
	{
    case 0: pidSel = eSel;  break;
    case 1: pidSel = muSel; break;
    case 2: pidSel = piSel; break;
    case 3: pidSel = kSel;  break;
    case 4: pidSel = pSel;  break;
	}
  if (NULL==pidSel) return false;
	
	switch (ctype)
	{
    case 0: pidSel->SetCriterion(veryLoose); break;
    case 1: pidSel->SetCriterion(loose);     break;
    case 2: pidSel->SetCriterion(tight);     break;
    case 3: pidSel->SetCriterion(veryTight); break;
    case 4: pidSel->SetCriterion(best);      break;
    case 5: pidSel->SetCriterion(all);       break;
	}
	
	if (select_chrg>0)
		l.Select(pidSel);
	else
		l.Select(fBaseList,pidSel);
	
	return true;
}
