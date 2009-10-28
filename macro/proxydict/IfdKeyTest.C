#include "IfdKey.h"
#include "IfdIntKey.h"
#include "IfdStrKey.h"
#include "IfdTypeKey.h"
#include "IfdCompositeKey.h"
#include "IfdDictKey.h"

//#include "TStopwatch.h"
#include <iostream>

void testPair( IfdKey* k1, IfdKey* k2 );

void IfdKeyTest(){
  //  TStopwatch timer;
//   timer.Start();
//   gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
//  rootlogon();
//   basiclibs();
//    gSystem->Load("libPndBase.so"); 



  enum { maxNkey = 100 };    
    IfdKey* KeysToTest[ maxNkey ];
   int nKey = 0;


   
   IfdTypeKey<int>  tKey1; KeysToTest[ nKey++] = &tKey1;
  IfdTypeKey<int>  tKey2;          KeysToTest[ nKey++] = &tKey2;
  IfdTypeKey<char> tKey3;          KeysToTest[ nKey++] = &tKey3;
  
  IfdIntKey iKey1(1);              KeysToTest[ nKey++] = &iKey1;
  IfdIntKey iKey5(5);              KeysToTest[ nKey++] = &iKey5;
  IfdIntKey iKey5b(5);             KeysToTest[ nKey++] = &iKey5b;
  
  IfdStrKey sKey1( "Hello," );     KeysToTest[ nKey++] = &sKey1;
  IfdStrKey sKey2( "world"  );     KeysToTest[ nKey++] = &sKey2;
  
  IfdStrKey sKey3( "vxworks world" );    KeysToTest[ nKey++] = &sKey3;
  
  IfdCompositeKey cKey1;           KeysToTest[ nKey++] = &cKey1;
  IfdCompositeKey cKey2;           KeysToTest[ nKey++] = &cKey2;
  IfdCompositeKey cKey3;           KeysToTest[ nKey++] = &cKey3;
  IfdCompositeKey cKey4;           KeysToTest[ nKey++] = &cKey4;
  IfdCompositeKey cKey5;           KeysToTest[ nKey++] = &cKey5;
  

    //End of building up keylist to test
    //
    if (nKey > maxNkey) {
      cout << "Too many keys to test.  Change maxNkey" << endl;
      ::exit(1);
    }

    // Output a sed string to allow us to do regression tests.
    // IfdTypeKeys pick up their values from statics, and different
    // systems init them differently.  Here we make a xlation table
    // for this particular run.

    cout << "sed "
         << " -e 's/" << tKey1 <<"/IfdTypeKey[int]/g' "
         << " -e 's/" << tKey3 <<"/IfdTypeKey[char]/g' "
         << " << ENDofTEST"
         << endl;

    cout << "This program creates a handfull of keys of various" <<endl;
    cout << "kinds and then forms all possible pairs from them." <<endl;
    cout << "Each pair is tested for equality in both directions," <<endl;
    cout << "e.g., both a==b and b==a.  This verifies reflexitivity."<<endl;
    cout << "You should also look for other aspects of correct"<<endl;
    cout << "behavior.  For example, make sure that composite" <<endl;
    cout << "keys get built up correctly, even if made of other"<<endl;
    cout << "composite keys.  Multiple composites are built up" <<endl;
    cout << "and tested for this reason.  We also make one composite"<<endl;
    cout << "that adds itself to itself to look for pathologies in"<<endl;
    cout << "the add() method." << endl;
    cout << endl;

  cout << "Size of IfdTypeKey     : "  << sizeof( IfdTypeKey<int> ) << endl;
  cout << "Size of IfdStrkKey     : "  << sizeof( IfdStrKey ) << endl;
  cout << "Size of IfdDictKey     : "  << sizeof( IfdDictKey ) << endl;
  cout << "Size of IfdCompositeKey: "  << sizeof( IfdCompositeKey ) << endl;
  cout << "Size of IfdIntKey      : "  << sizeof( IfdIntKey ) << endl;
  
  cKey1.add( sKey1 );
  cKey1.add( sKey2 );
  cKey1.add( iKey1 );
  
  cKey2.add( sKey1 );
  cKey2.add( sKey3 );
  cKey2.add( iKey1 );
  
  cKey3.add( sKey1 );
  cKey3.add( sKey3 );
  cKey3.add( iKey5 );
  
  
  
  IfdCompositeKey foo;
  foo.add( iKey5 );
  foo.add( cKey1 );
  cKey4.add( iKey5 );
  cKey4.add( cKey2 );
  cKey4.add( foo  );
  cKey4.add( sKey2 );
  
  cKey5.add( iKey5 );
  cKey5.add( cKey2 );
  cKey5.add( foo  );
  cKey5.add( sKey2 );
  
  // Make up pairs to test.  Do each pair twice to test
  // reflexitivity
  //
  for( int i=0; i< nKey; i++ ) {
    for( int j=i; j<nKey; j++) {
      testPair( KeysToTest[i], KeysToTest[j] );
      testPair( KeysToTest[j], KeysToTest[i] );
    }
  }
  // First, try to add a composite key to itself to see if it
  // blows up.  We will leave this one out of the pairwise test.
  // (just because we have plenty).
  
  cout << "****************************************";
  cout << "****************************************" << endl;
  cout << "Test adding a composite key to itself." << endl;
  IfdCompositeKey self;
  self.add( iKey1 );
  self.add( sKey1 );
  cout << "Initial composite: " << self << endl;
  self.add( self );
  cout << "Composite w/ itself added in: " << self << endl;
  
  cout << "****************************************";
  cout << "****************************************" << endl;
  
  
  cout << "Test of car(), cdr(), and tail() using output of last test." 
       << endl;
  
  
  IfdKey          *kcar = self.car();
  IfdCompositeKey *kcdr = self.cdr();
  IfdKey          *ktail= self.tail();
  cout << "car() = " << *kcar << endl;
  cout << "cdr() = " << *kcdr << endl;
  cout << "tail()= " << *ktail << endl;
  delete kcar;
  delete kcdr;
  
  cout << "Testing StrKey to HepString conversion"  << endl;
  cout << endl;
  IfdCompositeKey db;
  db.add( iKey1);
  db.add( sKey1 );
  IfdKey *skey = db.tail();
  const char* s = ((IfdStrKey *) skey)->asString();
  cout << "Composite  key : " << db << endl;
  cout << "Tail as string : " << s  << endl;
  cout << endl;
  
  
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
