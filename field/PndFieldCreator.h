// -------------------------------------------------------------------------
// -----                    PndFieldCreator header file                  -----
// -----                Created 15/01/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------


#ifndef PNDFIELDCREATOR_H
#define PNDFIELDCREATOR_H

#include "CbmField.h"
#include "CbmFieldFactory.h"
#include "PndFieldPar.h"

class  PndSolenoidPar;
class  PndDipolePar  ;
class  PndTransPar   ;
class  PndConstPar   ;
class  PndMultiFieldPar ;

class PndFieldCreator : public CbmFieldFactory 
{
public:
    PndFieldCreator();
    virtual ~PndFieldCreator();
    virtual CbmField* createCbmField();
    virtual void SetParm();
    ClassDef(PndFieldCreator,1);
protected:
 PndFieldPar* fFieldPar;
 PndSolenoidPar    *fSPar;
 PndDipolePar      *fDPar;
 PndTransPar       *fTPar;
 PndConstPar       *fCPar;
 PndMultiFieldPar  *fMPar;

};
#endif //PNDFIELDCREATOR_H
