#include "AstTestTools.h"
#include "TError.h"

#include <iostream>

#include <string>
#include <vector>
#include <set>
#include <map>

#include "AstSTLKeyMap.h"

using std::cout;
using std::endl;
using std::ostream;


void AstSTLKeyMapTest(){
  AstTestBanner("AstSTLKeyMap");

  // NOTE:  Solaris compiler bug rejects valid ctor below, use wrapper fn.
  //   AstKeyMap<std::string, std::string> theMap(&RWCString::hash);
  AstSTLKeyMap<std::string, std::string> theMap(&strValHashFunction);
  
  
  std::string firstString("BaBar");
  std::string *anotherString;
  anotherString = new std::string(" is an elephant.");
  theMap.append(firstString, anotherString);
  anotherString = new std::string(" married Celeste.");
  theMap.append(firstString, anotherString);

  
  
  std::string secondString("Cornelius");
  anotherString = new std::string(" is also an elephant.");
  theMap.append(secondString, anotherString);
  anotherString = new std::string(" made BaBar king.");
  theMap.append(secondString, anotherString);


  std::string thirdString("Arthur");
  anotherString = new std::string(" is BaBar's son");
  theMap.append(thirdString, anotherString);
  anotherString = new std::string(" wears a blue jacket.");
  theMap.append(thirdString, anotherString);

  cout << "Map contains " << theMap[firstString].size() 
       << " entries for string " << firstString << endl;

  std::vector<std::string*> found;

  int i;
  found = theMap.find(firstString)->second;
  for(i=0;i<(int) found.size();i++){
    cout<< firstString << " " << *(found[i]) << endl;
  }
  

  found = theMap.find(secondString)->second;
  for(register int j=0;j<(int) found.size();j++){
    cout<< secondString << " " << *(found[j]) << endl;
  }

  found = theMap.find(thirdString)->second;
  for(register int k=0;k<(int) found.size();k++){
    cout<< thirdString << " " << *(found[k]) << endl;
  }


  // NOTE:  Solaris compiler bug rejects valid ctor below, use wrapper fn.
  // RWTValHashSet<std::string> stringSet(&RWCString::hash);
  std::set<std::string> stringSet;

  stringSet.insert(secondString);
  stringSet.insert(thirdString);

  // NOTE:  Solaris compiler bug rejects valid ctor below, use wrapper fn.
  //   RWTValHashSet<std::string> stringSet2(&RWCString::hash);
  std::set<std::string> stringSet2;

  stringSet2.insert(secondString);
  
  found.clear();


  if (theMap.findIntersectionSet(stringSet, found)){
    cout << "Found associated set for strings:" << endl;
    ostream & myOs = cout;
    outputInOrder(myOs, stringSet.begin(), stringSet.end());
    myOs << "Which is " << endl;
    outputInOrder(myOs, found);
    myOs << endl;
  } else {
    Fatal("AstSTLKeyMapTest","No intersection set found");
  }
  
  found.clear();

  anotherString = new std::string("foober");
  stringSet.insert(*anotherString);

  cout << endl << endl;

  cout << "For original set" << endl;

  ostream &myOs1 = cout;
  outputInOrder(myOs1, stringSet.begin(), stringSet.end());
  myOs1 << endl;


  if (theMap.findIntersectionSet(stringSet, found, stringSet)){
    ostream &myOs2 = cout;
    myOs2 << "Found associated set " << endl;
    
    outputInOrder(myOs2, found);

    myOs2 << "And original set became: " << endl;

    outputInOrder(myOs2, stringSet.begin(), stringSet.end());
    myOs2 << endl;
  } else {
    Fatal("AstSTLKeyMapTest","No intersection set found");
  }

  cout << endl << endl;

  cout << "For original set" << endl;
  ostream &myOs3 = cout;
  outputInOrder(myOs3, stringSet.begin(), stringSet.end());
  myOs3 << endl;

  found.clear();

  if (theMap.findDifferenceSet(stringSet, found, stringSet)){
    ostream &myOs4 = cout;
    myOs4 << "Found associated difference set " << endl;

    outputInOrder(myOs4, found);

    myOs4 << "And original set became: " << endl;
    outputInOrder(myOs4, stringSet.begin(), stringSet.end());
    myOs4 << endl;
  } else {
    Fatal("AstSTLKeyMapTest","No difference set found");
  }
  
  cout << endl << endl;
  
  cout << "For original set" << endl;
  ostream &myOs5 = cout;
  outputInOrder(myOs5, stringSet.begin(), stringSet.end());
  myOs5 << endl;
  found.clear();

  if (theMap.findDifferenceSet(stringSet, found)){
    ostream &myOs6 = cout;
    myOs6 << "Found associated difference set " << endl;
    outputInOrder(myOs6, found);
    myOs6 << "And original set became: " << endl;
    outputInOrder(myOs6, stringSet.begin(), stringSet.end());
    myOs6 << endl;


  } else {
    Fatal("AstSTLKeyMapTest","No difference set found");
   }


  std::vector<std::string> stringVec;

  stringVec.push_back(firstString);
  stringVec.push_back(thirdString);

  cout << endl << endl;
  size_t index;

  found.clear();

  cout << "For original set" << endl;
  for (index=0; index < stringVec.size(); index++){
    cout << stringVec[index] << endl;
  }

  if (theMap.findIntersectionSet(stringVec, found)){
    ostream & myOs8  =    cout ;
    myOs8 << "Found associated set " << endl;
    
    outputInOrder(myOs8, found);
    myOs8 << endl;

  } else {
    Fatal("AstSTLKeyMapTest","No intersection set found");
  }

  found.clear();
  cout << "For original set" << endl;
  for (index=0; index < stringVec.size(); index++){
    cout << stringVec[index] << endl;
  }


  if (theMap.findDifferenceSet(stringVec, found)){
    ostream & myOs8  =    cout ;
    myOs8 << "Found difference set " << endl;
    
    outputInOrder(myOs8, found);
    myOs8 << endl;
  } else {
    Fatal("AstSTLKeyMapTest","No intersection set found");
  }


  exit(0);    
}
