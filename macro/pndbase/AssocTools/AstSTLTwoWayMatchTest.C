#include "AstTestTools.h"
#include "AstSTLTwoWayMatch.h"

#include <string>
#include <map>
#include "CollectionUtils.h"
#include "PndString.h"

#include <iostream>
using std::endl;
using std::cout;

void AstSTLTwoWayMatchTest(){
  AstTestBanner("AstSTLTwoWayMatch");
  {
    // scope of "don't delete" object
    AstSTLTwoWayMatch<std::string, std::string, std::string> *theTwoWayMatch = 
      new AstSTLTwoWayMatch<std::string, std::string, std::string>(&panda::PndString::rwHash, &panda::PndString::rwHash, false);

    std::string *babar = new std::string("BaBar");
    std::string *isan  = new std::string("is an");
    std::string *elephant = new std::string("elephant");
    std::string *wears = new std::string("wears");
    std::string *blackshoes = new std::string("black shoes");
    std::string *celeste = new std::string("Celeste");
    std::string *married = new std::string("married");
    std::string *cornelius = new std::string("Cornelius");
    std::string *is = new std::string("is");
    std::string *verywise = new std::string("very wise");
 
    std::string *elephant1 = new std::string("elephant");
    std::string *elephant2 = new std::string("elephant");
    std::string *babar1 = new std::string("BaBar");

    theTwoWayMatch->insertMatch(babar, isan, elephant);
    theTwoWayMatch->insertMatch(babar, wears, blackshoes);
    theTwoWayMatch->insertMatch(celeste, isan, elephant1);
    theTwoWayMatch->insertMatch(celeste, married, babar1);
    theTwoWayMatch->insertMatch(cornelius, isan, elephant2);
    theTwoWayMatch->insertMatch(cornelius, is, verywise);

    std::string *aRWCString;

    cout<< "Finding forward match sets: " << endl << endl;


    const std::map<std::string*, std::string*, PndPtrLess> *anotherMatchingSet1 = 
      theTwoWayMatch->findMatchSet1(babar);

    std::map<std::string*, std::string*, PndPtrLess>::const_iterator
      otherWords = anotherMatchingSet1->begin();
  
    cout<< *babar << endl;
    while (otherWords != anotherMatchingSet1->end()) { 
      cout<< *(otherWords->first) << " " << *(otherWords->second) << endl;
      ++otherWords;
    }

    cout<< "Finding match quality: " << endl << endl;
    cout<< *celeste << " " << *married << " " 
                   << *theTwoWayMatch->getMatchQuality(celeste, married) << endl;

    cout<< "Finding backward match sets: " << endl << endl;

    const std::map<std::string*, std::string*, PndPtrLess> *anotherMatchingSet2 = 
      theTwoWayMatch->findMatchSet2(isan);

    std::map<std::string*, std::string*, PndPtrLess>::const_iterator
      otherWords2 = anotherMatchingSet2->begin();
  
    while (otherWords2 != anotherMatchingSet2->end()) {
      cout<< *(otherWords2->first) << " " << *isan << " " 
                     << *(otherWords2 ->second) << endl;
      ++otherWords2;
    }

    cout<< "Removing matches for " << *celeste << endl;
    theTwoWayMatch->removeMatchesForKey1(celeste);
    if (theTwoWayMatch->findMatchSet1(celeste) || theTwoWayMatch->findMatchSet2(celeste)){
      cout<< "Still found match for " << *celeste << endl;
    } else {
      cout<< "Successfully removed all matches for " << *celeste << endl;
    }

    anotherMatchingSet2 = theTwoWayMatch->findMatchSet2(isan);
    otherWords2 = anotherMatchingSet2->begin();
    while (otherWords2 != anotherMatchingSet2->end()) {
      cout<< *(otherWords2->first) << " " << *isan << " " 
                     << *(otherWords2->second) << endl;
      ++otherWords2;
    }

    aRWCString = new std::string("Cornelius");
    if (theTwoWayMatch->contains(cornelius, isan)){
      cout<< "Match object contains the string " << *cornelius << " and "
                     << *isan << endl;
    } else {
      cout<< "Match object does not contain the string " << *cornelius << " and "
                     << *isan << endl;
    }

    if (theTwoWayMatch->removeSpecificMatch(cornelius, isan)){
      cout<< "Successfully removed specific match for " << *cornelius << " " 
                     << *isan << ", The following remains:" << endl;
      const std::map<std::string*, std::string*, PndPtrLess> *anotherMatchingSet2 
        = theTwoWayMatch->findMatchSet1(cornelius);
      std::map<std::string*, std::string*, PndPtrLess>::const_iterator 
        otherWords2 = anotherMatchingSet2->begin();
      while (otherWords2 != anotherMatchingSet2->end()) {
        cout<< *cornelius << " " << *(otherWords2->first) << " " 
                       << *(otherWords2->second) << endl;
        ++otherWords2;
      }
    } else {
      cout<< "Failed to remove match " << *cornelius << " " << *isan << endl;
    }

    delete theTwoWayMatch;


    // Next line should die if delete has destroyed match quality objects
    
    verywise->length();
    
  }


  { 
    // scope of "do  delete" object
    AstSTLTwoWayMatch<std::string, std::string, std::string> *theTwoWayMatch = 
      new AstSTLTwoWayMatch<std::string, std::string, std::string>(&panda::PndString::rwHash, &panda::PndString::rwHash, false);

    std::string *babar = new std::string("BaBar");
    std::string *isan  = new std::string("is an");
    std::string *elephant = new std::string("elephant");
    std::string *wears = new std::string("wears");
    std::string *blackshoes = new std::string("black shoes");
    std::string *celeste = new std::string("Celeste");
    std::string *married = new std::string("married");
    std::string *cornelius = new std::string("Cornelius");
    std::string *is = new std::string("is");
    std::string *verywise = new std::string("very wise");
 
    std::string *elephant1 = new std::string("elephant");
    std::string *elephant2 = new std::string("elephant");
    std::string *babar1 = new std::string("BaBar");

    theTwoWayMatch->insertMatch(babar, isan, elephant);
    theTwoWayMatch->insertMatch(babar, wears, blackshoes);
    theTwoWayMatch->insertMatch(celeste, isan, elephant1);
    theTwoWayMatch->insertMatch(celeste, married, babar1);
    theTwoWayMatch->insertMatch(cornelius, isan, elephant2);
    theTwoWayMatch->insertMatch(cornelius, is, verywise);

    delete theTwoWayMatch;
  
  }


  exit(0);    
}
