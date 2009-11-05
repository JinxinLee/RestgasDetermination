#include "AstTestTools.h"
#include "AstUniqMap2.h"
#include "PndString.h"

#include <string>
#include <iostream>
using std::endl;
using std::cout;

void AstUniqMap2Test(){
  AstTestBanner("AstUniqMap2");

  std::string *aWord = new std::string("Radiohead");
  std::string *aWord2 = new std::string("OK Computer");
  std::string *aWord3 = new std::string("Decon Blue");
  std::string *aWord4 = new std::string("Raintown");
  
  
  AstUniqMap2<std::string, std::string> aUniqMap(&panda::PndString::rwHash, &panda::PndString::rwHash);
  
  aUniqMap.append(aWord, aWord2);
  aUniqMap.append(aWord3, aWord4);
  
  const std::string *foundWord = aUniqMap.find1(aWord);
  if (foundWord !=0){
    cout<< *aWord << " wrote " << *foundWord << endl;
  } else {
    cout<< "Failed to find " << *aWord << " in map." << endl;
  }

  cout<< "Map contains " << aUniqMap.members() << " entries." << endl;
  
  if (aUniqMap.append(aWord, aWord4)) {
    cout<< "Ooops. Successfully apended " << *aWord << " and " 
                   << *aWord4 << " to map." << endl;
  } else {
    cout<< "Correctly failed to add " << *aWord << " and " 
                   << *aWord4 << " to map." << endl;
  }
  
  aUniqMap.clear();
  
  cout << "After clearing, map contains " << aUniqMap.members() << " entries. " << endl;

  exit(0);    
}
