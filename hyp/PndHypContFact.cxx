/////////////////////////////////////////////////////////////
//
//  PndHypContFact
//
//  Factory for the parameter containers in libHyp
//
//  
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndHypContFact.h"
#include "CbmRuntimeDb.h"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"
#include "PndGeoHypPar.h"
#include "PndHypDigiPar.h"
#include "PndHypStripDigiPar.h"
#include <iostream>
#include <iomanip>

ClassImp(PndHypContFact)

static PndHypContFact gPndHypContFact;

PndHypContFact::PndHypContFact() {
  // Constructor (called when the library is loaded)
  fName="PndHypContFact";
  fTitle="Factory for parameter containers in libHyp";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void PndHypContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Hyp library.*/

    CbmContainer* p= new CbmContainer("PndGeoHypPar",
                                          "Hyp Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);

    CbmContainer* p2 = new CbmContainer("PndHypDigiPar", 
					"Hyp Pixel Digitization Parameters", 
					"TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);
    
    CbmContainer* p3 = new CbmContainer("PndHypStripDigiPar", 
					"Hyp Strip Digitization Parameters", 
					"TestDefaultContext");
    p3->addContext("TestNonDefaultContext");
    containers->Add(p3);
    

}

CbmParSet* PndHypContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=NULL;
  if (strcmp(name,"PndGeoHypPar")==0) {
    p=new PndGeoHypPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }

  if (strcmp(name,"PndHypDigiPar")==0) {
	p=new PndHypDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }

  if (strcmp(name,"PndHypStripDigiPar")==0) {
	p=new PndHypStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
	}

  return p;
}
