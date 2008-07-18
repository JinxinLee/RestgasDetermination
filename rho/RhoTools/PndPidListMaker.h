#ifndef PNDPIDLISTMAKER_H
#define PNDPIDLISTMAKER_H 1

#include <string>
#include <vector>

#include "RhoBase/TCandList.h"

class TPidChargedSelector;
class TPidNeutralSelector;
class TPidPlusSelector;
class TPidMinusSelector;

class TPidSimpleElectronSelector;
class TPidSimpleMuonSelector;
class TPidSimplePionSelector;
class TPidSimpleKaonSelector;
class TPidSimpleProtonSelector;


class PndPidListMaker
{
public:
	PndPidListMaker();	
	PndPidListMaker(TCandList &l);
	~PndPidListMaker();
	
	void SetBaseList(TCandList &l);
	bool FillList(TCandList &l, std::string listkey="All");
	
private:
	void Init();
	
	// Private Member Variables
	
	TCandList		fBaseList;
		
	// **** the basic selectors
	//
	TPidPlusSelector           *plusSel;
	TPidMinusSelector          *minusSel;
	
	TPidSimpleElectronSelector *eSel;
	TPidSimpleMuonSelector     *muSel;
	TPidSimplePionSelector     *piSel;
	TPidSimpleKaonSelector     *kSel;
	TPidSimpleProtonSelector   *pSel;

	std::vector<std::string>   fListNames;	
	
  	ClassDef(PndPidListMaker,1);
};


#endif

