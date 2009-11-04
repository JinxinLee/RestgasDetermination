#include "AstTestTools.h"
#include "AstNamedMapVector.h"
//#include "ErrLogger/ErrLog.hh"

#include <string>
#include "String.h"

#include <iostream>
using std::endl;
using std::cout;

void AstNamedMapVectorTest(){
  AstTestBanner("AstNamedMapVector");

  std::string *aWord = new std::string("Radiohead");
  std::string *aWord2 = new std::string("OK Computer");
  std::string *aWord3 = new std::string("Decon Blue");
  std::string *aWord4 = new std::string("Raintown");

  AstNamedMapVector<std::string, std::string> anNM("MyMap", &panda::String::rwHash, &panda::String::rwHash);
  
  anNM.append(aWord, aWord2);
  anNM.append(aWord3, aWord4);

  cout << "Map has name " << anNM.name() << endl;

  std::vector<std::string*> list1(anNM.list1());
  std::vector<std::string*> list2(anNM.list2());

  for (size_t wombat = 0;wombat < list1.size() ; wombat++){
    cout << *list1[wombat] << " " << *list2[wombat] << endl;
  }

  const std::string *foundWord = anNM.find1(aWord);
  if (foundWord !=0){
    cout << *aWord << " wrote " << *foundWord << endl;
  } else {
    cout << "Failed to find " << *aWord << " in map." << endl;
  }
  
  cout << "Map contains " << anNM.members() << " entries." << endl;
  
  if (anNM.append(aWord, aWord4)) {
    cout << "Ooops. Successfully apended " << *aWord << " and " 
                   << *aWord4 << " to map." << endl;
  } else {
    cout << "Correctly failed to add " << *aWord << " and " 
                   << *aWord4 << " to map." << endl;
  }
  
  
  anNM.clear();

  cout << "After clearing, map contains " 
                  << anNM.members() << " entries. " << endl;

  exit(0);    
}
