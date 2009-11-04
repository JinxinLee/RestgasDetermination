#include "AstTestTools.h"
#include "AstSTLMatch.h"


#include <string>
#include <vector>
#include <map>
#include "CollectionUtils.h"
#include "String.h"

#include <iostream>
using std::endl;
using std::cout;

void AstSTLMatchTest(){
  AstTestBanner("AstSTLMatch");

  AstSTLMatch<std::string, std::string, std::string> *theMatch
    = new AstSTLMatch<std::string, std::string, std::string>(&panda::String::rwHash,
                                                    &panda::String::rwHash);

  std::string *babar = new std::string("BaBar");
  std::string *isan  = new std::string("is an");
  std::string *elephant = new std::string("elephant");
  
  theMatch->insertMatch(babar, isan, elephant);

  std::string *wears = new std::string("wears");
  std::string *blackshoes = new std::string("black shoes");

  theMatch->insertMatch(babar, wears, blackshoes);

  std::string *celeste = new std::string("Celeste");

  theMatch->insertMatch( celeste, isan, elephant);

  std::string *married = new std::string("married");

  theMatch->insertMatch(celeste, married, babar);
  
  std::string *cornelius = new std::string("Cornelius");
  theMatch->insertMatch(cornelius, isan, elephant);

  std::string *is = new std::string("is");
  std::string *verywise = new std::string("very wise");

  theMatch->insertMatch(cornelius, is , verywise);


  AstSTLMatch<std::string, std::string, std::string> *theMatch2
    = new AstSTLMatch<std::string, std::string, std::string>();

  std::string *string1 = new std::string("BaBar");
  std::string *string2 = new std::string("wears");
  std::string *string3 = new std::string("a green jacket");
  std::string *string4 = new std::string("is a");
  std::string *string5 = new std::string("small damp fish");

  theMatch2->insertMatch(string1, string2, string3);
  theMatch2->insertMatch(string1, string4, string5);


  std::vector<std::string*> *vecString1 = 
    new std::vector<std::string*>;
  vecString1->push_back(new std::string(*string3));
  vecString1->push_back(new std::string(*blackshoes));
  std::vector<std::string*> *vecString2 = 
    new std::vector<std::string*>;
  vecString2->push_back(new std::string(*string5));
  vecString2->push_back(new std::string(*elephant));

  AstSTLMatch<std::string, std::string, std::vector<std::string*> > 
    *vecMatch= new AstSTLMatch<std::string, std::string, 
    std::vector<std::string*> > (&panda::String::rwHash, &panda::String::rwHash);
  vecMatch->setMatchQualityCleanerFunction(AstSTLTestCleaner);
  
  vecMatch->insertMatch(string1, string2, vecString1);
  vecMatch->insertMatch(string1, string4, vecString2);

  // Don't want to do much with it, just make sure that the 
  // destructor doesn't crash. 
  delete vecMatch;


  const std::map<std::string*, std::string*, PndPtrLess> *theMatchingSet = 
    theMatch->findMatchSet(babar);
  std::map<std::string*, std::string*, PndPtrLess>::const_iterator 
    theWords = theMatchingSet->begin();
  
  cout << *babar << endl;
  while (theWords != theMatchingSet->end()) {
    cout << *(theWords->first) << " " << *(theWords->second) << endl;
    ++theWords;
  }

  if (theMatch->getMatchQuality(celeste, married)!=0){
    cout << *celeste << " " << *married << " " 
         << *theMatch->getMatchQuality(celeste, married) << endl;
  } else { 
    cout << "Failed to find match for " 
                   <<  *celeste << " " << *married << endl;
  }

  cout << "Removing matches for " << *celeste << endl;
  theMatch->removeMatchesForKey(celeste);

  babar = new std::string("BaBar");

  if (theMatch->findMatchSet(celeste)){
    cout << "Still found match for " << *celeste << endl;
  } else {
    cout<< "Successfully removed all matches for " 
                   << *celeste << endl;
  }

  if (theMatch->contains(cornelius, isan)){
    cout<< "Match object contains the string "
                   << *cornelius << " and "
         << *isan << endl;
  } else {
    cout<< "Match object does not contain the string " 
                   << *cornelius << " and "
                   << *isan << endl;
  }
  if (theMatch->removeSpecificMatch(cornelius, isan)){
    cout<< "Successfully removed specific match for " 
                   << *cornelius << " " 
                   << *isan << ", The following remains:" << endl;
    const std::map<std::string*, std::string*, PndPtrLess> *theMatchingSet2 
      = theMatch->findMatchSet(cornelius);
    std::map<std::string*, std::string*, PndPtrLess>::const_iterator 
      theWords2 = theMatchingSet2->begin();
    while (theWords2 != theMatchingSet2->end()) {
      cout<< *cornelius << " " << *(theWords2->first) << " " 
                     << *(theWords2->second) << endl;
      ++theWords2;
    }
  } else {
    cout<< "Failed to remove match " 
                   << *cornelius << " " << *isan << endl;
  }

  const std::map<std::string*, std::string*, PndPtrLess> *theMatchingSet1 = 
    theMatch2->findMatchSet(string1);
  std::map<std::string*, std::string*, PndPtrLess>::const_iterator 
    theWords1 = theMatchingSet1->begin();
  
  cout<< *string1 << endl;
  while (theWords1 != theMatchingSet1->end()) { 
    cout<< *(theWords1->first) << " " << *(theWords1->second) << endl;
    ++theWords1;
  }

  cout<< "Don't worry, I'm fibbing" << endl;

  elephant = new std::string("elephant");



  // Test deletion of members

  AstSTLMatch<std::string, std::string, std::string> *theMatchDeleteTest
    = new AstSTLMatch<std::string, std::string, std::string>(&panda::String::rwHash, &panda::String::rwHash);

  babar = new std::string("BaBar");
  isan  = new std::string("is an");
  elephant = new std::string("elephant");
  
  theMatchDeleteTest->insertMatch(babar, isan, elephant);
  
  wears = new std::string("wears");
  blackshoes = new std::string("black shoes");
  
  theMatchDeleteTest->insertMatch(babar, wears, blackshoes);
  
  celeste = new std::string("Celeste");
  theMatchDeleteTest->clearAndDestroy();


  exit(0);    
}
