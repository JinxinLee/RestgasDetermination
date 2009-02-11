// -------------------------------------------------------------------------
// -----                      PndMultiField source file                -----
// -----                 Created 29/01/07  by M. Al/Turany             -----
// -------------------------------------------------------------------------


#include <iomanip>
#include <iostream>
#include <fstream>

#include "PndMultiField.h"
#include "PndRegion.h"
#include "PndConstField.h"
#include "PndFieldMap.h"
#include "TObjArray.h"
#include "PndMapPar.h"
#include "PndMultiFieldPar.h"


using namespace std;

// -------------   Default constructor  ----------------------------------
PndMultiField::PndMultiField() {

    fMaps= new TObjArray(10);
    fNoOfMaps=0;
    fType = 5;
}
// ------------------------------------------------------------------------

// ------------   Constructor from PndFieldPar   --------------------------
PndMultiField::PndMultiField(PndMultiFieldPar* fieldPar) {
   fType = 5;
   fMaps= new TObjArray(10);
   fNoOfMaps=0;
   TObjArray *fArray= fieldPar->GetParArray();
   if(fArray->IsEmpty()) fType=-1;
//    TIterator *Iter=fArray->MakeIterator();
//    Iter->Reset();
//    PndMapPar *fpar = NULL;
//    Int_t Type=-1;
//    while( (fpar = (PndMapPar*)Iter->Next() ) ) {
//       Type=fpar->GetType();
//       if(Type==0){
//   	 cout << "Const field" << endl;
//       }
//       if(Type==2){
//   	 cout << "field 2" << endl;
//       }
//       if(Type==3){
//   	 cout << "field 3" << endl;
//       }
//       if(Type==4){
//   	 cout << "field 4" << endl;
//       }
//    }

}

// ------------   Destructor   --------------------------------------------
PndMultiField::~PndMultiField() {
}

// -----------   Adding fields   ------------------------------------------
void PndMultiField::AddField(FairField *field){
	
   if(field){
      fMaps->AddLast(field);	
      fNoOfMaps++;
   }

}

// -----------   Intialisation   ------------------------------------------
void PndMultiField::Init() {
   PndConstField *field=0;
   PndFieldMap *fieldMap=0;
   for (Int_t n=0; n<=fNoOfMaps; n++){
      fieldMap = dynamic_cast<PndFieldMap *>(fMaps->At(n));
      field = dynamic_cast<PndConstField *>(fMaps->At(n));
      if(fieldMap){
         fieldMap->Init();
	 fFieldMaps.insert( pair<PndRegion*, FairField*>(new PndRegion(fieldMap->GetZmin(),fieldMap->GetZmax()) , fieldMap ));
      }else if(field){
         field->Init();
	 fFieldMaps.insert( pair<PndRegion*, FairField*>(new PndRegion(field->GetZmin(),field->GetZmax() ), field ));
      }
   }

}

// ---------   Screen output   --------------------------------------------
void PndMultiField::Print() {  
 for (Int_t n=0; n<=fNoOfMaps; n++){
      FairField *fieldMap = dynamic_cast<FairField *>(fMaps->At(n));
      if(fieldMap) fieldMap->Print();
  }
}

// -------------------------------------------------------------------------
void PndMultiField::GetFieldValue(const Double_t point[3], Double_t* bField) 
{
  
   PndRegion *fReg=0;
   FairField *fField=0;
   for (fMapIter=fFieldMaps.begin(); fMapIter!= fFieldMaps.end();fMapIter++ ){
      fReg=fMapIter->first;
      if(fReg->IsInside(point[2])){
         fField=fMapIter->second;
	 break;
      }
   }
  if(fField){
   /* bField[0] = fField->GetBx(point[0], point[1], point[2]);
    bField[1] = fField->GetBy(point[0], point[1], point[2]);
    bField[2] = fField->GetBz(point[0], point[1], point[2]);
  */
  fField->GetBxyz(point, bField);

  //  cout <<"PndMultiField::GetFieldValue" << bField[0] <<" " << bField[1]<< " " << bField[2] << endl;
  }else{
    bField[0] = 0;
    bField[1] = 0;
    bField[2] = 0;

  }
}


ClassImp(PndMultiField)
