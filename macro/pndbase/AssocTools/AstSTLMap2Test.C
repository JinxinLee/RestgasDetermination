#include "AstSTLMap2.h"

#include <string>
#include <vector>
#include <iostream>
using std::endl;
using std::cout;

void AstSTLMap2Test(){

  AstSTLMap2<std::string, std::string> *theMap2 = new AstSTLMap2<std::string, std::string>;

  std::string *cornelius = new std::string("Cornelius");
  std::string *isanele = new std::string("is an elephant");

  theMap2->append(cornelius, isanele);

  std::string *isvery = new std::string("is very wise");

  theMap2->append(cornelius, isvery);


  std::string *pompadore = new std::string("Pompadore");
  std::string *isafriend = new std::string("is a friend of Cornelius");
  
  theMap2->append(pompadore, isafriend);

  std::vector<std::string*> retVal;
  if (theMap2->findValue1(cornelius, retVal)){
    cout<< "Found the following " 
         << retVal.size() << " matches to " << *cornelius << endl;    
    std::string *nextString;
    int i;
    for (i=0; i<(int) retVal.size();i++){
      nextString = retVal[i];
      cout<< *nextString << endl;
    }
  }

  std::string *foundString = theMap2->findFirstValue1(cornelius);
  if (foundString!=0){
    cout<< "Found string " << *foundString << " associated to " 
                   << *cornelius << endl;
  } else {
    cout<< "Failed to locate string " << *cornelius 
                   << " in the map " << endl;
  }
  std::string *found2 = theMap2->findFirstValue2(isanele);
  if (found2!=0){
    cout<< "Made reverse association between  " << *found2 
                   << " and " << *isanele << endl;
  } else {
    cout<< "But failed to make the reverse association." << endl;
  }


  if (theMap2->contains(cornelius, isanele)){
    cout<< "Map contains the string " << *cornelius << " " << *isanele << endl;
  } else { 
    cout<< "Failed to locate string " << endl;
  }

  cout<< "There are " << theMap2->entries1(cornelius) 
                 << " entries for the string " 
                 << *cornelius << endl;
  cout<< "There are " << theMap2->entries2(isanele) 
                 << " entries for the string " 
                 << *isanele << endl;

  size_t removeCornelius = theMap2->removeMatchesForKey1(cornelius);
  
  cout<< "Removed " << removeCornelius << " matches for " 
                 << *cornelius << endl;

  cout<< "There now remain " << theMap2->entries1(cornelius) 
                 << " entries for " 
                 << *cornelius << endl;

  cout<< "There remain " << theMap2->members1() << " entries in the map." 
                 << endl;
  theMap2->clear();

  cout<< "Clearing leaves " << theMap2->members1() << " entries " << endl;

  exit(0);    
}
