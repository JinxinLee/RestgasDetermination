#include "AstTestTools.h"
#include "AstSTLMapIndex.h"
#include "String.h"

#include <string>
#include <vector>
#include <iostream>
using std::endl;
using std::cout;


void AstSTLMapIndexTest(){
  AstTestBanner("AstSTLMapIndex");

  std::vector<std::string*> dummyList2;
  std::string *yetMore1 = new std::string("wibble1");
  std::string *yetMore2 = new std::string("wibble2");
  std::string *yetMore3 = new std::string("wibble3");
  std::string *yetMore4 = new std::string("wibble4");
  std::string *yetMore5 = new std::string("wibble5");

  dummyList2.push_back(yetMore1);
  dummyList2.push_back(yetMore2);
  dummyList2.push_back(yetMore3);
  dummyList2.push_back(yetMore4);
  dummyList2.push_back(yetMore5);

std::string *nextString(0);

  cout << "RW Ptr constructor" << endl;

  AstSTLMapIndex<std::string> rwPtrMap(dummyList2, &panda::String::rwHash);
  int kk;
  for (kk=0;kk<dummyList2.size();kk++){
    nextString = dummyList2[kk];
    int index;
    if (rwPtrMap.index(nextString, index)){
      cout << "Found string " << *nextString << " at index " 
                      << index << endl;
    } else {
      cout << "Failed to locate " << *nextString << " in map!!" << endl;
    }
  }

  exit(0);    
}
