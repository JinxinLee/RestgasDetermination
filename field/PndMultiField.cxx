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
void PndMultiField::AddField(CbmField *field){
	
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
	 fFieldMaps.insert( pair<PndRegion*, CbmField*>(new PndRegion(fieldMap->GetZmin(),fieldMap->GetZmax()) , fieldMap ));
      }else if(field){
         field->Init();
	 fFieldMaps.insert( pair<PndRegion*, CbmField*>(new PndRegion(field->GetZmin(),field->GetZmax() ), field ));
      }
   }

}

// -----------   Get x component of the field   ---------------------------
Double_t PndMultiField::GetBx(Double_t x, Double_t y, Double_t z) {
   PndRegion *fReg=0;
   CbmField *fField=0;
   for (fMapIter=fFieldMaps.begin(); fMapIter!= fFieldMaps.end();fMapIter++ ){
      fReg=fMapIter->first;
      if(fReg->IsInside(z)){
         fField=fMapIter->second;
	 break;
      }
   }
   if(fField)  return  fField->GetBx( x, y,  z);	
   else return 0;
}


// -----------   Get y component of the field   ---------------------------
Double_t PndMultiField::GetBy(Double_t x, Double_t y, Double_t z) {

   PndRegion *fReg=0;
   CbmField *fField=0;
   for (fMapIter=fFieldMaps.begin(); fMapIter!= fFieldMaps.end();fMapIter++ ){
      fReg=fMapIter->first;
      if(fReg->IsInside(z)){
         fField=fMapIter->second;
         break;
      }
   }
   if(fField)  return fField->GetBy( x, y,  z);
   else return 0;
}

// -----------   Get z component of the field   ---------------------------
Double_t PndMultiField::GetBz(Double_t x, Double_t y, Double_t z) {

   PndRegion *fReg=0;
   CbmField *fField=0;
   for (fMapIter=fFieldMaps.begin(); fMapIter!= fFieldMaps.end();fMapIter++ ){
      fReg=fMapIter->first;
      if(fReg->IsInside(z)){
         fField=fMapIter->second;
	 break;
      }
   }
   if(fField) return fField->GetBz( x, y,  z);
   else return 0;
}

// ---------   Screen output   --------------------------------------------
void PndMultiField::Print() {  
 for (Int_t n=0; n<=fNoOfMaps; n++){
      CbmField *fieldMap = dynamic_cast<CbmField *>(fMaps->At(n));
      if(fieldMap) fieldMap->Print();
  }
}



ClassImp(PndMultiField)
