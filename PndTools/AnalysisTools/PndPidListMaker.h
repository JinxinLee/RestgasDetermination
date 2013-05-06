#ifndef PNDPIDLISTMAKER_H
#define PNDPIDLISTMAKER_H 1

#include <string>
#include <vector>

#include "RhoBase/RhoCandList.h"
#include "RhoSelector/RhoPlusParticleSelector.h"
#include "RhoSelector/RhoMinusParticleSelector.h"
#include "RhoSelector/RhoSimpleElectronSelector.h"
#include "RhoSelector/RhoSimpleKaonSelector.h"
#include "RhoSelector/RhoSimpleMuonSelector.h"
#include "RhoSelector/RhoSimplePionSelector.h"
#include "RhoSelector/RhoSimpleProtonSelector.h"


class PndPidListMaker
{
  public:
    PndPidListMaker();
    PndPidListMaker(RhoCandList& l);
    virtual ~PndPidListMaker();

    void SetBaseList(RhoCandList& l);
    bool FillList(RhoCandList& l, std::string listkey="All");

  private:
    void Init();

    // Private Member Variables

    RhoCandList   fBaseList;

    // **** the basic selectors
    //
    RhoPlusParticleSelector*           plusSel;
    RhoMinusParticleSelector*          minusSel;

    RhoSimpleElectronSelector* eSel;
    RhoSimpleMuonSelector*     muSel;
    RhoSimplePionSelector*     piSel;
    RhoSimpleKaonSelector*     kSel;
    RhoSimpleProtonSelector*   pSel;

    std::vector<std::string>   fListNames;

    ClassDef(PndPidListMaker,1);
};


#endif

