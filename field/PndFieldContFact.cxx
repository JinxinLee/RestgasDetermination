#include "PndFieldContFact.h"
#include "PndFieldPar.h"
#include "PndSolenoidPar.h"
#include "PndDipolePar.h"
#include "PndTransPar.h"
#include "PndConstPar.h"
#include "CbmParSet.h"
#include "CbmRuntimeDb.h"
#include "PndMultiFieldPar.h"
#include <iostream>

using namespace std;
static PndFieldContFact gPndFieldContFact;


// -----   Constructor   ---------------------------------------------------
PndFieldContFact::PndFieldContFact() {
  fName = "PndFieldContFact";
  fTitle = "Factory for field parameter containers";
  SetAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndFieldContFact::~PndFieldContFact() { }
// -------------------------------------------------------------------------



// -----   Create containers   ---------------------------------------------
CbmParSet* PndFieldContFact::createContainer(CbmContainer* container) {


  const char* name = container->GetName();
  cout << "create PndFieldPar container " << name << endl;
  CbmParSet* set = NULL;
  if ( strcmp(name, "PndFieldPar") == 0 ) {
    set = new PndFieldPar( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndSolenoidPar") == 0 ) {
    set = new PndSolenoidPar( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndDipolePar") == 0 ) {
    set = new PndDipolePar( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndTransPar") == 0 ) {
    set = new PndTransPar( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndConstPar") == 0 ) {
    set = new PndConstPar( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  }else if ( strcmp(name, "PndMultiFieldPar") == 0 ) {
    set = new PndMultiFieldPar( container->getConcatName().Data(),
 			   container->GetTitle(),	
			   container->getContext() );
  }  
return set;
}
// -------------------------------------------------------------------------



// -----   Set all containers (private)   ----------------------------------
void PndFieldContFact::SetAllContainers() {
  CbmContainer* con1 = new CbmContainer("PndFieldPar",
					     "Field parameter container",
					     "Default field");
  containers->Add(con1);
  CbmContainer* con2 = new CbmContainer("PndSolenoidPar",
					     "Solenoid Field parameter container",
					     "Default field");
  containers->Add(con2);
  CbmContainer* con3 = new CbmContainer("PndDipolePar",
					     "Dipole Field parameter container",
					     "Default field");
  containers->Add(con3);
  CbmContainer* con4 = new CbmContainer("PndTransPar",
					     "Trans. Field parameter container",
					     "Default field");
  containers->Add(con4);
  CbmContainer* con5 = new CbmContainer("PndConstPar",
					     "Const Field parameter container",
					     "Default field");
  containers->Add(con5);
  CbmContainer* con6 = new CbmContainer("PndMultiFieldPar",
					     "Multiple Field parameter container",
					     "Default field");
  containers->Add(con6);

}
// -------------------------------------------------------------------------


ClassImp(PndFieldContFact)
