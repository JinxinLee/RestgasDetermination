//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemDigiPar source file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemDigiPar
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 12.02.2009
 *@version 1.0
 **
 ** PANDA task class for storing digitization parameters for GEM
 ** Task level RECO
 **/

#include <iostream>
#include <map>

#include "PndDrcDigiPar.h"
#include "PndDetectorList.h"

using std::cout;
using std::endl;
using std::flush;
using std::map;
using std::pair;

PndDrcDigiPar::PndDrcDigiPar(const char* name, 
			     const char* title, 
			     const char* context) : FairParGenericSet(name,title,context)
{
  clear();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDrcDigiPar::~PndDrcDigiPar(void) {}
// -------------------------------------------------------------------------


void PndDrcDigiPar::putParams(FairParamList* list)
{
  if(!list) return;
  //list->add("parameters", fGemParameters);
  
}

Bool_t PndDrcDigiPar::getParams(FairParamList* list)
{
  if (!list) {std::cout<<"list not found"<<std::endl; return kFALSE;}
  //if(!list->fill("parameters",&fGemParameters)) 
  //{std::cout<<"par: fGemParameters not found"<<std::endl; return kFALSE;}
  return kTRUE;
}


ClassImp(PndDrcDigiPar);

void PndDrcDigiPar::Print()
{
  Int_t arrayIndex = 0;
  cout << "-------------------------------------------------" << endl;
  cout<<"Drc Digitization Parameters:"<<endl;
  cout << "-------------------------------------------------" << endl;
}
