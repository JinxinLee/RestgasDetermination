#include "AstTestTools.h"
#include "AstSTLMap.h"
#include "TError.h"

#include <exception>
#include <string>
#include <map>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::ostream;

void AstSTLMapTest(){
  AstTestBanner("AstSTLMap");

  AstSTLMap<std::string, std::string> theMap(&stringHashFunction);
  
  
  std::string *firstString= new std::string("BaBar");
  std::string *anotherString;
  anotherString = new std::string(" is an elephant.");
  theMap.append(firstString, anotherString);
  anotherString = new std::string(" married Celeste.");
  theMap.append(firstString, anotherString);
  
  
  
  std::string *secondString = new std::string("Cornelius");
  anotherString = new std::string(" is also an elephant.");
  theMap.append(secondString, anotherString);
  anotherString = new std::string(" made BaBar king.");
  theMap.append(secondString, anotherString);

  std::string *thirdString = new std::string("Arthur");
  anotherString = new std::string(" is BaBar's son");
  theMap.append(thirdString, anotherString);
  anotherString = new std::string(" wears a blue jacket.");
  theMap.append(thirdString, anotherString);

  cout << "Map contains " << theMap[firstString].size() 
       << " entries for string " << *firstString << endl;

  std::vector<std::string*> found;

  cout << "********** Base Set **********" << endl;
  found = theMap.find(firstString)->second;
  ostream &myOs1 = cout;
  myOs1 << *firstString << endl;
  outputInOrder(myOs1, found);
  myOs1 << endl;

  found = theMap.find(secondString)->second;
  ostream &myOs2 = cout;
  myOs2 << *secondString << endl;
  outputInOrder(myOs2, found);
  myOs2 << endl;


  found = theMap.find(thirdString)->second;
  ostream &myOs3 = cout;
  myOs3 << *thirdString << endl;
  outputInOrder(myOs3, found);
  myOs3 << endl;

  std::set<std::string*> stringSet;
  stringSet.insert(secondString);
  stringSet.insert(thirdString);

  std::set<std::string*> stringSet2;
  stringSet2.insert(secondString);
  
  found.clear();
  std::set<std::string*>::iterator hsIterator = stringSet.begin();


  cout << "********** Intersection Tests **********" << endl;

  ostream &myOs4 = cout;
  myOs4<< "For original set" << endl;
  outputInOrder(myOs4, hsIterator, stringSet.end());
  myOs4 << endl;

  if (theMap.findIntersectionSet(stringSet, found)){

    myOs4 << "Found associated intersection set " << endl;
    outputInOrder(myOs4, found);
    myOs4 << endl;

    myOs4 << "And original set became (should be untouched): " << endl;

    hsIterator = stringSet.begin();
    outputInOrder(myOs4, hsIterator, stringSet.end());
    myOs4 << endl;

  } else {
    Error("AstSTLMapTest","No intersection set found");
  }

  found.clear();
  anotherString = new std::string("foober");
  stringSet.insert(anotherString);

  cout << endl << endl;


  hsIterator = stringSet.begin();
  ostream &myOs5 = cout;
  myOs5<< "For original set" << endl;
  outputInOrder(myOs5, hsIterator, stringSet.end());
  myOs5 << endl;

  std::set<std::string*> dummy1;

  if (theMap.findIntersectionSet(stringSet, found, dummy1)){
    myOs5 << "Found associated intersection set " << endl;
    
    outputInOrder(myOs5, found);

    myOs5<< "And original set became (should be intersection): " << endl;
    hsIterator = dummy1.begin();
    
    outputInOrder(myOs5, hsIterator, dummy1.end());

    myOs5 << endl;

  } else {
    Error("AstSTLMapTest","No intersection set found");
  }

   dummy1.clear();

  cout << endl << endl;

  cout << "********** Difference Tests **********" << endl;
  
  hsIterator = stringSet.begin();
  ostream &myOs6 = cout;
  myOs6 << "For original set" << endl;
  outputInOrder(myOs6, hsIterator, stringSet.end());
  
  found.clear();

   if (theMap.findDifferenceSet(stringSet, found, dummy1)){
    myOs6<< "Found associated difference set " << endl;
    
    outputInOrder(myOs6, found);

    myOs6 << "And original set became (should be difference): " << endl;
    hsIterator = dummy1.begin();

    outputInOrder(myOs6, hsIterator, dummy1.end());
    myOs6 << endl;
  } else {
    myOs6 << endl;
    Error("AstSTLMapTest","No difference set found");
  }
  
  cout << endl << endl;
  
  dummy1.clear();


  ostream &myOs7 = cout;
  myOs7<< "For original set" << endl;
  hsIterator = stringSet.begin();
  outputInOrder(myOs7, hsIterator, stringSet.end());
  found.clear();
  
  if (theMap.findDifferenceSet(stringSet, found)){
    myOs7 << "Found associated difference set " << endl;
    outputInOrder(myOs7, found);

    myOs7 << "And original set became (should be untouched): " << endl;

    hsIterator = stringSet.begin();
    outputInOrder(myOs7, hsIterator, stringSet.end());
    myOs7 << endl;

  } else {
    Error("AstSTLMapTest","No difference set found");
  }


  cout << "********** Vector Tests **********" << endl;

  std::vector<std::string*> stringVec;

  stringVec.push_back(firstString);
  stringVec.push_back(thirdString);

  cout << endl << endl;
  size_t index;

  found.clear();

  cout << "For original set" << endl;
  for (index=0; index < stringVec.size(); index++){
    cout << *(stringVec[index]) << endl;
  }

  if (theMap.findIntersectionSet(stringVec, found)){
    ostream &myOs8 = cout;
    myOs8 << "Found intersection set " << endl;
    outputInOrder(myOs8, found);
    myOs8 << endl; 

  } else {
    Error("AstSTLMapTest","No intersection set found");
  }

  found.clear();
  cout << "For original set" << endl;
  for (index=0; index < stringVec.size(); index++){
    cout << *(stringVec[index]) << endl;
  }


  if (theMap.findDifferenceSet(stringVec, found)){
    ostream &myOs8 = cout;
    myOs8 << "Found difference set " << endl;
    outputInOrder(myOs8, found);
    myOs8 << endl; 
  } else {
    Error("AstSTLMapTest","No difference set found");
  }


  exit(0);    
}
