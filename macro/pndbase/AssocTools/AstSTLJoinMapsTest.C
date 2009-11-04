#include "AstTestTools.h"
#include "AstSTLJoinMaps.h"
#include "AstSTLMap2.h"

#include <string>
#include <iostream>
using std::endl;
using std::cout;

void AstSTLJoinMapsTest(){
  AstTestBanner("AstJoinMaps");

  SimpleClass *mySP1 = new SimpleClass(4);
  SimpleClass *mySP2 = new SimpleClass(6);
  SimpleClass *mySP3 = new SimpleClass(2);
  
  std::string *testASA1 = new std::string("Four");
  std::string *testASA2 = new std::string("Six");
  std::string *testASA3 = new std::string("Two");
  
  AstSTLMap2<std::string, SimpleClass> *jMap1 = 
    new AstSTLMap2<std::string, SimpleClass>();

  AstSTLMap2<SimpleClass, std::string> *jMap2 = 
    new AstSTLMap2<SimpleClass, std::string>();

  jMap1->append(testASA1, mySP1);
  jMap1->append(testASA2, mySP2);
  jMap1->append(testASA3, mySP3);
  jMap1->append(testASA3, mySP1);


  jMap2->append(mySP1, testASA1);
  jMap2->append(mySP2, testASA2);
  jMap2->append(mySP3, testASA3);
  jMap2->append(mySP1, testASA3);

  AstSTLJoinMaps<std::string, SimpleClass, std::string> joinMaps;
  AstSTLMap2<std::string, std::string> *newMap = joinMaps.joinMaps(jMap1, jMap2);

  const std::map<std::string*, std::vector<std::string*>,PndPtrLess > map1 = 
     newMap->map1();
  std::map<std::string*, std::vector<std::string*>,
    PndPtrLess >::const_iterator iterator1;
  
  for (iterator1 = map1.begin(); iterator1 != map1.end(); iterator1++) {
    cout << "String " << *(iterator1->first) 
	 << " is associated to " << endl;
    std::vector<std::string*>::const_iterator iter;
    for (iter = iterator1->second.begin(); iter != iterator1->second.end();
         iter++) {
      cout << *(*iter) << endl;
    }
  }

  const std::map<std::string*, std::vector<std::string*>,PndPtrLess > map2 = 
     newMap->map2();
  std::map<std::string*, std::vector<std::string*>,
    PndPtrLess >::const_iterator iterator2;

  for (iterator2 = map2.begin(); iterator2 != map2.end(); iterator2++) {
    cout << "String " << *(iterator2->first) 
	 << " is associated to " << endl;
    std::vector<std::string*>::const_iterator iter;
    for (iter = iterator2->second.begin(); iter != iterator2->second.end();
         iter++) {
      cout << *(*iter) << endl;
    }
  }

  exit(0);    
}
