#include "PndMultiFieldPar.h"
#include "PndMultiFieldPar.h"
#include "PndConstField.h"
#include "PndConstPar.h"
#include "PndDipoleMap.h"
#include "PndSolenoidMap.h"
#include "PndDipolePar.h"
#include "PndSolenoidPar.h"
#include "PndTransPar.h"
#include "PndTransMap.h"
#include "CbmParamList.h"
#include "PndMapPar.h"
#include "CbmRuntimeDb.h"
#include "CbmRun.h"

// ------   Constructor   --------------------------------------------------
PndMultiFieldPar::PndMultiFieldPar(const char* name, const char* title, const char* context) 
  : PndMapPar(name, title, context) 
{

   fParArray=new TObjArray();
}

PndMultiFieldPar::PndMultiFieldPar() 
{
}

PndMultiFieldPar::~PndMultiFieldPar() { }

void PndMultiFieldPar::putParams(CbmParamList* list)
{
   if ( ! list ) return;
   list->addBinary("List of Field par", fParArray);
   list->add("Field Type", fType);
}



Bool_t PndMultiFieldPar::getParams(CbmParamList* l)
{
  if (!l->fillBinary("list of fields Par",fParArray))return kFALSE;
  if ( ! l->fill("Field Type", &fType) ) return kFALSE;

  return kTRUE;
} 
void PndMultiFieldPar:: SetParameters(CbmField* field)
{	 
   fType=5;
   CbmRuntimeDb *rtdb=CbmRuntimeDb::instance();
   CbmRun *fRun= CbmRun::Instance();
   PndMultiField *fmField = (PndMultiField *)field;
   TObjArray *fArray=fmField->GetFieldList();
   TIterator *Iter=fArray->MakeIterator();
   Iter->Reset();
   CbmField* fField = NULL;
   Int_t Type=-1;
   while( (fField = (CbmField*)Iter->Next() ) ) {
      Type=fField->GetType();
      if(Type==0){
	 PndConstField *fc= (PndConstField *)fField;
	 PndConstPar *cp = (PndConstPar*) rtdb->getContainer("PndConstPar");
	 cp->SetParameters(fc);
         cp->setInputVersion(fRun->GetRunId(),1);
	 fParArray->AddLast(cp);
      }
      if(Type==1){

      } 
      if(Type==2){
         PndSolenoidMap *fs= (PndSolenoidMap *)fField;
         PndSolenoidPar* cs = (PndSolenoidPar*) rtdb->getContainer("PndSolenoidPar");
         cs->SetParameters(fs);
	 cs->setInputVersion(fRun->GetRunId(),1);
         fParArray->AddLast(cs);
      }
      if(Type==3){
	 PndDipoleMap *fd= (PndDipoleMap *)fField;
         PndDipolePar* cd = (PndDipolePar*) rtdb->getContainer("PndDipolePar");
         cd->SetParameters(fd);
	 cd->setInputVersion(fRun->GetRunId(),1);
	 fParArray->AddLast(cd);
      }
      if(Type==4){
	 PndTransMap *ft= (PndTransMap *)fField;
         PndTransPar* ct = (PndTransPar*) rtdb->getContainer("PndTransPar");
         ct->SetParameters(ft);
	 ct->setInputVersion(fRun->GetRunId(),1);
	 fParArray->AddLast(ct);
      }
   }
    delete  Iter;
}                      


ClassImp(PndMultiFieldPar)

