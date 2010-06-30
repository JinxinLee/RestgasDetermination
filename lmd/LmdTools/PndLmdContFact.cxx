/*
 * PndLmdContFact.cxx *
 *  Created on: May 29, 2009
 *      Author: huagen
 */

#include "PndLmdContFact.h"

#include "PndSdsPixelDigiPar.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsTotDigiPar.h"
#include "PndSdsGeoPar.h"

#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include <iostream>
#include <iomanip>
#include "TList.h"

static PndLmdContFact gPndLmdContFact;

PndLmdContFact :: PndLmdContFact(){
	//called when the lib is loaded
	  fName="PndLmdContFact";
	  fTitle="Factory for parameter containers in libPndLmd";
	  fDigiParNames = new TList();
	  setAllContainers();
	  FairRuntimeDb::instance()->addContFactory(this);
}

PndLmdContFact::~PndLmdContFact()
{
	  if(0!=fDigiParNames)
	  {
	    fDigiParNames->Delete();
	    delete fDigiParNames;
	  }
}

void PndLmdContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the LMD library.*/

    FairContainer* p= new FairContainer("PndSdsGeoPar","PndLmd Geometry Parameters","TestDefaultContext");
    fDigiParNames->Add(new TObjString(p->GetName()));
    p->addContext("TestNonDefaultContext");
    containers->Add(p);


    FairContainer* p1= new FairContainer("LmdStripDigiParTrap","PndLmdStrip Digitization Paras","TestDefaultContext");
    fDigiParNames->Add(new TObjString(p1->GetName()));
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

}

FairParSet* PndLmdContFact :: createContainer(FairContainer* c){

	const char* name=c->GetName();
	  FairParSet* p=NULL;

	  if (strcmp(name,"PndSdsGeoPar")==0) {
	    p=new PndSdsGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
	  }
/*
	  if (strcmp(name,"PndLmdStripDigiParTrap")==0) {
	  p=new PndLmdDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
	  }
*/
	  if (strcmp(name,"LmdStripDigiParTrap")==0) {
	  p=new PndSdsStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
	  }

	  return p;
}

ClassImp(PndLmdContFact)

