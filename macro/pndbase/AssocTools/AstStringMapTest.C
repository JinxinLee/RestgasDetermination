#include "AstTestTools.h"
#include "AstStringMap.h"

#include <string>

#include <iostream>


void AstStringMapTest(){
  AstTestBanner("AstStringMap");

  AstStringMap<std::string> theMap;
  
  
  std::string *firstString= new std::string("BaBar");
  std::string *anotherString, *yetAnotherString;
  std::string* result = 0;

  anotherString = new std::string("is an elephant.");
  theMap.insert( *firstString ) = anotherString;

  // This one should overwrite the previous insert
  yetAnotherString = new std::string("married Celeste.");
  theMap.insert( *firstString ) = yetAnotherString;
  result = theMap[ *firstString ];
  assert( result == yetAnotherString );

  theMap.insert( *anotherString ) = firstString;
  theMap.insert( *yetAnotherString ) = firstString;

  result = theMap[ *anotherString ];
  assert( result == firstString );

  std::cout << theMap << std::endl;

  theMap.clear();
  delete firstString;
  delete anotherString;
  delete yetAnotherString;

  std::string antiu("anti-u"),antid("anti-d"),antib("anti-b");

  theMap.insert( antiu ) = &antiu;
  theMap.insert( antid ) = &antid;
  theMap.insert( antib ) = &antib;

  std::cout << theMap << std::endl;
  exit(0);    
}
