/////////////////////////////////////////////////////////////
//
//  PndMvdContFact
//
//  Factory for the parameter containers in libPndMvd
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndMvdContFact.h"
#include "CbmRuntimeDb.h"
#include "PndMvdGeoPar.h"
#include "PndMvdPixelDigiPar.h"
#include "PndMvdStripDigiPar.h"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndMvdContFact);

static PndMvdContFact gPndMvdContFact;

PndMvdContFact::PndMvdContFact() {
  // Constructor (called when the library is loaded)
  fName="PndMvdContFact";
  fTitle="Factory for parameter containers in libPndMvd";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void PndMvdContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the MVD library.*/

    CbmContainer* p= new CbmContainer("PndMvdGeoPar","PndMvd Geometry Parameters","TestDefaultContext");
    p->addContext("TestNonDefaultContext");
    containers->Add(p);

    CbmContainer* p2 = new CbmContainer("MVDPixelDigiPar", "PndMvd Pixel Digitization Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);
    
    CbmContainer* p3 = new CbmContainer("MVDStripDigiParRect", "PndMvd Strip Digitization Parameters (rectangular sensors)", "TestDefaultContext");
    p3->addContext("TestNonDefaultContext");
    containers->Add(p3);

    CbmContainer* p4 = new CbmContainer("MVDStripDigiParTrap", "PndMvd Strip Digitization Parameters (trapezoid sensors)", "TestDefaultContext");
    p4->addContext("TestNonDefaultContext");
    containers->Add(p4);
    
}

CbmParSet* PndMvdContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=NULL;
  if (strcmp(name,"PndMvdGeoPar")==0) {
    p=new PndMvdGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }

  if (strcmp(name,"MVDPixelDigiPar")==0) {
	p=new PndMvdPixelDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }

  if (strcmp(name,"MVDStripDigiParRect")==0) {
  p=new PndMvdStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  if (strcmp(name,"MVDStripDigiParTrap")==0) {
  p=new PndMvdStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  return p;
}
