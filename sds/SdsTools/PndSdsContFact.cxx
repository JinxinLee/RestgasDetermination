/////////////////////////////////////////////////////////////
//
//  PndSdsContFact
//
//  Factory for the parameter containers in libPndSds
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndSdsContFact.h"
#include "FairRuntimeDb.h"
#include "PndSdsGeoPar.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsStripDigiPar.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "TList.h"
#include "TObjString.h"
#include <iostream>
#include <iomanip>

ClassImp(PndSdsContFact);

static PndSdsContFact gPndSdsContFact;

PndSdsContFact::PndSdsContFact() {
  // Constructor (called when the library is loaded)
  fName="PndSdsContFact";
  fTitle="Factory for parameter containers in libPndSds";
  fDigiParNames = new TList();
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}
PndSdsContFact::~PndSdsContFact() {
if(0!=fDigiParNames) 
  {
    fDigiParNames->Delete(); 
    delete fDigiParNames;
  }
}

void PndSdsContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the MVD library.*/

    FairContainer* p= new FairContainer("PndSdsGeoPar","PndSds Geometry Parameters","TestDefaultContext");
    p->addContext("TestNonDefaultContext");
    fDigiParNames->Add(new TObjString(p->GetName()));
    containers->Add(p);

    FairContainer* p2 = new FairContainer("MVDPixelDigiPar", "PndSds Pixel Digitization Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    fDigiParNames->Add(new TObjString(p2->GetName()));
    containers->Add(p2);
    
    FairContainer* p3 = new FairContainer("MVDStripDigiParRect", "PndSds Strip Digitization Parameters (rectangular sensors)", "TestDefaultContext");
    p3->addContext("TestNonDefaultContext");
    fDigiParNames->Add(new TObjString(p3->GetName()));
    containers->Add(p3);

    FairContainer* p4 = new FairContainer("MVDStripDigiParTrap", "PndSds Strip Digitization Parameters (trapezoid sensors)", "TestDefaultContext");
    p4->addContext("TestNonDefaultContext");
    fDigiParNames->Add(new TObjString(p4->GetName()));
    containers->Add(p4);
    
}

FairParSet* PndSdsContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"PndSdsGeoPar")==0) {
    p=new PndSdsGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }

  if (strcmp(name,"MVDPixelDigiPar")==0) {
	p=new PndSdsPixelDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }

  if (strcmp(name,"MVDStripDigiParRect")==0) {
  p=new PndSdsStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  if (strcmp(name,"MVDStripDigiParTrap")==0) {
  p=new PndSdsStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  return p;
}
