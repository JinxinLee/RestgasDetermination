#include "PndFieldContFact.h"
#include "PndFieldPar.h"
#include "PndSolenoidPar.h"
#include "PndSolenoid1Par.h"
#include "PndSolenoid2Par.h"
#include "PndSolenoid3Par.h"
#include "PndSolenoid4Par.h"
#include "PndDipolePar.h"
#include "PndDipole1Par.h"
#include "PndDipole2Par.h"
#include "PndDipole3Par.h"
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
  } else if ( strcmp(name, "PndSolenoid1Par") == 0 ) {
    set = new PndSolenoid1Par( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndSolenoid2Par") == 0 ) {
    set = new PndSolenoid2Par( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndSolenoid3Par") == 0 ) {
    set = new PndSolenoid3Par( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndSolenoid4Par") == 0 ) {
    set = new PndSolenoid4Par( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );			   			   			   			   
  } else if ( strcmp(name, "PndDipolePar") == 0 ) {
    set = new PndDipolePar( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndDipole1Par") == 0 ) {
    set = new PndDipole1Par( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndDipole2Par") == 0 ) {
    set = new PndDipole2Par( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  } else if ( strcmp(name, "PndDipoleP3ar") == 0 ) {
    set = new PndDipole3Par( container->getConcatName().Data(),
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
 
  CbmContainer* con21 = new CbmContainer("PndSolenoid1Par",
					     "Solenoid 1st region parameter container",
					     "Default field");
  containers->Add(con21);
  CbmContainer* con22 = new CbmContainer("PndSolenoid2Par",
					     "Solenoid 2nd region parameter container",
					     "Default field");
  containers->Add(con22);					  
  
  CbmContainer* con23 = new CbmContainer("PndSolenoid3Par",
					     "Solenoid 3rd region parameter container",
					     "Default field");
  containers->Add(con23);
  
  CbmContainer* con24 = new CbmContainer("PndSolenoid4Par",
					     "Solenoid 4th region parameter container",
					     "Default field");					     					     					     					     
  containers->Add(con24);
  
  CbmContainer* con3 = new CbmContainer("PndDipolePar",
					     "Dipole Field parameter container",
					     "Default field");
  containers->Add(con3);
  
  
  CbmContainer* con31 = new CbmContainer("PndDipole1Par",
					     "Dipole Field parameter container",
					     "Default field");
  containers->Add(con31);
  
  CbmContainer* con32 = new CbmContainer("PndDipole2Par",
					     "Dipole Field parameter container",
					     "Default field");
  containers->Add(con32);
  
 
  CbmContainer* con33 = new CbmContainer("PndDipole3Par",
					     "Dipole Field parameter container",
					     "Default field");
  containers->Add(con33);
  
    
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
