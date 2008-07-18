/////////////////////////////////////////////////////////////
//
//  PndEmcContFact
//
//  Factory for the parameter containers in libEmc
//
//  Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndEmcContFact.h"
#include "CbmRuntimeDb.h"
#include "PndGeoEmcPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"					
#include "CbmParAsciiFileIo.h"
#include "TClass.h"										
#include <iostream>
#include <iomanip>

ClassImp(PndEmcContFact)

static PndEmcContFact gPndEmcContFact;

PndEmcContFact::PndEmcContFact() {
  // Constructor (called when the library is loaded)
  fName="PndEmcContFact";
  fTitle="Factory for parameter containers in libEmc";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void PndEmcContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Emc library.*/

	CbmContainer* p1= new CbmContainer("PndGeoEmcPar",
														"Emc Geometry Parameters",
														"TestDefaultContext");
	p1->addContext("TestNonDefaultContext");
	
	CbmContainer* p2= new CbmContainer("PndEmcDigiPar",
														"Emc Digitalization Parameters",
														"TestDefaultContext");
	p2->addContext("TestNonDefaultContext");
	
	CbmContainer* p3= new CbmContainer("PndEmcRecoPar",
														"Emc Reconstruction Parameters",
														"TestDefaultContext");
	p3->addContext("TestNonDefaultContext");

	containers->Add(p1);
	containers->Add(p2);
	containers->Add(p3);
}

CbmParSet* PndEmcContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  CbmParSet* p=NULL;
  if (strcmp(name,"PndGeoEmcPar")==0) {
    p=new PndGeoEmcPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  if (strcmp(name,"PndEmcDigiPar")==0) {
    p=new PndEmcDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  if (strcmp(name,"PndEmcRecoPar")==0) {
    p=new PndEmcRecoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  return p;
}

// void  PndEmcContFact::activateParIo(CbmParIo* io) {
//   // activates the input/output class for the parameters
//   // needed by the Emc
// 	
// /*  if (strcmp(io->IsA()->GetName(),"CbmParRootFileIo")==0) {
//     EmcParRootFileIo* p=new EmcParRootFileIo(((CbmParRootFileIo*)io)->getParRootFile());
//     io->setDetParIo(p);
//   }*/
//   
// 	if (strcmp(io->IsA()->GetName(),"CbmParAsciiFileIo")==0)
// 	{
// 		EmcParAsciiFileIo* p=new EmcParAsciiFileIo(((CbmParAsciiFileIo*)io)->getFile());
// 		io->setDetParIo(p);
// 	}
// }
