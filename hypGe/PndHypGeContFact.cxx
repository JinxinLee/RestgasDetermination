/////////////////////////////////////////////////////////////
//
//  PndHypGeContFact
//
//  Factory for the parameter containers in libHypGe
//

//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndHypGeContFact.h"
#include "FairRuntimeDb.h"
//#include "PndGeoHypGePar.h"
#include <iostream>
#include <iomanip>

ClassImp(PndHypGeContFact)

static PndHypGeContFact gPndHypGeContFact;

PndHypGeContFact::PndHypGeContFact() {
  // Constructor (called when the library is loaded)
  fName="PndHypGeContFact";
  fTitle="Factory for parameter containers in libHypGe";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndHypGeContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the HypGe library.*/

   // FairContainer* p= new FairContainer("PndGeoHypGePar",
     //                                     "HypGe Geometry Parameters",
     //                                     "TestDefaultContext");
    //p->addContext("TestNonDefaultContext");

  //  containers->Add(p);
}

FairParSet* PndHypGeContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  //if (strcmp(name,"PndGeoHypGePar")==0) {
  //  p=new PndGeoHypGePar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  //}
  return p;
}
