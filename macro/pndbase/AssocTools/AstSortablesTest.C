#include "AstTestTools.h"
#include "AstSortableAssoc.h"
#include "AstSorted2Vector.h"

#include <string>
#include <vector>
#include <algorithm>

#include <iostream>
using std::endl;
using std::cout;

bool compare (const AstSortableAssoc<std::string, SimpleClass>* lhs, 
              const AstSortableAssoc<std::string, SimpleClass>* rhs) {
  return (*lhs) < (*rhs);
}

void AstSortablesTest(){

  AstTestBanner("AstSortableAssoc");

  SimpleClass *mySP1 = new SimpleClass(4);
  SimpleClass *mySP2 = new SimpleClass(6);
  SimpleClass *mySP3 = new SimpleClass(2);
  
  std::string *testASA1 = new std::string("Four");
  std::string *testASA2 = new std::string("Six");
  std::string *testASA3 = new std::string("Two");

  std::vector<AstSortableAssoc<std::string, SimpleClass>* > 
    *mySortedVector= 
    new std::vector<AstSortableAssoc<std::string, SimpleClass>* >();
  
  mySortedVector->
    push_back(new AstSortableAssoc<std::string, SimpleClass>(testASA1, mySP1, 
                                                        simpleClassLessThan));
  mySortedVector->
    push_back(new AstSortableAssoc<std::string, SimpleClass>(testASA2, mySP2,
                                                        simpleClassLessThan));
  mySortedVector->
    push_back(new AstSortableAssoc<std::string, SimpleClass>(testASA3, mySP3,
                                                        simpleClassLessThan));

  std::sort(mySortedVector->begin(), mySortedVector->end(), compare);

  std::vector<AstSortableAssoc<std::string, SimpleClass>* >::const_iterator 
    iter;

  int i = 0;

  for (iter = mySortedVector->begin(); iter != mySortedVector->end(); iter++) {
    cout << "Vector entry " << i << ". " 
                    << (*iter)->key() 
                    << ", " << (*iter)->quality().getInt() 
                    << endl;
    i++;
  }

  AstTestBanner("AstSorted2Vector");

  AstSorted2Vector<std::string, SimpleClass> myVector(simpleClassLessThan);

  myVector.insert(testASA1, mySP1);
  myVector.insert(testASA2, mySP2);  
  myVector.insert(testASA3, mySP3);

  for (i=0;i<myVector.entries();i++){
    cout << "Vector entry " << i << ". " 
                    << myVector[i] << ", " << myVector.quality(i).getInt() 
                    << endl;
  }
  
  if (myVector.contains(testASA1, mySP1)){
    cout << "Correctly found combination " << *testASA1
                    << ", " << mySP1->getInt() << endl;
  }

  if (!myVector.contains(testASA1, mySP2)){
    cout << "Correctly did not find  combination " << *testASA1
                    << ", " << mySP2->getInt() << endl;
  }

  const std::string* haveFound = myVector.find( *mySP2 );
  if (  haveFound != 0 )
    {
      cout << "Correctly found " << *haveFound << endl;
    }
  
  const std::string* haveFound2 = myVector.find( SimpleClass( 1000 )  );
  if (  haveFound2 == 0 )
    {
      cout << "Correctly didn't find nonexistant class." << endl;
    }
  
  myVector.clearAndDestroy();

  exit(0);    
}
