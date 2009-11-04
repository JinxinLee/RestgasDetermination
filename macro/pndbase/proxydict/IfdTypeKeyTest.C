#include <iostream>
#include <stdlib.h>
#include "IfdKey.h"
#include "IfdCounter.h"
#include "IfdTypeKey.h"
#include "IfdTestClasses.h"

using std::cout;
using std::endl;

#ifdef IFDTYPEKEY_SPECIALIZE
  IfdTypeKeySpecialize( int, 0 )
  IfdTypeKeySpecialize( boogie, 1 )
  IfdTypeKeySpecialize( woogie, 2 )
#endif
void testPair( IfdKey* k1, IfdKey* k2 );

void IfdTypeKeyTest(){

    //  Keys used in the test

    IfdTypeKey<int>* tKey1 = new IfdTypeKey<int>;
    IfdTypeKey<int>* tKey2 = new IfdTypeKey<int>;
    IfdTypeKey<boogie>* tKey3 = new IfdTypeKey<boogie>;
    IfdTypeKey<boogie>* tKey4 = new IfdTypeKey<boogie>;
    IfdTypeKey<woogie>* tKey5 = new IfdTypeKey<woogie>;
    IfdTypeKey<woogie>* tKey6 = new IfdTypeKey<woogie>;

    // Output a sed string to allow us to do regression tests.
    // IfdTypeKeys pick up their values from statics, and different
    // systems init them differently.  Here we make a xlation table
    // for this particular run.

    cout << "sed "
       << " -e 's/" << *tKey1 <<"/IfdTypeKey[int]/g' "
       << " -e 's/" << *tKey3 <<"/IfdTypeKey[boogie]/g' "
       << " -e 's/" << *tKey5 <<"/IfdTypeKey[woogie]/g' "
       << " << ENDofTEST"
       << endl;

    cout << "Test the counters.  Make sure that" << endl;
    cout << "   1.  the value only increments, never decrements" << endl;
    cout << "   2.  the value of each counter is unaffected by" <<endl;
    cout << "       destruction of other counters." << endl;
    cout << endl;

    IfdCounter *ctr1, *ctr2, *ctr3, *ctr4;
    ctr1 = new IfdCounter;
    int val1 = ctr1->value();
    ctr2 = new IfdCounter;
    int val2 = ctr2->value();
    if ( val1 != ctr1->value() ) {
      cout << "   Error:  making new ctr affects old counter." << endl;
    }
    ctr3 = new IfdCounter;
    int val3 = ctr3->value();
    if ( val1 != ctr1->value() ||
         val2 != ctr2->value() ) {
      cout << "   Error:  making new ctr affects old counter." << endl;
    }
    delete ctr3;
    if ( val1 != ctr1->value() ||
         val2 != ctr2->value() ) {
      cout << "   Error:  deleting a ctr affects old counters." << endl;
    }
    ctr4 = new IfdCounter;
    int val4 = ctr4->value();
    if ( val1 != ctr1->value() ||
         val2 != ctr2->value() ) {
      cout << "   Error:  making a new ctr after deleting a counter "
           << "   affects old counters." << endl;
    }
    delete ctr1;
    if ( val2 != ctr2->value() ||
         val4 != ctr4->value() ) {
      cout << "   Error:  deleting a ctr affects old counters." << endl;
    }
    delete ctr2;
    if ( val4 != ctr4->value() ) {
      cout << "   Error:  deleting a ctr affects old counters." << endl;
    }
    delete ctr4;
    ctr1 = new IfdCounter;
    if ( val1 == ctr1->value() ||
         val2 == ctr1->value() ||
         val3 == ctr1->value() ||
         val4 == ctr1->value() ) {
      cout << "   Error: counter not monotonic" << endl;
    }
    cout << "Done with counter tests." << endl;
    cout << endl;

    // Test IfdTypeKeys
    //
    cout << "Testing IfdTypeKey.  Look for" << endl;
    cout << "   1.  Each type T yeilds a differnt IfdTypeKey value." <<endl;
    cout << "   2.  All instances of any instantiation IfdTypeKey<T> " << endl;
    cout << "       have the same value." << endl;
    cout << "   3.  IfdTypeKeys function as Keys (op== works)" << endl;
    cout << endl;


    cout << "IfdTypeKey<int>    tKey1 = " << *tKey1 << endl;
    cout << "IfdTypeKey<int>    tKey2 = " << *tKey2 << endl;
    cout << "IfdTypeKey<boogie> tKey3 = " << *tKey3 << endl;
    cout << "IfdTypeKey<boogie> tKey4 = " << *tKey4 << endl;
    cout << "IfdTypeKey<woogie> tKey5 = " << *tKey5 << endl;
    cout << "IfdTypeKey<woogie> tKey6 = " << *tKey6 << endl;
    cout << "Deteting the IfdTypeKey<int>..." << endl;
    delete tKey1;
    delete tKey2;
    cout << "Making the IfdTypeKey<int> over again" <<endl;
    tKey1 = new IfdTypeKey<int>;
    tKey2 = new IfdTypeKey<int>;
    cout << "Make sure they have their original values" << endl;
    cout << "IfdTypeKey<int>    tKey1 = " << *tKey1 << endl;
    cout << "IfdTypeKey<int>    tKey2 = " << *tKey2 << endl;
    cout << endl;

    enum { maxNkey = 100 };
    IfdKey* KeysToTest[ maxNkey ];
    int nKey = 0;
    // Make up pairs to test.  Do each pair twice to test
    // reflexitivity
    //
    KeysToTest[ nKey++ ] = tKey1;
    KeysToTest[ nKey++ ] = tKey2;
    KeysToTest[ nKey++ ] = tKey3;
    KeysToTest[ nKey++ ] = tKey4;
    KeysToTest[ nKey++ ] = tKey5;
    KeysToTest[ nKey++ ] = tKey6;

    for( int i=0; i< nKey; i++ ) {
        for( int j=i; j<nKey; j++) {
            testPair( KeysToTest[i], KeysToTest[j] );
            testPair( KeysToTest[j], KeysToTest[i] );
        }
    }


    cout << "ENDofTEST"
         << endl;

    exit(0);

}


void testPair( IfdKey* k1, IfdKey* k2 ) {
  cout << "****************************************";
  cout << "****************************************" << endl;
  cout << *k1 << endl;
  cout << *k2 << endl;
  if ( *k1 == *k2 ) {
    cout << "EQUAL" << endl;
  } else {
    cout << "DIFFER" << endl;
  }
}
