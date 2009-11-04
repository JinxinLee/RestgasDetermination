#include <fstream>
#include <iostream>
#include <strings.h>

#include <string>
#include "AstTestTools.h"
#include "AstAssociation.h"
#include "AstRefAssociation.h"

#include <iostream>
#include <string>
using std::endl;
using std::cout;

void AstAssocTest(){

  AstTestBanner("AstAssociation");

  AstAssociation<std::string, std::string>
    anAssociation(new std::string("Blackpool"), new std::string("Preston"));

  cout << *anAssociation.firstArgument() << " will always beat "
       << *anAssociation.secondArgument() << endl;

 

  AstTestBanner("AstRefAssociation");

  std::string bfc("Blackpool");
  std::string pne("Preston");
  AstRefAssociation<std::string, std::string> aRefAssociation(bfc,pne);
  cout << aRefAssociation.firstArgument() << " will always beat "
       << aRefAssociation.secondArgument() << endl;
  exit(0);    
}
